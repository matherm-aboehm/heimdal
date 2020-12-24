/*
 * Copyright (c) 1997 - 2005 Kungliga Tekniska Högskolan
 * (Royal Institute of Technology, Stockholm, Sweden).
 * All rights reserved.
 *
 * Portions Copyright (c) 2009 Apple Inc. All rights reserved.
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
 * 3. Neither the name of the Institute nor the names of its contributors
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

#include "gen_locl.h"

extern const char *enum_prefix;
extern int prefix_enum;

RCSID("$Id$");

FILE *privheaderfile, *headerfile, *oidsfile, *codefile, *logfile, *templatefile;

#define STEM "asn1"

static const char *orig_filename;
static char *privheader, *header, *template;
static const char *headerbase = STEM;

/*
 * list of all IMPORTs
 */

struct import {
    const char *module;
    struct import *next;
};

static struct import *imports = NULL;

void
add_import (const char *module)
{
    struct import *tmp = emalloc (sizeof(*tmp));

    tmp->module = module;
    tmp->next   = imports;
    imports     = tmp;

    fprintf (headerfile, "#include <%s_asn1.h>\n", module);
}

/*
 * List of all exported symbols
 */

struct sexport {
    const char *name;
    int defined;
    struct sexport *next;
};

static struct sexport *exports = NULL;

void
add_export (const char *name)
{
    struct sexport *tmp = emalloc (sizeof(*tmp));

    tmp->name   = name;
    tmp->next   = exports;
    exports     = tmp;
}

int
is_export(const char *name)
{
    struct sexport *tmp;

    if (exports == NULL) /* no export list, all exported */
	return 1;

    for (tmp = exports; tmp != NULL; tmp = tmp->next) {
	if (strcmp(tmp->name, name) == 0) {
	    tmp->defined = 1;
	    return 1;
	}
    }
    return 0;
}

const char *
get_filename (void)
{
    return orig_filename;
}

void
init_generate (const char *filename, const char *base)
{
    char *fn = NULL;

    orig_filename = filename;
    if (base != NULL) {
	headerbase = strdup(base);
	if (headerbase == NULL)
	    errx(1, "strdup");
    }

    /* public header file */
    if (asprintf(&header, "%s.h", headerbase) < 0 || header == NULL)
	errx(1, "malloc");
    if (asprintf(&fn, "%s.hx", headerbase) < 0 || fn == NULL)
	errx(1, "malloc");
    headerfile = fopen (fn, "w");
    if (headerfile == NULL)
	err (1, "open %s", fn);
    free(fn);
    fn = NULL;

    /* private header file */
    if (asprintf(&privheader, "%s-priv.h", headerbase) < 0 || privheader == NULL)
	errx(1, "malloc");
    if (asprintf(&fn, "%s-priv.hx", headerbase) < 0 || fn == NULL)
	errx(1, "malloc");
    privheaderfile = fopen (fn, "w");
    if (privheaderfile == NULL)
	err (1, "open %s", fn);
    free(fn);
    fn = NULL;

    /* template file */
    if (asprintf(&template, "%s-template.x", headerbase) < 0 || template == NULL)
	errx(1, "malloc");
    fprintf (headerfile,
	     "/* Generated from %s */\n"
	     "/* Do not edit */\n\n",
	     filename);
    fprintf (headerfile,
	     "#ifndef __%s_h__\n"
	     "#define __%s_h__\n\n", headerbase, headerbase);
    fprintf (headerfile,
	     "#include <stddef.h>\n"
	     "#include <time.h>\n\n");
    fprintf (headerfile,
	     "#ifndef __asn1_common_definitions__\n"
	     "#define __asn1_common_definitions__\n\n");
	fprintf (headerfile,
		 "#ifndef __HEIM_BASE_DATA__\n"
		 "#define __HEIM_BASE_DATA__ 1\n"
		 "struct heim_base_data {\n"
		 "    size_t length;\n"
		 "    void *data;\n"
		 "};\n"
		 "typedef struct heim_base_data heim_octet_string;\n"
		 "#endif\n\n");
    fprintf (headerfile,
	     "typedef struct heim_integer {\n"
	     "  size_t length;\n"
	     "  void *data;\n"
	     "  int negative;\n"
	     "} heim_integer;\n\n");
    fprintf (headerfile,
	     "typedef char *heim_general_string;\n\n"
	     );
    fprintf (headerfile,
	     "typedef char *heim_utf8_string;\n\n"
	     );
    fprintf (headerfile,
	     "typedef struct heim_base_data heim_printable_string;\n\n"
	     );
    fprintf (headerfile,
	     "typedef struct heim_base_data heim_ia5_string;\n\n"
	     );
    fprintf (headerfile,
	     "typedef struct heim_bmp_string {\n"
	     "  size_t length;\n"
	     "  uint16_t *data;\n"
	     "} heim_bmp_string;\n\n");
    fprintf (headerfile,
	     "typedef struct heim_universal_string {\n"
	     "  size_t length;\n"
	     "  uint32_t *data;\n"
	     "} heim_universal_string;\n\n");
    fprintf (headerfile,
	     "typedef char *heim_visible_string;\n\n"
	     );
    fprintf (headerfile,
	     "typedef struct heim_oid {\n"
	     "  size_t length;\n"
	     "  unsigned *components;\n"
	     "} heim_oid;\n\n");
    fprintf (headerfile,
	     "typedef struct heim_bit_string {\n"
	     "  size_t length;\n"
	     "  void *data;\n"
	     "} heim_bit_string;\n\n");
    fprintf (headerfile,
	     "typedef struct heim_base_data heim_any;\n"
	     "typedef struct heim_base_data heim_any_set;\n\n");
    fputs("#define ASN1_MALLOC_ENCODE(T, B, BL, S, L, R)                  \\\n"
	  "  do {                                                         \\\n"
	  "    (BL) = length_##T((S));                                    \\\n"
	  "    (B) = malloc((BL));                                        \\\n"
	  "    if((B) == NULL) {                                          \\\n"
	  "      (R) = ENOMEM;                                            \\\n"
	  "    } else {                                                   \\\n"
	  "      (R) = encode_##T(((unsigned char*)(B)) + (BL) - 1, (BL), \\\n"
	  "                       (S), (L));                              \\\n"
	  "      if((R) != 0) {                                           \\\n"
	  "        free((B));                                             \\\n"
	  "        (B) = NULL;                                            \\\n"
	  "      }                                                        \\\n"
	  "    }                                                          \\\n"
	  "  } while (0)\n\n",
	  headerfile);
    fputs("#ifdef _WIN32\n"
	  "#ifndef ASN1_LIB\n"
	  "#define ASN1EXP  __declspec(dllimport)\n"
	  "#else\n"
	  "#define ASN1EXP\n"
	  "#endif\n"
	  "#define ASN1CALL __stdcall\n"
	  "#else\n"
	  "#define ASN1EXP\n"
	  "#define ASN1CALL\n"
	  "#endif\n",
	  headerfile);
    fprintf (headerfile, "struct units;\n\n");
    fprintf (headerfile, "#endif\n\n");
    if (asprintf(&fn, "%s_files", base) < 0 || fn == NULL)
	errx(1, "malloc");
    logfile = fopen(fn, "w");
    if (logfile == NULL)
	err (1, "open %s", fn);
    free(fn);
    fn = NULL;

    if (asprintf(&fn, "%s_oids.x", base) < 0 || fn == NULL)
	errx(1, "malloc");
    oidsfile = fopen(fn, "w");
    if (oidsfile == NULL)
	err (1, "open %s", fn);
    free(fn);
    fn = NULL;

    /* if one code file, write into the one codefile */
    if (one_code_file)
	return;

    templatefile = fopen (template, "w");
    if (templatefile == NULL)
	err (1, "open %s", template);

    fprintf (templatefile,
	     "/* Generated from %s */\n"
	     "/* Do not edit */\n\n"
	     "#include <stdio.h>\n"
	     "#include <stdlib.h>\n"
	     "#include <time.h>\n"
	     "#include <string.h>\n"
	     "#include <errno.h>\n"
	     "#include <limits.h>\n"
	     "#include <%s>\n",
	     filename,
	     type_file_string);

    fprintf (templatefile,
	     "#include <%s>\n"
	     "#include <%s>\n"
	     "#include <der.h>\n"
	     "#include <asn1-template.h>\n",
	     header, privheader);


}

void
close_generate (void)
{
    fprintf (headerfile, "#endif /* __%s_h__ */\n", headerbase);

    if (headerfile && fclose(headerfile) == EOF)
        err(1, "writes to public header file failed");
    if (privheaderfile && fclose(privheaderfile) == EOF)
        err(1, "writes to private header file failed");
    if (templatefile && fclose(templatefile) == EOF)
        err(1, "writes to template file failed");
    if (logfile) {
        fprintf(logfile, "\n");
        if (fclose(logfile) == EOF)
            err(1, "writes to log file failed");
    }
}

void
gen_assign_defval(const char *var, struct value *val)
{
    switch(val->type) {
    case stringvalue:
	fprintf(codefile, "if((%s = strdup(\"%s\")) == NULL)\nreturn ENOMEM;\n", var, val->u.stringvalue);
	break;
    case integervalue:
	fprintf(codefile, "%s = %lld;\n",
		var, (long long)val->u.integervalue);
	break;
    case booleanvalue:
	if(val->u.booleanvalue)
	    fprintf(codefile, "%s = 1;\n", var);
	else
	    fprintf(codefile, "%s = 0;\n", var);
	break;
    default:
	abort();
    }
}

void
gen_compare_defval(const char *var, struct value *val)
{
    switch(val->type) {
    case stringvalue:
	fprintf(codefile, "if(strcmp(%s, \"%s\") != 0)\n", var, val->u.stringvalue);
	break;
    case integervalue:
	fprintf(codefile, "if(%s != %lld)\n",
		var, (long long)val->u.integervalue);
	break;
    case booleanvalue:
	if(val->u.booleanvalue)
	    fprintf(codefile, "if(!%s)\n", var);
	else
	    fprintf(codefile, "if(%s)\n", var);
	break;
    default:
	abort();
    }
}

void
generate_header_of_codefile(const char *name)
{
    char *filename = NULL;

    if (codefile != NULL)
	abort();

    if (asprintf (&filename, "%s_%s.x", STEM, name) < 0 || filename == NULL)
	errx(1, "malloc");
    codefile = fopen (filename, "w");
    if (codefile == NULL)
	err (1, "fopen %s", filename);
    if (logfile)
        fprintf(logfile, "%s ", filename);
    free(filename);
    filename = NULL;
    fprintf (codefile,
	     "/* Generated from %s */\n"
	     "/* Do not edit */\n\n"
	     "#define  ASN1_LIB\n\n"
	     "#include <stdio.h>\n"
	     "#include <stdlib.h>\n"
	     "#include <time.h>\n"
	     "#include <string.h>\n"
	     "#include <errno.h>\n"
	     "#include <limits.h>\n"
	     "#include <%s>\n",
	     orig_filename,
	     type_file_string);

    fprintf (codefile,
	     "#include \"%s\"\n"
	     "#include \"%s\"\n",
	     header, privheader);
    fprintf (codefile,
	     "#include <asn1_err.h>\n"
	     "#include <der.h>\n"
	     "#include <asn1-template.h>\n\n");

    if (parse_units_flag)
	fprintf (codefile,
		 "#include <parse_units.h>\n\n");

#ifdef _WIN32
    fprintf(codefile, "#pragma warning(disable: 4101)\n\n");
#endif
}

void
close_codefile(void)
{
    if (codefile == NULL)
	abort();

    if (fclose(codefile) == EOF)
        err(1, "writes to source code file failed");
    codefile = NULL;
}


void
generate_constant (const Symbol *s)
{
    switch(s->value->type) {
    case booleanvalue:
	break;
    case integervalue:
        /*
         * Work around the fact that OpenSSL defines macros for PKIX constants
         * that we want to generate as enums, which causes conflicts for things
         * like ub-name (ub_name).
         */
        fprintf(headerfile,
                "#ifdef %s\n"
                "#undef %s\n"
                "#endif\n"
                "enum { %s = %lld };\n\n",
                s->gen_name, s->gen_name, s->gen_name,
                (long long)s->value->u.integervalue);
	break;
    case nullvalue:
	break;
    case stringvalue:
	break;
    case objectidentifiervalue: {
	struct objid *o, **list;
	size_t i, len;
	char *gen_upper;

	if (!one_code_file)
	    generate_header_of_codefile(s->gen_name);

	len = 0;
	for (o = s->value->u.objectidentifiervalue; o != NULL; o = o->next)
	    len++;
	if (len == 0) {
	    printf("s->gen_name: %s",s->gen_name);
	    fflush(stdout);
	    break;
	}
	list = emalloc(sizeof(*list) * len);

	i = 0;
	for (o = s->value->u.objectidentifiervalue; o != NULL; o = o->next)
	    list[i++] = o;

	fprintf (headerfile, "/* OBJECT IDENTIFIER %s ::= { ", s->name);
	for (i = len ; i > 0; i--) {
	    o = list[i - 1];
	    fprintf(headerfile, "%s(%d) ",
		    o->label ? o->label : "label-less", o->value);
	}

	fprintf (codefile, "static unsigned oid_%s_variable_num[%lu] =  {",
		 s->gen_name, (unsigned long)len);
	for (i = len ; i > 0; i--) {
	    fprintf(codefile, "%d%s ", list[i - 1]->value, i > 1 ? "," : "");
	}
	fprintf(codefile, "};\n");

	fprintf (codefile, "const heim_oid asn1_oid_%s = "
		 "{ %lu, oid_%s_variable_num };\n\n",
		 s->gen_name, (unsigned long)len, s->gen_name);

        fprintf(oidsfile, "DEFINE_OID_WITH_NAME(%s)\n", s->gen_name);

	free(list);

	/* header file */

	gen_upper = strdup(s->gen_name);
	len = strlen(gen_upper);
	for (i = 0; i < len; i++)
	    gen_upper[i] = toupper((int)s->gen_name[i]);

	fprintf (headerfile, "} */\n");
	fprintf (headerfile,
		 "extern ASN1EXP const heim_oid asn1_oid_%s;\n"
		 "#define ASN1_OID_%s (&asn1_oid_%s)\n\n",
		 s->gen_name,
		 gen_upper,
		 s->gen_name);

	free(gen_upper);

	if (!one_code_file)
	    close_codefile();

	break;
    }
    default:
	abort();
    }
}

int
is_primitive_type(int type)
{
    switch(type) {
    case TInteger:
    case TBoolean:
    case TOctetString:
    case TBitString:
    case TEnumerated:
    case TGeneralizedTime:
    case TGeneralString:
    case TTeletexString:
    case TOID:
    case TUTCTime:
    case TUTF8String:
    case TPrintableString:
    case TIA5String:
    case TBMPString:
    case TUniversalString:
    case TVisibleString:
    case TNull:
	return 1;
    default:
	return 0;
    }
}

static void
space(int level)
{
    while(level-- > 0)
	fprintf(headerfile, "  ");
}

static const char *
last_member_p(struct member *m)
{
    struct member *n = HEIM_TAILQ_NEXT(m, members);
    if (n == NULL)
	return "";
    if (n->ellipsis && HEIM_TAILQ_NEXT(n, members) == NULL)
	return "";
    return ",";
}

static struct member *
have_ellipsis(Type *t)
{
    struct member *m;
    HEIM_TAILQ_FOREACH(m, t->members, members) {
	if (m->ellipsis)
	    return m;
    }
    return NULL;
}

static void
define_asn1 (int level, Type *t)
{
    switch (t->type) {
    case TType:
	fprintf (headerfile, "%s", t->symbol->name);
	break;
    case TInteger:
	if(t->members == NULL) {
            fprintf (headerfile, "INTEGER");
	    if (t->range)
		fprintf (headerfile, " (%lld..%lld)",
			 (long long)t->range->min,
			 (long long)t->range->max);
        } else {
	    Member *m;
            fprintf (headerfile, "INTEGER {\n");
	    HEIM_TAILQ_FOREACH(m, t->members, members) {
                space (level + 1);
		fprintf(headerfile, "%s(%d)%s\n", m->gen_name, m->val,
			last_member_p(m));
            }
	    space(level);
            fprintf (headerfile, "}");
        }
	break;
    case TBoolean:
	fprintf (headerfile, "BOOLEAN");
	break;
    case TOctetString:
	fprintf (headerfile, "OCTET STRING");
	break;
    case TEnumerated :
    case TBitString: {
	Member *m;

	space(level);
	if(t->type == TBitString)
	    fprintf (headerfile, "BIT STRING {\n");
	else
	    fprintf (headerfile, "ENUMERATED {\n");
	HEIM_TAILQ_FOREACH(m, t->members, members) {
	    space(level + 1);
	    fprintf (headerfile, "%s(%d)%s\n", m->name, m->val,
		     last_member_p(m));
	}
	space(level);
	fprintf (headerfile, "}");
	break;
    }
    case TChoice:
    case TSet:
    case TSequence: {
	Member *m;
	size_t max_width = 0;

	if(t->type == TChoice)
	    fprintf(headerfile, "CHOICE {\n");
	else if(t->type == TSet)
	    fprintf(headerfile, "SET {\n");
	else
	    fprintf(headerfile, "SEQUENCE {\n");
	HEIM_TAILQ_FOREACH(m, t->members, members) {
	    if(strlen(m->name) > max_width)
		max_width = strlen(m->name);
	}
	max_width += 3;
	if(max_width < 16) max_width = 16;
	HEIM_TAILQ_FOREACH(m, t->members, members) {
	    size_t width = max_width;
	    space(level + 1);
	    if (m->ellipsis) {
		fprintf (headerfile, "...");
	    } else {
		width -= fprintf(headerfile, "%s", m->name);
		fprintf(headerfile, "%*s", (int)width, "");
		define_asn1(level + 1, m->type);
		if(m->optional)
		    fprintf(headerfile, " OPTIONAL");
	    }
	    if(last_member_p(m))
		fprintf (headerfile, ",");
	    fprintf (headerfile, "\n");
	}
	space(level);
	fprintf (headerfile, "}");
	break;
    }
    case TSequenceOf:
	fprintf (headerfile, "SEQUENCE OF ");
	define_asn1 (0, t->subtype);
	break;
    case TSetOf:
	fprintf (headerfile, "SET OF ");
	define_asn1 (0, t->subtype);
	break;
    case TGeneralizedTime:
	fprintf (headerfile, "GeneralizedTime");
	break;
    case TGeneralString:
	fprintf (headerfile, "GeneralString");
	break;
    case TTeletexString:
	fprintf (headerfile, "TeletexString");
	break;
    case TTag: {
	const char *classnames[] = { "UNIVERSAL ", "APPLICATION ",
				     "" /* CONTEXT */, "PRIVATE " };
	if(t->tag.tagclass != ASN1_C_UNIV)
	    fprintf (headerfile, "[%s%d] ",
		     classnames[t->tag.tagclass],
		     t->tag.tagvalue);
	if(t->tag.tagenv == TE_IMPLICIT)
	    fprintf (headerfile, "IMPLICIT ");
	define_asn1 (level, t->subtype);
	break;
    }
    case TUTCTime:
	fprintf (headerfile, "UTCTime");
	break;
    case TUTF8String:
	space(level);
	fprintf (headerfile, "UTF8String");
	break;
    case TPrintableString:
	space(level);
	fprintf (headerfile, "PrintableString");
	break;
    case TIA5String:
	space(level);
	fprintf (headerfile, "IA5String");
	break;
    case TBMPString:
	space(level);
	fprintf (headerfile, "BMPString");
	break;
    case TUniversalString:
	space(level);
	fprintf (headerfile, "UniversalString");
	break;
    case TVisibleString:
	space(level);
	fprintf (headerfile, "VisibleString");
	break;
    case TOID :
	space(level);
	fprintf(headerfile, "OBJECT IDENTIFIER");
	break;
    case TNull:
	space(level);
	fprintf (headerfile, "NULL");
	break;
    default:
	abort ();
    }
}

static void
getnewbasename(char **newbasename, int typedefp, const char *basename, const char *name)
{
    if (typedefp)
	*newbasename = strdup(name);
    else {
	if (name[0] == '*')
	    name++;
	if (asprintf(newbasename, "%s_%s", basename, name) < 0)
	    errx(1, "malloc");
    }
    if (*newbasename == NULL)
	err(1, "malloc");
}

static void
define_type (int level, const char *name, const char *basename, Type *t, int typedefp, int preservep)
{
    const char *label_prefix = NULL;
    const char *label_prefix_sep = NULL;
    char *newbasename = NULL;

    switch (t->type) {
    case TType:
	space(level);
        fprintf(headerfile, "%s %s;\n", t->symbol->gen_name, name);
	break;
    case TInteger:
        if (t->symbol && t->symbol->emitted_definition)
            break;

	space(level);
	if(t->members) {
            Member *m;

            label_prefix = prefix_enum ? name : (enum_prefix ? enum_prefix : "");
            label_prefix_sep = prefix_enum ? "_" : "";
            fprintf (headerfile, "enum %s {\n", typedefp ? name : "");
	    HEIM_TAILQ_FOREACH(m, t->members, members) {
                space (level + 1);
                fprintf(headerfile, "%s%s%s = %d%s\n",
                        label_prefix, label_prefix_sep,
                        m->gen_name, m->val, last_member_p(m));
            }
            fprintf(headerfile, "} %s;\n", name);
	} else if (t->range == NULL) {
            fprintf(headerfile, "heim_integer %s;\n", name);
	} else if (t->range->min < INT_MIN && t->range->max <= INT64_MAX) {
            fprintf(headerfile, "int64_t %s;\n", name);
	} else if (t->range->min >= 0 && t->range->max > UINT_MAX) {
	    fprintf (headerfile, "uint64_t %s;\n", name);
	} else if (t->range->min >= INT_MIN && t->range->max <= INT_MAX) {
	    fprintf (headerfile, "int %s;\n", name);
	} else if (t->range->min >= 0 && t->range->max <= UINT_MAX) {
	    fprintf (headerfile, "unsigned int %s;\n", name);
	} else
	    errx(1, "%s: unsupported range %lld -> %lld",
		 name, (long long)t->range->min, (long long)t->range->max);
	break;
    case TBoolean:
	space(level);
	fprintf (headerfile, "int %s;\n", name);
	break;
    case TOctetString:
	space(level);
	fprintf (headerfile, "heim_octet_string %s;\n", name);
	break;
    case TBitString: {
	Member *m;
	Type i;
	struct range range = { 0, UINT_MAX };
        size_t max_memno = 0;
        size_t bitset_size;

        if (t->symbol && t->symbol->emitted_definition)
            break;
        memset(&i, 0, sizeof(i));

        /*
         * range.max implies the size of the base unsigned integer used for the
         * bitfield members.  If it's less than or equal to UINT_MAX, then that
         * will be unsigned int, otherwise it will be uint64_t.
         *
         * We could just use uint64_t, yes, but for now, and in case that any
         * projects were exposing the BIT STRING types' C representations in
         * ABIs prior to this compiler supporting BIT STRING with larger
         * members, we stick to this.
         */
        HEIM_TAILQ_FOREACH(m, t->members, members) {
            if (m->val > max_memno)
                max_memno = m->val;
        }
        if (max_memno > 63)
            range.max = INT64_MAX;
        else
            range.max = 1ULL << max_memno;

	i.type = TInteger;
	i.range = &range;
	i.members = NULL;
	i.constraint = NULL;

	space(level);
	if(HEIM_TAILQ_EMPTY(t->members))
	    fprintf (headerfile, "heim_bit_string %s;\n", name);
	else {
	    int pos = 0;
	    getnewbasename(&newbasename, typedefp || level == 0, basename, name);

	    fprintf (headerfile, "struct %s {\n", newbasename);
	    HEIM_TAILQ_FOREACH(m, t->members, members) {
		char *n = NULL;

		/*
                 * pad unused bits beween declared members (hopefully this
                 * forces the compiler to give us an obvious layout)
                 */
		while (pos < m->val) {
		    if (asprintf (&n, "_unused%d:1", pos) < 0 || n == NULL)
			err(1, "malloc");
		    define_type (level + 1, n, newbasename, &i, FALSE, FALSE);
		    free(n);
		    pos++;
		}

		n = NULL;
		if (asprintf (&n, "%s:1", m->gen_name) < 0 || n == NULL)
		    errx(1, "malloc");
		define_type (level + 1, n, newbasename, &i, FALSE, FALSE);
		free (n);
		n = NULL;
		pos++;
	    }
	    /* pad unused tail (ditto) */
            bitset_size = max_memno;
            if (max_memno > 31)
                bitset_size += 64 - (max_memno % 64);
            else
                bitset_size = 32;
	    while (pos < bitset_size) {
		char *n = NULL;
		if (asprintf (&n, "_unused%d:1", pos) < 0 || n == NULL)
		    errx(1, "malloc");
		define_type (level + 1, n, newbasename, &i, FALSE, FALSE);
		free(n);
		pos++;
	    }

	    space(level);
	    fprintf (headerfile, "} %s;\n\n", name);
	}
	break;
    }
    case TEnumerated: {
	Member *m;

        if (t->symbol && t->symbol->emitted_definition)
            break;

        label_prefix = prefix_enum ? name : (enum_prefix ? enum_prefix : "");
        label_prefix_sep = prefix_enum ? "_" : "";
	space(level);
	fprintf (headerfile, "enum %s {\n", typedefp ? name : "");
	HEIM_TAILQ_FOREACH(m, t->members, members) {
	    space(level + 1);
	    if (m->ellipsis)
		fprintf (headerfile, "/* ... */\n");
	    else
		fprintf(headerfile, "%s%s%s = %d%s\n",
                        label_prefix, label_prefix_sep,
                        m->gen_name, m->val, last_member_p(m));
	}
	space(level);
	fprintf (headerfile, "} %s;\n\n", name);
	break;
    }
    case TSet:
    case TSequence: {
	Member *m;

	getnewbasename(&newbasename, typedefp || level == 0, basename, name);

	space(level);
	fprintf (headerfile, "struct %s {\n", newbasename);
	if (t->type == TSequence && preservep) {
	    space(level + 1);
	    fprintf(headerfile, "heim_octet_string _save;\n");
	}
	HEIM_TAILQ_FOREACH(m, t->members, members) {
	    if (m->ellipsis) {
		;
	    } else if (m->optional) {
		char *n = NULL;

		if (asprintf (&n, "*%s", m->gen_name) < 0 || n == NULL)
		    errx(1, "malloc");
		define_type (level + 1, n, newbasename, m->type, FALSE, FALSE);
		free (n);
	    } else
		define_type (level + 1, m->gen_name, newbasename, m->type, FALSE, FALSE);
	}
	space(level);
	fprintf (headerfile, "} %s;\n", name);
	break;
    }
    case TSetOf:
    case TSequenceOf: {
	Type i;
	struct range range = { 0, UINT_MAX };

	getnewbasename(&newbasename, typedefp || level == 0, basename, name);

        memset(&i, 0, sizeof(i));
	i.type = TInteger;
	i.range = &range;

	space(level);
	fprintf (headerfile, "struct %s {\n", newbasename);
	define_type (level + 1, "len", newbasename, &i, FALSE, FALSE);
	define_type (level + 1, "*val", newbasename, t->subtype, FALSE, FALSE);
	space(level);
	fprintf (headerfile, "} %s;\n", name);
	break;
    }
    case TGeneralizedTime:
	space(level);
	fprintf (headerfile, "time_t %s;\n", name);
	break;
    case TGeneralString:
	space(level);
	fprintf (headerfile, "heim_general_string %s;\n", name);
	break;
    case TTeletexString:
	space(level);
	fprintf (headerfile, "heim_general_string %s;\n", name);
	break;
    case TTag:
        define_type (level, name, basename, t->subtype, typedefp, preservep);
	break;
    case TChoice: {
	int first = 1;
	Member *m;

	getnewbasename(&newbasename, typedefp || level == 0, basename, name);

	space(level);
	fprintf (headerfile, "struct %s {\n", newbasename);
	if (preservep) {
	    space(level + 1);
	    fprintf(headerfile, "heim_octet_string _save;\n");
	}
	space(level + 1);
	fprintf (headerfile, "enum %s_enum {\n", newbasename);
	m = have_ellipsis(t);
	if (m) {
	    space(level + 2);
	    fprintf (headerfile, "%s = 0,\n", m->label);
	    first = 0;
	}
	HEIM_TAILQ_FOREACH(m, t->members, members) {
	    space(level + 2);
	    if (m->ellipsis)
		fprintf (headerfile, "/* ... */\n");
	    else
		fprintf (headerfile, "%s%s%s\n", m->label,
			 first ? " = 1" : "",
			 last_member_p(m));
	    first = 0;
	}
	space(level + 1);
	fprintf (headerfile, "} element;\n");
	space(level + 1);
	fprintf (headerfile, "union {\n");
	HEIM_TAILQ_FOREACH(m, t->members, members) {
	    if (m->ellipsis) {
		space(level + 2);
		fprintf(headerfile, "heim_octet_string asn1_ellipsis;\n");
	    } else if (m->optional) {
		char *n = NULL;

		if (asprintf (&n, "*%s", m->gen_name) < 0 || n == NULL)
		    errx(1, "malloc");
		define_type (level + 2, n, newbasename, m->type, FALSE, FALSE);
		free (n);
	    } else
		define_type (level + 2, m->gen_name, newbasename, m->type, FALSE, FALSE);
	}
	space(level + 1);
	fprintf (headerfile, "} u;\n");
	space(level);
	fprintf (headerfile, "} %s;\n", name);
	break;
    }
    case TUTCTime:
	space(level);
	fprintf (headerfile, "time_t %s;\n", name);
	break;
    case TUTF8String:
	space(level);
	fprintf (headerfile, "heim_utf8_string %s;\n", name);
	break;
    case TPrintableString:
	space(level);
	fprintf (headerfile, "heim_printable_string %s;\n", name);
	break;
    case TIA5String:
	space(level);
	fprintf (headerfile, "heim_ia5_string %s;\n", name);
	break;
    case TBMPString:
	space(level);
	fprintf (headerfile, "heim_bmp_string %s;\n", name);
	break;
    case TUniversalString:
	space(level);
	fprintf (headerfile, "heim_universal_string %s;\n", name);
	break;
    case TVisibleString:
	space(level);
	fprintf (headerfile, "heim_visible_string %s;\n", name);
	break;
    case TOID :
	space(level);
	fprintf (headerfile, "heim_oid %s;\n", name);
	break;
    case TNull:
	space(level);
	fprintf (headerfile, "int %s;\n", name);
	break;
    default:
	abort ();
    }
    free(newbasename);
}

static void
declare_type(const Symbol *s, Type *t, int typedefp)
{
    char *newbasename = NULL;

    if (typedefp)
        fprintf(headerfile, "typedef ");

    switch (t->type) {
    case TType:
        define_type(0, s->gen_name, s->gen_name, s->type, TRUE, TRUE);
        if (template_flag)
            generate_template_type_forward(s->gen_name);
        emitted_declaration(s);
        return;
    case TInteger:
    case TBoolean:
    case TOctetString:
    case TBitString: 
    case TEnumerated: 
    case TGeneralizedTime:
    case TGeneralString:
    case TTeletexString:
    case TUTCTime:
    case TUTF8String:
    case TPrintableString:
    case TIA5String:
    case TBMPString:
    case TUniversalString:
    case TVisibleString:
    case TOID :
    case TNull:
        define_type(0, s->gen_name, s->gen_name, s->type, TRUE, TRUE);
        if (template_flag)
            generate_template_type_forward(s->gen_name);
        emitted_declaration(s);
        emitted_definition(s);
        return;
    case TTag:
	declare_type(s, t->subtype, FALSE);
        emitted_declaration(s);
	return;
    default:
        break;
    }

    switch (t->type) {
    case TSet:
    case TSequence:
	getnewbasename(&newbasename, TRUE, s->gen_name, s->gen_name);
	fprintf(headerfile, "struct %s %s;\n", newbasename, s->gen_name);
	break;
    case TSetOf:
    case TSequenceOf:
	getnewbasename(&newbasename, TRUE, s->gen_name, s->gen_name);
	fprintf(headerfile, "struct %s %s;\n", newbasename, s->gen_name);
	break;
    case TChoice:
	getnewbasename(&newbasename, TRUE, s->gen_name, s->gen_name);
	fprintf(headerfile, "struct %s %s;\n", newbasename, s->gen_name);
	break;
    default:
	abort ();
    }
    free(newbasename);
    emitted_declaration(s);
}

static void generate_subtypes_header_helper(const Member *m);
static void generate_type_header(const Symbol *);

static void
generate_subtypes_header_helper(const Member *m)
{
    Member *sm;
    Symbol *s;

    if (m->ellipsis)
        return;
    if (m->type->symbol && (s = getsym(m->type->symbol->name)) &&
        !s->emitted_definition) {
        /* A field of some named type; recurse */
        if (!m->optional && !m->defval)
            generate_type_header(s);
        return;
    }
    if (!m->type->subtype && !m->type->members)
        return;
    if (m->type->type == TTag &&
        m->type->subtype && m->type->subtype->symbol &&
        (s = getsym(m->type->subtype->symbol->name))) {
        if (!m->optional && !m->defval)
            generate_type_header(s);
        return;
    }
    if (m->type->subtype) {
        switch (m->type->subtype->type) {
        case TSet:
        case TSequence:
        case TChoice:
            break;
        default:
            return;
        }
        /* A field of some anonymous (inlined) structured type */
        HEIM_TAILQ_FOREACH(sm, m->type->subtype->members, members) {
            generate_subtypes_header_helper(sm);
        }
    }
    if (m->type->members) {
        HEIM_TAILQ_FOREACH(sm, m->type->members, members) {
            generate_subtypes_header_helper(sm);
        }
    }
}

static void
generate_subtypes_header(const Symbol *s)
{
    Type *t = s->type;
    Member *m;

    /*
     * Recurse down structured types to make sure top-level types get
     * defined before they are referenced.
     *
     * We'll take care to skip OPTIONAL member fields of constructed types so
     * that we can have circular types like:
     *
     *  Foo ::= SEQUENCE {
     *    bar Bar OPTIONAL
     *  }
     *
     *  Bar ::= SEQUENCE {
     *    foo Foo OPTIONAL
     *  }
     *
     * not unlike XDR, which uses `*' to mean "optional", except in XDR it's
     * called a "pointer".  With some care we should be able to eventually
     * support the silly XDR linked list example:
     *
     *  ListOfFoo ::= SEQUENCE {
     *    someField SomeType,
     *    next ListOfFoo OPTIONAL
     *  }
     *
     * Not that anyone needs it -- just use a SEQUENCE OF and be done.
     */

    while (t->type == TTag && t->subtype) {
        switch (t->subtype->type) {
        case TTag:
        case TSet:
        case TSequence:
        case TChoice:
            t = t->subtype;
            continue;
        default:
            break;
        }
        break;
    }

    switch (t->type) {
    default: return;
    case TType:
        if (t->symbol && (s = getsym(t->symbol->name)))
            generate_type_header(s);
        return;
    case TSet:
    case TSequence:
    case TChoice:
        break;
    }

    HEIM_TAILQ_FOREACH(m, t->members, members) {
        generate_subtypes_header_helper(m);
    }
}

static void
generate_type_header (const Symbol *s)
{

    /*
     * Recurse down the types of member fields of `s' to make sure that
     * referenced types have had their definitions emitted already if the
     * member fields are not OPTIONAL/DEFAULTed.
     */
    if (s->type)
        generate_subtypes_header(s);

    if (!s->type)
        return;

    fprintf(headerfile, "/*\n");
    fprintf(headerfile, "%s ::= ", s->name);
    define_asn1 (0, s->type);
    fprintf(headerfile, "\n*/\n\n");

    if (s->emitted_definition)
        return;

    fprintf(headerfile, "typedef ");
    define_type(0, s->gen_name, s->gen_name, s->type, TRUE,
                preserve_type(s->name) ? TRUE : FALSE);

    fprintf(headerfile, "\n");

    if (template_flag)
        generate_template_type_forward(s->gen_name);

    emitted_definition(s);
}

void
generate_type_header_forwards(const Symbol *s)
{
    declare_type(s, s->type, TRUE);
    fprintf(headerfile, "\n");
}

void
generate_type (const Symbol *s)
{
    FILE *h;
    const char * exp;

    if (!one_code_file)
	generate_header_of_codefile(s->gen_name);

    generate_type_header (s);

    if (template_flag)
	generate_template(s);

    if (template_flag == 0 || is_template_compat(s) == 0) {
	generate_type_encode (s);
	generate_type_decode (s);
	generate_type_free (s);
	generate_type_length (s);
	generate_type_copy (s);
    }
    generate_type_seq (s);
    generate_glue (s->type, s->gen_name);

    /* generate prototypes */

    if (is_export(s->name)) {
	h = headerfile;
	exp = "ASN1EXP ";
    } else {
	h = privheaderfile;
	exp = "";
    }

    fprintf (h,
	     "%sint    ASN1CALL "
	     "decode_%s(const unsigned char *, size_t, %s *, size_t *);\n",
	     exp,
	     s->gen_name, s->gen_name);
    fprintf (h,
	     "%sint    ASN1CALL "
	     "encode_%s(unsigned char *, size_t, const %s *, size_t *);\n",
	     exp,
	     s->gen_name, s->gen_name);
    fprintf (h,
	     "%ssize_t ASN1CALL length_%s(const %s *);\n",
	     exp,
	     s->gen_name, s->gen_name);
    fprintf (h,
	     "%sint    ASN1CALL copy_%s  (const %s *, %s *);\n",
	     exp,
	     s->gen_name, s->gen_name, s->gen_name);
    fprintf (h,
	     "%svoid   ASN1CALL free_%s  (%s *);\n",
	     exp,
	     s->gen_name, s->gen_name);

    fprintf(h, "\n\n");

    if (!one_code_file) {
	fprintf(codefile, "\n\n");
	close_codefile();
	}
}
