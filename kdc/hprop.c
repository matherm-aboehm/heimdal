/*
 * Copyright (c) 1997, 1998, 1999 Kungliga Tekniska H�gskolan
 * (Royal Institute of Technology, Stockholm, Sweden). 
 * All rights reserved. 
 *
 * Redistribution and use in source and binary forms, with or without 
 * modification, are permitted provided that the following conditions 
 * are met: 
 *
 * 1. Redistributions of source code must retain the above copyright 
 *    notice, this list of conditions and the following disclaimer. 
 *
 * 2. Redistributions in binary form must reproduce the above copyright 
 *    notice, this list of conditions and the following disclaimer in the 
 *    documentation and/or other materials provided with the distribution. 
 *
 * 3. All advertising materials mentioning features or use of this software 
 *    must display the following acknowledgement: 
 *      This product includes software developed by Kungliga Tekniska 
 *      H�gskolan and its contributors. 
 *
 * 4. Neither the name of the Institute nor the names of its contributors 
 *    may be used to endorse or promote products derived from this software 
 *    without specific prior written permission. 
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND 
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE 
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS 
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) 
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT 
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY 
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF 
 * SUCH DAMAGE. 
 */

#include "hprop.h"

RCSID("$Id$");

static int version_flag;
static int help_flag;
static char *ktname = HPROP_KEYTAB;
static char *database;
static char *mkeyfile;
static int to_stdout;
static int verbose_flag;
static int encrypt_flag;
static int decrypt_flag;
static EncryptionKey mkey5;
static krb5_data msched5;

#ifdef KRB4
static int v4_db;

#ifdef KASERVER_DB
static int ka_db;
static char *afs_cell;
#endif
#endif

static int
open_socket(krb5_context context, const char *hostname)
{
    int s;
    struct hostent *hp;
    struct sockaddr_in sin;
    s = socket(AF_INET, SOCK_STREAM, 0);
    if(s < 0){
	warn("socket");
	return -1;
    }
    hp = roken_gethostbyname(hostname);
    if(hp == NULL){
	warnx("%s: %s", hostname, hstrerror(h_errno));
	close(s);
	return -1;
    }
    memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_port = krb5_getportbyname (context, "hprop", "tcp", HPROP_PORT);
    memcpy(&sin.sin_addr, hp->h_addr, hp->h_length);
    if(connect(s, (struct sockaddr*)&sin, sizeof(sin)) < 0){
	warn("connect");
	close(s);
	return -1;
    }
    return s;
}

struct prop_data{
    krb5_context context;
    krb5_auth_context auth_context;
    int sock;
};

int hdb_entry2value(krb5_context, hdb_entry*, krb5_data*);

static krb5_error_code
v5_prop(krb5_context context, HDB *db, hdb_entry *entry, void *appdata)
{
    krb5_error_code ret;
    struct prop_data *pd = appdata;
    krb5_data data;

    if(encrypt_flag)
	hdb_seal_keys(entry, msched5);
    if(decrypt_flag)
	hdb_unseal_keys(entry, msched5);

    ret = hdb_entry2value(context, entry, &data);
    if(ret) return ret;

    if(to_stdout)
	ret = send_clear(context, STDOUT_FILENO, data);
    else
	ret = send_priv(context, pd->auth_context, &data, pd->sock);
    krb5_data_free(&data);
    return ret;
}

#ifdef KRB4
static des_cblock mkey4;
static des_key_schedule msched4;
static char realm[REALM_SZ];

static int
v4_prop(void *arg, Principal *p)
{
    struct prop_data *pd = arg;
    hdb_entry ent;
    krb5_error_code ret;

    memset(&ent, 0, sizeof(ent));

    ret = krb5_425_conv_principal(pd->context, p->name, p->instance, realm,
				  &ent.principal);
    if(ret){
	krb5_warn(pd->context, ret, "%s.%s@%s", p->name, p->instance, realm);
	return 0;
    }

    if(verbose_flag){
	char *s;
	krb5_unparse_name_short(pd->context, ent.principal, &s);
	krb5_warnx(pd->context, "%s.%s -> %s", p->name, p->instance, s);
	free(s);
    }

    ent.kvno = p->key_version;
    ent.keys.len = 3;
    ent.keys.val = malloc(ent.keys.len * sizeof(*ent.keys.val));
    ent.keys.val[0].mkvno = p->kdc_key_ver;
    ent.keys.val[0].salt = calloc(1, sizeof(*ent.keys.val[0].salt));
    ent.keys.val[0].salt->type = pa_pw_salt;
    ent.keys.val[0].key.keytype = ETYPE_DES_CBC_MD5;
    krb5_data_alloc(&ent.keys.val[0].key.keyvalue, sizeof(des_cblock));
    
    {
	unsigned char *key = ent.keys.val[0].key.keyvalue.data;
	unsigned char null_key[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
	memcpy(key, &p->key_low, 4);
	memcpy(key + 4, &p->key_high, 4);
	kdb_encrypt_key((des_cblock*)key, (des_cblock*)key, &mkey4, msched4, 0);
	if(memcmp(key, null_key, sizeof(null_key)) == 0) {
	    free_Key(&ent.keys.val[0]);
	    ent.keys.val = 0;
	    ent.flags.invalid = 1;
	}
    }
    copy_Key(&ent.keys.val[0], &ent.keys.val[1]);
    ent.keys.val[1].key.keytype = ETYPE_DES_CBC_MD4;
    copy_Key(&ent.keys.val[0], &ent.keys.val[2]);
    ent.keys.val[2].key.keytype = ETYPE_DES_CBC_CRC;

    ALLOC(ent.max_life);
    *ent.max_life = krb_life_to_time(0, p->max_life);
    if(*ent.max_life == NEVERDATE){
	free(ent.max_life);
	ent.max_life = NULL;
    }

    ALLOC(ent.pw_end);
    *ent.pw_end = p->exp_date;
    ret = krb5_make_principal(pd->context, &ent.created_by.principal,
			      realm,
			      "kadmin",
			      "hprop",
			      NULL);
    if(ret){
	krb5_warn(pd->context, ret, "krb5_make_principal");
	ret = 0;
	goto out;
    }
    ent.created_by.time = time(NULL);
    ALLOC(ent.modified_by);
    ret = krb5_425_conv_principal(pd->context, p->mod_name, p->mod_instance, 
				  realm, &ent.modified_by->principal);
    if(ret){
	krb5_warn(pd->context, ret, "%s.%s@%s", p->name, p->instance, realm);
	ent.modified_by->principal = NULL;
	ret = 0;
	goto out;
    }
    ent.modified_by->time = p->mod_date;

    ent.flags.forwardable = 1;
    ent.flags.renewable = 1;
    ent.flags.proxiable = 1;
    ent.flags.postdate = 1;
    ent.flags.client = 1;
    ent.flags.server = 1;
    
    /* special case password changing service */
    if(strcmp(p->name, "changepw") == 0 && 
       strcmp(p->instance, "kerberos") == 0) {
	ent.flags.forwardable = 0;
	ent.flags.renewable = 0;
	ent.flags.proxiable = 0;
	ent.flags.postdate = 0;
	ent.flags.initial = 1;
	ent.flags.change_pw = 1;
    }

    ret = v5_prop(pd->context, NULL, &ent, pd);

    if (strcmp (p->name, "krbtgt") == 0
	&& strcmp (realm, p->instance) != 0) {
	krb5_free_principal (pd->context, ent.principal);
	ret = krb5_425_conv_principal (pd->context, p->name,
				       realm, p->instance,
				       &ent.principal);
	if (ret == 0)
	    ret = v5_prop (pd->context, NULL, &ent, pd);
    }

out:
    hdb_free_entry(pd->context, &ent);
    return ret;
}

#ifdef KASERVER_DB

#include "kadb.h"

/* read a `ka_entry' from `fd' at offset `pos' */
static void
read_block(krb5_context context, int fd, int32_t pos, void *buf, size_t len)
{
    krb5_error_code ret;
    if(lseek(fd, 64 + pos, SEEK_SET) == (off_t)-1)
	krb5_err(context, 1, errno, "lseek(%u)", 64 + pos);
    ret = read(fd, buf, len);
    if(ret < 0)
	krb5_err(context, 1, errno, "read(%u)", len);
    if(ret != len)
	krb5_errx(context, 1, "read(%u) = %u", len, ret);
}

static int
ka_convert(struct prop_data *pd, int fd, struct ka_entry *ent, const char *cell)
{
    int32_t flags = ntohl(ent->flags);
    krb5_error_code ret;
    hdb_entry hdb;
    if((flags & KAFNORMAL) == 0) /* remove special entries */
	return 0;
    memset(&hdb, 0, sizeof(hdb));
    ret = krb5_425_conv_principal(pd->context, ent->name, ent->instance, realm,
				  &hdb.principal);
    if(ret) {
	krb5_warn(pd->context, ret,
		  "krb5_425_conv_principal (%s.%s@%s)",
		  ent->name, ent->instance, realm);
	return 0;
    }
    hdb.kvno = ntohl(ent->kvno);
    hdb.keys.len = 3;
    hdb.keys.val = malloc(hdb.keys.len * sizeof(*hdb.keys.val));
    hdb.keys.val[0].mkvno = 0; /* XXX */
    hdb.keys.val[0].salt = calloc(1, sizeof(*hdb.keys.val[0].salt));
    hdb.keys.val[0].salt->type = hdb_afs3_salt;
    hdb.keys.val[0].salt->salt.data = strdup(cell);
    hdb.keys.val[0].salt->salt.length = strlen(cell);
    
    hdb.keys.val[0].key.keytype = ETYPE_DES_CBC_MD5;
    krb5_data_copy(&hdb.keys.val[0].key.keyvalue, ent->key, sizeof(ent->key));
    copy_Key(&hdb.keys.val[0], &hdb.keys.val[1]);
    hdb.keys.val[1].key.keytype = ETYPE_DES_CBC_MD4;
    copy_Key(&hdb.keys.val[0], &hdb.keys.val[2]);
    hdb.keys.val[2].key.keytype = ETYPE_DES_CBC_CRC;

    ALLOC(hdb.max_life);
    *hdb.max_life = ntohl(ent->max_life);

    if(ntohl(ent->pw_end) != NEVERDATE && ntohl(ent->pw_end) != -1){
	ALLOC(hdb.pw_end);
	*hdb.pw_end = ntohl(ent->pw_end);
    }
    
    ret = krb5_make_principal(pd->context, &hdb.created_by.principal,
			      realm,
			      "kadmin",
			      "hprop",
			      NULL);
    hdb.created_by.time = time(NULL);

    if(ent->mod_ptr){
	struct ka_entry mod;
	ALLOC(hdb.modified_by);
	read_block(pd->context, fd, ntohl(ent->mod_ptr), &mod, sizeof(mod));
	
	krb5_425_conv_principal(pd->context, mod.name, mod.instance, realm, 
				&hdb.modified_by->principal);
	hdb.modified_by->time = ntohl(ent->mod_time);
	memset(&mod, 0, sizeof(mod));
    }

    hdb.flags.forwardable = 1;
    hdb.flags.renewable = 1;
    hdb.flags.proxiable = 1;
    hdb.flags.postdate = 1;
    /* XXX - AFS 3.4a creates krbtgt.REALMOFCELL as NOTGS+NOSEAL */
    if (strcmp(ent->name, "krbtgt") == 0 &&
	(flags & (KAFNOTGS|KAFNOSEAL)) == (KAFNOTGS|KAFNOSEAL))
	flags &= ~KAFNOTGS;

    hdb.flags.client = (flags & KAFNOTGS) == 0;
    hdb.flags.server = (flags & KAFNOSEAL) == 0;

    ret = v5_prop(pd->context, NULL, &hdb, pd);
    hdb_free_entry(pd->context, &hdb);
    return ret;
}

static int
ka_dump(struct prop_data *pd, const char *file, const char *cell)
{
    struct ka_header header;
    int i;
    int fd = open(file, O_RDONLY);

    if(fd < 0)
	krb5_err(pd->context, 1, errno, "open(%s)", file);
    read_block(pd->context, fd, 0, &header, sizeof(header));
    if(header.version1 != header.version2)
	krb5_errx(pd->context, 1, "Version mismatch in header: %d/%d", 
		  ntohl(header.version1), ntohl(header.version2));
    if(ntohl(header.version1) != 5)
	krb5_errx(pd->context, 1, "Unknown database version %d (expected 5)", 
		  ntohl(header.version1));
    for(i = 0; i < ntohl(header.hashsize); i++){
	int32_t pos = ntohl(header.hash[i]);
	while(pos){
	    struct ka_entry ent;
	    read_block(pd->context, fd, pos, &ent, sizeof(ent));
	    ka_convert(pd, fd, &ent, cell);
	    pos = ntohl(ent.next);
	}
    }
    return 0;
}

#endif /* KASERVER_DB */

#endif /* KRB4 */


struct getargs args[] = {
    { "master-key", 'm', arg_string, &mkeyfile, "v5 master key file", "file" },
#ifdef KRB4
#endif
    { "database", 'd',	arg_string, &database, "database", "file" },
#ifdef KRB4
    { "v4-db",    '4',	arg_flag, &v4_db, "use version 4 database" },
#endif
#ifdef KASERVER_DB
    { "ka-db",	  'K',  arg_flag, &ka_db, "use kaserver database" },
    { "cell",	  'c',  arg_string, &afs_cell, "name of AFS cell" },
#endif
    { "keytab",   'k',	arg_string, &ktname, "keytab to use for authentication", "keytab" },
    { "decrypt",  'D',  arg_flag,   &decrypt_flag,   "decrypt keys" },
    { "encrypt",  'E',  arg_flag,   &encrypt_flag,   "encrypt keys" },
    { "stdout",	  'n',  arg_flag,   &to_stdout, "dump to stdout" },
    { "verbose",  'v',	arg_flag, &verbose_flag },
    { "version",   0,	arg_flag, &version_flag },
    { "help",     'h',	arg_flag, &help_flag }
};

static int num_args = sizeof(args) / sizeof(args[0]);

static void
usage(int ret)
{
    arg_printusage (args, num_args, NULL, "host ...");
    exit (ret);
}

static void
get_creds(krb5_context context, krb5_ccache *cache)
{
    krb5_keytab keytab;
    krb5_principal client;
    krb5_error_code ret;
    krb5_get_init_creds_opt init_opts;
    krb5_preauthtype preauth = KRB5_PADATA_ENC_TIMESTAMP;
    krb5_creds creds;
    
    ret = krb5_kt_resolve(context, ktname, &keytab);
    if(ret) krb5_err(context, 1, ret, "krb5_kt_resolve");
    
    ret = krb5_make_principal(context, &client, NULL, 
			      "kadmin", HPROP_NAME, NULL);
    if(ret) krb5_err(context, 1, ret, "krb5_make_principal");

    krb5_get_init_creds_opt_init(&init_opts);
    krb5_get_init_creds_opt_set_preauth_list(&init_opts, &preauth, 1);

    ret = krb5_get_init_creds_keytab(context, &creds, client, keytab, 0, NULL, &init_opts);
    if(ret) krb5_err(context, 1, ret, "krb5_get_init_creds");
    
    ret = krb5_kt_close(context, keytab);
    if(ret) krb5_err(context, 1, ret, "krb5_kt_close");
    
    ret = krb5_cc_gen_new(context, &krb5_mcc_ops, cache);
    if(ret) krb5_err(context, 1, ret, "krb5_cc_gen_new");

    ret = krb5_cc_initialize(context, *cache, client);
    if(ret) krb5_err(context, 1, ret, "krb5_cc_initialize");

    ret = krb5_cc_store_cred(context, *cache, &creds);
    if(ret) krb5_err(context, 1, ret, "krb5_cc_store_cred");
}

int main(int argc, char **argv)
{
    krb5_error_code ret;
    krb5_context context;
    krb5_auth_context ac;
    krb5_principal server;
    krb5_ccache ccache;
    int fd;
    HDB *db;
    int optind = 0;
    int i;
#ifdef KRB4
    int e;
#endif


    set_progname(argv[0]);

    if(getarg(args, num_args, argc, argv, &optind))
	usage(1);

    if(help_flag)
	usage(0);
    
    if(version_flag){
	print_version(NULL);
	exit(0);
    }

    ret = krb5_init_context(&context);
    if(ret)
	exit(1);

    if(encrypt_flag && decrypt_flag)
	krb5_errx(context, 1, 
		  "Only one of `--encrypt' and `--decrypt' is meaningful");

    if(!to_stdout)
	get_creds(context, &ccache);
    
    ret = hdb_read_master_key(context, mkeyfile, &mkey5);
    if(ret && ret != ENOENT)
	krb5_err(context, 1, ret, "hdb_read_master_key");
    if(ret){
	if(encrypt_flag || decrypt_flag)
	    krb5_errx(context, 1, "No master key file found");
    }else{
	ret = hdb_process_master_key(context, mkey5, &msched5);
	if(ret)
	    krb5_err(context, 1, ret, "hdb_process_master_key");
    }
    
#ifdef KRB4
    if(v4_db){
	e = kerb_db_set_name (database);
	if(e) krb5_errx(context, 1, "kerb_db_set_name: %s", krb_get_err_text(e));
	e = kdb_get_master_key(0, &mkey4, msched4);
	if(e) krb5_errx(context, 1, "kdb_get_master_key: %s", krb_get_err_text(e));
	e = krb_get_lrealm(realm, 1);
	if(e) krb5_errx(context, 1, "krb_get_lrealm: %s", krb_get_err_text(e));
#ifdef KASERVER_DB
    }else if(ka_db) {
	e = krb_get_lrealm(realm, 1);
	if(e) krb5_errx(context, 1, "krb_get_lrealm: %s", krb_get_err_text(e));
#endif
    }else
#endif
	{
	    ret = hdb_create (context, &db, database);
	    if(ret)
		krb5_err(context, 1, ret, "hdb_create: %s", database);
	    ret = db->open(context, db, O_RDONLY, 0);
	    if(ret)
		krb5_err(context, 1, ret, "db->open");
	}

    if(to_stdout){
	struct prop_data pd;
	pd.context = context;
	pd.auth_context = ac;
	pd.sock = fd;
	
#ifdef KRB4
	if(v4_db){
	    e = kerb_db_iterate ((k_iter_proc_t)v4_prop, &pd);
	    if(e)
		krb5_errx(context, 1, "kerb_db_iterate: %s", 
			  krb_get_err_text(e));
#ifdef KASERVER_DB
	} else if(ka_db) {
	    e = ka_dump(&pd, database, afs_cell);
	    if(e) krb5_errx(context, 1, "ka_dump: %s", krb_get_err_text(e));
#endif
	} else
#endif
	{
	    ret = hdb_foreach(context, db, v5_prop, &pd);
	    if(ret)
		krb5_err(context, 1, ret, "hdb_foreach");
	}
    }else{

	for(i = optind; i < argc; i++){
	    fd = open_socket(context, argv[i]);
	    if(fd < 0)
		continue;

	    ret = krb5_sname_to_principal(context, argv[i], 
					  HPROP_NAME, KRB5_NT_SRV_HST, &server);
	    if(ret) {
		krb5_warn(context, ret, "krb5_sname_to_principal(%s)", argv[i]);
		close(fd);
		continue;
	    }
    
	    ac = NULL;
	    ret = krb5_sendauth(context,
				&ac,
				&fd,
				HPROP_VERSION,
				NULL,
				server,
				AP_OPTS_MUTUAL_REQUIRED,
				NULL, /* in_data */
				NULL, /* in_creds */
				ccache,
				NULL,
				NULL,
				NULL);

	    if(ret){
		krb5_warn(context, ret, "krb5_sendauth");
		close(fd);
		continue;
	    }

	    {
		struct prop_data pd;
		pd.context = context;
		pd.auth_context = ac;
		pd.sock = fd;
	
#ifdef KRB4
		if(v4_db)
		    e = kerb_db_iterate ((k_iter_proc_t)v4_prop, &pd);
#ifdef KASERVER_DB
		else if(ka_db) {
		    e = ka_dump(&pd, database, afs_cell);
		    if(e) krb5_errx(context, 1, "ka_dump: %s", 
				    krb_get_err_text(e));
		}
#endif
		else
#endif
		    ret = hdb_foreach(context, db, v5_prop, &pd);
	    }
	    if(ret)
		krb5_warn(context, ret, "krb5_sendauth");
	    else {
		krb5_data data;
		data.data = NULL;
		data.length = 0;
		ret = send_priv(context, ac, &data, fd);
	    }

	    {
		krb5_data data;
		ret = recv_priv(context, ac, fd, &data);
		if(ret) krb5_warn(context, ret, "recv_priv");
		if(data.length != 0)
		    krb5_data_free(&data); /* XXX */
	    }
	
	    if(ret) krb5_warn(context, ret, "send_priv");
	    krb5_auth_con_free(context, ac);
	    close(fd);
	}
    }
    exit(0);
}
