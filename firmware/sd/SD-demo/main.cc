#include <stdio.h>

#include "SD.hpp"
#include "pico/stdlib.h"

int main() {
	stdio_init_all();

	// wait for user to press a key
	getchar();
	sleep_ms(50);

	printf("Hello, world!\n");

	SD sd(5);

	printf("Initializing SD card\n");
	if (sd.open()) {
		printf("SD card initialized\n");
	} else {
		printf("SD card failed to initialize\n");
		while (1);
	}

	auto files = sd.listFiles("/");

	int max_fileno = 0;

	printf("Files:\n");
	for (const auto& file : files) {
		printf("File: %s\n", file.c_str());
	}

	printf("Closing SD card\n");
	if ( sd.close() ) {
		printf("SD card closed\n");
	} else {
		printf("SD card failed to close\n");
	}

	while (1) {}
}
