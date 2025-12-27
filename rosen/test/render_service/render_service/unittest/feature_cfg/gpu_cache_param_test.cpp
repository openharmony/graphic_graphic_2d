/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "gpu_cache_param.h"

using namespace testing;
using namespace testing::ext;
namespace OHOS {
namespace Rosen {

class GpuCacheParamTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void GpuCacheParamTest::SetUpTestCase() {}
void GpuCacheParamTest::TearDownTestCase() {}
void GpuCacheParamTest::SetUp() {}
void GpuCacheParamTest::TearDown() {}

/**
 * @tc.name: GetRSGpuCacheSize
 * @tc.desc: Verify the GetRSGpuCacheSize function
 * @tc.type: FUNC
 * @tc.require: #IBIE4T
 */
HWTEST_F(GpuCacheParamTest, GetRSGpuCacheSize, Function | SmallTest | Level1)
{
    GpuCacheParam::SetRSGpuCacheSize(500);
    EXPECT_EQ(GpuCacheParam::GetRSGpuCacheSize(), 500);
}

/**
 * @tc.name: GetGpuCacheConfigEnable
 * @tc.desc: Verify the GetGpuCacheConfigEnable function
 * @tc.type: FUNC
 * @tc.require: #IBIE4T
 */
HWTEST_F(GpuCacheParamTest, GetGpuCacheConfigEnable, Function | SmallTest | Level1)
{
    GpuCacheParam::SetGpuCacheConfigEnable(true);
    EXPECT_EQ(GpuCacheParam::GetGpuCacheConfigEnable(), true);
}

} // namespace Rosen
} // namespace OHOS