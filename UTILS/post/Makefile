# Makefile
#
# Author: Luigi Pertoldi - luigi.pertoldi@pd.infn.it
# Created: 21/01/2017
#
# temptative makefile to launch the post-processing execution queue
#
# NOTES: use the --dry-run flag to see what would be done without
#        actually executing it
#
PROC    = julia process-raw.jl
JOB     = qsub <...>
DESTDIR = <...>

# get all directories to be processed
DIRS = $(wildcard ../../*/*/*/edep ../../*/*/*/coin)
# construct tier4ized file names
T4ZFILES = $(foreach dir, $(DIRS), $(dir)/t4z-$(shell echo $(dir) | cut -d "/" -f3 )-$(shell echo $(dir) | cut -d "/" -f4 )-$(shell echo $(dir) | cut -d "/" -f5 )-$(shell echo $(dir) | cut -d "/" -f6 ).root)

all : $(T4ZFILES)

$(T4ZFILES) : $(wildcard $(dir $@)/*.root)
ifneq (,$(findstring dry-run,  $(MAKEFLAGS)))
	@echo "$(JOB) $(dir $@) $(DESTDIR)"
else
	$(JOB) $(dir $@) $(DESTDIR)
endif

clean :
	-rm -rf $(T4ZFILES)

.PHONY : dry-run clean all
