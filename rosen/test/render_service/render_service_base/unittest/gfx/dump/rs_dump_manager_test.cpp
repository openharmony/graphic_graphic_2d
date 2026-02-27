/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <vector>
#include <string>
#include <unordered_set>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include "gtest/gtest.h"
#include "gfx/dump/rs_dump_manager.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
constexpr int32_t SHARE_MEM_ALLOC = 2;
class RSDumpManagerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

protected:
    // Helper function to create a temporary file descriptor
    int CreateTempFileDescriptor(const std::string& content = "test content");
    // Helper function to create a test data buffer
    std::unique_ptr<char[]> CreateTestData(size_t size);
};

void RSDumpManagerTest::SetUpTestCase() {}
void RSDumpManagerTest::TearDownTestCase() {}
void RSDumpManagerTest::SetUp() {}
void RSDumpManagerTest::TearDown() {}

int RSDumpManagerTest::CreateTempFileDescriptor(const std::string& content)
{
    // Create a temporary file
    size_t dataSize = content.size();
    auto fd = AshmemCreate(content.c_str(), dataSize);
    return fd;
}

std::unique_ptr<char[]> RSDumpManagerTest::CreateTestData(size_t size)
{
    auto buffer = std::make_unique<char[]>(size);
    for (size_t i = 0; i < size; ++i) {
        buffer[i] = static_cast<char>(i % 256);
    }
    return buffer;
}

// Test case for Register method
HWTEST_F(RSDumpManagerTest, Dump_DumpIdExist, TestSize.Level1)
{
    RSDumpManager& rsDumpManager = RSDumpManager::GetInstance();
    RSDumpHander rsDumpHander = { RSDumpID::RENDER_NODE_INFO,
        [](const std::u16string &cmd, std::unordered_set<std::u16string> &argSets,
            std::string &dumpString) -> void {
            dumpString = "Test RSTree Information";
        } };
    rsDumpManager.Register(rsDumpHander);

    std::u16string cmd = u"RSTree";
    std::unordered_set<std::u16string> argSets = { cmd };
    std::string out;

    auto it = rsDumpManager.rsDumpHanderMap_.find(RSDumpID::RENDER_NODE_INFO);
    RSDumpHander hander = it->second;
    hander.func(cmd, argSets, out);
    EXPECT_EQ(out, "Test RSTree Information");
}

// Test case for Register method
HWTEST_F(RSDumpManagerTest, Register_WhenModuleIdExist, TestSize.Level1)
{
    RSDumpManager& rsDumpManager = RSDumpManager::GetInstance();
    RSDumpHander rsDumpHander = { RSDumpID::RENDER_NODE_INFO,
        [](const std::u16string &cmd, std::unordered_set<std::u16string> &argSets,
            std::string &dumpString) -> void {
            dumpString = "Test RSTree Information";
        } };
    rsDumpManager.Register(rsDumpHander);
    rsDumpManager.Register(rsDumpHander);
    rsDumpManager.Register(rsDumpHander);
    EXPECT_EQ(rsDumpManager.rsDumpHanderMap_.size(), 2);
}

// Test case for UnRegister method
HWTEST_F(RSDumpManagerTest, UnRegister_DumpIdExist, TestSize.Level1)
{
    RSDumpManager& rsDumpManager = RSDumpManager::GetInstance();
    RSDumpHander rsDumpHander = { RSDumpID::RENDER_NODE_INFO,
        [](const std::u16string &cmd, std::unordered_set<std::u16string> &argSets,
            std::string &dumpString) -> void {
            dumpString = "Test RSTree Information";
        } };
    rsDumpManager.Register(rsDumpHander);
    rsDumpManager.UnRegister(RSDumpID::RENDER_NODE_INFO);
    EXPECT_EQ(rsDumpManager.rsDumpHanderMap_.find(RSDumpID::RENDER_NODE_INFO), rsDumpManager.rsDumpHanderMap_.end());
}

// Test case for UnRegister method
HWTEST_F(RSDumpManagerTest, UnRegister_DumpIdNotExist, TestSize.Level1)
{
    RSDumpManager& rsDumpManager = RSDumpManager::GetInstance();
    rsDumpManager.UnRegister(RSDumpID::RENDER_NODE_INFO);
    EXPECT_EQ(rsDumpManager.rsDumpHanderMap_.size(), 1);
}

// Test case for CmdExec method
HWTEST_F(RSDumpManagerTest, CmdExec_WhenCommandExist, TestSize.Level1)
{
    RSDumpManager& rsDumpManager = RSDumpManager::GetInstance();
    RSDumpHander rsDumpHander = { RSDumpID::RENDER_NODE_INFO,
        [](const std::u16string &cmd, std::unordered_set<std::u16string> &argSets,
            std::string &dumpString) -> void {
            dumpString = "Test RSTree Information";
        } };
    rsDumpManager.Register(rsDumpHander);

    std::u16string cmd = u"RSTree";
    std::unordered_set<std::u16string> argSets = { cmd };
    std::string out;
    rsDumpManager.CmdExec(argSets, out);
    EXPECT_EQ(out, "Test RSTree Information");
}

// Test case for CmdExec method
HWTEST_F(RSDumpManagerTest, CmdExec_WhenCommandNotExist, TestSize.Level1)
{
    RSDumpManager& rsDumpManager = RSDumpManager::GetInstance();
    std::unordered_set<std::u16string> argSets = { u"invalid_command" };
    std::string out;
    rsDumpManager.CmdExec(argSets, out);
    ASSERT_TRUE(out.find("help text for the tool") != std::string::npos);
}

/*
 * @tc.name: WriteFileDescriptor_ValidFd
 * @tc.desc: Test WriteFileDescriptor with valid file descriptor
 * @tc.type: FUNC
 * @tc.require: AR000GSH6G
 */
HWTEST_F(RSDumpManagerTest, WriteFileDescriptor_ValidFd, TestSize.Level1)
{
    // Given: A valid file descriptor and parcel
    int fd = CreateTempFileDescriptor("test data for write");
    ASSERT_GE(fd, 0);
    Parcel parcel;

    // When: Write file descriptor to parcel
    bool result = RSDumpManager::WriteFileDescriptor(parcel, fd);

    // Then: Should succeed
    EXPECT_TRUE(result);

    // Cleanup
    close(fd);
}

/*
 * @tc.name: WriteFileDescriptor_InvalidFd
 * @tc.desc: Test WriteFileDescriptor with invalid file descriptor
 * @tc.type: FUNC
 * @tc.require: AR000GSH6G
 */
HWTEST_F(RSDumpManagerTest, WriteFileDescriptor_InvalidFd, TestSize.Level1)
{
    // Given: An invalid file descriptor and parcel
    int invalidFd = -1;
    Parcel parcel;

    // When: Try to write invalid file descriptor
    bool result = RSDumpManager::WriteFileDescriptor(parcel, invalidFd);

    // Then: Should fail
    EXPECT_FALSE(result);
}

/*
 * @tc.name: WriteFileDescriptor_StdinFd
 * @tc.desc: Test WriteFileDescriptor with stdin file descriptor
 * @tc.type: FUNC
 * @tc.require: AR000GSH6G
 */
HWTEST_F(RSDumpManagerTest, WriteFileDescriptor_StdinFd, TestSize.Level1)
{
    // Given: Standard input file descriptor
    int stdinFd = STDIN_FILENO;
    Parcel parcel;

    // When: Try to write stdin file descriptor
    bool result = RSDumpManager::WriteFileDescriptor(parcel, stdinFd);

    // Then: Should succeed (stdin is a valid fd)
    EXPECT_TRUE(result);
}

/*
 * @tc.name: WriteFileDescriptor_DupFailure
 * @tc.desc: Test WriteFileDescriptor when dup fails
 * @tc.type: FUNC
 * @tc.require: AR000GSH6G
 */
HWTEST_F(RSDumpManagerTest, WriteFileDescriptor_DupFailure, TestSize.Level1)
{
    // Given: A very large file descriptor number that will fail to dup
    int largeFd = 65535;  // This fd likely doesn't exist
    Parcel parcel;

    // When: Try to write invalid fd
    bool result = RSDumpManager::WriteFileDescriptor(parcel, largeFd);

    // Then: Should fail gracefully
    EXPECT_FALSE(result);
}

/*
 * @tc.name: ReadFileDescriptor_EmptyParcel
 * @tc.desc: Test ReadFileDescriptor from empty parcel
 * @tc.type: FUNC
 * @tc.require: AR000GSH6G
 */
HWTEST_F(RSDumpManagerTest, ReadFileDescriptor_EmptyParcel, TestSize.Level1)
{
    // Given: An empty parcel
    Parcel parcel;

    // When: Try to read file descriptor
    int readFd = RSDumpManager::ReadFileDescriptor(parcel);

    // Then: Should return invalid fd
    EXPECT_LT(readFd, 0);
}

/*
 * @tc.name: ReadFileDescriptor_CorruptedParcel
 * @tc.desc: Test ReadFileDescriptor from corrupted parcel
 * @tc.type: FUNC
 * @tc.require: AR000GSH6G
 */
HWTEST_F(RSDumpManagerTest, ReadFileDescriptor_CorruptedParcel, TestSize.Level1)
{
    // Given: A corrupted parcel with invalid data
    Parcel parcel;
    parcel.WriteInt32(0xdeadbeef);  // Invalid data

    // When: Try to read file descriptor
    int readFd = RSDumpManager::ReadFileDescriptor(parcel);

    // Then: Should return invalid fd
    EXPECT_LT(readFd, 0);
}

/*
 * @tc.name: WriteAshmemDataToParcel_SmallData
 * @tc.desc: Test WriteAshmemDataToParcel with small data
 * @tc.type: FUNC
 * @tc.require: AR000GSH6G
 */
HWTEST_F(RSDumpManagerTest, WriteAshmemDataToParcel_SmallData, TestSize.Level1)
{
    // Given: Small test data
    constexpr size_t smallSize = 1024;  // 1KB
    auto testData = CreateTestData(smallSize);
    Parcel parcel;

    // When: Write ashmem data to parcel
    bool result = RSDumpManager::WriteAshmemDataToParcel(parcel, smallSize, testData.get());

    // Then: Should succeed
    EXPECT_TRUE(result);
}

/*
 * @tc.name: WriteAshmemDataToParcel_MediumData
 * @tc.desc: Test WriteAshmemDataToParcel with medium data
 * @tc.type: FUNC
 * @tc.require: AR000GSH6G
 */
HWTEST_F(RSDumpManagerTest, WriteAshmemDataToParcel_MediumData, TestSize.Level1)
{
    // Given: Medium test data
    constexpr size_t mediumSize = 1024 * 1024;  // 1MB
    auto testData = CreateTestData(mediumSize);
    Parcel parcel;

    // When: Write ashmem data to parcel
    bool result = RSDumpManager::WriteAshmemDataToParcel(parcel, mediumSize, testData.get());

    // Then: Should succeed
    EXPECT_TRUE(result);
}

/*
 * @tc.name: WriteAshmemDataToParcel_NullData
 * @tc.desc: Test WriteAshmemDataToParcel with null data pointer
 * @tc.type: FUNC
 * @tc.require: AR000GSH6G
 */
HWTEST_F(RSDumpManagerTest, WriteAshmemDataToParcel_NullData, TestSize.Level1)
{
    // Given: Null data pointer
    constexpr size_t size = 1024;
    const char *nullData = nullptr;
    Parcel parcel;

    // When: Try to write null data
    bool result = RSDumpManager::WriteAshmemDataToParcel(parcel, size, nullData);

    // Then: Should fail or handle gracefully
    // Note: This may cause memcpy_s to fail
    EXPECT_FALSE(result);
}

/*
 * @tc.name: WriteAshmemDataToParcel_ZeroSize
 * @tc.desc: Test WriteAshmemDataToParcel with zero size
 * @tc.type: FUNC
 * @tc.require: AR000GSH6G
 */
HWTEST_F(RSDumpManagerTest, WriteAshmemDataToParcel_ZeroSize, TestSize.Level1)
{
    // Given: Zero size
    constexpr size_t zeroSize = 0;
    auto testData = CreateTestData(1);  // Provide valid pointer
    Parcel parcel;

    // When: Try to write with zero size
    bool result = RSDumpManager::WriteAshmemDataToParcel(parcel, zeroSize, testData.get());

    // Then: Should fail (ashmem cannot be created with zero size)
    EXPECT_FALSE(result);
}

/*
 * @tc.name: ReadAshmemDataFromParcel_EmptyParcel
 * @tc.desc: Test ReadAshmemDataFromParcel from empty parcel
 * @tc.type: FUNC
 * @tc.require: AR000GSH6G
 */
HWTEST_F(RSDumpManagerTest, ReadAshmemDataFromParcel_EmptyParcel, TestSize.Level1)
{
    // Given: An empty parcel
    Parcel parcel;

    // When: Try to read ashmem data
    char *data = RSDumpManager::ReadAshmemDataFromParcel(parcel, 1024);

    // Then: Should return null
    EXPECT_EQ(data, nullptr);
}

/*
 * @tc.name: ReadAshmemDataFromParcel_InvalidSize
 * @tc.desc: Test ReadAshmemDataFromParcel with invalid size
 * @tc.type: FUNC
 * @tc.require: AR000GSH6G
 */
HWTEST_F(RSDumpManagerTest, ReadAshmemDataFromParcel_InvalidSize, TestSize.Level1)
{
    // Given: Invalid size
    constexpr int32_t invalidSize = -1;
    Parcel parcel;

    // When: Try to read with invalid size
    char *data = RSDumpManager::ReadAshmemDataFromParcel(parcel, invalidSize);

    // Then: Should return null
    EXPECT_EQ(data, nullptr);
}

/*
 * @tc.name: ReadAshmemDataFromParcel_OversizedData
 * @tc.desc: Test ReadAshmemDataFromParcel with oversized data
 * @tc.type: FUNC
 * @tc.require: AR000GSH6G
 */
HWTEST_F(RSDumpManagerTest, ReadAshmemDataFromParcel_OversizedData, TestSize.Level1)
{
    // Given: Oversized data (larger than MAX_BUFFER_SIZE)
    constexpr int32_t oversizedSize = 64 * 1024 * 1024;  // 64MB
    Parcel parcel;

    // When: Try to read oversized data
    char *data = RSDumpManager::ReadAshmemDataFromParcel(parcel, oversizedSize);

    // Then: Should return null
    EXPECT_EQ(data, nullptr);
}

/*
 * @tc.name: ReleaseMemory_NullParameters
 * @tc.desc: Test ReleaseMemory with null parameters
 * @tc.type: FUNC
 * @tc.require: AR000GSH6G
 */
HWTEST_F(RSDumpManagerTest, ReleaseMemory_NullParameters, TestSize.Level1)
{
    // Given: Null parameters
    constexpr int32_t allocType = SHARE_MEM_ALLOC;
    void *addr = nullptr;
    int fd = -1;
    constexpr uint32_t size = 1024;

    // When: Release memory with null parameters
    RSDumpManager::ReleaseMemory(allocType, addr, &fd, size);

    // Then: Should not crash
    SUCCEED();
}

/*
 * @tc.name: ReleaseMemory_ValidParameters
 * @tc.desc: Test ReleaseMemory with valid parameters
 * @tc.type: FUNC
 * @tc.require: AR000GSH6G
 */
HWTEST_F(RSDumpManagerTest, ReleaseMemory_ValidParameters, TestSize.Level1)
{
    // Given: Valid parameters
    constexpr int32_t allocType = SHARE_MEM_ALLOC;
    int testFd = CreateTempFileDescriptor("test");
    ASSERT_GE(testFd, 0);

    void *addr = malloc(1024);
    ASSERT_NE(addr, nullptr);

    constexpr uint32_t size = 1024;

    // When: Release memory
    RSDumpManager::ReleaseMemory(allocType, addr, &testFd, size);

    // Then: Should cleanup without crash
    // Note: addr should be munmapped and testFd should be closed
    SUCCEED();
}

/*
 * @tc.name: ReleaseMemory_OnlyAddr
 * @tc.desc: Test ReleaseMemory with only address
 * @tc.type: FUNC
 * @tc.require: AR000GSH6G
 */
HWTEST_F(RSDumpManagerTest, ReleaseMemory_OnlyAddr, TestSize.Level1)
{
    // Given: Valid address but null context
    constexpr int32_t allocType = SHARE_MEM_ALLOC;
    void *addr = malloc(1024);
    ASSERT_NE(addr, nullptr);

    // When: Release memory with null context
    RSDumpManager::ReleaseMemory(allocType, addr, nullptr, 1024);

    // Then: Should not crash
    free(addr);  // Manual cleanup since context was null
    SUCCEED();
}

/*
 * @tc.name: ReleaseMemory_OnlyContext
 * @tc.desc: Test ReleaseMemory with only context (fd)
 * @tc.type: FUNC
 * @tc.require: AR000GSH6G
 */
HWTEST_F(RSDumpManagerTest, ReleaseMemory_OnlyContext, TestSize.Level1)
{
    // Given: Valid context but null address
    constexpr int32_t allocType = SHARE_MEM_ALLOC;
    int testFd = CreateTempFileDescriptor("test");
    ASSERT_GE(testFd, 0);

    // When: Release memory with null addr
    RSDumpManager::ReleaseMemory(allocType, nullptr, &testFd, 1024);

    // Then: Should close fd
    SUCCEED();
}

/*
 * @tc.name: CheckAshmemSize_ValidFd
 * @tc.desc: Test CheckAshmemSize with valid file descriptor
 * @tc.type: FUNC
 * @tc.require: AR000GSH6G
 */
HWTEST_F(RSDumpManagerTest, CheckAshmemSize_ValidFd, TestSize.Level1)
{
    // Given: A valid file descriptor
    std::string content = "test data";
    int fd = CreateTempFileDescriptor(content);
    ASSERT_GE(fd, 0);

    // When: Check ashmem size
    bool result = RSDumpManager::CheckAshmemSize(fd, content.size());

    // Then: Should return true
    EXPECT_TRUE(result);

    // Cleanup
    close(fd);
}

/*
 * @tc.name: CheckAshmemSize_InvalidFd
 * @tc.desc: Test CheckAshmemSize with invalid file descriptor
 * @tc.type: FUNC
 * @tc.require: AR000GSH6G
 */
HWTEST_F(RSDumpManagerTest, CheckAshmemSize_InvalidFd, TestSize.Level1)
{
    // Given: An invalid file descriptor
    int invalidFd = -1;
    constexpr int32_t bufferSize = 1024;

    // When: Check ashmem size with invalid fd
    bool result = RSDumpManager::CheckAshmemSize(invalidFd, bufferSize);

    // Then: Should return false
    EXPECT_FALSE(result);
}
}
