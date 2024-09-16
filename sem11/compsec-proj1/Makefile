.ONESHELL:
.DEFAULT_GOAL := help

SHELL=/bin/bash
.SHELLFLAGS=-c -e

BLUE=\033[34m
GREEN=\033[32m
RED=\033[31m
RESET=\033[0m


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

brun: id_rsa clear_build
	@ # Clear and rebuild everything in VM
	echo -e "${BLUE}Clearing build files in VM...${RESET}"
	ssh -i ./id_rsa -p 5555 user@localhost "rm -rf proj1" > /dev/null
	echo -e "${BLUE}Copying sources into VM...${RESET}"
	scp -i ./id_rsa -rpP 5555 proj1 user@localhost:/home/user > /dev/null
	echo -e "${BLUE}Updating timestamps of the copied files...${RESET}"
	ssh -i ./id_rsa -p 5555 user@localhost "find ./ -type f -exec touch {} +"
	echo -e "${BLUE}Building and installing targets...${RESET}"
	ssh -i ./id_rsa -p 5555 user@localhost "cd proj1/targets && make && echo 'comp4634' | sudo -S make install" &> /dev/null
	echo -e "${BLUE}Building exploits...${RESET}"
	ssh -i ./id_rsa -p 5555 user@localhost "cd proj1/xploits && make" > /dev/null
.PHONY: brun


%.xpl: brun
	@ # Build and run an exploit
	echo -e "${BLUE}Running $* and testing output...${RESET}"
	if [ $$(ssh -i ./id_rsa -p 5555 user@localhost "echo 'whoami' | proj1/xploits/$*") = "root" ]; then
		echo -e "\t${GREEN}Exploit successful!!${RESET}"
		exit 0
	else
		echo -e "\t${RED}Exploit failed :(${RESET}"
		exit 1
	fi
.PHONY: %.xpl

xploit1: xploit1.xpl
.PHONY: xploit1

xploit2: xploit2.xpl
.PHONY: xploit2

xploit3: xploit3.xpl
.PHONY: xploit3


clear_build:
	@ # Clear all the build files locally
	rm -f proj1/targets/target1 proj1/targets/target2 proj1/targets/target3 proj1/targets/target4 proj1/targets/target5
	rm -f proj1/xploits/xploit1 proj1/xploits/xploit2 proj1/xploits/xploit3 proj1/xploits/xploit4 proj1/xploits/xploit5
	rm -f proj1/xploits/xploit1.o proj1/xploits/xploit2.o proj1/xploits/xploit3.o proj1/xploits/xploit4.o proj1/xploits/xploit5.o
	rm -f proj1/xploits/run-shellcode proj1/xploits/run-shellcode.o proj1/xploits/shellcode.bin
.PHONY: clear_build

clear: clear_build
	@ # Clear all the generated files
	rm -f id_rsa.pub id_rsa
.PHONY: clear
