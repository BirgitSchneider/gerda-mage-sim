# Makefile
#
# Author: Luigi Pertoldi - luigi.pertoldi@pd.infn.it
# Created: 21/01/2017
#
# USAGE:
#  - make sim : launch the simulations in the cluster
#  - make bin : compile the command line tools
#  - make post : lauch post-processing in the cluster
#
# NOTES:
#  - use the --dry-run flag to see what would be done without
#    actually executing it (useful also in conjunction with --debug)
#
# *CHANGE ME* important variables
SRCDIR  = /lfs/l2/gerda/gerda-simulations/gerda-mage-sim
DESTDIR = /lfs/l2/gerda/gerda-simulations/gerda-pdfs/v0.2
QSUB    = $(shell which qsub)

all :
	@echo "Please pick up one of the following options:"
	@echo " - make sim : launch the simulations in the cluster"
	@echo " - make bin : compile the command line tools"
	@echo " - make post : lauch post-processing in the cluster"

### Post-processing #####################################################################

# qsub script must accept the syntax:
# qsub job-scheduler/mpik-t4z-gen.qsub <folder-with-raw> <dest-dir-tree>
# qsub job-scheduler/mpik-pdf-gen.qsub <volume>/<part>/<isotope> <dest-dir-tree>
# qsub job-scheduler/mpik-pdf-gen-volume.qsub <volume> <dest-dir-tree>
JOBT4Z     = job-scheduler/mpik-t4z-gen.qsub
JOBPDF     = job-scheduler/mpik-pdf-gen.qsub
JOBPDFVOL  = job-scheduler/mpik-pdf-gen-volume.qsub

# function to change the position of the directory tree
changetree = $(shell echo $(2)/$(shell echo $(1) | rev | cut -d'/' -f-4 | rev))
changetree2 = $(shell echo $(2)/$(shell echo $(1) | rev | cut -d'/' -f-5 | rev))

rm1 = $(shell echo $(1) | cut -d'/' -f2-)

# info extraction
volume  = $(shell echo $(1) | cut -d'/' -f2)
part    = $(shell echo $(1) | cut -d'/' -f3)
isotope = $(shell echo $(1) | cut -d'/' -f4)
deptype = $(shell echo $(1) | cut -d'/' -f5)

# get all directories to be processed with raw- files
# 'sort' will remove duplicates
ISOTOPES = $(sort $(foreach d,$(wildcard ../*/*/*/edep ../*/*/*/coin),$(shell dirname $(d))))
ALL_PDF_FILES = $(foreach i,$(ISOTOPES),$(call changetree,$(i)/pdf-$(call volume,$(i))-$(call part,$(i))-$(call isotope,$(i)).root,$(DESTDIR)))

CABLES_ISOTOPES = $(sort $(foreach d,$(wildcard ../cables/*/*/edep ../cables/*/*/coin),$(shell dirname $(d))))
CABLES_ALL_PDF_FILES = $(foreach i,$(CABLES_ISOTOPES),$(call changetree,$(i)/pdf-$(call volume,$(i))-$(call part,$(i))-$(call isotope,$(i)).root,$(DESTDIR)))
CABLES_PDF_FILES = $(sort $(foreach i,$(CABLES_ISOTOPES),$(DESTDIR)/cables/pdf-cables-$(call isotope,$(i)).root))

MINIS_ISOTOPES = $(sort $(foreach d,$(wildcard ../minishroud/*/*/edep ../minishroud/*/*/coin),$(shell dirname $(d))))
MINIS_ALL_PDF_FILES = $(foreach i,$(MINIS_ISOTOPES),$(call changetree,$(i)/pdf-$(call volume,$(i))-$(call part,$(i))-$(call isotope,$(i)).root,$(DESTDIR)))
MINIS_PDF_FILES = $(sort $(foreach i,$(MINIS_ISOTOPES),$(DESTDIR)/minishroud/pdf-minishroud-$(call isotope,$(i)).root))

GEDET_ISOTOPES = $(sort $(foreach d,$(wildcard ../gedet/*/*/edep ../gedet/*/*/coin),$(shell dirname $(d))))
GEDET_ALL_PDF_FILES = $(foreach i,$(GEDET_ISOTOPES),$(call changetree,$(i)/pdf-$(call volume,$(i))-$(call part,$(i))-$(call isotope,$(i)).root,$(DESTDIR)))

GE_HOLDERS_ISOTOPES = $(sort $(foreach d,$(wildcard ../ge_holders/*/*/edep ../ge_holders/*/*/coin),$(shell dirname $(d))))
GE_HOLDERS_ALL_PDF_FILES = $(foreach i,$(GE_HOLDERS_ISOTOPES),$(call changetree,$(i)/pdf-$(call volume,$(i))-$(call part,$(i))-$(call isotope,$(i)).root,$(DESTDIR)))

LAR_ISOTOPES = $(sort $(foreach d,$(wildcard ../lar/*/*/edep ../lar/*/*/coin),$(shell dirname $(d))))
LAR_ALL_PDF_FILES = $(foreach i,$(LAR_ISOTOPES),$(call changetree,$(i)/pdf-$(call volume,$(i))-$(call part,$(i))-$(call isotope,$(i)).root,$(DESTDIR)))

LARVETO_ISOTOPES = $(sort $(foreach d,$(wildcard ../larveto/*/*/edep ../larveto/*/*/coin),$(shell dirname $(d))))
LARVETO_ALL_PDF_FILES = $(foreach i,$(LARVETO_ISOTOPES),$(call changetree,$(i)/pdf-$(call volume,$(i))-$(call part,$(i))-$(call isotope,$(i)).root,$(DESTDIR)))

get_pdf_files_for_iso = $(foreach d,$(wildcard ../$(1)/*/$(2)),$(call changetree,$(d)/pdf-$(1)-$(call part,$(d))-$(2).root,$(DESTDIR)))

# function to check if a job is already in the qsub queue
getjob = $(shell if [[ `qstat -r | grep $(1) | grep "Full jobname:"` ]]; then echo 1; else echo 0; fi)

post : mkdir $(CABLES_PDF_FILES) $(MINIS_PDF_FILES) $(GEDET_ALL_PDF_FILES) $(GE_HOLDERS_ALL_PDF_FILES) $(LAR_ALL_PDF_FILES) $(LARVETO_ALL_PDF_FILES)

mkdir :
	@mkdir -p $(DESTDIR)

space := $(null) #
comma := ,
define rule_for_volume_pdf
  $(1) : $$(call get_pdf_files_for_iso,$(2),$$(shell echo $$(basename $(1)) | rev | cut -d'-' -f1 | rev))
	@if [[ $$(call getjob,$$(notdir $$@)) == 0 ]]; then \
	$$(QSUB) -hold_jid $$(subst $$(space),$$(comma),$$(notdir $$(call get_pdf_files_for_iso,$(2),$$(shell echo $$(basename $(1)) | rev | cut -d'-' -f1 | rev)))) \
                   -N $$(notdir $$@) \
                   $$(JOBPDFVOL) $$(shell echo $$@ | rev | cut -d'-' -f3 | rev) \
                   $$(DESTDIR); \
	else \
	echo "There's already a job for $$(notdir $$@) in queue"; \
	fi
endef
$(foreach t,$(CABLES_PDF_FILES),$(eval $(call rule_for_volume_pdf,$(t),cables)))
$(foreach t,$(MINIS_PDF_FILES),$(eval $(call rule_for_volume_pdf,$(t),minishroud)))

# *CHANGE ME*: array with runIDs
RUNS = $(shell seq 53 65) 67 $(shell seq 69 89)
# function to build the t4ized file name
# ARG1: directory with raw files
# ARG2: runID
t4zfile = t4z-$(call volume,$(1))-$(call part,$(1))-$(call isotope,$(1))-$(call deptype,$(1))-run$(2).root
# function to get the list of t4z- files that will be created
# ARG1: directory with raw files
out4zfiles    = $(foreach id,$(RUNS),$(call changetree2,$(1)/$(call t4zfile,$(1),$(id)),$(DESTDIR)))
ALLDIRWITHRAW = $(wildcard ../*/*/*/edep ../*/*/*/coin)
ALL_T4Z_FILES = $(foreach d,$(ALLDIRWITHRAW),$(call out4zfiles,$(d)))

# make each single pdf- file depend on t4z- ones
define rule_for_single_pdf
  $(1) : $$(foreach d,$$(wildcard $$(call changetree,$$(dir $(1)),..)edep $$(call changetree,$$(dir $(1)),..)coin),$$(call out4zfiles,$$(d)))
	@if [[ $$(call getjob,$$(notdir $$@)) == 0 ]]; then \
	$$(QSUB) -hold_jid t4z-$$(shell echo $$(notdir $$(basename $$@)) | cut -d'-' -f2-)-edep,t4z-$$(shell echo $$(notdir $$(basename $$@)) | cut -d'-' -f2-)-coin \
                   -N $$(notdir $$@) \
                   $$(JOBPDF) $$(shell echo $$@ | rev | cut -d'-' -f3 | rev)/$$(shell echo $$@ | rev | cut -d'-' -f2 | rev)/$$(shell echo $$(basename $$@) | rev | cut -d'-' -f1 | rev) \
                   $$(DESTDIR); \
	else \
	echo "There's already a job for $$(notdir $$@) in queue"; \
	fi
endef
$(foreach t,$(ALL_PDF_FILES),$(eval $(call rule_for_single_pdf,$(t))))

define rule_for_single_t4z
  $(1) : $$(wildcard $$(dir $$(call changetree,$(1),$$(SRCDIR)))/raw-*.root)
	@if [[ $$(call getjob,$$(shell echo $$(notdir $$@) | cut -d'-' -f-5)) == 0 ]]; then \
	$$(QSUB) -N $$(shell echo $$(notdir $$@) | cut -d'-' -f-5) \
                    $$(JOBT4Z) $$(call changetree2,$$(dir $$@),$$(SRCDIR)) $$(DESTDIR); \
	else \
	echo "There's already a $$(shell echo $$(notdir $$@) | cut -d'-' -f-5) job in queue"; \
	fi
endef
$(foreach t,$(ALL_T4Z_FILES),$(eval $(call rule_for_single_t4z,$(t))))

.PHONY : post mkdir

### Run simulations ###########################################################################

# qsub script must accept the syntax:
# qsub job-scheduler/mpik-run-mage.qsub <macro-file>
JOBSIM = job-scheduler/mpik-run-mage.qsub

# find coin and edep directories
SIMDIRS   = $(wildcard ../*/*/*/edep ../*/*/*/coin)
# find macros
MACROS    = $(foreach dir,$(SIMDIRS),$(realpath $(wildcard $(dir)/log/raw-*.mac)))
# build up root file names
ROOTFILES = $(foreach mac,$(MACROS),$(subst .mac,.root,$(subst log/,,$(mac))))

sim : $(ROOTFILES)

# second expansion is needed here to build up macro file name from target
.SECONDEXPANSION:
$(ROOTFILES) : %.root : $$(dir $$*)log/$$(notdir $$*).mac
	@if [[ $(call getjob,$(notdir $@)) == 0 ]]; then \
	$(QSUB) -N $(notdir $@) $(JOBSIM) $^; \
	else \
	echo "There's already a $(notdir $@) job in queue"; \
	fi

# another (very slow) way to do it
#define run-sim-template
#  $(1) : $$(dir $(1))log/$$(subst .root,.mac,$$(notdir $(1)))
#  ifeq '$$(shell if [[ `qstat -r | grep "$$(notdir $(1))"` ]]; then echo 1; else echo 0; fi)' '0'
#	$$(QSUB) -N $$(notdir $(1)) $$(JOBSIM) $$^
#  else
#	@echo "There's already a job for $(1) in queue"
#  endif
#endef
#
#$(foreach f,$(ROOTFILES),$(eval $(call run-sim-template,$(f))))

.PHONY : sim

### Compile ####################################################################################

CC = c++ -std=c++1y
INSTALLDIR     = bin
ROOTFLAGS      = $(shell root-config --libs --cflags) -lTreePlayer
GERDA-ADAFLAGS = $(shell gerda-ada-config --libs --cflags)
GELATIOFLAGS   = $(shell gelatio-config --libs --cflags)
MGDOFLAGS      = $(shell mgdo-config --libs --cflags)
DATABRIXXFLAGS = $(shell databricxx-config --libs --cflags)
JSONCPPFLAGS   = -I jsoncpp
ALLFLAGS       = $(ROOTFLAGS) $(GERDA-ADAFLAGS) $(GELATIOFLAGS) $(MGDOFLAGS) $(DATABRIXXFLAGS) $(JSONCPPFLAGS)

bin : dirs $(foreach b,$(wildcard post/*.cxx),$(addprefix $(INSTALLDIR)/,$(notdir $(basename $(b)))))

dirs :
	@mkdir -p $(INSTALLDIR)

$(INSTALLDIR)/pdf-gen : post/pdf-gen.cxx
	$(CC) $(ROOTFLAGS) $(JSONCPPFLAGS) -o $@ $< jsoncpp/jsoncpp.cpp

$(INSTALLDIR)/pdf-gen-volume : post/pdf-gen-volume.cxx
	$(CC) $(ROOTFLAGS) $(JSONCPPFLAGS) -o $@ $< jsoncpp/jsoncpp.cpp

$(INSTALLDIR)/check-simulation : post/check-simulation.cxx post/progressbar/ProgressBar.cc post/progressbar/ProgressBar.h
	$(CC) $(ROOTFLAGS) -Ipost/progressbar -o $@ post/check-simulation.cxx post/progressbar/ProgressBar.cc

$(INSTALLDIR)/livetime-calc-ph2 : post/livetime-calc-ph2.cxx
	$(CC) $(ALLFLAGS) -o $@ $< jsoncpp/jsoncpp.cpp

$(INSTALLDIR)/t4z-gen : post/t4z-gen.cxx
	$(CC) $(ALLFLAGS) -o $@ $< jsoncpp/jsoncpp.cpp

.PHONY: clean-bin bin

clean_$(INSTALLDIR) :
	-rm -r $(INSTALLDIR)
