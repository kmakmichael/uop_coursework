#!/usr/bin/env python3

# Project 1: Network Discoverer
# Michael Kmak
# Comp 177
#
# Online interfaces are discovered by sending an ARP packet to the broadcast MAC address.
# replies are then sorted into answers and unresponses, the answers are parsed and printed.
#
# Notes:
# get_mac_addr and get_cird_addr return lists because its possible to have multiple mac and cidr addresses
# ipaddress is a stdlib package, shouldn't need any extra installs
# haven't tested this in pycharm, can't figure out sudo perms required

from scapy.all import *
import netifaces
from ipaddress import IPv4Interface


# function definitions
def list_interfaces():
    print("Interfaces")
    for iface in netifaces.interfaces():
        print("*", iface)
    print("------------")


def get_mac_addr(family):
    addrs = []
    for entry in family:
        addrs.append(entry["addr"])
    return addrs


def get_cidr_addr(family):
    addrs = []
    for entry in family:
        netmask = "/24" if "netmask" not in entry else entry["netmask"]
        addrs.append(IPv4Interface(entry["addr"] + "/" + netmask).with_prefixlen)
    return addrs


def iface_details():
    print("Interface details:")
    for iface in netifaces.interfaces():
        addr_families = netifaces.ifaddresses(iface)
        print(iface, ":\t",
              "MAC=", ', '.join(get_mac_addr(addr_families[netifaces.AF_LINK])), "\t",
              "IP=", ', '.join(get_cidr_addr(addr_families[netifaces.AF_INET])))
    print("------------")


def scan_interfaces():
    interfaces = netifaces.interfaces()
    interfaces.remove('lo')
    for iface in interfaces:
        print("Scanning on interface", iface)
        print("Results:")
        for addr in get_cidr_addr(netifaces.ifaddresses(iface)[netifaces.AF_INET]):
            # thank you scapy docs
            ans, unans = srp(scapy.layers.l2.Ether(dst="ff:ff:ff:ff:ff:ff") / scapy.layers.l2.ARP(pdst=addr), timeout=2)
            print("Hosts responding:", len(ans))
            print("Hosts not responding:", len(unans))
            ans.summary(lambda s, r: r.sprintf("MAC=%Ether.src%\tIP=%ARP.psrc%"))
        print("------------")


# begin execution
list_interfaces()
iface_details()
scapy.config.conf.verb = 0
scan_interfaces()
