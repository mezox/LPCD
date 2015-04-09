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

	void Decoder::synthetize()
	{
		std::vector<double> init_conditions(COEFF_CNT, 0.0);
		std::vector<uint32_t> sound(FRAME_SAMPLES * m_cbgains.size(), 0);

		init_conditions.insert(init_conditions.begin(), 1.0);
	}
}