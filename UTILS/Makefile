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
SRCDIR  = /lfs/l2/gerda/Hades/gerda-mage-sim
DESTDIR = /lfs/l2/gerda/Hades/post-proc
# qsub script must accept the syntax:
# qsub job-scheduler/t4z-gen.qsub <folder-with-raw> <dest-dir-tree>
JOBT4Z  = qsub job-scheduler/t4z-gen.qsub

# function to change the position of the directory tree
changetree = $(shell echo $(2)/$(shell echo $(1) | rev | cut -d'/' -f-4 | rev))

# get all directories to be processed with raw- files
#DIRS = $(wildcard ../../*/*/*/edep ../../*/*/*/coin)
DIRS = ../../minishroud/glue_rings_bottom/Bi212/edep ../../minishroud/glue_rings_bottom/Pb214/edep

# extract infos from path
volume  = $(shell echo $(1) | cut -d "/" -f3)
part    = $(shell echo $(1) | cut -d "/" -f4)
isotope = $(shell echo $(1) | cut -d "/" -f5)
deptype = $(shell echo $(1) | cut -d "/" -f6)

# *CHANGE ME*: array with runIDs
RUNS = $(shell seq 53 65) 67 $(shell seq 69 89)

# function to build the t4ized file name
# ARG1: directory with raw files
# ARG2: runID
t4zfile = t4z-$(call volume,$(1))-$(call part,$(1))-$(call isotope,$(1))-$(call deptype,$(1))-run$(2).root

# function to get the list of t4z- files that will be created
# ARG1: directory with raw files
out4zfiles = $(foreach id,$(RUNS),$(call changetree,$(1)/$(call t4zfile,$(1),$(id)),$(DESTDIR)))

ALLT4Z = $(foreach dir,$(DIRS),$(call out4zfiles,$(dir)))

post : $(ALLT4Z)

# the dependencies are set to be the raw- files, so if they change the
# corresponding t4z- files are re-processed
define t4z-recipe-template
  $(call out4zfiles,$(1)) : $$(wildcard $$(call changetree,$(1),$(SRCDIR))/raw-*.root); $$(JOBT4Z) $$(call changetree,$(1),$$(SRCDIR)) $$(DESTDIR) > /dev/null
endef

$(foreach dir,$(DIRS),$(eval $(call t4z-recipe-template,$(dir))))

clean_t4z :
	-rm -rf $(T4ZFILES)

.PHONY : clean_t4z post

### Run simulations ###########################################################################

QSUB_RUNSIM = qsub job-scheduler/mpik-run-mage.qsub

SIMDIRS   = $(wildcard ../*/*/*/edep ../*/*/*/coin)
MACROS    = $(foreach dir,$(SIMDIRS),$(realpath $(wildcard $(dir)/log/raw-*.mac)))
ROOTFILES = $(foreach mac,$(MACROS),$(subst .mac,.root,$(subst log/,,$(mac))))

sim : $(ROOTFILES)

define run-sim-template
  $(1) : $$(dir $(1))log/$$(subst .root,.mac,$$(notdir $(1))); $$(QSUB_RUNSIM) $$^
endef

$(foreach f,$(ROOTFILES),$(eval $(call run-sim-template,$(f))))

.PHONY : sim

### Compile ####################################################################################

CC = c++ -std=c++1y
ROOTFLAGS      = $(shell root-config --libs --cflags) -lTreePlayer
GERDA-ADAFLAGS = $(shell gerda-ada-config --libs --cflags)
GELATIOFLAGS   = $(shell gelatio-config --libs --cflags)
MGDOFLAGS      = $(shell mgdo-config --libs --cflags)
DATABRIXXFLAGS = $(shell databricxx-config --libs --cflags)
JSONCPPFLAGS   = -I jsoncpp
ALLFLAGS       = $(ROOTFLAGS) $(GERDA-ADAFLAGS) $(GELATIOFLAGS) $(MGDOFLAGS) $(DATABRIXXFLAGS) $(JSONCPPFLAGS)

bin : dirs $(foreach b,$(wildcard post/*.cxx),$(addprefix bin/,$(notdir $(basename $(b)))))

dirs :
	@mkdir -p bin

bin/pdf-gen : post/pdf-gen.cxx
	$(CC) $(ROOTFLAGS) $(JSONCPPFLAGS) -I post/progressbar -o $@ $< jsoncpp/jsoncpp.cpp post/progressbar/ProgressBar.cc

bin/check-simulation : post/check-simulation.cxx post/progressbar/ProgressBar.cc post/progressbar/ProgressBar.h
	$(CC) $(ROOTFLAGS) -Ipost/progressbar -o $@ post/check-simulation.cxx post/progressbar/ProgressBar.cc

bin/livetime-calc-ph2 : post/livetime-calc-ph2.cxx
	$(CC) $(ALLFLAGS) -o $@ $< jsoncpp/jsoncpp.cpp

bin/t4z-gen : post/t4z-gen.cxx
	$(CC) $(ALLFLAGS) -o $@ $< jsoncpp/jsoncpp.cpp

.PHONY: clean_bin bin

clean_bin :
	-rm -r bin
