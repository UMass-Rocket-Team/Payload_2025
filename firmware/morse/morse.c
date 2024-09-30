#include "morse.h"

#include <math.h>

#include "string.h"

// order - A...Z 0...9 . ,
static const char morse_lut[][7] = {
	".-",	 "-...",  "-.-.",  "-..",	".",	  "..-.",  "--.",	"....",
	"..",	 ".---",  "-.-",   ".-..",	"--",	  "-.",	   "---",	".--.",
	"--.-",	 ".-.",	  "...",   "-",		"..-",	  "...-",  ".--",	"-..-",
	"-.--",	 "--..",  "-----", ".----", "..---",  "...--", "....-", ".....",
	"-....", "--...", "---..", "----.", ".-.-.-", "--..--"};

int str2morse(const char* str, char* morse, int maxlen) {
	if (str == NULL || morse == NULL || maxlen == 0) {
		return -1;
	}

	int inpos = 0;
	int outpos = 0;
	char chin = 0;

	while (1) {
		if (str[inpos] == '\0') {
			morse[outpos] = '\0';
			return outpos;
		}
		chin = str[inpos];

		// make lowercase
		if (chin >= 'A' && chin <= 'Z') {
			chin += ('a' - 'A');
		}

		const char* copysrc = NULL;

		// if the character is a letter
		if (chin >= 'a' && chin <= 'z') {
			copysrc = morse_lut[chin - 'a'];
		} else if (chin >= '0' && chin <= '9') {
			copysrc = morse_lut[chin + 26 - '0'];
		} else if (chin == ' ') {
			copysrc = "/";
		} else if (chin == '.') {
			copysrc = morse_lut[26];
		} else if (chin == ',') {
			copysrc = morse_lut[27];
		}

		if (copysrc != NULL) {
			int copylen = (int)strlen(copysrc);
			if (copylen + outpos + 2 > maxlen) {
				morse[outpos] = '\0';
				return outpos;
			}
			memcpy(morse + outpos, copysrc, copylen);
			outpos += copylen;
			morse[outpos++] = ' ';
		}

		inpos++;
	}
}

int get_morse_string_len(const char* str) {
	if (str == NULL) {
		return -1;
	}

	int inpos = 0;
	int ret = 0;
	char chin = 0;

	while (1) {
		if (str[inpos] == '\0') {
			return ret;
		}
		chin = str[inpos];

		// make lowercase
		if (chin >= 'A' && chin <= 'Z') {
			chin += ('a' - 'A');
		}

		const char* copysrc = NULL;

		// if the character is a letter
		if (chin >= 'a' && chin <= 'z') {
			copysrc = morse_lut[chin - 'a'];
		} else if (chin >= '0' && chin <= '9') {
			copysrc = morse_lut[chin + 26 - '0'];
		} else if (chin == ' ') {
			copysrc = "/";
		} else if (chin == '.') {
			copysrc = morse_lut[26];
		} else if (chin == ',') {
			copysrc = morse_lut[27];
		}

		if (copysrc != NULL) {
			int copylen = (int)strlen(copysrc);
			ret += copylen;
		}

		inpos++;
	}

	return ret;
}

int get_morse_duration(morse_input_t* settings) {
	if (settings == NULL) {
		return -1;
	}
	if (settings->code == NULL) {
		return -1;
	}

	int ret = 0;
	int msg_position = 0;
	while (settings->code[msg_position] != '\0') {
		switch (settings->code[msg_position]) {
			case ('.'): {
				ret += 2;
				break;
			}
			case ('-'): {
				ret += 4;
				break;
			}
			case (' '): {
				ret += 1;
				break;
			}
			case ('/'): {
				ret += 2;
				break;
			}
			default: {
				break;
			}
		}

		msg_position++;
	}
	return ret;
}

float generate_morse_signal(morse_input_t* settings, float t) {
	if (settings == NULL) {
		return -1;
	}
	if (settings->code == NULL) {
		return -1;
	}

	float tmp = t;
	int char_idx = 0;
	while (tmp > 0) {
		switch (settings->code[char_idx]) {
			case ('\0'): {
				return 0.0;
			}
			case ('.'): {
				tmp -= 2.f * settings->time_interval;
				break;
			}
			case ('-'): {
				tmp -= 4.f * settings->time_interval;
				break;
			}
			case (' '): {
				tmp -= 1.f * settings->time_interval;
				break;
			}
			case ('/'): {
				tmp -= 2.f * settings->time_interval;
				break;
			}
			default: {
				return 0.0f;
			}
		}

		if (tmp < 0) {
			break;
		}
		char_idx++;
	}

	if (settings->code[char_idx] == '.' || settings->code[char_idx] == '-') {
		float tone_duration =
			settings->time_interval * (settings->code[char_idx] == '.' ? 2 : 4);
		float stop_time = tone_duration - settings->time_interval;

		tmp += tone_duration;

		if (tmp <= stop_time) {
			switch (settings->wave_type) {
				case (morse_wave_sine): {
					return sinf(
						tmp * 2.f * 3.141592f *
						(float)(settings->carrier_freq));
				}
				case (morse_wave_saw): {
					return fmodf(tmp * (float)(settings->carrier_freq), 1.0);
				}
				case (morse_wave_square): {
					return (float)(fmodf(
									   tmp * (float)(settings->carrier_freq),
									   1.f) > 0.5f);
				}
				default: {
					return 0.0;
				}
			}
		}
	}

	return 0.0;
}
