"""Assignment 6: send one datagram, wait for its reply, repeat."""
import socket

MAX_BYTES = 1024


def run(host="127.0.0.1", port=8080):
    server_address = (host, port)
    with socket.socket(socket.AF_INET, socket.SOCK_DGRAM) as client:
        client.settimeout(5)  # A missing UDP reply must not hang the terminal.
        while True:
            message = input("Enter a string (exit to quit): ")
            data = message.encode("utf-8")
            if len(data) > MAX_BYTES:
                print("Use at most 1024 UTF-8 bytes.")
                continue
            client.sendto(data, server_address)
            try:
                reply, address = client.recvfrom(MAX_BYTES)
            except socket.timeout:
                print("No reply in 5 seconds. Check the server, then restart the client.")
                break
            if address != server_address:
                print("Unexpected sender. Restart the client.")
                break
            print("Server reply:", reply.decode("utf-8"), flush=True)
            if message == "exit":
                break
    print("Client closed.")


if __name__ == "__main__":
    try:
        run()
    except (OSError, EOFError, KeyboardInterrupt):
        print("Client stopped. Check the server and try again.")
