#include "genode_functions.h"

#ifdef __cplusplus

#include <base/env.h>
#include <base/log.h>

extern "C" {
#endif

//	using namespace Genode;

	void printConsole(char* text) {
		string _text = std::str(text);
		Genode::log(_text);
	}

#ifdef __cplusplus
} //end extern "C"
#endif
