#include "genode_functions.h"

#ifdef __cplusplus

#include <base/env.h>
#include <base/log.h>

extern "C" {
#endif

//	using namespace Genode;

	void printConsole(char* text) {
		std::string _textStr(text);
		Genode::log(_textStr);
	}

#ifdef __cplusplus
} //end extern "C"
#endif
