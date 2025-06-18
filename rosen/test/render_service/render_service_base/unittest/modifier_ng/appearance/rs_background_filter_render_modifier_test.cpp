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
#include "modifier_ng/appearance/rs_background_filter_render_modifier.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS::Rosen::ModifierNG;
namespace OHOS::Rosen {
class RSBackgroundFilterRenderModifierNGTypeTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSBackgroundFilterRenderModifierNGTypeTest::SetUpTestCase() {}
void RSBackgroundFilterRenderModifierNGTypeTest::TearDownTestCase() {}
void RSBackgroundFilterRenderModifierNGTypeTest::SetUp() {}
void RSBackgroundFilterRenderModifierNGTypeTest::TearDown() {}

/**
 * @tc.name: RSBackgroundFilterRenderModifierTest
 * @tc.desc:GetType & ResetProperties
 * @tc.type:FUNC
 */
HWTEST_F(RSBackgroundFilterRenderModifierNGTypeTest, RSBackgroundFilterRenderModifierTest, TestSize.Level1)
{
    RSBackgroundFilterRenderModifier modifier;
    RSProperties properties;
    EXPECT_EQ(modifier.GetType(), ModifierNG::RSModifierType::BACKGROUND_FILTER);
    modifier.ResetProperties(properties);
    EXPECT_EQ(properties.GetSystemBarEffect(), false);
    EXPECT_EQ(properties.GetWaterRippleProgress(), 0.0f);
    EXPECT_EQ(properties.GetWaterRippleParams(), std::nullopt);
    EXPECT_EQ(properties.magnifierPara_, nullptr);
    EXPECT_EQ(properties.GetBackgroundBlurRadius(), 0.0f);
    EXPECT_EQ(properties.GetBackgroundBlurSaturation(), 1.0f);
    EXPECT_EQ(properties.GetBackgroundBlurBrightness(), 1.0f);
    EXPECT_EQ(properties.GetBackgroundBlurMaskColor(), Color());
    EXPECT_EQ(properties.GetBackgroundBlurColorMode(), 0);
    EXPECT_EQ(properties.GetBackgroundBlurRadiusX(), 0.0f);
    EXPECT_EQ(properties.GetBackgroundBlurRadiusY(), 0.0f);
    EXPECT_EQ(properties.GetBgBlurDisableSystemAdaptation(), true);
    EXPECT_EQ(properties.GetGreyCoef(), std::nullopt);
}

/**
 * @tc.name: RSBackgroundFilterRenderModifierTest
 * @tc.desc:AttachRenderFilterProperty
 * @tc.type:FUNC
 */
HWTEST_F(RSBackgroundFilterRenderModifierNGTypeTest, AttachRenderFilterPropertyTest, TestSize.Level1)
{
    RSBackgroundFilterRenderModifier modifier;
    auto property = std::make_shared<RSRenderProperty<std::shared_ptr<RSRenderFilter>>>();

    modifier.AttachRenderFilterProperty(nullptr, ModifierNG::RSPropertyType::HDR_UI_BRIGHTNESS);
    modifier.AttachRenderFilterProperty(nullptr, ModifierNG::RSPropertyType::BACKGROUND_UI_FILTER);
    modifier.AttachRenderFilterProperty(property, ModifierNG::RSPropertyType::BACKGROUND_UI_FILTER);

    RSRenderNode node;
    modifier.target_ = node.weak_from_this();
    modifier.AttachRenderFilterProperty(property, ModifierNG::RSPropertyType::BACKGROUND_UI_FILTER);

    auto property_int = std::make_shared < RSRenderProperty<int>();
    modifier.AttachRenderFilterProperty(property_int, ModifierNG::RSPropertyType::BACKGROUND_UI_FILTER);

    auto rsRenderFilter = std::make_shared<RSRenderFilter>();
    auto rsRenderFilterParaBase = std::make_shared<RSRenderFilterParaBase>(RSUIFilterType::BLUR);
    rsRenderFilter->Insert(RSUIFilterType::BLUR, rsRenderFilterParaBase);
    auto rsRenderProperty = std::make_shared<RSRenderProperty<std::shared_ptr<RSRenderFilter>>>(rsRenderFilter, 0);
    modifier.AttachRenderFilterProperty(property, ModifierNG::RSPropertyType::BACKGROUND_UI_FILTER);
}

/**
 * @tc.name: RSBackgroundFilterRenderModifierTest
 * @tc.desc:DetachRenderFilterProperty
 * @tc.type:FUNC
 */
HWTEST_F(RSBackgroundFilterRenderModifierNGTypeTest, DetachRenderFilterPropertyTest, TestSize.Level1)
{
    RSBackgroundFilterRenderModifier modifier;
    auto property = std::make_shared<RSRenderProperty<std::shared_ptr<RSRenderFilter>>>();

    modifier.DetachRenderFilterProperty(nullptr, ModifierNG::RSPropertyType::HDR_UI_BRIGHTNESS);
    modifier.DetachRenderFilterProperty(nullptr, ModifierNG::RSPropertyType::BACKGROUND_UI_FILTER);
    modifier.DetachRenderFilterProperty(property, ModifierNG::RSPropertyType::BACKGROUND_UI_FILTER);

    RSRenderNode node;
    modifier.target_ = node.weak_from_this();
    modifier.DetachRenderFilterProperty(property, ModifierNG::RSPropertyType::BACKGROUND_UI_FILTER);

    auto property_int = std::make_shared < RSRenderProperty<int>();
    modifier.DetachRenderFilterProperty(property_int, ModifierNG::RSPropertyType::BACKGROUND_UI_FILTER);

    auto rsRenderFilter = std::make_shared<RSRenderFilter>();
    auto rsRenderFilterParaBase = std::make_shared<RSRenderFilterParaBase>(RSUIFilterType::BLUR);
    rsRenderFilter->Insert(RSUIFilterType::BLUR, rsRenderFilterParaBase);
    auto rsRenderProperty = std::make_shared<RSRenderProperty<std::shared_ptr<RSRenderFilter>>>(rsRenderFilter, 0);
    modifier.DetachRenderFilterProperty(property, ModifierNG::RSPropertyType::BACKGROUND_UI_FILTER);
}
} // namespace OHOS::Rosen