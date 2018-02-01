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
#  - unfortunately the GNU make functionalities are not fully available.
#    This is due to the fact that the qsub submit commands exit after the
#    job has been successfully spawned and don't keep track of the real
#    execution time.
#    So when you run 'make post' you reprocess everything, and not only new
#    or modified files. 'make sim' is instead fully capable!
#  - use the --dry-run flag to see what would be done without
#    actually executing it (useful also in conjunction with --debug)
#
SRCDIR  = /lfs/l2/gerda/gerda-simulations/gerda-mage-sim
DESTDIR = /lfs/l2/gerda/gerda-simulations/gerda-pdfs/v0.1
QSUB = qsub
# qsub script must accept the syntax:
# qsub job-scheduler/mpik-t4z-gen.qsub <folder-with-raw> <dest-dir-tree>
# qsub job-scheduler/mpik-pdf-gen.qsub <volume>/<part>/<isotope> <dest-dir-tree>
JOBT4Z  = job-scheduler/mpik-t4z-gen.qsub
JOBPDF  = job-scheduler/mpik-pdf-gen.qsub

all :
	@echo "Please pick up one of the following options:"
	@echo " - make sim : launch the simulations in the cluster"
	@echo " - make bin : compile the command line tools"
	@echo " - make post : lauch post-processing in the cluster"

# function to change the position of the directory tree
changetree = $(shell echo $(2)/$(shell echo $(1) | rev | cut -d'/' -f-4 | rev))
rm1 = $(shell echo $(1) | cut -d'/' -f2-)

# info extraction
volume  = $(shell echo $(1) | cut -d'/' -f2)
part    = $(shell echo $(1) | cut -d'/' -f3)
isotope = $(shell echo $(1) | cut -d'/' -f4)

# get all directories to be processed with raw- files
DIRSWITHRAW = $(foreach d,$(wildcard ../*/*/*/edep ../*/*/*/coin),$(shell dirname $(d)))

TARGETS = $(sort $(foreach dir,$(DIRSWITHRAW),$(dir)/post-proc))

post : mkdir $(TARGETS)

mkdir :
	@mkdir -p $(DESTDIR)

getjob = $(shell if [[ `qstat -r | grep $(1) | grep "Full jobname:"` ]]; then echo 1; else echo 0; fi)

$(TARGETS) :
	@if [[ $(call getjob,$(subst /,-,$(call rm1,$(dir $@))edep.t4z)) == 0 ]]; then \
	$(QSUB) -N $(subst /,-,$(call rm1,$(dir $@))edep.t4z) $(JOBT4Z) \
                   $(call changetree,$(dir $@),$(SRCDIR))edep \
                   $(DESTDIR) >/dev/null; \
	echo "$(subst /,-,$(call rm1,$(dir $@))edep.t4z) job sent"; \
	else \
	echo "There's already a $(subst /,-,$(call rm1,$(dir $@))edep.t4z) job in queue"; \
	fi
	@if [[ $(call getjob,$(subst /,-,$(call rm1,$(dir $@))coin.t4z)) == 0 ]]; then \
	$(QSUB) -N $(subst /,-,$(call rm1,$(dir $@))coin.t4z) $(JOBT4Z) \
                   $(call changetree,$(dir $@),$(SRCDIR))coin \
                   $(DESTDIR) >/dev/null; \
	echo "$(subst /,-,$(call rm1,$(dir $@))coin.t4z) job sent"; \
	else \
	echo "There's already a $(subst /,-,$(call rm1,$(dir $@))coin.t4z) job for in queue"; \
	fi
	@if [[ $(call getjob,$(subst /,-,$(call rm1,$(dir $@))all.pdf)) == 0 ]]; then \
	$(QSUB) -hold_jid $(subst /,-,$(call rm1,$(dir $@))edep.t4z),$(subst /,-,$(call rm1,$(dir $@))coin.t4z) \
                   -N $(subst /,-,$(call rm1,$(dir $@))all.pdf) \
                   $(JOBPDF) $(call volume,$@)/$(call part,$@)/$(call isotope,$@) \
                   $(DESTDIR) >/dev/null; \
	echo "$(subst /,-,$(call rm1,$(dir $@))all.pdf) job sent"; \
	else \
	echo "There's already a job for $@-all.pdf in queue"; \
	fi

clean-t4z :
	-rm -rf $(DESTDIR)

.PHONY : clean-t4z post mkdir

### Run simulations ###########################################################################

JOBSIM = job-scheduler/mpik-run-mage.qsub

SIMDIRS   = $(wildcard ../*/*/*/edep ../*/*/*/coin)
MACROS    = $(foreach dir,$(SIMDIRS),$(realpath $(wildcard $(dir)/log/raw-*.mac)))
ROOTFILES = $(foreach mac,$(MACROS),$(subst .mac,.root,$(subst log/,,$(mac))))

sim : $(ROOTFILES)

define run-sim-template
  $(1) : $$(dir $(1))log/$$(subst .root,.mac,$$(notdir $(1)))
  ifeq '$$(shell if [[ `qstat -r | grep "$$(notdir $(1))"` ]]; then echo 1; else echo 0; fi)' '0'
	$$(QSUB) -N $$(notdir $(1)) $$(JOBSIM) $$^
  else
	@echo "There's already a job for $(1) in queue"
  endif
endef

$(foreach f,$(ROOTFILES),$(eval $(call run-sim-template,$(f))))

#$(ROOTFILES) : % : $(call volume,%)
#ifeq '$$(shell if [[ `qstat -r | grep "$(notdir $@)"` ]]; then echo 1; else echo 0; fi)' '0'
#	$(QSUB) -N $(notdir $@) $(JOBSIM) $^
#else
#	@echo "There's already a job for $@ in queue"
#endif

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
	$(CC) $(ROOTFLAGS) $(JSONCPPFLAGS) -o $@ $< jsoncpp/jsoncpp.cpp

bin/pdf-gen-volume : post/pdf-gen-volume.cxx
	$(CC) $(ROOTFLAGS) $(JSONCPPFLAGS) -o $@ $< jsoncpp/jsoncpp.cpp

bin/check-simulation : post/check-simulation.cxx post/progressbar/ProgressBar.cc post/progressbar/ProgressBar.h
	$(CC) $(ROOTFLAGS) -Ipost/progressbar -o $@ post/check-simulation.cxx post/progressbar/ProgressBar.cc

bin/livetime-calc-ph2 : post/livetime-calc-ph2.cxx
	$(CC) $(ALLFLAGS) -o $@ $< jsoncpp/jsoncpp.cpp

bin/t4z-gen : post/t4z-gen.cxx
	$(CC) $(ALLFLAGS) -o $@ $< jsoncpp/jsoncpp.cpp

.PHONY: clean_bin bin

clean_bin :
	-rm -r bin

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
