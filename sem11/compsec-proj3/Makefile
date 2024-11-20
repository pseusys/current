CGO_ENABLED=1


SCAN_HOST=scanme.nmap.org
PCAP_FILE=sample.pcap


run_nmap:
	docker run --rm -it instrumentisto/nmap -sS -A -T4 -p1-65535 ${SCAN_HOST}
.PHONY: run_nmap



compile_part3:
	go -C part3 mod tidy
.PHONY: compile_part3

debug_part3: compile_part3
	go -C part3 run -ldflags="-X main.verbose=true" detector.go ${PCAP_FILE}
.PHONY: debug_part3

run_part3: compile_part3
	go -C part3 run detector.go ${PCAP_FILE}
.PHONY: run_part3



compile_part4:
	go -C part4 mod tidy
.PHONY: compile_part4

client_part4:
	bash part4/run_client.sh
.PHONY: client_part4

start_part4:
	bash part4/start_images.sh
.PHONY: start_part4

stop_part4:
	bash part4/stop_images.sh
.PHONY: stop_part4



submission:
	tar cvzf comp4634_proj3_submission.tar.gz part1/PortScanAnswers.* part2/WiresharkAnswers.* part3/detector.go part4/mitm.go
.PHONY: submission
