import socket
import json
import tempfile
import os
from datetime import datetime
from dotenv import load_dotenv
import threading

load_dotenv()
HOST = os.getenv('HOST_IP', '127.0.0.1')
PORT = int(os.getenv('PORT_NUM', '30000'))

# The path to the data file not to depend on the current environment.
BASE_DIR = os.path.dirname(os.path.abspath(__file__))
MOISTURE_FILE = os.path.join(BASE_DIR, "..", "soil_moisture.json")

# Create a TCP/IP socket
server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

# Bind the socket to the port
host = HOST
port = PORT
server_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
server_socket.bind((host, port))

# Listen for incoming connections
# Set to 7, inspired by the legend of Japanese Prince Shotoku's ability 
# to listen to multiple people simultaneously.
server_socket.listen(7)
print(f"Server listening on {host}:{port}")

def update_sticker(label, value, file_path):
    """Atomically update the sticker data in the JSON file"""
    try:
        data_to_save = {
            label: value,
            "timestamp": datetime.now().isoformat()
        }
        # Get the directory of the file to ensure the temp file is created in the same location
        dir_name = os.path.dirname(os.path.abspath(file_path))
        # 1. Write to a temporary file first
        with tempfile.NamedTemporaryFile('w', dir=dir_name, delete=False, suffix='.tmp') as tf:
            json.dump(data_to_save, tf, indent=4)
            temp_name = tf.name
        # 2. Atomically replace the original file with the temporary file
        os.replace(temp_name, file_path)
        print(f"Sticker updated: {label}={value}")
    except Exception as e:
        print(f"Failed to update sticker: {e}")

def handle_client(client_socket, address):
    """Handle client connection"""
    print(f"Client connected: {address}")
    try:
        while True:
            data = client_socket.recv(1024)
            if not data:
                break
            try:
                message = data.decode('utf-8').strip()
                print(f"Received: {message}")
                if ':' in message:
                    parts = message.split(':')
                    update_sticker(parts[0], int(parts[1]), MOISTURE_FILE)
                client_socket.send(f"Echo: {message}".encode('utf-8'))
            except UnicodeDecodeError:
                print("Decode error occurred and ignored")
                continue
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