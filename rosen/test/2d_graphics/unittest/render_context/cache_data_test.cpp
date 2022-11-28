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
 * @tc.name: CacheDataTest001
 * @tc.desc: Verify the initialization function of shader cache
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(CacheDataTest, CacheDataTest001, TestSize.Level1)
{
#ifdef ACE_ENABLE_GL
    GTEST_LOG_(INFO) << "CacheDataTest CacheDataTest001 start";
    /**
     * @tc.steps: step1. init a cachedata
     */
    std::shared_ptr<CacheData> cacheData = std::make_shared<CacheData>(0, 0, 0, "TestDir");
    /**
     * @tc.steps: step2. test initialization function
     */
    EXPECT_NE(nullptr, cacheData.get());
#endif
}

/**
 * @tc.name: CacheDataTest002
 * @tc.desc: Verify the function that computes the file size after serialization
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(CacheDataTest, CacheDataTest002, TestSize.Level1)
{
#ifdef ACE_ENABLE_GL
    GTEST_LOG_(INFO) << "CacheDataTest CacheDataTest002 start";
    /**
     * @tc.steps: step1. init a cachedata
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
 * @tc.name: CacheDataTest003
 * @tc.desc: Verify the function that gets a k-v pair of data
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(CacheDataTest, CacheDataTest003, TestSize.Level1)
{
#ifdef ACE_ENABLE_GL
    GTEST_LOG_(INFO) << "CacheDataTest CacheDataTest003 start";
    /**
     * @tc.steps: step1. init a cachedata
     */
    std::string testFileDir = "test file dir for cachedata";
    std::shared_ptr<CacheData> cacheData = std::make_shared<CacheData>(0, 0, 0, testFileDir);
    /**
     * @tc.steps: step2. test the data grabbing function
     */
    size_t sizeGet = cacheData->Get(nullptr, 1, nullptr, 1);
    EXPECT_EQ(0, sizeGet);
#endif
}

/**
 * @tc.name: CacheDataTest004
 * @tc.desc: Verify the serialization function
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(CacheDataTest, CacheDataTest004, TestSize.Level1)
{
#ifdef ACE_ENABLE_GL
    GTEST_LOG_(INFO) << "CacheDataTest CacheDataTest004 start";
    /**
     * @tc.steps: step1. init a cachedata
     */
    std::string testFileDir = "test file dir for cachedata";
    std::shared_ptr<CacheData> cacheData = std::make_shared<CacheData>(0, 0, 0, testFileDir);
    /**
     * @tc.steps: step2. test the serialization function
     */
    uint8_t *tempBuffer = new uint8_t[sizeof(size_t)];
    int retSerialized = cacheData->Serialize(tempBuffer, sizeof(size_t));
    EXPECT_NE(retSerialized, -EINVAL);
    delete[] tempBuffer;
#endif
}
} // namespace Rosen
} // namespace OHOS