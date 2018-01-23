# Makefile
#
# Author: Luigi Pertoldi - luigi.pertoldi@pd.infn.it
# Created: 21/01/2017
#
# temptative makefile to launch the post-processing execution queue
#
# NOTES: - use the --dry-run flag to see what would be done without
#          actually executing it (useful also in conjunction with --debug)
#        - this makefile assumes that the qsub script accepts the syntax:
#          qsub submitfile.qsub <folder-with-raw> <dest-dir-tree>
#
SRCDIR  = /lfs/l2/gerda/Hades/gerda-mage-sim
DESTDIR = /lfs/l2/gerda/Hades/post-proc
JOB     = qsub ../job-scheduler/process-raw.qsub

# function to change the position of the directory tree
changetree = $(shell echo $(2)/$(shell echo $(1) | rev | cut -d'/' -f-5 | rev))

# get all directories to be processed with raw- files
DIRS = $(wildcard ../../*/*/*/edep ../../*/*/*/coin)

# extract infos from path
volume  = $(shell echo $(1) | cut -d "/" -f3)
part    = $(shell echo $(1) | cut -d "/" -f4)
isotope = $(shell echo $(1) | cut -d "/" -f5)
deptype = $(shell echo $(1) | cut -d "/" -f6)

# function to build the t4ized file name
buildname = t4z-$(call volume, $(1))-$(call part, $(1))-$(call isotope, $(1))-$(call deptype, $(1)).root

# construct tier4ized file names in final position
T4ZFILES = $(foreach dir, $(DIRS), $(call changetree, $(dir)/$(call buildname, $(dir)), $(DESTDIR)))

all : $(T4ZFILES)

# in the following a rule is created for each item in T4ZFILES
# the dependencies are set to be the raw- files, so if they change the
# corresponding t4z- file is re-processed
$(T4ZFILES) : $(wildcard $(dir $(call changetree, $@, $(SRCDIR)))raw-*.root)
	@$(JOB) $(dir $(call changetree, $@, $(SRCDIR))) $(DESTDIR)

clean :
	-rm -rf $(T4ZFILES)

.PHONY : clean all
