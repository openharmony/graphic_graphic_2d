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
#include <limits>
#include <test_header.h>

#include "hgm_lru_cache.h"
#include "hgm_test_base.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class HgmLRUCacheTest : public HgmTestBase {
public:
    static void SetUpTestCase()
    {
        HgmTestBase::SetUpTestCase();
    }
    static void TearDownTestCase() {}
    void SetUp() {}
    void TearDown() {}
};

/**
 * @tc.name: Put
 * @tc.desc: Verify the result of Put function
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmLRUCacheTest, Put, Function | SmallTest | Level0)
{
    constexpr int32_t cacheSize = 3;
    constexpr int32_t val0 = 0;
    constexpr int32_t val1 = 1;
    constexpr int32_t val2 = 2;
    constexpr int32_t val3 = 3;
    constexpr int32_t val4 = 4;

    auto cache = HgmLRUCache<int32_t>(cacheSize);
    cache.Put(val0);
    ASSERT_TRUE(cache.Existed(val0));

    cache.Put(val1);
    ASSERT_TRUE(cache.Existed(val0));
    ASSERT_TRUE(cache.Existed(val1));

    cache.Put(val2);
    ASSERT_TRUE(cache.Existed(val0));
    ASSERT_TRUE(cache.Existed(val1));
    ASSERT_TRUE(cache.Existed(val2));

    cache.Put(val3);
    ASSERT_FALSE(cache.Existed(val0));
    ASSERT_TRUE(cache.Existed(val1));
    ASSERT_TRUE(cache.Existed(val2));
    ASSERT_TRUE(cache.Existed(val3));

    cache.Put(val2);
    ASSERT_TRUE(cache.Existed(val1));
    ASSERT_TRUE(cache.Existed(val2));
    ASSERT_TRUE(cache.Existed(val3));

    cache.Put(val1);
    ASSERT_TRUE(cache.Existed(val1));
    ASSERT_TRUE(cache.Existed(val2));
    ASSERT_TRUE(cache.Existed(val3));

    cache.Put(val4);
    ASSERT_FALSE(cache.Existed(val0));
    ASSERT_TRUE(cache.Existed(val1));
    ASSERT_TRUE(cache.Existed(val2));
    ASSERT_FALSE(cache.Existed(val3));
    ASSERT_TRUE(cache.Existed(val4));
}
} // namespace Rosen
} // namespace OHOS