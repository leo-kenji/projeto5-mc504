#include "caesar_cipher.h"

void cifrar(char *string, int shift)
{   
    int i;
	for (i = 0; string[i] != '\0' && i < DRIVER_BUFFER; i++) {
		if (string[i] >= 'A' && string[i] <= 'Z') {
			string[i] = ((string[i] + shift - 65) % 26) + 65;
		} else if (string[i] >= 'a' && string[i] <= 'z') {
			string[i] = ((string[i] + shift - 97) % 26) + 97;
		}
	}
}

//desencriptando
void decifrar(char *string, int shift)
{
	cifrar(string, 26 - shift);
}
