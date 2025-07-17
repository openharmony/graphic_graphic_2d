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

#include "gtest/gtest.h"

#include "modifier/rs_modifier_type.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSModifierTypeTest : public testing::Test {
public:
    static void SetUpTestCase() {};
    static void TearDownTestCase() {};
    void SetUp() override {};
    void TearDown() override {};
};

/*
 * @tc.name: FixSurfaceNodeCommandTest001
 * @tc.desc: Fix enum class to prevent undetectable changes
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSModifierTypeTest, FixSurfaceNodeCommandTest001, Level1 | Standard)
{
    auto value = 0;
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::INVALID), value++);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::BOUNDS), value++);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::FRAME), value++);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::POSITION_Z), value++);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::POSITION_Z_APPLICABLE_CAMERA3D), value++);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::PIVOT), value++);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::PIVOT_Z), value++);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::QUATERNION), value++);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::ROTATION), value++);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::ROTATION_X), value++);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::ROTATION_Y), value++);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::CAMERA_DISTANCE), value++);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::SCALE), value++);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::SCALE_Z), value++);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::SKEW), value++);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::PERSP), value++);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::TRANSLATE), value++);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::TRANSLATE_Z), value++);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::SUBLAYER_TRANSFORM), value++);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::CORNER_RADIUS), value++);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::ALPHA), value++);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::ALPHA_OFFSCREEN), value++);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::FOREGROUND_COLOR), value++);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::BACKGROUND_COLOR), value++);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::BACKGROUND_SHADER), value++);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::BACKGROUND_SHADER_PROGRESS), value++);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::BG_IMAGE), value++);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::BG_IMAGE_INNER_RECT), value++);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::BG_IMAGE_WIDTH), value++);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::BG_IMAGE_HEIGHT), value++);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::BG_IMAGE_POSITION_X), value++);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::BG_IMAGE_POSITION_Y), value++);
    value++;
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::BORDER_COLOR), value++);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::BORDER_WIDTH), value++);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::BORDER_STYLE), value++);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::BORDER_DASH_WIDTH), value++);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::BORDER_DASH_GAP), value++);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::FILTER), value++);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::BACKGROUND_FILTER), value++);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::LINEAR_GRADIENT_BLUR_PARA), value++);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::DYNAMIC_LIGHT_UP_RATE), value++);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::DYNAMIC_LIGHT_UP_DEGREE), value++);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::FG_BRIGHTNESS_RATES), value++);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::FG_BRIGHTNESS_SATURATION), value++);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::FG_BRIGHTNESS_POSCOEFF), value++);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::FG_BRIGHTNESS_NEGCOEFF), value++);
}

/*
 * @tc.name: FixSurfaceNodeCommandTest002
 * @tc.desc: Fix enum class to prevent undetectable changes
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSModifierTypeTest, FixSurfaceNodeCommandTest002, Level1 | Standard)
{
    auto value = 47;
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::FG_BRIGHTNESS_FRACTION), value++);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::BG_BRIGHTNESS_RATES), value++);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::BG_BRIGHTNESS_SATURATION), value++);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::BG_BRIGHTNESS_POSCOEFF), value++);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::BG_BRIGHTNESS_NEGCOEFF), value++);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::BG_BRIGHTNESS_FRACTION), value++);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::FRAME_GRAVITY), value++);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::CLIP_RRECT), value++);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::CLIP_BOUNDS), value++);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::CLIP_TO_BOUNDS), value++);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::CLIP_TO_FRAME), value++);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::VISIBLE), value++);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::SHADOW_COLOR), value++);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::SHADOW_OFFSET_X), value++);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::SHADOW_OFFSET_Y), value++);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::SHADOW_ALPHA), value++);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::SHADOW_ELEVATION), value++);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::SHADOW_RADIUS), value++);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::SHADOW_PATH), value++);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::SHADOW_MASK), value++);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::SHADOW_COLOR_STRATEGY), value++);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::MASK), value++);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::SPHERIZE), value++);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::LIGHT_UP_EFFECT), value++);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::PIXEL_STRETCH), value++);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::PIXEL_STRETCH_PERCENT), value++);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::PIXEL_STRETCH_TILE_MODE), value++);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::USE_EFFECT), value++);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::USE_EFFECT_TYPE), value++);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::COLOR_BLEND_MODE), value++);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::COLOR_BLEND_APPLY_TYPE), value++);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::SANDBOX), value++);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::GRAY_SCALE), value++);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::BRIGHTNESS), value++);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::CONTRAST), value++);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::SATURATE), value++);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::SEPIA), value++);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::INVERT), value++);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::AIINVERT), value++);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::SYSTEMBAREFFECT), value++);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::WATER_RIPPLE_PROGRESS), value++);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::WATER_RIPPLE_PARAMS), value++);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::HUE_ROTATE), value++);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::COLOR_BLEND), value++);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::PARTICLE), value++);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::SHADOW_IS_FILLED), value++);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::OUTLINE_COLOR), value++);
}

/*
 * @tc.name: FixSurfaceNodeCommandTest003
 * @tc.desc: Fix enum class to prevent undetectable changes
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSModifierTypeTest, FixSurfaceNodeCommandTest003, Level1 | Standard)
{
    auto value = 94;
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::OUTLINE_WIDTH), value++);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::OUTLINE_STYLE), value++);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::OUTLINE_DASH_WIDTH), value++);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::OUTLINE_DASH_GAP), value++);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::OUTLINE_RADIUS), value++);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::GREY_COEF), value++);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::LIGHT_INTENSITY), value++);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::LIGHT_COLOR), value++);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::LIGHT_POSITION), value++);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::ILLUMINATED_BORDER_WIDTH), value++);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::ILLUMINATED_TYPE), value++);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::BLOOM), value++);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::FOREGROUND_EFFECT_RADIUS), value++);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::USE_SHADOW_BATCHING), value++);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::MOTION_BLUR_PARA), value++);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::PARTICLE_EMITTER_UPDATER), value++);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::PARTICLE_NOISE_FIELD), value++);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::FLY_OUT_DEGREE), value++);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::FLY_OUT_PARAMS), value++);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::DISTORTION_K), value++);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::DYNAMIC_DIM_DEGREE), value++);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::MAGNIFIER_PARA), value++);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::BACKGROUND_BLUR_RADIUS), value++);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::BACKGROUND_BLUR_SATURATION), value++);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::BACKGROUND_BLUR_BRIGHTNESS), value++);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::BACKGROUND_BLUR_MASK_COLOR), value++);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::BACKGROUND_BLUR_COLOR_MODE), value++);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::BACKGROUND_BLUR_RADIUS_X), value++);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::BACKGROUND_BLUR_RADIUS_Y), value++);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::BG_BLUR_DISABLE_SYSTEM_ADAPTATION), value++);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::FOREGROUND_BLUR_RADIUS), value++);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::FOREGROUND_BLUR_SATURATION), value++);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::FOREGROUND_BLUR_BRIGHTNESS), value++);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::FOREGROUND_BLUR_MASK_COLOR), value++);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::FOREGROUND_BLUR_COLOR_MODE), value++);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::FOREGROUND_BLUR_RADIUS_X), value++);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::FOREGROUND_BLUR_RADIUS_Y), value++);
}

/*
 * @tc.name: FixSurfaceNodeCommandTest004
 * @tc.desc: Fix enum class to prevent undetectable changes
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSModifierTypeTest, FixSurfaceNodeCommandTest004, Level1 | Standard)
{
    auto value = 131;
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::FG_BLUR_DISABLE_SYSTEM_ADAPTATION), value++);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::ATTRACTION_FRACTION), value++);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::ATTRACTION_DSTPOINT), value++);

    value = 200;
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::CUSTOM), value++);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::EXTENDED), value++);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::TRANSITION), value++);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::BACKGROUND_STYLE), value++);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::CONTENT_STYLE), value++);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::FOREGROUND_STYLE), value++);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::OVERLAY_STYLE), value++);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::NODE_MODIFIER), value++);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::ENV_FOREGROUND_COLOR), value++);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::ENV_FOREGROUND_COLOR_STRATEGY), value++);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::GEOMETRYTRANS), value++);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::CUSTOM_CLIP_TO_FRAME), value++);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::HDR_BRIGHTNESS), value++);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::BEHIND_WINDOW_FILTER_RADIUS), value++);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::BEHIND_WINDOW_FILTER_SATURATION), value++);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::BEHIND_WINDOW_FILTER_BRIGHTNESS), value++);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::BEHIND_WINDOW_FILTER_MASK_COLOR), value++);

    EXPECT_EQ(static_cast<int16_t>(RSModifierType::CHILDREN), 240);

    EXPECT_EQ(static_cast<int16_t>(RSModifierType::MAX_RS_MODIFIER_TYPE), 255);
}
} // namespace OHOS::Rosen