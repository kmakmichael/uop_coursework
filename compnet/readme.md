# Computer Networking
Work for UOP's Computer Networking course.

### Project 1: Network Discoverer
Online interfaces are discovered by sending an ARP packet to the broadcast MAC address.
Replies are then sorted into answers and unresponses, the answers are parsed and printed.

### Project 2: Traceroute
Traceroute program using scapy.

### Project 3: Web Server
Serves basic static websites via simple HTTP requests.
Replies either with content, a 404, or a 501.

### Project 4: Web Server (Multi-Threaded)
Same as Project 3, but with multiple threads.
Python doesn't really occupy multiple processor threads, so this was more of a learning exercise than a practical one.

### Project 5: Port Scanner
Scans for open ports within a given range at a given target.
Uses Scapy to generate and send the packets. Can specify a tcp or udp scan.
