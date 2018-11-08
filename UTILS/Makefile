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

all :
	@echo "Please pick up one of the following options:"
	@echo " - make bin : compile the command line tools"

CC = c++ -std=c++1y
INSTALLDIR     = bin
ROOTFLAGS      = $(shell root-config --libs --cflags) -lTreePlayer
GERDA-ADAFLAGS = $(shell gerda-ada-config --libs --cflags)
GELATIOFLAGS   = $(shell gelatio-config --libs --cflags)
MGDOFLAGS      = $(shell mgdo-config --libs --cflags)
DATABRIXXFLAGS = $(shell databricxx-config --libs --cflags)
JSONCPPFLAGS   = -I jsoncpp
ALLFLAGS       = $(ROOTFLAGS) $(GERDA-ADAFLAGS) $(GELATIOFLAGS) $(MGDOFLAGS) $(DATABRIXXFLAGS) $(JSONCPPFLAGS)

bin : dirs $(foreach b,$(wildcard post/*.cxx surf-sampling/*.cxx health-dep/*.cxx),$(addprefix $(INSTALLDIR)/,$(notdir $(basename $(b)))))

dirs :
	@mkdir -p $(INSTALLDIR)

$(INSTALLDIR)/pdf-gen : post/pdf-gen.cxx
	$(CC) $(ROOTFLAGS) $(JSONCPPFLAGS) -o $@ $< jsoncpp/jsoncpp.cpp

$(INSTALLDIR)/check-simulation : post/check-simulation.cxx progressbar/ProgressBar.cc progressbar/ProgressBar.h
	$(CC) $(ROOTFLAGS) -Iprogressbar -o $@ post/check-simulation.cxx progressbar/ProgressBar.cc

$(INSTALLDIR)/livetime-calc-ph2 : post/livetime-calc-ph2.cxx
	$(CC) $(ALLFLAGS) -o $@ $< jsoncpp/jsoncpp.cpp

$(INSTALLDIR)/res-curves-jsonizer : post/res-curves-jsonizer.cxx
	$(CC) $(ROOTFLAGS) $(JSONCPPFLAGS) -o $@ $< jsoncpp/jsoncpp.cpp

$(INSTALLDIR)/t4z-gen : post/t4z-gen.cxx
	$(CC) $(ALLFLAGS) -o $@ $< jsoncpp/jsoncpp.cpp

$(INSTALLDIR)/shuffle-tree : surf-sampling/shuffle-tree.cxx
	$(CC) $(ALLFLAGS) -o $@ $<

$(INSTALLDIR)/sim-doctor : health-dep/sim-doctor.cxx read-dir-cxx/ReadDir.cxx progressbar/ProgressBar.cc
	$(CC) $(ROOTFLAGS) -Iprogressbar -Iread-dir-cxx -o $@ $^

$(INSTALLDIR)/pdf-doctor : health-dep/pdf-doctor.cxx read-dir-cxx/ReadDir.cxx progressbar/ProgressBar.cc
	$(CC) $(ROOTFLAGS) -Iprogressbar -Iread-dir-cxx -o $@ $^

$(INSTALLDIR)/alphas-post-processing : post/alphas-post-processing.cxx read-dir-cxx/ReadDir.cxx progressbar/ProgressBar.cc
	$(CC) $(ROOTFLAGS) -Iprogressbar -Iread-dir-cxx -o $@ $^

$(INSTALLDIR)/split-volumes-K42 : post/split-volumes-K42.cxx 
	$(CC) $(ROOTFLAGS) -o $@ $^

.PHONY: clean-bin bin

clean_$(INSTALLDIR) :
	-rm -r $(INSTALLDIR)
