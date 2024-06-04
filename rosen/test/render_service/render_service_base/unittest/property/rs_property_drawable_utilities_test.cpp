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

#include "property/rs_property_drawable_utilities.h"
#include "property/rs_properties.h"
#include "pipeline/rs_paint_filter_canvas.h"
#include "pipeline/rs_render_content.h"
#include "pipeline/rs_render_node.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class RSModifierDrawableTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSModifierDrawableTest::SetUpTestCase() {}
void RSModifierDrawableTest::TearDownTestCase() {}
void RSModifierDrawableTest::SetUp() {}
void RSModifierDrawableTest::TearDown() {}

class RSAlphaDrawableTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSAlphaDrawableTest::SetUpTestCase() {}
void RSAlphaDrawableTest::TearDownTestCase() {}
void RSAlphaDrawableTest::SetUp() {}
void RSAlphaDrawableTest::TearDown() {}

/**
 * @tc.name: Draw001
 * @tc.desc: test results of Draw
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSModifierDrawableTest, Draw001, TestSize.Level1)
{
    RSRenderContent content;
    Drawing::Canvas canvas;
    RSPaintFilterCanvas fileCanvas(&canvas);
    RSModifierType type = RSModifierType::BOUNDS;
    RSModifierDrawable drawable(type);
    drawable.Draw(content, fileCanvas);
    EXPECT_EQ(type, RSModifierType::BOUNDS);

    auto sharedPtr = std::make_shared<RSRenderNode>(1);
    RSProperties& properties = content.GetMutableRenderProperties();
    properties.backref_ = sharedPtr;
    drawable.Draw(content, fileCanvas);
    EXPECT_TRUE(sharedPtr != nullptr);

    std::list<std::shared_ptr<RSRenderModifier>> modifierList;
    auto property = std::make_shared<RSRenderProperty<Drawing::Matrix>>();
    auto modifier = std::make_shared<RSGeometryTransRenderModifier>(property);
    modifierList.push_back(modifier);
    modifierList.push_back(modifier);
    content.drawCmdModifiers_.insert(std::make_pair(RSModifierType::BOUNDS, modifierList));
    drawable.Draw(content, fileCanvas);
    EXPECT_EQ(type, RSModifierType::BOUNDS);
}

/**
 * @tc.name: Generate001
 * @tc.desc: test results of Generate
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSAlphaDrawableTest, Generate001, TestSize.Level1)
{
    RSRenderContent content;
    RSProperties& properties = content.GetMutableRenderProperties();
    RSAlphaDrawable::Generate(content);
    EXPECT_EQ(properties.GetAlpha(), 1.f);

    properties.SetAlpha(2.f);
    RSAlphaDrawable::Generate(content);
    EXPECT_EQ(properties.GetAlpha(), 2.f);
}
} // namespace Rosen
} // namespace OHOS