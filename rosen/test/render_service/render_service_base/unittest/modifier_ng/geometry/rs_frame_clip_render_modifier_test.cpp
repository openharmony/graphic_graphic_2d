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

#include <cstdlib>
#include <cstring>
#include <memory>
#include <securec.h>

#include "gtest/gtest.h"
#include "gtest/hwext/gtest-tag.h"

#include "modifier_ng/geometry/rs_frame_clip_render_modifier.h"
#include "property/rs_properties.h"
#include "pipeline/rs_canvas_render_node.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS::Rosen::ModifierNG;

namespace OHOS::Rosen {
class RSFrameClipRenderModifierNGTypeTest : public testing::Test {
public:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
    void SetUp() override {}
    void TearDown() override {}
};

/**
 * @tc.name: RSFramClipRenderModifierTest
 * @tc.desc: ResetProperties & GetType
 * @tc.type: FUNC
 */
HWTEST_F(RSFrameClipRenderModifierNGTypeTest, RSFrameClipRenderModifierTest, TestSize.Level1)
{
    RSFrameClipRenderModifier modifier;
    RSProperties properties;
    modifier.ResetProperties(properties);

    EXPECT_EQ(modifier.GetType(), ModifierNG::RSModifierType::CLIP_TO_FRAME);

    Drawing::Canvas canvas;
    RSPaintFilterCanvas filterCanvas(&canvas);
    modifier.Apply(&filterCanvas, properties);

    EXPECT_EQ(properties.GetClipToFrame(), false);
    EXPECT_EQ(properties.GetFrameGravity(), Gravity::DEFAULT);
}

/**
 * @tc.name: OnSetDirtyTest
 * @tc.desc: Test the function OnSetDirty
 * @tc.type: FUNC
 */
HWTEST_F(RSFrameClipRenderModifierNGTypeTest, OnSetDirtyTest, TestSize.Level1)
{
    uint64_t id = 0;
    float value = 0.0f;
    auto property = std::make_shared<RSRenderProperty<float>>(value, id);
    auto modifier = ModifierNG::RSRenderModifier::MakeRenderModifier(
        ModifierNG::RSModifierType::CLIP_TO_FRAME, property, id, ModifierNG::RSPropertyType::CUSTOM_CLIP_TO_FRAME);
    modifier->OnSetDirty();
    EXPECT_TRUE(modifier->HasProperty(ModifierNG::RSPropertyType::CUSTOM_CLIP_TO_FRAME));

    RSCanvasRenderNode node(id);
    modifier->target_ = node.weak_from_this();
    modifier->OnSetDirty();
    EXPECT_NE(modifier->target_.lock(), nullptr);
}
} // namespace OHOS::Rosen