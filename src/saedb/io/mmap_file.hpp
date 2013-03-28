#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>

struct MMapFile {
    static MMapFile* Open(int fd) {
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
        MMapFile* mf = new MMapFile();
        mf->fd = fd;
        mf->data = data;
        mf->size = size;
        return mf;
    }

    static MMapFile* Open(const char * filepath) {
        int fd = open(filepath, O_RDWR);
        if (fd < 0) return nullptr;
        MMapFile* mf = Open(fd);
        close(fd);
        return mf;
    }

    static MMapFile* Create(const char * filepath, size_t size) {
        int fd = open(filepath, O_RDWR | O_CREAT | O_TRUNC, (mode_t) 0666);
        if (fd < 0) return nullptr;

        // strech the file to size
        int result = lseek(fd, size - 1, SEEK_SET);
        result |= write(fd, "", 1);
        if (result < 0) {
            close(fd);
            return nullptr;
        }

        MMapFile* mf = Open(fd);
        close(fd);
        return mf;
    }

    void* Data() {
        return data;
    }

    size_t Size() {
        return size;
    }

    bool Close() {
        return munmap(data, size) == 0;
    }

private:
    int fd = -1;
    void* data = nullptr;
    size_t size = 0;
};
