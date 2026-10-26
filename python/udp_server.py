"""Assignment 6: receive a UDP string and return its reversal."""
import socket

MAX_BYTES = 1024


def run(host="127.0.0.1", port=8080):
    with socket.socket(socket.AF_INET, socket.SOCK_DGRAM) as server:
        server.bind((host, port))
        print("UDP server waiting for messages...", flush=True)
        while True:
            data, client_address = server.recvfrom(MAX_BYTES + 1)
            if len(data) > MAX_BYTES:
                server.sendto(b"Use at most 1024 UTF-8 bytes.", client_address)
                continue
            try:
                message = data.decode("utf-8")
            except UnicodeDecodeError:
                server.sendto(b"Use UTF-8 text.", client_address)
                continue
            print("Client:", message, flush=True)
            reply = "Server shutting down" if message == "exit" else message[::-1]
            server.sendto(reply.encode("utf-8"), client_address)
            if message == "exit":
                break
    print("Server closed.")


if __name__ == "__main__":
    try:
        run()
    except (OSError, KeyboardInterrupt):
        print("Server stopped. Check whether the port is already in use.")
