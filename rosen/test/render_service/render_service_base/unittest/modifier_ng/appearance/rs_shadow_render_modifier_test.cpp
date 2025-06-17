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
#include "modifier_ng/appearance/rs_shadow_render_modifier.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS::Rosen::ModifierNG;
namespace OHOS::Rosen {
class RSShadowRenderModifierNGTypeTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSShadowRenderModifierNGTypeTest::SetUpTestCase() {}
void RSShadowRenderModifierNGTypeTest::TearDownTestCase() {}
void RSShadowRenderModifierNGTypeTest::SetUp() {}
void RSShadowRenderModifierNGTypeTest::TearDown() {}

/**
 * @tc.name: RSShadowRenderModifierTest
 * @tc.desc: GetType & ResetProperties
 * @tc.type: FUNC
 */
HWTEST_F(RSShadowRenderModifierNGTypeTest, RSShadowRenderModifierTest, TestSize.Level1)
{
    RSShadowRenderModifier modifier;
    RSProperties properties;
    EXPECT_EQ(modifier.GetType(), ModifierNG::RSModifierType::SHADOW);
    modifier.ResetProperties(properties);
    EXPECT_EQ(properties.GetShadowColor(), Color());
    EXPECT_EQ(properties.GetShadowOffsetX(), 0.0f);
    EXPECT_EQ(properties.GetShadowOffsetY(), 0.0f);
    EXPECT_EQ(properties.GetShadowAlpha(), 0.0f);
    EXPECT_EQ(properties.GetShadowElevation(), 0.0f);
    EXPECT_EQ(properties.GetShadowRadius(), 0.0f);
    EXPECT_EQ(properties.GetShadowPath(), nullptr);
    EXPECT_EQ(properties.GetShadowMask(), false);
    EXPECT_EQ(properties.GetShadowColorStrategy(), static_cast<int>(SHADOW_COLOR_STRATEGY::COLOR_STRATEGY_NONE));
    EXPECT_EQ(properties.GetShadowIsFilled(), false);
    EXPECT_EQ(properties.GetUseShadowBatching(), false);
}
} // namespace OHOS::Rosen