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

#include "stencil_pixel_occlusion_culling_param.h"

using namespace testing;
using namespace testing::ext;
namespace OHOS {
namespace Rosen {

class StencilPixelOcclusionCullingParamTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void StencilPixelOcclusionCullingParamTest::SetUpTestCase() {}
void StencilPixelOcclusionCullingParamTest::TearDownTestCase() {}
void StencilPixelOcclusionCullingParamTest::SetUp() {}
void StencilPixelOcclusionCullingParamTest::TearDown() {}

/**
 * @tc.name: SetStencilPixelOcclusionCullingEnable
 * @tc.desc: Verify the SetStencilPixelOcclusionCullingEnable function
 * @tc.type: FUNC
 * @tc.require: #IBPXUM
 */
HWTEST_F(StencilPixelOcclusionCullingParamTest, SetStencilPixelOcclusionCullingEnable, Function | SmallTest | Level1)
{
    StencilPixelOcclusionCullingParam::SetStencilPixelOcclusionCullingEnable(true);
    ASSERT_TRUE(StencilPixelOcclusionCullingParam::IsStencilPixelOcclusionCullingEnable());
}

/**
 * @tc.name: IsStencilPixelOcclusionCullingEnable
 * @tc.desc: Verify the result of IsStencilPixelOcclusionCullingEnable function
 * @tc.type: FUNC
 * @tc.require: #IBPXUM
 */
HWTEST_F(StencilPixelOcclusionCullingParamTest, IsStencilPixelOcclusionCullingEnable, Function | SmallTest | Level1)
{
    StencilPixelOcclusionCullingParam::SetStencilPixelOcclusionCullingEnable(false);
    ASSERT_FALSE(StencilPixelOcclusionCullingParam::IsStencilPixelOcclusionCullingEnable());
}
} // namespace Rosen
} // namespace OHOS