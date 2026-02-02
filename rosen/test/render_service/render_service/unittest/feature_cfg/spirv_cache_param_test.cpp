/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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
#include <test_header.h>

#include "spirv_cache_param.h"

using namespace testing;
using namespace testing::ext;
namespace OHOS {
namespace Rosen {

class SpirvCacheParamTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void SpirvCacheParamTest::SetUpTestCase() {}
void SpirvCacheParamTest::TearDownTestCase() {}
void SpirvCacheParamTest::SetUp() {}
void SpirvCacheParamTest::TearDown() {}

/**
 * @tc.name: SetSpirvCacheSize
 * @tc.desc: Verify the SetSpirvCacheSize function
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SpirvCacheParamTest, SetSpirvCacheSize, Function | SmallTest | Level1)
{
    SpirvCacheParam::SetSpirvCacheSize("10");
    ASSERT_EQ(SpirvCacheParam::GetSpirvCacheSize(), "10");
    SpirvCacheParam::SetSpirvCacheSize("5");
    ASSERT_EQ(SpirvCacheParam::GetSpirvCacheSize(), "5");
}
} // namespace Rosen
} // namespace OHOS