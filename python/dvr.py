"""Assignment 4: synchronous distance vector routing with nonnegative costs."""

INF = float("inf")


def distance_vector(cost):
    n = len(cost)
    if not 1 <= n <= 26 or any(len(row) != n for row in cost):
        raise ValueError("Use a square matrix with 1 to 26 routers.")
    if any(value < 0 or value != value for row in cost for value in row):
        raise ValueError("Use nonnegative costs; use 999 for no link.")
    cost = [row[:] for row in cost]
    for i in range(n):
        cost[i][i] = 0
    distance = [row[:] for row in cost]
    hop = [[j if i != j and cost[i][j] < INF else None
            for j in range(n)] for i in range(n)]
    history = [[row[:] for row in distance]]
    while True:
        old = [row[:] for row in distance]  # Every router reads the SAME round.
        changed = False
        for i in range(n):               # Current router
            for k in range(n):           # Direct neighbor
                if k == i or cost[i][k] == INF:
                    continue
                for j in range(n):       # Destination
                    candidate = cost[i][k] + old[k][j]
                    if candidate < distance[i][j]:
                        distance[i][j] = candidate
                        hop[i][j] = k
                        changed = True
        history.append([row[:] for row in distance])
        if not changed:
            return distance, hop, history


def path_to(hop, source, destination):
    path = [source]
    while source != destination:
        source = hop[source][destination]
        if source is None:
            return []
        if source in path:
            raise ValueError("Routing loop in next-hop table.")
        path.append(source)
    return path


def main():
    try:
        n = int(input("Number of routers (1-26): "))
        if not 1 <= n <= 26:
            raise ValueError("Choose 1 to 26 routers.")
        print("Enter one row per router. Use 999 for no direct link.")
        cost = []
        for i in range(n):
            row = [int(x) for x in input(f"{chr(65 + i)}: ").split()]
            cost.append([INF if x == 999 else x for x in row])
        distance, hop, history = distance_vector(cost)
        for round_number, table in enumerate(history):
            print(f"\nRound {round_number} (last round confirms convergence):")
            for i, row in enumerate(table):
                print(chr(65 + i), " ".join("INF" if x == INF else str(x)
                                           for x in row))
        print("\nFinal routing tables: destination / cost / next hop")
        for i in range(n):
            print("Router", chr(65 + i))
            for j in range(n):
                next_hop = "-" if hop[i][j] is None else chr(65 + hop[i][j])
                print(chr(65 + j), distance[i][j], next_hop)
        source = input("Source router: ").strip().upper()
        target = input("Destination router: ").strip().upper()
        if any(len(x) != 1 or not "A" <= x <= chr(64 + n)
               for x in (source, target)):
            raise ValueError("Choose router letters from this network.")
        i, j = ord(source) - 65, ord(target) - 65
        path = path_to(hop, i, j)
        if path:
            print("Path:", " -> ".join(chr(65 + x) for x in path))
            print("Minimum cost:", distance[i][j])
        else:
            print("No path exists.")
    except ValueError as exc:
        print(exc)


if __name__ == "__main__":
    main()
