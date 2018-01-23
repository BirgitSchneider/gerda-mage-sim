CC = c++
ROOTFLAGS = $(shell root-config --libs --cflags) -lTreePlayer

all : dirs bin/gen-spectra bin/check-simulation

dirs :
	@mkdir -p bin

bin/gen-spectra : post/gen-spectra.cxx post/progressbar/ProgressBar.cc post/progressbar/ProgressBar.h
	$(CC) $(ROOTFLAGS) -Ipost/progressbar -o $@ post/gen-spectra.cxx post/progressbar/ProgressBar.cc

bin/check-simulation : post/check-simulation.cxx post/progressbar/ProgressBar.cc post/progressbar/ProgressBar.h
	$(CC) $(ROOTFLAGS) -Ipost/progressbar -o $@ post/check-simulation.cxx post/progressbar/ProgressBar.cc

.PHONY: clean all

clean :
	-rm -r bin
