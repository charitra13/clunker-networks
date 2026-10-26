"""Assignment 5: two-way, turn-taking TCP chat. Start this terminal first."""
import socket


def run(host="127.0.0.1", port=8080):
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as server:
        server.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        server.bind((host, port))
        server.listen(1)
        print("TCP server waiting for client...", flush=True)
        connection, address = server.accept()
        with connection, connection.makefile("r", encoding="utf-8") as reader:
            print("Client connected:", address, flush=True)
            while True:
                line = reader.readline()  # One complete newline-delimited message.
                if line == "":             # EOF means peer disconnected.
                    print("Client disconnected.")
                    break
                message = line.rstrip("\n")
                print("Client:", message, flush=True)
                if message == "exit":
                    break
                reply = input("You: ")
                connection.sendall((reply + "\n").encode("utf-8"))
                if reply == "exit":
                    break
    print("Chat ended.")


if __name__ == "__main__":
    try:
        run()
    except (OSError, EOFError, KeyboardInterrupt):
        print("Chat stopped. Check the port and the other terminal.")
