#!/usr/bin/env python3
#
# referenced:
#   https://gist.github.com/joncardasis/cc67cfb160fa61a0457d6951eff2aeae
#

import socket
import sys
import argparse


def main():
    parser = argparse.ArgumentParser(description='Simple Python HTTP Server')
    parser.add_argument('--base', action="store", dest="base", help="website directory")
    parser.add_argument('--port', action="store", dest="port", type=int, default=8080, help="port to run on")
    args = parser.parse_args()
    base = args.base
    port = args.port

    # Create TCP socket
    try:
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    except socket.error as msg:
        # print("Error: could not create socket")
        # print("Description: " + str(msg))
        sys.exit()

    # Bind to listening port
    try:
        host = ''
        s.bind((host, port))
    except socket.error as msg:
        # print("Error: unable to bind on port %d" % port)
        # print("Description: " + str(msg))
        sys.exit()

    # print("Listening socket bound to port %d" % port)

    # Listen
    try:
        backlog = 10
        s.listen(backlog)
    except socket.error as msg:
        # print("Error: unable to listen()")
        # print("Description: " + str(msg))
        sys.exit()

    while True:
        try:
            (client_s, client_addr) = s.accept()
        except socket.error as msg:
            # print("Error: unable to accept()")
            # print("Description: " + str(msg))
            sys.exit()

        # print("Accepted incoming connection from client")
        # print("Client IP, Port = %s" % str(client_addr))

        # Receive data
        try:
            buffer_size = 4096
            raw_bytes = client_s.recv(buffer_size)
        except socket.error as msg:
            # print("Error: unable to recv()")
            # print("Description: " + str(msg))
            sys.exit()

        string_unicode = raw_bytes.decode('ascii')
        # print("Received %d bytes from client" % len(raw_bytes))
        # print("Message contents: %s" % string_unicode)

        # send response
        request = string_unicode.split(' ')
        if len(request) > 2:
            path = request[1]
            response_header = ''
            if request[0] == 'GET':
                try:
                    file = open(f'{base}{path}', 'rb')
                    response_header = create_header(200)
                    response_data = file.read()
                except Exception as msg:
                    response_header = create_header(404)
                    # send simple 404 page
                    response_data = b"<html><body><center><h1>404: File not found</h1></center></body></html>"
                    # print(str(msg))
            else:
                response_header = create_header(501)
                # send simple 501 page
                response_data = b"<html><body><center><h1>501: Not Implemented</h1></center></body></html>"

            # print(f'Sending {path}')

            response = response_header.encode() + response_data
            client_s.send(response)
        try:
            client_s.close()
        except socket.error as msg:
            # print("Error: unable to close() socket")
            # print("Description: " + str(msg))
            sys.exit()
        # print('\n=======================================\n')

    try:
        s.close()
    except socket.error as msg:
        # print("Error: unable to close() socket")
        # print("Description: " + str(msg))
        sys.exit()

    # print("Sockets closed, now exiting")


def create_header(status_code):
    header = f'HTTP/1.1 {status_code} '
    if status_code == 200:
        header += 'OK\n'
    elif status_code == 404:
        header += 'Not Found\n'
    elif status_code == 501:
        header += 'Not Implemented\n'
    header += 'Server: Python HTTP\n'
    return header + '\n'


if __name__ == '__main__':
    sys.exit(main())


