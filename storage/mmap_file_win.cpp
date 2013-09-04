#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#undef WIN32_LEAN_AND_MEAN

#include "mmap_file.hpp"

struct MMapFileImpl : public MMapFile {
	static MMapFileImpl* Open(const HANDLE hFile, const bool priv) {
		if (hFile == INVALID_HANDLE_VALUE) return NULL;

		// get file size
		LARGE_INTEGER size;
		if (GetFileSizeEx(hFile, &size) == 0) return NULL;

		// figure out access rights
		SECURITY_ATTRIBUTES sec = { sizeof(SECURITY_ATTRIBUTES), (void*)0, FALSE };
		DWORD flProtect = PAGE_READONLY;
		DWORD dwAccess = FILE_MAP_READ;

		// shared mapping
		if (!priv)
		{
			sec.bInheritHandle = TRUE;
			flProtect = PAGE_READWRITE;
			dwAccess = FILE_MAP_ALL_ACCESS;
		}
		else // private mapping
		{
			flProtect = PAGE_WRITECOPY;
			dwAccess = FILE_MAP_COPY;
		}

		// create the mapping
		const HANDLE hMap = CreateFileMapping(hFile, &sec, flProtect, size.HighPart, size.LowPart, (LPCSTR) NULL);
		if(hMap == INVALID_HANDLE_VALUE) return NULL;
		void* ptr = MapViewOfFile(hMap, dwAccess, 0, 0, size.QuadPart);
		CloseHandle(hMap);

		if(!ptr) return NULL;

		MMapFileImpl * mf = new MMapFileImpl();
		mf->data = ptr;
		mf->size = size.QuadPart;
		return mf;
	}

	static MMapFileImpl* Open(char const * filename /*, bool private_mapping */) {
		HANDLE hFile = CreateFile(filename, GENERIC_WRITE | GENERIC_READ, FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		MMapFileImpl* mapping = Open(hFile, false);
		CloseHandle(hFile);
		return mapping;
	}

	static MMapFileImpl* Create(char const * filename, std::size_t size) {
		HANDLE hFile = CreateFile(filename, GENERIC_WRITE | GENERIC_READ, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		
        LARGE_INTEGER size_;
		size_.QuadPart = size;
		SetFilePointerEx(hFile, size_, 0, FILE_BEGIN);
		SetEndOfFile(hFile);
        
		MMapFileImpl* mapping = Open(hFile, false);
		CloseHandle(hFile);
		return mapping;
	}

	void* Data() {
		return data;
	}
    
	std::size_t Size() {
		return size;
	}
    
	bool Sync() {
		return FlushViewOfFile(data, size) != 0;
	}
    
	bool Close() {
		return UnmapViewOfFile(data) != 0;
	}
private:
	void * data;
	size_t size;
};

MMapFile* MMapFile::Open(char const * filename) {
	return MMapFileImpl::Open(filename);
}

MMapFile* MMapFile::Create(char const * filename, std::size_t size) {
	return MMapFileImpl::Create(filename, size);
}