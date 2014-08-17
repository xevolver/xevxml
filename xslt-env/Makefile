


##############################################
# user input
INPUT_DIR	= xev-input-f90
EXP_F90_DIR	= expected-f90
XSLT_DIR	= xev-translate


##############################################
# set env

ifeq ($(shell dnsdomainname),aq.sc.isc.tohoku.ac.jp)
XEVXML_ENV = . /shome/hirasawa/local/share/env-xevxml-aq.sh
else ifeq ($(HOSTNAME), hagi-a)
#XEVXML_ENV = . /opt/xevxml/xevxml-current/env-xevxml-hagi.sh
XEVXML_ENV = . /opt/xevxml/xevxml-20140531/env-xevxml-hagi.sh
#XEVXML_ENV = . /opt/xevxml/xevxml-prev/env-xevxml-hagi.sh
#XEVXML_ENV = . /opt/xevxml/xevxml-exp/env-xevxml-hagi.sh
endif

##############################################
# xev generated
SRC2XML_DIR = xev-src2xml
DIR2XML_DIR = xev-dir2xml
XML2SRC_DIR = xev-xml2src
EXP_XML_DIR = expected-xml2src

##############################################
# source and target files

SRCS		= ${wildcard $(INPUT_DIR)/*.f90}
SRC2XMLS	= ${SRCS:$(INPUT_DIR)/%.f90=$(SRC2XML_DIR)/%.xml}
DIR2XMLS	= ${SRCS:$(INPUT_DIR)/%.f90=$(DIR2XML_DIR)/%.xml}
TRANS_XMLS	= ${SRCS:$(INPUT_DIR)/%.f90=$(XML2SRC_DIR)/%.xml}
XML2SRCS	= ${SRCS:$(INPUT_DIR)/%.f90=$(XML2SRC_DIR)/%.f90}

EXP_XMLS	= ${SRCS:$(INPUT_DIR)/%.f90=$(EXP_XML_DIR)/%.xml}
EXP_SRCS	= ${SRCS:$(INPUT_DIR)/%.f90=$(EXP_XML_DIR)/%.f90}

XML_DIFFS	= ${SRCS:$(INPUT_DIR)/%.f90=$(XML2SRC_DIR)/%.xmldiff}

.PRECIOUS: $(SRC2XMLS) $(DIR2XMLS) $(TRANS_XMLS) $(XML2SRCS) $(EXP_XMLS) $(EXP_SRCS) $(XML_DIFFS)

##############################################
# targets

all:
	@echo "usage: make {var-replace-simple, *}.check}"
	@echo "xevxml-env: $(XEVXML_ENV)"
	@hostname -a

src2xml-files: $(SRC2XMLS)
	@echo "TARGET: $(SRC2XMLS)"

dir2xml-files: $(DIR2XMLS)
	@echo "TARGET: $(SRC2XMLS)"

xml2src-files: $(XML2SRCS)
	@echo "TARGET: $(XML2SRCS)"

expected-f90-files: $(EXP_SRCS)

##############################################
# rules

#.SUFFIXES: .f90 .xml .check .xmldiff


# test translation
#%.check: $(XML2SRC_DIR)/%.xmldiff $(EXP_XML_DIR)/%.f90 $(XML2SRC_DIR)/%.f90
%.check: $(EXP_XML_DIR)/%.f90 $(XML2SRC_DIR)/%.f90
	diff $(EXP_XML_DIR)/$*.f90 $(XML2SRC_DIR)/$*.f90

# src2xml translation
$(SRC2XML_DIR)/%.xml: $(INPUT_DIR)/%.f90
	$(XEVXML_ENV) ; src2xml $< > $@

# dir2xml translation
$(DIR2XML_DIR)/%.xml: $(SRC2XML_DIR)/%.xml ./dir-defs.xml
	$(XEVXML_ENV) ; dir2xml ./dir-defs.xml < $< > $@

$(XML2SRC_DIR)/%.xmldiff: $(XML2SRC_DIR)/%.xml $(EXP_XML_DIR)/%.xml
	diff $(XML2SRC_DIR)/$*.xml $(EXP_XML_DIR)/$*.xml > $@ 

# translation
$(XML2SRC_DIR)/%.xml: $(DIR2XML_DIR)/%.xml $(XSLT_DIR)/%.xsl
	$(XEVXML_ENV) ; xsltrans $(XSLT_DIR)/$*.xsl < $< > $@

# xml2src translation
$(XML2SRC_DIR)/%.f90: $(XML2SRC_DIR)/%.xml
	$(XEVXML_ENV) ; xml2src < $< $@

# expected .xml file 
$(EXP_XML_DIR)/%.xml: $(EXP_F90_DIR)/%.f90
	$(XEVXML_ENV) ; src2xml $< > $@

# expected .f90 source file 
$(EXP_XML_DIR)/%.f90: $(EXP_XML_DIR)/%.xml
	$(XEVXML_ENV) ; xml2src < $< $@

##############################################
# other tests

command-test:
	$(XEVXML_ENV) ; which xsltrans
#	$(XEVXML_ENV) ; xml2src simple-dir-add-output.f < simple-dir-add.xml
#	$(XEVXML_ENV) ; src2xml simple-dir-add.f | dir2xml ../xslt/defs.xml | $(DIR2XML_DIR)/tmp-output.xml

test-dir-add-trans:
	xsltproc ../xslt/example-echo.xsl simple-dir-add.xml

test-dir-replace-trans:
	xsltproc ../xslt/example-echo.xsl simple-dir-replace.xml

test-var-replace-trans:
	xsltproc ../xslt/example-echo.xsl simple-var-replace.xml

test-include-plain-code:
	which xsltrans
	$(XEVXML_ENV) ; \
	xsltproc ../xslt/example-include-plain-code.xsl $(SRC2XML_DIR)/simple-dir-add.xml \
	| xml2src \
	> ./xev-xml2src/test-include-plain-code.f

#	xsltrans ../xslt/example-include-plain-code.xsl $(SRC2XML_DIR)/simple-dir-add.xml \

test-template:
	#$(XEVXML_ENV) ; xml2src $(SRC2XML_DIR)/test-template.xml
	#$(XEVXML_ENV) ; xml2src $(SRC2XML_DIR)/simple-dir-replace.xml
	$(XEVXML_ENV) ; xml2src $(SRC2XML_DIR)/simple-var-replace.xml

##############################################
sync:
	@sync
	@echo "sync sync sync"

clean-translated:
	-rm -f $(XML2SRC_DIR)/*.xml $(XML2SRC_DIR)/*.f90 $(XML2SRC_DIR)/*.xmldiff

clean-generated: clean-translated
	-rm -f $(SRC2XML_DIR)/*.xml $(DIR2XML_DIR)/*.xml
	-rm -f $(EXP_XML_DIR)/*.xml $(EXP_XML_DIR)/*.f90
