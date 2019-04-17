# Makefile
#
# Author: Luigi Pertoldi - luigi.pertoldi@pd.infn.it
# Created: 21/01/2017
#
# USAGE:
#  - make sim : launch the simulations in the cluster
#
# NOTES:
#  - use the --dry-run flag to see what would be done without
#    actually executing it (useful also in conjunction with --debug)
#
# *CHANGE ME* important variables
SRCDIR  = /lfs/l2/gerda/gerda-simulations/gerda-mage-sim
QSUB    = $(shell which qsub)

all :
	@echo "Please pick up one of the following options:"
	@echo " - make sim : launch the simulations in the cluster"

# qsub script must accept the syntax:
# qsub job-scheduler/mpik-run-mage.qsub <macro-file>
JOBSIM = UTILS/job-scheduler/mpik-run-mage.qsub

# find coin and edep directories
SIMDIRS   = $(wildcard */*/*/edep */*/*/coin)
# find macros
MACROS    = $(foreach dir,$(SIMDIRS),$(realpath $(wildcard $(dir)/log/raw-*.mac)))
# build up root file names
ROOTFILES = $(foreach mac,$(MACROS),$(subst .mac,.root,$(subst log/,,$(mac))))

sim : $(ROOTFILES)

# function to check if a job is already in the qsub queue
getjob = $(shell if [[ `qstat -r | grep $(1) | grep "Full jobname:"` ]]; then echo 1; else echo 0; fi)

# second expansion is needed here to build up macro file name from target
.SECONDEXPANSION:
$(ROOTFILES) : %.root : $$(dir $$*)log/$$(notdir $$*).mac $$(dir $$*)log/preamble.mac
	@if [[ $(call getjob,$(notdir $@)) == 0 ]]; then \
	$(QSUB) -N $(notdir $@) $(JOBSIM) $^; \
	else \
	echo "There's already a $(notdir $@) job in queue"; \
	fi

# another (very slow) way to do it
#define run-sim-template
#  $(1) : $$(dir $(1))log/$$(subst .root,.mac,$$(notdir $(1)))
#  ifeq '$$(shell if [[ `qstat -r | grep "$$(notdir $(1))"` ]]; then echo 1; else echo 0; fi)' '0'
#    $$(QSUB) -N $$(notdir $(1)) $$(JOBSIM) $$^
#  else
#    @echo "There's already a job for $(1) in queue"
#  endif
#endef
#
#$(foreach f,$(ROOTFILES),$(eval $(call run-sim-template,$(f))))

.PHONY : sim
