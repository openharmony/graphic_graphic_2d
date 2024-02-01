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
 * @tc.desc: test
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
 * @tc.desc: test
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
 * @tc.desc: test
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
 * @tc.desc: test
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
 * @tc.desc: test
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
 * @tc.desc: test
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
 * @tc.desc: test
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
 * @tc.desc: test
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
 * @tc.desc: test
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
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPropertiesTest, SetShadowOffsetY001, TestSize.Level1)
{
    RSProperties properties;
    float offsetY = 0.1f;
    properties.SetShadowOffsetX(offsetY);
    offsetY = 1.0f;
    properties.SetShadowOffsetX(offsetY);
}

/**
 * @tc.name: SetShadowElevation001
 * @tc.desc: test
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
 * @tc.desc: test
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
 * @tc.desc: test
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
 * @tc.desc: test
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
 * @tc.desc: test
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
 * @tc.desc: test
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
 * @tc.desc: test
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
 * @tc.desc: test
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
 * @tc.desc: test
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
 * @tc.desc: test
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
 * @tc.desc: test
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
 * @tc.desc: test
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
 * @tc.desc: test
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
 * @tc.desc: test
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
 * @tc.desc: test
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
 * @tc.desc: test
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
 * @tc.desc: test
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
 * @tc.desc: test
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
 * @tc.desc: test
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
 * @tc.desc: test
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
 * @tc.desc: test
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
} // namespace Rosen
} // namespace OHOS