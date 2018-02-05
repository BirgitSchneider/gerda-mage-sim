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
