#pragma once

#include <filesystem>
#include <exception>

struct CannotOpenFileException : public std::exception { };
struct CannotCloseFileException : public std::exception { };
struct CannotMapFileException : public std::exception { };
struct CannotUnMapFileException : public std::exception { };

using Handle = int;
using FilePathType = std::filesystem::path;

class MemoryMappedFile
{
public:
    MemoryMappedFile() noexcept { }
    MemoryMappedFile(FilePathType const& filepath);
    MemoryMappedFile(MemoryMappedFile const&) = delete;
    MemoryMappedFile& operator=(MemoryMappedFile const&) = delete;

    explicit MemoryMappedFile(MemoryMappedFile&&) = default;
    MemoryMappedFile& operator=(MemoryMappedFile&&) = default;

    void Open(FilePathType const& filepath);
    void Close();

    inline const std::uint8_t* Data(std::size_t const offset = 0) const noexcept
    {
        return mBaseOffset + offset;
    }

    inline bool IsOpen() const noexcept { return Data() != nullptr; }

    inline std::size_t Size() const noexcept { return mSize; }

    ~MemoryMappedFile();

private:
    void OpenFile(FilePathType const& filepath);
    void CloseFile();
    void MapFile(FilePathType const& filepath);
    void UnmapFile();

    inline bool IsInvalidHandle() const noexcept
    {
        return mHandle == InvalidHandle;
    }

    static constexpr Handle InvalidHandle{-1};
    Handle                  mHandle{InvalidHandle};
    std::size_t             mSize{0};
    std::uint8_t           *mBaseOffset{nullptr};
};