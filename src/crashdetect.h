// Copyright (c) 2011 Zeex
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef CRASHDETECT_H
#define CRASHDETECT_H

#include <stack>
#include <string>

#include "amxdebuginfo.h"

#define AMX_EXEC_GDK (-10) // Compatibility with GDK

class Crashdetect {
public:	
	static void ReportCrash();
	static void KeyboardInterrupt();

	explicit Crashdetect(AMX *amx);

	int AmxDebug();
	int AmxCallback(cell index, cell *result, cell *params);
	int AmxExec(cell *retval, int index);

	void HandleCrash();
	void HandleNativeError(int index);
	void HandleRuntimeError(int index, int error);	
	
	void PrintCallStack() const;

private:
	AMX         *amx_;
	AMX_HEADER  *amxhdr_;
	AMXDebugInfo debugInfo_;

	// Path to the .amx file
	std::string  amxFileName_;

	AMX_CALLBACK prevCallback_;
	AMX_DEBUG    prevDebugHook_;	

	class CSEntry {
	public:
		CSEntry(AMX *amx, cell index)
			: amx_(amx), index_(index)
		{}

		AMX *GetAmx() const 
			{ return amx_; }

		cell GetIndex() const 
			{ return index_; }
	private:
		AMX *amx_;
		cell index_;
	};

	// Active public/native calls
	static std::stack<CSEntry> publicCallStack_;
	static std::stack<CSEntry> nativeCallStack_;

	// Set to true on Runtime Error (to prevent double-reporting)
	static bool errorCaught_;
};

#endif // !CRASHDETECT_H