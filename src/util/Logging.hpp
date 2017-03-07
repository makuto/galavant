#pragma once

// Including Plog here for convenience and abstraction; that way users can just include Logging.hpp
#include <plog/Log.h>

namespace gv
{
void InitializeConsoleOnlyLogging();

// Feel free to pass in NULL to get the default log file
void InitializeFileOnlyLogging(const char* filename);
}