/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, Hardware
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <gtest/gtest.h>

#include "common/rs_obj_abs_geometry.h"
#include "pipeline/rs_paint_filter_canvas.h"
#include "pipeline/rs_render_content.h"
#include "property/rs_property_drawable_bounds_geometry.h"
#include "render/rs_shadow.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class RSClipBoundsDrawableTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSClipBoundsDrawableTest::SetUpTestCase() {}
void RSClipBoundsDrawableTest::TearDownTestCase() {}
void RSClipBoundsDrawableTest::SetUp() {}
void RSClipBoundsDrawableTest::TearDown() {}

class RSPointLightDrawableTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSPointLightDrawableTest::SetUpTestCase() {}
void RSPointLightDrawableTest::TearDownTestCase() {}
void RSPointLightDrawableTest::SetUp() {}
void RSPointLightDrawableTest::TearDown() {}

class RSBorderDrawableTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSBorderDrawableTest::SetUpTestCase() {}
void RSBorderDrawableTest::TearDownTestCase() {}
void RSBorderDrawableTest::SetUp() {}
void RSBorderDrawableTest::TearDown() {}

class RSBorderDRRectDrawableTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSBorderDRRectDrawableTest::SetUpTestCase() {}
void RSBorderDRRectDrawableTest::TearDownTestCase() {}
void RSBorderDRRectDrawableTest::SetUp() {}
void RSBorderDRRectDrawableTest::TearDown() {}

class RSBorderFourLineDrawableTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSBorderFourLineDrawableTest::SetUpTestCase() {}
void RSBorderFourLineDrawableTest::TearDownTestCase() {}
void RSBorderFourLineDrawableTest::SetUp() {}
void RSBorderFourLineDrawableTest::TearDown() {}

class RSBorderFourLineRoundCornerDrawableTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSBorderFourLineRoundCornerDrawableTest::SetUpTestCase() {}
void RSBorderFourLineRoundCornerDrawableTest::TearDownTestCase() {}
void RSBorderFourLineRoundCornerDrawableTest::SetUp() {}
void RSBorderFourLineRoundCornerDrawableTest::TearDown() {}

class RSOutlineDrawableTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSOutlineDrawableTest::SetUpTestCase() {}
void RSOutlineDrawableTest::TearDownTestCase() {}
void RSOutlineDrawableTest::SetUp() {}
void RSOutlineDrawableTest::TearDown() {}

class RSMaskDrawableTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSMaskDrawableTest::SetUpTestCase() {}
void RSMaskDrawableTest::TearDownTestCase() {}
void RSMaskDrawableTest::SetUp() {}
void RSMaskDrawableTest::TearDown() {}

class RSShadowBaseDrawableTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSShadowBaseDrawableTest::SetUpTestCase() {}
void RSShadowBaseDrawableTest::TearDownTestCase() {}
void RSShadowBaseDrawableTest::SetUp() {}
void RSShadowBaseDrawableTest::TearDown() {}

class RSShadowDrawableTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSShadowDrawableTest::SetUpTestCase() {}
void RSShadowDrawableTest::TearDownTestCase() {}
void RSShadowDrawableTest::SetUp() {}
void RSShadowDrawableTest::TearDown() {}

class RSHardwareAccelerationShadowDrawableTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSHardwareAccelerationShadowDrawableTest::SetUpTestCase() {}
void RSHardwareAccelerationShadowDrawableTest::TearDownTestCase() {}
void RSHardwareAccelerationShadowDrawableTest::SetUp() {}
void RSHardwareAccelerationShadowDrawableTest::TearDown() {}

class RSColorfulShadowDrawableTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSColorfulShadowDrawableTest::SetUpTestCase() {}
void RSColorfulShadowDrawableTest::TearDownTestCase() {}
void RSColorfulShadowDrawableTest::SetUp() {}
void RSColorfulShadowDrawableTest::TearDown() {}

class RSDynamicLightUpDrawableTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSDynamicLightUpDrawableTest::SetUpTestCase() {}
void RSDynamicLightUpDrawableTest::TearDownTestCase() {}
void RSDynamicLightUpDrawableTest::SetUp() {}
void RSDynamicLightUpDrawableTest::TearDown() {}

class RSLightUpEffectDrawableTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSLightUpEffectDrawableTest::SetUpTestCase() {}
void RSLightUpEffectDrawableTest::TearDownTestCase() {}
void RSLightUpEffectDrawableTest::SetUp() {}
void RSLightUpEffectDrawableTest::TearDown() {}

class RSDynamicDimDrawableTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSDynamicDimDrawableTest::SetUpTestCase() {}
void RSDynamicDimDrawableTest::TearDownTestCase() {}
void RSDynamicDimDrawableTest::SetUp() {}
void RSDynamicDimDrawableTest::TearDown() {}

class RSBinarizationDrawableTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSBinarizationDrawableTest::SetUpTestCase() {}
void RSBinarizationDrawableTest::TearDownTestCase() {}
void RSBinarizationDrawableTest::SetUp() {}
void RSBinarizationDrawableTest::TearDown() {}

class RSBackgroundFilterDrawableTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSBackgroundFilterDrawableTest::SetUpTestCase() {}
void RSBackgroundFilterDrawableTest::TearDownTestCase() {}
void RSBackgroundFilterDrawableTest::SetUp() {}
void RSBackgroundFilterDrawableTest::TearDown() {}

class RSCompositingFilterDrawableTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSCompositingFilterDrawableTest::SetUpTestCase() {}
void RSCompositingFilterDrawableTest::TearDownTestCase() {}
void RSCompositingFilterDrawableTest::SetUp() {}
void RSCompositingFilterDrawableTest::TearDown() {}

class RSForegroundFilterDrawableTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSForegroundFilterDrawableTest::SetUpTestCase() {}
void RSForegroundFilterDrawableTest::TearDownTestCase() {}
void RSForegroundFilterDrawableTest::SetUp() {}
void RSForegroundFilterDrawableTest::TearDown() {}

class RSForegroundFilterRestoreDrawableTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSForegroundFilterRestoreDrawableTest::SetUpTestCase() {}
void RSForegroundFilterRestoreDrawableTest::TearDownTestCase() {}
void RSForegroundFilterRestoreDrawableTest::SetUp() {}
void RSForegroundFilterRestoreDrawableTest::TearDown() {}

class RSEffectDataGenerateDrawableTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSEffectDataGenerateDrawableTest::SetUpTestCase() {}
void RSEffectDataGenerateDrawableTest::TearDownTestCase() {}
void RSEffectDataGenerateDrawableTest::SetUp() {}
void RSEffectDataGenerateDrawableTest::TearDown() {}

class RSEffectDataApplyDrawableTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSEffectDataApplyDrawableTest::SetUpTestCase() {}
void RSEffectDataApplyDrawableTest::TearDownTestCase() {}
void RSEffectDataApplyDrawableTest::SetUp() {}
void RSEffectDataApplyDrawableTest::TearDown() {}

class RSForegroundColorDrawableTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSForegroundColorDrawableTest::SetUpTestCase() {}
void RSForegroundColorDrawableTest::TearDownTestCase() {}
void RSForegroundColorDrawableTest::SetUp() {}
void RSForegroundColorDrawableTest::TearDown() {}

class RSParticleDrawableTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSParticleDrawableTest::SetUpTestCase() {}
void RSParticleDrawableTest::TearDownTestCase() {}
void RSParticleDrawableTest::SetUp() {}
void RSParticleDrawableTest::TearDown() {}

class RSPixelStretchDrawableTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSPixelStretchDrawableTest::SetUpTestCase() {}
void RSPixelStretchDrawableTest::TearDownTestCase() {}
void RSPixelStretchDrawableTest::SetUp() {}
void RSPixelStretchDrawableTest::TearDown() {}

class RSBackgroundDrawableTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSBackgroundDrawableTest::SetUpTestCase() {}
void RSBackgroundDrawableTest::TearDownTestCase() {}
void RSBackgroundDrawableTest::SetUp() {}
void RSBackgroundDrawableTest::TearDown() {}

class RSBackgroundColorDrawableTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSBackgroundColorDrawableTest::SetUpTestCase() {}
void RSBackgroundColorDrawableTest::TearDownTestCase() {}
void RSBackgroundColorDrawableTest::SetUp() {}
void RSBackgroundColorDrawableTest::TearDown() {}

class RSBackgroundShaderDrawableTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSBackgroundShaderDrawableTest::SetUpTestCase() {}
void RSBackgroundShaderDrawableTest::TearDownTestCase() {}
void RSBackgroundShaderDrawableTest::SetUp() {}
void RSBackgroundShaderDrawableTest::TearDown() {}

class RSBackgroundImageDrawableTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSBackgroundImageDrawableTest::SetUpTestCase() {}
void RSBackgroundImageDrawableTest::TearDownTestCase() {}
void RSBackgroundImageDrawableTest::SetUp() {}
void RSBackgroundImageDrawableTest::TearDown() {}

class RSBlendSaveLayerDrawableTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSBlendSaveLayerDrawableTest::SetUpTestCase() {}
void RSBlendSaveLayerDrawableTest::TearDownTestCase() {}
void RSBlendSaveLayerDrawableTest::SetUp() {}
void RSBlendSaveLayerDrawableTest::TearDown() {}

class RSBlendFastDrawableTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSBlendFastDrawableTest::SetUpTestCase() {}
void RSBlendFastDrawableTest::TearDownTestCase() {}
void RSBlendFastDrawableTest::SetUp() {}
void RSBlendFastDrawableTest::TearDown() {}

/**
 * @tc.name: Draw001
 * @tc.desc: test results of Draw
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSClipBoundsDrawableTest, Draw001, TestSize.Level1)
{
    RSClipBoundsDrawable clipBoundsDrawable;
    RSRenderContent content;
    Drawing::Canvas canvas;
    RSPaintFilterCanvas fileCanvas(&canvas);
    RSProperties& properties = content.GetMutableRenderProperties();
    clipBoundsDrawable.Draw(content, fileCanvas);

    Vector4f cornerRadius = { 1.0, 1.0, 1.0, 1.0 };
    properties.SetCornerRadius(cornerRadius);
    clipBoundsDrawable.Draw(content, fileCanvas);
    EXPECT_NE(properties.GetCornerRadius().IsZero(), true);

    RectT<float> rect(1.f, 1.f, 1.f, 1.f);
    RRect clipRRect(rect, 1.f, 1.f);
    properties.SetClipRRect(clipRRect);
    clipBoundsDrawable.Draw(content, fileCanvas);
    EXPECT_EQ(properties.GetClipToRRect(), true);

    auto path = std::make_shared<RSPath>();
    properties.SetClipBounds(path);
    clipBoundsDrawable.Draw(content, fileCanvas);
    EXPECT_NE(properties.GetClipBounds(), nullptr);
}

/**
 * @tc.name: Generate001
 * @tc.desc: test results of Generate
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSPointLightDrawableTest, Generate001, TestSize.Level1)
{
    RSRenderContent content;
    RSProperties& properties = content.GetMutableRenderProperties();
    RSPointLightDrawable::Generate(content);
    EXPECT_EQ(properties.GetIlluminated(), nullptr);

    properties.SetIlluminatedBorderWidth(1.f);
    IlluminatedType illuminatedType = IlluminatedType::BORDER;
    properties.illuminatedPtr_->SetIlluminatedType(illuminatedType);
    RSPointLightDrawable::Generate(content);
    EXPECT_TRUE(properties.illuminatedPtr_->IsIlluminatedValid());
}

/**
 * @tc.name: Draw001
 * @tc.desc: test results of Draw
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSPointLightDrawableTest, Draw001, TestSize.Level1)
{
    RSPointLightDrawable pointLightDrawable;
    RSRenderContent content;
    Drawing::Canvas canvas;
    RSPaintFilterCanvas fileCanvas(&canvas);
    RSProperties& properties = content.GetMutableRenderProperties();
    properties.SetIlluminatedBorderWidth(1.f);
    IlluminatedType illuminatedType = IlluminatedType::BORDER;
    properties.illuminatedPtr_->SetIlluminatedType(illuminatedType);
    pointLightDrawable.Draw(content, fileCanvas);
    EXPECT_TRUE(properties.illuminatedPtr_->IsIlluminatedValid());
}

/**
 * @tc.name: Generate001
 * @tc.desc: test results of Generate
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSBorderDrawableTest, Generate001, TestSize.Level1)
{
    RSRenderContent content;
    RSProperties& properties = content.GetMutableRenderProperties();
    RSBorderDrawable::Generate(content);
    EXPECT_EQ(properties.GetBorder(), nullptr);

    Vector4f width = { 1.0, 1.0, 1.0, 1.0 };
    properties.SetBorderWidth(width);
    properties.GetBorder()->SetWidth(1.f);
    properties.GetBorder()->SetColor(Color(1, 1, 1));
    properties.GetBorder()->SetStyle(BorderStyle::SOLID);
    RSBorderDrawable::Generate(content);
    EXPECT_NE(properties.GetBorder(), nullptr);

    properties.GetBorder()->SetStyle(BorderStyle::NONE);
    Vector4f corner = { 0.f, 0.f, 0.f, 0.f };
    properties.SetCornerRadius(corner);
    RSBorderDrawable::Generate(content);
    EXPECT_EQ(properties.GetCornerRadius().IsZero(), true);

    corner = { 1.f, 1.f, 1.f, 1.f };
    properties.SetCornerRadius(corner);
    RSBorderDrawable::Generate(content);
    EXPECT_EQ(properties.GetCornerRadius().IsZero(), false);
}

/**
 * @tc.name: OnBoundsChange001
 * @tc.desc: test results of OnBoundsChange
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSBorderDRRectDrawableTest, OnBoundsChange001, TestSize.Level1)
{
    Drawing::Brush brush;
    Drawing::Pen pen;
    RSProperties properties;
    bool drawBorder = true;
    RSBorderDRRectDrawable borderDRRectDrawable(std::move(brush), std::move(pen), properties, drawBorder);
    Vector4f cornerRadius(-1.0, -1.0, -1.0, 1.0);
    properties.SetCornerRadius(cornerRadius);
    properties.boundsGeo_->SetWidth(1.f);
    properties.boundsGeo_->SetHeight(1.f);
    borderDRRectDrawable.OnBoundsChange(properties);
    EXPECT_EQ(borderDRRectDrawable.drawBorder_, true);

    borderDRRectDrawable.drawBorder_ = false;
    Vector4f radius = { 1.0, 1.0, 1.0, 1.0 };
    properties.SetOutlineRadius(radius);
    properties.GetOutline()->SetRadiusFour(radius);
    borderDRRectDrawable.OnBoundsChange(properties);
    EXPECT_EQ(borderDRRectDrawable.drawBorder_, false);
}

/**
 * @tc.name: OnBoundsChange001
 * @tc.desc: test results of OnBoundsChange
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSBorderFourLineDrawableTest, OnBoundsChange001, TestSize.Level1)
{
    Drawing::Brush brush;
    Drawing::Pen pen;
    RSProperties properties;
    bool drawBorder = true;
    RSBorderFourLineDrawable borderFourLineDrawable(std::move(brush), std::move(pen), properties, drawBorder);
    borderFourLineDrawable.OnBoundsChange(properties);
    EXPECT_EQ(borderFourLineDrawable.drawBorder_, true);

    borderFourLineDrawable.drawBorder_ = false;
    Vector4f radius = { 1.0, 1.0, 1.0, 1.0 };
    properties.SetOutlineRadius(radius);
    borderFourLineDrawable.OnBoundsChange(properties);
    EXPECT_EQ(borderFourLineDrawable.drawBorder_, false);
}

/**
 * @tc.name: Draw001
 * @tc.desc: test results of Draw
 * @tc.type: FUNC
 * @tc.require: issueI9QMC2
 */
HWTEST_F(RSBorderFourLineDrawableTest, Draw001, TestSize.Level1)
{
    RSRenderContent content;
    Drawing::Brush brush;
    Drawing::Pen pen;
    RSProperties& properties = content.GetMutableRenderProperties();
    bool drawBorder = true;
    Drawing::Canvas canvas;
    RSPaintFilterCanvas fileCanvas(&canvas);
    RSBorderFourLineDrawable borderFourLineDrawable(std::move(brush), std::move(pen), properties, drawBorder);
    Vector4f width = { 1.0, 1.0, 1.0, 1.0 };
    properties.SetBorderWidth(width);
    borderFourLineDrawable.Draw(content, fileCanvas);
    EXPECT_EQ(borderFourLineDrawable.drawBorder_, true);

    borderFourLineDrawable.drawBorder_ = false;
    Vector4f radius = { 1.0, 1.0, 1.0, 1.0 };
    properties.SetOutlineRadius(radius);
    borderFourLineDrawable.Draw(content, fileCanvas);
    EXPECT_EQ(borderFourLineDrawable.drawBorder_, false);
}

/**
 * @tc.name: Draw001
 * @tc.desc: test results of Draw
 * @tc.type: FUNC
 * @tc.require: issueI9QMC2
 */
HWTEST_F(RSBorderFourLineRoundCornerDrawableTest, Draw001, TestSize.Level1)
{
    RSRenderContent content;
    Drawing::Canvas canvas;
    RSPaintFilterCanvas fileCanvas(&canvas);
    Drawing::Brush brush;
    Drawing::Pen pen;
    RSProperties& properties = content.GetMutableRenderProperties();
    bool drawBorder = true;
    RSBorderFourLineRoundCornerDrawable drawable(std::move(brush), std::move(pen), properties, drawBorder);
    Vector4f width = { 1.0, 1.0, 1.0, 1.0 };
    properties.SetBorderWidth(width);
    drawable.Draw(content, fileCanvas);
    EXPECT_EQ(drawable.drawBorder_, true);

    drawable.drawBorder_ = false;
    Vector4f radius = { 1.0, 1.0, 1.0, 1.0 };
    properties.SetOutlineRadius(radius);
    drawable.Draw(content, fileCanvas);
    EXPECT_EQ(drawable.drawBorder_, false);
}

/**
 * @tc.name: OnBoundsChange001
 * @tc.desc: test results of OnBoundsChange
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSBorderFourLineRoundCornerDrawableTest, OnBoundsChange001, TestSize.Level1)
{
    Drawing::Brush brush;
    Drawing::Pen pen;
    RSProperties properties;
    bool drawBorder = true;
    RSBorderFourLineRoundCornerDrawable drawable(std::move(brush), std::move(pen), properties, drawBorder);
    drawable.OnBoundsChange(properties);
    EXPECT_EQ(drawable.drawBorder_, true);

    drawable.drawBorder_ = false;
    Vector4f radius = { 1.0, 1.0, 1.0, 1.0 };
    properties.SetOutlineRadius(radius);
    drawable.OnBoundsChange(properties);
    EXPECT_EQ(drawable.drawBorder_, false);
}

/**
 * @tc.name: Generate001
 * @tc.desc: test results of Generate
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOutlineDrawableTest, Generate001, TestSize.Level1)
{
    RSRenderContent content;
    RSProperties& properties = content.GetMutableRenderProperties();
    RSOutlineDrawable::Generate(content);
    EXPECT_EQ(properties.GetOutline(), nullptr);

    Vector4f radius = { 1.0, 1.0, 1.0, 1.0 };
    properties.SetOutlineRadius(radius);
    properties.GetOutline()->SetWidth(1.f);
    properties.GetOutline()->SetColor(Color(1, 1, 1));
    properties.GetOutline()->SetStyle(BorderStyle::SOLID);
    RSOutlineDrawable::Generate(content);
    EXPECT_NE(properties.GetOutline(), nullptr);

    properties.GetOutline()->SetStyle(BorderStyle::NONE);
    Vector4f corner = { 0.f, 0.f, 0.f, 0.f };
    properties.SetCornerRadius(corner);
    RSOutlineDrawable::Generate(content);
    EXPECT_EQ(properties.GetOutline()->GetRadiusFour().IsZero(), false);

    corner = { 1.f, 1.f, 1.f, 1.f };
    properties.SetCornerRadius(corner);
    RSOutlineDrawable::Generate(content);
    EXPECT_NE(properties.GetOutline(), nullptr);
}

/**
 * @tc.name: Generate001
 * @tc.desc: test results of Generate
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMaskDrawableTest, Generate001, TestSize.Level1)
{
    RSRenderContent content;
    RSProperties& properties = content.GetMutableRenderProperties();
    RSMaskDrawable::Generate(content);
    EXPECT_EQ(properties.GetMask(), nullptr);

    auto mask = std::make_shared<RSMask>();
    properties.SetMask(mask);
    RSMaskDrawable::Generate(content);
    EXPECT_NE(mask, nullptr);

    mask->SetMaskType(MaskType::SVG);
    RSMaskDrawable::Generate(content);
    EXPECT_EQ(mask->IsSvgMask(), true);

    mask->SetMaskType(MaskType::GRADIENT);
    RSMaskDrawable::Generate(content);
    EXPECT_EQ(mask->IsGradientMask(), true);

    mask->SetMaskType(MaskType::PATH);
    RSMaskDrawable::Generate(content);
    EXPECT_EQ(mask->IsPathMask(), true);

    mask->SetMaskType(MaskType::PIXEL_MAP);
    RSMaskDrawable::Generate(content);
    EXPECT_EQ(mask->IsPixelMapMask(), true);
}

/**
 * @tc.name: Generate001
 * @tc.desc: test results of Generate
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSShadowBaseDrawableTest, Generate001, TestSize.Level1)
{
    RSRenderContent content;
    RSProperties& properties = content.GetMutableRenderProperties();
    RSShadowBaseDrawable::Generate(content);
    EXPECT_EQ(properties.IsShadowValid(), false);

    properties.shadow_ = std::make_optional<RSShadow>();
    properties.shadow_->elevation_ = 1.f;
    properties.shadow_->color_.alpha_ = 255;
    properties.shadow_->radius_ = 1.f;
    RSShadowBaseDrawable::Generate(content);
    EXPECT_EQ(properties.GetShadowElevation(), 1.f);
    EXPECT_EQ(properties.IsShadowValid(), true);

    properties.shadow_->imageMask_ = true;
    RSShadowBaseDrawable::Generate(content);
    EXPECT_EQ(properties.IsShadowValid(), true);
}

/**
 * @tc.name: ClipShadowPath001
 * @tc.desc: test results of ClipShadowPath
 * @tc.type: FUNC
 * @tc.require: issueI9QMC2
 */
HWTEST_F(RSShadowBaseDrawableTest, ClipShadowPath001, TestSize.Level1)
{
    RSRenderContent content;
    Drawing::Canvas canvas;
    RSPaintFilterCanvas fileCanvas(&canvas);
    Drawing::Path path;
    RSProperties& properties = content.GetMutableRenderProperties();
    RSShadowDrawable shadowBaseDrawable(properties);
    shadowBaseDrawable.ClipShadowPath(content, fileCanvas, path);
    EXPECT_EQ(properties.GetShadowIsFilled(), false);

    properties.clipPath_ = std::make_shared<RSPath>();
    shadowBaseDrawable.ClipShadowPath(content, fileCanvas, path);
    EXPECT_NE(properties.clipPath_, nullptr);
}

/**
 * @tc.name: GetColorForShadow001
 * @tc.desc: test results of GetColorForShadow
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSShadowDrawableTest, GetColorForShadow001, TestSize.Level1)
{
    RSRenderContent content;
    RSProperties& properties = content.GetMutableRenderProperties();
    RSShadowDrawable shadowDrawable(properties);
    Drawing::Canvas canvas;
    RSPaintFilterCanvas fileCanvas(&canvas);
    Drawing::Path skPath;
    Drawing::Matrix matrix;
    Drawing::RectI deviceClipBounds;
    shadowDrawable.GetColorForShadow(content, fileCanvas, skPath, matrix, deviceClipBounds);
    EXPECT_NE(properties.GetColorPickerCacheTaskShadow(), nullptr);

    properties.shadow_ = std::make_optional<RSShadow>();
    properties.shadow_->SetColorStrategy(SHADOW_COLOR_STRATEGY::COLOR_STRATEGY_AVERAGE);
    shadowDrawable.GetColorForShadow(content, fileCanvas, skPath, matrix, deviceClipBounds);
    EXPECT_TRUE(properties.visible_);
}

/**
 * @tc.name: Draw001
 * @tc.desc: test results of Draw
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSShadowDrawableTest, Draw001, TestSize.Level1)
{
    RSRenderContent content;
    RSProperties& properties = content.GetMutableRenderProperties();
    RSShadowDrawable shadowDrawable(properties);
    Drawing::Canvas canvas;
    RSPaintFilterCanvas fileCanvas(&canvas);
    shadowDrawable.Draw(content, fileCanvas);
    EXPECT_EQ(shadowDrawable.radius_, 0.f);

    fileCanvas.SetCacheType(RSPaintFilterCanvas::CacheType::ENABLED);
    shadowDrawable.Draw(content, fileCanvas);
    EXPECT_NE(shadowDrawable.radius_, -0.1f);

    properties.SetNeedSkipShadow(true);
    shadowDrawable.Draw(content, fileCanvas);
    EXPECT_NE(shadowDrawable.radius_, -0.1f);
}

/**
 * @tc.name: Draw001
 * @tc.desc: test results of Draw
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSHardwareAccelerationShadowDrawableTest, Draw001, TestSize.Level1)
{
    RSRenderContent content;
    RSProperties& properties = content.GetMutableRenderProperties();
    RSHardwareAccelerationShadowDrawable shadowDrawable(properties);
    Drawing::Canvas canvas;
    RSPaintFilterCanvas fileCanvas(&canvas);
    shadowDrawable.Draw(content, fileCanvas);
    EXPECT_NE(fileCanvas.GetCacheType(), RSPaintFilterCanvas::CacheType::ENABLED);

    fileCanvas.SetCacheType(RSPaintFilterCanvas::CacheType::ENABLED);
    shadowDrawable.Draw(content, fileCanvas);
    EXPECT_EQ(fileCanvas.GetCacheType(), RSPaintFilterCanvas::CacheType::ENABLED);
}

/**
 * @tc.name: Draw001
 * @tc.desc: test results of Draw
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSColorfulShadowDrawableTest, Draw001, TestSize.Level1)
{
    RSRenderContent content;
    RSProperties& properties = content.GetMutableRenderProperties();
    RSColorfulShadowDrawable shadowDrawable(properties);
    Drawing::Canvas canvas;
    RSPaintFilterCanvas fileCanvas(&canvas);
    shadowDrawable.Draw(content, fileCanvas);
    EXPECT_NE(fileCanvas.GetCacheType(), RSPaintFilterCanvas::CacheType::ENABLED);

    fileCanvas.SetCacheType(RSPaintFilterCanvas::CacheType::ENABLED);
    shadowDrawable.Draw(content, fileCanvas);
    EXPECT_EQ(fileCanvas.GetCacheType(), RSPaintFilterCanvas::CacheType::ENABLED);
}

/**
 * @tc.name: Generate001
 * @tc.desc: test results of Generate
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSDynamicLightUpDrawableTest, Generate001, TestSize.Level1)
{
    RSRenderContent content;
    RSProperties& properties = content.GetMutableRenderProperties();
    RSDynamicLightUpDrawable::Generate(content);
    EXPECT_EQ(properties.IsDynamicLightUpValid(), false);

    std::optional<float> rate = std::optional<float>(1.f);
    properties.SetDynamicLightUpRate(rate);
    std::optional<float> lightUpDegree = std::optional<float>(1.f);
    properties.SetDynamicLightUpDegree(lightUpDegree);
    RSDynamicLightUpDrawable::Generate(content);
    EXPECT_EQ(properties.IsDynamicLightUpValid(), true);
}

/**
 * @tc.name: Generate001
 * @tc.desc: test results of Generate
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSLightUpEffectDrawableTest, Generate001, TestSize.Level1)
{
    RSRenderContent content;
    RSProperties& properties = content.GetMutableRenderProperties();
    RSLightUpEffectDrawable::Generate(content);
    EXPECT_EQ(content.GetRenderProperties().IsLightUpEffectValid(), false);

    properties.SetLightUpEffect(0.f);
    RSLightUpEffectDrawable::Generate(content);
    EXPECT_EQ(content.GetRenderProperties().IsLightUpEffectValid(), true);
}

/**
 * @tc.name: Generate001
 * @tc.desc: test results of Generate
 * @tc.type: FUNC
 * @tc.require: issueI9QMC2
 */
HWTEST_F(RSDynamicDimDrawableTest, Generate001, TestSize.Level1)
{
    RSRenderContent content;
    RSProperties& properties = content.GetMutableRenderProperties();
    RSDynamicDimDrawable::Generate(content);
    EXPECT_EQ(properties.IsLightUpEffectValid(), false);
}

/**
 * @tc.name: Generate001
 * @tc.desc: test results of Generate
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSBinarizationDrawableTest, Generate001, TestSize.Level1)
{
    RSRenderContent content;
    RSBinarizationDrawable::Generate(content);

    RSProperties& properties = content.GetMutableRenderProperties();
    float x = -1.0;
    float y = -1.0;
    float z = -1.0;
    float w = 1.0;
    Vector4f aiInvert(x, y, z, w);
    properties.SetAiInvert(aiInvert);
    RSBinarizationDrawable::Generate(content);
    EXPECT_TRUE(true);
}

/**
 * @tc.name: Generate001
 * @tc.desc: test results of Generate
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSBackgroundFilterDrawableTest, Generate001, TestSize.Level1)
{
    RSRenderContent content;
    RSProperties& properties = content.GetMutableRenderProperties();
    RSBackgroundFilterDrawable::Generate(content);
    EXPECT_EQ(properties.GetBackgroundFilter(), nullptr);

    auto backgroundFilter = std::make_shared<RSFilter>();
    properties.SetBackgroundFilter(backgroundFilter);
    RSBackgroundFilterDrawable::Generate(content);
    EXPECT_NE(properties.GetBackgroundFilter(), nullptr);

    content.type_ = RSRenderNodeType::EFFECT_NODE;
    RSBackgroundFilterDrawable::Generate(content);
    EXPECT_EQ(content.GetType(), RSRenderNodeType::EFFECT_NODE);
}

/**
 * @tc.name: Draw001
 * @tc.desc: test results of Draw
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSBackgroundFilterDrawableTest, Draw001, TestSize.Level1)
{
    RSBackgroundFilterDrawable drawable;
    RSRenderContent content;
    Drawing::Canvas canvas;
    RSPaintFilterCanvas fileCanvas(&canvas);
    drawable.Draw(content, fileCanvas);
    EXPECT_NE(fileCanvas.GetCacheType(), RSPaintFilterCanvas::CacheType::OFFSCREEN);

    fileCanvas.SetCacheType(RSPaintFilterCanvas::CacheType::OFFSCREEN);
    drawable.Draw(content, fileCanvas);
    EXPECT_EQ(fileCanvas.GetCacheType(), RSPaintFilterCanvas::CacheType::OFFSCREEN);
}

/**
 * @tc.name: Generate001
 * @tc.desc: test results of Generate
 * @tc.type: FUNC
 * @tc.require: issueI9QMC2
 */
HWTEST_F(RSCompositingFilterDrawableTest, Generate001, TestSize.Level1)
{
    RSRenderContent content;
    RSProperties& properties = content.GetMutableRenderProperties();
    RSBackgroundFilterDrawable::Generate(content);
    EXPECT_EQ(properties.GetFilter(), nullptr);

    properties.filter_ = std::make_shared<RSFilter>();
    RSBackgroundFilterDrawable::Generate(content);
    EXPECT_NE(properties.GetFilter(), nullptr);
}

/**
 * @tc.name: Generate001
 * @tc.desc: test results of Generate
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSForegroundFilterDrawableTest, Generate001, TestSize.Level1)
{
    RSRenderContent content;
    RSProperties& properties = content.GetMutableRenderProperties();
    RSForegroundFilterDrawable::Generate(content);
    EXPECT_EQ(properties.GetForegroundFilter(), nullptr);

    properties.foregroundFilter_ = std::make_shared<RSFilter>();
    RSForegroundFilterDrawable::Generate(content);
    EXPECT_NE(properties.GetForegroundFilter(), nullptr);
}

/**
 * @tc.name: Draw001
 * @tc.desc: test results of Draw
 * @tc.type: FUNC
 * @tc.require: issueI9QMC2
 */
HWTEST_F(RSForegroundFilterDrawableTest, Draw001, TestSize.Level1)
{
    RSForegroundFilterDrawable drawable;
    RSRenderContent content;
    Drawing::Canvas canvas;
    RSPaintFilterCanvas fileCanvas(&canvas);
    drawable.Draw(content, fileCanvas);
}

/**
 * @tc.name: Generate001
 * @tc.desc: test results of Generate
 * @tc.type: FUNC
 * @tc.require: issueI9QMC2
 */
HWTEST_F(RSForegroundFilterRestoreDrawableTest, Generate001, TestSize.Level1)
{
    RSRenderContent content;
    RSProperties& properties = content.GetMutableRenderProperties();
    RSForegroundFilterRestoreDrawable::Generate(content);
    EXPECT_EQ(properties.GetForegroundFilter(), nullptr);

    properties.foregroundFilter_ = std::make_shared<RSFilter>();
    RSForegroundFilterRestoreDrawable::Generate(content);
    EXPECT_NE(properties.GetForegroundFilter(), nullptr);
}

/**
 * @tc.name: Draw001
 * @tc.desc: test results of Draw
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSEffectDataGenerateDrawableTest, Draw001, TestSize.Level1)
{
    RSRenderContent content;
    RSProperties& properties = content.GetMutableRenderProperties();
    Drawing::Canvas canvas;
    RSPaintFilterCanvas fileCanvas(&canvas);
    RSEffectDataGenerateDrawable drawable;
    properties.SetHaveEffectRegion(true);
    auto backgroundFilter = std::make_shared<RSFilter>();
    properties.SetBackgroundFilter(backgroundFilter);
    drawable.Draw(content, fileCanvas);
    EXPECT_EQ(properties.GetHaveEffectRegion(), true);
}

/**
 * @tc.name: Draw001
 * @tc.desc: test results of Draw
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSEffectDataApplyDrawableTest, Draw001, TestSize.Level1)
{
    RSRenderContent content;
    Drawing::Canvas canvas;
    RSPaintFilterCanvas fileCanvas(&canvas);
    RSEffectDataApplyDrawable drawable;
    drawable.Draw(content, fileCanvas);
    EXPECT_NE(fileCanvas.GetCacheType(), RSPaintFilterCanvas::CacheType::OFFSCREEN);

    fileCanvas.SetCacheType(RSPaintFilterCanvas::CacheType::OFFSCREEN);
    drawable.Draw(content, fileCanvas);
    EXPECT_EQ(fileCanvas.GetCacheType(), RSPaintFilterCanvas::CacheType::OFFSCREEN);
}

/**
 * @tc.name: Generate001
 * @tc.desc: test results of Generate
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSEffectDataApplyDrawableTest, Generate001, TestSize.Level1)
{
    RSRenderContent content;
    RSProperties& properties = content.GetMutableRenderProperties();
    RSEffectDataApplyDrawable::Generate(content);
    EXPECT_EQ(properties.GetUseEffect(), false);

    properties.SetUseEffect(true);
    RSEffectDataApplyDrawable::Generate(content);
    EXPECT_EQ(properties.GetUseEffect(), true);
}

/**
 * @tc.name: Generate001
 * @tc.desc: test results of Generate
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSForegroundColorDrawableTest, Generate001, TestSize.Level1)
{
    RSRenderContent content;
    RSProperties& properties = content.GetMutableRenderProperties();
    Color color(1, 1, 1, 1); // for test
    properties.SetForegroundColor(color);
    RSForegroundColorDrawable::Generate(content);

    color.SetAlpha(0);
    color.SetGreen(0);
    color.SetRed(0);
    color.SetBlue(0);
    RSForegroundColorDrawable::Generate(content);
    EXPECT_TRUE(true);
}

/**
 * @tc.name: Generate001
 * @tc.desc: test results of Generate
 * @tc.type: FUNC
 * @tc.require: issueI9QMC2
 */
HWTEST_F(RSParticleDrawableTest, Generate001, TestSize.Level1)
{
    RSRenderContent content;
    RSProperties& properties = content.GetMutableRenderProperties();
    RSParticleDrawable::Generate(content);
    EXPECT_EQ(properties.GetParticles().GetParticleSize(), 0);
}

/**
 * @tc.name: Generate001
 * @tc.desc: test results of Generate
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSPixelStretchDrawableTest, Generate001, TestSize.Level1)
{
    RSRenderContent content;
    RSProperties& properties = content.GetMutableRenderProperties();
    RSPixelStretchDrawable::Generate(content);
    EXPECT_EQ(properties.GetPixelStretch().has_value(), false);

    Vector4f stretchSize(1.f, 1.f, 1.f, 1.f); // for test
    properties.SetPixelStretch(stretchSize);
    RSPixelStretchDrawable::Generate(content);
    EXPECT_EQ(properties.GetPixelStretch().has_value(), true);
}

/**
 * @tc.name: Draw001
 * @tc.desc: test results of Draw
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSBackgroundDrawableTest, Draw001, TestSize.Level1)
{
    RSRenderContent content;
    RSProperties& properties = content.GetMutableRenderProperties();
    Drawing::Canvas canvas;
    RSPaintFilterCanvas fileCanvas(&canvas);
    RSBackgroundDrawable drawable;
    Vector4f width = { 1.f, 1.f, 1.f, 1.f }; // for test
    properties.SetBorderWidth(width);
    drawable.Draw(content, fileCanvas);
    EXPECT_EQ(properties.GetCornerRadius().IsZero(), true);

    properties.GetBorderColor()[0].SetAlpha(255);
    drawable.Draw(content, fileCanvas);
    EXPECT_NE(properties.GetCornerRadius().IsZero(), false);
}

/**
 * @tc.name: Generate001
 * @tc.desc: test results of Generate
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSBackgroundColorDrawableTest, Generate001, TestSize.Level1)
{
    RSRenderContent content;
    RSProperties& properties = content.GetMutableRenderProperties();
    Color color(1, 1, 1, 1); // for test
    properties.decoration_ = std::make_optional<Decoration>();
    properties.decoration_->backgroundColor_ = color;
    RSBackgroundColorDrawable::Generate(content);

    properties.decoration_->backgroundColor_ = RgbPalette::Transparent();
    RSBackgroundColorDrawable::Generate(content);
    EXPECT_TRUE(true);
}

/**
 * @tc.name: Update001
 * @tc.desc: test results of Update
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSBackgroundColorDrawableTest, Update001, TestSize.Level1)
{
    RSRenderContent content;
    RSProperties& properties = content.GetMutableRenderProperties();
    Drawing::Color color;
    RSBackgroundColorDrawable drawable(color);
    Color groundColor(1, 1, 1, 1); // for test
    properties.decoration_ = std::make_optional<Decoration>();
    properties.decoration_->backgroundColor_ = groundColor;
    bool res = drawable.Update(content);
    EXPECT_EQ(res, true);

    properties.decoration_->backgroundColor_ = RgbPalette::Transparent();
    res = drawable.Update(content);
    EXPECT_EQ(res, false);
}

/**
 * @tc.name: Generate001
 * @tc.desc: test results of Generate
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSBackgroundShaderDrawableTest, Generate001, TestSize.Level1)
{
    RSRenderContent content;
    RSProperties& properties = content.GetMutableRenderProperties();
    RSBackgroundShaderDrawable::Generate(content);
    EXPECT_EQ(properties.GetBackgroundShader(), nullptr);

    properties.decoration_ = std::make_optional<Decoration>();
    properties.decoration_->bgShader_ = std::make_shared<RSShader>();
    RSBackgroundShaderDrawable::Generate(content);
    EXPECT_NE(properties.GetBackgroundShader(), nullptr);
}

/**
 * @tc.name: Generate001
 * @tc.desc: test results of Generate
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSBackgroundImageDrawableTest, Generate001, TestSize.Level1)
{
    RSRenderContent content;
    RSProperties& properties = content.GetMutableRenderProperties();
    RSBackgroundImageDrawable::Generate(content);
    EXPECT_EQ(properties.GetBgImage(), nullptr);

    properties.decoration_ = std::make_optional<Decoration>();
    properties.decoration_->bgImage_ = std::make_shared<RSImage>();
    RSBackgroundImageDrawable::Generate(content);
    EXPECT_NE(properties.GetBgImage(), nullptr);
}

/**
 * @tc.name: Update001
 * @tc.desc: test results of Update
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSBackgroundImageDrawableTest, Update001, TestSize.Level1)
{
    RSRenderContent content;
    RSProperties& properties = content.GetMutableRenderProperties();
    RSBackgroundImageDrawable drawable;
    bool res = drawable.Update(content);
    EXPECT_EQ(res, false);

    properties.decoration_ = std::make_optional<Decoration>();
    properties.decoration_->bgImage_ = std::make_shared<RSImage>();
    res = drawable.Update(content);
    EXPECT_EQ(res, true);
}

/**
 * @tc.name: Draw001
 * @tc.desc: test results of Draw
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSBlendSaveLayerDrawableTest, Draw001, TestSize.Level1)
{
    RSRenderContent content;
    Drawing::Canvas canvas;
    RSPaintFilterCanvas fileCanvas(&canvas);
    auto drawable = std::make_shared<RSBlendSaveLayerDrawable>(2);
    drawable->Draw(content, fileCanvas);
    EXPECT_NE(drawable, nullptr);
}

/**
 * @tc.name: Draw001
 * @tc.desc: test results of Draw
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSBlendFastDrawableTest, Draw001, TestSize.Level1)
{
    RSRenderContent content;
    Drawing::Canvas canvas;
    RSPaintFilterCanvas fileCanvas(&canvas);
    auto drawable = std::make_shared<RSBlendFastDrawable>(2);
    drawable->Draw(content, fileCanvas);
    EXPECT_NE(drawable, nullptr);
}
} // namespace Rosen
} // namespace OHOS