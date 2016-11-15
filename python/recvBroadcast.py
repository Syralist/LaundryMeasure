import socket
import csv
import time

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
sock.setsockopt(socket.SOL_SOCKET, socket.SO_BROADCAST, 1) 
UDP_PORT = 42042
sock.bind(('', UDP_PORT))

timestamp = str(int(time.time()))

with open(timestamp+'.csv', 'wb') as csvfile:
    spamwriter = csv.writer(csvfile)
    while True:
        try:
            data = sock.recv(2048)
            value = float(data)/1000.0
            spamwriter.writerow([int(time.time()), value])
            print value
        except KeyboardInterrupt:
            print "Interupted by user"
            break
