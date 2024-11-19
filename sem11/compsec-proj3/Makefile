CGO_ENABLED=1


SCAN_HOST=scanme.nmap.org
PCAP_FILE=sample.pcap


run_nmap:
	docker run --rm -it instrumentisto/nmap -sS -A -T4 -p1-65535 ${SCAN_HOST}
.PHONY: run_nmap



compile_go:
	go -C part3 mod tidy
.PHONY: compile_go

debug_go: compile_go
	go -C part3 run -ldflags="-X main.verbose=true" detector.go ${PCAP_FILE}
.PHONY: debug_go

run_go: compile_go
	go -C part3 run detector.go ${PCAP_FILE}
.PHONY: run_go



submission:
	tar cvzf comp4634_proj3_submission.tar.gz part1/PortScanAnswers.* part2/WiresharkAnswers.* part3/detector.go part4/mitm.go part4/README.md
.PHONY: submission
