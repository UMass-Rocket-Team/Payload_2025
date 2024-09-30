#pragma once

#include <stdint.h>

/// @brief Type declaration for different sound wave formats.
typedef enum {
	/// @brief sine wave
	morse_wave_sine = 0,
	/// @brief sawtooth wave
	morse_wave_saw = 1,
	/// @brief 50% duty cycle square wave
	morse_wave_square = 2
} morse_wave_t;

/// @brief Morse code generator settings.
typedef struct {
	// The type of sound wave the signal will be encoded as.
	morse_wave_t wave_type;

	// The frequency of the signal's carrier tone.
	float carrier_freq;

	// The duration one short pulse will take, all other signal durations are
	// derived from this.
	float time_interval;

	// Sample rate of the encoded audio.
	int sample_rate;

	// Pointer to morse code message to be encoded.
	const char* code;

	// Audio output buffer.
	float* output_buffer;

	// Length of the output buffer.
	int output_buffer_len;

} morse_input_t;

/// @brief Convert a string to morse code. All non-morse compatible characters
/// will be ignored.
/// @param str pointer to a c-string to convert
/// @param morse pointer to a c-string that will hold the translated morse code
/// @param maxlen maximum length of the output buffer
/// @return number of characters encoded, or -1 upon failure
int str2morse(const char* str, char* morse, int maxlen);

/// @brief Calculates the number of characters needed to store the morse code
/// encoding of a given string
/// @param str Pointer to input string
/// @return Number of characters in the encoded message, or -1 if an error
/// occured
int get_morse_string_len(const char* str);

/// @brief Fills an array with an audio signal encoding of the input message.
/// @param settings Settings for encoding.
float generate_morse_signal(morse_input_t* settings, float t);

/// @brief Calculates the time required to transmit the message in multiples of
/// settings.time_interval
/// @param settings pointer to the morse code settings
/// @return Time required for transmission in multiples of
/// settings.time_interval
int get_morse_duration(morse_input_t* settings);