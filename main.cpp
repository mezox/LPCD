#include <iostream>

#include <string>
#include <vector>

#include <lpcd_platform.h>
#include <lpcd_loader.h>
#include <lpcd_wavWriter.h>

int main(int argc, char** argv)
{
	std::string codebookFilename = "cb512.txt";
	std::string gainCodebookFilename = "gcb128.txt";
	std::string inFilename = "testmale.cod";
	std::string outFilename = "out.wav";
	std::vector<int16_t> sound;

	LPCD::WavWriter ww(8000, 1, 1, outFilename);

	LPCD::Decoder decoder;
	decoder.loadFiles(codebookFilename, gainCodebookFilename, inFilename);
	decoder.decode();
	decoder.synthetize();

	return 0;
}