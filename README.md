# Computer networks lab

Seven networking assignments: Packet Tracer topologies and server configuration,
Hamming code, classful IPv4 classification, distance vector routing, TCP chat,
and UDP string reversal. Original C++ programs and Packet Tracer files are kept
alongside Python practice programs.

## Setup and usage

Use Python 3.10 or later; no extra packages are required.

Open `lessons/network-lab.html` in a browser for interactive visual exercises,
Python syntax reminders, source code, prediction questions, and exam practice.
On macOS:

```sh
open lessons/network-lab.html
```

Run the algorithm assignments from the repository root:

```sh
python3 python/hamming.py
python3 python/ipv4.py
python3 python/dvr.py
```

Run each socket server first, then its client in a second terminal:

```sh
python3 python/tcp_server.py
python3 python/tcp_client.py
```

For UDP, use `python/udp_server.py` and `python/udp_client.py` instead. Both
pairs use `127.0.0.1:8080` for same-machine practice. TCP chat takes turns, with
the client sending first. UDP returns a reversed string. Type `exit` to finish.

Validate the programs with:

```sh
python3 -B -m unittest discover -s python -v
```

Open assignments 1 and 7 with Cisco Packet Tracer.
