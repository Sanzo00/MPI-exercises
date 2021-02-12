EXECS = random_rank
MPICC ?= mpicc

all: ${EXECS}

random_rank: random_rank.c
	${MPICC} -o random_rank random_rank.c

clean:
	rm -f ${EXECS}