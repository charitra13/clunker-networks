// UDP - Client
#include <arpa/inet.h>
#include <cstring>
#include <iostream>
#include <unistd.h>
using namespace std;

int main() {
  int clientSocket;
  struct sockaddr_in serverAddr{};

  // Create UDP socket
  clientSocket = socket(AF_INET, SOCK_DGRAM, 0);

  if (clientSocket < 0) {
    cerr << "Socket creation failed\n";
    return 1;
  }

  // Configure server address
  serverAddr.sin_family = AF_INET;
  serverAddr.sin_port = htons(8080);
  inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr);

  while (true) {
    string message;

    cout << "Enter a string (exit to quit): ";
    getline(cin, message);

    // Send message to server
    if (sendto(clientSocket, message.c_str(), message.size(), 0,
               (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
      cerr << "Send failed\n";
      break;
    }

    char buffer[1024] = {};
    struct sockaddr_in replyAddr{};
    socklen_t addrLen = sizeof(replyAddr);

    // Receive server reply
    int bytes = recvfrom(clientSocket, buffer, sizeof(buffer) - 1, 0,
                         (struct sockaddr *)&replyAddr, &addrLen);

    if (bytes < 0) {
      cerr << "Receive failed\n";
      break;
    }

    buffer[bytes] = '\0';
    cout << "Server reply: " << buffer << endl;

    if (message == "exit")
      break;
  }

  close(clientSocket);
  cout << "Client closed.\n";

  return 0;
}