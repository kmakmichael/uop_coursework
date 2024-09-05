#!/usr/bin/env python3

# Project 2: Traceroute
# Michael Kmak
# Comp 177
#
# usage: sudo ./traceroute.py <hostname/ip address>
#
# uses scapy to create and send UDP packets with increasing lifetimes. The ICMP timeout replies are used to
# trace a route from the local host to the given remote host. Each probe is sent to a destination port
# between 3343 and 33464, and sent from a source port between 500000 and 599999. socket is used to get the
# hostnames of each step, and probing stops when it reaches the destination.
#

from scapy.all import *
import sys
import socket
import random

# constants
MAX_HOPS = 20
MAX_PROBES = 1


# function definitions
def create_packet(ttl):
    dport = random.choice(range(33434, 33464))
    sport = random.choice(range(50000, 59999))
    return scapy.layers.inet.IP(dst=ip_addr, ttl=ttl) / scapy.layers.inet.UDP(dport=dport, sport=sport)


# begin execution
if __name__ == "__main__":
    scapy.config.conf.verb = 0
    if len(sys.argv) != 2:
        print("usage: traceroute.py <Hostname/IP address>")
        sys.exit()
    try:
        ip_addr = socket.gethostbyname(sys.argv[1])
    except socket.error as e:
        print("Error: Invalid hostname or IP address")
        sys.exit()

    print("traceroute to {} ({})".format(sys.argv[1], ip_addr))

    for i in range(1, MAX_HOPS):
        reply = scapy.sendrecv.sr1(create_packet(i), timeout=2)
        if reply is not None:
            try:
                hostname = socket.gethostbyaddr(reply[scapy.layers.inet.IP].src)[0]
            except socket.error as e:
                hostname = ""
            print("{hop:>3}  {hostname} ({ip})"
                  .format(
                        hop=i,
                        hostname=hostname,
                        ip=reply[scapy.layers.inet.IP].src))
            if reply[scapy.layers.inet.ICMP].type == 3 or reply[scapy.layers.inet.ICMP].code == 3:
                break
        else:
            print("{hop:>3}  *".format(hop=i))
