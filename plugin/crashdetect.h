// Copyright (c) 2011-2013 Zeex
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice,
//    this list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.

#ifndef CRASHDETECT_H
#define CRASHDETECT_H

#include <map>
#include <stack>
#include <string>

#include <amx/amx.h>

#include "amxdebuginfo.h"
#include "amxscript.h"
#include "amxservice.h"

class AMXError;
class NPCall;

class CrashDetect : public AMXService<CrashDetect> {
 public:
  virtual ~CrashDetect() {}
 
  virtual int Load();
  virtual int Unload();

 public:
  explicit CrashDetect(AMX *amx);

 public:
  int DoAmxCallback(cell index, cell *result, cell *params);
  int DoAmxExec(cell *retval, int index);

  void HandleException();
  void HandleInterrupt();
  void HandleExecError(int index, cell *retval, const AMXError &error);

  static void OnException(void *context);
  static void OnInterrupt(void *context);

 public:
  static void PrintAmxBacktrace();
  static void PrintAmxBacktrace(std::ostream &stream);

  static void PrintNativeBacktrace(void *context);
  static void PrintNativeBacktrace(std::ostream &stream, void *context);

 private:
  static void Printf(const char *format, ...);
  static void PrintLines(std::string string);

  static void PrintError(AMXScript amx, const AMXError &error);

 private:
  AMXScript amx_;
  AMXDebugInfo debug_info_;
  std::string amx_path_;
  std::string amx_name_;
  AMX_CALLBACK prev_callback_;

 private:
  static bool block_exec_errors_;
  static std::stack<NPCall*> np_calls_;
};

#endif // !CRASHDETECT_H
