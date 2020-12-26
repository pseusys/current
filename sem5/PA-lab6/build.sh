PROCESSORS=$1
shift

mkdir -p ./build
mpicc -o ./build/runnable $* -lm
mpiexec -np "$PROCESSORS" ./build/runnable
