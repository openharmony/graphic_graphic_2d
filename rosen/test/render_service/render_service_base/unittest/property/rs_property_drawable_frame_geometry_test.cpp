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

#include "property/rs_property_drawable_frame_geometry.h"
#include "property/rs_properties.h"
#include "pipeline/rs_paint_filter_canvas.h"
#include "pipeline/rs_render_content.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class RSColorFilterDrawableTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSColorFilterDrawableTest::SetUpTestCase() {}
void RSColorFilterDrawableTest::TearDownTestCase() {}
void RSColorFilterDrawableTest::SetUp() {}
void RSColorFilterDrawableTest::TearDown() {}

/**
 * @tc.name: Draw001
 * @tc.desc: test results of Draw
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSColorFilterDrawableTest, Draw001, TestSize.Level1)
{
    RSRenderContent content;
    Drawing::Canvas canvas;
    RSPaintFilterCanvas fileCanvas(&canvas);
    Drawing::Brush brush;
    RSColorFilterDrawable drawable(std::move(brush));
    drawable.Draw(content, fileCanvas);
    EXPECT_EQ(fileCanvas.GetSurface(), nullptr);
}

/**
 * @tc.name: Generate001
 * @tc.desc: test results of Generate
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSColorFilterDrawableTest, Generate001, TestSize.Level1)
{
    RSRenderContent content;
    RSProperties properties;
    RSColorFilterDrawable::Generate(content);
    EXPECT_EQ(properties.GetColorFilter(), nullptr);

    properties.colorFilter_ = std::make_shared<Drawing::ColorFilter>();
    RSColorFilterDrawable::Generate(content);
    EXPECT_NE(properties.GetColorFilter(), nullptr);
}

/**
 * @tc.name: Update001
 * @tc.desc: test results of Update
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSColorFilterDrawableTest, Update001, TestSize.Level1)
{
    RSRenderContent content;
    RSProperties properties;
    Drawing::Brush brush;
    RSColorFilterDrawable drawable(std::move(brush));
    bool res = drawable.Update(content);
    EXPECT_EQ(res, false);

    properties.colorFilter_ = std::make_shared<Drawing::ColorFilter>();
    res = drawable.Update(content);
    EXPECT_NE(res, true);
}
} // namespace Rosen
} // namespace OHOS