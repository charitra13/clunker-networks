"""Run with: python3 -B -m unittest discover -s python -v"""
import random
import selectors
import socket
import subprocess
import sys
import unittest
from pathlib import Path

from dvr import INF, distance_vector, path_to
from hamming import decode, encode
from ipv4 import classify

HERE = Path(__file__).resolve().parent


class Algorithms(unittest.TestCase):
    def test_every_hamming_message_and_single_bit_error(self):
        for number in range(16):
            data = f"{number:04b}"
            code = encode(data)
            for group in ((1, 3, 5, 7), (2, 3, 6, 7), (4, 5, 6, 7)):
                self.assertEqual(sum(int(code[p - 1]) for p in group) % 2, 0)
            self.assertEqual(decode(code), (0, code, data))
            for position in range(1, 8):
                damaged = list(code)
                damaged[position - 1] = str(1 - int(damaged[position - 1]))
                self.assertEqual(decode("".join(damaged)), (position, code, data))
        self.assertEqual(encode("1011"), "0110011")

    def test_hamming_rejects_invalid_input(self):
        for text in ("", "101", "10101", "10x1", "１011"):
            with self.assertRaises(ValueError):
                encode(text)
        for text in ("", "101010", "1234567"):
            with self.assertRaises(ValueError):
                decode(text)

    def test_ipv4_boundaries(self):
        for first, expected in ((0, "A"), (127, "A"), (128, "B"), (191, "B"),
                                (192, "C"), (223, "C"), (224, "D"), (239, "D"),
                                (240, "E"), (255, "E")):
            self.assertEqual(classify(f"{first}.0.0.1")["Class"], expected)
        self.assertEqual(classify("192.168.1.10")["Hosts per network"], 254)
        self.assertEqual(classify("172.16.5.9")["Default mask"], "255.255.0.0")
        self.assertEqual(classify("10.0.0.1")["Theoretical network IDs"], 128)
        for text in ("1.2.3", "1.2.3.256", "-1.2.3.4", "1.2.3.4x",
                     "1..2.3", "1.2.3.4.5", "１.2.3.4", "1. 2.3.4"):
            with self.assertRaises(ValueError):
                classify(text)

    def test_routing_rounds_and_disconnected_router(self):
        cost = [[0, 1, 7, INF], [1, 0, 2, 8],
                [7, 2, 0, 1], [INF, 8, 1, 0]]
        original = [row[:] for row in cost]
        distance, hop, history = distance_vector(cost)
        self.assertEqual(cost, original)
        self.assertEqual([round_[0] for round_ in history],
                         [[0, 1, 7, INF], [0, 1, 3, 8], [0, 1, 3, 4], [0, 1, 3, 4]])
        self.assertEqual(path_to(hop, 0, 3), [0, 1, 2, 3])
        self.assertEqual(distance[0][3], 4)
        distance, hop, _ = distance_vector([[0, INF], [INF, 0]])
        self.assertEqual(distance[0][1], INF)
        self.assertEqual(path_to(hop, 0, 1), [])
        self.assertEqual(path_to(hop, 0, 0), [0])

    def test_routing_against_independent_floyd_warshall(self):
        rng = random.Random(31)
        for _ in range(100):
            n = rng.randint(1, 7)
            cost = [[0 if i == j else rng.choice([INF, 0, 1, 3, 10, 700])
                     for j in range(n)] for i in range(n)]
            expected = [row[:] for row in cost]
            for k in range(n):
                for i in range(n):
                    for j in range(n):
                        expected[i][j] = min(expected[i][j], expected[i][k] + expected[k][j])
            actual, hop, _ = distance_vector(cost)
            self.assertEqual(actual, expected)
            for i in range(n):
                for j in range(n):
                    path = path_to(hop, i, j)
                    if actual[i][j] == INF:
                        self.assertEqual(path, [])
                    else:
                        self.assertEqual(sum(cost[a][b] for a, b in zip(path, path[1:])), actual[i][j])


class Sockets(unittest.TestCase):
    def start_server(self, module, replies=""):
        kind = socket.SOCK_DGRAM if module.startswith("udp") else socket.SOCK_STREAM
        with socket.socket(socket.AF_INET, kind) as probe:
            probe.bind(("127.0.0.1", 0))
            port = probe.getsockname()[1]
        process = subprocess.Popen(
            [sys.executable, "-B", "-u", "-c", f"import {module}; {module}.run(port={port})"],
            cwd=HERE, stdin=subprocess.PIPE, stdout=subprocess.PIPE,
            stderr=subprocess.STDOUT, text=True)
        self.addCleanup(self.stop_process, process)
        process.stdin.write(replies)
        process.stdin.flush()
        with selectors.DefaultSelector() as ready:
            ready.register(process.stdout, selectors.EVENT_READ)
            self.assertTrue(ready.select(5), "Server did not become ready")
        self.assertIn("waiting", process.stdout.readline())
        return process, port

    @staticmethod
    def stop_process(process):
        if process.poll() is None:
            process.kill()
        process.communicate(timeout=5)

    def run_client(self, module, port, messages):
        return subprocess.run(
            [sys.executable, "-B", "-u", "-c", f"import {module}; {module}.run(port={port})"],
            cwd=HERE, input=messages, capture_output=True, text=True, timeout=8, check=True)

    def test_tcp_actual_client_and_server(self):
        server, port = self.start_server("tcp_server", "hi\n\n")
        result = self.run_client("tcp_client", port, "hello\n\nexit\n")
        self.assertIn("Server: hi", result.stdout)
        output, _ = server.communicate(timeout=5)
        self.assertEqual(server.returncode, 0, output)
        self.assertIn("Client: hello", output)
        self.assertIn("Client: exit", output)

    def test_tcp_fragmentation_unicode_coalescing_and_server_exit(self):
        server, port = self.start_server("tcp_server", "first reply\nexit\n")
        with socket.create_connection(("127.0.0.1", port), timeout=3) as client:
            with client.makefile("rb") as reader:
                payload = ("नमस्ते" + "x" * 5000 + "\nnext\n").encode()
                for start in range(0, len(payload), 7):
                    client.sendall(payload[start:start + 7])
                self.assertEqual(reader.readline(), b"first reply\n")
                self.assertEqual(reader.readline(), b"exit\n")
        output, _ = server.communicate(timeout=5)
        self.assertEqual(server.returncode, 0, output)
        self.assertIn("नमस्ते", output)
        self.assertIn("Client: next", output)

    def test_tcp_client_handles_server_exit(self):
        server, port = self.start_server("tcp_server", "exit\n")
        result = self.run_client("tcp_client", port, "hello\n")
        self.assertIn("Server: exit", result.stdout)
        server.communicate(timeout=5)
        self.assertEqual(server.returncode, 0)

    def test_udp_actual_client_and_server(self):
        server, port = self.start_server("udp_server")
        result = self.run_client("udp_client", port, "network\n\né🙂\nexit\n")
        for expected in ("krowten", "🙂é", "Server shutting down"):
            self.assertIn(expected, result.stdout)
        output, _ = server.communicate(timeout=5)
        self.assertEqual(server.returncode, 0, output)

    def test_udp_datagram_boundaries_empty_and_invalid_utf8(self):
        server, port = self.start_server("udp_server")
        with socket.socket(socket.AF_INET, socket.SOCK_DGRAM) as client:
            client.settimeout(3)
            for data, expected in ((b"", b""), (b"abc", b"cba"),
                                   (b"\xff", b"Use UTF-8 text."),
                                   (b"a" * 1025, b"Use at most 1024 UTF-8 bytes."),
                                   (b"exit", b"Server shutting down")):
                client.sendto(data, ("127.0.0.1", port))
                reply, address = client.recvfrom(2048)
                self.assertEqual(reply, expected)
                self.assertEqual(address, ("127.0.0.1", port))
        server.communicate(timeout=5)
        self.assertEqual(server.returncode, 0)


if __name__ == "__main__":
    unittest.main()
