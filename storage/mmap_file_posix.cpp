#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>

#include "mmap_file.hpp"

struct MMapFileImpl : MMapFile {
    static MMapFileImpl* Open(int fd) {
        void* data;
        size_t size;

        // get file size
        struct stat st;
        int status;
        status = fstat(fd, &st);
        if (status != 0) {
            return nullptr;
        }
        size = st.st_size;

        // map the file
        data = mmap(0, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

        if (data == MAP_FAILED) {
            return nullptr;
        }

        // return mmap file descriptor
        MMapFileImpl* mf = new MMapFileImpl();
        mf->data = data;
        mf->size = size;
        return mf;
    }

    static MMapFileImpl* Open(const char * filepath) {
        int fd = open(filepath, O_RDWR);
        if (fd < 0) return NULL;
        MMapFileImpl* mf = Open(fd);
        close(fd);
        return mf;
    }

    static MMapFileImpl* Create(const char * filepath, size_t size) {
        int fd = open(filepath, O_RDWR | O_CREAT | O_TRUNC, (mode_t) 0666);
        if (fd < 0) return NULL;

        // strech the file to size
        off_t result = lseek(fd, size - 1, SEEK_SET);
	if (result < 0 ||  write(fd, "", 1) < 0) {
            close(fd);
            return NULL;
        }

        MMapFileImpl* mf = Open(fd);
        close(fd);
        return mf;
    }

    void* Data() {
        return data;
    }

    size_t Size() {
        return size;
    }

    bool Sync() {
        return msync(data, size, MS_SYNC) == 0;
    }

    bool Close() {
        return munmap(data, size) == 0;
    }

private:
    void* data = nullptr;
    size_t size = 0;
};

MMapFile* MMapFile::Open(char const * filename) {
	return MMapFileImpl::Open(filename);
}

MMapFile* MMapFile::Create(char const * filename, std::size_t size) {
	return MMapFileImpl::Create(filename, size);
}
