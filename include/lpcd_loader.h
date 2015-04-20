#ifndef LPCD_LOADER_H
#define LPCD_LOADER_H

#include <vector>
#include <cstdint>
#include <random>
#include <chrono>
#include <algorithm>

namespace LPCD
{
	class Decoder
	{
		static const uint16_t CB_VECTOR_CNT = 512;
		static const uint8_t CB_GAINS_CNT = 128;
		static const uint8_t COEFF_CNT = 10;
		static const uint8_t FRAME_SAMPLES = 160;

		enum class COD_DATA : uint8_t
		{
			DATA = 0,
			GAIN = 1,
			LAG = 2,
			COLS = 3,
		};

	public:
		Decoder();
		~Decoder();

		void loadFiles(const std::string&, const std::string&, const std::string&);
		void decode();
		std::vector<int16_t> synthetize();
		std::vector<double> doFilter(double&, std::vector<double>&, std::vector<double>&);

	private:
		void loadCodeBook(const std::string& cbfile);
		void loadGains(const std::string& gcbfile);
		void loadCod(const std::string& codfile);


	private:
		std::vector<double>m_codebook[CB_VECTOR_CNT];
		std::vector<double>m_cbgains;
		std::vector<uint32_t>m_data[static_cast<std::size_t>(COD_DATA::COLS)];
		std::vector<std::vector<double>>m_asym;
		std::vector<double>m_gsym;

		std::normal_distribution<double> distribution;
		std::default_random_engine generator;
		std::vector<double> coefficients;
	};
}

#endif 
