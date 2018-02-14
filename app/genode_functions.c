#ifdef __cplusplus

#include <base/env.h>
#include <base/printf.h>
#include <base/log.h>

extern "C" {
#endif
	
	using namespace Genode;
	
	void printConsole(char* text) {
		Genode::log(text);
	}

#ifdef __cplusplus
} //end extern "C"
#endif
