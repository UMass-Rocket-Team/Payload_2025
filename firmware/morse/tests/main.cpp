#include <stdio.h>
#include <stdlib.h>

#include "AudioFile.h"

extern "C" {
#include "morse.h"
}

int main(int argc, char** argv) {
	// Input message for morse code.
	const char* test_message_input = "this is a test message";

	if (argc > 1) {
		test_message_input = argv[1];
	}

	printf("Encoding {%s} to morse code...\n", test_message_input);

	int codelen = get_morse_string_len(test_message_input);
	printf("Encoding length: %i\n", codelen);

	char morse_code_buf[1024] = {0};

	int encoding_ret = str2morse(test_message_input, morse_code_buf, 1024);
	if (encoding_ret == -1) {
		printf("Encoding error!\n");
		return -1;
	}

	printf("Encoded input message to morse code:\n%s\n", morse_code_buf);

	// Create the output file object
	AudioFile<float> fileout;
	morse_input_t morse_input = {

		.wave_type = morse_wave_sine,
		.carrier_freq = 550,
		.time_interval = 1.f / 20.f,
		.sample_rate = 16000,
		.code = morse_code_buf

	};

	// Find the duration (in seconds / time_interval) of the transmission
	int signal_dur_raw = get_morse_duration(&morse_input);
	float signal_dur = (float)(signal_dur_raw)*morse_input.time_interval;

	printf(
		"Encoding morse code into audio, final duration is: %.2fs\n",
		signal_dur);

	// Set WAV file parameters.
	fileout.shouldLogErrorsToConsole(true);
	fileout.setNumChannels(1);
	fileout.setSampleRate(16000);
	fileout.setAudioBufferSize(
		1, (int)(signal_dur * (float)(fileout.getSampleRate())));

	// Copy audio data into wav file.
	for (int i = 0; i < fileout.getNumChannels(); i++) {
		for (int j = 0; j < fileout.getNumSamplesPerChannel(); j++) {
			fileout.samples[i][j] = generate_morse_signal(
				&morse_input, float(j) / float(fileout.getSampleRate()));
		}
	}

	printf("Done.\n");

	fileout.save("output.wav");

	return 0;
}