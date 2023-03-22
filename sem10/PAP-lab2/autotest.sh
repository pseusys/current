#!/bin/bash

function test_array_length {
    ALGORITHMS_NUM="${#ALGORITHMS[@]}"
    TIMEOUT="$(($NBEXPERIMENTS * $ALGORITHMS_NUM + 5))"

    echo -n "array size;"
    for i in "${ALGORITHMS[@]}"; do
        echo -n "$i;"
    done
    echo

    for i in {5..25}; do
        echo -n "$i;"
        EARLY_BREAK=0
        for ((a=0;a<ALGORITHMS_NUM;a++)); do
            TH=$(nproc --all)
            echo -e "\ttesting with: ALGO=$ALGO TIMES=$i THREADS=$TH ALGORITHM=$a (${ALGORITHMS[$a]})" 1>&2
            timeout --preserve-status ${TIMEOUT}s bash -c "make -s autorun ALGO=$ALGO TIMES=$i ALGORITHM=$a VERBOSE=0 QUICK_FAIL=1" 2> /dev/null || {
                EARLY_BREAK="$(($EARLY_BREAK + 1))"
                echo -e "\t\ttimeout!" 1>&2
                echo -n "None;"
            }
        done
        echo
        if [[ $EARLY_BREAK == $ALGORITHMS_NUM ]]; then break; fi
    done
    echo
}

function test_threads_number {
    ALGORITHMS_NUM="${#ALGORITHMS[@]}"
    TIMEOUT="$(($NBEXPERIMENTS * $ALGORITHMS_NUM + 5))"

    echo -n "threads number;"
    for i in "${ALGORITHMS[@]}"; do
        echo -n "$i;"
    done
    echo

    for i in {0..10}; do
        THREADS="$((2 ** $i))"
        echo -n "$THREADS;"
        EARLY_BREAK=0
        for ((a=0;a<ALGORITHMS_NUM;a++)); do
            echo -e "\ttesting with: ALGO=$ALGO TIMES=$TIMES THREADS=$THREADS ALGORITHM=$a (${ALGORITHMS[$a]})" 1>&2
            timeout --preserve-status ${TIMEOUT}s bash -c "make -s autorun ALGO=$ALGO THREADS=$THREADS ALGORITHM=$a VERBOSE=0 QUICK_FAIL=1" 2> /dev/null || {
                EARLY_BREAK="$(($EARLY_BREAK + 1))"
                echo -e "\t\ttimeout!" 1>&2
                echo -n "None;"
            }
        done
        echo
        if [[ $EARLY_BREAK == $ALGORITHMS_NUM ]]; then break; fi
    done
}

case "$ALGO" in
    bubble)
        ALGORITHMS=("bubble optimized" "bubble sequential" "bubble parallel")
        ;;

    odd-even)
        ALGORITHMS=("odd-even optimized" "odd-even sequential" "odd-even parallel")
        ;;

    mergesort)
        ALGORITHMS=("mergesort optimized" "mergesort sequential" "mergesort parallel")
        ;;

    quicksort)
        ALGORITHMS=("quicksort parallel" "quicksort sequential")
        ;;

    *)
        echo "Test algorithm '$ALGO' undefined!"
        exit 1;
        ;;
esac

echo -e "\tTesting with fixed process number and different array sizes..." 1>&2
rm -f test/${ALGO}_array_length.csv 2> /dev/null
test_array_length > "test/${ALGO}_array_length.csv"
echo -e "\tTested with fixed process number and different array sizes!" 1>&2

echo -e "\tTesting with fixed array size and different thread numbers..." 1>&2
rm -f test/${ALGO}_threads_number.csv 2> /dev/null
test_threads_number > "test/${ALGO}_threads_number.csv"
echo -e "\tTested with fixed array size and different thread numbers!" 1>&2
