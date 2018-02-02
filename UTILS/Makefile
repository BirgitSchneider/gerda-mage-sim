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
JOBT4Z  = job-scheduler/mpik-t4z-gen.qsub
JOBPDF  = job-scheduler/mpik-pdf-gen.qsub

# function to change the position of the directory tree
changetree = $(shell echo $(2)/$(shell echo $(1) | rev | cut -d'/' -f-4 | rev))
rm1 = $(shell echo $(1) | cut -d'/' -f2-)

# info extraction
volume  = $(shell echo $(1) | cut -d'/' -f2)
part    = $(shell echo $(1) | cut -d'/' -f3)
isotope = $(shell echo $(1) | cut -d'/' -f4)

# get all directories to be processed with raw- files
PATH_TO_ISOTOPES = $(sort $(foreach d,$(wildcard ../*/*/*/edep ../*/*/*/coin),$(shell dirname $(d))))
PDF_FILES = $(foreach i,$(PATH_TO_ISOTOPES),$(call changetree,$(i)/pdf-$(call volume,$(i))-$(call part,$(i))-$(call isotope,$(i)).root,$(DESTDIR)))

debug :
	@echo $(PDF_FILES)

post : mkdir $(TARGETS)

mkdir :
	@mkdir -p $(DESTDIR)

# function to check if a job is already in the qsub queue
getjob = $(shell if [[ `qstat -r | grep $(1) | grep "Full jobname:"` ]]; then echo 1; else echo 0; fi)

$(TARGETS) :
	@if [[ $(call getjob,$(subst /,-,$(call rm1,$(dir $@))edep.t4z)) == 0 ]]; then \
	$(QSUB) -N $(subst /,-,$(call rm1,$(dir $@))edep.t4z) $(JOBT4Z) \
                   $(call changetree,$(dir $@),$(SRCDIR))edep \
                   $(DESTDIR); \
	else \
	echo "There's already a $(subst /,-,$(call rm1,$(dir $@))edep.t4z) job in queue"; \
	fi
	@if [[ $(call getjob,$(subst /,-,$(call rm1,$(dir $@))coin.t4z)) == 0 ]]; then \
	$(QSUB) -N $(subst /,-,$(call rm1,$(dir $@))coin.t4z) $(JOBT4Z) \
                   $(call changetree,$(dir $@),$(SRCDIR))coin \
                   $(DESTDIR); \
	else \
	echo "There's already a $(subst /,-,$(call rm1,$(dir $@))coin.t4z) job for in queue"; \
	fi
	@if [[ $(call getjob,$(subst /,-,$(call rm1,$(dir $@))all.pdf)) == 0 ]]; then \
	$(QSUB) -hold_jid $(subst /,-,$(call rm1,$(dir $@))edep.t4z),$(subst /,-,$(call rm1,$(dir $@))coin.t4z) \
                   -N $(subst /,-,$(call rm1,$(dir $@))all.pdf) \
                   $(JOBPDF) $(call volume,$@)/$(call part,$@)/$(call isotope,$@) \
                   $(DESTDIR); \
	else \
	echo "There's already a job for $@-all.pdf in queue"; \
	fi

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

### Utilities ##########################################################################################
# extract infos from path
#volume  = $(shell echo $(1) | cut -d "/" -f3)
#part    = $(shell echo $(1) | cut -d "/" -f4)
#isotope = $(shell echo $(1) | cut -d "/" -f5)
#deptype = $(shell echo $(1) | cut -d "/" -f6)
# *CHANGE ME*: array with runIDs
#RUNS = $(shell seq 53 65) 67 $(shell seq 69 89)
# function to build the t4ized file name
# ARG1: directory with raw files
# ARG2: runID
# #t4zfile = t4z-$(call volume,$(1))-$(call part,$(1))-$(call isotope,$(1))-$(call deptype,$(1))-run$(2).root
# function to get the list of t4z- files that will be created
# ARG1: directory with raw files
#out4zfiles = $(foreach id,$(RUNS),$(call changetree,$(1)/$(call t4zfile,$(1),$(id)),$(DESTDIR)))
#ALLT4Z = $(foreach dir,$(DIRS),$(call out4zfiles,$(dir)))
#$(foreach dir,$(DIRS),$(eval $(call t4z-recipe-template,$(dir))))
