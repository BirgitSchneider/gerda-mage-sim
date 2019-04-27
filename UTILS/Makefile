# Makefile
#
# Author: Luigi Pertoldi - luigi.pertoldi@pd.infn.it
# Created: 21/01/2017
#

CC = c++ -std=c++1y -O3 -g
INSTALLDIR     = bin
ROOTFLAGS      = $(shell root-config --libs --cflags) -lTreePlayer
GERDA-ADAFLAGS = $(shell gerda-ada-config --libs --cflags)
GELATIOFLAGS   = $(shell gelatio-config --libs --cflags)
MGDOFLAGS      = $(shell mgdo-config --libs --cflags)
DATABRIXXFLAGS = $(shell databricxx-config --libs --cflags)
JSONCPPFLAGS   = -I jsoncpp
ALLFLAGS       = $(ROOTFLAGS) $(GERDA-ADAFLAGS) $(GELATIOFLAGS) $(MGDOFLAGS) $(DATABRIXXFLAGS) $(JSONCPPFLAGS)

all : dirs $(foreach b,$(wildcard post/*.cxx surf-sampling/*.cxx health-dep/*.cxx),$(addprefix $(INSTALLDIR)/,$(notdir $(basename $(b)))))

dirs :
	@mkdir -p $(INSTALLDIR)

$(INSTALLDIR)/pdf-gen : post/pdf-gen.cxx
	$(CC) $(ROOTFLAGS) $(JSONCPPFLAGS) -o $@ $< jsoncpp/jsoncpp.cpp

$(INSTALLDIR)/check-simulation : post/check-simulation.cxx progressbar/ProgressBar.cc progressbar/ProgressBar.h
	$(CC) $(ROOTFLAGS) -Iprogressbar -o $@ post/check-simulation.cxx progressbar/ProgressBar.cc

$(INSTALLDIR)/livetime-calc-ph2 : post/livetime-calc-ph2.cxx
	$(CC) $(ALLFLAGS) -o $@ $< jsoncpp/jsoncpp.cpp

$(INSTALLDIR)/livetime-calc-calib : post/livetime-calc-calib.cxx
	$(CC) $(ALLFLAGS) -o $@ $< jsoncpp/jsoncpp.cpp

$(INSTALLDIR)/res-curves-jsonizer : post/res-curves-jsonizer.cxx
	$(CC) $(ROOTFLAGS) $(JSONCPPFLAGS) -o $@ $< jsoncpp/jsoncpp.cpp

$(INSTALLDIR)/t4z-gen : post/t4z-gen.cxx
	$(CC) $(ALLFLAGS) -o $@ $< jsoncpp/jsoncpp.cpp

$(INSTALLDIR)/t4z-gen-calib : post/t4z-gen-calib.cxx
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

.PHONY: clean

clean :
	-rm -rf $(INSTALLDIR)
