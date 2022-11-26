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

#include <gtest/gtest.h>

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
 * @tc.name: ShadeCacheTest001
 * @tc.desc: Verify the initialization function of shader cache
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(ShaderCacheTest, ShadeCacheTest001, TestSize.Level1)
{
#ifdef ACE_ENABLE_GL
    GTEST_LOG_(INFO) << "ShaderCacheTest ShadeCacheTest001 start";
    /**
     * @tc.steps: step1. init a shader cache instance and prepare parameters
     */
    auto &cache = ShaderCache::Instance();
    const char* identity = nullptr;
    void* fakeBuffer = malloc(4);
    sk_sp<SkData> fakeData = SkData::MakeFromMalloc(fakeBuffer, 4);
    /**
     * @tc.steps: step2. test initialization function
     */
    cache.InitShaderCache(identity, 0, false);
    EXPECT_EQ(nullptr, cache.load(*fakeData));
    free(fakeBuffer);
#endif
}

/**
 * @tc.name: ShadeCacheTest002
 * @tc.desc: Verify the file directory setting function of shader cache
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(ShaderCacheTest, ShadeCacheTest002, TestSize.Level1)
{
#ifdef ACE_ENABLE_GL
    GTEST_LOG_(INFO) << "ShaderCacheTest ShadeCacheTest002 start";
    /**
     * @tc.steps: step1. init a shader cache instance and set the file dir
     */
    auto &cache = ShaderCache::Instance();
    std::string testedFileDir = "TestFileDir";
    cache.SetFilePath(testedFileDir);
    const char* identity = nullptr;
    void* fakeBuffer = malloc(4);
    sk_sp<SkData> fakeData = SkData::MakeFromMalloc(fakeBuffer, 4);
    /**
     * @tc.steps: step2. test file dir identification function
     */
    cache.InitShaderCache(identity, 0, false);
    EXPECT_EQ(nullptr, cache.load(*fakeData));
    free(fakeBuffer);
#endif
}
} // namespace Rosen
} // namespace OHOS