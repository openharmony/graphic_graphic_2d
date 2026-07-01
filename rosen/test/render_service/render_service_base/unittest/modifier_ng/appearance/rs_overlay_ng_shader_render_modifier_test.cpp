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

#include "effect/rs_render_shader_base.h"
#include "modifier_ng/rs_modifier_ng_type.h"
#include "pipeline/rs_canvas_drawing_render_node.h"
#include "property/rs_properties.h"
#include "modifier_ng/appearance/rs_overlay_ng_shader_render_modifier.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS::Rosen::ModifierNG;

namespace OHOS::Rosen {
class RSOverlayNGShaderRenderModifierNGTypeTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSOverlayNGShaderRenderModifierNGTypeTest::SetUpTestCase() {}
void RSOverlayNGShaderRenderModifierNGTypeTest::TearDownTestCase() {}
void RSOverlayNGShaderRenderModifierNGTypeTest::SetUp() {}
void RSOverlayNGShaderRenderModifierNGTypeTest::TearDown() {}

/**
 * @tc.name: RSOverlayNGShaderRenderModifierTest001
 * @tc.desc: GetType test
 * @tc.type: FUNC
 */
HWTEST_F(RSOverlayNGShaderRenderModifierNGTypeTest, RSOverlayNGShaderRenderModifierTest001, TestSize.Level1)
{
    RSOverlayNGShaderRenderModifier modifier;
    EXPECT_EQ(modifier.GetType(), ModifierNG::RSModifierType::OVERLAY_NG_SHADER);
}

/**
 * @tc.name: RSOverlayNGShaderRenderModifierTest002
 * @tc.desc: ResetProperties test
 * @tc.type: FUNC
 */
HWTEST_F(RSOverlayNGShaderRenderModifierNGTypeTest, RSOverlayNGShaderRenderModifierTest002, TestSize.Level1)
{
    RSOverlayNGShaderRenderModifier modifier;
    RSProperties properties;
    
    modifier.ResetProperties(properties);
    EXPECT_EQ(properties.GetOverlayNGShader(), nullptr);
}

/**
 * @tc.name: RSOverlayNGShaderRenderModifierTest003
 * @tc.desc: ResetProperties test with existing shader
 * @tc.type: FUNC
 */
HWTEST_F(RSOverlayNGShaderRenderModifierNGTypeTest, RSOverlayNGShaderRenderModifierTest003, TestSize.Level1)
{
    RSOverlayNGShaderRenderModifier modifier;
    RSProperties properties;
    
    auto shader = RSNGRenderShaderBase::Create(RSNGEffectType::AURORA_NOISE);
    properties.SetOverlayNGShader(shader);
    EXPECT_NE(properties.GetOverlayNGShader(), nullptr);
    
    modifier.ResetProperties(properties);
    EXPECT_EQ(properties.GetOverlayNGShader(), nullptr);
}

/**
 * @tc.name: RSOverlayNGShaderRenderModifierTest004
 * @tc.desc: Property applier mapping test
 * @tc.type: FUNC
 */
HWTEST_F(RSOverlayNGShaderRenderModifierNGTypeTest, RSOverlayNGShaderRenderModifierTest004, TestSize.Level1)
{
    const auto& map = RSOverlayNGShaderRenderModifier::LegacyPropertyApplierMap_;
    auto it = map.find(ModifierNG::RSPropertyType::OVERLAY_NG_SHADER);
    EXPECT_NE(it, map.end());
}
} // namespace OHOS::Rosen