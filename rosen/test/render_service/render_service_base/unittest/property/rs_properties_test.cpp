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
    EXPECT_EQ(properties.GetSublayerTransform().value(), sublayerTransform);
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
    EXPECT_EQ(properties.GetBackgroundShader(), nullptr);
}

/**
 * @tc.name: SetBgImageInnerRect001
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPropertiesTest, SetBgImageInnerRect001, TestSize.Level1)
{
    Vector4f rect{1.f, 1.f, 1.f, 1.f};
    RSProperties properties;
    properties.SetBgImageInnerRect(rect);
    EXPECT_EQ(properties.GetBgImageInnerRect(), rect);
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
 * @tc.name: GetShadow001
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPropertiesTest, GetShadow001, TestSize.Level1)
{
    RSProperties properties;
    EXPECT_FALSE(properties.GetShadow().has_value());
    EXPECT_FALSE(properties.IsShadowValid());
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
    EXPECT_EQ(properties.GetPerspX(), 1.f);
    properties.SetPerspY(1.f);
    EXPECT_EQ(properties.GetPerspY(), 1.f);
    Vector2f vec(1.f, 1.f);
    EXPECT_EQ(properties.GetPersp(), vec);
    Color color1;
    properties.SetForegroundColor(color1);
    properties.SetBackgroundColor(color1);
    std::shared_ptr<RSImage> image = std::make_shared<RSImage>();
    properties.SetBgImage(image);
    properties.SetShadowColor(color1);
    EXPECT_EQ(properties.GetShadowColor(), color1);
    properties.SetShadowOffsetX(1.f);
    EXPECT_EQ(properties.GetShadowOffsetX(), 1.f);
    properties.SetShadowOffsetY(1.f);
    EXPECT_EQ(properties.GetShadowOffsetY(), 1.f);
    properties.SetShadowAlpha(1.f);
    EXPECT_EQ(properties.GetShadowAlpha(), 1.f);
    properties.SetShadowElevation(1.f);
    EXPECT_EQ(properties.GetShadowElevation(), 1.f);
    properties.SetShadowRadius(1.f);
    EXPECT_EQ(properties.GetShadowRadius(), 1.f);
    properties.Dump();
}

#if defined(NEW_SKIA) && (defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK))
/**
 * @tc.name: CreateFilterCacheManagerIfNeed001
 * @tc.desc: test results of CreateFilterCacheManagerIfNeed
 * @tc.type: FUNC
 * @tc.require: issueI9QKVM
 */
HWTEST_F(RSPropertiesTest, CreateFilterCacheManagerIfNeed001, TestSize.Level1)
{
    RSProperties properties;
    properties.CreateFilterCacheManagerIfNeed();

    properties.backgroundFilter_ = std::make_shared<RSFilter>();
    properties.CreateFilterCacheManagerIfNeed();

    properties.filter_ = std::make_shared<RSFilter>();
    properties.CreateFilterCacheManagerIfNeed();
    EXPECT_TRUE(properties.filter_ != nullptr);
}

/**
 * @tc.name: ClearFilterCache001
 * @tc.desc: test results of ClearFilterCache
 * @tc.type: FUNC
 * @tc.require: issueI9QKVM
 */
HWTEST_F(RSPropertiesTest, ClearFilterCache001, TestSize.Level1)
{
    RSProperties properties;
    properties.foregroundFilterCacheManager_ = std::make_unique<RSFilterCacheManager>();
    properties.ClearFilterCache();

    properties.backgroundFilterCacheManager_ = std::make_unique<RSFilterCacheManager>();
    properties.ClearFilterCache();
    EXPECT_TRUE(properties.backgroundFilterCacheManager_ != nullptr);
}
#endif

/**
 * @tc.name: OnApplyModifiers001
 * @tc.desc: test results of OnApplyModifiers
 * @tc.type: FUNC
 * @tc.require: issueI9QKVM
 */
HWTEST_F(RSPropertiesTest, OnApplyModifiers001, TestSize.Level1)
{
    RSProperties properties;
    properties.OnApplyModifiers();
    EXPECT_EQ(properties.GetPixelStretch(), std::nullopt);

    properties.geoDirty_ = true;
    properties.OnApplyModifiers();
    EXPECT_TRUE(properties.geoDirty_);

    properties.hasBounds_ = true;
    properties.OnApplyModifiers();
    EXPECT_TRUE(properties.hasBounds_);

    properties.clipToFrame_ = true;
    properties.OnApplyModifiers();
    EXPECT_TRUE(properties.clipToFrame_);

    properties.clipToBounds_ = true;
    properties.OnApplyModifiers();
    EXPECT_TRUE(properties.clipToBounds_);

    properties.colorFilterNeedUpdate_ = true;
    properties.OnApplyModifiers();
    EXPECT_TRUE(!properties.colorFilterNeedUpdate_);

    properties.pixelStretchNeedUpdate_ = true;
    properties.OnApplyModifiers();
    EXPECT_TRUE(!properties.pixelStretchNeedUpdate_);

    properties.geoDirty_ = false;
    properties.OnApplyModifiers();
    EXPECT_TRUE(!properties.geoDirty_);

    properties.greyCoefNeedUpdate_ = true;
    properties.OnApplyModifiers();
    EXPECT_TRUE(!properties.greyCoefNeedUpdate_);

    properties.shadow_ = std::make_optional<RSShadow>();
    properties.shadow_->colorStrategy_ = SHADOW_COLOR_STRATEGY::COLOR_STRATEGY_MAIN;
    properties.backgroundFilter_ = std::make_shared<RSFilter>();
    properties.filter_ = std::make_shared<RSFilter>();
    properties.foregroundEffectRadius_ = 1.f;
    Vector2f scaleAnchor = Vector2f(0.f, 0.f);
    properties.motionBlurPara_ = std::make_shared<MotionBlurParam>(1.f, scaleAnchor);
    properties.OnApplyModifiers();
    EXPECT_TRUE(properties.filter_ != nullptr);
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
 * @tc.name: SetBoundsSize001
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPropertiesTest, SetBoundsSize001, TestSize.Level1)
{
    RSProperties properties;
    Vector2f size{1.0f, 1.0f};
    properties.SetBoundsSize(size);
}

/**
 * @tc.name: SetFrameSize001
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPropertiesTest, SetFrameSize001, TestSize.Level1)
{
    RSProperties properties;
    Vector2f size{1.0f, 1.0f};
    properties.SetFrameSize(size);
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
 * @tc.name: GetFrame002
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPropertiesTest, GetFrame002, TestSize.Level1)
{
    RSProperties properties;
    Vector4f frame{1.0f, 1.0f, 1.0f, 1.0f};

    properties.SetFrame(frame);
    EXPECT_EQ(properties.GetFrameOffsetX(), 0);
    EXPECT_EQ(properties.GetFrameOffsetY(), 0);

    properties.GetBoundsGeometry();
    properties.GetFrameGeometry();
}

/**
 * @tc.name: UpdateGeometryByParent001
 * @tc.desc: test results of UpdateGeometryByParent
 * @tc.type: FUNC
 * @tc.require: issueI9QKVM
 */
HWTEST_F(RSPropertiesTest, UpdateGeometryByParent001, TestSize.Level1)
{
    RSProperties properties;
    Drawing::Matrix* parentMatrix = new Drawing::Matrix();
    Drawing::Point point;
    std::optional<Drawing::Point> offset = point;
    bool res = properties.UpdateGeometryByParent(parentMatrix, offset);
    EXPECT_TRUE(res);

    RectI rect;
    properties.lastRect_ = rect;
    res = properties.UpdateGeometryByParent(parentMatrix, offset);
    EXPECT_TRUE(!res);
    delete parentMatrix;
    parentMatrix = nullptr;
}

/**
 * @tc.name: SandBox001
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPropertiesTest, SandBox001, TestSize.Level1)
{
    RSProperties properties;
    Vector2f sandbox{1.0f, 1.0f};

    properties.SetSandBox(sandbox);

    ASSERT_TRUE(properties.GetSandBox().has_value());
    EXPECT_EQ(properties.GetSandBox().value(), sandbox);
    properties.ResetSandBox();

    std::optional<Drawing::Matrix> mat;
    properties.UpdateSandBoxMatrix(mat);
    EXPECT_EQ(properties.GetSandBoxMatrix(), std::nullopt);
}

/**
 * @tc.name: SetNGetQuaternion001
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPropertiesTest, SetNGetQuaternion001, TestSize.Level1)
{
    RSProperties properties;
    Quaternion quaternion;
    properties.SetQuaternion(quaternion);
    EXPECT_EQ(properties.GetQuaternion(), quaternion);
}

/**
 * @tc.name: SetNGetCameraDistance001
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPropertiesTest, SetNGetCameraDistance001, TestSize.Level1)
{
    RSProperties properties;
    float cameraDistance = 1.f;
    properties.SetCameraDistance(cameraDistance);
    EXPECT_EQ(properties.GetCameraDistance(), cameraDistance);
}

/**
 * @tc.name: SetParticles001
 * @tc.desc: test results of SetParticles
 * @tc.type: FUNC
 * @tc.require: issueI9QKVM
 */
HWTEST_F(RSPropertiesTest, SetParticles001, TestSize.Level1)
{
    RSProperties properties;
    RSRenderParticleVector particles;
    auto particleParams = std::make_shared<ParticleRenderParams>();
    std::shared_ptr<RSRenderParticle> newv = std::make_shared<RSRenderParticle>(particleParams);
    properties.particles_.renderParticleVector_.push_back(newv);
    properties.SetParticles(particles);
    EXPECT_EQ(properties.particles_.renderParticleVector_.size(), 0);
}

/**
 * @tc.name: SetBorderStyle001
 * @tc.desc: test results of SetBorderStyle
 * @tc.type: FUNC
 * @tc.require: issueI9QKVM
 */
HWTEST_F(RSPropertiesTest, SetBorderStyle001, TestSize.Level1)
{
    RSProperties properties;
    Vector4<uint32_t> style = { 1, 1, 1, 1 };
    properties.border_ = nullptr;
    properties.SetBorderStyle(style);
    EXPECT_NE(properties.border_, nullptr);
}

/**
 * @tc.name: GetBorderColorIsTransparent001
 * @tc.desc: test results of GetBorderColorIsTransparent
 * @tc.type: FUNC
 * @tc.require: issueI9QKVM
 */
HWTEST_F(RSPropertiesTest, GetBorderColorIsTransparent001, TestSize.Level1)
{
    RSProperties properties;
    properties.border_ = std::make_shared<RSBorder>();
    bool res = properties.GetBorderColorIsTransparent();
    EXPECT_NE(res, false);

    properties.border_ = nullptr;
    res = properties.GetBorderColorIsTransparent();
    EXPECT_NE(res, true);
}

/**
 * @tc.name: SetOutlineWidth001
 * @tc.desc: test results of SetOutlineWidth
 * @tc.type: FUNC
 * @tc.require: issueI9QKVM
 */
HWTEST_F(RSPropertiesTest, SetOutlineWidth001, TestSize.Level1)
{
    RSProperties properties;
    Vector4f width = { 1.f, 1.f, 1.f, 1.f };
    properties.outline_ = std::make_shared<RSBorder>(true);
    properties.SetOutlineWidth(width);
    EXPECT_NE(properties.outline_, nullptr);

    properties.outline_ = nullptr;
    properties.SetOutlineWidth(width);
    EXPECT_NE(properties.outline_, nullptr);
}

/**
 * @tc.name: SetOutlineStyle001
 * @tc.desc: test results of SetOutlineStyle
 * @tc.type: FUNC
 * @tc.require: issueI9QKVM
 */
HWTEST_F(RSPropertiesTest, SetOutlineStyle001, TestSize.Level1)
{
    RSProperties properties;
    Vector4<uint32_t> style = { 1, 1, 1, 1 };
    properties.outline_ = nullptr;
    properties.SetOutlineStyle(style);
    EXPECT_NE(properties.outline_, nullptr);
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
    ASSERT_EQ(properties.GetParticles(), particles2);

    properties.SetSkewX(1.0);
    properties.SetSkewY(1.0);
    Vector2f skew2 = { 1.0, 1.0 };
    properties.SetSkew(skew2);
    auto skew = properties.GetSkew();
    ASSERT_NE(0, skew.GetLength());
    auto skewX = properties.GetSkewX();
    auto skewY = properties.GetSkewY();
    ASSERT_NE(0, skewX);
    ASSERT_NE(0, skewY);

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
    EXPECT_EQ(properties.GetOutlineColor(), outLineColor);
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
 * @tc.name: SetEmitterUpdater001
 * @tc.desc: test results of SetEmitterUpdater
 * @tc.type: FUNC
 * @tc.require: issueI9QKVM
 */
HWTEST_F(RSPropertiesTest, SetEmitterUpdater001, TestSize.Level1)
{
    RSProperties properties;
    std::vector<std::shared_ptr<EmitterUpdater>> para;
    properties.SetEmitterUpdater(para);
    EXPECT_EQ(properties.emitterUpdater_.empty(), true);

    auto emitter = std::make_shared<EmitterUpdater>(0);
    para.push_back(emitter);
    properties.SetEmitterUpdater(para);
    EXPECT_EQ(properties.emitterUpdater_.empty(), false);

    std::shared_ptr<RSRenderNode> node = std::make_shared<RSRenderNode>(1);
    properties.backref_ = node;
    properties.SetEmitterUpdater(para);
    EXPECT_EQ(properties.emitterUpdater_.empty(), false);
}

/**
 * @tc.name: SetParticleNoiseFields001
 * @tc.desc: test results of SetParticleNoiseFields
 * @tc.type: FUNC
 * @tc.require: issueI9QKVM
 */
HWTEST_F(RSPropertiesTest, SetParticleNoiseFields001, TestSize.Level1)
{
    RSProperties properties;
    std::shared_ptr<ParticleNoiseFields> para = nullptr;
    properties.SetParticleNoiseFields(para);
    EXPECT_EQ(para, nullptr);

    para = std::make_shared<ParticleNoiseFields>();
    properties.SetParticleNoiseFields(para);
    EXPECT_NE(para, nullptr);

    std::shared_ptr<RSRenderNode> node = std::make_shared<RSRenderNode>(1);
    properties.backref_ = node;
    properties.SetParticleNoiseFields(para);
    EXPECT_NE(para, nullptr);
}

/**
 * @tc.name: SetNGetForegroundEffectRadius001
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPropertiesTest, SetNGetForegroundEffectRadius001, TestSize.Level1)
{
    RSProperties properties;
    properties.SetForegroundEffectRadius(1.f);
    EXPECT_EQ(properties.GetForegroundEffectRadius(), 1.f);
    EXPECT_TRUE(properties.IsForegroundEffectRadiusValid());
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

    properties.sandbox_ = std::make_unique<Sandbox>();
    properties.SetFramePositionY(1.0f);
    properties.lightSourcePtr_ = std::make_shared<RSLightSource>();
    properties.lightSourcePtr_->intensity_ = 1.f;
    properties.illuminatedPtr_ = std::make_shared<RSIlluminated>();
    properties.illuminatedPtr_->illuminatedType_ = IlluminatedType::BLOOM_BORDER;
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
    EXPECT_EQ(properties.GetBackgroundFilter(), backgroundFilter);
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
    EXPECT_EQ(properties.GetLinearGradientBlurPara(), para);
    properties.IfLinearGradientBlurInvalid();
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
    ASSERT_TRUE(properties.GetDynamicLightUpRate().has_value());
    EXPECT_EQ(properties.GetDynamicLightUpRate().value(), rate.value());
    EXPECT_FALSE(properties.IsDynamicLightUpValid());
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
    ASSERT_TRUE(properties.GetDynamicLightUpDegree().has_value());
    EXPECT_EQ(properties.GetDynamicLightUpDegree().value(), lightUpDegree.value());
}

/**
 * @tc.name: SetFgBrightnessParams001
 * @tc.desc: test results of SetFgBrightnessParams
 * @tc.type: FUNC
 * @tc.require: issueI9QKVM
 */
HWTEST_F(RSPropertiesTest, SetFgBrightnessParams001, TestSize.Level1)
{
    RSProperties properties;
    RSDynamicBrightnessPara para;
    std::optional<RSDynamicBrightnessPara> params;
    properties.SetFgBrightnessParams(params);
    EXPECT_EQ(params.has_value(), false);

    params = para;
    properties.SetFgBrightnessParams(params);
    EXPECT_EQ(params.has_value(), true);
}

/**
 * @tc.name: SetBgBrightnessParams001
 * @tc.desc: test results of SetBgBrightnessParams
 * @tc.type: FUNC
 * @tc.require: issueI9QKVM
 */
HWTEST_F(RSPropertiesTest, SetBgBrightnessParams001, TestSize.Level1)
{
    RSProperties properties;
    RSDynamicBrightnessPara para;
    std::optional<RSDynamicBrightnessPara> params;
    properties.SetBgBrightnessParams(params);
    EXPECT_EQ(params.has_value(), false);

    params = para;
    properties.SetBgBrightnessParams(params);
    EXPECT_EQ(params.has_value(), true);
}

/**
 * @tc.name: SetGreyCoef001
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPropertiesTest, SetGreyCoef001, TestSize.Level1)
{
    RSProperties properties;
    std::optional<Vector2f> greyCoef({1.f, 1.f});
    properties.SetGreyCoef(greyCoef);
    ASSERT_TRUE(properties.GetGreyCoef().has_value());
    EXPECT_EQ(properties.GetGreyCoef().value(), greyCoef.value());
}

/**
 * @tc.name: SetNGetDynamicDimDegree001
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPropertiesTest, SetNGetDynamicDimDegree001, TestSize.Level1)
{
    RSProperties properties;
    std::optional<float> dimDegree(1.f);
    properties.SetDynamicDimDegree(dimDegree);
    ASSERT_TRUE(properties.GetDynamicDimDegree().has_value());
    EXPECT_EQ(properties.GetDynamicDimDegree().value(), dimDegree.value());
    EXPECT_TRUE(properties.IsDynamicDimValid());
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
    EXPECT_EQ(properties.GetFilter(), filter);
}

/**
 * @tc.name: SetMotionBlurPara001
 * @tc.desc: test results of SetMotionBlurPara
 * @tc.type: FUNC
 * @tc.require: issueI9QKVM
 */
HWTEST_F(RSPropertiesTest, SetMotionBlurPara001, TestSize.Level1)
{
    RSProperties properties;
    std::shared_ptr<MotionBlurParam> para;
    properties.SetMotionBlurPara(para);
    EXPECT_EQ(para, nullptr);

    Vector2f scaleAnchor = Vector2f(0.f, 0.f);
    para = std::make_shared<MotionBlurParam>(0.f, scaleAnchor);
    properties.SetMotionBlurPara(para);
    EXPECT_NE(para, nullptr);

    para->radius = 1.f;
    properties.SetMotionBlurPara(para);
    EXPECT_NE(para, nullptr);
}

/**
 * @tc.name: SetForegroundFilter001
 * @tc.desc: test results of SetForegroundFilter
 * @tc.type: FUNC
 * @tc.require: issueI9QKVM
 */
HWTEST_F(RSPropertiesTest, SetForegroundFilter001, TestSize.Level1)
{
    RSProperties properties;
    std::shared_ptr<RSFilter> foregroundFilter;
    properties.SetForegroundFilter(foregroundFilter);
    EXPECT_EQ(foregroundFilter, nullptr);

    foregroundFilter = std::make_shared<RSFilter>();
    properties.SetForegroundFilter(foregroundFilter);
    EXPECT_NE(foregroundFilter, nullptr);
}

/**
 * @tc.name: SetShadowAlpha001
 * @tc.desc: test results of SetShadowAlpha
 * @tc.type: FUNC
 * @tc.require: issueI9QKVM
 */
HWTEST_F(RSPropertiesTest, SetShadowAlpha001, TestSize.Level1)
{
    RSProperties properties;
    float alpha = 0.f;
    properties.SetShadowAlpha(alpha);
    EXPECT_EQ(alpha, 0.f);

    alpha = 1.f;
    properties.shadow_ = std::make_optional<RSShadow>();
    properties.shadow_->elevation_ = 1.f;
    properties.shadow_->color_.alpha_ = 255;
    properties.shadow_->radius_ = 1.f;
    properties.SetShadowAlpha(alpha);
    EXPECT_NE(alpha, 0.f);
}

/**
 * @tc.name: GetLocalBoundsAndFramesRect001
 * @tc.desc: test results of GetLocalBoundsAndFramesRect
 * @tc.type: FUNC
 * @tc.require: issueI9QKVM
 */
HWTEST_F(RSPropertiesTest, GetLocalBoundsAndFramesRect001, TestSize.Level1)
{
    RSProperties properties;
    properties.GetLocalBoundsAndFramesRect();
    EXPECT_TRUE(!properties.clipToBounds_);

    properties.frameGeo_->width_ = 1.f;
    properties.GetLocalBoundsAndFramesRect();
    EXPECT_TRUE(!properties.clipToBounds_);

    properties.frameGeo_->height_ = 1.f;
    properties.GetLocalBoundsAndFramesRect();
    EXPECT_TRUE(!properties.clipToBounds_);

    properties.clipToBounds_ = true;
    properties.GetLocalBoundsAndFramesRect();
    EXPECT_TRUE(properties.clipToBounds_);
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
    EXPECT_EQ(properties.GetShadowPath(), shadowPath);
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
 * @tc.name: SetShadowColorStrategy002
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPropertiesTest, SetShadowColorStrategy002, TestSize.Level1)
{
    RSProperties properties;
    int shadowColorStrategy = SHADOW_COLOR_STRATEGY::COLOR_STRATEGY_AVERAGE;
    properties.SetShadowColorStrategy(shadowColorStrategy);
    EXPECT_EQ(properties.GetShadowColorStrategy(), shadowColorStrategy);

    shadowColorStrategy = SHADOW_COLOR_STRATEGY::COLOR_STRATEGY_MAIN;
    properties.SetShadowColorStrategy(shadowColorStrategy);
    EXPECT_EQ(properties.GetShadowColorStrategy(), shadowColorStrategy);
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
    EXPECT_EQ(properties.GetFrameGravity(), gravity);
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
    EXPECT_EQ(properties.GetClipRRect(), clipRRect);
    EXPECT_TRUE(properties.GetClipToRRect());
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
    ASSERT_TRUE(properties.GetClipToFrame() == true);
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
 * @tc.name: GetFrameRect001
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPropertiesTest, GetFrameRect001, TestSize.Level1)
{
    RSProperties properties;
    RectF rect(0, 0, properties.GetFrameWidth(), properties.GetFrameHeight());
    properties.GetFrameRect();
}

/**
 * @tc.name: GetBgImageRect001
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPropertiesTest, GetBgImageRect001, TestSize.Level1)
{
    RSProperties properties;
    EXPECT_EQ(properties.GetBgImageRect(), RectF());
}

/**
 * @tc.name: SetNGetVisible001
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPropertiesTest, SetNGetVisible001, TestSize.Level1)
{
    RSProperties properties;
    properties.SetVisible(true);
    EXPECT_EQ(properties.GetVisible(), true);
}

/**
 * @tc.name: GenerateNGetRRect001
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPropertiesTest, GenerateNGetRRect001, TestSize.Level1)
{
    RSProperties properties;
    properties.GenerateRRect();
    properties.GetRRect();
}

/**
 * @tc.name: NeedFilterNClip001
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPropertiesTest, NeedFilterNClip001, TestSize.Level1)
{
    RSProperties properties;
    EXPECT_FALSE(properties.NeedFilter());
    EXPECT_FALSE(properties.NeedClip());
}

/**
 * @tc.name: Dirty001
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPropertiesTest, Dirty001, TestSize.Level1)
{
    RSProperties properties;
    properties.SetDirty();
    EXPECT_TRUE(properties.IsDirty());
    properties.ResetDirty();
    EXPECT_FALSE(properties.IsDirty());
    EXPECT_FALSE(properties.IsGeoDirty());
    EXPECT_FALSE(properties.IsContentDirty());
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
    EXPECT_NE(properties.frameGeo_, nullptr);

    properties.frameGeo_->SetWidth(1.f);
    properties.GetDirtyRect();
    EXPECT_EQ(properties.clipToBounds_, false);

    properties.frameGeo_->SetHeight(1.f);
    properties.GetDirtyRect();
    EXPECT_EQ(properties.clipToBounds_, false);

    properties.clipToBounds_ = true;
    properties.GetDirtyRect();
    EXPECT_EQ(properties.clipToBounds_, true);

    std::shared_ptr<RectF> rect = std::make_shared<RectF>(1.f, 1.f, 1.f, 1.f);
    properties.SetDrawRegion(rect);
    properties.GetDirtyRect();
    EXPECT_NE(properties.frameGeo_, nullptr);
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

    properties.frameGeo_->SetWidth(1.f);
    properties.GetDirtyRect(drawRegion);
    EXPECT_EQ(properties.clipToBounds_, false);

    properties.frameGeo_->SetHeight(1.f);
    properties.GetDirtyRect(drawRegion);
    EXPECT_EQ(properties.clipToBounds_, false);

    properties.clipToBounds_ = true;
    properties.GetDirtyRect(drawRegion);
    EXPECT_EQ(properties.clipToBounds_, true);

    std::shared_ptr<RectF> rect = std::make_shared<RectF>(1.f, 1.f, 1.f, 1.f);
    properties.SetDrawRegion(rect);
    properties.GetDirtyRect(drawRegion);
    EXPECT_EQ(properties.clipToBounds_, true);
}

/**
 * @tc.name: SetBackgroundBlurRadius001
 * @tc.desc: test results of SetBackgroundBlurRadius
 * @tc.type: FUNC
 * @tc.require: issueI9QKVM
 */
HWTEST_F(RSPropertiesTest, SetBackgroundBlurRadius001, TestSize.Level1)
{
    RSProperties properties;
    float backgroundBlurRadius = 0.f;
    properties.SetBackgroundBlurRadius(backgroundBlurRadius);
    EXPECT_NE(backgroundBlurRadius, 1.f);

    backgroundBlurRadius = 2.f;
    properties.SetBackgroundBlurRadius(backgroundBlurRadius);
    EXPECT_NE(backgroundBlurRadius, 1.f);
}

/**
 * @tc.name: SetBackgroundBlurSaturation001
 * @tc.desc: test results of SetBackgroundBlurSaturation
 * @tc.type: FUNC
 * @tc.require: issueI9QKVM
 */
HWTEST_F(RSPropertiesTest, SetBackgroundBlurSaturation001, TestSize.Level1)
{
    RSProperties properties;
    float backgroundBlurRadius = 1.f;
    properties.SetBackgroundBlurSaturation(backgroundBlurRadius);
    EXPECT_NE(backgroundBlurRadius, 0.f);

    backgroundBlurRadius = 0.f;
    properties.SetBackgroundBlurSaturation(backgroundBlurRadius);
    EXPECT_NE(backgroundBlurRadius, 1.f);

    backgroundBlurRadius = -2.f;
    properties.SetBackgroundBlurSaturation(backgroundBlurRadius);
    EXPECT_NE(backgroundBlurRadius, 1.f);
}

/**
 * @tc.name: SetBackgroundBlurBrightness001
 * @tc.desc: test results of SetBackgroundBlurBrightness
 * @tc.type: FUNC
 * @tc.require: issueI9QKVM
 */
HWTEST_F(RSPropertiesTest, SetBackgroundBlurBrightness001, TestSize.Level1)
{
    RSProperties properties;
    float backgroundBlurBrightness = 1.f;
    properties.SetBackgroundBlurBrightness(backgroundBlurBrightness);
    EXPECT_NE(backgroundBlurBrightness, 0.f);

    backgroundBlurBrightness = 0.f;
    properties.SetBackgroundBlurBrightness(backgroundBlurBrightness);
    EXPECT_NE(backgroundBlurBrightness, 1.f);

    backgroundBlurBrightness = -2.f;
    properties.SetBackgroundBlurBrightness(backgroundBlurBrightness);
    EXPECT_NE(backgroundBlurBrightness, 1.f);
}

/**
 * @tc.name: SetBackgroundBlurMaskColor001
 * @tc.desc: test results of SetBackgroundBlurMaskColor
 * @tc.type: FUNC
 * @tc.require: issueI9QKVM
 */
HWTEST_F(RSPropertiesTest, SetBackgroundBlurMaskColor001, TestSize.Level1)
{
    RSProperties properties;
    Color backgroundMaskColor;
    properties.SetBackgroundBlurMaskColor(backgroundMaskColor);
    EXPECT_EQ(backgroundMaskColor.GetAlpha(), 0);

    backgroundMaskColor.SetAlpha(255);
    properties.SetBackgroundBlurMaskColor(backgroundMaskColor);
    EXPECT_NE(backgroundMaskColor.GetAlpha(), 0);
}

/**
 * @tc.name: SetBackgroundBlurRadiusX001
 * @tc.desc: test results of SetBackgroundBlurRadiusX
 * @tc.type: FUNC
 * @tc.require: issueI9QKVM
 */
HWTEST_F(RSPropertiesTest, SetBackgroundBlurRadiusX001, TestSize.Level1)
{
    RSProperties properties;
    float backgroundBlurRadiusX = 0.f;
    properties.SetBackgroundBlurRadiusX(backgroundBlurRadiusX);
    EXPECT_EQ(backgroundBlurRadiusX, 0.f);

    backgroundBlurRadiusX = 2.f;
    properties.SetBackgroundBlurRadiusX(backgroundBlurRadiusX);
    EXPECT_NE(backgroundBlurRadiusX, 0.f);
}

/**
 * @tc.name: SetBackgroundBlurRadiusY001
 * @tc.desc: test results of SetBackgroundBlurRadiusY
 * @tc.type: FUNC
 * @tc.require: issueI9QKVM
 */
HWTEST_F(RSPropertiesTest, SetBackgroundBlurRadiusY001, TestSize.Level1)
{
    RSProperties properties;
    float backgroundBlurRadiusY = 0.f;
    properties.SetBackgroundBlurRadiusY(backgroundBlurRadiusY);
    EXPECT_EQ(backgroundBlurRadiusY, 0.f);

    backgroundBlurRadiusY = 2.f;
    properties.SetBackgroundBlurRadiusY(backgroundBlurRadiusY);
    EXPECT_NE(backgroundBlurRadiusY, 0.f);
}

/**
 * @tc.name: SetForegroundBlurRadius001
 * @tc.desc: test results of SetForegroundBlurRadius
 * @tc.type: FUNC
 * @tc.require: issueI9QKVM
 */
HWTEST_F(RSPropertiesTest, SetForegroundBlurRadius001, TestSize.Level1)
{
    RSProperties properties;
    float foregroundBlurRadius = 0.f;
    properties.SetForegroundBlurRadius(foregroundBlurRadius);
    EXPECT_EQ(foregroundBlurRadius, 0.f);

    foregroundBlurRadius = 2.f;
    properties.SetForegroundBlurRadius(foregroundBlurRadius);
    EXPECT_NE(foregroundBlurRadius, 0.f);
}

/**
 * @tc.name: SetForegroundBlurSaturation001
 * @tc.desc: test results of SetForegroundBlurSaturation
 * @tc.type: FUNC
 * @tc.require: issueI9QKVM
 */
HWTEST_F(RSPropertiesTest, SetForegroundBlurSaturation001, TestSize.Level1)
{
    RSProperties properties;
    float foregroundBlurSaturation = -1.f;
    properties.SetForegroundBlurSaturation(foregroundBlurSaturation);
    EXPECT_EQ(foregroundBlurSaturation, -1.f);

    foregroundBlurSaturation = 2.f;
    properties.SetForegroundBlurSaturation(foregroundBlurSaturation);
    EXPECT_NE(foregroundBlurSaturation, 0.f);
}

/**
 * @tc.name: SetForegroundBlurBrightness001
 * @tc.desc: test results of SetForegroundBlurBrightness
 * @tc.type: FUNC
 * @tc.require: issueI9QKVM
 */
HWTEST_F(RSPropertiesTest, SetForegroundBlurBrightness001, TestSize.Level1)
{
    RSProperties properties;
    float foregroundBlurBrightness = -1.f;
    properties.SetForegroundBlurBrightness(foregroundBlurBrightness);
    EXPECT_EQ(foregroundBlurBrightness, -1.f);

    foregroundBlurBrightness = 2.f;
    properties.SetForegroundBlurBrightness(foregroundBlurBrightness);
    EXPECT_NE(foregroundBlurBrightness, 0.f);
}

/**
 * @tc.name: SetForegroundBlurMaskColor001
 * @tc.desc: test results of SetForegroundBlurMaskColor
 * @tc.type: FUNC
 * @tc.require: issueI9QKVM
 */
HWTEST_F(RSPropertiesTest, SetForegroundBlurMaskColor001, TestSize.Level1)
{
    RSProperties properties;
    Color foregroundMaskColor;
    properties.SetForegroundBlurMaskColor(foregroundMaskColor);
    EXPECT_EQ(foregroundMaskColor.GetAlpha(), 0);

    foregroundMaskColor.SetAlpha(255);
    properties.SetForegroundBlurMaskColor(foregroundMaskColor);
    EXPECT_NE(foregroundMaskColor.GetAlpha(), 0);
}

/**
 * @tc.name: SetForegroundBlurRadiusX001
 * @tc.desc: test results of SetForegroundBlurRadiusX
 * @tc.type: FUNC
 * @tc.require: issueI9QKVM
 */
HWTEST_F(RSPropertiesTest, SetForegroundBlurRadiusX001, TestSize.Level1)
{
    RSProperties properties;
    float foregroundBlurRadiusX = -1.f;
    properties.SetForegroundBlurRadiusX(foregroundBlurRadiusX);
    EXPECT_EQ(foregroundBlurRadiusX, -1.f);

    foregroundBlurRadiusX = 2.f;
    properties.SetForegroundBlurRadiusX(foregroundBlurRadiusX);
    EXPECT_NE(foregroundBlurRadiusX, 0.f);
}

/**
 * @tc.name: SetForegroundBlurRadiusY001
 * @tc.desc: test results of SetForegroundBlurRadiusY
 * @tc.type: FUNC
 * @tc.require: issueI9QKVM
 */
HWTEST_F(RSPropertiesTest, SetForegroundBlurRadiusY001, TestSize.Level1)
{
    RSProperties properties;
    float foregroundBlurRadiusY = -1.f;
    properties.SetForegroundBlurRadiusY(foregroundBlurRadiusY);
    EXPECT_EQ(foregroundBlurRadiusY, -1.f);

    foregroundBlurRadiusY = 2.f;
    properties.SetForegroundBlurRadiusY(foregroundBlurRadiusY);
    EXPECT_NE(foregroundBlurRadiusY, 0.f);
}

/**
 * @tc.name: GenerateBackgroundBlurFilter001
 * @tc.desc: test results of GenerateBackgroundBlurFilter
 * @tc.type: FUNC
 * @tc.require: issueI9QKVM
 */
HWTEST_F(RSPropertiesTest, GenerateBackgroundBlurFilter001, TestSize.Level1)
{
    RSProperties properties;
    Vector2f vectorValue = { 1.f, 1.f };
    properties.GenerateBackgroundBlurFilter();
    EXPECT_EQ(vectorValue.x_, 1.f);

    properties.greyCoef_ = vectorValue;
    properties.GenerateBackgroundBlurFilter();
    EXPECT_EQ(vectorValue.x_, 1.f);
}

/**
 * @tc.name: GenerateBackgroundMaterialBlurFilter001
 * @tc.desc: test results of GenerateBackgroundMaterialBlurFilter
 * @tc.type: FUNC
 * @tc.require: issueI9QKVM
 */
HWTEST_F(RSPropertiesTest, GenerateBackgroundMaterialBlurFilter001, TestSize.Level1)
{
    RSProperties properties;
    Vector2f vectorValue = { 1.f, 1.f };
    properties.GenerateBackgroundMaterialBlurFilter();
    EXPECT_EQ(vectorValue.x_, 1.f);

    properties.greyCoef_ = vectorValue;
    properties.GenerateBackgroundMaterialBlurFilter();
    EXPECT_EQ(vectorValue.x_, 1.f);

    properties.backgroundColorMode_ = BLUR_COLOR_MODE::AVERAGE;
    properties.GenerateBackgroundMaterialBlurFilter();
    EXPECT_EQ(vectorValue.x_, 1.f);
}

/**
 * @tc.name: GenerateForegroundBlurFilter001
 * @tc.desc: test results of GenerateForegroundBlurFilter
 * @tc.type: FUNC
 * @tc.require: issueI9QKVM
 */
HWTEST_F(RSPropertiesTest, GenerateForegroundBlurFilter001, TestSize.Level1)
{
    RSProperties properties;
    Vector2f vectorValue = { 1.f, 1.f };
    properties.GenerateForegroundBlurFilter();
    EXPECT_EQ(vectorValue.x_, 1.f);

    properties.greyCoef_ = vectorValue;
    properties.GenerateForegroundBlurFilter();
    EXPECT_EQ(vectorValue.x_, 1.f);
}

/**
 * @tc.name: GenerateForegroundMaterialBlurFilter001
 * @tc.desc: test results of GenerateForegroundMaterialBlurFilter
 * @tc.type: FUNC
 * @tc.require: issueI9QKVM
 */
HWTEST_F(RSPropertiesTest, GenerateForegroundMaterialBlurFilter001, TestSize.Level1)
{
    RSProperties properties;
    Vector2f vectorValue = { 1.f, 1.f };
    properties.GenerateForegroundMaterialBlurFilter();
    EXPECT_EQ(vectorValue.x_, 1.f);

    properties.greyCoef_ = vectorValue;
    properties.GenerateForegroundMaterialBlurFilter();
    EXPECT_EQ(vectorValue.x_, 1.f);

    properties.backgroundColorMode_ = BLUR_COLOR_MODE::AVERAGE;
    properties.GenerateForegroundMaterialBlurFilter();
    EXPECT_EQ(vectorValue.x_, 1.f);
}

/**
 * @tc.name: GenerateAIBarFilter001
 * @tc.desc: test results of GenerateAIBarFilter
 * @tc.type: FUNC
 * @tc.require: issueI9QKVM
 */
HWTEST_F(RSPropertiesTest, GenerateAIBarFilter001, TestSize.Level1)
{
    RSProperties properties;
    properties.GenerateAIBarFilter();
    EXPECT_EQ(properties.backgroundBlurSaturation_, 1.f);
}

/**
 * @tc.name: GenerateBackgroundFilter001
 * @tc.desc: test results of GenerateBackgroundFilter
 * @tc.type: FUNC
 * @tc.require: issueI9QKVM
 */
HWTEST_F(RSPropertiesTest, GenerateBackgroundFilter001, TestSize.Level1)
{
    RSProperties properties;
    properties.GenerateBackgroundFilter();
    EXPECT_EQ(properties.backgroundBlurSaturation_, 1.f);

    properties.backgroundBlurRadiusX_ = 2.f;
    properties.GenerateBackgroundFilter();
    EXPECT_TRUE(properties.IsBackgroundBlurRadiusXValid());

    properties.backgroundBlurRadiusY_ = 2.f;
    properties.GenerateBackgroundFilter();
    EXPECT_TRUE(properties.IsBackgroundBlurRadiusYValid());

    properties.backgroundBlurRadius_ = 2.f;
    properties.GenerateBackgroundFilter();
    EXPECT_TRUE(properties.IsBackgroundBlurRadiusValid());

    properties.backgroundBlurBrightness_ = 0.f;
    properties.GenerateBackgroundFilter();
    EXPECT_TRUE(properties.IsBackgroundBlurBrightnessValid());

    properties.backgroundBlurSaturation_ = 0.f;
    properties.GenerateBackgroundFilter();
    EXPECT_TRUE(properties.IsBackgroundBlurSaturationValid());

    Vector4f aiInvert = { 1.f, 1.f, 1.f, 1.f };
    properties.aiInvert_ = aiInvert;
    properties.GenerateBackgroundFilter();
    EXPECT_EQ(properties.aiInvert_.has_value(), true);

    properties.systemBarEffect_ = true;
    properties.GenerateBackgroundFilter();
    EXPECT_TRUE(properties.systemBarEffect_);
}

/**
 * @tc.name: GenerateForegroundFilter001
 * @tc.desc: test results of GenerateForegroundFilter
 * @tc.type: FUNC
 * @tc.require: issueI9QKVM
 */
HWTEST_F(RSPropertiesTest, GenerateForegroundFilter001, TestSize.Level1)
{
    RSProperties properties;
    properties.GenerateForegroundFilter();
    EXPECT_EQ(properties.backgroundBlurSaturation_, 1.f);

    properties.foregroundBlurRadiusX_ = 2.f;
    properties.GenerateForegroundFilter();
    EXPECT_TRUE(properties.IsForegroundBlurRadiusXValid());

    properties.foregroundBlurRadiusY_ = 2.f;
    properties.GenerateForegroundFilter();
    EXPECT_TRUE(properties.IsForegroundBlurRadiusYValid());

    properties.foregroundBlurRadius_ = 2.f;
    properties.GenerateForegroundFilter();
    EXPECT_TRUE(properties.IsForegroundMaterialFilterVaild());

    std::vector<std::pair<float, float>> fractionStops;
    GradientDirection direction;
    properties.linearGradientBlurPara_ = std::make_shared<RSLinearGradientBlurPara>(-1.f, fractionStops, direction);
    properties.GenerateForegroundFilter();
    EXPECT_TRUE(properties.linearGradientBlurPara_ == nullptr);
}

/**
 * @tc.name: SetUseEffect002
 * @tc.desc: test results of SetUseEffect
 * @tc.type: FUNC
 * @tc.require: issueI9QKVM
 */
HWTEST_F(RSPropertiesTest, SetUseEffect002, TestSize.Level1)
{
    RSProperties properties;
    properties.SetUseEffect(false);
    EXPECT_TRUE(!properties.GetUseEffect());

    properties.SetUseEffect(true);
    EXPECT_TRUE(properties.GetUseEffect());
}

/**
 * @tc.name: SetUseShadowBatching002
 * @tc.desc: test results of SetUseShadowBatching
 * @tc.type: FUNC
 * @tc.require: issueI9QKVM
 */
HWTEST_F(RSPropertiesTest, SetUseShadowBatching002, TestSize.Level1)
{
    RSProperties properties;
    bool useShadowBatching = false;
    properties.SetUseShadowBatching(useShadowBatching);
    EXPECT_TRUE(!useShadowBatching);

    useShadowBatching = true;
    properties.SetUseShadowBatching(useShadowBatching);
    EXPECT_TRUE(useShadowBatching);
}

/**
 * @tc.name: SetPixelStretch002
 * @tc.desc: test results of SetPixelStretch
 * @tc.type: FUNC
 * @tc.require: issueI9QKVM
 */
HWTEST_F(RSPropertiesTest, SetPixelStretch002, TestSize.Level1)
{
    RSProperties properties;
    Vector4f stretch = { 1.f, 1.f, 1.f, 1.f };
    std::optional<Vector4f> stretchSize;
    properties.SetPixelStretch(stretchSize);
    EXPECT_TRUE(!properties.pixelStretch_.has_value());

    stretchSize = stretch;
    properties.SetPixelStretch(stretchSize);
    EXPECT_TRUE(properties.pixelStretch_.has_value());

    Vector4f size = { 0.f, 0.f, 0.f, 0.f };
    stretchSize = size;
    properties.SetPixelStretch(stretchSize);
    EXPECT_TRUE(properties.pixelStretch_->IsZero());
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
    EXPECT_EQ(properties.GetMask(), mask);
    EXPECT_EQ(properties.contentDirty_, true);
}

/**
 * @tc.name: SetNGetSpherize001
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPropertiesTest, SetNGetSpherize001, TestSize.Level1)
{
    RSProperties properties;
    float spherizeDegree{1.f};
    properties.SetSpherize(spherizeDegree);
    EXPECT_EQ(properties.GetSpherize(), spherizeDegree);
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
 * @tc.name: SetLightColor001
 * @tc.desc: test results of SetLightColor
 * @tc.type: FUNC
 * @tc.require: issueI9QKVM
 */
HWTEST_F(RSPropertiesTest, SetLightColor001, TestSize.Level1)
{
    RSProperties properties;
    Color lightColor;
    properties.SetLightColor(lightColor);
    EXPECT_NE(properties.lightSourcePtr_, nullptr);

    properties.lightSourcePtr_ = std::make_shared<RSLightSource>();
    properties.SetLightColor(lightColor);
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

    properties.illuminatedPtr_ = std::make_shared<RSIlluminated>();
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

    properties.illuminatedPtr_ = std::make_shared<RSIlluminated>();
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

    properties.illuminatedPtr_ = std::make_shared<RSIlluminated>();
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

    screenRotation = ScreenRotation::ROTATION_0;
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
    properties.colorFilter_ = std::make_shared<Drawing::ColorFilter>();
    properties.GenerateColorFilter();
    EXPECT_EQ(properties.colorFilterNeedUpdate_, false);
}

/**
 * @tc.name: GenerateColorFilter002
 * @tc.desc: test results of GenerateColorFilter
 * @tc.type: FUNC
 * @tc.require: issueI9QKVM
 */
HWTEST_F(RSPropertiesTest, GenerateColorFilter002, TestSize.Level1)
{
    RSProperties properties;
    properties.GenerateColorFilter();
    EXPECT_EQ(properties.colorFilterNeedUpdate_, false);

    std::optional<float> grayScale = std::optional<float>(1.f);
    properties.SetGrayScale(grayScale);
    properties.GenerateColorFilter();
    EXPECT_EQ(properties.colorFilterNeedUpdate_, false);

    std::optional<float> brightness = std::optional<float>(2.f);
    properties.SetBrightness(brightness);
    properties.GenerateColorFilter();
    EXPECT_EQ(properties.colorFilterNeedUpdate_, false);

    std::optional<float> contrast = std::optional<float>(2.f);
    properties.SetContrast(contrast);
    properties.GenerateColorFilter();
    EXPECT_EQ(properties.colorFilterNeedUpdate_, false);

    std::optional<float> saturate = std::optional<float>(2.f);
    properties.SetSaturate(saturate);
    properties.GenerateColorFilter();
    EXPECT_EQ(properties.colorFilterNeedUpdate_, false);

    std::optional<float> sepia = std::optional<float>(1.f);
    properties.SetSepia(sepia);
    properties.GenerateColorFilter();
    EXPECT_EQ(properties.colorFilterNeedUpdate_, false);

    std::optional<float> invert = std::optional<float>(2.f);
    properties.SetInvert(invert);
    properties.colorFilter_ = std::make_shared<Drawing::ColorFilter>();
    properties.GenerateColorFilter();
    EXPECT_EQ(properties.colorFilterNeedUpdate_, false);

    std::optional<float> hueRotate = std::optional<float>(1.f);
    properties.SetHueRotate(hueRotate);
    properties.GenerateColorFilter();
    EXPECT_EQ(properties.colorFilterNeedUpdate_, false);

    hueRotate = std::optional<float>(360.f);
    properties.SetHueRotate(hueRotate);
    hueRotate = std::optional<float>(480.f);
    properties.SetHueRotate(hueRotate);
    properties.GenerateColorFilter();
    EXPECT_EQ(properties.colorFilterNeedUpdate_, false);

    hueRotate = std::optional<float>(600.f);
    properties.SetHueRotate(hueRotate);
    properties.GenerateColorFilter();
    EXPECT_EQ(properties.colorFilterNeedUpdate_, false);

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

    properties.frameGeo_->SetX(-INFINITY);
    properties.frameGeo_->SetY(-INFINITY);
    properties.boundsGeo_->SetX(-INFINITY);
    properties.boundsGeo_->SetY(-INFINITY);
    properties.CalculateFrameOffset();
    EXPECT_EQ(properties.isDrawn_, true);
}

/**
 * @tc.name: SetHaveEffectRegion001
 * @tc.desc: test results of SetHaveEffectRegion
 * @tc.type: FUNC
 * @tc.require: issueI9QKVM
 */
HWTEST_F(RSPropertiesTest, SetHaveEffectRegion001, TestSize.Level1)
{
    RSProperties properties;
    properties.backgroundFilterCacheManager_ = std::make_unique<RSFilterCacheManager>();
    properties.backgroundFilterCacheManager_->cachedSnapshot_ =
        std::make_shared<RSPaintFilterCanvas::CachedEffectData>();
    properties.backgroundFilterCacheManager_->cachedFilteredSnapshot_ =
        std::make_shared<RSPaintFilterCanvas::CachedEffectData>();
    properties.SetHaveEffectRegion(false);
    EXPECT_NE(properties.isDrawn_, true);
}

/**
 * @tc.name: ReleaseColorPickerTaskShadow001
 * @tc.desc: test results of ReleaseColorPickerTaskShadow
 * @tc.type: FUNC
 * @tc.require: issueI9QKVM
 */
HWTEST_F(RSPropertiesTest, ReleaseColorPickerTaskShadow001, TestSize.Level1)
{
    RSProperties properties;
    properties.ReleaseColorPickerTaskShadow();
    EXPECT_NE(properties.isDrawn_, true);

    properties.shadow_ = std::make_optional<RSShadow>();
    properties.ReleaseColorPickerTaskShadow();
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