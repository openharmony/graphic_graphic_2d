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
 * @tc.name: Generate001
 * @tc.desc: test results of Generate
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSEffectDataApplyDrawableTest, Generate001, TestSize.Level1)
{
    RSRenderContent content;
    RSEffectDataApplyDrawable::Generate(content);
    EXPECT_EQ(content.GetRenderProperties().GetUseEffect(), false);

    RSProperties properties;
    properties.SetUseEffect(true);
    RSEffectDataApplyDrawable::Generate(content);
    EXPECT_NE(content.GetRenderProperties().GetUseEffect(), true);
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
    RSProperties properties;
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
 * @tc.require:
 */
HWTEST_F(RSPixelStretchDrawableTest, Generate001, TestSize.Level1)
{
    RSRenderContent content;
    RSProperties properties;
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
    Drawing::Canvas canvas;
    RSPaintFilterCanvas fileCanvas(&canvas);
    RSBackgroundDrawable drawable;
    RSProperties properties;
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
    RSProperties properties;
    Color color(1, 1, 1, 1); // for test
    properties.SetBackgroundColor(color);
    RSBackgroundColorDrawable::Generate(content);

    color.SetAlpha(0);
    color.SetGreen(0);
    color.SetRed(0);
    color.SetBlue(0);
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
    Drawing::Color color;
    RSBackgroundColorDrawable drawable(color);
    RSProperties properties;
    Color groundColor(1, 1, 1, 1); // for test
    properties.SetBackgroundColor(groundColor);
    bool res = drawable.Update(content);
    EXPECT_EQ(res, false);

    groundColor.SetAlpha(0);
    groundColor.SetGreen(0);
    groundColor.SetRed(0);
    groundColor.SetBlue(0);
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
    RSProperties properties;
    RSBackgroundShaderDrawable::Generate(content);
    EXPECT_EQ(properties.GetBackgroundShader(), nullptr);

    auto shader = std::make_shared<RSShader>();
    properties.SetBackgroundShader(shader);
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
    RSProperties properties;
    RSBackgroundImageDrawable::Generate(content);
    EXPECT_EQ(properties.GetBgImage(), nullptr);

    auto image = std::make_shared<RSImage>();
    properties.SetBgImage(image);
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
    RSProperties properties;
    RSBackgroundImageDrawable drawable;
    bool res = drawable.Update(content);
    EXPECT_EQ(res, false);

    auto image = std::make_shared<RSImage>();
    properties.SetBgImage(image);
    res = drawable.Update(content);
    EXPECT_EQ(res, false);
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