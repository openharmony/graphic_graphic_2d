/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, Hardware
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <gtest/gtest.h>

#include <memory>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <vector>
#include <array>
#include <cstring>
#include "egl_blob_cache.h"

#include "egl_defs.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class EglBlobFileTest : public testing::Test {
public:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
    void SetUp() {}
    void TearDown() {}
};

/**
 * @tc.name: EglBlobFileInit001
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(EglBlobFileTest, EglBlobFileInit001, Level1)
{
    BlobCache* ret = BlobCache::Get();
    ASSERT_NE(ret, nullptr);
}

/**
 * @tc.name: EglBlobFileInit002
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(EglBlobFileTest, EglBlobFileInit002, Level1)
{
    BlobCache* ret = BlobCache::Get();
    ret->SetCacheDir("/data/local/tmp");
    ASSERT_NE(ret, nullptr);

    void *a = malloc(4);
    void *b = malloc(4);
    int* intPtr = static_cast<int*>(a);
    int* intPtr2 = static_cast<int*>(b);
    *intPtr = 1;
    *intPtr2 = 2;
    const void* key = static_cast<const void*>(a);
    void *value = static_cast<void*>(b);
    int d = *static_cast<int *>(value);
    ASSERT_EQ(d, 2);
    EGLsizeiANDROID keysize = 4;
    EGLsizeiANDROID valuesize = 4;
    void *value2 = malloc(4);
    BlobCache::SetBlobFunc(key, keysize, value, valuesize);
    BlobCache::GetBlobFunc(key, keysize, value2, valuesize);
    int c = *static_cast<int *>(value2);
    ASSERT_EQ(c, 2);
    BlobCache::Get()->Terminate();
    sleep(5);
    ret = BlobCache::Get();
    ret->SetCacheDir("/data/local/tmp");
    int h = ret->GetMapSize();
    ASSERT_EQ(h, 0); // not really read from disk
}

/**
 * @tc.name: EglBlobFileInit003
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(EglBlobFileTest, EglBlobFileInit003, Level1)
{
    BlobCache* ret = BlobCache::Get();
    ret->SetCacheDir("/data/local/tmp");
    ASSERT_NE(ret, nullptr);

    void *a = malloc(4);
    void *b = malloc(4);
    int* intPtr = static_cast<int*>(a);
    int* intPtr2 = static_cast<int*>(b);
    *intPtr = 1;
    *intPtr2 = 2;
    const void* key = static_cast<const void*>(a);
    void *value = static_cast<void*>(b);
    int d = *static_cast<int *>(value);
    ASSERT_EQ(d, 2);
    EGLsizeiANDROID keysize = 4;
    EGLsizeiANDROID valuesize = 4;
    void *value2 = malloc(4);
    BlobCache::SetBlobFunc(key, keysize, value, valuesize);
    BlobCache::GetBlobFunc(key, keysize, value2, valuesize);
    int c = *static_cast<int *>(value2);
    ASSERT_EQ(c, 2);
    BlobCache::Get()->Terminate(); // write to disk and delete from memory
    sleep(5);
    ret = BlobCache::Get();
    ret->SetCacheDir("/data/local/tmp");
    BlobCache::GetBlobFunc(key, keysize, value2, valuesize); // actually read from disk again
    int h = ret->GetMapSize();
    ASSERT_EQ(h, 1);
    c = *static_cast<int *>(value2);
    ASSERT_EQ(c, 2);
}

/**
 * @tc.name: BlobCacheTerminateTest
 * @tc.desc: Test BlobCache::Terminate with nullptr blobCache_
 * @tc.type: FUNC
 */
HWTEST_F(EglBlobFileTest, BlobCacheTerminateTest, Level1)
{
    BlobCache* oldCache = BlobCache::blobCache_;
    EXPECT_NE(oldCache, nullptr);

    delete oldCache;
    BlobCache::blobCache_ = nullptr;

    BlobCache* temp = new BlobCache();
    temp->Terminate();
    delete temp;
    EXPECT_EQ(BlobCache::blobCache_, nullptr);
}

/**
 * @tc.name: BlobCacheTerminateWithDataTest
 * @tc.desc: Test BlobCache::Terminate with non-null blobCache_
 * @tc.type: FUNC
 */
HWTEST_F(EglBlobFileTest, BlobCacheTerminateWithDataTest, Level1)
{
    BlobCache* oldCache = BlobCache::Get();
    EXPECT_NE(oldCache, nullptr);

    oldCache->SetCacheDir("/data/local/tmp");
    oldCache->Terminate();
    EXPECT_EQ(BlobCache::blobCache_, nullptr);
}

/**
 * @tc.name: BlobCacheReadFromDiskOverflowTest
 * @tc.desc: Test BlobCacheReadFromDisk with value overflow (AddOverflow returns true)
 * @tc.type: FUNC
 */
HWTEST_F(EglBlobFileTest, BlobCacheReadFromDiskValueOverflowTest, Level1)
{
    BlobCache* ret = BlobCache::Get();
    EXPECT_NE(ret, nullptr);

    std::string testDir = "/data/local/tmp/blob_test_overflow";
    mkdir(testDir.c_str(), S_IRWXU);
    std::string testFile = testDir + "/blobShader";

    const char* magic = "OSOH";
    uint32_t crc = 0;
    size_t keysize = 8;
    size_t valuesize = SIZE_MAX;
    size_t actualKeyDataSize = 8;
    size_t actualValueDataSize = 1;
    size_t totalSize = CACHE_HEAD + sizeof(keysize) + sizeof(valuesize) + actualKeyDataSize + actualValueDataSize;

    std::vector<uint8_t> buffer(totalSize);
    memcpy_s(buffer.data(), 4, magic, 4);
    memcpy_s(buffer.data() + 4, sizeof(crc), &crc, sizeof(crc));
    size_t offset = CACHE_HEAD;
    memcpy_s(buffer.data() + offset, sizeof(keysize), &keysize, sizeof(keysize));
    offset += sizeof(keysize);
    memcpy_s(buffer.data() + offset, sizeof(valuesize), &valuesize, sizeof(valuesize));
    offset += sizeof(valuesize);
    uint8_t keyData[] = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08 };
    memcpy_s(buffer.data() + offset, actualKeyDataSize, keyData, actualKeyDataSize);
    offset += actualKeyDataSize;
    uint8_t valueData[1] = { 0x01 };
    memcpy_s(buffer.data() + offset, actualValueDataSize, valueData, actualValueDataSize);

    uint32_t computedCrc = ret->CrcGen(buffer.data() + CACHE_HEAD, totalSize - CACHE_HEAD);
    memcpy_s(buffer.data() + 4, sizeof(computedCrc), &computedCrc, sizeof(computedCrc));

    int fd = open(testFile.c_str(), O_CREAT | O_RDWR, S_IRWXU);
    EXPECT_NE(fd, -1);
    write(fd, buffer.data(), totalSize);
    close(fd);

    ret->BlobCacheReadFromDisk(testFile);
    EXPECT_EQ(ret->GetMapSize(), 0);

    unlink(testFile.c_str());
    rmdir(testDir.c_str());
}

/**
 * @tc.name: BlobCacheReadFromDiskKeyOverflowTest
 * @tc.desc: Test BlobCacheReadFromDisk with key overflow (AddOverflow returns true)
 * @tc.type: FUNC
 */
HWTEST_F(EglBlobFileTest, BlobCacheReadFromDiskKeyOverflowTest, Level1)
{
    BlobCache* ret = BlobCache::Get();
    EXPECT_NE(ret, nullptr);

    std::string testDir = "/data/local/tmp/blob_test_key_overflow";
    mkdir(testDir.c_str(), S_IRWXU);
    std::string testFile = testDir + "/blobShader";

    const char* magic = "OSOH";
    uint32_t crc = 0;
    size_t keysize = SIZE_MAX;
    size_t valuesize = 8;
    size_t actualKeyDataSize = 1;
    size_t actualValueDataSize = 8;
    size_t totalSize = CACHE_HEAD + sizeof(keysize) + sizeof(valuesize) + actualKeyDataSize + actualValueDataSize;

    std::vector<uint8_t> buffer(totalSize);
    memcpy_s(buffer.data(), 4, magic, 4);
    memcpy_s(buffer.data() + 4, sizeof(crc), &crc, sizeof(crc));
    size_t offset = CACHE_HEAD;
    memcpy_s(buffer.data() + offset, sizeof(keysize), &keysize, sizeof(keysize));
    offset += sizeof(keysize);
    memcpy_s(buffer.data() + offset, sizeof(valuesize), &valuesize, sizeof(valuesize));
    offset += sizeof(valuesize);
    uint8_t keyData[1] = { 0x01 };
    memcpy_s(buffer.data() + offset, actualKeyDataSize, keyData, actualKeyDataSize);
    offset += actualKeyDataSize;
    uint8_t valueData[] = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08 };
    memcpy_s(buffer.data() + offset, actualValueDataSize, valueData, actualValueDataSize);

    uint32_t computedCrc = ret->CrcGen(buffer.data() + CACHE_HEAD, totalSize - CACHE_HEAD);
    memcpy_s(buffer.data() + 4, sizeof(computedCrc), &computedCrc, sizeof(computedCrc));

    int fd = open(testFile.c_str(), O_CREAT | O_RDWR, S_IRWXU);
    EXPECT_NE(fd, -1);
    write(fd, buffer.data(), totalSize);
    close(fd);

    ret->BlobCacheReadFromDisk(testFile);
    EXPECT_EQ(ret->GetMapSize(), 0);

    unlink(testFile.c_str());
    rmdir(testDir.c_str());
}

/**
 * @tc.name: BlobCacheReadFromDiskKeySizeExceedFileSizeTest
 * @tc.desc: Test BlobCacheReadFromDisk with keysize > filesize
 * @tc.type: FUNC
 */
HWTEST_F(EglBlobFileTest, BlobCacheReadFromDiskKeySizeExceedFileSizeTest, Level1)
{
    BlobCache* ret = BlobCache::Get();
    EXPECT_NE(ret, nullptr);

    std::string testDir = "/data/local/tmp/blob_test_key_exceed";
    mkdir(testDir.c_str(), S_IRWXU);
    std::string testFile = testDir + "/blobShader";

    const char* magic = "OSOH";
    uint32_t crc = 0;
    size_t keysize = 64;
    size_t valuesize = 4;
    size_t totalSize = CACHE_HEAD + sizeof(keysize) + sizeof(valuesize) + 4;

    std::vector<uint8_t> buffer(totalSize);
    memcpy_s(buffer.data(), 4, magic, 4);
    memcpy_s(buffer.data() + 4, sizeof(crc), &crc, sizeof(crc));
    size_t offset = CACHE_HEAD;
    memcpy_s(buffer.data() + offset, sizeof(keysize), &keysize, sizeof(keysize));
    offset += sizeof(keysize);
    memcpy_s(buffer.data() + offset, sizeof(valuesize), &valuesize, sizeof(valuesize));
    offset += sizeof(valuesize);
    uint8_t valueData[] = { 0x01, 0x02, 0x03, 0x04 };
    memcpy_s(buffer.data() + offset, valuesize, valueData, valuesize);

    uint32_t computedCrc = ret->CrcGen(buffer.data() + CACHE_HEAD, totalSize - CACHE_HEAD);
    memcpy_s(buffer.data() + 4, sizeof(computedCrc), &computedCrc, sizeof(computedCrc));

    int fd = open(testFile.c_str(), O_CREAT | O_RDWR, S_IRWXU);
    EXPECT_NE(fd, -1);
    write(fd, buffer.data(), totalSize);
    close(fd);

    ret->BlobCacheReadFromDisk(testFile);
    EXPECT_EQ(ret->GetMapSize(), 0);

    unlink(testFile.c_str());
    rmdir(testDir.c_str());
}

/**
 * @tc.name: BlobCacheReadFromDiskValueSizeExceedFileSizeTest
 * @tc.desc: Test BlobCacheReadFromDisk with valuesize > filesize
 * @tc.type: FUNC
 */
HWTEST_F(EglBlobFileTest, BlobCacheReadFromDiskValueSizeExceedFileSizeTest, Level1)
{
    BlobCache* ret = BlobCache::Get();
    EXPECT_NE(ret, nullptr);

    std::string testDir = "/data/local/tmp/blob_test_value_exceed";
    mkdir(testDir.c_str(), S_IRWXU);
    std::string testFile = testDir + "/blobShader";

    const char* magic = "OSOH";
    uint32_t crc = 0;
    size_t keysize = 4;
    size_t valuesize = 64;
    size_t totalSize = CACHE_HEAD + sizeof(keysize) + sizeof(valuesize) + keysize + 4;

    std::vector<uint8_t> buffer(totalSize);
    memcpy_s(buffer.data(), 4, magic, 4);
    memcpy_s(buffer.data() + 4, sizeof(crc), &crc, sizeof(crc));
    size_t offset = CACHE_HEAD;
    memcpy_s(buffer.data() + offset, sizeof(keysize), &keysize, sizeof(keysize));
    offset += sizeof(keysize);
    memcpy_s(buffer.data() + offset, sizeof(valuesize), &valuesize, sizeof(valuesize));
    offset += sizeof(valuesize);
    uint8_t keyData[] = { 0x01, 0x02, 0x03, 0x04 };
    memcpy_s(buffer.data() + offset, keysize, keyData, keysize);
    offset += keysize;
    uint8_t valueData[] = { 0x01, 0x02, 0x03, 0x04 };
    memcpy_s(buffer.data() + offset, 4, valueData, 4);

    uint32_t computedCrc = ret->CrcGen(buffer.data() + CACHE_HEAD, totalSize - CACHE_HEAD);
    memcpy_s(buffer.data() + 4, sizeof(computedCrc), &computedCrc, sizeof(computedCrc));

    int fd = open(testFile.c_str(), O_CREAT | O_RDWR, S_IRWXU);
    EXPECT_NE(fd, -1);
    write(fd, buffer.data(), totalSize);
    close(fd);

    ret->BlobCacheReadFromDisk(testFile);
    EXPECT_EQ(ret->GetMapSize(), 0);

    unlink(testFile.c_str());
    rmdir(testDir.c_str());
}

/**
 * @tc.name: BlobCacheReadFromDiskValidTest
 * @tc.desc: Test BlobCacheReadFromDisk with valid data
 * @tc.type: FUNC
 */
HWTEST_F(EglBlobFileTest, BlobCacheReadFromDiskValidTest, Level1)
{
    BlobCache* ret = BlobCache::Get();
    EXPECT_NE(ret, nullptr);

    std::string testDir = "/data/local/tmp/blob_test_valid";
    mkdir(testDir.c_str(), S_IRWXU);
    std::string testFile = testDir + "/blobShader";

    const char* magic = "OSOH";
    uint32_t crc = 0;
    size_t keysize = 4;
    size_t valuesize = 4;
    uint8_t keyData[] = { 0x01, 0x02, 0x03, 0x04 };
    uint8_t valueData[] = { 0x05, 0x06, 0x07, 0x08 };
    size_t totalSize = CACHE_HEAD + sizeof(keysize) + sizeof(valuesize) + keysize + valuesize;

    std::vector<uint8_t> buffer(totalSize);
    memcpy_s(buffer.data(), 4, magic, 4);
    memcpy_s(buffer.data() + 4, sizeof(crc), &crc, sizeof(crc));
    size_t offset = CACHE_HEAD;
    memcpy_s(buffer.data() + offset, sizeof(keysize), &keysize, sizeof(keysize));
    offset += sizeof(keysize);
    memcpy_s(buffer.data() + offset, sizeof(valuesize), &valuesize, sizeof(valuesize));
    offset += sizeof(valuesize);
    memcpy_s(buffer.data() + offset, keysize, keyData, keysize);
    offset += keysize;
    memcpy_s(buffer.data() + offset, valuesize, valueData, valuesize);

    uint32_t computedCrc = ret->CrcGen(buffer.data() + CACHE_HEAD, totalSize - CACHE_HEAD);
    memcpy_s(buffer.data() + 4, sizeof(computedCrc), &computedCrc, sizeof(computedCrc));

    int fd = open(testFile.c_str(), O_CREAT | O_RDWR, S_IRWXU);
    EXPECT_NE(fd, -1);
    write(fd, buffer.data(), totalSize);
    close(fd);

    ret->BlobCacheReadFromDisk(testFile);
    EXPECT_EQ(ret->GetMapSize(), 1);

    unlink(testFile.c_str());
    rmdir(testDir.c_str());
}

} // OHOS::Rosen
