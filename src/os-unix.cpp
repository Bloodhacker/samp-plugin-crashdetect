// Copyright (c) 2011-2012, Zeex
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met: 
//
// 1. Redistributions of source code must retain the above copyright notice, this
//    list of conditions and the following disclaimer. 
// 2. Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution. 
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
// ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// // LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#include "os.h"

#include <csignal>
#include <cstdio>
#include <cstring>

#ifndef _GNU_SOURCE
	#define _GNU_SOURCE 1 // for dladdr()
#endif
#include <dlfcn.h> 

std::string os::GetModuleNameBySymbol(void *symbol) {
	char module[FILENAME_MAX] = "";

	if (symbol != 0) {
		Dl_info info;
		dladdr(symbol, &info);
		strcpy(module, info.dli_fname);
	}
	
	return std::string(module);
}

// The crash handler - it is set via SetCrashHandler()
static void (*crashHandler)() = 0;

// Previous SIGSEGV handler
static void (*previousSIGSEGVHandler)(int);

static void HandleSIGSEGV(int sig)
{
	if (::crashHandler != 0) {
		::crashHandler();
	}
	signal(sig, SIG_DFL);
}

void os::SetCrashHandler(void (*handler)()) {
	::crashHandler = handler;
	if (handler != 0) {
		::previousSIGSEGVHandler = signal(SIGSEGV, HandleSIGSEGV);
	} else {
		signal(SIGSEGV, ::previousSIGSEGVHandler);
	}
}

// The interrupt (Ctrl+C) handler - set via SetInterruptHandler
static void (*interruptHandler)();

// Previous SIGINT handler
static void (*previousSIGINTHandler)(int);

// Out SIGINT handler
static void HandleSIGINT(int sig) {
	if (::interruptHandler != 0) {
		::interruptHandler();
	}
	signal(sig, ::previousSIGINTHandler);
	raise(sig);
}

void os::SetInterruptHandler(void (*handler)()) {
	::interruptHandler = handler;
	::previousSIGINTHandler = signal(SIGINT, HandleSIGINT);
}
