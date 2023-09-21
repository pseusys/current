#!/bin/bash

function test_matrix_size {
    touch $1
    ALGORITHMS_NUM="${#ALGORITHMS[@]}"
    TIMEOUT="$(($ALGORITHMS_NUM * 360))"

    echo -n "matrix size;" >> $1
    for i in "${ALGORITHMS[@]}"; do
        echo -n "$i;" >> $1
    done
    echo >> $1

    TH=$(nproc --all)
    for i in {2..15}; do
        MT="$((2 ** $i))"

        echo -n "$MT;" >> $1
        EARLY_BREAK=0
        for ((a=0;a<ALGORITHMS_NUM;a++)); do
            echo -e "\ttesting with: THREADS=$TH ALGO=${ALGORITHMS[$a]} MATRIX=${MT}"
            START=`date +%s%N`
            timeout --preserve-status ${TIMEOUT}s bash -c "mpirun -np $TH --oversubscribe ./build/${ALGORITHMS[$a]}.run $MT" &> /dev/null && {
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

    for i in {1..20}; do
        echo -n "$i;" >> $1
        EARLY_BREAK=0
        for ((a=0;a<ALGORITHMS_NUM;a++)); do
            echo -e "\ttesting with: THREADS=$i ALGO=${ALGORITHMS[$a]} MATRIX=40"
            START=`date +%s%N`
            timeout --preserve-status ${TIMEOUT}s bash -c "mpirun -np $i --oversubscribe ./build/${ALGORITHMS[$a]}.run 40" &> /dev/null && {
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


ALGORITHMS=("sequential" "matrix-vector" "matrix-matrix")

echo -e "Testing with fixed thread number and different matrix size..."
rm -f test/matrix_size.csv 2> /dev/null
test_matrix_size "test/matrix_size.csv"
echo -e "Tested with fixed thread number and different matrix size!"

echo -e "Testing with different thread number and fixed matrix size..."
rm -f test/threads_number.csv 2> /dev/null
test_threads_number "test/threads_number.csv"
echo -e "Tested with different thread number and fixed matrix size!"
