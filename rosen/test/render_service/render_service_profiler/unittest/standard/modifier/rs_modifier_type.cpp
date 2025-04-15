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
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::INVALID), 0);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::BOUNDS), 1);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::FRAME), 2);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::POSITION_Z), 3);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::POSITION_Z_APPLICABLE_CAMERA3D), 4);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::PIVOT), 5);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::PIVOT_Z), 6);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::QUATERNION), 7);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::ROTATION), 8);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::ROTATION_X), 9);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::ROTATION_Y), 10);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::CAMERA_DISTANCE), 11);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::SCALE), 12);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::SCALE_Z), 13);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::SKEW), 14);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::PERSP), 15);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::TRANSLATE), 16);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::TRANSLATE_Z), 17);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::SUBLAYER_TRANSFORM), 18);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::CORNER_RADIUS), 19);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::ALPHA), 20);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::ALPHA_OFFSCREEN), 21);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::FOREGROUND_COLOR), 22);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::BACKGROUND_COLOR), 23);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::BACKGROUND_SHADER), 24);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::BACKGROUND_SHADER_PROGRESS), 25);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::BG_IMAGE), 26);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::BG_IMAGE_INNER_RECT), 27);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::BG_IMAGE_WIDTH), 28);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::BG_IMAGE_HEIGHT), 29);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::BG_IMAGE_POSITION_X), 30);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::BG_IMAGE_POSITION_Y), 31);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::SURFACE_BG_COLOR), 32);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::BORDER_COLOR), 33);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::BORDER_WIDTH), 34);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::BORDER_STYLE), 35);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::BORDER_DASH_WIDTH), 36);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::BORDER_DASH_GAP), 37);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::FILTER), 38);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::BACKGROUND_FILTER), 39);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::LINEAR_GRADIENT_BLUR_PARA), 40);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::DYNAMIC_LIGHT_UP_RATE), 41);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::DYNAMIC_LIGHT_UP_DEGREE), 42);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::FG_BRIGHTNESS_RATES), 43);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::FG_BRIGHTNESS_SATURATION), 44);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::FG_BRIGHTNESS_POSCOEFF), 45);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::FG_BRIGHTNESS_NEGCOEFF), 46);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::FG_BRIGHTNESS_FRACTION), 47);
}

/*
 * @tc.name: FixSurfaceNodeCommandTest002
 * @tc.desc: Fix enum class to prevent undetectable changes
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSModifierTypeTest, FixSurfaceNodeCommandTest002, Level1 | Standard)
{
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::BG_BRIGHTNESS_RATES), 48);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::BG_BRIGHTNESS_SATURATION), 49);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::BG_BRIGHTNESS_POSCOEFF), 50);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::BG_BRIGHTNESS_NEGCOEFF), 51);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::BG_BRIGHTNESS_FRACTION), 52);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::FRAME_GRAVITY), 53);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::CLIP_RRECT), 54);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::CLIP_BOUNDS), 55);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::CLIP_TO_BOUNDS), 56);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::CLIP_TO_FRAME), 57);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::VISIBLE), 58);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::SHADOW_COLOR), 59);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::SHADOW_OFFSET_X), 60);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::SHADOW_OFFSET_Y), 61);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::SHADOW_ALPHA), 62);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::SHADOW_ELEVATION), 63);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::SHADOW_RADIUS), 64);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::SHADOW_PATH), 65);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::SHADOW_MASK), 66);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::SHADOW_COLOR_STRATEGY), 67);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::MASK), 68);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::SPHERIZE), 69);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::LIGHT_UP_EFFECT), 70);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::PIXEL_STRETCH), 71);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::PIXEL_STRETCH_PERCENT), 72);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::PIXEL_STRETCH_TILE_MODE), 73);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::USE_EFFECT), 74);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::USE_EFFECT_TYPE), 75);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::COLOR_BLEND_MODE), 76);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::COLOR_BLEND_APPLY_TYPE), 77);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::SANDBOX), 78);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::GRAY_SCALE), 79);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::BRIGHTNESS), 80);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::CONTRAST), 81);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::SATURATE), 82);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::SEPIA), 83);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::INVERT), 84);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::AIINVERT), 85);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::SYSTEMBAREFFECT), 86);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::WATER_RIPPLE_PROGRESS), 87);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::WATER_RIPPLE_PARAMS), 88);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::HUE_ROTATE), 89);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::COLOR_BLEND), 90);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::PARTICLE), 91);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::SHADOW_IS_FILLED), 92);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::OUTLINE_COLOR), 93);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::OUTLINE_WIDTH), 94);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::OUTLINE_STYLE), 95);
}

/*
 * @tc.name: FixSurfaceNodeCommandTest003
 * @tc.desc: Fix enum class to prevent undetectable changes
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSModifierTypeTest, FixSurfaceNodeCommandTest003, Level1 | Standard)
{
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::OUTLINE_DASH_WIDTH), 96);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::OUTLINE_DASH_GAP), 97);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::OUTLINE_RADIUS), 98);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::GREY_COEF), 99);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::LIGHT_INTENSITY), 100);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::LIGHT_COLOR), 101);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::LIGHT_POSITION), 102);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::ILLUMINATED_BORDER_WIDTH), 103);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::ILLUMINATED_TYPE), 104);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::BLOOM), 105);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::FOREGROUND_EFFECT_RADIUS), 106);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::USE_SHADOW_BATCHING), 107);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::MOTION_BLUR_PARA), 108);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::PARTICLE_EMITTER_UPDATER), 109);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::PARTICLE_NOISE_FIELD), 110);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::FLY_OUT_DEGREE), 111);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::FLY_OUT_PARAMS), 112);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::DISTORTION_K), 113);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::DYNAMIC_DIM_DEGREE), 114);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::MAGNIFIER_PARA), 115);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::BACKGROUND_BLUR_RADIUS), 116);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::BACKGROUND_BLUR_SATURATION), 117);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::BACKGROUND_BLUR_BRIGHTNESS), 118);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::BACKGROUND_BLUR_MASK_COLOR), 119);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::BACKGROUND_BLUR_COLOR_MODE), 120);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::BACKGROUND_BLUR_RADIUS_X), 121);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::BACKGROUND_BLUR_RADIUS_Y), 122);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::BG_BLUR_DISABLE_SYSTEM_ADAPTATION), 123);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::FOREGROUND_BLUR_RADIUS), 124);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::FOREGROUND_BLUR_SATURATION), 125);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::FOREGROUND_BLUR_BRIGHTNESS), 126);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::FOREGROUND_BLUR_MASK_COLOR), 127);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::FOREGROUND_BLUR_COLOR_MODE), 128);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::FOREGROUND_BLUR_RADIUS_X), 129);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::FOREGROUND_BLUR_RADIUS_Y), 130);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::FG_BLUR_DISABLE_SYSTEM_ADAPTATION), 131);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::ATTRACTION_FRACTION), 132);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::ATTRACTION_DSTPOINT), 133);
}

/*
 * @tc.name: FixSurfaceNodeCommandTest004
 * @tc.desc: Fix enum class to prevent undetectable changes
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSModifierTypeTest, FixSurfaceNodeCommandTest004, Level1 | Standard)
{
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::CUSTOM), 200);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::EXTENDED), 201);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::TRANSITION), 202);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::BACKGROUND_STYLE), 203);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::CONTENT_STYLE), 204);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::FOREGROUND_STYLE), 205);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::OVERLAY_STYLE), 206);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::NODE_MODIFIER), 207);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::ENV_FOREGROUND_COLOR), 208);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::ENV_FOREGROUND_COLOR_STRATEGY), 209);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::GEOMETRYTRANS), 210);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::CUSTOM_CLIP_TO_FRAME), 211);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::HDR_BRIGHTNESS), 212);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::BEHIND_WINDOW_FILTER_RADIUS), 213);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::BEHIND_WINDOW_FILTER_SATURATION), 214);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::BEHIND_WINDOW_FILTER_BRIGHTNESS), 215);
    EXPECT_EQ(static_cast<int16_t>(RSModifierType::BEHIND_WINDOW_FILTER_MASK_COLOR), 216);

    EXPECT_EQ(static_cast<int16_t>(RSModifierType::CHILDREN), 240);

    EXPECT_EQ(static_cast<int16_t>(RSModifierType::MAX_RS_MODIFIER_TYPE), 255);
}
} // namespace OHOS::Rosen