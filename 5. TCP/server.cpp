// TCP - Server
#include <arpa/inet.h>
#include <cstring>
#include <iostream>
#include <string>
#include <sys/socket.h>
#include <unistd.h>
using namespace std;

int main() {
  int serverSocket, clientSocket;
  struct sockaddr_in serverAddr{};
  char buffer[1024];

  serverSocket = socket(AF_INET, SOCK_STREAM, 0);
  if (serverSocket < 0) {
    cerr << "Socket creation failed\n";
    return 1;
  }

  int opt = 1;
  setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

  serverAddr.sin_family = AF_INET;
  serverAddr.sin_port = htons(8080);
  serverAddr.sin_addr.s_addr = INADDR_ANY;

  if (::bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) <
      0) {
    cerr << "Bind failed\n";
    close(serverSocket);
    return 1;
  }

  listen(serverSocket, 1);
  cout << "Server waiting for client...\n";

  clientSocket = accept(serverSocket, nullptr, nullptr);
  if (clientSocket < 0) {
    cerr << "Accept failed\n";
    close(serverSocket);
    return 1;
  }

  cout << "Client connected!\n";

  while (true) {
    memset(buffer, 0, sizeof(buffer));

    ssize_t bytes = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
    if (bytes <= 0) {
      cout << "Client disconnected.\n";
      break;
    }

    // Strip trailing newline / carriage return
    while (bytes > 0 &&
           (buffer[bytes - 1] == '\n' || buffer[bytes - 1] == '\r'))
      buffer[--bytes] = '\0';

    cout << "Client: " << buffer << endl;

    if (strcmp(buffer, "exit") == 0)
      break;

    cout << "You: ";
    string message;
    getline(cin, message);

    if (send(clientSocket, message.c_str(), message.size(), 0) < 0) {
      cerr << "Send failed\n";
      break;
    }

    if (message == "exit")
      break;
  }

  close(clientSocket);
  close(serverSocket);

  cout << "Chat ended.\n";
  return 0;
}