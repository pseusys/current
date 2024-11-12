.ONESHELL:
.DEFAULT_GOAL := help

SHELL=/bin/bash
.SHELLFLAGS=-c -e

BLUE=\033[34m
GREEN=\033[32m
RED=\033[31m
RESET=\033[0m

ATTACK_TYPE=SYMLINK


id_rsa:
	@ # Generate key for VM access and set it up
	echo -e "${BLUE}Generating SSH key for passwordless access...${RESET}"
	ssh-keygen -f ./$@ -N "" > /dev/null
	echo "${PASSWORD}" | ssh-copy-id -i ./id_rsa.pub -p 5555 user@localhost > /dev/null
	rm -f id_rsa.pub > /dev/null
	echo -e "\t${GREEEN}Passwordless access set up!${RESET}"


up:
	@ # Start VM (in a separate terminal or in the background)
	qemu-system-x86_64 -hda disk.img -m 2G -nic user,hostfwd=tcp::5555-:22 -nographic
.PHONY: up

build: id_rsa
	@ # Clear and rebuild everything in VM
	echo -e "${BLUE}Clearing build files in VM...${RESET}"
	ssh -i ./id_rsa -p 5555 user@localhost "rm -rf test" > /dev/null
	echo -e "${BLUE}Copying sources into VM...${RESET}"
	scp -i ./id_rsa -rpP 5555 test user@localhost:/home/user > /dev/null
	echo -e "${BLUE}Updating timestamps of the copied files...${RESET}"
	ssh -i ./id_rsa -p 5555 user@localhost "find ./test/ -type f -exec touch {} +"
.PHONY: build


problem2: build
	@ # Execute problem2
	echo -e "${BLUE}Running problem2...${RESET}"
	ssh -i ./id_rsa -p 5555 user@localhost "cd test/problem2 && python3 run_exploit.py"
.PHONY: problem2

problem4: build
	@ # Execute problem4
	echo -e "${BLUE}Running problem4...${RESET}"
	ssh -i ./id_rsa -p 5555 user@localhost "export ATTACK_TYPE=${ATTACK_TYPE} && cd test/problem4 && python3 run_exploit.py"
.PHONY: problem4

report:
	@  # Generate PDF file from the markdown
	pandoc -f markdown -t pdf -o assignment.pdf assignment.md
	zip -r problems_code.zip test > /dev/null
.PHONY: report
