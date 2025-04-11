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
    if (RSSystemProperties::IsUseVulkan()) {
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
    if (RSSystemProperties::IsUseVulkan()) {
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
    if (RSSystemProperties::IsUseVulkan()) {
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
    if (RSSystemProperties::IsUseVulkan()) {
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
    if (RSSystemProperties::IsUseVulkan()) {
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
    if (RSSystemProperties::IsUseVulkan()) {
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
    if (RSSystemProperties::IsUseVulkan()) {
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

/**
 * @tc.name: load_test_001
 * @tc.desc: Imporve Coverage
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(ShaderCacheTest, load_test_001, TestSize.Level1)
{
    auto &cache = ShaderCache::Instance();
    std::shared_ptr<Drawing::Data> fakeData = std::make_shared<Drawing::Data>();
    EXPECT_EQ(nullptr, cache.Load(*fakeData));
}

/**
 * @tc.name: querryShaderNum_test_001
 * @tc.desc: Imporve Coverage
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(ShaderCacheTest, querryShaderNum_test_001, TestSize.Level1)
{
    auto &cache = ShaderCache::Instance();
    const char* identity = nullptr;
    cache.InitShaderCache(identity, 0, false);
    cache.CleanAllShaders();
    EXPECT_EQ(0, cache.QuerryShaderNum());
    const char* testKey1 = "testKey1";
    const char* testValue1 = "testValue1";
    cache.cacheData_->Rewrite(testKey1, 8, testValue1, 8);
    EXPECT_EQ(1, cache.QuerryShaderNum()); // 1, the true shardernum
}

/**
 * @tc.name: storeShader_test_001
 * @tc.desc: Imporve Coverage
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(ShaderCacheTest, storeShader_test_001, TestSize.Level1)
{
    auto &cache = ShaderCache::Instance();
    std::string testedFileDir = "testFilePath";
    cache.SetFilePath(testedFileDir);
    const char* identity = nullptr;
    std::shared_ptr<Drawing::Data> fakeKey = std::make_shared<Drawing::Data>();
    std::shared_ptr<Drawing::Data> fakeData = std::make_shared<Drawing::Data>();
    cache.InitShaderCache(identity, 0, false);
    cache.Store(*fakeKey, *fakeData);
    EXPECT_EQ(nullptr, cache.Load(*fakeKey));
    cache.cacheData_ = nullptr;
    cache.Store(*fakeKey, *fakeData);
    cache.Load(*fakeKey);
}

/**
 * @tc.name: InitShaderCacheTest
 * @tc.desc: Imporve Coverage
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(ShaderCacheTest, InitShaderCacheTest, TestSize.Level1)
{
    auto &cache = ShaderCache::Instance();
    cache.filePath_ = "";
    cache.InitShaderCache(nullptr, 0, false);
    cache.filePath_ = "test";
    cache.InitShaderCache(nullptr, 0, false);
    EXPECT_NE(cache.cacheData_, nullptr);
}

/**
 * @tc.name: StoreTest
 * @tc.desc: Imporve Coverage
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(ShaderCacheTest, StoreTest, TestSize.Level1)
{
    auto &cache = ShaderCache::Instance();
    cache.InitShaderCache();
    cache.CleanAllShaders();
    Drawing::Data key;
    Drawing::Data value;
    cache.Store(key, value);
    size_t test1 = cache.QuerryShaderSize();
    EXPECT_EQ(0, test1);
    // for test
    size_t size = 4;
    key.BuildUninitialized(size);
    value.BuildUninitialized(size);
    cache.Store(key, value);
    size_t test2 = cache.QuerryShaderSize();
    EXPECT_NE(0, test2);
}
} // namespace Rosen
} // namespace OHOS