"""Assignment 3: historical classful IPv4 classification, as in the C++ lab."""


def classify(address):
    parts = address.split(".")
    if (len(parts) != 4
            or any(not p or not p.isascii() or not p.isdigit() for p in parts)):
        raise ValueError("Use four decimal octets separated by dots.")
    octets = [int(p) for p in parts]
    if any(n > 255 for n in octets):
        raise ValueError("Each octet must be between 0 and 255.")
    first = octets[0]
    if first <= 127:
        cls, low, high, prefix, network_bits = "A", 0, 127, "0", 8
    elif first <= 191:
        cls, low, high, prefix, network_bits = "B", 128, 191, "10", 16
    elif first <= 223:
        cls, low, high, prefix, network_bits = "C", 192, 223, "110", 24
    elif first <= 239:
        cls, low, high, prefix, network_bits = "D", 224, 239, "1110", None
    else:
        cls, low, high, prefix, network_bits = "E", 240, 255, "1111", None
    result = {
        "Class": cls,
        "First octet range": f"{low} - {high}",
        "First octet binary": f"{first:08b}",
        "Class bit pattern": prefix + "X" * (8 - len(prefix)),
        "IP range": f"{low}.0.0.0 - {high}.255.255.255",
        "Default mask": "N/A",
        "Hosts per network": "N/A",
        "Theoretical network IDs": "N/A",
    }
    if network_bits is not None:
        result["Default mask"] = ".".join(
            ["255"] * (network_bits // 8) + ["0"] * (4 - network_bits // 8))
        result["Hosts per network"] = 2 ** (32 - network_bits) - 2
        result["Theoretical network IDs"] = 2 ** (network_bits - len(prefix))
    return result


def main():
    try:
        result = classify(input("Enter an IPv4 address: ").strip())
        for label, value in result.items():
            print(f"{label:26}: {value}")
        print("Historical classful exercise; modern networks use CIDR prefixes.")
        print("0/8 and 127/8 are special; 128 Class A IDs is a theoretical count.")
        print("Class D is multicast; Class E is reserved, including special addresses.")
    except ValueError as exc:
        print(exc)


if __name__ == "__main__":
    main()
