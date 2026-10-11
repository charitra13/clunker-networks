// UDP - Server
#include <algorithm>
#include <arpa/inet.h>
#include <cstring>
#include <iostream>
#include <string>
#include <sys/socket.h>
#include <unistd.h>
using namespace std;

int main() {
  int serverSocket;
  struct sockaddr_in serverAddr{}, clientAddr{};
  char buffer[1024];

  // Create UDP socket
  serverSocket = socket(AF_INET, SOCK_DGRAM, 0);
  if (serverSocket < 0) {
    cerr << "Socket creation failed\n";
    return 1;
  }

  // Configure server address
  serverAddr.sin_family = AF_INET;
  serverAddr.sin_port = htons(8080);
  serverAddr.sin_addr.s_addr = INADDR_ANY;

  // Bind socket (::bind avoids std::bind)
  if (::bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) <
      0) {
    cerr << "Bind failed\n";
    close(serverSocket);
    return 1;
  }

  cout << "UDP Server waiting for messages...\n";

  while (true) {
    memset(buffer, 0, sizeof(buffer));
    socklen_t clientLen = sizeof(clientAddr);

    ssize_t bytes = recvfrom(serverSocket, buffer, sizeof(buffer) - 1, 0,
                             (struct sockaddr *)&clientAddr, &clientLen);

    if (bytes < 0) {
      cerr << "Receive failed\n";
      break;
    }

    buffer[bytes] = '\0';

    // Strip trailing newline / carriage return
    while (bytes > 0 &&
           (buffer[bytes - 1] == '\n' || buffer[bytes - 1] == '\r'))
      buffer[--bytes] = '\0';

    cout << "Client: " << buffer << endl;

    if (strcmp(buffer, "exit") == 0) {
      const char *reply = "Server shutting down";
      sendto(serverSocket, reply, strlen(reply), 0,
             (struct sockaddr *)&clientAddr, clientLen);
      break;
    }

    // Reverse the received string
    string message(buffer);
    reverse(message.begin(), message.end());

    sendto(serverSocket, message.c_str(), message.size(), 0,
           (struct sockaddr *)&clientAddr, clientLen);

    cout << "Reversed: " << message << endl;
  }

  close(serverSocket);
  cout << "Server closed.\n";

  return 0;
}