CC = c++
ROOTFLAGS      = $(shell root-config --libs --cflags) -lTreePlayer
GERDA-ADAFLAGS = $(shell gerda-ada-config --libs --cflags)
GELATIOFLAGS   = $(shell gelatio-config --libs --cflags)
JSONCPPFLAGS   = -I jsoncpp
ALLFLAGS       = $(ROOTFLAGS) $(GERDA-ADAFLAGS) $(GELATIOFLAGS) $(JSONCPPFLAGS)

all : dirs bin/gen-spectra bin/check-simulation bin/livetime-calc-ph2

dirs :
	@mkdir -p bin

bin/gen-spectra : post/gen-spectra.cxx post/progressbar/ProgressBar.cc post/progressbar/ProgressBar.h
	$(CC) $(ROOTFLAGS) -Ipost/progressbar -o $@ post/gen-spectra.cxx post/progressbar/ProgressBar.cc

bin/check-simulation : post/check-simulation.cxx post/progressbar/ProgressBar.cc post/progressbar/ProgressBar.h
	$(CC) $(ROOTFLAGS) -Ipost/progressbar -o $@ post/check-simulation.cxx post/progressbar/ProgressBar.cc

bin/livetime-calc-ph2 : post/livetime-calc-ph2.cxx
	$(CC) $(ALLFLAGS) -o $@ $< jsoncpp.cpp

.PHONY: clean all

clean :
	-rm -r bin
