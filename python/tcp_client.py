"""Assignment 5: TCP client sends first; the server replies."""
import socket


def run(host="127.0.0.1", port=8080):
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as client:
        client.connect((host, port))
        print("Connected to server!", flush=True)
        with client.makefile("r", encoding="utf-8") as reader:
            while True:
                message = input("You: ")
                client.sendall((message + "\n").encode("utf-8"))
                if message == "exit":
                    break
                line = reader.readline()
                if line == "":
                    print("Server disconnected.")
                    break
                reply = line.rstrip("\n")
                print("Server:", reply, flush=True)
                if reply == "exit":
                    break
    print("Disconnected from server.")


if __name__ == "__main__":
    try:
        run()
    except (OSError, EOFError, KeyboardInterrupt):
        print("Chat stopped. Start the server first and check the port.")
