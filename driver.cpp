#include <iostream>
#include <getopt.h>
#include <string>

int main(int argc, char ** argv){
	// Command line input
	
	bool verbose = false, median = false, midpoint = false, transfers = false;
	bool VWAP = false, help = false, ttt = false;
	int ttt_size = 0;
	std::string * ttt_symbols = NULL;

	struct option longOps[]{
		{"verbose", no_argument, NULL, 'v'},
		{"median", no_argument, NULL, 'm'},
		{"midpoint", no_argument, NULL, 'p'},
		{"transfers", no_argument, NULL, 't'},
		{"VWAP", no_argument, NULL, 'w'},
		{"help", no_argument, NULL, 'h'},
		{"ttt", required_argument, NULL, 'g'},
	};
	int opt = 0, index = 0;
	while ((opt = getopt_long(argc, argv, "vmptwhg:", longOps, &index)) != -1){
		switch(opt){
			case 'v':
				verbose = true;
				break;
			case 'm':
				median = true;
				break;
			case 'p':
				midpoint = true;
				break;
			case 't':
				transfers = true;
				break;
			case 'w':
				VWAP = true;
				break;
			case 'h':
				help = true;
				break;
			case 'g':{
				ttt = true;
				// Create a new array of strings holding one more equity symbol 
				// than the previous one
				std::string * new_symbols = new std::string[ttt_size + 1];
				for(int i = 0; i < ttt_size; i++)
					new_symbols[i] = ttt_symbols[i];
				new_symbols[ttt_size] = optarg;
				// Delete the old array of strings and reassign the pointer
				delete[] ttt_symbols;
				ttt_symbols = new_symbols;
				new_symbols = NULL;
				// Increase the number of arguments by one
				ttt_size++;
				break;
			}
			case '?':
				break;
		}
	}
	return 0;
}
