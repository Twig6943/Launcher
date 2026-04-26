#include "pch.h"
#include "VersionInfo.h"

#define CYPRESS_BUILDDATE (__DATE__ " " __TIME__)

std::string GetCypressVersion()
{
	return std::format("{}.{}.{} ({})", CYPRESS_VERSION_MAJOR, CYPRESS_VERSION_MINOR, CYPRESS_VERSION_PATCH, CYPRESS_BUILDDATE);
}
