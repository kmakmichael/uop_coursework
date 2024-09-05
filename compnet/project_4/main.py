#!/usr/bin/env python3
#
# referenced:
#   https://gist.github.com/joncardasis/cc67cfb160fa61a0457d6951eff2aeae
#
import os
import socket
import sys
import re
import threading
import argparse
import time
import signal
import mimetypes

verbose = False
all_threads = []


class ServerThread(threading.Thread):
    def __init__(self, accept, base, recv_len):
        threading.Thread.__init__(self)
        (self.s, self.addr) = accept
        self.base = base
        self.recv_len = recv_len

        log("Accepted incoming connection from client")
        log("Client IP, Port = %s" % str(self.addr))

    def run(self):
        start_time = time.time()
        while True:
            # Receive data
            string_unicode = self.recv()
            log(f'Recieved from client:\n{string_unicode}=========\n')
            # send response
            request = string_unicode.split(' ')
            if len(request) > 2:
                self.reply(request)

    def recv(self):
        request = ""
        while not re.search("\r\n\r\n", request):
            try:
                raw_bytes = self.s.recv(self.recv_len)
            except socket.error as msg:
                log("Error: unable to recv()")
                log("Description: " + str(msg))
                sys.exit()
            request += raw_bytes.decode('ascii')
            log(f'request: \n{request}')
            if not request:
                self.shutdown()
        return request

    def reply(self, request):
        path = request[1]
        response_header = ''
        if request[0] == 'GET':
            try:
                self.send_file(f'{self.base}{path}')
            except Exception as msg:
                # send simple 404 page
                response_data = b"<html><body><center><h1>404: File not found</h1></center></body></html>"
                response_header = create_header(404)
                response_header += f'Content-Type: text/html\n'
                response_header += f'Content-Length: {len(response_data)}\n'
                self.s.send(response_header.encode() + b'\n' + response_data)
                log(str(msg))
        elif request[0] == 'HEAD':
            response_header = create_header(200)
            self.s.send(response_header.encode() + b'\n')
        else:
            # send simple 501 page
            response_data = b"<html><body><center><h1>501: Not Implemented</h1></center></body></html>"
            response_header = create_header(501)
            response_header += f'Content-Type: text/html\n'
            response_header += f'Content-Length: {len(response_data)}\n'
            self.s.send(response_header.encode() + b'\n' + response_data)

    def send_file(self, file):
        log(f'sending {file}:')
        f = open(file, 'rb')
        # send header
        response_header = create_header(200)
        (filetype, encoding) = mimetypes.guess_type(file)
        response_header += f'Content-Type: {filetype}\n'
        response_header += f'Content-Length: {os.path.getsize(file)}\n'
        self.s.send(response_header.encode() + b'\n')
        log(f'Sending ({os.path.getsize(file)}b)\n======\n{file}:\n{response_header}\n\n=====\n\n')
        # send data
        response_data = f.read(64 * 1024)
        while response_data:
            log(f'Sending {len(response_data)} bytes...')
            self.s.send(response_data)
            response_data = f.read(64 * 1024)

    def shutdown(self):
        try:
            self.s.close()
        except socket.error as msg:
            log("Error: unable to close() socket")
            log("Description: " + str(msg))
            sys.exit()
        log(f'Closing socket on port {self.addr}')


def main():
    parser = argparse.ArgumentParser(description='Simple Python HTTP Server')
    parser.add_argument('--base', action="store", dest="base", help="website directory")
    parser.add_argument('--port', action="store", dest="port", type=int, default=8080, help="port to run on")
    parser.add_argument('--recv', action="store", dest="recv_size", type=int, default=64*1024, help="packet body size")
    parser.add_argument('--verbose', action="store_true", dest="verbose", help="increase program verbosity")
    args = parser.parse_args()
    base = args.base
    port = args.port
    global verbose
    verbose = args.verbose
    recv_size = args.recv_size

    # Create TCP socket
    try:
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    except socket.error as msg:
        log("Error: could not create socket")
        log("Description: " + str(msg))
        sys.exit()

    # Bind to listening port
    try:
        host = ''
        s.bind((host, port))
    except socket.error as msg:
        log("Error: unable to bind on port %d" % port)
        log("Description: " + str(msg))
        sys.exit()

    log("Listening socket bound to port %d" % port)

    # Listen
    try:
        backlog = 10
        s.listen(backlog)
    except socket.error as msg:
        log("Error: unable to listen()")
        log("Description: " + str(msg))
        sys.exit()

    global all_threads
    while True:
        try:
            all_threads.append(ServerThread(s.accept(), base, recv_size))
            all_threads[-1].start()
        except socket.error as msg:
            log("Error: unable to accept()")
            log("Description: " + str(msg))
            server_shutdown(s)


def create_header(status_code):
    header = f'HTTP/1.1 {status_code} '
    if status_code == 200:
        header += 'OK\n'
    elif status_code == 206:
        header += 'Partial Content\n'
    elif status_code == 404:
        header += 'Not Found\n'
    elif status_code == 501:
        header += 'Not Implemented\n'
    date = time.strftime("%a, %d %b %Y %H:%M:%S %Z", time.gmtime())
    expiry = time.strftime("%a, %d %b %Y %H:%M:%S %Z", time.gmtime(time.time() + 12 * 60 * 60))
    header += f'Date: {date}\n'
    header += 'Server: Python HTTP\n'
    header += 'Connection: keep-alive\n'
    header += f'Expires: {expiry}\n'
    return header


def server_shutdown(s):
    log("waiting for threads to finish")
    for thr in all_threads:
        thr.join()
    log("all threads finished")

    try:
        s.close()
    except socket.error as msg:
        log("Error: unable to close() socket")
        log("Description: " + str(msg))
        sys.exit()

    log("Sockets closed, now exiting")
    sys.exit()


def log(output):
    global verbose
    if verbose:
        print(output)


if __name__ == '__main__':
    sys.exit(main())


