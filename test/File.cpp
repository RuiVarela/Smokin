#include <UnitTest.hpp>
#include <File.hpp>
#include <iostream>

using namespace smk;

static const Path test_filename(U"unit_test_data/xml_test.xml");
static const Path test_filename_unix(test_filename);
static const Path test_filename_windows(U"unit_test_data\\xml_test.xml");

Path testRoot();


TEST_BEGIN(TestFilename)
{
    //std::cout << "Bin File: " << str32To8(executablePath()) << std::endl;
    
    TEST_FAIL_NOT_EQUAL(convertFileNameToUnixStyle(test_filename), test_filename_unix);
	TEST_FAIL_NOT_EQUAL(convertFileNameToWindowsStyle(test_filename), test_filename_windows);

	TEST_FAIL_NOT_EQUAL(convertFileNameToUnixStyle(test_filename_windows), test_filename_unix);
	TEST_FAIL_NOT_EQUAL(convertFileNameToWindowsStyle(test_filename_unix), test_filename_windows);

#if defined( _WIN32 )
	TEST_FAIL_IF( isFileNameNativeStyle(test_filename), "Filename should not be in native style" );
    TEST_FAIL_NOT_EQUAL(convertFileNameToNativeStyle(test_filename), test_filename_windows);
#else 
	TEST_FAIL_IF(!isFileNameNativeStyle(test_filename), "Filename should be in native style" );
	TEST_FAIL_NOT_EQUAL(convertFileNameToNativeStyle(test_filename), test_filename_unix);
#endif
}
TEST_END(TestFilename)


TEST_BEGIN(TestPath)
{
    TEST_FAIL_NOT_EQUAL(U"unit_test_data", getFilePath(test_filename));
	TEST_FAIL_NOT_EQUAL(U"xml", getFileExtension(test_filename));
	TEST_FAIL_NOT_EQUAL(U"XmL", getFileExtension(U"unit_test_data/xmlss.XmL"));
	TEST_FAIL_NOT_EQUAL(U"xml", getLowerCaseFileExtension(U"unit_test_data/xmlss.XmL"));
	TEST_FAIL_NOT_EQUAL(U"xml_test.xml", getSimpleFileName(test_filename));
	TEST_FAIL_NOT_EQUAL(U"unit_test_data/xml_test", getNameLessExtension(test_filename));
	TEST_FAIL_NOT_EQUAL(U"xml_test", getStrippedName(test_filename));
    TEST_FAIL_NOT_EQUAL(U"folder1/folder2/folder3", getFilePath(U"folder1/folder2/folder3/kk.ls"));
    TEST_FAIL_NOT_EQUAL(U"folder1/folder2", getFilePath(U"folder1/folder2/folder3"));
    TEST_FAIL_NOT_EQUAL(U"folder1", getFilePath(U"folder1/folder2"));
    TEST_FAIL_NOT_EQUAL(U"", getFilePath(U"folder1"));
}
TEST_END(TestPath)


TEST_BEGIN(TestDirectory)
{
    Path test_root = testRoot();
    
    Path folder = mergePaths(test_root, U"data/File");
    TEST_ASSERT(fileType(folder) == FileType::FileDirectory);
    DirectoryContents contents = getDirectoryContents(folder);

    //TEST_FAIL_NOT_EQUAL(size_t(6), contents.size());
    TEST_ASSERT(std::find(contents.begin(), contents.end(), U"One") != contents.end());
    TEST_ASSERT(std::find(contents.begin(), contents.end(), U"Tw o") != contents.end());
    TEST_ASSERT(std::find(contents.begin(), contents.end(), U"one_text_file.txt") != contents.end());
    TEST_ASSERT(std::find(contents.begin(), contents.end(), U"another file.txt") != contents.end());
    TEST_ASSERT(std::find(contents.begin(), contents.end(), U"utf8.txt") != contents.end());
    
    TEST_ASSERT(
        (std::find(contents.begin(), contents.end(), U"joão cãoçarola.txt") != contents.end()) ||
        (std::find(contents.begin(), contents.end(), U"joão cãoçarola.txt") != contents.end()) 
    );

    TEST_ASSERT(std::find(contents.begin(), contents.end(), U"emojis_😍😒😖😟.txt") != contents.end());
    TEST_ASSERT(fileType(mergePaths(test_root, U"data/File/One")) == FileType::FileDirectory);
    TEST_ASSERT(fileType(mergePaths(test_root, U"data/File/Tw o")) == FileType::FileDirectory);
    TEST_ASSERT(fileType(mergePaths(test_root, U"data/File/one_text_file.txt")) == FileType::FileRegular);
    TEST_ASSERT(fileType(mergePaths(test_root, U"data/File/another file.txt")) == FileType::FileRegular);
    TEST_ASSERT(fileType(mergePaths(test_root, U"data/File/emojis_😍😒😖😟.txt")) == FileType::FileRegular);
    
}
TEST_END(TestDirectory)


TEST_BEGIN(TestDataLoad)
{
    {
        str8 data;
        TEST_ASSERT(readRawText(mergePaths(testRoot(), U"data/File/another file.txt"), data));
        TEST_FAIL_NOT_EQUAL(data, "hello world\n");
    }
    
    {
        str32 data;
        TEST_ASSERT(readUtf8Text(mergePaths(testRoot(), U"data/File/utf8.txt"), data));
        
        str32 data_loaded;
        TEST_ASSERT(writeUtf8Text(mergePaths(testRoot(), U"data/File/utf8_test_tmp.txt"), data));
        TEST_ASSERT(readUtf8Text(mergePaths(testRoot(), U"data/File/utf8_test_tmp.txt"), data_loaded));
        TEST_FAIL_NOT_EQUAL(data, data_loaded);
        TEST_ASSERT(deleteFile(mergePaths(testRoot(), U"data/File/utf8_test_tmp.txt")));
    }
    
    {
        str32 data;
        TEST_ASSERT(readUtf8Text(mergePaths(testRoot(), U"data/File/utf8.txt"), data));
        
        str32 data_loaded;
        TEST_ASSERT(writeUtf16Text(mergePaths(testRoot(), U"data/File/utf16_test_tmp.txt"), data));
        TEST_ASSERT(readUtf16Text(mergePaths(testRoot(), U"data/File/utf16_test_tmp.txt"), data_loaded));
        TEST_FAIL_NOT_EQUAL(data, data_loaded);
        TEST_ASSERT(deleteFile(mergePaths(testRoot(), U"data/File/utf16_test_tmp.txt")));
    }
}
TEST_END(TestDataLoad)
