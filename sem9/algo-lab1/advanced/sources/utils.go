package main

import (
	crand "crypto/rand"
	"encoding/binary"
	"fmt"
	rand "math/rand"
	"os"
	"strings"
)

func LogF(format string, a ...any) {
	if *verbose {
		fmt.Printf(format, a...)
	}
}

func LogE(message string, err error) {
	fmt.Printf("%s: %v\n", message, err)
	os.Exit(1)
}

func RandUint() (v uint) {
	err := binary.Read(crand.Reader, binary.BigEndian, &v)
	if err != nil {
		v = uint(rand.Int())
	}
	return v
}

func JoinError(message string, errs ...any) error {
	traces := make([]string, len(errs))
	for i := range errs {
		traces[i] = fmt.Sprint(errs[i])
	}
	return fmt.Errorf("%s: %v", message, strings.Join(traces, ", "))
}
