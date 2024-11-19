/*
 * HKUST COMP4634 Project 3
 * Part 3. Anomaly Detection
 *
 * detector.go
 *
 * When completed (by you!) and compiled, this program will:
 *
 *  - Open a .pcap file supplied as a command-line argument, and analyze the TCP,
 *    IP, Ethernet, and ARP layers
 *
 *  - Print the IP addresses that: 1) sent more than 3 times as many SYN packets
 *    as the number of SYN+ACK packets they received, and 2) sent more than 5 SYN
 *    packets in total
 *
 *  - Print the MAC addresses that send more than 5 unsolicited ARP replies
 *
 * This starter code is provided solely for convenience, to help build
 * familiarity with Go. You are free to use as much or as little of this code
 * as you see fit.
 */

package main

import (
	// You may use any packages in gopacket or the Go standard library,
	// but we think these should be sufficient. You MAY NOT use any
	// third party libraries. The autograder will not build with these.

	"fmt"
	"net"
	"os"

	"github.com/google/gopacket"
	"github.com/google/gopacket/layers"
	"github.com/google/gopacket/pcap"
)

var verbose = "false"

func log(format string, a ...any) {
	if verbose == "true" {
		fmt.Fprintf(os.Stderr, format, a...)
	}
}

var (
	addresses   = map[string][2]int{}
	arpRequests = map[string]map[string]int{}
	arpMac      = map[string]int{}
)

func handleSynAck(ipReceiver *net.IP) {
	// Get receiver IP address
	receiverName := ipReceiver.String()
	res, ok := addresses[receiverName]
	// Add 1 to his received packet count
	if ok {
		res[1] += 1
	} else {
		res = [2]int{0, 1}
	}
	addresses[receiverName] = res
	log("SYN+ACK: %s (%d, %d)\n", ipReceiver, res[0], res[1])
}

func handleSyn(ipSender *net.IP) {
	// Get sender IP address
	senderName := ipSender.String()
	res, ok := addresses[senderName]
	// Add 1 to his sent packet count
	if ok {
		res[0] += 1
	} else {
		res = [2]int{1, 0}
	}
	addresses[senderName] = res
	log("SYN: %s (%d, %d)\n", ipSender, res[0], res[1])
}

func handleArpReq(ipSender *net.IP, ipRequested *net.IP) {
	// Get sender and requested IP addresses
	senderName := ipSender.String()
	requestedName := ipRequested.String()
	resf, ok := arpRequests[senderName]
	// Record that sender IP is expecting a response from the requested IP
	if ok {
		ress, ok := resf[requestedName]
		if ok {
			ress += 1
		} else {
			ress = 1
		}
		resf[requestedName] = ress
	} else {
		arpRequests[senderName] = map[string]int{requestedName: 1}
	}
	log("ARP: %s asked %s (%d)\n", ipSender, ipRequested, arpRequests[senderName][requestedName])
}

func handleArpRes(ipSender *net.IP, ipResponse *net.IP, macSender *net.HardwareAddr) {
	isUnsolicited := false
	// Get receiver and response IP addresses, and also sender MAC address
	senderName := ipSender.String()
	responseName := ipResponse.String()
	senderHWName := macSender.String()
	resf, ok := arpRequests[responseName]
	// Record that receiver is not anymore expecting a response from the response IP, otherwise, mark response as unsolicited
	if ok {
		ress, ok := resf[senderName]
		if ok && ress > 0 {
			resf[senderName] = ress - 1
			log("ARP: %s replied to %s (%d)\n", ipResponse, ipSender, arpRequests[responseName][senderName])
		} else {
			isUnsolicited = true
		}
	} else {
		isUnsolicited = true
	}
	// If the response is unsolicited, record the sender MAC address
	if isUnsolicited {
		res, ok := arpMac[senderHWName]
		if ok {
			arpMac[senderHWName] = res + 1
		} else {
			arpMac[senderHWName] = 1
		}
		log("Unsolicited ARP: from %s to %s (%s): %d\n", ipResponse, ipSender, macSender, arpMac[senderHWName])
	}
}

func main() {
	if len(os.Args) != 2 {
		panic("Invalid command-line arguments")
	}
	pcapFile := os.Args[1]

	if handle, err := pcap.OpenOffline(pcapFile); err != nil {
		panic(fmt.Sprintf("Could not find or load input file %s: %v", pcapFile, err))
	} else {
		packetSource := gopacket.NewPacketSource(handle, handle.LinkType())

		for packet := range packetSource.Packets() {
			tcpLayer := packet.Layer(layers.LayerTypeTCP)
			ipLayer := packet.Layer(layers.LayerTypeIPv4)
			etherLayer := packet.Layer(layers.LayerTypeEthernet)
			arpLayer := packet.Layer(layers.LayerTypeARP)

			if tcpLayer != nil && ipLayer != nil && etherLayer != nil {

				// Extract IP and TCP packets data
				ipData, _ := ipLayer.(*layers.IPv4)
				tcpData, _ := tcpLayer.(*layers.TCP)

				// Handle packet depending on its type
				if tcpData.SYN && tcpData.ACK {
					handleSynAck(&ipData.DstIP)
				} else if tcpData.SYN {
					handleSyn(&ipData.SrcIP)
				}

			} else if arpLayer != nil {

				// Extract ARP packet data
				arp, _ := arpLayer.(*layers.ARP)

				// Extract sender and destination IP, also sender MAC
				ipSender := net.IP(arp.SourceProtAddress)
				ipDestination := net.IP(arp.DstProtAddress)
				macSender := net.HardwareAddr(arp.SourceHwAddress)

				// Handle packet depending on its type
				if arp.Operation == 1 {
					handleArpReq(&ipSender, &ipDestination)
				} else if arp.Operation == 2 {
					handleArpRes(&ipSender, &ipDestination, &macSender)
				}
			}
		}

		fmt.Println("\nUnauthorized SYN scanners:")
		for ip, addr := range addresses {
			if addr[0] > 5 && addr[1]*3 < addr[0] {
				fmt.Println(ip)
			}
		}

		fmt.Println("\nUnauthorized ARP spoofers:")
		for mac, count := range arpMac {
			if count > 5 {
				fmt.Println(mac)
			}
		}
	}
}
