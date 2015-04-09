#include <lpcd_decoder.h>
#include <lpcd_utils.h>

Decoder::Decoder() {
	uint32_t seed = std::chrono::system_clock::now().time_since_epoch().count();
	_generator = std::default_random_engine(seed);
	_distribution = std::normal_distribution<double>(0.0, 1.0);
}

void Decoder::loadCodeBook(const std::string& cbfile)
{
	std::ifstream cbis;

	try {
		cbis.open(cbfile);
	}
	catch (std::ifstream::failure e) {
		std::cerr << "Failed to open codebook file!" << std::endl;
	}

	std::string tmp;
	double cb_tmp;
	uint8_t c_count = 0;

	while (std::getline(cbis, tmp)) {
		if (c_count >= COEFF_CNT)
			throw std::exception("Input file contains more data than expected!");

		std::stringstream ss(tmp);

		uint8_t i = 0;

		while (ss >> cb_tmp) {
			m_codebook[i].push_back(cb_tmp);
			i++;
		}

		//Check if we loaded exactly 512 vectors
		if (i != CB_VECTOR_CNT) {
			throw std::exception("Number of vectors on the line doesn't match predefined value!");
		}
	}

}

void Decoder::loadGainCodebook(std::string const& filename) {
	std::ifstream file(filename.c_str());
	if (!file) {
		throw DecoderException(std::string("Gain codebook file " + filename + " could not be opened for reading."));
	}

	double tmp;
	while (file >> tmp) {
		_gainCodebook.push_back(tmp);
	}

	uint32_t gainCodebookSize = _gainCodebook.size();
	if (gainCodebookSize != C_GAIN_COUNT) {
		throw DecoderException(std::string("Wrong gain codebook size (size=" + toStdString(gainCodebookSize) + ")"));
	}

	file.close();
}

void Decoder::loadData(std::string const& filename) {
	std::ifstream file(filename.c_str());
	if (!file) {
		throw DecoderException(std::string("Data file " + filename + " could not be opened for reading."));
	}

	uint32_t tmp;
	std::string buffer;
	while (std::getline(file, buffer)) {

		std::stringstream ss(buffer);
		for (int i = PARAMS; i < MAX_DATA; ++i) {
			ss >> tmp;
			if (i == PARAMS || i == GAINS) // input indexes start at 1
				--tmp;

			switch (i) {
			case PARAMS:
				if (tmp >= C_LPC_COUNT)
					throw DecoderException(std::string("Wrong LPC codebook index (tmp=" + toStdString(tmp) + ")"));

				break;
			case GAINS:
				if (tmp >= C_GAIN_COUNT)
					throw DecoderException(std::string("Wrong gain codebook size (tmp=" + toStdString(tmp) + ")"));

				break;
			}

			_data[i].push_back(tmp);
		}
	}

	file.close();
}

void Decoder::loadFiles(std::string const& codebookFilename, std::string const& gainFilename, std::string const& dataFilename) {
	loadCodebook(codebookFilename);
	loadGainCodebook(gainFilename);
	loadData(dataFilename);
}

void Decoder::decode() {
	for (std::vector<uint32_t>::iterator ii = _data[PARAMS].begin(); ii != _data[PARAMS].end(); ++ii) {
		uint32_t index = *ii;
		_decodedParams.push_back(_codebook[index]);
	}

	for (std::vector<uint32_t>::iterator ii = _data[GAINS].begin(); ii != _data[GAINS].end(); ++ii) {
		uint32_t index = *ii;
		_decodedGains.push_back(_gainCodebook[index]);
	}
}

double Decoder::randomNumber() {
	return _distribution(_generator);
}

std::vector<double> Decoder::genPulses(uint32_t lag, uint32_t& nextVoiced) {
	std::vector<double> pulses(LENGTH_OF_WINDOW, 0.0);

	// unvoiced
	// generates some random noise    
	if (lag == 0) {
		for (std::vector<double>::iterator ii = pulses.begin(); ii != pulses.end(); ++ii) {
			*ii = randomNumber();
		}
		// voiced
	}
	else {
		std::vector<uint32_t> pulseIndexes;
		for (; nextVoiced < LENGTH_OF_WINDOW; nextVoiced += lag) {
			pulseIndexes.push_back(nextVoiced);
		}

		uint32_t maxPulseIndex = *(std::max_element(pulseIndexes.begin(), pulseIndexes.end()));
		nextVoiced = maxPulseIndex + lag - LENGTH_OF_WINDOW;

		for (std::vector<uint32_t>::const_iterator ii = pulseIndexes.begin(); ii != pulseIndexes.end(); ++ii) {
			pulses[*ii] = 1.0;
		}
	}

	double sum = 0.0;
	for (std::vector<double>::iterator ii = pulses.begin(); ii != pulses.end(); ++ii) {
		sum += pow((*ii), 2.0);
	}
	sum /= static_cast<double> (LENGTH_OF_WINDOW);
	for (std::vector<double>::iterator ii = pulses.begin(); ii != pulses.end(); ++ii) {
		*ii /= sqrt(sum);
	}

	return pulses;
}

std::vector<int16_t> Decoder::synthesize() {

	const std::vector<uint32_t> lags = _data[LAGS];

	std::vector<double> initFilterConditions(C_P, 0.0);
	std::vector<int16_t> result;

	uint32_t nextVoiced = 0;
	LPCD::FIR firFilter;

	double gain;
	uint32_t lag;
	std::vector<double> coeffs;

	for (uint32_t i = 0; i < _decodedGains.size(); ++i) { // loop through all gains

		coeffs = _decodedParams[i];
		coeffs.insert(coeffs.begin(), 1.0); // prepends 1.0
		gain = _decodedGains[i];
		lag = lags[i];

		std::vector<double> pulses = genPulses(lag, nextVoiced);

		firFilter.setCoeffs(coeffs);
		std::vector<double> processed = firFilter.process(gain, pulses, initFilterConditions);

		clip(processed, -1.0, 1.0);

		for (auto& ii : processed) {
			result.push_back(static_cast<int16_t>(ii * static_cast<double>(INT16_MAX)));
		}


	}
	return result;
}



