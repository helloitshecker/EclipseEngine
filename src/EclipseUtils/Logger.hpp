#pragma once
#include <stdio.h>

class LOG {
public: static void SEND(char* message) {
		printf("%s\n", message);
	}
};