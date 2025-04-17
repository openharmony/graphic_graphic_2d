/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "cache_data.h"

#include <cerrno>

#include <gtest/gtest.h>
#include "platform/common/rs_system_properties.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class CacheDataTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void CacheDataTest::SetUpTestCase() {}
void CacheDataTest::TearDownTestCase() {}
void CacheDataTest::SetUp() {}
void CacheDataTest::TearDown() {}

/**
 * @tc.name: cachedata_init_test_001
 * @tc.desc: Verify the initialization function of shader cache
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(CacheDataTest, cachedata_init_test_001, TestSize.Level1)
{
#ifdef ACE_ENABLE_GL
    if (RSSystemProperties::IsUseVulkan()) {
        GTEST_LOG_(INFO) << "vulkan enable! skip opengl test case";
        return;
    }
    GTEST_LOG_(INFO) << "CacheDataTest cachedata_init_test_001 start";
    /**
     * @tc.steps: step1. initialize a cachedata
     */
    std::shared_ptr<CacheData> cacheData = std::make_shared<CacheData>(0, 0, 0, "TestDir");
    /**
     * @tc.steps: step2. test the result of initialization function
     */
    EXPECT_NE(nullptr, cacheData.get());
#endif
}

/**
 * @tc.name: serialized_size_test_001
 * @tc.desc: Verify the function that computes the file size after serialization
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(CacheDataTest, serialized_size_test_001, TestSize.Level1)
{
#ifdef ACE_ENABLE_GL
    if (RSSystemProperties::IsUseVulkan()) {
        GTEST_LOG_(INFO) << "vulkan enable! skip opengl test case";
        return;
    }
    GTEST_LOG_(INFO) << "CacheDataTest serialized_size_test_001 start";
    /**
     * @tc.steps: step1. initialize a cachedata
     */
    std::shared_ptr<CacheData> cacheData = std::make_shared<CacheData>(0, 0, 0, "TestDir");
    struct tempHeader {
        size_t numShaders_;
    };
    /**
     * @tc.steps: step2. tests the size computation
     */
    size_t serSize = cacheData->SerializedSize();
    EXPECT_EQ(sizeof(tempHeader), serSize);
#endif
}

/**
 * @tc.name: get_data_test_001
 * @tc.desc: Verify the function that gets a k-v pair of data
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(CacheDataTest, get_data_test_001, TestSize.Level1)
{
#ifdef ACE_ENABLE_GL
    if (RSSystemProperties::IsUseVulkan()) {
        GTEST_LOG_(INFO) << "vulkan enable! skip opengl test case";
        return;
    }
    GTEST_LOG_(INFO) << "CacheDataTest get_data_test_001 start";
    /**
     * @tc.steps: step1. initialize a cachedata
     */
    std::string testFileDir = "test file dir for cachedata";
    std::shared_ptr<CacheData> cacheData = std::make_shared<CacheData>(0, 0, 0, testFileDir);
    /**
     * @tc.steps: step2. test the data grabbing function
     */
    auto [errorCode, sizeGet] = cacheData->Get(nullptr, 1, nullptr, 1);
    EXPECT_EQ(0, sizeGet);
#endif
}

/**
 * @tc.name: serialization_test_001
 * @tc.desc: Verify the serialization function
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(CacheDataTest, serialization_test_001, TestSize.Level1)
{
#ifdef ACE_ENABLE_GL
    if (RSSystemProperties::IsUseVulkan()) {
        GTEST_LOG_(INFO) << "vulkan enable! skip opengl test case";
        return;
    }
    GTEST_LOG_(INFO) << "CacheDataTest serialization_test_001 start";
    /**
     * @tc.steps: step1. initialize a cachedata
     */
    std::string testFileDir = "test file dir for cachedata";
    std::shared_ptr<CacheData> cacheData = std::make_shared<CacheData>(0, 0, 0, testFileDir);
    /**
     * @tc.steps: step2. test the serialization function
     */
    uint8_t *tempBuffer = new uint8_t[sizeof(size_t)]();
    int retSerialized = cacheData->Serialize(tempBuffer, sizeof(size_t));
    EXPECT_NE(retSerialized, -EINVAL);
    delete[] tempBuffer;
#endif
}

/**
 * @tc.name: deserialization_test_001
 * @tc.desc: Verify the deserialization function
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(CacheDataTest, deserialization_test_001, TestSize.Level1)
{
#ifdef ACE_ENABLE_GL
    if (RSSystemProperties::IsUseVulkan()) {
        GTEST_LOG_(INFO) << "vulkan enable! skip opengl test case";
        return;
    }
    GTEST_LOG_(INFO) << "CacheDataTest deserialization_test_001 start";
    /**
     * @tc.steps: step1. initialize a cachedata
     */
    std::string testFileDir = "test file dir for cachedata";
    std::shared_ptr<CacheData> cacheData = std::make_shared<CacheData>(0, 0, 0, testFileDir);
    /**
     * @tc.steps: step2. test the deserialization function
     */
    uint8_t *tempBuffer = new uint8_t[sizeof(size_t)]();
    int retDeserialized = cacheData->DeSerialize(tempBuffer, sizeof(size_t));
    EXPECT_NE(retDeserialized, -EINVAL);
    delete[] tempBuffer;
#endif
}

/**
 * @tc.name: write_data_test_001
 * @tc.desc: Verify the rewrite and get function with overloaded data
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(CacheDataTest, write_data_test_001, TestSize.Level1)
{
#ifdef ACE_ENABLE_GL
    if (RSSystemProperties::IsUseVulkan()) {
        GTEST_LOG_(INFO) << "vulkan enable! skip opengl test case";
        return;
    }
    GTEST_LOG_(INFO) << "CacheDataTest write_data_test_001 start";
    /**
     * @tc.steps: step1. initialize a cachedata
     */
    std::string testFileDir = "test file dir for cachedata";
    std::shared_ptr<CacheData> cacheData = std::make_shared<CacheData>(2, 2, 4, testFileDir);
    /**
     * @tc.steps: step2. test the rewrite and get function
     */
    uint8_t *tempBuffer = new uint8_t[8]();
    cacheData->Rewrite(tempBuffer, 8, tempBuffer, 8);
    auto [errorCode, sizeGet] = cacheData->Get(tempBuffer, 8, tempBuffer, 8);
    EXPECT_EQ(0, sizeGet);
    delete[] tempBuffer;
#endif
}

/**
 * @tc.name: clean_data_test_001
 * @tc.desc: Verify the clean function
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(CacheDataTest, clean_data_test_001, TestSize.Level1)
{
#ifdef ACE_ENABLE_GL
    if (RSSystemProperties::IsUseVulkan()) {
        GTEST_LOG_(INFO) << "vulkan enable! skip opengl test case";
        return;
    }
    GTEST_LOG_(INFO) << "CacheDataTest clean_data_test_001 start";
    /**
     * @tc.steps: step1. initialize a cachedata
     */
    std::string testFileDir = "test file dir for cachedata";
    std::shared_ptr<CacheData> cacheData = std::make_shared<CacheData>(8, 8, 15, testFileDir);
    /**
     * @tc.steps: step2. test the clean function
     */
    uint8_t *tempBuffer = new uint8_t[4]();
    const char *testKey1 = "Key1";
    const char *testKey2 = "Key2";
    const char *testKey3 = "Key3";
    const char *testKey4 = "Key4";
    const char *testValue = "aVal";
    cacheData->Rewrite(testKey1, 4, testValue, 4);
    cacheData->Rewrite(testKey2, 4, testValue, 4);
    cacheData->Rewrite(testKey3, 4, testValue, 4);
    cacheData->Rewrite(testKey4, 4, testValue, 4);
    auto [errorCode, sizeGet] = cacheData->Get(testKey4, 4, tempBuffer, 4);
    EXPECT_EQ(4, sizeGet);
    delete[] tempBuffer;
#endif
}

/**
 * @tc.name: clean_data_test_002
 * @tc.desc: Verify a vain clean function
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(CacheDataTest, clean_data_test_002, TestSize.Level1)
{
#ifdef ACE_ENABLE_GL
    if (RSSystemProperties::IsUseVulkan()) {
        GTEST_LOG_(INFO) << "vulkan enable! skip opengl test case";
        return;
    }
    GTEST_LOG_(INFO) << "CacheDataTest clean_data_test_002 start";
    /**
     * @tc.steps: step1. initialize a cachedata
     */
    std::string testFileDir = "test file dir for cachedata";
    std::shared_ptr<CacheData> cacheData = std::make_shared<CacheData>(4, 4, 6, testFileDir);
    /**
     * @tc.steps: step2. test the clean function that skips a vain clean
     */
    uint8_t *tempBuffer = new uint8_t[4]();
    const char *testKey1 = "Key1";
    const char *testKey2 = "Key2";
    const char *testValue = "aVal";
    cacheData->Rewrite(testKey1, 4, testValue, 4);
    cacheData->Rewrite(testKey2, 4, testValue, 4);
    auto [errorCode, sizeGet] = cacheData->Get(testKey2, 4, tempBuffer, 4);
    EXPECT_EQ(0, sizeGet);
    delete[] tempBuffer;
#endif
}

/**
 * @tc.name: ReadFromFileTest
 * @tc.desc: Imporve Coverage
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
 HWTEST_F(CacheDataTest, ReadFromFileTest, TestSize.Level1)
{
    std::shared_ptr<CacheData> cacheData1 = std::make_shared<CacheData>(0, 0, 0, "TestDir");
    EXPECT_NE(nullptr, cacheData1.get());
    cacheData1->ReadFromFile();
    std::shared_ptr<CacheData> cacheData2 = std::make_shared<CacheData>(0, 0, 0, "");
    EXPECT_NE(nullptr, cacheData2.get());
    cacheData2->ReadFromFile();
}

/**
 * @tc.name: WriteToFileTest
 * @tc.desc: Imporve Coverage
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
 HWTEST_F(CacheDataTest, WriteToFileTest, TestSize.Level1)
{
    std::shared_ptr<CacheData> cacheData1 = std::make_shared<CacheData>(0, 0, 0, "TestDir");
    EXPECT_NE(nullptr, cacheData1.get());
    cacheData1->WriteToFile();
    std::shared_ptr<CacheData> cacheData2 = std::make_shared<CacheData>(0, 0, 0, "");
    EXPECT_NE(nullptr, cacheData2.get());
    cacheData2->WriteToFile();
}


/**
 * @tc.name: SerializeTest001
 * @tc.desc: Imporve Coverage
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
 HWTEST_F(CacheDataTest, SerializeTest001, TestSize.Level1)
{
    std::string testFileDir = "testCachedata";
    std::shared_ptr<CacheData> cacheData = std::make_shared<CacheData>(0, 0, 0, testFileDir);
    uint8_t *tempBuffer = new uint8_t[sizeof(size_t)]();
    int retSerialized1 = cacheData->Serialize(tempBuffer, sizeof(size_t));
    EXPECT_NE(retSerialized1, -EINVAL);
    int retSerialized2 =  cacheData->Serialize(tempBuffer, 0);
    EXPECT_EQ(retSerialized2, -EINVAL);
    delete[] tempBuffer;
}

/**
 * @tc.name: DeSerializeTest001
 * @tc.desc: Imporve Coverage
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
 HWTEST_F(CacheDataTest, DeSerializeTest001, TestSize.Level1)
{
    std::string testFileDir = "testCachedata";
    std::shared_ptr<CacheData> cacheData = std::make_shared<CacheData>(0, 0, 0, testFileDir);
    uint8_t *tempBuffer = new uint8_t[sizeof(size_t)]();
    int retDeserialized1 = cacheData->DeSerialize(tempBuffer, sizeof(size_t));
    EXPECT_NE(retDeserialized1, -EINVAL);
    delete[] tempBuffer;
}

/**
 * @tc.name: RewriteTest001
 * @tc.desc: Imporve Coverage
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
 HWTEST_F(CacheDataTest, RewriteTest001, TestSize.Level1)
{
    std::string testFileDir = "testCachedata";
    std::shared_ptr<CacheData> cacheData = std::make_shared<CacheData>(4, 4, 6, testFileDir);
    uint8_t *tempBuffer = new uint8_t[4]();
    const char *testKey1 = "key1";
    const char *testValue = "val1";
    cacheData->Rewrite(testKey1, 4, testValue, 4);
    auto [errorCode, sizeGet] = cacheData->Get(testKey1, 4, tempBuffer, 4);
    EXPECT_EQ(0, sizeGet);
    delete[] tempBuffer;
}

/**
 * @tc.name: IfSizeValidateTest
 * @tc.desc: Imporve Coverage
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
 HWTEST_F(CacheDataTest, IfSizeValidateTest, TestSize.Level1)
{
    std::string testFileDir = "testCachedata";
    std::shared_ptr<CacheData> cacheData = std::make_shared<CacheData>(4, 4, 6, testFileDir);
    EXPECT_TRUE(cacheData->IfSizeValidate(2, 2));
    EXPECT_TRUE(cacheData->IfSizeValidate(7, 0));
    EXPECT_FALSE(cacheData->IfSizeValidate(7, 2));
}

/**
 * @tc.name: IfSkipCleanTest
 * @tc.desc: Imporve Coverage
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
 HWTEST_F(CacheDataTest, IfSkipCleanTest, TestSize.Level1)
{
    std::string testFileDir = "testCachedata";
    std::shared_ptr<CacheData> cacheData = std::make_shared<CacheData>(4, 4, 6, testFileDir);
    EXPECT_TRUE(cacheData->IfSkipClean(4));
    EXPECT_FALSE(cacheData->IfSkipClean(3));
}

/**
 * @tc.name: IfSkipCleanTest
 * @tc.desc: Imporve Coverage
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
 HWTEST_F(CacheDataTest, IfCleanFinishedTest, TestSize.Level1)
{
    std::string testFileDir = "testCachedata";
    std::shared_ptr<CacheData> cacheData = std::make_shared<CacheData>(4, 4, 6, testFileDir);
    cacheData->cleanThreshold_ = 0;
    EXPECT_TRUE(cacheData->IfCleanFinished());
    cacheData->cleanThreshold_ = 1;
    EXPECT_FALSE(cacheData->IfCleanFinished());
}

/**
 * @tc.name: RandCleanTest
 * @tc.desc: Imporve Coverage
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
 HWTEST_F(CacheDataTest, RandCleanTest, TestSize.Level1)
{
    std::string testFileDir = "testCachedata";
    std::shared_ptr<CacheData> cacheData = std::make_shared<CacheData>(4, 4, 6, testFileDir);
    cacheData->RandClean(0);
    cacheData->cleanThreshold_ = 0;
    cacheData->RandClean(1);
    EXPECT_LE(cacheData->totalSize_, 1);
}

/**
 * @tc.name: CleanTest
 * @tc.desc: Imporve Coverage
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
 HWTEST_F(CacheDataTest, CleanTest, TestSize.Level1)
{
    std::string testFileDir = "testCachedata";
    std::shared_ptr<CacheData> cacheData = std::make_shared<CacheData>(4, 4, 6, testFileDir);
    cacheData->shaderPointers_ = {};
    EXPECT_EQ(0, cacheData->Clean(2));
}

/**
 * @tc.name: GetTest
 * @tc.desc: Imporve Coverage
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
 HWTEST_F(CacheDataTest, GetTest, TestSize.Level1)
{
    std::string testFileDir = "testCachedata";
    std::shared_ptr<CacheData> cacheData = std::make_shared<CacheData>(4, 4, 6, testFileDir);
    auto returnData = cacheData->Get(nullptr, 5, nullptr, 1);
    std::tuple<CacheData::ErrorCode, size_t> testData = {CacheData::ErrorCode::VALUE_SIZE_OVER_MAX_SIZE, 0};
    EXPECT_EQ(std::get<0>(returnData), std::get<0>(testData));
    EXPECT_EQ(std::get<1>(returnData), std::get<1>(testData));
}

/**
 * @tc.name: CleanInitTest_001
 * @tc.desc: Purge Shader Cache at Free Time
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
 HWTEST_F(CacheDataTest, CleanInitTest_001, TestSize.Level1)
{
    std::string testFileDir = "testCachedata";
    std::shared_ptr<CacheData> cacheData = std::make_shared<CacheData>(4, 4, 6, testFileDir);
    EXPECT_EQ(cacheData->CleanInit(), true);
}

/**
 * @tc.name: CleanInitTest_002
 * @tc.desc: Purge Shader Cache at Free Time
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
 HWTEST_F(CacheDataTest, CleanInitTest_002, TestSize.Level1)
{
    std::string testFileDir = "testCachedata";
    std::shared_ptr<CacheData> cacheData = std::make_shared<CacheData>(4, 4, 6, testFileDir);
    std::vector<unsigned short> originValues(cacheData->cleanInit_, cacheData->cleanInit_ + cacheData->randLength_);
    cacheData->CleanInit();
    bool isChanged = false;
    for (int i = 0; i < cacheData->randLength_; ++i) {
        if (cacheData->cleanInit_[i] != originValues[i]) {
            isChanged = true;
            break;
        }
    }
    EXPECT_EQ(isChanged, true);
}

/**
 * @tc.name: CheckShaderCacheOverSoftLimitTest
 * @tc.desc: Purge Shader Cache at Free Time
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
 HWTEST_F(CacheDataTest, CheckShaderCacheOverSoftLimitTest, TestSize.Level1)
{
    std::string testFileDir = "testCachedata";
    std::shared_ptr<CacheData> cacheData = std::make_shared<CacheData>(4, 4, 6, testFileDir);
    bool check = cacheData->totalSize_ > cacheData->softLimit_;
    EXPECT_EQ(cacheData->CheckShaderCacheOverSoftLimit(), check);
}

/**
 * @tc.name: PurgeShaderCacheAfterAnimateTest
 * @tc.desc: Purge Shader Cache at Free Time
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
 HWTEST_F(CacheDataTest, PurgeShaderCacheAfterAnimateTest, TestSize.Level1)
{
    std::string testFileDir = "testCachedata";
    std::shared_ptr<CacheData> cacheData = std::make_shared<CacheData>(4, 4, 6, testFileDir);
    cacheData->PurgeShaderCacheAfterAnimate([]() { return true; });
    EXPECT_GE(6, cacheData->totalSize_);
}
} // namespace Rosen
} // namespace OHOS