#include "MemoryMappedFile.h"

#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>

MemoryMappedFile::MemoryMappedFile(FilePathType const& filepath)
{
    Open(filepath);
}

MemoryMappedFile::~MemoryMappedFile()
{
    try {
        if (IsOpen())
            Close();
    }
    catch (CannotCloseFileException const&)
    { }
}

void MemoryMappedFile::Open(FilePathType const& filepath)
{
    OpenFile(filepath);
    MapFile(filepath);
}

void MemoryMappedFile::OpenFile(FilePathType const& filepath)
{
    mHandle = open(filepath.c_str(), O_RDONLY);
    if (IsInvalidHandle())
        throw CannotOpenFileException();
}

void MemoryMappedFile::CloseFile()
{
    if (close(mHandle) == -1)
        throw CannotCloseFileException();
    mHandle = InvalidHandle;
}

void MemoryMappedFile::Close()
{
    CloseFile();
    UnmapFile();
}

void MemoryMappedFile::MapFile(FilePathType const& filepath)
{
    auto map_size = std::filesystem::file_size(filepath);
    void *map_addr = mmap(nullptr,
                          map_size,
                          PROT_READ,
                          MAP_PRIVATE,
                          mHandle,
                          0);
    if (map_addr == MAP_FAILED)
        throw CannotMapFileException();

    mBaseOffset = reinterpret_cast<std::uint8_t*>(map_addr);
    mSize = map_size;
}

void MemoryMappedFile::UnmapFile()
{
    if (munmap(mBaseOffset, mSize) == -1)
        throw CannotUnMapFileException();
    mBaseOffset = nullptr;
    mSize = 0;
}
