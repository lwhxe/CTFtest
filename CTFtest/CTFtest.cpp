#include <iostream>
#include <cstdint>  // For uint8_t
#include <iomanip>

using namespace std;

string bytes_to_string(const uint8_t* array, size_t size) {
	string result;
	for (size_t i = 0; i < size; i++) {
		result += static_cast<char>(array[i]);
	}
	return result;
}
void printhex(uint8_t REG[3][16]) {
	for (int i = 0; i < 3; i++) {
		std::cout << "REG[" << i << "]:	";
		for (int j = 0; j < 16; j++) {
			std::cout << hex << setw(2) << setfill('0') << static_cast<int>(REG[i][j]) << " ";
		}
		std::cout << "	";
	}
	std::cout << endl;
}
void printstr(uint8_t REG[3][16]) {
	// Convert each REG array to a string
	for (int i = 0; i < 3; i++) {
		string reg_string = bytes_to_string(REG[i], 16);
		std::cout << "REG[" << i << "]: " << reg_string << std::endl;
	}
}

int main() {
 	uint8_t REG[3][16] = {
			{0x6e, 0x1d, 0x71, 0x30, 0xaf, 0x40, 0x3a, 0xac, 0xb4, 0x9a, 0xed, 0x58, 0xe4, 0x63, 0x29, 0x94},
			{0xa4, 0xed, 0xb6, 0x6c, 0x74, 0xf6, 0x9b, 0x01, 0xae, 0x91, 0xa4, 0x86, 0x02, 0x04, 0x32, 0xcd},
			{0xaf, 0xe1, 0x63, 0x1d, 0x45, 0x9b, 0xe1, 0xbd, 0x87, 0x14, 0xe3, 0x52, 0x60, 0xf7, 0xed, 0x9d}
	};

 	uint8_t TEMP[16] = {};

	printhex(REG);

	std::cout << "Values\n";
	/*
	movaps xmm2, xmm3
	movaps xmm0, xmm2
	movaps xmm3, xmm0
	*/
	memcpy(TEMP, &REG[2], sizeof(TEMP));     // Save REG[2]
	printhex(REG);
	memcpy(REG[2], &REG[1], sizeof(REG[2])); // Move REG[1] to REG[2]
	printhex(REG);
	memcpy(REG[1], &REG[0], sizeof(REG[1])); // Move REG[0] to REG[1]
	printhex(REG);
	memcpy(REG[0], &TEMP, sizeof(REG[0]));   // Move original REG[2] to REG[0]
	printhex(REG);

	for (int cl = 0; cl != 48; cl++) {
		/*	
		movaps xmm2, xmm3
		movaps xmm1, xmm2
		movaps xmm0, xmm1
		*/
		std::cout << "Rotate\n";
		memcpy(TEMP, REG[2], sizeof(TEMP));     // Save REG[2]
		printhex(REG);
		memcpy(REG[2], REG[1], sizeof(REG[2])); // Restore REG[2]'s original value
		printhex(REG);
		memcpy(REG[1], REG[0], sizeof(REG[1])); // Restore REG[1]'s original value
		printhex(REG);
		memcpy(REG[0], TEMP, sizeof(REG[0]));   // Restore REG[0]'s original value
		printhex(REG);
		
		std::cout << "AES\n";
		// aesenc xmm3, xmm0
		uint8_t* decrypted = aes_dec(TEMP, REG[0]);

		// XOR the result with REG[2]
		for (int i = 0; i < 16; i++) {
			decrypted[i] ^= TEMP[i];
		}

		memcpy(TEMP, &decrypted, 16);
		
		printhex(REG);
		
		std::cout << "XOR\n";
		// xorps xmm3, xmm2
		for (int i = 0; i < 16; i++) {
			TEMP[i] = TEMP[i] ^ REG[2][i];
		}

		printhex(REG);
		
		std::cout << "MOVAPS\n";
		// movaps xmm3, xmm1
		memcpy(REG[1], &TEMP, sizeof(REG[1]));
	}
	
	printhex(REG);
	printstr(REG);

	return 0;
}
