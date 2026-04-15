import socket
import os
from dotenv import load_dotenv

load_dotenv()
HOST = os.getenv('HOST_IP', '0.0.0.0')
PORT = os.getenv('PORT_NUM', 30000)

with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
    s.bind((HOST, PORT))
    s.listen()
    print(f"[*] Dispatch Center is online. Waiting for Haniwa on port {PORT}...")
    conn, addr = s.accept()
    with conn:
        print(f"[+] Connection established from: {addr}")
        data = conn.recv(1024)
        print(f"[!] Message received: {data.decode()}")