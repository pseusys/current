#!/bin/bash

function test_matrix_size {
    touch $1
    ALGORITHMS_NUM="${#ALGORITHMS[@]}"
    TIMEOUT="$(($ALGORITHMS_NUM * 360))"

    echo -n "matrix size;" >> $1
    for i in "${ALGORITHMS[@]}"; do
        echo -n "$i;" 
    done
    echo >> $1

    for i in {0..10}; do
        MT="$((2^$i))"

        echo -n "$MT;" >> $1
        EARLY_BREAK=0
        for ((a=0;a<ALGORITHMS_NUM;a++)); do
            if [ $a == 0 ]; then TH=1;
            else TH=$(nproc --all); fi

            echo -e "\ttesting with: THREADS=$TH EXERCISE=$a MATRIX=${MT}) ITERATIONS=16000"
            START=`date +%s%N`
            timeout --preserve-status ${TIMEOUT}s bash -c "mpirun -np $TH --oversubscribe ./build/$a.run ${MT}" &> /dev/null && {
                END=`date +%s%N`
                DURATION="$(($END - $START))"
                echo -n "$DURATION;" >> $1
            } || {
                EARLY_BREAK="$(($EARLY_BREAK + 1))"
                echo -e "\t\ttimeout!"
                echo -n "None;" >> $1
            }
        done
        echo >> $1
        if [[ $EARLY_BREAK == $ALGORITHMS_NUM ]]; then break; fi
    done
    echo >> $1

}

function test_threads_number {
    touch $1
    ALGORITHMS_NUM="${#ALGORITHMS[@]}"
    TIMEOUT="$(($ALGORITHMS_NUM * 360))"

    echo -n "threads number;" >> $1
    for i in "${ALGORITHMS[@]}"; do
        echo -n "$i;" >> $1
    done
    echo >> $1

    for i in {0..15}; do
        TH="$((2 * $i))"
        if [ $i == 0 ]; then TH=1; fi

        echo -n "$TH;" >> $1
        EARLY_BREAK=0
        for ((a=0;a<ALGORITHMS_NUM;a++)); do
            echo -e "\ttesting with: THREADS=$TH ALGO=$a MATRIX=50x50 ITERATIONS=16000"
            START=`date +%s%N`
            timeout --preserve-status ${TIMEOUT}s bash -c "mpirun -np $TH --oversubscribe ./build/$a.run 50 " &> /dev/null && {
                END=`date +%s%N`
                DURATION="$(($END - $START))"
                echo -n "$DURATION;" >> $1
            } || {
                EARLY_BREAK="$(($EARLY_BREAK + 1))"
                echo -e "\t\ttimeout!"
                echo -n "None;" >> $1
            }
        done
        echo >> $1
        if [[ $EARLY_BREAK == $ALGORITHMS_NUM ]]; then break; fi
    done
    echo >> $1
}


ALGORITHMS=(sequential matrix-matrix )

echo -e "Testing with fixed thread number, different matrix size and fixed iterations number..."
rm -f test/matrix_size.csv 2> /dev/null
test_matrix_size "test/matrix_size.csv"
echo -e "Tested with fixed thread number, different matrix size and fixed iterations number!"

echo -e "Testing with different thread number, fixed matrix size and iterations number..."
rm -f test/threads_number.csv 2> /dev/null
test_threads_number "test/threads_number.csv"
echo -e "Tested with different thread number, fixed matrix size and iterations number!"
