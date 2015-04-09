#include <lpcd_wavWriter.h>

namespace LPCD
{
	WavWriter::WavWriter()
	{
	}

	WavWriter::~WavWriter()
	{
		m_stream.close();
	}

	WavWriter::WavWriter(int sr, int ch, int f, std::string of)
	{
		m_sampleRate = (i32_t)sr;
		m_channels = (i16_t)ch;
		m_format = (i16_t)f;

		m_stream = std::ofstream(of, std::ios::binary);
	}
}