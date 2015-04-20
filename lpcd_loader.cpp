#include <lpcd_loader.h>

#include <iostream>
#include <ostream>
#include <fstream>
#include <sstream>
#include <lpcd_platform.h>

namespace LPCD
{
	Decoder::Decoder()
	{
		uint32_t seed = std::chrono::system_clock::now().time_since_epoch().count();
		generator = std::default_random_engine(seed);
		distribution = std::normal_distribution<double>(0.0, 1.0);
	}

	Decoder::~Decoder()
	{

	}

	void Decoder::loadCodeBook(const std::string& cbfile)
	{
		std::ifstream cb_ifs;

		try {
			cb_ifs.open(cbfile.c_str());

			std::string tmp;
			double cb_tmp;
			uint8_t c_count = 0;

			while (std::getline(cb_ifs, tmp)) {
				if (c_count >= COEFF_CNT)
					throw std::exception("Input file contains more data than expected!");

				std::stringstream ss(tmp);
				uint16_t i = 0;

				while (ss >> cb_tmp)
				{
					m_codebook[i].push_back(cb_tmp);
					i++;
				}

				if (i != CB_VECTOR_CNT) {
					throw std::exception("Number of vectors on the line doesn't match predefined value!");
				}

				c_count++;
			}
		}
		catch (std::ifstream::failure e) {
			std::cerr << "Failed to open codebook file!" << std::endl;
		}

		cb_ifs.close();
	}

	void Decoder::loadGains(const std::string& cbgfile)
	{
		std::ifstream g_ifs;

		try {
			g_ifs.open(cbgfile);

			std::string tmp;
			double g_tmp;
			uint8_t g_count = 0;

			while (g_ifs >> g_tmp) {
				m_cbgains.push_back(g_tmp);
			}

			//Check if we loaded exactly 128 gains
			if (m_cbgains.size() != CB_GAINS_CNT) {
				throw std::exception("Number of loaded gains doesn't match 128!");
			}
		}
		catch (std::ifstream::failure e) {
			std::cerr << "Failed to open gains codebook file!" << std::endl;
		}

		g_ifs.close();
	}

	void Decoder::loadCod(const std::string& codfile)
	{
		std::ifstream cod_ifs;

		try {
			cod_ifs.open(codfile);

			std::string tmp;
			uint16_t cod_tmp;

			while (std::getline(cod_ifs, tmp)) {
				std::stringstream ss(tmp);

				for (uint8_t i = 0; i < static_cast<size_t>(COD_DATA::COLS); i++) {
					ss >> cod_tmp;

					/*if (i == 0 || i == 1) {
					--cod_tmp;
					}*/
					--cod_tmp;

					m_data[i].push_back(cod_tmp);
				}
			}

			if (m_data[0].size() != m_data[1].size() && m_data[0].size() != m_data[2].size())
				throw std::exception("Invalid input data in .cod file!");
		}
		catch (std::ifstream::failure e) {
			std::cerr << "Failed to open cod file!" << std::endl;
		}

		cod_ifs.close();
	}

	void Decoder::loadFiles(const std::string& cbfile, const std::string& gcbfile, const std::string& codfile)
	{
		loadCodeBook(cbfile);
		loadGains(gcbfile);
		loadCod(codfile);
	}

	void Decoder::decode()
	{
#ifdef CPP11_SUPPORTED
		for (auto& i : m_data[static_cast<std::size_t>(COD_DATA::DATA)]) {
			m_asym.push_back(m_codebook[i]);
		}

		for (auto& i : m_data[static_cast<std::size_t>(COD_DATA::GAIN)]) {
			m_gsym.push_back(m_cbgains[i]);
		}
#else
		//DO IT OLD WAY
#endif
	}

	std::vector<double> Decoder::doFilter(double& gain, std::vector<double>& input, std::vector<double>& initConditions) {
		std::vector<double> output(input.size(), 0.0);

		for (int i = 0; i < output.size(); i++) {
			output[i] = gain * input[i] + initConditions[0];
			for (int j = 1; j < coefficients.size() - 1; j++) {
				initConditions[j - 1] = initConditions[j] - coefficients[j] * output[i];
			}
		}

		return output;
	}

	std::vector<int16_t> Decoder::synthetize()
	{
		std::vector<uint32_t> lags = m_data[static_cast<std::size_t>(COD_DATA::LAG)];

		std::vector<double> initConditions(COEFF_CNT, 0.0);
		std::vector<int16_t> result;

		double gain;
		uint32_t lag;
		int nextVoiced = 0;

		for (int i = 0; i < m_gsym.size(); i++) {
			coefficients = m_asym[i];
			coefficients.insert(coefficients.begin(), 1.0);
			gain = m_gsym[i];
			lag = lags[i];

			std::vector<double>e(FRAME_SAMPLES, 0.0);

			if (lag == 0 || lag == 65535) {
				for (int i = 0; i < FRAME_SAMPLES; i++) {
					e[i] = distribution(generator);
				}
			}
			else {
				int step = nextVoiced;
				std::vector<int> position;
				while (step <= FRAME_SAMPLES) {
					position.push_back(step);
					step += lag;
				}
				auto maximum = max_element(std::begin(position), std::end(position));
				nextVoiced = *maximum + lag - FRAME_SAMPLES;
				for (int i = 0; i < position.size() - 1; i++) {
					e[position[i]] = 1;
				}
			}

			double power = 0;
			for (int i = 0; i < e.size(); i++) {
				power += pow(e[i], 2);
			}

			power /= FRAME_SAMPLES;

			for (int i = 0; i < e.size() - 1; i++) {
				e[i] = e[i] / sqrt(power);
			}

			std::vector<double> filter = doFilter(gain, e, initConditions);;

			for (std::vector<double>::iterator j = filter.begin(); j != filter.end(); j++) {
				result.push_back((*j * INT16_MAX));
			}

		}
		//getchar();
		return result;

	}
}