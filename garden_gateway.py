import socket
import os
from dotenv import load_dotenv
import threading

load_dotenv()
HOST = os.getenv('HOST_IP', '127.0.0.1')
PORT = int(os.getenv('PORT_NUM', '30000'))

# Create a TCP/IP socket
server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

# Bind the socket to the port
host = HOST
port = PORT
server_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
server_socket.bind((host, port))

# Listen for incoming connections
server_socket.listen(1)
print(f"Server listening on {host}:{port}")

def handle_client(client_socket, address):
    """Handle client connection"""
    print(f"Client connected: {address}")
    try:
        while True:
            data = client_socket.recv(1024)
            if data:
                try:
                    message = data.decode('utf-8')
                    print(f"Received: {message}")
                except UnicodeDecodeError:
                    print("Decode error occurred and ignored")
                    continue
                # Echo the message back to client
                client_socket.send(f"Echo: {message}".encode('utf-8'))
            else:
                break
    finally:
        client_socket.close()
        print(f"Client disconnected: {address}")

try:
    while True:
        # Accept incoming connections
        client_socket, address = server_socket.accept()
        # Handle client in a separate thread
        client_thread = threading.Thread(target=handle_client, args=(client_socket, address))
        client_thread.daemon = True
        client_thread.start()
except KeyboardInterrupt:
    print("Server shutting down...")
finally:
    server_socket.close()