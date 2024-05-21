/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include <gtest/gtest.h>

#include "ge_cache.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace GraphicsEffectEngine {

class GECacheTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void GECacheTest::SetUpTestCase(void) {}
void GECacheTest::TearDownTestCase(void) {}

void GECacheTest::SetUp() {}
void GECacheTest::TearDown() {}

/**
 * @tc.name: SetAndGet
 * @tc.desc: Verify the Set and Get
 * @tc.type: FUNC
 */
HWTEST_F(GECacheTest, SetAndGet, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GECacheTest SetAndGet start";

    // Arrange
    GECache& cache = GECache::GetInstance();
    size_t key = 123;
    std::shared_ptr<Drawing::Image> value = std::make_shared<Drawing::Image>();

    // Act
    cache.Set(key, value);
    std::shared_ptr<Drawing::Image> retrievedValue = cache.Get(key);

    // Assert
    EXPECT_TRUE(retrievedValue != nullptr);

    GTEST_LOG_(INFO) << "GECacheTest SetAndGet end";
}

} // namespace Drawing
} // namespace OHOS
