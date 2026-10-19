/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include "effect/include/glass_effect_para.h"
#include "effect/include/visual_effect.h"
#include "effect/include/visual_effect_para.h"
#include "mask/include/ripple_mask_para.h"
#include "pixel_map.h"

namespace OHOS {
namespace Rosen {

using namespace testing;
using namespace testing::ext;

class RSGlassEffectParaTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSGlassEffectParaTest::SetUpTestCase() {}
void RSGlassEffectParaTest::TearDownTestCase() {}
void RSGlassEffectParaTest::SetUp() {}
void RSGlassEffectParaTest::TearDown() {}

/**
 * @tc.name: GlassEffectParaType001
 * @tc.desc: Verify GlassEffectPara type is GLASS_EFFECT
 * @tc.type: FUNC
 */
HWTEST_F(RSGlassEffectParaTest, GlassEffectParaType001, TestSize.Level1)
{
    auto para = std::make_shared<GlassEffectPara>();
    ASSERT_NE(para, nullptr);
    EXPECT_EQ(para->type_, VisualEffectPara::ParaType::GLASS_EFFECT);
}

/**
 * @tc.name: GlassEffectParaDefaultValues001
 * @tc.desc: Verify GlassEffectPara default values
 * @tc.type: FUNC
 */
HWTEST_F(RSGlassEffectParaTest, GlassEffectParaDefaultValues001, TestSize.Level1)
{
    auto para = std::make_shared<GlassEffectPara>();
    ASSERT_NE(para, nullptr);
    EXPECT_FLOAT_EQ(para->GetSphereCenter().x_, 0.5f);
    EXPECT_FLOAT_EQ(para->GetSphereCenter().y_, 0.5f);
    EXPECT_FLOAT_EQ(para->GetSphereRadius(), 0.18f);
    EXPECT_FLOAT_EQ(para->GetOpacity(), 0.0f);
    EXPECT_FLOAT_EQ(para->GetShapeScale(), 0.0f);
    EXPECT_FLOAT_EQ(para->GetShadowOffset(), 0.0f);
    EXPECT_FLOAT_EQ(para->GetShadowRadius(), 0.0f);
    EXPECT_FLOAT_EQ(para->GetShadowEdgeSoftness(), 0.0f);
    EXPECT_FLOAT_EQ(para->GetShadowOpacity(), 0.0f);
    EXPECT_FLOAT_EQ(para->GetCausticOffset(), 0.0f);
    EXPECT_FLOAT_EQ(para->GetCausticRadius(), 0.0f);
    EXPECT_FLOAT_EQ(para->GetCausticEdgeSoftness(), 0.0f);
    EXPECT_FLOAT_EQ(para->GetCausticOpacity(), 0.0f);
    EXPECT_FLOAT_EQ(para->GetContentScale(), 0.0f);
    EXPECT_FLOAT_EQ(para->GetContentSaturation(), 0.0f);
    EXPECT_FLOAT_EQ(para->GetContentDispersion(), 0.0f);
    EXPECT_EQ(para->GetShapeMask(), nullptr);
    EXPECT_EQ(para->GetContentMask(), nullptr);
    EXPECT_EQ(para->GetReflectionImage(), nullptr);
}

/**
 * @tc.name: GlassEffectParaSetterGetter001
 * @tc.desc: Verify GlassEffectPara setters and getters
 * @tc.type: FUNC
 */
HWTEST_F(RSGlassEffectParaTest, GlassEffectParaSetterGetter001, TestSize.Level1)
{
    auto para = std::make_shared<GlassEffectPara>();
    ASSERT_NE(para, nullptr);

    Vector2f center(0.1f, 0.2f);
    para->SetSphereCenter(center);
    EXPECT_FLOAT_EQ(para->GetSphereCenter().x_, center.x_);
    EXPECT_FLOAT_EQ(para->GetSphereCenter().y_, center.y_);

    para->SetSphereRadius(0.25f);
    EXPECT_FLOAT_EQ(para->GetSphereRadius(), 0.25f);

    Vector4f bgColor(0.1f, 0.2f, 0.3f, 0.4f);
    para->SetAverageBgColor(bgColor);
    EXPECT_FLOAT_EQ(para->GetAverageBgColor().x_, bgColor.x_);
    EXPECT_FLOAT_EQ(para->GetAverageBgColor().y_, bgColor.y_);
    EXPECT_FLOAT_EQ(para->GetAverageBgColor().z_, bgColor.z_);
    EXPECT_FLOAT_EQ(para->GetAverageBgColor().w_, bgColor.w_);

    para->SetOpacity(0.5f);
    EXPECT_FLOAT_EQ(para->GetOpacity(), 0.5f);

    para->SetShapeScale(1.0f);
    EXPECT_FLOAT_EQ(para->GetShapeScale(), 1.0f);

    para->SetShadowOffset(2.0f);
    EXPECT_FLOAT_EQ(para->GetShadowOffset(), 2.0f);

    para->SetShadowRadius(3.0f);
    EXPECT_FLOAT_EQ(para->GetShadowRadius(), 3.0f);

    para->SetShadowEdgeSoftness(0.5f);
    EXPECT_FLOAT_EQ(para->GetShadowEdgeSoftness(), 0.5f);

    para->SetShadowOpacity(0.7f);
    EXPECT_FLOAT_EQ(para->GetShadowOpacity(), 0.7f);

    para->SetCausticOffset(1.5f);
    EXPECT_FLOAT_EQ(para->GetCausticOffset(), 1.5f);

    para->SetCausticRadius(2.5f);
    EXPECT_FLOAT_EQ(para->GetCausticRadius(), 2.5f);

    para->SetCausticEdgeSoftness(0.3f);
    EXPECT_FLOAT_EQ(para->GetCausticEdgeSoftness(), 0.3f);

    para->SetCausticOpacity(0.8f);
    EXPECT_FLOAT_EQ(para->GetCausticOpacity(), 0.8f);

    Vector4f tintColor(0.2f, 0.4f, 0.6f, 0.8f);
    para->SetContentTintColor(tintColor);
    EXPECT_FLOAT_EQ(para->GetContentTintColor().x_, tintColor.x_);
    EXPECT_FLOAT_EQ(para->GetContentTintColor().y_, tintColor.y_);
    EXPECT_FLOAT_EQ(para->GetContentTintColor().z_, tintColor.z_);
    EXPECT_FLOAT_EQ(para->GetContentTintColor().w_, tintColor.w_);

    para->SetContentScale(1.2f);
    EXPECT_FLOAT_EQ(para->GetContentScale(), 1.2f);

    para->SetContentSaturation(0.9f);
    EXPECT_FLOAT_EQ(para->GetContentSaturation(), 0.9f);

    para->SetContentDispersion(0.4f);
    EXPECT_FLOAT_EQ(para->GetContentDispersion(), 0.4f);
}

/**
 * @tc.name: GlassEffectParaMaskParams001
 * @tc.desc: Verify GlassEffectPara mask and reflection image setters
 * @tc.type: FUNC
 */
HWTEST_F(RSGlassEffectParaTest, GlassEffectParaMaskParams001, TestSize.Level1)
{
    auto para = std::make_shared<GlassEffectPara>();
    ASSERT_NE(para, nullptr);

    auto shapeMask = std::make_shared<RippleMaskPara>();
    para->SetShapeMask(shapeMask);
    EXPECT_EQ(para->GetShapeMask(), shapeMask);

    auto contentMask = std::make_shared<RippleMaskPara>();
    para->SetContentMask(contentMask);
    EXPECT_EQ(para->GetContentMask(), contentMask);

    Media::InitializationOptions opts;
    opts.size.width = 16;
    opts.size.height = 16;
    auto pixelMap = std::shared_ptr<Media::PixelMap>(Media::PixelMap::Create(opts));
    ASSERT_NE(pixelMap, nullptr);
    para->SetReflectionImage(pixelMap);
    EXPECT_EQ(para->GetReflectionImage(), pixelMap);
}

/**
 * @tc.name: VisualEffectAddGlassPara001
 * @tc.desc: Verify VisualEffect accepts GlassEffectPara
 * @tc.type: FUNC
 */
HWTEST_F(RSGlassEffectParaTest, VisualEffectAddGlassPara001, TestSize.Level1)
{
    auto para = std::make_shared<GlassEffectPara>();
    ASSERT_NE(para, nullptr);
    auto visualEffect = std::make_shared<VisualEffect>();
    ASSERT_NE(visualEffect, nullptr);
    visualEffect->AddPara(para);
    SUCCEED();
}
} // namespace Rosen
} // namespace OHOS
