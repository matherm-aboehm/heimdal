# $Id$

include $(top_srcdir)/Makefile.am.common

YFLAGS = -d -t

AM_CPPFLAGS += $(ROKEN_RENAME)

lib_LTLIBRARIES = libasn1.la
libasn1_la_LDFLAGS = -version-info 8:0:0

noinst_LTLIBRARIES = libasn1base.la

if versionscript
libasn1_la_LDFLAGS += $(LDFLAGS_VERSION_SCRIPT)$(srcdir)/version-script.map
endif


libasn1_la_LIBADD = \
	libasn1base.la \
	@LIB_com_err@ \
	$(LIBADD_roken)

BUILT_SOURCES =				\
	$(gen_files_rfc2459:.x=.c)	\
	$(gen_files_rfc4043:.x=.c)	\
	$(gen_files_rfc4108:.x=.c)	\
	$(gen_files_tcg:.x=.c)	        \
	$(gen_files_cms:.x=.c)		\
	$(gen_files_krb5:.x=.c)		\
	$(gen_files_ocsp:.x=.c)	        \
	$(gen_files_pkinit:.x=.c)	\
	$(gen_files_pkcs8:.x=.c)	\
	$(gen_files_pkcs9:.x=.c)	\
	$(gen_files_pkcs10:.x=.c)	\
	$(gen_files_pkcs12:.x=.c)	\
	$(gen_files_digest:.x=.c)	\
	$(gen_files_kx509:.x=.c)	\
	$(gen_files_x690sample:.x=.c)

gen_files_krb5 = asn1_krb5_asn1.x
gen_files_cms = asn1_cms_asn1.x
gen_files_crmf = asn1_crmf_asn1.x
gen_files_rfc2459 = asn1_rfc2459_asn1.x
gen_files_rfc4043 = asn1_rfc4043_asn1.x
gen_files_rfc4108 = asn1_rfc4108_asn1.x
gen_files_tcg = asn1_tcg_asn1.x
gen_files_ocsp = asn1_ocsp_asn1.x
gen_files_pkinit = asn1_pkinit_asn1.x
gen_files_pkcs10 = asn1_pkcs10_asn1.x
gen_files_pkcs12 = asn1_pkcs12_asn1.x
gen_files_pkcs8 = asn1_pkcs8_asn1.x
gen_files_pkcs9 = asn1_pkcs9_asn1.x
gen_files_test_template = asn1_test_template_asn1.x
gen_files_test = asn1_test_asn1.x
gen_files_digest = asn1_digest_asn1.x
gen_files_kx509 = asn1_kx509_asn1.x
gen_files_x690sample = asn1_x690sample_asn1.x

oid_resolution.lo: $(BUILT_SOURCES)

noinst_PROGRAMS = asn1_gen

libexec_heimdal_PROGRAMS = asn1_compile asn1_print

TESTS = check-der check-gen check-gen-template check-timegm check-ber check-template
check_PROGRAMS = $(TESTS)

asn1_gen_SOURCES = asn1_gen.c
asn1_print_SOURCES = asn1_print.c
check_der_SOURCES = check-der.c check-common.c check-common.h

check_template_SOURCES = check-template.c check-common.c check-common.h
nodist_check_template_SOURCES = $(gen_files_test_template:.x=.c)

dist_check_gen_template_SOURCES = check-gen.c check-common.c check-common.h
nodist_check_gen_template_SOURCES = $(gen_files_test_template:.x=.c)

dist_check_gen_SOURCES = check-gen.c check-common.c check-common.h
nodist_check_gen_SOURCES = $(gen_files_test:.x=.c)

build_HEADERZ = asn1-template.h

asn1_compile_SOURCES = 				\
	asn1parse.y				\
	der.h					\
	gen.c					\
	gen_copy.c				\
	gen_decode.c				\
	gen_encode.c				\
	gen_free.c				\
	gen_glue.c				\
	gen_length.c				\
	gen_locl.h				\
	gen_seq.c				\
	gen_template.c				\
	hash.c					\
	hash.h					\
	lex.l					\
	lex.h					\
	main.c					\
	asn1-template.h				\
	symbol.c				\
	symbol.h

dist_libasn1base_la_SOURCES =			\
	der_locl.h 				\
	der.c					\
	der.h					\
	der_get.c				\
	der_put.c				\
	der_free.c				\
	der_length.c				\
	der_copy.c				\
	der_cmp.c				\
	der_format.c				\
	fuzzer.c				\
	heim_asn1.h				\
	extra.c					\
	roken_rename.h				\
	template.c				\
	timegm.c

dist_libasn1_la_SOURCES = oid_resolution.c

nodist_libasn1base_la_SOURCES = 	\
	asn1_err.h			\
	asn1_err.c

nodist_libasn1_la_SOURCES = $(BUILT_SOURCES)

asn1_compile_LDADD = \
	$(LIB_roken) $(LEXLIB)

check_der_LDADD = \
	libasn1base.la \
	$(LIB_roken)

check_template_LDADD = $(check_der_LDADD)
asn1_print_LDADD = libasn1.la $(LIB_roken) $(LIB_com_err)
asn1_gen_LDADD = $(check_der_LDADD)
check_timegm_LDADD = $(check_der_LDADD)

check_gen_template_LDADD = \
	libasn1.la \
	$(LIB_roken)

check_gen_LDADD = \
	libasn1.la \
	$(LIB_roken)

check_ber_LDADD = $(check_gen_LDADD)

CLEANFILES = \
	$(BUILT_SOURCES) \
	$(gen_files_rfc2459) \
	$(gen_files_rfc4043) \
	$(gen_files_rfc4108) \
	$(gen_files_tcg) \
	$(gen_files_cms) \
	$(gen_files_krb5) \
	$(gen_files_ocsp) \
	$(gen_files_pkinit) \
	$(gen_files_pkcs8) \
	$(gen_files_pkcs9) \
	$(gen_files_pkcs10) \
	$(gen_files_pkcs12) \
	$(gen_files_digest) \
	$(gen_files_kx509) \
	$(gen_files_x690sample) \
	$(gen_files_test) \
	$(gen_files_test_template) \
	$(nodist_check_gen_SOURCES) \
	asn1parse.c asn1parse.h lex.c \
	asn1_err.c asn1_err.h \
	rfc2459_asn1_files rfc2459_asn1*.h* rfc2459_asn1*.x \
	rfc4043_asn1_files rfc4043_asn1*.h* rfc4043_asn1*.x \
	rfc4108_asn1_files rfc4108_asn1*.h* rfc4108_asn1*.x \
	tcg_asn1_files tcg_asn1*.h* tcg_asn1*.x \
	cms_asn1_files cms_asn1*.h* cms_asn1*.x \
	crmf_asn1_files crmf_asn1*.h* crmf_asn1*.x \
	krb5_asn1_files krb5_asn1*.h* krb5_asn1*.x \
	ocsp_asn1_files ocsp_asn1*.h* ocsp_asn1*.x \
	pkinit_asn1_files pkinit_asn1*.h* pkinit_asn1*.x \
	pkcs8_asn1_files pkcs8_asn1*.h* pkcs8_asn1*.x* \
	pkcs9_asn1_files pkcs9_asn1*.h* pkcs9_asn1*.x \
	pkcs10_asn1_files pkcs10_asn1*.h* pkcs10_asn1*.x \
	pkcs12_asn1_files pkcs12_asn1*.h* pkcs12_asn1*.x \
	digest_asn1_files digest_asn1*.h* digest_asn1*.x \
	kx509_asn1_files kx509_asn1*.h* kx509_asn1*.x \
	x690sample_asn1_files x690sample_asn1*.h* x690sample_asn1*.x \
	test_asn1_files test_asn1*.h* test_asn1*.x \
	test_template_asn1* \
	asn1_*.tmp.c asn1_*.x

dist_include_HEADERS = der.h heim_asn1.h
dist_include_HEADERS += $(srcdir)/der-protos.h $(srcdir)/der-private.h
dist_include_HEADERS += asn1-common.h

nodist_include_HEADERS = asn1_err.h
nodist_include_HEADERS += krb5_asn1.h
nodist_include_HEADERS += pkinit_asn1.h
nodist_include_HEADERS += cms_asn1.h
nodist_include_HEADERS += crmf_asn1.h
nodist_include_HEADERS += rfc2459_asn1.h
nodist_include_HEADERS += rfc4043_asn1.h
nodist_include_HEADERS += rfc4108_asn1.h
nodist_include_HEADERS += tcg_asn1.h
nodist_include_HEADERS += ocsp_asn1.h
nodist_include_HEADERS += pkcs8_asn1.h
nodist_include_HEADERS += pkcs9_asn1.h
nodist_include_HEADERS += pkcs10_asn1.h
nodist_include_HEADERS += pkcs12_asn1.h
nodist_include_HEADERS += digest_asn1.h
nodist_include_HEADERS += kx509_asn1.h
nodist_include_HEADERS += x690sample_asn1.h

priv_headers  = krb5_asn1-priv.h
priv_headers += pkinit_asn1-priv.h
priv_headers += cms_asn1-priv.h
priv_headers += crmf_asn1-priv.h
priv_headers += rfc2459_asn1-priv.h
priv_headers += rfc4043_asn1-priv.h
priv_headers += rfc4108_asn1-priv.h
priv_headers += tcg_asn1-priv.h
priv_headers += ocsp_asn1-priv.h
priv_headers += pkcs8_asn1-priv.h
priv_headers += pkcs9_asn1-priv.h
priv_headers += pkcs10_asn1-priv.h
priv_headers += pkcs12_asn1-priv.h
priv_headers += digest_asn1-priv.h
priv_headers += kx509_asn1-priv.h
priv_headers += x690sample_asn1-priv.h
priv_headers += test_template_asn1.h test_template_asn1-priv.h
priv_headers += test_asn1.h test_asn1-priv.h



$(asn1_compile_OBJECTS): asn1parse.h asn1parse.c $(srcdir)/der-protos.h $(srcdir)/der-private.h
$(libasn1_la_OBJECTS): $(nodist_include_HEADERS) $(priv_headers) asn1_err.h $(srcdir)/der-protos.h $(srcdir)/der-private.h
$(libasn1base_la_OBJECTS): asn1_err.h $(srcdir)/der-protos.h $(srcdir)/der-private.h
$(check_gen_OBJECTS): test_asn1.h
$(check_template_OBJECTS): test_asn1_files
$(asn1_print_OBJECTS): krb5_asn1.h

asn1parse.h: asn1parse.c

$(gen_files_krb5) krb5_asn1.hx krb5_asn1-priv.hx: krb5_asn1_files
$(gen_files_ocsp) ocsp_asn1.hx ocsp_asn1-priv.hx: ocsp_asn1_files
$(gen_files_pkinit) pkinit_asn1.hx pkinit_asn1-priv.hx: pkinit_asn1_files
$(gen_files_pkcs8) pkcs8_asn1.hx pkcs8_asn1-priv.hx: pkcs8_asn1_files
$(gen_files_pkcs9) pkcs9_asn1.hx pkcs9_asn1-priv.hx: pkcs9_asn1_files
$(gen_files_pkcs10) pkcs10_asn1.hx pkcs10_asn1-priv.hx: pkcs10_asn1_files
$(gen_files_pkcs12) pkcs12_asn1.hx pkcs12_asn1-priv.hx: pkcs12_asn1_files
$(gen_files_digest) digest_asn1.hx digest_asn1-priv.hx: digest_asn1_files
$(gen_files_kx509) kx509_asn1.hx kx509_asn1-priv.hx: kx509_asn1_files
$(gen_files_rfc2459) rfc2459_asn1.hx rfc2459_asn1-priv.hx: rfc2459_asn1_files
$(gen_files_rfc4043) rfc4043_asn1.hx rfc4043_asn1-priv.hx: rfc4043_asn1_files
$(gen_files_rfc4108) rfc4108_asn1.hx rfc4108_asn1-priv.hx: rfc4108_asn1_files
$(gen_files_tcg) tcg_asn1.hx tcg_asn1-priv.hx: tcg_asn1_files
$(gen_files_cms) cms_asn1.hx cms_asn1-priv.hx: cms_asn1_files
$(gen_files_crmf) crmf_asn1.hx crmf_asn1-priv.hx: crmf_asn1_files
$(gen_files_x690sample) x690sample_asn1.hx x690sample_asn1-priv.hx: x690sample_asn1_files
$(gen_files_test) test_asn1.hx test_asn1-priv.hx: test_asn1_files
$(gen_files_test_template) test_template_asn1.hx test_template_asn1-priv.hx: test_template_asn1_files

if ASN1_TEMPLATING
TEMPLATE_OPTION=--template
else
TEMPLATE_OPTION=
endif

# XXX Currently using the template compiler for rfc2459.asn1 breaks
rfc2459_asn1_files: asn1_compile$(EXEEXT) $(srcdir)/rfc2459.asn1
	$(ASN1_COMPILE) --one-code-file $(TEMPLATE_OPTION) --option-file=$(srcdir)/rfc2459.opt $(srcdir)/rfc2459.asn1 rfc2459_asn1 || (rm -f rfc2459_asn1_files ; exit 1)

rfc4043_asn1_files: asn1_compile$(EXEEXT) $(srcdir)/rfc4043.asn1
	$(ASN1_COMPILE) --one-code-file $(TEMPLATE_OPTION) $(srcdir)/rfc4043.asn1 rfc4043_asn1 || (rm -f rfc4043_asn1_files ; exit 1)

rfc4108_asn1_files: asn1_compile$(EXEEXT) $(srcdir)/rfc4108.asn1
	$(ASN1_COMPILE) --one-code-file $(TEMPLATE_OPTION) $(srcdir)/rfc4108.asn1 rfc4108_asn1 || (rm -f rfc4108_asn1_files ; exit 1)

tcg_asn1_files: asn1_compile$(EXEEXT) $(srcdir)/tcg.asn1
	$(ASN1_COMPILE) --prefix-enum --one-code-file $(TEMPLATE_OPTION) $(srcdir)/tcg.asn1 tcg_asn1 || (rm -f tcg_asn1_files ; exit 1)

cms_asn1_files: asn1_compile$(EXEEXT) $(srcdir)/cms.asn1 $(srcdir)/cms.opt
	$(ASN1_COMPILE) --one-code-file $(TEMPLATE_OPTION) --option-file=$(srcdir)/cms.opt $(srcdir)/cms.asn1 cms_asn1 || (rm -f cms_asn1_files ; exit 1)

crmf_asn1_files: asn1_compile$(EXEEXT) $(srcdir)/crmf.asn1 $(srcdir)/crmf.opt
	$(ASN1_COMPILE) --one-code-file $(TEMPLATE_OPTION) --option-file=$(srcdir)/crmf.opt $(srcdir)/crmf.asn1 crmf_asn1 || (rm -f crmf_asn1_files ; exit 1)

krb5_asn1_files: asn1_compile$(EXEEXT) $(srcdir)/krb5.asn1 $(srcdir)/krb5.opt
	$(ASN1_COMPILE) --one-code-file $(TEMPLATE_OPTION) --option-file=$(srcdir)/krb5.opt $(srcdir)/krb5.asn1 krb5_asn1 || (rm -f krb5_asn1_files ; exit 1)

ocsp_asn1_files: asn1_compile$(EXEEXT) $(srcdir)/ocsp.asn1
	$(ASN1_COMPILE) --one-code-file $(TEMPLATE_OPTION) --option-file=$(srcdir)/ocsp.opt $(srcdir)/ocsp.asn1 ocsp_asn1 || (rm -f ocsp_asn1_files ; exit 1)

pkinit_asn1_files: asn1_compile$(EXEEXT) $(srcdir)/pkinit.asn1
	$(ASN1_COMPILE) --one-code-file $(TEMPLATE_OPTION) $(srcdir)/pkinit.asn1 pkinit_asn1 || (rm -f pkinit_asn1_files ; exit 1)

pkcs8_asn1_files: asn1_compile$(EXEEXT) $(srcdir)/pkcs8.asn1
	$(ASN1_COMPILE) --one-code-file $(TEMPLATE_OPTION) $(srcdir)/pkcs8.asn1 pkcs8_asn1 || (rm -f pkcs8_asn1_files ; exit 1)

pkcs9_asn1_files: asn1_compile$(EXEEXT) $(srcdir)/pkcs9.asn1
	$(ASN1_COMPILE) --one-code-file $(TEMPLATE_OPTION) $(srcdir)/pkcs9.asn1 pkcs9_asn1 || (rm -f pkcs9_asn1_files ; exit 1)

pkcs10_asn1_files: asn1_compile$(EXEEXT) $(srcdir)/pkcs10.asn1
	$(ASN1_COMPILE) --one-code-file $(TEMPLATE_OPTION) --option-file=$(srcdir)/pkcs10.opt $(srcdir)/pkcs10.asn1 pkcs10_asn1 || (rm -f pkcs10_asn1_files ; exit 1)

pkcs12_asn1_files: asn1_compile$(EXEEXT) $(srcdir)/pkcs12.asn1
	$(ASN1_COMPILE) --one-code-file $(TEMPLATE_OPTION) $(srcdir)/pkcs12.asn1 pkcs12_asn1 || (rm -f pkcs12_asn1_files ; exit 1)

digest_asn1_files: asn1_compile$(EXEEXT) $(srcdir)/digest.asn1
	$(ASN1_COMPILE) --one-code-file $(TEMPLATE_OPTION) $(srcdir)/digest.asn1 digest_asn1 || (rm -f digest_asn1_files ; exit 1)

kx509_asn1_files: asn1_compile$(EXEEXT) $(srcdir)/kx509.asn1
	$(ASN1_COMPILE) --one-code-file $(TEMPLATE_OPTION) $(srcdir)/kx509.asn1 kx509_asn1 || (rm -f kx509_asn1_files ; exit 1)

x690sample_asn1_files: asn1_compile$(EXEEXT) $(srcdir)/x690sample.asn1
	$(ASN1_COMPILE) --one-code-file $(TEMPLATE_OPTION) $(srcdir)/x690sample.asn1 x690sample_asn1 || (rm -f x690sample_asn1_files ; exit 1)

test_template_asn1_files: asn1_compile$(EXEEXT) $(srcdir)/test.asn1
	$(ASN1_COMPILE) --one-code-file --template --sequence=TESTSeqOf $(srcdir)/test.asn1 test_template_asn1 || (rm -f test_template_asn1_files ; exit 1)

test_asn1_files: asn1_compile$(EXEEXT) $(srcdir)/test.asn1
	$(ASN1_COMPILE) --one-code-file --sequence=TESTSeqOf $(srcdir)/test.asn1 test_asn1 || (rm -f test_asn1_files ; exit 1)


EXTRA_DIST =		\
	NTMakefile	\
	README.template \
	asn1_compile-version.rc \
	libasn1-exports.def \
	cms.asn1	\
	cms.opt		\
	crmf.asn1	\
	crmf.opt	\
	asn1_err.et	\
	canthandle.asn1	\
	digest.asn1	\
	krb5.asn1	\
	krb5.opt	\
	kx509.asn1	\
	ocsp.asn1	\
	pkcs12.asn1	\
	pkcs8.asn1	\
	pkcs9.asn1	\
	pkcs10.asn1	\
	pkinit.asn1	\
	rfc2459.asn1	\
	rfc4043.asn1	\
	rfc4108.asn1	\
	tcg.asn1	\
	setchgpw2.asn1	\
	x690sample.asn1	\
	test.asn1	\
	test.gen	\
	version-script.map

DER_PROTOS = $(srcdir)/der-protos.h $(srcdir)/der-private.h

ALL_OBJECTS  = $(libasn1_la_OBJECTS)
ALL_OBJECTS += $(libasn1base_la_OBJECTS)
ALL_OBJECTS += $(asn1_print_OBJECTS)
ALL_OBJECTS += $(asn1_compile_OBJECTS)
ALL_OBJECTS += $(asn1_gen_OBJECTS)
ALL_OBJECTS += $(check_template_OBJECTS)

$(ALL_OBJECTS): $(DER_PROTOS) asn1_err.h

$(srcdir)/der-protos.h: $(dist_libasn1base_la_SOURCES) $(dist_libasn1_la_SOURCES)
	cd $(srcdir) && perl ../../cf/make-proto.pl -q -P comment -o der-protos.h $(dist_libasn1base_la_SOURCES) $(dist_libasn1_la_SOURCES) || rm -f der-protos.h

$(srcdir)/der-private.h: $(dist_libasn1base_la_SOURCES) $(dist_libasn1_la_SOURCES)
	cd $(srcdir) && perl ../../cf/make-proto.pl -q -P comment -p der-private.h $(dist_libasn1base_la_SOURCES) $(dist_libasn1_la_SOURCES) || rm -f der-private.h
