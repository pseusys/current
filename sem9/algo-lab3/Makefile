.ONESHELL:

EXAMPLE_FILES := $(words $(wildcard examples/example_*.txt))
ALGORITHM_FILE = substracting_max_flow

PATH := venv/bin:$(PATH)


venv:
	@
	python3 -m venv venv
	pip install --upgrade pip
	pip install --upgrade pip PyMaxflow~=1.3.0


print_test_%: examples/example_%.txt venv
	@
	python3 $(ALGORITHM_FILE).py < $<
.PHONY: print_test_%

run_test_%: examples/answers.txt venv
	@
	EXPECTED=$(shell cat $< | head -$* | tail -1)
	ACTUAL=$(shell make --no-print-directory -s print_test_$*)
	if [ $$EXPECTED != $$ACTUAL ]; then
		echo "\t\e[31;1mTest of example $* failed!\e[0m" && exit 1
	else
		echo "\t\e[32;1mTest of example $* passed!\e[0m"
	fi
.PHONY: run_test_%

run_tests: venv
	@
	$(foreach ex,$(shell seq 1 1 $(EXAMPLE_FILES)),make --no-print-directory -s run_test_$(ex);)
.PHONY: run_tests
