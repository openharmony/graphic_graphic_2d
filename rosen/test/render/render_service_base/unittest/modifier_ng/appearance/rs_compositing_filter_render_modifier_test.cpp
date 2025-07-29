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

#include <cstdlib>
#include <cstring>
#include <memory>
#include <securec.h>

#include "gtest/gtest.h"
#include "gtest/hwext/gtest-tag.h"
#include "message_parcel.h"

#include "common/rs_vector4.h"
#include "modifier_ng/rs_modifier_ng_type.h"
#include "pipeline/rs_canvas_drawing_render_node.h"
#include "property/rs_properties.h"
#include "modifier_ng/appearance/rs_compositing_filter_render_modifier.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS::Rosen::ModifierNG;
namespace OHOS::Rosen {
class RSCompositingFilterRenderModifierNGTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSCompositingFilterRenderModifierNGTest::SetUpTestCase() {}
void RSCompositingFilterRenderModifierNGTest::TearDownTestCase() {}
void RSCompositingFilterRenderModifierNGTest::SetUp() {}
void RSCompositingFilterRenderModifierNGTest::TearDown() {}

/**
 * @tc.name: RSCompositingFilterRenderModifierTest
 * @tc.desc:GetType & ResetProperties & IsForeground
 * @tc.type:FUNC
 */
HWTEST_F(RSCompositingFilterRenderModifierNGTest, RSCompositingFilterRenderModifierTest, TestSize.Level1)
{
    RSCompositingFilterRenderModifier modifier;
    RSProperties properties;
    EXPECT_EQ(modifier.GetType(), ModifierNG::RSModifierType::COMPOSITING_FILTER);
    EXPECT_EQ(modifier.IsForeground(), true);
    modifier.ResetProperties(properties);
    EXPECT_EQ(properties.GetAiInvert(), std::nullopt);
    EXPECT_EQ(properties.GetGrayScale(), std::nullopt);
    EXPECT_EQ(properties.GetBrightness(), std::nullopt);
    EXPECT_EQ(properties.GetContrast(), std::nullopt);
    EXPECT_EQ(properties.GetSaturate(), std::nullopt);
    EXPECT_EQ(properties.GetSepia(), std::nullopt);
    EXPECT_EQ(properties.GetInvert(), std::nullopt);
    EXPECT_EQ(properties.GetHueRotate(), std::nullopt);
    EXPECT_EQ(properties.GetColorBlend(), std::nullopt);
    EXPECT_EQ(properties.GetLightUpEffect(), 1.f);
    EXPECT_EQ(properties.GetDynamicDimDegree(), std::nullopt);
    EXPECT_EQ(properties.GetFilter(), nullptr);
    EXPECT_EQ(properties.GetLinearGradientBlurPara(), nullptr);
    EXPECT_EQ(properties.GetForegroundBlurRadius(), 0.0f);
    EXPECT_EQ(properties.GetForegroundBlurSaturation(), 1.f);
    EXPECT_EQ(properties.GetForegroundBlurBrightness(), 1.f);
    EXPECT_EQ(properties.GetForegroundBlurMaskColor(), Color());
    EXPECT_EQ(properties.GetForegroundBlurColorMode(), 0);
    EXPECT_EQ(properties.GetForegroundBlurRadiusX(), 0.0f);
    EXPECT_EQ(properties.GetForegroundBlurRadiusY(), 0.0f);
    EXPECT_EQ(properties.GetFgBlurDisableSystemAdaptation(), true);
}
} // namespace OHOS::Rosen