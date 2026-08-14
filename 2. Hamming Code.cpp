#include <iostream>
#include <string>

using namespace std;

bool isBinary(const string& value) {
    for (char bit : value) {
        if (bit != '0' && bit != '1') {
            return false;
        }
    }

    return true;
}

int main() {
    string data;

    cout << "Enter 4 data bits: ";
    cin >> data;

    if (data.length() != 4 || !isBinary(data)) {
        cout << "Error: Enter exactly 4 binary bits.\n";
        return 1;
    }

    int hamming[8] = {0};

    // Insert data bits.
    hamming[3] = data[0] - '0';
    hamming[5] = data[1] - '0';
    hamming[6] = data[2] - '0';
    hamming[7] = data[3] - '0';

    // Calculate even parity bits.
    hamming[1] = hamming[3] ^ hamming[5] ^ hamming[7];
    hamming[2] = hamming[3] ^ hamming[6] ^ hamming[7];
    hamming[4] = hamming[5] ^ hamming[6] ^ hamming[7];

    cout << "\nGenerated Hamming code: ";

    for (int i = 1; i <= 7; i++) {
        cout << hamming[i];
    }

    cout << "\n";

    string received;

    cout << "\nEnter received 7-bit Hamming code: ";
    cin >> received;

    if (received.length() != 7 || !isBinary(received)) {
        cout << "Error: Enter exactly 7 binary bits.\n";
        return 1;
    }

    int receivedCode[8] = {0};

    for (int i = 1; i <= 7; i++) {
        receivedCode[i] = received[i - 1] - '0';
    }

    // Recheck parity groups.
    int s1 = receivedCode[1] ^
             receivedCode[3] ^
             receivedCode[5] ^
             receivedCode[7];

    int s2 = receivedCode[2] ^
             receivedCode[3] ^
             receivedCode[6] ^
             receivedCode[7];

    int s4 = receivedCode[4] ^
             receivedCode[5] ^
             receivedCode[6] ^
             receivedCode[7];

    // Syndrome = S4 S2 S1
    int errorPosition = (s4 * 4) + (s2 * 2) + s1;

    if (errorPosition == 0) {
        cout << "\nNo error detected.\n";
    } else {
        cout << "\nError detected at position: "
             << errorPosition << "\n";

        // Correct the erroneous bit.
        receivedCode[errorPosition] =
            1 - receivedCode[errorPosition];

        cout << "Corrected Hamming code: ";

        for (int i = 1; i <= 7; i++) {
            cout << receivedCode[i];
        }

        cout << "\n";
    }

    cout << "Original data bits: "
         << receivedCode[3]
         << receivedCode[5]
         << receivedCode[6]
         << receivedCode[7]
         << "\n";

    return 0;
}
