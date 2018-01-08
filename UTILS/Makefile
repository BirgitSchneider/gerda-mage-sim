CC = c++
ROOTFLAGS = $(shell root-config --libs --cflags) -lTreePlayer

all : post/gen-spectra post/check-simulation

post/gen-spectra : post/gen-spectra.cxx post/progressbar/ProgressBar.cc post/progressbar/ProgressBar.h
	$(CC) $(ROOTFLAGS) -Ipost/progressbar -o $@ post/gen-spectra.cxx post/progressbar/ProgressBar.cc

post/check-simulation : post/check-simulation.cxx
	$(CC) $(ROOTFLAGS) -o $@ $<

.PHONY: clean

clean : rm -f $(all)
