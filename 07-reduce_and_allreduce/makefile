EXECS = reduce_avg allreduce_std
MPICC ?= mpicc
CFLAG = -lm

all: ${EXECS}

reduce_avg: reduce_avg.c
	${MPICC} -o reduce_avg reduce_avg.c

allreduce_std: allreduce_std.c
	${MPICC} ${CFLAG} -o allreduce_std allreduce_std.c

clean:
	rm -f ${EXECS}