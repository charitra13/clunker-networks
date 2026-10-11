// TCP - Client
#include <arpa/inet.h>
#include <cstring>
#include <iostream>
#include <unistd.h>
using namespace std;

int main() {
  int clientSocket;
  struct sockaddr_in serverAddr{};
  char buffer[1024];

  // Create TCP socket
  clientSocket = socket(AF_INET, SOCK_STREAM, 0);

  if (clientSocket < 0) {
    cerr << "Socket creation failed\n";
    return 1;
  }

  // Configure server address
  serverAddr.sin_family = AF_INET;
  serverAddr.sin_port = htons(8080);
  inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr);

  // Connect to server
  if (connect(clientSocket, (struct sockaddr *)&serverAddr,
              sizeof(serverAddr)) < 0) {
    cerr << "Connection failed. Start the server first.\n";
    close(clientSocket);
    return 1;
  }

  cout << "Connected to server!\n";

  while (true) {
    cout << "You: ";
    string message;
    getline(cin, message);

    if (send(clientSocket, message.c_str(), message.size(), 0) < 0) {
      cerr << "Send failed\n";
      break;
    }

    if (message == "exit")
      break;

    memset(buffer, 0, sizeof(buffer));

    int bytes = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);

    if (bytes <= 0) {
      cout << "Server disconnected.\n";
      break;
    }

    cout << "Server: " << buffer << endl;

    if (strcmp(buffer, "exit") == 0)
      break;
  }

  close(clientSocket);
  cout << "Disconnected from server.\n";

  return 0;
}
