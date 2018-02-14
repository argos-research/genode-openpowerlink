#include "genode_functions.h"

#ifdef __cplusplus

#include <base/env.h>
#include <base/log.h>

#include <string>

extern "C" {
#endif

//	using namespace Genode;

	void printConsole(char* text) {
		Genode::log(std::string(text));
	}

#ifdef __cplusplus
} //end extern "C"
#endif
