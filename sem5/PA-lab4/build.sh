PROCESSORS=$1
shift

mkdir -p ./build
mpicc -o ./build/runnable "$*"
mpiexec -np "$PROCESSORS" ./build/runnable
