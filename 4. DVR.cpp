#include <iomanip>
#include <iostream>
#include <limits>
#include <vector>
using namespace std;

const int INF = 999;

struct RoutingTable {
  vector<int> distance;
  vector<int> nextHop;
};

void printCostMatrix(const vector<vector<int>> &cost, int n) {
  cout << "\n========== INITIAL NETWORK COST MATRIX ==========\n\n";

  cout << setw(8) << "";
  for (int i = 0; i < n; i++)
    cout << setw(8) << char('A' + i);
  cout << "\n";

  for (int i = 0; i < n; i++) {
    cout << setw(8) << char('A' + i);

    for (int j = 0; j < n; j++) {
      if (cost[i][j] >= INF)
        cout << setw(8) << "INF";
      else
        cout << setw(8) << cost[i][j];
    }
    cout << "\n";
  }
}

void printRoutingTables(const vector<RoutingTable> &tables, int n) {
  cout << "\n========== ROUTING TABLES ==========\n";

  for (int i = 0; i < n; i++) {
    cout << "\nRouter " << char('A' + i) << ":\n";
    cout << setw(15) << "Destination" << setw(15) << "Cost" << setw(15)
         << "Next Hop" << "\n";

    for (int j = 0; j < n; j++) {
      cout << setw(15) << char('A' + j);

      if (tables[i].distance[j] >= INF) {
        cout << setw(15) << "INF";
        cout << setw(15) << "-";
      } else {
        cout << setw(15) << tables[i].distance[j];

        if (i == j)
          cout << setw(15) << "-";
        else
          cout << setw(15) << char('A' + tables[i].nextHop[j]);
      }

      cout << "\n";
    }
  }
}

void distanceVectorRouting(const vector<vector<int>> &cost,
                           vector<RoutingTable> &tables, int n) {
  // Initial routing tables
  for (int i = 0; i < n; i++) {
    tables[i].distance = cost[i];
    tables[i].nextHop.resize(n);

    for (int j = 0; j < n; j++) {
      if (i == j)
        tables[i].nextHop[j] = -1;
      else if (cost[i][j] < INF)
        tables[i].nextHop[j] = j;
      else
        tables[i].nextHop[j] = -1;
    }
  }

  cout << "\n========== INITIAL ROUTING TABLES ==========\n";
  printRoutingTables(tables, n);

  bool updated;
  int iteration = 0;

  // Bellman-Ford / Distance Vector iterations
  do {
    updated = false;
    iteration++;

    cout << "\n========== ITERATION " << iteration << " ==========\n";

    // Create a copy so that all routers use
    // the previous iteration's information.
    vector<RoutingTable> oldTables = tables;

    for (int i = 0; i < n; i++) {

      // Check every neighboring router
      for (int k = 0; k < n; k++) {

        if (cost[i][k] >= INF)
          continue;

        // Check every possible destination
        for (int j = 0; j < n; j++) {

          if (oldTables[k].distance[j] >= INF)
            continue;

          int newCost = cost[i][k] + oldTables[k].distance[j];

          if (newCost < tables[i].distance[j]) {

            cout << "Router " << char('A' + i) << ": Destination "
                 << char('A' + j) << " updated via Router " << char('A' + k)
                 << " | Cost: " << tables[i].distance[j] << " -> " << newCost
                 << "\n";

            tables[i].distance[j] = newCost;
            tables[i].nextHop[j] = k;

            updated = true;
          }
        }
      }
    }

  } while (updated);

  cout << "\n========== NETWORK CONVERGED ==========\n";
  cout << "Total iterations: " << iteration << "\n";

  cout << "\n========== FINAL ROUTING TABLES ==========\n";
  printRoutingTables(tables, n);
}

void showShortestPath(const vector<RoutingTable> &tables, int source,
                      int destination, int n) {
  if (tables[source].distance[destination] >= INF) {
    cout << "\nNo path exists between " << char('A' + source) << " and "
         << char('A' + destination) << ".\n";
    return;
  }

  cout << "\n========== SHORTEST PATH ==========\n";

  cout << "Source      : " << char('A' + source) << "\n";
  cout << "Destination : " << char('A' + destination) << "\n";
  cout << "Minimum Cost: " << tables[source].distance[destination] << "\n";

  cout << "Path        : ";

  int current = source;

  cout << char('A' + current);

  while (current != destination) {
    current = tables[current].nextHop[destination];

    if (current == -1) {
      cout << "\nPath reconstruction failed.\n";
      return;
    }

    cout << " -> " << char('A' + current);
  }

  cout << "\n";
}

int main() {

  int n;

  cout << "=============================================\n";
  cout << "     DISTANCE VECTOR ROUTING ALGORITHM\n";
  cout << "=============================================\n";

  cout << "\nEnter number of routers: ";
  cin >> n;

  vector<vector<int>> cost(n, vector<int>(n));

  cout << "\nEnter the network cost matrix.\n";
  cout << "Use " << INF << " for no direct connection.\n\n";

  for (int i = 0; i < n; i++) {
    cout << "Enter costs for Router " << char('A' + i) << ":\n";

    for (int j = 0; j < n; j++) {

      cin >> cost[i][j];

      if (i == j)
        cost[i][j] = 0;
    }
  }

  printCostMatrix(cost, n);

  vector<RoutingTable> tables(n);

  // Run Distance Vector Routing
  distanceVectorRouting(cost, tables, n);

  // Find shortest path
  char sourceChar, destinationChar;

  cout << "\nEnter source router (A-" << char('A' + n - 1) << "): ";
  cin >> sourceChar;

  cout << "Enter destination router (A-" << char('A' + n - 1) << "): ";
  cin >> destinationChar;

  int source = sourceChar - 'A';
  int destination = destinationChar - 'A';

  if (source < 0 || source >= n || destination < 0 || destination >= n) {

    cout << "\nInvalid router entered.\n";
    return 0;
  }

  showShortestPath(tables, source, destination, n);

  return 0;
}