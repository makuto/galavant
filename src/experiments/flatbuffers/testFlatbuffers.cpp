#include <vector>
#include <iostream>
#include <fstream>

#include "bogusSchema_generated.h"
#include "lastBogusSchema_generated.h"

// This is awful code written only to test flatbuffers. It should be burned in hell.

void printHello(const Galavant::Test::Hello *hello)
{
	if (hello)
	{
		std::string message = hello->message() ? hello->message()->message()->str() : "";
		std::cout << "From buffer:\n\tstatus: " << Galavant::Test::EnumNameFuckYou(hello->status())
		          << " value: " << hello->value() << " message: \"" << message << "\"\n";
	}
}

void writeBuffer(flatbuffers::FlatBufferBuilder &builder)
{
	std::ofstream outputFile;

	std::cout << "Writing out SavedHello.bin...\n";

	outputFile.open("SavedHello.bin", std::ios::binary);
	if (outputFile.is_open())
	{
		// outputFile.seekg(0, std::ios::beg);
		outputFile.write((char *)builder.GetBufferPointer(), builder.GetSize());
		outputFile.close();

		std::cout << "Done writing to SavedHello.bin\n\t(" << builder.GetSize() << " bytes)\n";
	}
	else
		std::cout << "Failed to write out to SavedHello.bin\n";
}

char *readBuffer()
{
	std::ifstream inputFile;

	std::cout << "Reading in SavedHello.bin...\n";

	// open with std::ios::ate so buffer pointer starts at end. tellg() tells us the size,
	//  then we move the pointer back to the start and read in the entire file
	// This sucks and I should be ashamed to have written it
	inputFile.open("SavedHello.bin", std::ios::binary | std::ios::ate);
	if (inputFile.is_open())
	{
		std::streampos size = inputFile.tellg();
		char *memblock = new char[size];
		inputFile.seekg(0, std::ios::beg);
		inputFile.read(memblock, size);
		inputFile.close();

		std::cout << "Successfully read in SavedHello.bin\n\t(" << size << " bytes)\n";
		return memblock;
	}
	else
		std::cout << "Failed to read in SavedHello.bin\n";

	return nullptr;
}

void createPlaceholderHelloArray(std::vector<flatbuffers::Offset<Galavant::Test::Hello>> &array,
                                 flatbuffers::FlatBufferBuilder &builder, int count)
{
	for (int i = 0; i < count; i++)
	{
		flatbuffers::Offset<flatbuffers::String> messageString =
		    builder.CreateString("This is the message!!!1 hello codegomad, mckenna and 123ran");

		flatbuffers::Offset<Galavant::Test::HelloReply> message =
		    Galavant::Test::CreateHelloReply(builder, messageString);

		flatbuffers::Offset<Galavant::Test::Hello> testHello = Galavant::Test::CreateHello(
		    builder, Galavant::Test::FuckYou::FuckYou_FuckYouToo, i, message);

		array.push_back(testHello);
	}
}

void testHellos()
{
	flatbuffers::FlatBufferBuilder builder;

	flatbuffers::Offset<flatbuffers::String> messageString =
	    builder.CreateString("This is the message!!!1 hello codegomad and 123ran");

	flatbuffers::Offset<Galavant::Test::HelloReply> message =
	    Galavant::Test::CreateHelloReply(builder, messageString);

	flatbuffers::Offset<Galavant::Test::Hello> testHello = Galavant::Test::CreateHello(
	    builder, Galavant::Test::FuckYou::FuckYou_FuckYouToo, 58008, message);

	builder.Finish(testHello);

	const Galavant::Test::Hello *actualHello = Galavant::Test::GetHello(builder.GetBufferPointer());

	printHello(actualHello);

	writeBuffer(builder);

	char *memblock = readBuffer();
	if (memblock)
	{
		const Galavant::Test::Hello *readInHello = Galavant::Test::GetHello(memblock);
		printHello(readInHello);
		delete memblock;
	}
}

void testHelloDict()
{
	flatbuffers::FlatBufferBuilder builder;
	std::vector<flatbuffers::Offset<Galavant::Test::Hello>> helloArray;

	createPlaceholderHelloArray(helloArray, builder, 10000);

	flatbuffers::Offset<Galavant::Test::HelloDict> helloDict =
	    Galavant::Test::CreateHelloDictDirect(builder, &helloArray);

	builder.Finish(helloDict);

	writeBuffer(builder);

	char *memblock = readBuffer();
	if (memblock)
	{
		const Galavant::Test::HelloDict *readInHelloDict = Galavant::Test::GetHelloDict(memblock);

		if (readInHelloDict)
		{
			const flatbuffers::Vector<flatbuffers::Offset<Galavant::Test::Hello>> *helloArray =
			    readInHelloDict->helloArray();

			if (helloArray)
			{
				for (unsigned int i = 0; i < helloArray->Length(); i++)
				{
					const Galavant::Test::Hello *readInHello = helloArray->Get(i);

					if (readInHello)
						printHello(readInHello);
				}
			}
		}

		delete memblock;
	}
}

int main()
{
	std::cout << "Test Flatbuffers\n";

	testHellos();
	testHelloDict();

	return 1;
}