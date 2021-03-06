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

#include <cassert>
#include <vector>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "os.h"

std::string os::GetModulePathFromAddr(void *address, std::size_t max_length) {
  std::vector<char> name(max_length + 1);
  if (address != 0) {
    MEMORY_BASIC_INFORMATION mbi;
    VirtualQuery(address, &mbi, sizeof(mbi));
    GetModuleFileName((HMODULE)mbi.AllocationBase, &name[0], max_length);
  }
  return std::string(&name[0]);
}

static os::ExceptionHandler except_handler = 0;
static LPTOP_LEVEL_EXCEPTION_FILTER prev_except_handler;
static DWORD except_handler_thread_id;

static LONG WINAPI ExceptionFilter(LPEXCEPTION_POINTERS exception) {
  if (::except_handler_thread_id == GetCurrentThreadId()) {
    if (::except_handler != 0) {
      ::except_handler(exception->ContextRecord);
    }
    if (::prev_except_handler != 0) {
      return ::prev_except_handler(exception);
    }
  }
  return EXCEPTION_CONTINUE_SEARCH;
}

void os::SetExceptionHandler(ExceptionHandler handler) {
  assert(::except_handler == 0 && "Only one thread may set exception handler");
  ::except_handler = handler;
  ::except_handler_thread_id = GetCurrentThreadId();
  if (handler != 0) {
    ::prev_except_handler = SetUnhandledExceptionFilter(ExceptionFilter);
  } else {
    SetUnhandledExceptionFilter(::prev_except_handler);
  }
}

static os::InterruptHandler interrupt_handler;
static DWORD ctrl_handler_thread_id;

static BOOL WINAPI ConsoleCtrlHandler(DWORD dwCtrlType) {
  switch (dwCtrlType) {
  case CTRL_C_EVENT:
    if (::interrupt_handler != 0 &&
        ::ctrl_handler_thread_id == GetCurrentThreadId()) {
      ::interrupt_handler(0);
    }
  }
  return FALSE;
}

void os::SetInterruptHandler(InterruptHandler handler) {
  assert(::interrupt_handler == 0 && "Only one thread may set interrupt handler");
  ::interrupt_handler = handler;
  ::ctrl_handler_thread_id = GetCurrentThreadId();
  SetConsoleCtrlHandler(ConsoleCtrlHandler, TRUE);
}
