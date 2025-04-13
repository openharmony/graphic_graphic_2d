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

#include "intra_app_occlusion_culling_param.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {

class IntraAppOcclusionCullingParamTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void IntraAppOcclusionCullingParamTest::SetUpTestCase() {}
void IntraAppOcclusionCullingParamTest::TearDownTestCase() {}
void IntraAppOcclusionCullingParamTest::SetUp() {}
void IntraAppOcclusionCullingParamTest::TearDown() {}

/**
 * @tc.name: IntraAppOcclusionCullingParamTest
 * @tc.desc: Verify the IsIntraAppOcclusionCullingEnable function
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(IntraAppOcclusionCullingParamTest, IsIntraAppOcclusionCullingEnable, Function | SmallTest | Level1)
{
    IntraAppOcclusionCullingParam::SetIntraAppOcclusionCullingEnable(true);
    EXPECT_EQ(IntraAppOcclusionCullingParam::IsIntraAppOcclusionCullingEnable(), true);
    IntraAppOcclusionCullingParam::SetIntraAppOcclusionCullingEnable(false);
    EXPECT_EQ(IntraAppOcclusionCullingParam::IsIntraAppOcclusionCullingEnable(), false);
}

} // namespace Rosen
} // namespace OHOS