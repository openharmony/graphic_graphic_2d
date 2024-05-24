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

#include "ge_hash.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace GraphicsEffectEngine {

// Test case for Hash32Next
TEST(GEHashTest, Hash32Next)
{
    GTEST_LOG_(INFO) << "GECacheTest Hash32Next start";

    const char* data = "Hello, world!";
    size_t bytes = strlen(data);
    uint32_t seed = 12345;              // Example seed
    uint32_t previousHash = 0;          // Initial hash value
    uint32_t expectedHash = 2835288348; // Compute expected hash manually

    // Compute hash using Hash32Next
    uint32_t hash = Hash32Next(previousHash, data, bytes, seed);

    // Check if the computed hash matches the expected hash
    EXPECT_EQ(hash, expectedHash);

    GTEST_LOG_(INFO) << "GECacheTest Hash32Next end";
}

// Test case for Hash64Next
TEST(GEHashTest, Hash64Next)
{
    GTEST_LOG_(INFO) << "GECacheTest Hash64Next start";

    const char* data = "Hello, world!";
    size_t bytes = strlen(data);
    uint64_t seed = 12345;     // Example seed
    uint64_t previousHash = 0; // Initial hash value
    uint64_t expectedHash = 18252719311013571113ull;

    // Compute hash using Hash64Next
    uint64_t hash = Hash64Next(previousHash, data, bytes, seed);

    // Check if the computed hash matches the expected hash
    EXPECT_EQ(hash, expectedHash);

    GTEST_LOG_(INFO) << "GECacheTest Hash64Next end";
}

} // namespace GraphicsEffectEngine
} // namespace OHOS
