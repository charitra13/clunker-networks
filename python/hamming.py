"""Assignment 2: Hamming(7,4), even parity; positions run left to right."""


def bits(text, length):
    if len(text) != length or any(bit not in "01" for bit in text):
        raise ValueError(f"Enter exactly {length} binary bits.")
    return [int(bit) for bit in text]


def encode(data):
    d = bits(data, 4)
    h = [0] * 8  # Ignore index 0 so positions match the diagram.
    h[3], h[5], h[6], h[7] = d
    h[1] = h[3] ^ h[5] ^ h[7]
    h[2] = h[3] ^ h[6] ^ h[7]
    h[4] = h[5] ^ h[6] ^ h[7]
    return "".join(str(bit) for bit in h[1:])


def decode(received):
    h = [0] + bits(received, 7)
    s1 = h[1] ^ h[3] ^ h[5] ^ h[7]
    s2 = h[2] ^ h[3] ^ h[6] ^ h[7]
    s4 = h[4] ^ h[5] ^ h[6] ^ h[7]
    error = 4 * s4 + 2 * s2 + s1
    if error:
        h[error] ^= 1
    corrected = "".join(str(bit) for bit in h[1:])
    data = "".join(str(h[i]) for i in (3, 5, 6, 7))
    return error, corrected, data


def main():
    try:
        print("Positions 1..7 run left to right. Assume at most ONE bit error.")
        data = input("Enter 4 data bits: ").strip()
        print("Generated Hamming code:", encode(data))
        received = input("Enter received 7-bit code: ").strip()
        error, corrected, recovered = decode(received)
        print(f"Error at position {error}" if error else "No error detected")
        print("Corrected code:", corrected)
        print("Recovered data:", recovered)
    except ValueError as exc:
        print(exc)


if __name__ == "__main__":
    main()
