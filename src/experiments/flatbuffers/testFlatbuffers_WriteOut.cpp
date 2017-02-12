#include <vector>
#include <iostream>
#include <fstream>

#include "flatbuffers/idl.h"

#include "bogusSchema_generated.h"

char *readSchema(const char *filename)
{
	std::ifstream inputFile;

	std::cout << "Reading in " << filename << "...\n";

	// open with std::ios::ate so buffer pointer starts at end. tellg() tells us the size,
	//  then we move the pointer back to the start and read in the entire file
	// This sucks and I should be ashamed to have written it
	inputFile.open(filename, std::ios::ate);
	if (inputFile.is_open())
	{
		std::streampos size = inputFile.tellg();
		char *memblock = new char[size];
		inputFile.seekg(0, std::ios::beg);
		inputFile.read(memblock, size);
		inputFile.close();

		std::cout << "Successfully read in " << filename << "\n\t(" << size << " bytes)\n";
		return memblock;
	}
	else
		std::cout << "Failed to read in " << filename << "\n";

	return nullptr;
}

char *readBinary(const char *filename)
{
	std::ifstream inputFile;

	std::cout << "Reading in " << filename << "...\n";

	// open with std::ios::ate so buffer pointer starts at end. tellg() tells us the size,
	//  then we move the pointer back to the start and read in the entire file
	// This sucks and I should be ashamed to have written it
	inputFile.open(filename, std::ios::binary | std::ios::ate);
	if (inputFile.is_open())
	{
		std::streampos size = inputFile.tellg();
		char *memblock = new char[size];
		inputFile.seekg(0, std::ios::beg);
		inputFile.read(memblock, size);
		inputFile.close();

		std::cout << "Successfully read in " << filename << "\n\t(" << size << " bytes)\n";
		return memblock;
	}
	else
		std::cout << "Failed to read in " << filename << "\n";

	return nullptr;
}

void testFlatbufferToJSON()
{
	const char *outputFilename = "Output.json";
	const char *flatbufferFilename = "SavedHelloForWrite.bin";
	const char *schemaFilename = "bogusSchema.flb";
	const char *includePaths = {
	    "/home/macoy/Development/code/repositories/galavant/src/experiments/flatbuffers"};

	char *schemaBlock = readSchema(schemaFilename);
	char *memblock = readBinary(flatbufferFilename);

	if (memblock && schemaBlock)
	{
		const Galavant::Test::Hello *readInHello = Galavant::Test::GetHello(memblock);
		// printHello(readInHello);

		flatbuffers::Parser parser;
		parser.Parse(schemaBlock, &includePaths, schemaFilename);

		std::string outputString = "";
		flatbuffers::GenerateText(parser, memblock, &outputString);

		std::cout << outputString << "\n";

		//std::cout << "Generating text file...\n";
		//flatbuffers::GenerateTextFile(parser, memblock, outputFilename);

		delete memblock;
		delete schemaBlock;
	}
}

int main()
{
	std::cout << "Test Flatbuffers\n";

	testFlatbufferToJSON();

	return 1;
}