#ifndef LPCD_WAVWRITER_H
#define LPCD_WAVWRITER_H

#include <lpcd_platform.h>
#include <string>
#include <vector>
#include <fstream>

namespace LPCD
{
	class WavWriter
	{
	public:
		WavWriter();
		WavWriter(int sr, int ch, int f, std::string of);
		~WavWriter();

		template <typename T>
		void writeFile(std::vector<T>& data);

	private:
		template <typename T>
		void writeHeader(std::vector<T>& data);

		template <typename T>
		void writeData(std::vector<T>& data);

		template <typename T>
		inline void write(const T& type);

	private:
		i32_t m_sampleRate;
		i16_t m_channels;
		i16_t m_format;
		std::ofstream m_stream;
	};



	template <typename T>
	inline void WavWriter::writeFile(std::vector<T>& data)
	{
		writeHeader(data);
		writeData(data);

		m_stream.close();
	}

	template <typename T>
	inline void WavWriter::writeHeader(std::vector<T>& data)
	{
		m_stream.write("RIFF", 4);
		write<i32_t>(36 + data.size() * sizeof(T));
		m_stream.write("WAVE", 4);
		m_stream.write("fmt ", 4);
		write<i32_t>(16);
		write<i16_t>(1);
		write<i16_t>(m_channels);
		write<i32_t>(m_sampleRate);
		write<i32_t>(m_sampleRate * m_channels * sizeof(T));
		write<i16_t>(m_channels * sizeof(T));
		write<i16_t>(8 * sizeof(T));
	}

		template <typename T>
	inline void WavWriter::writeData(std::vector<T>& data)
	{
		m_stream.write("data", 4);
		write<i32_t>(sizeof(T) * data.size() * m_channels);

		#ifdef CPP11_SUPPORTED
		for (auto& it : data)
		{
			write<T>(it);
		}
		#else
			//DO SOMETHING
		#endif
	}

	template <typename T>
	inline void WavWriter::write(const T& type)
	{
		m_stream.write(reinterpret_cast<const char*>(&type), sizeof(T));
	}
}

#endif
