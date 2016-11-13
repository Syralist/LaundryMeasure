import socket

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

sock.setsockopt(socket.SOL_SOCKET, socket.SO_BROADCAST, 1) 

UDP_PORT = 42042

# using the '' address works
sock.bind(('', UDP_PORT))
# using the address of eth1 doesn't
#sock.bind(('192.168.2.123', UDP_PORT))
# and neither does using the local loopback
#sock.bind(('127.0.0.1', UDP_PORT))

recording = False

while True:
    try:
        data = sock.recv(2048)
        value = float(data)/1000.0
        print value
    except KeyboardInterrupt:
        print "Interupted by user"
        break
