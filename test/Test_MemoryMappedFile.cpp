#include <gmock/gmock.h>
#include <filesystem>
#include <fstream>

#include <MemoryMappedFile.h>

using namespace ::testing;

class AMemoryMappedFile : public Test
{
public:
    void SetUp() override
    {
        {
            std::fstream fs(TEST_FILEPATH, fs.out | fs.trunc);
            fs.write(TEST_FILE_CONTENT.data(), TEST_FILE_CONTENT.length());
        }
        {
            std::fstream fs(EMPTY_FILE_TEST_FILEPATH, fs.out | fs.trunc);
        }
    }

    void TearDown() override
    {
        std::filesystem::remove(TEST_FILEPATH);
    }

    static const std::filesystem::path TEST_FILEPATH;
    static const std::filesystem::path EMPTY_FILE_TEST_FILEPATH;
    static const std::string TEST_FILE_CONTENT;
    MemoryMappedFile   mMMapFile;
};

class MockMemoryMappedFile
{
public:
    MOCK_METHOD(bool, IsOpen, ());
    MOCK_METHOD(void, Close, ());
    ~MockMemoryMappedFile() { if (IsOpen()) Close(); }
};

const std::filesystem::path AMemoryMappedFile::TEST_FILEPATH{"testfile"};
const std::filesystem::path
    AMemoryMappedFile::EMPTY_FILE_TEST_FILEPATH{"emptyfile"};
const std::string AMemoryMappedFile::TEST_FILE_CONTENT{"This is a test file."};

TEST_F(AMemoryMappedFile, CanOpenAFile)
{
    mMMapFile.Open(TEST_FILEPATH);
    ASSERT_TRUE(mMMapFile.IsOpen());
}

TEST_F(AMemoryMappedFile, ThrowsIfFileDoesNotExist)
{
    ASSERT_THROW(mMMapFile.Open("NONEXISTANT"), CannotOpenFileException);
}

TEST_F(AMemoryMappedFile, ThrowsIfCannotOpenFile)
{
    std::filesystem::permissions(TEST_FILEPATH, std::filesystem::perms::none);
    ASSERT_THROW(mMMapFile.Open(TEST_FILEPATH), CannotOpenFileException);
}

TEST_F(AMemoryMappedFile, OpenedFileCanBeClosed)
{
    mMMapFile.Open(TEST_FILEPATH);
    mMMapFile.Close();
    ASSERT_FALSE(mMMapFile.IsOpen());
}

TEST_F(AMemoryMappedFile, ThrowsIfClosedFileIsNotOpen)
{
    ASSERT_THROW(mMMapFile.Close(), CannotCloseFileException);
}

TEST_F(AMemoryMappedFile, ClosesOpenFileUponDestruction)
{
    InSequence s;

    MockMemoryMappedFile mockMemoryMappedFile = MockMemoryMappedFile();

    EXPECT_CALL(mockMemoryMappedFile, IsOpen()).WillOnce(Return(true));
    EXPECT_CALL(mockMemoryMappedFile, Close()).Times(1);
}

TEST_F(AMemoryMappedFile, DoesNotCloseUnopenFileUponDestruction)
{
    MockMemoryMappedFile mockMemoryMappedFile = MockMemoryMappedFile();

    EXPECT_CALL(mockMemoryMappedFile, IsOpen()).WillOnce(Return(false));
    EXPECT_CALL(mockMemoryMappedFile, Close()).Times(0);
}

TEST_F(AMemoryMappedFile, CreatesAMemoryMapWhenOpeningAFile)
{
    mMMapFile.Open(TEST_FILEPATH);
    ASSERT_NE(mMMapFile.Data(), nullptr);
}

TEST_F(AMemoryMappedFile, ThrowsIfFailesToCreateAMemoryMap)
{
    ASSERT_THROW(mMMapFile.Open(EMPTY_FILE_TEST_FILEPATH),
                 CannotMapFileException);
}

TEST_F(AMemoryMappedFile, HasNoDataIfMapFails)
{
    EXPECT_THROW(mMMapFile.Open(EMPTY_FILE_TEST_FILEPATH),
                 CannotMapFileException);
    ASSERT_EQ(mMMapFile.Data(), nullptr);
}

TEST_F(AMemoryMappedFile, HasNoDataAfterClosingTheMappedFile)
{
    mMMapFile.Open(TEST_FILEPATH);
    mMMapFile.Close();
    ASSERT_EQ(mMMapFile.Data(), nullptr);
}

TEST_F(AMemoryMappedFile, CanReadDataFromMappedFile)
{
    mMMapFile.Open(TEST_FILEPATH);
    auto data = mMMapFile.Data();

    const std::string actual{reinterpret_cast<const char*>(data) + 5, 2};
    const std::string expected{"is"};

    ASSERT_STREQ(actual.c_str(), expected.c_str());
}

TEST_F(AMemoryMappedFile, ReturnsTheMappedSizeOfTheFile)
{
    mMMapFile.Open(TEST_FILEPATH);
    auto actual = mMMapFile.Size();
    auto expected = TEST_FILE_CONTENT.length();

    ASSERT_EQ(actual, expected);
}

TEST_F(AMemoryMappedFile, CanReadDataFromMappedFileAtSpecifiedOffset)
{
    mMMapFile.Open(TEST_FILEPATH);
    auto data = mMMapFile.Data(5);
    const std::string actual{reinterpret_cast<const char*>(data), 2};
    const std::string expected{"is"};

    ASSERT_EQ(actual, expected);
}

TEST_F(AMemoryMappedFile, CanOpenAMapDuringConstruction)
{
    ASSERT_FALSE(mMMapFile.IsOpen());
    mMMapFile = MemoryMappedFile(TEST_FILEPATH);
    ASSERT_TRUE(mMMapFile.IsOpen());
}
