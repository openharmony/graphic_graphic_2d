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
    EXPECT_EQ(retrievedValue, value);
    cache.Clear();

    GTEST_LOG_(INFO) << "GECacheTest SetAndGet end";
}

HWTEST_F(GECacheTest, OverwriteValue, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GECacheTest OverwriteValue start";

    // Arrange
    GECache& cache = GECache::GetInstance();
    size_t key = 123; // Set a random key
    std::shared_ptr<Drawing::Image> value1 = std::make_shared<Drawing::Image>();
    std::shared_ptr<Drawing::Image> value2 = std::make_shared<Drawing::Image>();

    // Act
    cache.Set(key, value1);
    cache.Set(key, value2);
    std::shared_ptr<Drawing::Image> retrievedValue = cache.Get(key);
    cache.Clear();

    // Assert
    EXPECT_TRUE(retrievedValue != nullptr);
    EXPECT_EQ(retrievedValue, value2);

    GTEST_LOG_(INFO) << "GECacheTest OverwriteValue end";
}

HWTEST_F(GECacheTest, EvictLeastUsed, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GECacheTest EvictLeastUsed start";

    // Arrange
    GECache& cache = GECache::GetInstance();
    cache.Clear();
    cache.SetMaxCapacity(2); // Assuming a method to set max capacity
    std::shared_ptr<Drawing::Image> value1 = std::make_shared<Drawing::Image>();
    std::shared_ptr<Drawing::Image> value2 = std::make_shared<Drawing::Image>();
    std::shared_ptr<Drawing::Image> value3 = std::make_shared<Drawing::Image>();
    // Act
    cache.Set(1, value1); // Use 1 as the key here
    cache.Set(2, value2); // Use 2 as the key here
    cache.Set(3, value3); // This should evict the least used value1, use 3 as the key here
    // Assert
    EXPECT_EQ(cache.Get(1), nullptr);
    EXPECT_TRUE(cache.Get(2) == value2);
    EXPECT_TRUE(cache.Get(3) == value3);

    GTEST_LOG_(INFO) << "GECacheTest EvictLeastUsed end";
}
} // namespace GraphicsEffectEngine
} // namespace OHOS
