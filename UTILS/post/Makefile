# Makefile
#
# Author: Luigi Pertoldi - luigi.pertoldi@pd.infn.it
# Created: 21/01/2017
#
# temptative makefile to launch the post-processing execution queue
#
# NOTES: - use the --dry-run flag to see what would be done without
#          actually executing it
#        - this makefile assumes that the qsub script accepts the syntax:
#          qsub submitfile.qsub <folder-with-raw> <dest-dir-tree>
#
SRCDIR  = /path/to/gerda-mage-sim
DESTDIR = /path/to/dest/tree
JOB     = qsub submitfile.qsub

# function to change the position of the directory tree
changetree = $(shell echo $(2)/$(shell echo $(1) | rev | cut -d'/' -f-5 | rev))

# get all directories to be processed with raw- files
DIRS = $(wildcard ../../*/*/*/edep ../../*/*/*/coin)

# function to build the t4ized file name
buildname = t4z-$(shell echo $(1) | cut -d "/" -f3)-$(shell echo $(1) | cut -d "/" -f4)-$(shell echo $(1) | cut -d "/" -f5)-$(shell echo $(1) | cut -d "/" -f6).root

# construct tier4ized file names in final position
T4ZFILES = $(foreach dir, $(DIRS), $(call changetree, $(dir)/$(call buildname, $(dir)), $(DESTDIR)))

all : $(T4ZFILES)

# the following runs for each item in T4ZFILES
# the dependencies are set to be the raw- files, so if they change the corresponding t4z- file
# is re-processed
$(T4ZFILES) : $(wildcard $(dir $(call changetree, $@, $(SRCDIR)))/*.root)
	@$(JOB) $(dir $(call changetree, $@, $(SRCDIR))) $(DESTDIR)

clean :
	-rm -rf $(T4ZFILES)

.PHONY : clean all
