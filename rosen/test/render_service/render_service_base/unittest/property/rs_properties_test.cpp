/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "property/rs_properties.h"
#include "common/rs_obj_abs_geometry.h"
#include "property/rs_point_light_manager.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class RSPropertiesTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSPropertiesTest::SetUpTestCase() {}
void RSPropertiesTest::TearDownTestCase() {}
void RSPropertiesTest::SetUp() {}
void RSPropertiesTest::TearDown() {}

/**
 * @tc.name: SetSublayerTransform001
 * @tc.desc: test results of SetSublayerTransform
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPropertiesTest, SetSublayerTransform001, TestSize.Level1)
{
    RSProperties properties;
    Matrix3f sublayerTransform;
    properties.SetSublayerTransform(sublayerTransform);
    properties.SetSublayerTransform(sublayerTransform);
}

/**
 * @tc.name: SetBackgroundShader001
 * @tc.desc: test results of SetBackgroundShader
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPropertiesTest, SetBackgroundShader001, TestSize.Level1)
{
    RSProperties properties;
    properties.SetBackgroundShader(RSShader::CreateRSShader());
    properties.SetBackgroundShader(RSShader::CreateRSShader());

    properties.SetBackgroundShader(nullptr);
}

/**
 * @tc.name: SetBgImageWidth001
 * @tc.desc: test results of SetBgImageWidth
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPropertiesTest, SetBgImageWidth001, TestSize.Level1)
{
    RSProperties properties;
    ASSERT_TRUE(properties.GetBgImageWidth() == 0.f);
    properties.SetBgImageWidth(1.0f);
    ASSERT_TRUE(properties.GetBgImageWidth() == 1.0f);
    properties.SetBgImageWidth(2.0f);
    ASSERT_TRUE(properties.GetBgImageWidth() == 2.0f);
}

/**
 * @tc.name: SetBgImagePositionX001
 * @tc.desc: test results of SetBgImagePositionX
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPropertiesTest, SetBgImagePositionX001, TestSize.Level1)
{
    RSProperties properties;
    ASSERT_TRUE(properties.GetBgImagePositionX() == 0.f);
    properties.SetBgImagePositionX(1.0f);
    ASSERT_TRUE(properties.GetBgImagePositionX() == 1.0f);
    properties.SetBgImagePositionX(2.0f);
    ASSERT_TRUE(properties.GetBgImagePositionX() == 2.0f);
}

/**
 * @tc.name: SetBgImagePositionY001
 * @tc.desc: test results of SetBgImagePositionY
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPropertiesTest, SetBgImagePositionY001, TestSize.Level1)
{
    RSProperties properties;
    ASSERT_TRUE(properties.GetBgImagePositionY() == 0.f);
    properties.SetBgImagePositionY(1.0f);
    ASSERT_TRUE(properties.GetBgImagePositionY() == 1.0f);
    properties.SetBgImagePositionY(2.0f);
    ASSERT_TRUE(properties.GetBgImagePositionY() == 2.0f);
}

/**
 * @tc.name: SetBgImageHeight001
 * @tc.desc: test results of SetBgImageHeight
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPropertiesTest, SetBgImageHeight001, TestSize.Level1)
{
    RSProperties properties;
    ASSERT_TRUE(properties.GetBgImageHeight() == 0.f);
    properties.SetBgImageHeight(1.0f);
    ASSERT_TRUE(properties.GetBgImageHeight() == 1.0f);
    properties.SetBgImageHeight(2.0f);
    ASSERT_TRUE(properties.GetBgImageHeight() == 2.0f);
}

/**
 * @tc.name: SetBgImage001
 * @tc.desc: test results of SetBgImage
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPropertiesTest, SetBgImage001, TestSize.Level1)
{
    RSProperties properties;
    std::shared_ptr<RSImage> image = std::make_shared<RSImage>();
    ASSERT_TRUE(properties.GetBgImage() == nullptr);
    properties.SetBgImage(image);
    ASSERT_TRUE(properties.GetBgImage() == image);
    std::shared_ptr<RSImage> image2 = std::make_shared<RSImage>();
    properties.SetBgImage(image2);
    ASSERT_TRUE(properties.GetBgImage() == image2);

    properties.SetBgImage(nullptr);
    ASSERT_TRUE(properties.GetBgImage() == nullptr);
}

/**
 * @tc.name: SetShadowColor001
 * @tc.desc: test results of SetShadowColor
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPropertiesTest, SetShadowColor001, TestSize.Level1)
{
    RSProperties properties;
    RSColor color;
    properties.SetShadowColor(color);
    RSColor color2;
    properties.SetShadowColor(color);
}

/**
 * @tc.name: SetShadowOffsetX001
 * @tc.desc: test results of SetShadowOffsetX
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPropertiesTest, SetShadowOffsetX001, TestSize.Level1)
{
    RSProperties properties;
    float offsetX = 0.1f;
    properties.SetShadowOffsetX(offsetX);
    offsetX = 1.1f;
    properties.SetShadowOffsetX(offsetX);
}

/**
 * @tc.name: SetShadowOffsetY001
 * @tc.desc: test results of SetShadowOffsetY
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPropertiesTest, SetShadowOffsetY001, TestSize.Level1)
{
    RSProperties properties;
    float offsetY = 0.1f;
    properties.SetShadowOffsetY(offsetY);
    offsetY = 1.0f;
    properties.SetShadowOffsetY(offsetY);
}

/**
 * @tc.name: SetShadowElevation001
 * @tc.desc: test results of SetShadowElevation
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPropertiesTest, SetShadowElevation001, TestSize.Level1)
{
    RSProperties properties;
    float elevation = 0.1f;
    properties.SetShadowElevation(elevation);
    elevation = 1.0f;
    properties.SetShadowElevation(elevation);
}

/**
 * @tc.name: SetShadowRadius001
 * @tc.desc: test results of SetShadowRadius
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPropertiesTest, SetShadowRadius001, TestSize.Level1)
{
    RSProperties properties;
    float radius = 0.1f;
    properties.SetShadowRadius(radius);
    radius = 1.0f;
    properties.SetShadowRadius(radius);
}

/**
 * @tc.name: SetShadowMask001
 * @tc.desc: test results of SetShadowMask
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPropertiesTest, SetShadowMask001, TestSize.Level1)
{
    RSProperties properties;
    bool shadowMask = true;
    properties.SetShadowMask(shadowMask);
    EXPECT_EQ(properties.GetShadowMask(), shadowMask);

    shadowMask = false;
    properties.SetShadowMask(shadowMask);
    EXPECT_EQ(properties.GetShadowMask(), shadowMask);
}

/**
 * @tc.name: SetClipBounds001
 * @tc.desc: test results of SetClipBounds
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPropertiesTest, SetClipBounds001, TestSize.Level1)
{
    RSProperties properties;
    std::shared_ptr<RSPath> path = std::make_shared<RSPath>();
    properties.SetClipBounds(path);
    ASSERT_TRUE(properties.GetClipBounds() == path);
    properties.SetClipBounds(path);
    ASSERT_TRUE(properties.GetClipBounds() == path);
}

/**
 * @tc.name: SetClipToBounds001
 * @tc.desc: test results of SetClipToBounds
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPropertiesTest, SetClipToBounds001, TestSize.Level1)
{
    RSProperties properties;
    properties.SetClipToBounds(true);
    ASSERT_TRUE(properties.GetClipToBounds() == true);
    properties.SetClipToBounds(true);
    ASSERT_TRUE(properties.GetClipToBounds() == true);
}

/**
 * @tc.name: Dump001
 * @tc.desc: test results of Dump
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPropertiesTest, Dump001, TestSize.Level1)
{
    RSProperties properties;
    properties.Dump();
    properties.SetBoundsPositionX(1.0f);
    properties.SetBoundsPositionY(1.0f);
    properties.SetBoundsWidth(2.0f);
    properties.SetBoundsHeight(3.f);
    properties.SetFramePositionX(1.f);
    properties.SetFramePositionY(1.f);
    properties.SetFrameWidth(2.f);
    properties.SetFrameHeight(3.f);
    properties.Dump();
    properties.SetPositionZ(1.f);
    properties.SetRotation(2.f);
    properties.SetRotationX(1.f);
    properties.SetRotationY(2.f);
    properties.SetTranslateX(3.f);
    properties.SetTranslateY(4.f);
    properties.SetTranslateZ(4.f);
    properties.Dump();
    properties.SetAlpha(1.f);
    properties.SetScaleX(1.f);
    properties.SetScaleY(1.f);
    properties.SetSkewX(1.f);
    properties.SetSkewY(1.f);
    properties.SetPerspX(1.f);
    properties.SetPerspY(1.f);
    Color color1;
    properties.SetForegroundColor(color1);
    properties.SetBackgroundColor(color1);
    std::shared_ptr<RSImage> image = std::make_shared<RSImage>();
    properties.SetBgImage(image);
    properties.SetShadowColor(color1);
    properties.SetShadowOffsetX(1.f);
    properties.SetShadowOffsetY(1.f);
    properties.SetShadowAlpha(1.f);
    properties.SetShadowElevation(1.f);
    properties.SetShadowRadius(1.f);
    properties.Dump();
}

/**
 * @tc.name: IsPixelStretchValid001
 * @tc.desc: test results of OnApplyModifiers
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPropertiesTest, IsPixelStretchValid001, TestSize.Level1)
{
    RSProperties properties;
    EXPECT_EQ(properties.GetPixelStretch(), std::nullopt);

    Vector4f stretchSize;
    properties.SetPixelStretch(stretchSize);
    properties.OnApplyModifiers();
    EXPECT_EQ(properties.GetPixelStretch(), std::nullopt);
}

/**
 * @tc.name: IsPixelStretchValid002
 * @tc.desc: test results of OnApplyModifiers
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPropertiesTest, IsPixelStretchValid002, TestSize.Level1)
{
    RSProperties properties;
    float x = 1e-6f;
    float y = 1e-6f;
    float z = 1e-6f;
    float w = 1.0;
    Vector4f stretchSize(x, y, z, w);
    properties.SetPixelStretch(stretchSize);
    properties.OnApplyModifiers();
    EXPECT_NE(properties.GetPixelStretch(), std::nullopt);

    x = -(1e-6f);
    y = -(1e-6f);
    z = -(1e-6f);
    w = -1.0;
    stretchSize = Vector4f(x, y, z, w);
    properties.SetPixelStretch(stretchSize);
    properties.OnApplyModifiers();
    EXPECT_NE(properties.GetPixelStretch(), std::nullopt);
}

/**
 * @tc.name: IsPixelStretchValid003
 * @tc.desc: test results of OnApplyModifiers
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPropertiesTest, IsPixelStretchValid003, TestSize.Level1)
{
    RSProperties properties;
    float x = -1.0;
    float y = -1.0;
    float z = -1.0;
    float w = 1.0;
    Vector4f stretchSize(x, y, z, w);
    properties.SetPixelStretch(stretchSize);
    properties.OnApplyModifiers();
    EXPECT_EQ(properties.GetPixelStretch(), std::nullopt);

    x = 1.0;
    y = 1.0;
    z = 1.0;
    w = -1.0;
    stretchSize = Vector4f(x, y, z, w);
    properties.SetPixelStretch(stretchSize);
    properties.OnApplyModifiers();
    EXPECT_EQ(properties.GetPixelStretch(), std::nullopt);
}

/**
 * @tc.name: IsPixelStretchValid004
 * @tc.desc: test results of OnApplyModifiers
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPropertiesTest, IsPixelStretchValid004, TestSize.Level1)
{
    RSProperties properties;
    Vector4f stretchSize(-(1e-6f));
    properties.SetPixelStretch(stretchSize);
    properties.OnApplyModifiers();
    EXPECT_EQ(properties.GetPixelStretch(), std::nullopt);

    stretchSize = Vector4f(1e-6f);
    properties.SetPixelStretch(stretchSize);
    properties.OnApplyModifiers();
    EXPECT_EQ(properties.GetPixelStretch(), std::nullopt);
}

/**
 * @tc.name: IsPixelStretchPercentValid001
 * @tc.desc: test results of OnApplyModifiers
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPropertiesTest, IsPixelStretchPercentValid001, TestSize.Level1)
{
    RSProperties properties;
    properties.OnApplyModifiers();
    EXPECT_EQ(properties.GetPixelStretch(), std::nullopt);

    Vector4f stretchPercent;
    properties.SetPixelStretchPercent(stretchPercent);
    properties.OnApplyModifiers();
    EXPECT_EQ(properties.GetPixelStretch(), std::nullopt);
}

/**
 * @tc.name: IsPixelStretchPercentValid002
 * @tc.desc: test results of OnApplyModifiers
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPropertiesTest, IsPixelStretchPercentValid002, TestSize.Level1)
{
    RSProperties properties;
    float x = 1e-6f;
    float y = 1e-6f;
    float z = 1e-6f;
    float w = 1.0;
    Vector4f stretchPercent(x, y, z, w);
    properties.SetPixelStretchPercent(stretchPercent);
    properties.OnApplyModifiers();
    EXPECT_EQ(properties.GetPixelStretch(), std::nullopt);

    x = -(1e-6f);
    y = -(1e-6f);
    z = -(1e-6f);
    w = -1.0;
    stretchPercent = Vector4f(x, y, z, w);
    properties.SetPixelStretchPercent(stretchPercent);
    properties.OnApplyModifiers();
    EXPECT_EQ(properties.GetPixelStretch(), std::nullopt);
}

/**
 * @tc.name: IsPixelStretchPercentValid003
 * @tc.desc: test results of OnApplyModifiers
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPropertiesTest, IsPixelStretchPercentValid003, TestSize.Level1)
{
    RSProperties properties;
    float x = -1.0;
    float y = -1.0;
    float z = -1.0;
    float w = 1.0;
    Vector4f stretchPercent(x, y, z, w);
    properties.SetPixelStretchPercent(stretchPercent);
    properties.OnApplyModifiers();
    EXPECT_EQ(properties.GetPixelStretch(), std::nullopt);

    x = 1.0;
    y = 1.0;
    z = 1.0;
    w = -1.0;
    stretchPercent = Vector4f(x, y, z, w);
    properties.SetPixelStretchPercent(stretchPercent);
    properties.OnApplyModifiers();
    EXPECT_EQ(properties.GetPixelStretch(), std::nullopt);
}

/**
 * @tc.name: IsPixelStretchPercentValid004
 * @tc.desc: test results of OnApplyModifiers
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPropertiesTest, IsPixelStretchPercentValid004, TestSize.Level1)
{
    RSProperties properties;
    Vector4f stretchPercent(-(1e-6f));
    properties.SetPixelStretchPercent(stretchPercent);
    properties.OnApplyModifiers();
    EXPECT_EQ(properties.GetPixelStretch(), std::nullopt);

    stretchPercent = Vector4f(1e-6f);
    properties.SetPixelStretchPercent(stretchPercent);
    properties.OnApplyModifiers();
    EXPECT_EQ(properties.GetPixelStretch(), std::nullopt);
}

/**
 * @tc.name: SetBounds001
 * @tc.desc: test results of SetBounds
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPropertiesTest, SetBounds001, TestSize.Level1)
{
    RSProperties properties;
    Vector4f bounds = { 1.0, 1.0, 1.0, 1.0 };

    properties.SetBoundsWidth(0);
    properties.SetBoundsHeight(0);
    properties.SetBounds(bounds);

    properties.SetBoundsWidth(1.0);
    properties.SetBoundsHeight(0);
    properties.SetBounds(bounds);

    properties.SetBoundsWidth(0);
    properties.SetBoundsHeight(1.0);
    properties.SetBounds(bounds);

    properties.SetBoundsWidth(1.0);
    properties.SetBoundsHeight(1.0);
    properties.SetBounds(bounds);

    auto resBounds = properties.GetBounds();
    ASSERT_EQ(false, resBounds.IsZero());
}

/**
 * @tc.name: GetBounds001
 * @tc.desc: test results of GetBounds
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPropertiesTest, GetBounds001, TestSize.Level1)
{
    RSProperties properties;
    Vector4f bounds = { 1.0, 1.0, 1.0, 1.0 };
    properties.SetBoundsWidth(1.0);
    properties.SetBoundsHeight(1.0);
    properties.SetBoundsPositionX(1.0f);
    properties.SetBoundsPositionY(1.0f);
    properties.SetBounds(bounds);

    auto size = properties.GetBoundsSize();
    ASSERT_NE(0, size.GetLength());

    auto width = properties.GetBoundsWidth();
    ASSERT_EQ(1.0, width);

    auto height = properties.GetBoundsHeight();
    ASSERT_EQ(1.0, height);

    auto positionX = properties.GetBoundsPositionX();
    ASSERT_EQ(1.0, positionX);

    auto positionY = properties.GetBoundsPositionY();
    ASSERT_EQ(1.0, positionY);

    auto position = properties.GetBoundsPosition();
    ASSERT_NE(0, position.GetLength());
}

/**
 * @tc.name: SetFrame001
 * @tc.desc: test results of SetFrame
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPropertiesTest, SetFrame001, TestSize.Level1)
{
    RSProperties properties;
    Vector4f frame = { 1.0, 1.0, 1.0, 1.0 };

    properties.SetFrame(frame);

    properties.SetFrameWidth(1.f);
    properties.SetFrame(frame);

    properties.SetFrameHeight(1.f);
    properties.SetFrame(frame);

    properties.SetFramePositionX(1.f);
    properties.SetFramePositionY(1.f);
    properties.SetFrame(frame);
}

/**
 * @tc.name: GetFrame001
 * @tc.desc: test results of GetFrame
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPropertiesTest, GetFrame001, TestSize.Level1)
{
    RSProperties properties;
    Vector4f frame = { 1.0, 1.0, 1.0, 1.0 };
    properties.SetFrameWidth(1.f);
    properties.SetFrameHeight(1.0);
    properties.SetFramePositionX(1.0f);
    properties.SetFramePositionY(1.0f);
    properties.SetFrame(frame);

    auto size = properties.GetFrameSize();
    ASSERT_NE(0, size.GetLength());

    auto width = properties.GetFrameWidth();
    ASSERT_EQ(1.0, width);

    auto height = properties.GetFrameHeight();
    ASSERT_EQ(1.0, height);

    auto positionX = properties.GetFramePositionX();
    ASSERT_EQ(1.0, positionX);

    auto positionY = properties.GetFramePositionY();
    ASSERT_EQ(1.0, positionY);

    auto position = properties.GetFramePosition();
    ASSERT_NE(0, position.GetLength());
}

/**
 * @tc.name: SetGet001
 * @tc.desc: test results of SetGet001
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPropertiesTest, SetGet001, TestSize.Level1)
{
    RSProperties properties;
    properties.SetPositionZ(1.0);
    auto positionZ = properties.GetPositionZ();
    ASSERT_EQ(1.0, positionZ);

    properties.SetPivotX(1.0);
    properties.SetPivotY(1.0);
    properties.SetPivotZ(1.0);
    ASSERT_EQ(1.0, properties.GetPivotX());
    ASSERT_EQ(1.0, properties.GetPivotY());
    ASSERT_EQ(1.0, properties.GetPivotZ());
    auto pivot = properties.GetPivot();
    ASSERT_NE(0, pivot.GetLength());

    Vector4f corner = { 1.0, 1.0, 1.0, 1.0 };
    properties.SetCornerRadius(corner);
    properties.GetCornerRadius();

    properties.SetRotationX(1.0);
    properties.SetRotationY(1.0);
    properties.SetRotation(1.0);
    auto rotation = properties.GetRotation();
    ASSERT_NE(0, rotation);
    auto rotationX = properties.GetRotationX();
    auto rotationY = properties.GetRotationY();
    ASSERT_NE(0, rotationX);
    ASSERT_NE(0, rotationY);

    properties.SetScaleX(1.0);
    properties.SetScaleY(1.0);
    Vector2f scale2 = { 1.0, 1.0 };
    properties.SetScale(scale2);
    auto scale = properties.GetScale();
    ASSERT_NE(0, scale.GetLength());
    auto scaleX = properties.GetScaleX();
    auto scaleY = properties.GetScaleY();
    ASSERT_NE(0, scaleX);
    ASSERT_NE(0, scaleY);

    properties.SetTranslateX(1.0);
    properties.SetTranslateY(1.0);
    properties.SetTranslateZ(1.0);
    Vector2f translate2 = { 1.0, 1.0 };
    properties.SetTranslate(translate2);
    auto translate = properties.GetTranslate();
    ASSERT_NE(0, translate.GetLength());
    auto translateX = properties.GetTranslateX();
    auto translateY = properties.GetTranslateY();
    auto translateZ = properties.GetTranslateZ();
    ASSERT_NE(0, translateX);
    ASSERT_NE(0, translateY);
    ASSERT_NE(0, translateZ);
}

/**
 * @tc.name: SetGet002
 * @tc.desc: test results of SetGet002
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPropertiesTest, SetGet002, TestSize.Level1)
{
    RSProperties properties;

    RSRenderParticleVector particles2;
    properties.SetParticles(particles2);

    properties.SetAlpha(0.f);
    float alpha = properties.GetAlpha();
    ASSERT_EQ(0.f, alpha);
    properties.SetAlpha(1.f);
    alpha = properties.GetAlpha();
    ASSERT_EQ(1.f, alpha);

    properties.SetAlphaOffscreen(true);
    bool offScreen = properties.GetAlphaOffscreen();
    ASSERT_EQ(true, offScreen);

    Color color(1, 1, 1);
    properties.SetForegroundColor(color);
    EXPECT_EQ(true, properties.IsContentDirty());
    EXPECT_NE(0, properties.GetForegroundColor().GetBlue());
    properties.SetBackgroundColor(color);
    EXPECT_NE(0, properties.GetBackgroundColor().GetBlue());

    Vector4<Color> borderColor = { color, color, color, color };
    properties.SetBorderColor(borderColor);
    properties.GetBorderColor();
    Vector4f zeroWidth = { 0, 0, 0, 0 };
    Vector4f width = { 1.0, 1.0, 1.0, 1.0 };
    properties.SetBorderWidth(zeroWidth);
    properties.SetBorderWidth(width);
    EXPECT_NE(0, properties.GetBorderWidth().GetLength());

    Vector4<uint32_t> style = { 1, 1, 1, 1 };
    properties.SetBorderStyle(style);
    EXPECT_NE(0, properties.GetBorderStyle().GetLength());

    EXPECT_NE(nullptr, properties.GetBorder());
}

/**
 * @tc.name: SetGet003
 * @tc.desc: test results of SetGet003
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPropertiesTest, SetGet003, TestSize.Level1)
{
    RSProperties properties;
    Color color(1, 1, 1);
    Vector4<Color> outLineColor = { color, color, color, color };
    properties.SetOutlineColor(outLineColor);
    Vector4f zeroWidth = { 0, 0, 0, 0 };
    Vector4f width = { 1.0, 1.0, 1.0, 1.0 };
    properties.SetOutlineWidth(zeroWidth);
    properties.SetOutlineWidth(width);

    Vector4<uint32_t> style = { 1, 1, 1, 1 };
    properties.SetOutlineStyle(style);
    properties.GetOutlineStyle();

    Vector4f radius = { 1.0, 1.0, 1.0, 1.0 };
    properties.SetOutlineRadius(radius);
    properties.GetOutlineRadius();

    EXPECT_NE(nullptr, properties.GetOutline());
}

/**
 * @tc.name: ResetProperty001
 * @tc.desc: test results of ResetProperty
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPropertiesTest, ResetProperty001, TestSize.Level1)
{
    RSProperties properties;
    std::bitset<static_cast<int>(RSModifierType::MAX_RS_MODIFIER_TYPE)> dirtyTypes;
    properties.ResetProperty(dirtyTypes);

    dirtyTypes.set(5);
    dirtyTypes.set(0);
    properties.ResetProperty(dirtyTypes);
    EXPECT_TRUE(true);
}

/**
 * @tc.name: UpdateGeometry001
 * @tc.desc: test results of UpdateGeometry
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPropertiesTest, UpdateGeometry001, TestSize.Level1)
{
    RSProperties properties;
    RSProperties* parent = &properties;
    std::optional<Drawing::Point> offset;
    bool res = properties.UpdateGeometry(parent, false, offset);
    EXPECT_EQ(res, false);

    properties.SetFramePositionY(1.0f);
    res = properties.UpdateGeometry(parent, true, offset);
    EXPECT_EQ(res, true);

    auto newVect = std::make_shared<Vector2f>(1.f, 1.f);
    std::optional<Vector2f> parentPosition = *newVect;
    properties.SetSandBox(parentPosition);
    Vector4f lightPosition = {1.f, 1.f, 1.f, 1.f};
    properties.SetLightPosition(lightPosition);
    properties.SetIlluminatedBorderWidth(1.f);
    properties.UpdateGeometry(parent, true, offset);
    EXPECT_EQ(res, true);
}

/**
 * @tc.name: SetSandBox001
 * @tc.desc: test results of SetSandBox
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPropertiesTest, SetSandBox001, TestSize.Level1)
{
    RSProperties properties;
    auto newVect = std::make_shared<Vector2f>(1.f, 1.f);
    std::optional<Vector2f> parentPosition = *newVect;
    properties.SetSandBox(parentPosition);
    EXPECT_NE(properties.sandbox_, nullptr);

    properties.SetSandBox(parentPosition);
    EXPECT_NE(properties.sandbox_, nullptr);
}

/**
 * @tc.name: UpdateSandBoxMatrix001
 * @tc.desc: test results of UpdateSandBoxMatrix
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPropertiesTest, UpdateSandBoxMatrix001, TestSize.Level1)
{
    RSProperties properties;
    std::optional<Drawing::Matrix> rootMatrix;
    properties.UpdateSandBoxMatrix(rootMatrix);
    EXPECT_EQ(properties.sandbox_, nullptr);

    auto newVect = std::make_shared<Vector2f>(1.f, 1.f);
    std::optional<Vector2f> parentPosition = *newVect;
    properties.SetSandBox(parentPosition);
    properties.UpdateSandBoxMatrix(rootMatrix);
    EXPECT_NE(properties.sandbox_, nullptr);

    auto newMatrix = std::make_shared<Drawing::Matrix>();
    rootMatrix = *newMatrix;
    properties.UpdateSandBoxMatrix(rootMatrix);
    EXPECT_NE(properties.sandbox_, nullptr);
}

/**
 * @tc.name: SetBackgroundFilter001
 * @tc.desc: test results of SetBackgroundFilter
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPropertiesTest, SetBackgroundFilter001, TestSize.Level1)
{
    RSProperties properties;
    std::shared_ptr<RSFilter> backgroundFilter = std::make_shared<RSFilter>();
    properties.SetBackgroundFilter(backgroundFilter);
    EXPECT_NE(properties.backgroundFilter_, nullptr);
}

/**
 * @tc.name: SetLinearGradientBlurPara001
 * @tc.desc: test results of SetLinearGradientBlurPara
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPropertiesTest, SetLinearGradientBlurPara001, TestSize.Level1)
{
    RSProperties properties;
    float blurRadius = 1.f;
    std::vector<std::pair<float, float>>fractionStops;
    GradientDirection direction = GradientDirection::LEFT;
    auto para = std::make_shared<RSLinearGradientBlurPara>(blurRadius, fractionStops, direction);
    properties.SetLinearGradientBlurPara(para);
    EXPECT_NE(properties.linearGradientBlurPara_, nullptr);
}

/**
 * @tc.name: SetDynamicLightUpRate001
 * @tc.desc: test results of SetDynamicLightUpRate
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPropertiesTest, SetDynamicLightUpRate001, TestSize.Level1)
{
    RSProperties properties;
    std::optional<float> rate = std::optional<float>(1.f);
    properties.SetDynamicLightUpRate(rate);
    EXPECT_EQ(properties.filterNeedUpdate_, true);
}

/**
 * @tc.name: SetDynamicLightUpDegree001
 * @tc.desc: test results of SetDynamicLightUpDegree
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPropertiesTest, SetDynamicLightUpDegree001, TestSize.Level1)
{
    RSProperties properties;
    std::optional<float> lightUpDegree = std::optional<float>(1.f);
    properties.SetDynamicLightUpDegree(lightUpDegree);
    EXPECT_EQ(properties.filterNeedUpdate_, true);
}

/**
 * @tc.name: SetFilter001
 * @tc.desc: test results of SetFilter
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPropertiesTest, SetFilter001, TestSize.Level1)
{
    RSProperties properties;
    std::shared_ptr<RSFilter> filter = std::make_shared<RSFilter>();
    properties.SetFilter(filter);
    EXPECT_NE(properties.filter_, nullptr);
}

/**
 * @tc.name: IfLinearGradientBlurInvalid001
 * @tc.desc: test results of IfLinearGradientBlurInvalid
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPropertiesTest, IfLinearGradientBlurInvalid001, TestSize.Level1)
{
    RSProperties properties;
    float blurRadius = -0.001f;
    std::vector<std::pair<float, float>>fractionStops;
    GradientDirection direction = GradientDirection::LEFT;
    std::shared_ptr<RSLinearGradientBlurPara> para = std::make_shared<RSLinearGradientBlurPara>(blurRadius, fractionStops, direction);
    properties.SetLinearGradientBlurPara(para);
    properties.IfLinearGradientBlurInvalid();
    EXPECT_EQ(properties.linearGradientBlurPara_, nullptr);
}

/**
 * @tc.name: SetShadowPath001
 * @tc.desc: test results of SetShadowPath
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPropertiesTest, SetShadowPath001, TestSize.Level1)
{
    RSProperties properties;
    std::shared_ptr<RSPath> shadowPath = std::make_shared<RSPath>();
    properties.SetShadowPath(shadowPath);
    EXPECT_EQ(properties.contentDirty_, true);
}

/**
 * @tc.name: SetShadowIsFilled001
 * @tc.desc: test results of SetShadowIsFilled
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPropertiesTest, SetShadowIsFilled001, TestSize.Level1)
{
    RSProperties properties;
    properties.SetShadowIsFilled(true);
    EXPECT_EQ(properties.contentDirty_, true);
}

/**
 * @tc.name: SetShadowColorStrategy001
 * @tc.desc: test results of SetShadowColorStrategy
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPropertiesTest, SetShadowColorStrategy001, TestSize.Level1)
{
    RSProperties properties;
    properties.SetShadowColorStrategy(1);
    EXPECT_EQ(properties.filterNeedUpdate_, true);
}

/**
 * @tc.name: SetFrameGravity001
 * @tc.desc: test results of SetFrameGravity
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPropertiesTest, SetFrameGravity001, TestSize.Level1)
{
    RSProperties properties;
    Gravity gravity = Gravity::CENTER;
    properties.SetFrameGravity(gravity);
    EXPECT_EQ(properties.contentDirty_, true);
}

/**
 * @tc.name: SetClipRRect001
 * @tc.desc: test results of SetClipRRect
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPropertiesTest, SetClipRRect001, TestSize.Level1)
{
    RSProperties properties;
    RectT<float> rect(1.f, 1.f, 1.f, 1.f);
    RRect clipRRect(rect, 1.f, 1.f);
    properties.SetClipRRect(clipRRect);
    EXPECT_EQ(properties.geoDirty_, true);
}

/**
 * @tc.name: SetClipToFrame001
 * @tc.desc: test results of SetClipToFrame
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPropertiesTest, SetClipToFrame001, TestSize.Level1)
{
    RSProperties properties;
    properties.SetClipToFrame(true);
    EXPECT_EQ(properties.isDrawn_, true);
}

/**
 * @tc.name: GetBoundsRect001
 * @tc.desc: test results of GetBoundsRect
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPropertiesTest, GetBoundsRect001, TestSize.Level1)
{
    RSProperties properties;
    properties.GetBoundsRect();
    EXPECT_NE(properties.boundsGeo_, nullptr);

    properties.boundsGeo_->SetWidth(1.f);
    properties.boundsGeo_->SetHeight(1.f);
    properties.GetBoundsRect();
    EXPECT_NE(properties.boundsGeo_, nullptr);
}

/**
 * @tc.name: SetVisible001
 * @tc.desc: test results of SetVisible
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPropertiesTest, SetVisible001, TestSize.Level1)
{
    RSProperties properties;
    properties.SetVisible(false);
    EXPECT_NE(properties.visible_, true);
}

/**
 * @tc.name: GetInnerRRect001
 * @tc.desc: test results of GetInnerRRect
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPropertiesTest, GetInnerRRect001, TestSize.Level1)
{
    RSProperties properties;
    properties.boundsGeo_->SetWidth(1.f);
    properties.boundsGeo_->SetHeight(1.f);
    float x = -1.0;
    float y = -1.0;
    float z = -1.0;
    float w = 1.0;
    Vector4f cornerRadius(x, y, z, w);
    properties.SetCornerRadius(cornerRadius);
    Color color(1, 1, 1);
    Vector4<Color> borderColor = { color, color, color, color };
    properties.SetBorderColor(borderColor);
    properties.GetInnerRRect();
    EXPECT_NE(properties.border_, nullptr);
}

/**
 * @tc.name: GetDirtyRect001
 * @tc.desc: test results of GetDirtyRect
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPropertiesTest, GetDirtyRect001, TestSize.Level1)
{
    RSProperties properties;
    properties.GetDirtyRect();
    EXPECT_NE(properties.boundsGeo_, nullptr);

    properties.boundsGeo_->SetWidth(1.f);
    properties.boundsGeo_->SetHeight(1.f);
    properties.GetDirtyRect();
    EXPECT_EQ(properties.clipToBounds_, false);

    std::shared_ptr<RectF> rect = std::make_shared<RectF>(1.f, 1.f, 1.f, 1.f);
    properties.SetDrawRegion(rect);
    properties.GetDirtyRect();
    EXPECT_NE(properties.boundsGeo_, nullptr);
}

/**
 * @tc.name: GetDirtyRect002
 * @tc.desc: test results of GetDirtyRect
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPropertiesTest, GetDirtyRect002, TestSize.Level1)
{
    RSProperties properties;
    RectI drawRegion;
    properties.GetDirtyRect(drawRegion);
    EXPECT_NE(properties.boundsGeo_, nullptr);

    properties.boundsGeo_->SetWidth(1.f);
    properties.boundsGeo_->SetHeight(1.f);
    properties.GetDirtyRect(drawRegion);
    EXPECT_EQ(properties.clipToBounds_, false);

    std::shared_ptr<RectF> rect = std::make_shared<RectF>(1.f, 1.f, 1.f, 1.f);
    properties.SetDrawRegion(rect);
    properties.GetDirtyRect(drawRegion);
    EXPECT_EQ(properties.clipToBounds_, false);
}

/**
 * @tc.name: CheckEmptyBounds001
 * @tc.desc: test results of CheckEmptyBounds
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPropertiesTest, CheckEmptyBounds001, TestSize.Level1)
{
    RSProperties properties;
    properties.CheckEmptyBounds();
    EXPECT_EQ(properties.hasBounds_, false);
}

/**
 * @tc.name: SetMask001
 * @tc.desc: test results of SetMask
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPropertiesTest, SetMask001, TestSize.Level1)
{
    RSProperties properties;
    std::shared_ptr<RSMask> mask = std::make_shared<RSMask>();
    properties.SetMask(mask);
    EXPECT_EQ(properties.contentDirty_, true);
}

/**
 * @tc.name: SetLightUpEffect001
 * @tc.desc: test results of SetLightUpEffect
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPropertiesTest, SetLightUpEffect001, TestSize.Level1)
{
    RSProperties properties;
    properties.SetLightUpEffect(1.0f);
    EXPECT_NE(properties.isDrawn_, true);
}

/**
 * @tc.name: SetUseEffect001
 * @tc.desc: test results of SetUseEffect
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPropertiesTest, SetUseEffect001, TestSize.Level1)
{
    RSProperties properties;
    properties.SetUseEffect(true);
    EXPECT_EQ(properties.isDrawn_, true);
}

/**
 * @tc.name: SetUseShadowBatching001
 * @tc.desc: test results of SetUseShadowBatching
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPropertiesTest, SetUseShadowBatching001, TestSize.Level1)
{
    RSProperties properties;
    properties.SetUseShadowBatching(true);
    EXPECT_EQ(properties.isDrawn_, true);
}

/**
 * @tc.name: SetPixelStretch001
 * @tc.desc: test results of SetPixelStretch
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPropertiesTest, SetPixelStretch001, TestSize.Level1)
{
    RSProperties properties;
    float x = -1.0;
    float y = -1.0;
    float z = -1.0;
    float w = 1.0;
    Vector4f stretchSize(x, y, z, w);
    properties.SetPixelStretch(stretchSize);
    EXPECT_EQ(properties.pixelStretchNeedUpdate_, true);

    x = 1.0;
    y = 1.0;
    z = 1.0;
    w = -1.0;
    stretchSize = Vector4f(x, y, z, w);
    properties.SetPixelStretch(stretchSize);
    EXPECT_EQ(properties.contentDirty_, true);
}

/**
 * @tc.name: SetPixelStretchPercent001
 * @tc.desc: test results of SetPixelStretchPercent
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPropertiesTest, SetPixelStretchPercent001, TestSize.Level1)
{
    RSProperties properties;
    float x = -1.0;
    float y = -1.0;
    float z = -1.0;
    float w = 1.0;
    Vector4f stretchPercent(x, y, z, w);
    properties.SetPixelStretchPercent(stretchPercent);
    EXPECT_EQ(properties.pixelStretchNeedUpdate_, true);

    x = 1.0;
    y = 1.0;
    z = 1.0;
    w = -1.0;
    stretchPercent = Vector4f(x, y, z, w);
    properties.SetPixelStretchPercent(stretchPercent);
    EXPECT_EQ(properties.contentDirty_, true);
}

/**
 * @tc.name: SetLightIntensity001
 * @tc.desc: test results of SetLightIntensity
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPropertiesTest, SetLightIntensity001, TestSize.Level1)
{
    RSProperties properties;
    properties.SetLightIntensity(-1.f);
    EXPECT_NE(properties.lightSourcePtr_, nullptr);

    std::shared_ptr<RSRenderNode> node = std::make_shared<RSRenderNode>(1);
    properties.backref_ = node;
    properties.SetLightIntensity(1.f);
    EXPECT_EQ(properties.contentDirty_, true);

    properties.lightSourcePtr_->SetLightIntensity(1.f);
    properties.SetLightIntensity(0.f);
    EXPECT_NE(properties.lightSourcePtr_, nullptr);
}

/**
 * @tc.name: SetLightPosition001
 * @tc.desc: test results of SetLightPosition
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPropertiesTest, SetLightPosition001, TestSize.Level1)
{
    RSProperties properties;
    Vector4f lightPosition = {1.f, 1.f, 1.f, 1.f};
    properties.SetLightPosition(lightPosition);
    EXPECT_NE(properties.lightSourcePtr_, nullptr);
}

/**
 * @tc.name: SetIlluminatedBorderWidth001
 * @tc.desc: test results of SetIlluminatedBorderWidth
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPropertiesTest, SetIlluminatedBorderWidth001, TestSize.Level1)
{
    RSProperties properties;
    properties.SetIlluminatedBorderWidth(1.f);
    EXPECT_NE(properties.illuminatedPtr_, nullptr);
}

/**
 * @tc.name: SetIlluminatedType001
 * @tc.desc: test results of SetIlluminatedType
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPropertiesTest, SetIlluminatedType001, TestSize.Level1)
{
    RSProperties properties;
    properties.SetIlluminatedType(-1);
    EXPECT_NE(properties.illuminatedPtr_, nullptr);

    std::shared_ptr<RSRenderNode> node = std::make_shared<RSRenderNode>(1);
    properties.backref_ = node;
    properties.SetIlluminatedType(1);
    EXPECT_EQ(properties.isDrawn_, true);

    IlluminatedType illuminatedType = IlluminatedType::BORDER;
    properties.illuminatedPtr_->SetIlluminatedType(illuminatedType);
    properties.SetIlluminatedType(0);
    EXPECT_EQ(properties.contentDirty_, true);
}

/**
 * @tc.name: SetBloom001
 * @tc.desc: test results of SetBloom
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPropertiesTest, SetBloom001, TestSize.Level1)
{
    RSProperties properties;
    properties.SetBloom(1.f);
    EXPECT_NE(properties.illuminatedPtr_, nullptr);
}

/**
 * @tc.name: CalculateAbsLightPosition001
 * @tc.desc: test results of CalculateAbsLightPosition
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPropertiesTest, CalculateAbsLightPosition001, TestSize.Level1)
{
    RSProperties properties;
    float x = -1.0;
    float y = -1.0;
    float z = -1.0;
    float w = 1.0;
    Vector4f stretchSize(x, y, z, w);
    properties.SetLightPosition(stretchSize);
    properties.CalculateAbsLightPosition();

    ScreenRotation screenRotation = ScreenRotation::ROTATION_90;
    RSPointLightManager::Instance()->SetScreenRotation(screenRotation);
    properties.CalculateAbsLightPosition();

    screenRotation = ScreenRotation::ROTATION_180;
    RSPointLightManager::Instance()->SetScreenRotation(screenRotation);
    properties.CalculateAbsLightPosition();

    screenRotation = ScreenRotation::ROTATION_270;
    RSPointLightManager::Instance()->SetScreenRotation(screenRotation);
    properties.CalculateAbsLightPosition();

    screenRotation = ScreenRotation::INVALID_SCREEN_ROTATION;
    RSPointLightManager::Instance()->SetScreenRotation(screenRotation);
    properties.CalculateAbsLightPosition();
    EXPECT_TRUE(true);
}

/**
 * @tc.name: GenerateColorFilter001
 * @tc.desc: test results of GenerateColorFilter
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPropertiesTest, GenerateColorFilter001, TestSize.Level1)
{
    RSProperties properties;
    properties.GenerateColorFilter();
    EXPECT_EQ(properties.colorFilterNeedUpdate_, false);

    std::optional<float> rate = std::optional<float>(1.f);
    properties.SetGrayScale(rate);
    properties.GenerateColorFilter();
    EXPECT_EQ(properties.colorFilterNeedUpdate_, false);

    std::optional<float> grayScale = std::optional<float>(1.f);
    properties.SetGrayScale(grayScale);
    std::optional<float> brightness = std::optional<float>(2.f);
    properties.SetBrightness(brightness);
    std::optional<float> contrast = std::optional<float>(2.f);
    properties.SetContrast(contrast);
    std::optional<float> saturate = std::optional<float>(2.f);
    properties.SetSaturate(saturate);
    std::optional<float> sepia = std::optional<float>(1.f);
    properties.SetSepia(sepia);
    std::optional<float> aiInvert = std::optional<float>(1.f);
    properties.SetAiInvert(aiInvert);
    std::optional<float> hueRotate = std::optional<float>(1.f);
    properties.SetHueRotate(hueRotate);
    Color color(255, 0, 0);
    std::optional<Color> colorBlend = color;
    properties.SetColorBlend(colorBlend);
    properties.GenerateColorFilter();
    EXPECT_EQ(properties.colorFilterNeedUpdate_, false);
}

/**
 * @tc.name: CalculatePixelStretch001
 * @tc.desc: test results of CalculatePixelStretch
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPropertiesTest, CalculatePixelStretch001, TestSize.Level1)
{
    RSProperties properties;
    properties.CalculatePixelStretch();
    EXPECT_EQ(properties.pixelStretchNeedUpdate_, false);

    float x = -1.0;
    float y = -1.0;
    float z = -1.0;
    float w = 1.0;
    Vector4f stretchSize(x, y, z, w);
    EXPECT_EQ(properties.pixelStretchNeedUpdate_, false);

    x = -1.0;
    y = -1.0;
    z = -1.0;
    w = 1.0;
    Vector4f stretchPercent(x, y, z, w);
    properties.SetPixelStretchPercent(stretchPercent);
    properties.CalculatePixelStretch();
    EXPECT_EQ(properties.pixelStretchNeedUpdate_, false);

    properties.boundsGeo_->SetWidth(1.f);
    properties.boundsGeo_->SetHeight(1.f);
    properties.CalculatePixelStretch();
    EXPECT_EQ(properties.pixelStretchNeedUpdate_, false);
}

/**
 * @tc.name: CalculateFrameOffset001
 * @tc.desc: test results of CalculateFrameOffset
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPropertiesTest, CalculateFrameOffset001, TestSize.Level1)
{
    RSProperties properties;
    properties.frameGeo_->SetX(1.f);
    properties.frameGeo_->SetY(1.f);
    properties.boundsGeo_->SetX(1.f);
    properties.boundsGeo_->SetY(1.f);
    properties.CalculateFrameOffset();
    EXPECT_NE(properties.isDrawn_, true);
}

/**
 * @tc.name: CheckGreyCoef001
 * @tc.desc: test results of CheckGreyCoef
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPropertiesTest, CheckGreyCoef001, TestSize.Level1)
{
    RSProperties properties;
    properties.CheckGreyCoef();

    auto newVect = std::make_shared<Vector2f>(-1.f, -1.f);
    std::optional<Vector2f> parentPosition = *newVect;
    properties.SetGreyCoef(parentPosition);
    properties.CheckGreyCoef();
    EXPECT_EQ(properties.greyCoef_, std::nullopt);
}

/**
 * @tc.name: SetColorBlendMode001
 * @tc.desc: test results of SetColorBlendMode
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPropertiesTest, SetColorBlendMode001, TestSize.Level1)
{
    RSProperties properties;
    properties.SetColorBlendMode(1);
    EXPECT_EQ(properties.contentDirty_, true);
}

/**
 * @tc.name: SetPixelStretchTileMode001
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPropertiesTest, SetPixelStretchTileMode001, TestSize.Level1)
{
    RSProperties properties;

    properties.SetPixelStretchTileMode(static_cast<int>(Drawing::TileMode::CLAMP));
    properties.SetPixelStretchTileMode(static_cast<int>(Drawing::TileMode::DECAL));
    properties.SetPixelStretchTileMode(static_cast<int>(Drawing::TileMode::REPEAT));
    properties.SetPixelStretchTileMode(static_cast<int>(Drawing::TileMode::MIRROR));

    auto mode = properties.GetPixelStretchTileMode();
    ASSERT_EQ(static_cast<int>(Drawing::TileMode::MIRROR), mode);
}

/**
 * @tc.name: GetPixelStretchTileMode001
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPropertiesTest, GetPixelStretchTileMode001, TestSize.Level1)
{
    RSProperties properties;

    properties.SetPixelStretchTileMode(static_cast<int>(Drawing::TileMode::CLAMP));
    auto mode = properties.GetPixelStretchTileMode();
    ASSERT_EQ(static_cast<int>(Drawing::TileMode::CLAMP), mode);

    properties.SetPixelStretchTileMode(static_cast<int>(Drawing::TileMode::REPEAT));
    mode = properties.GetPixelStretchTileMode();
    ASSERT_EQ(static_cast<int>(Drawing::TileMode::REPEAT), mode);

    properties.SetPixelStretchTileMode(static_cast<int>(Drawing::TileMode::MIRROR));
    mode = properties.GetPixelStretchTileMode();
    ASSERT_EQ(static_cast<int>(Drawing::TileMode::MIRROR), mode);

    properties.SetPixelStretchTileMode(static_cast<int>(Drawing::TileMode::DECAL));
    mode = properties.GetPixelStretchTileMode();
    ASSERT_EQ(static_cast<int>(Drawing::TileMode::DECAL), mode);
}
} // namespace Rosen
} // namespace OHOS