CC=gcc
CFLAGS=-fopenmp -std=c99

all: experiment

experiment: experiment.c
    $(CC) $(CFLAGS) $(DFLAGS) -o $@ $<

experiment_t1_s1_c1:
    $(MAKE) DFLAGS="-DNUM_THREADS=1 -DSCHEDULE=static -DCONSTRUCT=reduction"

clean:
    rm -f experiment
