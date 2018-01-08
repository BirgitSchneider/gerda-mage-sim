CC = c++
ROOTFLAGS = $(shell root-config --libs --cflags) -lTreePlayer

all : post/gen-spectra post/check-simulation

post/gen-spectra : post/gen-spectra.cxx post/progressbar/ProgressBar.cc post/progressbar/ProgressBar.h
	$(CC) $(ROOTFLAGS) -Ipost/progressbar -o $@ post/gen-spectra.cxx post/progressbar/ProgressBar.cc

post/check-simulation : post/check-simulation.cxx post/progressbar/ProgressBar.cc post/progressbar/ProgressBar.h
	$(CC) $(ROOTFLAGS) -Ipost/progressbar -o $@ post/check-simulation.cxx post/progressbar/ProgressBar.cc

.PHONY: clean

clean : rm -f $(all)
