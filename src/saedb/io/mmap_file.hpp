#include <cstring>

struct MMapFile {
	static MMapFile* Open(char const * filename);
	static MMapFile* Create(char const * filename, std::size_t size);
	virtual void* Data() = 0;
    virtual std::size_t Size() = 0;
    virtual bool Sync() = 0;
    virtual bool Close() = 0;
};