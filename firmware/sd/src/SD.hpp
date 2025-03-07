#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "f_util.h"
#include "ff.h"
#include "file_stream.h"
#include "hardware/spi.h"
#include "hw_config.h"
#include "pico/stdlib.h"
#include "wave/wave.h"

#ifdef __cplusplus
}  // extern "C"
#endif

#include <string>
#include <vector>

class SDFile {
   public:
	SDFile() = default;
	~SDFile() = default;

	bool open(const std::string& filename, const char mode);
	bool close();

	bool isOpen() { return fil.obj.fs != NULL; }

   protected:
	FIL fil;
};

class TextFile : public SDFile {
   public:
	TextFile() = default;
	~TextFile() = default;

	std::string readLine(const char endl);
	int read(char* buffer, int numBytes);
	bool writeLine(const std::string& line);

	size_t size() { return f_size(&fil); }
};

class WaveFile : public SDFile {
   public:
	WaveFile() = default;
	~WaveFile();

	bool open(const std::string& filename);
	bool close();

	uint16_t getSampleRate();
	uint16_t getNumChannels();
	uint16_t getBitsPerSample();
	uint32_t getNumSamples();

	int readSamples(uint16_t* buffer, uint32_t numSamples, size_t offset);

   private:
	WAVFile wavFile;
	uint8_t* dataPtr;
	size_t dataSize;
};

class SD {
   public:
	SD(uint cs);
	~SD() = default;

	bool open();
	bool close();

	std::vector<std::string> listFiles(const std::string& dir);

	SDFile openFile(const std::string& filename, char mode);

   private:
	FATFS fs;
};
