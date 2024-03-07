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

#include "shader_cache.h"

#include <cstring>

#include <gtest/gtest.h>
#include "platform/common/rs_system_properties.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class ShaderCacheTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void ShaderCacheTest::SetUpTestCase() {}
void ShaderCacheTest::TearDownTestCase() {}
void ShaderCacheTest::SetUp() {}
void ShaderCacheTest::TearDown() {}

/**
 * @tc.name: instance_test_001
 * @tc.desc: Verify the instanciation of ShaderCache
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(ShaderCacheTest, instance_test_001, TestSize.Level1)
{
#ifdef ACE_ENABLE_GL
    if (RSSystemProperties::GetGpuApiType() == GpuApiType::VULKAN ||
        RSSystemProperties::GetGpuApiType() == GpuApiType::DDGR) {
        GTEST_LOG_(INFO) << "vulkan enable! skip opengl test case";
        return;
    }
    GTEST_LOG_(INFO) << "ShaderCacheTest instance_test_001 start";
    /**
     * @tc.steps: step1. initialize a shader cache with empty cache directory
     */
    auto &cache = ShaderCache::Instance();
    std::string testedFileDir = "";
    cache.SetFilePath(testedFileDir);
    const char* identity = nullptr;
    std::shared_ptr<Drawing::Data> fakeData = std::make_shared<Drawing::Data>();
    /**
     * @tc.steps: step2. test initialization function
     */
    cache.InitShaderCache(identity, 0, false);
    EXPECT_EQ(nullptr, cache.Load(*fakeData));
#endif
}

/**
 * @tc.name: initialization_test_001
 * @tc.desc: Verify the initialization function of shader cache
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(ShaderCacheTest, initialization_test_001, TestSize.Level1)
{
#ifdef ACE_ENABLE_GL
    if (RSSystemProperties::GetGpuApiType() == GpuApiType::VULKAN ||
        RSSystemProperties::GetGpuApiType() == GpuApiType::DDGR) {
        GTEST_LOG_(INFO) << "vulkan enable! skip opengl test case";
        return;
    }
    GTEST_LOG_(INFO) << "ShaderCacheTest initialization_test_001 start";
    /**
     * @tc.steps: step1. initialize a shader cache instance and prepare parameters
     */
    auto &cache = ShaderCache::Instance();
    const char* identity = nullptr;
    std::shared_ptr<Drawing::Data> fakeData = std::make_shared<Drawing::Data>();
    /**
     * @tc.steps: step2. test initialization function
     */
    cache.InitShaderCache(identity, 0, false);
    EXPECT_EQ(nullptr, cache.Load(*fakeData));
#endif
}


/**
 * @tc.name: initialization_test_002
 * @tc.desc: Verify the file directory setting function of shader cache
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(ShaderCacheTest, initialization_test_002, TestSize.Level1)
{
#ifdef ACE_ENABLE_GL
    if (RSSystemProperties::GetGpuApiType() == GpuApiType::VULKAN ||
        RSSystemProperties::GetGpuApiType() == GpuApiType::DDGR) {
        GTEST_LOG_(INFO) << "vulkan enable! skip opengl test case";
        return;
    }
    GTEST_LOG_(INFO) << "ShaderCacheTest initialization_test_002 start";
    /**
     * @tc.steps: step1. initialize a shader cache instance and set the file dir
     */
    auto &cache = ShaderCache::Instance();
    std::string testedFileDir = "TemporalFilePath";
    cache.SetFilePath(testedFileDir);
    const char* identity = nullptr;
    const char* fakeBuffer = "testStr";
    std::shared_ptr<Drawing::Data> fakeData = std::make_shared<Drawing::Data>();
    fakeData->BuildWithCopy(fakeBuffer, 8);
    /**
     * @tc.steps: step2. test if the file direction is correctly set
     */
    cache.InitShaderCache(identity, 0, false);
    EXPECT_EQ(nullptr, cache.Load(*fakeData));
#endif
}

/**
 * @tc.name: store_test_001
 * @tc.desc: Verify the store function of shader cache with a zero-length
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(ShaderCacheTest, store_test_001, TestSize.Level1)
{
#ifdef ACE_ENABLE_GL
    if (RSSystemProperties::GetGpuApiType() == GpuApiType::VULKAN ||
        RSSystemProperties::GetGpuApiType() == GpuApiType::DDGR) {
        GTEST_LOG_(INFO) << "vulkan enable! skip opengl test case";
        return;
    }
    GTEST_LOG_(INFO) << "ShaderCacheTest store_test_001 start";
    /**
     * @tc.steps: step1. initialize a shader cache instance and prepare parameters
     */
    auto &cache = ShaderCache::Instance();
    std::string testedFileDir = "TemporalFilePath";
    cache.SetFilePath(testedFileDir);
    const char* identity = nullptr;
    std::shared_ptr<Drawing::Data> fakeKey = std::make_shared<Drawing::Data>();
    std::shared_ptr<Drawing::Data> fakeData = std::make_shared<Drawing::Data>();
    /**
     * @tc.steps: step2. test the store function with given parameters
     */
    cache.InitShaderCache(identity, 0, false);
    cache.Store(*fakeKey, *fakeData);
    EXPECT_EQ(nullptr, cache.Load(*fakeKey));
#endif
}

/**
 * @tc.name: store_test_002
 * @tc.desc: Verify the store function of shader cache with uninitialized status
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(ShaderCacheTest, store_test_002, TestSize.Level1)
{
#ifdef ACE_ENABLE_GL
    if (RSSystemProperties::GetGpuApiType() == GpuApiType::VULKAN ||
        RSSystemProperties::GetGpuApiType() == GpuApiType::DDGR) {
        GTEST_LOG_(INFO) << "vulkan enable! skip opengl test case";
        return;
    }
    GTEST_LOG_(INFO) << "ShaderCacheTest store_test_002 start";
    /**
     * @tc.steps: step1. initialize a shader cache instance and prepare parameters
     */
    auto &cache = ShaderCache::Instance();
    std::string testedFileDir = "";
    cache.SetFilePath(testedFileDir);
    const char* identity = nullptr;
    std::shared_ptr<Drawing::Data> fakeKey = std::make_shared<Drawing::Data>();
    std::shared_ptr<Drawing::Data> fakeData = std::make_shared<Drawing::Data>();
    /**
     * @tc.steps: step2. test the store function with given parameters
     */
    cache.InitShaderCache(identity, 0, false);
    cache.Store(*fakeKey, *fakeData);
    EXPECT_EQ(nullptr, cache.Load(*fakeKey));
#endif
}

/**
 * @tc.name: writing_test_001
 * @tc.desc: Verify the writeToDisk function with a zero-length hashID
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(ShaderCacheTest, writing_test_001, TestSize.Level1)
{
#ifdef ACE_ENABLE_GL
    if (RSSystemProperties::GetGpuApiType() == GpuApiType::VULKAN ||
        RSSystemProperties::GetGpuApiType() == GpuApiType::DDGR) {
        GTEST_LOG_(INFO) << "vulkan enable! skip opengl test case";
        return;
    }
    GTEST_LOG_(INFO) << "ShaderCacheTest writing_test_001 start";
    /**
     * @tc.steps: step1. initialize a shader cache instance and prepare parameters
     */
    auto &cache = ShaderCache::Instance();
    std::string testedFileDir = "TemporalFilePath";
    cache.SetFilePath(testedFileDir);
    const char* identity = nullptr;
    const char* fakeBuffer = "testStr";
    std::shared_ptr<Drawing::Data> fakeKey = std::make_shared<Drawing::Data>();
    fakeKey->BuildWithCopy(fakeBuffer, 8);
    std::shared_ptr<Drawing::Data> fakeData = std::make_shared<Drawing::Data>();
    fakeData->BuildWithCopy(fakeBuffer, 8);
    /**
     * @tc.steps: step2. verify the empty result of writeToDisk function
     */
    cache.InitShaderCache(identity, 0, false);
    cache.Store(*fakeKey, *fakeData);
    EXPECT_EQ(fakeData->GetSize(), cache.Load(*fakeKey)->GetSize());

#endif
}

/**
 * @tc.name: writing_test_002
 * @tc.desc: Verify the writeToDisk function with a non-zero-length hashID
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(ShaderCacheTest, writing_test_002, TestSize.Level1)
{
#ifdef ACE_ENABLE_GL
    if (RSSystemProperties::GetGpuApiType() == GpuApiType::VULKAN ||
        RSSystemProperties::GetGpuApiType() == GpuApiType::DDGR) {
        GTEST_LOG_(INFO) << "vulkan enable! skip opengl test case";
        return;
    }
    GTEST_LOG_(INFO) << "ShaderCacheTest writing_test_002 start";
    /**
     * @tc.steps: step1. initialize a shader cache instance and prepare parameters
     */
    auto &cache = ShaderCache::Instance();
    std::string testedFileDir = "TemporalFilePath";
    cache.SetFilePath(testedFileDir);
    const char* identity = "testIdentity";
    const char* fakeBuffer = "testStr";
    std::shared_ptr<Drawing::Data> fakeKey = std::make_shared<Drawing::Data>();
    fakeKey->BuildWithCopy(fakeBuffer, 8);
    std::shared_ptr<Drawing::Data> fakeData = std::make_shared<Drawing::Data>();
    fakeData->BuildWithCopy(fakeBuffer, 8);
    /**
     * @tc.steps: step2. verify the non-empty result of writeToDisk function
     */
    cache.InitShaderCache(identity, 16, false);
    cache.Store(*fakeKey, *fakeData);
    EXPECT_EQ(fakeData->GetSize(), cache.Load(*fakeKey)->GetSize());
#endif
}
} // namespace Rosen
} // namespace OHOS