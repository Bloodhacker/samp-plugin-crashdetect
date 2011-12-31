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

#ifndef AMXDEBUGINFO_H
#define AMXDEBUGINFO_H

#include <cassert>
#include <iterator>
#include <string>
#include <vector>

#include <boost/shared_ptr.hpp>

#include "amx/amx.h"
#include "amx/amxdbg.h"

class AMXDebugInfo {
public:

	template<typename EntryT, typename EntryClassT> class Table {
	public:
		Table() 
			: entries_(0), size_(0)
		{}

		Table(EntryT *table, size_t size) 
			: entries_(table), size_(size) 
		{}

		class iterator;

		typedef const iterator const_iterator;

		class iterator : public std::iterator<std::forward_iterator_tag, EntryT> {
		public:
			iterator(EntryT *entries) : cur_(entries), curw_(*cur_) {}

			const EntryClassT &operator*() const { return curw_; }
			EntryClassT       &operator*()       { return curw_; }

			const EntryClassT *operator->() const { return &curw_; }
			EntryClassT       *operator->()       { return &curw_; }

			const_iterator &operator++() const {
				++cur_;
				curw_ = EntryClassT(*cur_);
				return *this;
			}

			iterator &operator++() { 
				++cur_; 
				curw_ = EntryClassT(*cur_); 
				return *this; 
			}
			
			const_iterator &operator=(const iterator &rhs) const { 
				cur_ = rhs.cur_;
				curw_ = rhs.curw_;
				return *this;
			}

			bool operator==(const iterator &rhs) const { return rhs.cur_ == cur_;}
			bool operator!=(const iterator &rhs) const { return rhs.cur_ != cur_;}

		private:
			mutable EntryT      *cur_;
			mutable EntryClassT  curw_;
		};

		inline iterator begin() { return entries_; } 
		inline const_iterator begin() const { return entries_; }
		inline const_iterator end() const { return entries_ + size_; }
		inline iterator end() { return entries_ + size_; }

		size_t size() const { return size_; }

		EntryClassT operator[](size_t index) const { 
			assert(index >= 0 && index < size_);
			return entries_[index]; 
		}

	private:
		EntryT *entries_;
		size_t  size_;
	};

	class File {
		friend class AMXDebugInfo;
	public:
		File() : file_(0) {}
		File(const AMX_DBG_FILE *file) : file_(file) {}

		std::string GetName() const    { return file_->name; }
		ucell       GetAddress() const { return file_->address; }

		operator bool() { return file_ != 0; }

	private:
		const AMX_DBG_FILE *file_;
	};

	class Line {
		friend class AMXDebugInfo;
	public:
		Line() { line_.address = 0; }
		Line(AMX_DBG_LINE line) : line_(line) {}

		int32_t GetNumber() const  { return line_.line; }
		ucell   GetAddress() const { return line_.address; }

		operator bool() { return line_.address != 0; }

	private:
		AMX_DBG_LINE line_;
	};

	class Tag {
		friend class AMXDebugInfo;
	public:
		Tag() : tag_(0) {}
		Tag(const AMX_DBG_TAG *tag) : tag_(tag) {}

		int32_t     GetID() const   { return tag_->tag; }
		std::string GetName() const { return tag_->name; }		

		operator bool() { return tag_ != 0; }

	private:
		const AMX_DBG_TAG *tag_;
	};

	class SymbolDim;

	class Symbol {
		friend class AMXDebugInfo;
	public:
		enum VClass {
			Global      = 0,
			Local       = 1,
			StaticLocal = 2
		};

		enum Kind {
			Variable    = 1,
			Reference   = 2,
			Array       = 3,
			ArrayRef    = 4,
			Function    = 9,
			FunctionRef = 10
		};

		Symbol() : symbol_(0) {}
		Symbol(const AMX_DBG_SYMBOL *symbol) : symbol_(symbol) {}

		const AMX_DBG_SYMBOL *GetPOD() const { return symbol_; }

		bool IsGlobal() const      { return GetVClass() == Global; }
		bool IsLocal() const       { return GetVClass() == Local; }
		bool IsStaticLocal() const { return GetVClass() == StaticLocal; }

		bool IsVariable() const    { return GetKind() == Variable; }
		bool IsReference() const   { return GetKind() == Reference; }
		bool IsArray() const       { return GetKind() == Array; }
		bool IsArrayRef() const    { return GetKind() == ArrayRef; }
		bool IsFunction() const    { return GetKind() == Function; }
		bool IsFunctionRef() const { return GetKind() == FunctionRef; }

		ucell       GetAddress() const          { return symbol_->address; }
		int16_t     GetTag() const              { return symbol_->tag; }
		ucell       GetCodeStartAddress() const { return symbol_->codestart; }
		ucell       GetCodeEndAddress() const   { return symbol_->codeend; }
		Kind        GetKind() const             { return static_cast<Kind>(symbol_->ident); }
		VClass      GetVClass() const           { return static_cast<VClass>(symbol_->vclass); }
		int16_t     GetArrayDim() const         { return symbol_->dim; }
		std::string GetName() const             { return symbol_->name; }
		int16_t     GetNumDims() const           { return symbol_->dim; }

		std::vector<SymbolDim> GetDims() const;

		cell GetValue(AMX *amx) const;

		operator bool() { return symbol_ != 0; }

	private:
		const AMX_DBG_SYMBOL *symbol_;
	};

	class SymbolDim {
	public:
		SymbolDim() : symdim_(0) {}
		SymbolDim(const AMX_DBG_SYMDIM *symdim) : symdim_(symdim) {}

		int16_t GetTag() const  { return symdim_->tag; }
		ucell   GetSize() const { return symdim_->size; }

		operator bool() { return symdim_ != 0; }

	private:
		const AMX_DBG_SYMDIM *symdim_;
	};

	AMXDebugInfo();
	explicit AMXDebugInfo(const std::string &filename);

	void Load(const std::string &filename);
	bool IsLoaded() const;
	void Free();

	Line   GetLine(ucell address) const;
	File   GetFile(ucell address) const;
	Symbol GetFunction(ucell address) const;
	Tag    GetTag(int tagID) const;	

	int32_t     GetLineNumber(ucell addrss) const;
	std::string GetFileName(ucell address) const;
	std::string GetFunctionName(ucell address) const;
	std::string GetTagName(ucell address) const;

	ucell GetFunctionAddress(const std::string &functionName, const std::string &fileName) const;
	ucell GetFunctionStartAddress(ucell address) const;
	ucell GetLineAddress(long line, const std::string &fileName) const;

	typedef Table<AMX_DBG_FILE*, File> FileTable;

	FileTable GetFiles() const { 
		return FileTable(amxdbgPtr_->filetbl, amxdbgPtr_->hdr->files); 
	}

	typedef Table<AMX_DBG_LINE, Line> LineTable;

	LineTable GetLines() const { 
		return LineTable(amxdbgPtr_->linetbl, amxdbgPtr_->hdr->lines);
	}

	typedef Table<AMX_DBG_TAG*, Tag> TagTable;

	TagTable GetTags() const {
		return TagTable(amxdbgPtr_->tagtbl, amxdbgPtr_->hdr->tags);
	}

	typedef Table<AMX_DBG_SYMBOL*, Symbol> SymbolTable;

	SymbolTable GetSymbols() const {
		return SymbolTable(amxdbgPtr_->symboltbl, amxdbgPtr_->hdr->symbols);
	}

private:
	static bool HasDebugInfo(AMX *amx);
	static void FreeAmxDbg(AMX_DBG *amxdbg);

	boost::shared_ptr<AMX_DBG> amxdbgPtr_;
};

#endif // !AMXDEBUGINFO_H