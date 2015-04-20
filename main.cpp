#include <iostream>

#include <string>
#include <vector>

#include <lpcd_platform.h>
#include <lpcd_loader.h>
#include <lpcd_wavWriter.h>

int main(int argc, char** argv)
{
	if (argc != 5) {
		fprintf(stderr, "Chybne zadane parametry!\nPouziti: zre_proj1_win.exe cb_lpc.txt cb_gain.txt in.cod out.wav\n");
		//getchar();
		return 1;
	}
	else {
		std::string codebookFilename = argv[1];
		std::string gainCodebookFilename = argv[2];
		std::string inFilename = argv[3];
		std::string outFilename = argv[4];
		std::vector<int16_t> sound;

		LPCD::WavWriter ww(8000, 1, 1, outFilename);

		LPCD::Decoder decoder;
		decoder.loadFiles(codebookFilename, gainCodebookFilename, inFilename);
		decoder.decode();
		sound = decoder.synthetize();

		ww.writeFile(sound);
	}

	/*Implementation without params*/
	/*
	std::string codebookFilename = "cb512.txt";
	std::string gainCodebookFilename = "gcb128.txt";
	std::string inFilename = "testmale.cod";
	std::string outFilename = "out.wav";
	std::vector<int16_t> sound;

	LPCD::WavWriter ww(8000, 1, 1, outFilename);

	LPCD::Decoder decoder;
	decoder.loadFiles(codebookFilename, gainCodebookFilename, inFilename);
	decoder.decode();
	//decoder.synthetize();
	*/

	return 0;
}