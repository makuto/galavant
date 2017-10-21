#include "Logging.hpp"

#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <algorithm>

#include <iostream>

namespace gv
{
namespace Logging
{
Record::Record(Severity newSeverity, const char* func, size_t line, const char* file)
    : severity(newSeverity), Function(func), Line(line), File(file), Offset(0)
{
}

Record& Record::operator<<(char data)
{
	Offset += snprintf(OutBuffer + Offset, sizeof(OutBuffer) - Offset, "%d", (int)data);
	return *this;
}

Record& Record::operator<<(const char* data)
{
	Offset += snprintf(OutBuffer + Offset, sizeof(OutBuffer) - Offset, "%s", data);
	return *this;
}

Record& Record::operator<<(const int data)
{
	Offset += snprintf(OutBuffer + Offset, sizeof(OutBuffer) - Offset, "%d", data);
	return *this;
}

Record& Record::operator<<(const unsigned int data)
{
	Offset += snprintf(OutBuffer + Offset, sizeof(OutBuffer) - Offset, "%u", data);
	return *this;
}

Record& Record::operator<<(const float data)
{
	Offset += snprintf(OutBuffer + Offset, sizeof(OutBuffer) - Offset, "%f", data);
	return *this;
}

Record& Record::operator<<(const bool data)
{
	Offset +=
	    snprintf(OutBuffer + Offset, sizeof(OutBuffer) - Offset, "%s", data ? "True" : "False");
	return *this;
}

Record& Record::operator<<(const size_t data)
{
	Offset += snprintf(OutBuffer + Offset, sizeof(OutBuffer) - Offset, "%lu", (unsigned long)data);
	return *this;
}

void FormatFuncName(char* buffer, const char* func, size_t bufferSize)
{
	const char* funcBegin = func;
	const char* funcEnd = strchr(funcBegin, '(');

	if (!funcEnd)
	{
		strncpy(buffer, func, bufferSize - 1);
		return;
	}

	for (const char* i = funcEnd - 1; i >= funcBegin;
	     --i)  // search backwards for the first space char
	{
		if (*i == ' ')
		{
			funcBegin = i + 1;
			break;
		}
	}

	unsigned long numCharsToCopy = std::min((funcEnd - funcBegin), (long)bufferSize - 1);
	strncpy(buffer, funcBegin, numCharsToCopy);
	buffer[numCharsToCopy] = '\0';
}

Logger::Logger(Severity maxSeverity, CustomLogOutputFunc customOutputFunc)
    : MaxSeverity(maxSeverity), CustomOutputFunc(customOutputFunc)
{
	Singleton = this;
	// std::cout << "Logger initialized at " << Singleton << "\n";
}
bool Logger::checkSeverity(Severity severity) const
{
	return severity <= MaxSeverity;
}

void Logger::operator+=(const Record& record)
{
	if (CustomOutputFunc)
		CustomOutputFunc(record);
	else
	{
		static char funcNameBuffer[256];
		FormatFuncName(funcNameBuffer, record.Function, sizeof(funcNameBuffer));
		std::cout << severityToString(record.severity) << " " << record.File << ":"
		          << funcNameBuffer << "():" << record.Line << ": " << record.OutBuffer << "\n";
	}
}
Logger* Logger::GetSingleton()
{
	// TODO: @Stability: This could be a problem. We can't print yet though...
	// if (!Singleton)
	// 	std::cout << "Warning: something tried to access Logger before any Logger had been "
	// 	             "initialized!\n";
	return Singleton;
}

Logger* Logger::Singleton = nullptr;
}
}