#include "SD.hpp"

// This library only works with one SD card

#define VBUF_SIZE 1024

// Internal state for SD
static bool sd_initialized = false;
static FATFS fs;

static uint8_t wav_fbuf[128*1024];  // 128 KB buffer for reading WAV files
static bool wav_file_in_use = false;

// we might want to change these later to be configurable, but it's fine for now

/* Configuration of hardware SPI object */
static spi_t spi = {
	.hw_inst = spi0,  // SPI component
	.miso_gpio = 4,
	.mosi_gpio = 3,
	.sck_gpio = 2,	// GPIO number (not Pico pin number)
	//.baud_rate = 125 * 1000 * 1000 / 8  // 15625000 Hz
	//.baud_rate = 125 * 1000 * 1000 / 6  // 20833333 Hz
	.baud_rate = 125 * 1000 * 1000 / 4	// 31250000 Hz
	//.baud_rate = 125 * 1000 * 1000 / 2  // 62500000 Hz
};

/* SPI Interface */
static sd_spi_if_t spi_if = {
	.spi = &spi,  // Pointer to the SPI driving this card
	.ss_gpio = 0  // The SPI slave select GPIO for this SD card
};

/* Configuration of the SD Card socket object */
static sd_card_t sd_card = {
	.type = SD_IF_SPI,
	.spi_if_p = &spi_if	 // Pointer to the SPI interface driving this card
};

/* ===========================================================================
 */
// SD library functions
// I copied this from the demo file, should be fine, maybe

size_t sd_get_num() { return 1; }

/**
 * @brief Get a pointer to an SD card object by its number.
 *
 * @param[in] num The number of the SD card to get.
 *
 * @return A pointer to the SD card object, or @c NULL if the number is invalid.
 */
sd_card_t* sd_get_by_num(size_t num) {
	if (0 == num) {
		// The number 0 is a valid SD card number.
		// Return a pointer to the sd_card object.
		return &sd_card;
	} else {
		// The number is invalid. Return @c NULL.
		return NULL;
	}
}

/* ===========================================================================
 */
// SD Class functions

SD::SD(uint cs) { spi_if.ss_gpio = cs; }

bool SD::open() {
	FRESULT fr = f_mount(&fs, "", 1);
	if (FR_OK != fr) {
		// panic("f_mount error: %s (%d)\n", FRESULT_str(fr), fr);
		printf("f_mount error: %s (%d)\n", FRESULT_str(fr), fr);
		return false;
	}

	return true;
}

bool SD::close() {
	FRESULT fr = f_unmount("");
	if (FR_OK != fr) {
		// panic("f_unmount error: %s (%d)\n", FRESULT_str(fr), fr);
		printf("f_unmount error: %s (%d)\n", FRESULT_str(fr), fr);
		return false;
	}
	return true;
}

std::vector<std::string> SD::listFiles(const std::string& dir) {
	std::vector<std::string> files;

	DIR dp;
	FILINFO fno;
	FRESULT fr;

	fr = f_opendir(&dp, dir.c_str());
	if (FR_OK != fr) {
		panic("f_opendir error: %s (%d)\n", FRESULT_str(fr), fr);
	}

	while (true) {
		fr = f_readdir(&dp, &fno);
		if (FR_OK != fr || 0 == fno.fname[0]) {
			break;
		}
		files.push_back(fno.fname);
	}

	return files;
}

SDFile SD::openFile(const std::string& filename, char mode) { return SDFile(); }

/* ===========================================================================
 */
// SDFile Class functions

bool SDFile::open(const std::string& filename, const char mode) {
	FRESULT fr = f_open(&fil, filename.c_str(), FA_OPEN_APPEND | FA_WRITE | FA_READ);

	if (fr != FR_OK) {
		// potentially add error handling?
		printf("f_open error: %s (%d)\n", FRESULT_str(fr), fr);
		return false;
	}

	return true;
}

bool SDFile::close() {
	// fflush might not belong here, needs to be tested
	FRESULT fr = f_close(&fil);
	if ( fr != FR_OK ) {
		printf("f_close error: %s (%d)\n", FRESULT_str(fr), fr);
		return false;
	}

	return true;

}

/* ===========================================================================
 */
// TextFile Class functions

int TextFile::read(char* buffer, int numBytes) {
	uint bytes_read = 0;
	f_read(&fil, buffer, numBytes, &bytes_read);
	return bytes_read;
}

std::string TextFile::readLine(const char endl = '\n') {
	std::string line;

	char buffer[256];
	uint bytes_read = 0;

	while (f_read(&fil, buffer, 250, &bytes_read) == FR_OK) {
		for (int i = 0; i < bytes_read; i++) {
			if (buffer[i] == endl) {
				// Add up to the newline.
				line.append(buffer, i);

				// If we read more than the newline, push it back.
				f_lseek(&fil, f_tell(&fil) - sizeof(buffer) + i + 1);

				return line;
			}
			line += buffer[i];
		}
	}

	return line;
}

bool TextFile::writeLine(const std::string& line) {
	return f_puts(line.c_str(), &fil) != -1;
	// return f_printf(&fil, "%s\n", line.c_str()) >= 0;
	// f_puts(line.c_str(), &fil);
}

/* ===========================================================================
 */
// WaveFile Class functions

bool WaveFile::open(const std::string& filename) {
	FRESULT fr = f_open(&fil, filename.c_str(), FA_READ);

	dataSize = f_size(&fil);
	printf("dataSize: %d\n", dataSize);
	dataPtr = wav_file_in_use ? NULL : wav_fbuf;
	if (dataPtr == NULL) {
		printf("Error: Could not allocate memory for file\n");
		return false;
	}
	uint bytes_read = 0;

	f_read(&fil, dataPtr, dataSize, &bytes_read);

	if (bytes_read != dataSize) {
		printf("Error: read %d bytes, expected %d\n", bytes_read, dataSize);
		return false;
	}

	wavFile = WAV_ParseFileData(dataPtr);

	return true;
}

bool WaveFile::close() {

	wav_file_in_use = false;

	// fflush might not belong here, needs to be tested
	FRESULT fr = f_close(&fil);
	if ( fr != FR_OK ) {
		printf("f_close error: %s (%d)\n", FRESULT_str(fr), fr);
		return false;
	}

	return true;
}

WaveFile::~WaveFile() { close(); }

uint16_t WaveFile::getSampleRate() { return wavFile.header.sample_rate; }

uint16_t WaveFile::getNumChannels() {
	return wavFile.header.number_of_channels;
}

uint16_t WaveFile::getBitsPerSample() { return wavFile.header.bits_per_sample; }

uint32_t WaveFile::getNumSamples() {
	return dataSize / (wavFile.header.number_of_channels * wavFile.header.bits_per_sample / 8);
}

int WaveFile::readSamples(
	uint16_t* buffer, uint32_t numSamples, size_t offset = 0) {
	if (buffer == NULL) {
		return 0;
	}

	uint32_t bytesPerSample = getNumChannels() * getBitsPerSample() / 8;
	uint32_t bytesToRead = numSamples * 2;

	if (offset + bytesToRead > dataSize) {
		bytesToRead = dataSize - offset;
	}

	memcpy(buffer, dataPtr + offset, bytesToRead);

	return bytesToRead / 2;
}
