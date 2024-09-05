#!/usr/bin/env python3
#
# Project 5: Port Scanner
# Michael Kmak
# Comp 177
#
#   Scans for open ports within a given range at a given target. Uses
# Scapy to generate and send the packets. Can specify a tcp or udp scan.
#
# Program will parse the port or port range and scan whether each port
# is open. If a range is given, ports will be scanned in a random order.
# Stealth scanning is used for TCP ports. UDP ports are sent dummy app
# data - with the exception of port 53, to which a DNS request is sent -
# Ports without a response will be identified as "Filtered|Open, because
# this method might miss applications that don't respond.


import sys
import argparse
from scapy.all import *

verbose = False


def log(output):
    global verbose
    if verbose:
        print(output)


def main():
    # disable scapy output
    scapy.config.conf.verb = 0

    # argparse
    parser = argparse.ArgumentParser(description='simple Python Port Scanner')
    parser.add_argument('--tcp', action="store_true", dest="tcp", help="run using TCP (mutually exclusive with --udp)")
    parser.add_argument('--udp', action="store_true", dest="udp", help="run using UDP (mutually exclusive with --tcp)")
    parser.add_argument('--port', action="store", dest="port", default="0-1023", help="port or port range to scan")
    parser.add_argument('--target', action="store", dest="target", default="127.0.0.1", help="IP address or hostname of the target machine")
    parser.add_argument('--verbose', action="store_true", dest="verbose", help="display all scanned ports, not just open ones")
    args = parser.parse_args()
    scan_tcp = args.tcp
    scan_udp = args.udp
    port = args.port
    target = args.target
    global verbose
    verbose = args.verbose
    if scan_tcp and scan_udp:
        print("--tcp and --udp are mutually exclusive! please specify only one")
        sys.exit()

    # process the inputs
    (ip, port_range) = process_inputs(target, port)
    # randomize scanning order
    random.shuffle(port_range)

    sct = "udp" if scan_udp else "tcp"
    print(f'Scan type: {sct}')
    print(f'Target: {target}')
    print(f'Port(s): {port}')
    print(f'{sct.upper()} Scanning...')

    # perform scan
    if scan_udp:
        scapy_udp(ip, port_range)
    else:
        scapy_tcp(ip, port_range)


# in: target address and ports
# out: reformatted addr and ports
# ex:
# process_inputs(dns.google, 79-81)
# --> ("8.8.8.8", [79, 80, 81])
def process_inputs(target, port):
    ip = "127.0.0.1"
    port_range = ["1"]

    # process IP
    try:
        ip = socket.gethostbyname(target)
    except socket.error as e:
        print("Error: Invalid hostname or IP address")
        sys.exit()

    # process port
    try:
        port_range = [int(port)]
    except ValueError:
        try:
            port_range = port.split("-")
            if len(port_range) > 2:
                print("invalid input! ports must be in format \"start-end\"")
                sys.exit()
            port_range = [*range(int(port_range[0]), int(port_range[1]) + 1)]
        except Exception as e:
            print(f'invalid input: {e}')
            sys.exit()
    except Exception as e:
        print(f'invalid input: {e}')
        sys.exit()

    # print(f'post-processing: ports {port_range} at {ip}')
    return ip, port_range


# performs UDP scan
def scapy_udp(ip, ports):
    scans = []
    for p in ports:
        if p == 53:
            query = scapy.layers.dns.DNSQR(qname="https://castlecyberskull.neocities.org/")
            pac = scapy.layers.inet.IP(dst=ip) / scapy.layers.inet.UDP(dport=p) / scapy.layers.dns.DNS(rd=1, qd=query)
            reply = scapy.sendrecv.sr1(pac, timeout=1)
        else:
            pac = scapy.layers.inet.IP(dst=ip) / scapy.layers.inet.UDP(dport=p)
            pac.add_payload(b'udp port scan')
            reply = scapy.sendrecv.sr1(pac, timeout=1)
        if not reply:
            scans.append({
                "port": p,
                "status": "Open|Filtered",
                "reason": "No Response"
            })
        elif scapy.layers.inet.ICMP in reply.layers():
            scans.append({
                "port": p,
                "status": "Closed",
                "reason": "Received ICMP Unreachable"
            })
        else:
            scans.append({
                "port": p,
                "status": "Open",
                "reason": "Received Response"
            })
    results_display(scans)


# performs TCP scan
def scapy_tcp(ip, ports):
    scans = []
    for p in ports:
        pac = scapy.layers.inet.IP(dst=ip) / scapy.layers.inet.TCP(dport=p, flags="S", seq=100)
        reply = scapy.sendrecv.sr1(pac, timeout=1)
        if not reply:
            scans.append({
                "port": p,
                "status": "Closed",
                "reason": "No Response"
            })
        else:
            scans.append({
                "port": p,
                "status": "Open",
                "reason": "Received Response"
            })
    results_display(scans)


# sorts the given list of scan data and displays it accordingly
def results_display(scans):
    global verbose
    scans.sort(key=sortfunc)
    for s in scans:
        if s['status'] == "Open":
            print(f'Port: {str(s["port"])}        Status: {s["status"]}        Reason: {s["reason"]}')
        elif verbose:
            print(f'Port: {str(s["port"])}        Status: {s["status"]}        Reason: {s["reason"]}')


# sort function for the scan data list
def sortfunc(item):
    return item['port']


#################
# Deprecated - Attempt at using raw sockets instead of scapy
#################
def tcp_scan(ip, ports):
    for p in ports:
        # Create TCP socket
        try:
            s = socket.socket(socket.AF_INET, socket.SOCK_RAW, socket.IPPROTO_TCP)
        except socket.error as e:
            print("Error: could not create socket")
            print("Description: " + str(e))
            sys.exit()

        try:
            pac = scapy.layers.inet.IP(dst=ip) / scapy.layers.inet.TCP(sport=random.choice(range(50000, 59999)), dport=p, seq=100)
            print(str(pac))
            pac.show2()
            s.sendto(bytes(pac), (ip, p))
        except socket.error as e:
            print("Error: Could not open connection")
            print("Description: " + str(e))
            sys.exit()
        try:
            s.recv(2 * 1024)
        except socket.error as e:
            print("error: " + str(e))


def udp_scan(ip, ports):
    for p in ports:
        # Create UDP socket
        try:
            s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM, socket.IPPROTO_UDP)
        except socket.error as e:
            print("Error: could not create socket")
            print("Description: " + str(e))
            sys.exit()

        try:
            pac = scapy.layers.inet.IP(dst=ip) / scapy.layers.inet.UDP(dport=p)
            pac.add_payload(b'udp port scan')
            # print(pac.do_build())
            s.sendto(pac.do_build(), (ip, p))
        except socket.error as e:
            print("Error: Could not send UDP packet")
            print("Description: " + str(e))
            sys.exit()

        try:
            s.recv(2 * 1024)
        except socket.error as e:
            print("did not receive")
            print("Description: " + str(e))
            sys.exit()

if __name__ == '__main__':
    sys.exit(main())
