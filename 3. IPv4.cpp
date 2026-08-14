#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>

using namespace std;

int main() {
    string ipAddress;

    int octet1, octet2, octet3, octet4;
    char dot1, dot2, dot3;

    cout << "Enter an IPv4 address: ";
    cin >> ipAddress;

    stringstream ss(ipAddress);

    // Read IP address in a.b.c.d format
    if (!(ss >> octet1 >> dot1
             >> octet2 >> dot2
             >> octet3 >> dot3
             >> octet4) ||
        dot1 != '.' ||
        dot2 != '.' ||
        dot3 != '.') {

        cout << "Invalid IP address format.\n";
        return 1;
    }

    // Ignore trailing spaces
    ss >> ws;

    // Reject additional characters
    if (!ss.eof()) {
        cout << "Invalid IP address format.\n";
        return 1;
    }

    // Check whether every octet is between 0 and 255
    if (octet1 < 0 || octet1 > 255 ||
        octet2 < 0 || octet2 > 255 ||
        octet3 < 0 || octet3 > 255 ||
        octet4 < 0 || octet4 > 255) {

        cout << "Invalid IP address.\n";
        cout << "Each octet must be between 0 and 255.\n";
        return 1;
    }

    string ipClass;
    string decimalRange;
    string binaryRange;
    string ipRange;
    string subnetMask;
    string hostsPerNetwork;
    string numberOfNetworks;

    // Class A
    if (octet1 >= 0 && octet1 <= 127) {
        ipClass = "Class A";
        decimalRange = "0 - 127";
        binaryRange = "0XXXXXXX";
        ipRange = "0.0.0.0 - 127.255.255.255";
        subnetMask = "255.0.0.0";
        hostsPerNetwork = "2^24 - 2 = 16777214";
        numberOfNetworks = "2^7 = 128";
    }

    // Class B
    else if (octet1 >= 128 && octet1 <= 191) {
        ipClass = "Class B";
        decimalRange = "128 - 191";
        binaryRange = "10XXXXXX";
        ipRange = "128.0.0.0 - 191.255.255.255";
        subnetMask = "255.255.0.0";
        hostsPerNetwork = "2^16 - 2 = 65534";
        numberOfNetworks = "2^14 = 16384";
    }

    // Class C
    else if (octet1 >= 192 && octet1 <= 223) {
        ipClass = "Class C";
        decimalRange = "192 - 223";
        binaryRange = "110XXXXX";
        ipRange = "192.0.0.0 - 223.255.255.255";
        subnetMask = "255.255.255.0";
        hostsPerNetwork = "2^8 - 2 = 254";
        numberOfNetworks = "2^21 = 2097152";
    }

    // Class D
    else if (octet1 >= 224 && octet1 <= 239) {
        ipClass = "Class D";
        decimalRange = "224 - 239";
        binaryRange = "1110XXXX";
        ipRange = "224.0.0.0 - 239.255.255.255";
        subnetMask = "N/A";
        hostsPerNetwork = "N/A";
        numberOfNetworks = "N/A";
    }

    // Class E
    else {
        ipClass = "Class E";
        decimalRange = "240 - 255";
        binaryRange = "1111XXXX";
        ipRange = "240.0.0.0 - 255.255.255.255";
        subnetMask = "N/A";
        hostsPerNetwork = "N/A";
        numberOfNetworks = "N/A";
    }

    cout << "\nEntered IP Address: " << ipAddress << "\n\n";

    cout << left;

    cout << setw(12) << "Class"
         << setw(24) << "First Octet Decimal"
         << setw(24) << "First Octet Binary"
         << setw(36) << "IP Range"
         << setw(20) << "Subnet Mask"
         << setw(28) << "Hosts per Network ID"
         << "# of Networks"
         << endl;

    cout << string(165, '-') << endl;

    cout << setw(12) << ipClass
         << setw(24) << decimalRange
         << setw(24) << binaryRange
         << setw(36) << ipRange
         << setw(20) << subnetMask
         << setw(28) << hostsPerNetwork
         << numberOfNetworks
         << endl;

    return 0;
}
