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
#include "modifier_ng/appearance/rs_foreground_filter_render_modifier.h"
#include "modifier_ng/rs_modifier_ng_type.h"
#include "pipeline/rs_canvas_render_node.h"
#include "property/rs_properties.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS::Rosen::ModifierNG;
namespace OHOS::Rosen {
class RSForegroundFilterRenderModifierNGTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSForegroundFilterRenderModifierNGTest::SetUpTestCase() {}
void RSForegroundFilterRenderModifierNGTest::TearDownTestCase() {}
void RSForegroundFilterRenderModifierNGTest::SetUp() {}
void RSForegroundFilterRenderModifierNGTest::TearDown() {}

/**
 * @tc.name: RSForegroundFilterRenderModifierTest
 * @tc.desc:GetType & ResetProperties
 * @tc.type:FUNC
 */
HWTEST_F(RSForegroundFilterRenderModifierNGTest, RSForegroundFilterRenderModifierTest, TestSize.Level1)
{
    RSForegroundFilterRenderModifier modifier;
    EXPECT_EQ(modifier.GetType(), ModifierNG::RSModifierType::FOREGROUND_FILTER);
    RSProperties properties;
    modifier.ResetProperties(properties);
    EXPECT_EQ(properties.GetSpherize(), 0.f);
    EXPECT_EQ(properties.GetForegroundEffectRadius(), 0.f);
    EXPECT_EQ(properties.GetMotionBlurPara(), nullptr);
    EXPECT_EQ(properties.GetFlyOutParams(), std::nullopt);
    EXPECT_EQ(properties.GetFlyOutDegree(), 0.f);
    EXPECT_EQ(properties.GetDistortionK(), std::nullopt);
    EXPECT_EQ(properties.GetAttractionFraction(), 0.f);
    EXPECT_EQ(properties.GetAttractionDstPoint(), Vector2f());
}

/**
 * @tc.name: RSForegroundFilterRenderModifierTest
 * @tc.desc:AttachRenderFilterProperty
 * @tc.type:FUNC
 */
HWTEST_F(RSForegroundFilterRenderModifierNGTest, AttachRenderFilterPropertyTest, TestSize.Level1)
{
    RSForegroundFilterRenderModifier modifier;
    auto property = std::make_shared<RSRenderProperty<std::shared_ptr<RSRenderFilter>>>();

    modifier.AttachRenderFilterProperty(nullptr, ModifierNG::RSPropertyType::HDR_UI_BRIGHTNESS);
    modifier.AttachRenderFilterProperty(nullptr, ModifierNG::RSPropertyType::FOREGROUND_UI_FILTER);
    modifier.AttachRenderFilterProperty(property, ModifierNG::RSPropertyType::FOREGROUND_UI_FILTER);

    RSCanvasRenderNode node(0);
    modifier.target_ = node.weak_from_this();
    modifier.AttachRenderFilterProperty(property, ModifierNG::RSPropertyType::FOREGROUND_UI_FILTER);

    auto property_int = std::make_shared<RSRenderProperty<int>>();
    EXPECT_NE(property_int, nullptr);
    modifier.AttachRenderFilterProperty(property_int, ModifierNG::RSPropertyType::FOREGROUND_UI_FILTER);

    auto rsRenderFilter = std::make_shared<RSRenderFilter>();
    auto rsRenderFilterParaBase = std::make_shared<RSRenderFilterParaBase>(RSUIFilterType::BLUR);
    rsRenderFilter->Insert(RSUIFilterType::BLUR, rsRenderFilterParaBase);
    auto rsRenderProperty = std::make_shared<RSRenderProperty<std::shared_ptr<RSRenderFilter>>>(rsRenderFilter, 0);
    modifier.AttachRenderFilterProperty(property, ModifierNG::RSPropertyType::FOREGROUND_UI_FILTER);
}

/**
 * @tc.name: RSForegroundFilterRenderModifierTest
 * @tc.desc:DetachRenderFilterProperty
 * @tc.type:FUNC
 */
HWTEST_F(RSForegroundFilterRenderModifierNGTest, DetachRenderFilterPropertyTest, TestSize.Level1)
{
    RSForegroundFilterRenderModifier modifier;
    auto property = std::make_shared<RSRenderProperty<std::shared_ptr<RSRenderFilter>>>();

    modifier.DetachRenderFilterProperty(nullptr, ModifierNG::RSPropertyType::HDR_UI_BRIGHTNESS);
    modifier.DetachRenderFilterProperty(nullptr, ModifierNG::RSPropertyType::FOREGROUND_UI_FILTER);
    modifier.DetachRenderFilterProperty(property, ModifierNG::RSPropertyType::FOREGROUND_UI_FILTER);

    RSCanvasRenderNode node(0);
    modifier.target_ = node.weak_from_this();
    modifier.DetachRenderFilterProperty(property, ModifierNG::RSPropertyType::FOREGROUND_UI_FILTER);

    auto property_int = std::make_shared<RSRenderProperty<int>>();
    EXPECT_NE(property_int, nullptr);
    modifier.DetachRenderFilterProperty(property_int, ModifierNG::RSPropertyType::FOREGROUND_UI_FILTER);

    auto rsRenderFilter = std::make_shared<RSRenderFilter>();
    auto rsRenderFilterParaBase = std::make_shared<RSRenderFilterParaBase>(RSUIFilterType::BLUR);
    rsRenderFilter->Insert(RSUIFilterType::BLUR, rsRenderFilterParaBase);
    auto rsRenderProperty = std::make_shared<RSRenderProperty<std::shared_ptr<RSRenderFilter>>>(rsRenderFilter, 0);
    modifier.DetachRenderFilterProperty(property, ModifierNG::RSPropertyType::FOREGROUND_UI_FILTER);
}
} // namespace OHOS::Rosen