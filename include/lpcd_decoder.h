#ifndef LPCD_DECODER_H
#define LPCD_DECODER_H

#include <iostream>
#include <algorithm>
#include <random>
#include <chrono>
#include <sstream>
#include <fstream>

#include <lpcd_fir.h>

const uint8_t C_P = 10; //konstanta LPC
const uint8_t C_GAIN_COUNT = 128;
const uint32_t C_LPC_COUNT = 512;
const uint32_t LENGTH_OF_WINDOW = 160;

template <typename T>
inline std::string toStdString(T v) {
	std::stringstream ss;
	ss << v;
	return ss.str();
}

enum DataDefinition {
	PARAMS = 1,
	GAINS,
	LAGS,

	MAX_DATA
};

struct DecoderException : public std::exception {
public:
	DecoderException(std::string const& message) : _message(message) { }
	~DecoderException() throw () { }
	const char* what() const throw () { return _message.c_str(); }

private:
	std::string _message;
};


class Decoder {
public:
	Decoder();

	double randomNumber();

	void loadCodebook(std::string const& filename);
	void loadGainCodebook(std::string const& filename);
	void loadData(std::string const& filename);
	void loadFiles(std::string const& codebookFilename, std::string const& gainFilename, std::string const& dataFilename);

	void decode();

	std::vector<double> genPulses(uint32_t lag, uint32_t& nextVoiced);
	std::vector<int16_t> synthesize();
	void normalize();

private:
	// initial data
	std::vector<double> _codebook[C_LPC_COUNT];
	std::vector<double> _gainCodebook;
	std::vector<uint32_t> _data[MAX_DATA];

	// decoded data
	std::vector< std::vector<double> > _decodedParams;
	std::vector<double> _decodedGains;

	// random num generator
	std::normal_distribution<double> _distribution;
	std::default_random_engine _generator;
};


#endif
