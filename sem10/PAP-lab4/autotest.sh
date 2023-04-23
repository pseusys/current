#!/bin/bash

function flush_config {
    echo "# GENERATED TEST CONFIG FILE, DO NOT EDIT!
    iterations           = $2
    width                = $(($3 * 5))
    height               = $3
    reynolds             = 200
    inflow_max_velocity  = 0.100000
    output_filename      = output.raw
    write_interval       = 50" > $1
}

function test_iterations_number {
    touch $1
    ALGORITHMS_NUM="${#ALGORITHMS[@]}"
    TIMEOUT="$(($ALGORITHMS_NUM * 360))"

    echo -n "iteration number;" >> $1
    for i in "${ALGORITHMS[@]}"; do
        echo -n "exercise_$i;" >> $1
    done
    echo >> $1

    for i in {1..30}; do
        flush_config "test/test_config.csv" ${i}000 160

        echo -n "${i}000;" >> $1
        EARLY_BREAK=0
        for ((a=0;a<ALGORITHMS_NUM;a++)); do
            if [ $a == 0 ]; then TH=1;
            else TH=$(nproc --all); fi

            echo -e "\ttesting with: THREADS=$TH EXERCISE=$a MATRIX=160x800 ITERATIONS=${i}000"
            START=`date +%s%N`
            timeout --preserve-status ${TIMEOUT}s bash -c "mpirun --oversubscribe -np $TH ./lbm -e $a" &> /dev/null || {
                EARLY_BREAK="$(($EARLY_BREAK + 1))"
                echo -e "\t\ttimeout!"
                echo -n "None;" >> $1
            }
            END=`date +%s%N`
            DURATION="$(($END - $START))"
            echo -n "$DURATION;" >> $1
        done
        echo >> $1
        if [[ $EARLY_BREAK == $ALGORITHMS_NUM ]]; then break; fi
    done
    echo >> $1

    rm -f test/test_config.csv 2> /dev/null
}

function test_matrix_size {
    touch $1
    ALGORITHMS_NUM="${#ALGORITHMS[@]}"
    TIMEOUT="$(($ALGORITHMS_NUM * 360))"

    echo -n "matrix size;" >> $1
    for i in "${ALGORITHMS[@]}"; do
        echo -n "exercise_$i;" >> $1
    done
    echo >> $1

    for i in {0..7}; do
        MT="$((20 * 2 ** $i))"
        flush_config "test/test_config.csv" 16000 $MT

        echo -n "$MT;" >> $1
        EARLY_BREAK=0
        for ((a=0;a<ALGORITHMS_NUM;a++)); do
            if [ $a == 0 ]; then TH=1;
            else TH=$(nproc --all); fi

            echo -e "\ttesting with: THREADS=$TH EXERCISE=$a MATRIX=${MT}x$(($MT * 5)) ITERATIONS=16000"
            START=`date +%s%N`
            timeout --preserve-status ${TIMEOUT}s bash -c "mpirun --oversubscribe -np $TH ./lbm -e $a" &> /dev/null || {
                EARLY_BREAK="$(($EARLY_BREAK + 1))"
                echo -e "\t\ttimeout!"
                echo -n "None;" >> $1
            }
            END=`date +%s%N`
            DURATION="$(($END - $START))"
            echo "$DURATION;" >> $1
        done
        echo >> $1
        if [[ $EARLY_BREAK == $ALGORITHMS_NUM ]]; then break; fi
    done
    echo >> $1

    rm -f test/test_config.csv 2> /dev/null
}

function test_threads_number {
    touch $1
    ALGORITHMS_NUM="${#ALGORITHMS[@]}"
    TIMEOUT="$(($ALGORITHMS_NUM * 360))"

    echo -n "threads number;" >> $1
    for i in "${ALGORITHMS[@]}"; do
        echo -n "exercise_$i;" >> $1
    done
    echo >> $1

    for i in {1..15}; do
        TH="$((2 * $i))"
        flush_config "test/test_config.csv" 16000 160

        echo -n "$TH;" >> $1
        EARLY_BREAK=0
        for ((a=0;a<ALGORITHMS_NUM;a++)); do
            if [ $a == 0 ]; then TH=1;
            else TH=$TH; fi

            echo -e "\ttesting with: THREADS=$TH EXERCISE=$a MATRIX=160x800 ITERATIONS=16000"
            START=`date +%s%N`
            timeout --preserve-status ${TIMEOUT}s bash -c "mpirun --oversubscribe -np $TH ./lbm -e $a" &> /dev/null || {
                EARLY_BREAK="$(($EARLY_BREAK + 1))"
                echo -e "\t\ttimeout!"
                echo -n "None;" >> $1
            }
            END=`date +%s%N`
            DURATION="$(($END - $START))"
            echo "$DURATION;" >> $1
        done
        echo >> $1
        if [[ $EARLY_BREAK == $ALGORITHMS_NUM ]]; then break; fi
    done
    echo >> $1

    rm -f test/test_config.csv 2> /dev/null
}


ALGORITHMS=(0 1 2 3 4 5 6)

echo -e "Testing with fixed thread number, matrix size and different iterations number..."
rm -f test/iterations_number.csv 2> /dev/null
test_iterations_number "test/iterations_number.csv"
echo -e "Tested with fixed thread number, matrix size and different iterations number!"

echo -e "Testing with fixed thread number, different matrix size and fixed iterations number..."
rm -f test/matrix_size.csv 2> /dev/null
test_matrix_size "test/matrix_size.csv"
echo -e "Tested with fixed thread number, different matrix size and fixed iterations number!"

echo -e "Testing with different thread number, fixed matrix size and iterations number..."
rm -f test/threads_number.csv 2> /dev/null
test_threads_number "test/threads_number.csv"
echo -e "Tested with different thread number, fixed matrix size and iterations number!"
