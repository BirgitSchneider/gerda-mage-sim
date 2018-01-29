CC = c++ -std=c++1y
ROOTFLAGS      = $(shell root-config --libs --cflags) -lTreePlayer
GERDA-ADAFLAGS = $(shell gerda-ada-config --libs --cflags)
GELATIOFLAGS   = $(shell gelatio-config --libs --cflags)
MGDOFLAGS      = $(shell mgdo-config --libs --cflags)
DATABRIXXFLAGS = $(shell databricxx-config --libs --cflags)
JSONCPPFLAGS   = -I jsoncpp
ALLFLAGS       = $(ROOTFLAGS) $(GERDA-ADAFLAGS) $(GELATIOFLAGS) $(MGDOFLAGS) $(DATABRIXXFLAGS) $(JSONCPPFLAGS)

all : dirs bin/gen-spectra bin/check-simulation bin/livetime-calc-ph2 bin/t4z-gen bin/pdf-gen

dirs :
	@mkdir -p bin

bin/gen-spectra : post/gen-spectra.cxx
	$(CC) $(ROOTFLAGS) -o $@ $<

bin/pdf-gen : post/pdf-gen.cxx
	$(CC) $(ROOTFLAGS) $(JSONCPPFLAGS) -I post/progressbar -o $@ $< jsoncpp/jsoncpp.cpp post/progressbar/ProgressBar.cc

bin/check-simulation : post/check-simulation.cxx post/progressbar/ProgressBar.cc post/progressbar/ProgressBar.h
	$(CC) $(ROOTFLAGS) -Ipost/progressbar -o $@ post/check-simulation.cxx post/progressbar/ProgressBar.cc

bin/livetime-calc-ph2 : post/livetime-calc-ph2.cxx
	$(CC) $(ALLFLAGS) -o $@ $< jsoncpp/jsoncpp.cpp

bin/t4z-gen : post/t4z-gen.cxx
	$(CC) $(ALLFLAGS) -o $@ $< jsoncpp/jsoncpp.cpp

.PHONY: clean all

clean :
	-rm -r bin
