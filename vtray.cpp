// implement vtray main here
#include <iostream>
#include "json_parser.hpp"
#include "render_image.hpp"

using namespace std;

int main(int argc, char *argv[])
{
	try {
		if (argc == 3) {
			string inputFilename = argv[1];
			string outputFilename = argv[2];
			Render b(inputFilename, true);
			//b.findAllIntersect(1);
			b.createThreads(1);
			b.createImage(outputFilename);
		}
		else if (argc == 5) {
			string command = argv[1];
			if (command != "-t") {
				cerr << "Error: Invalid command" << endl;
				return EXIT_FAILURE;
			}
			string inputFilename = argv[3];
			string outputFilename = argv[4];
			Render b(inputFilename, true);
			b.createThreads(stoi(argv[2]));
			b.createImage(outputFilename);
		}
		else if (argc != 3 && argc != 5) {
			cerr << "Error: Incorrect number of arguments." << endl;
			throw runtime_error("bad args");
		}
	}
	catch (exception& ex)
	{
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}