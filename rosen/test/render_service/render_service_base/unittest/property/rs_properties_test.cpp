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

} // namespace Rosen
} // namespace OHOS