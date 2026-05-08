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

#include "common/rs_vector4.h"
#include "gtest/gtest.h"
#include "limit_number.h"
#include "modifier_ng/appearance/rs_behind_window_filter_render_modifier.h"
#include "modifier_ng/appearance/rs_hdr_brightness_render_modifier.h"
#include "modifier_ng/foreground/rs_env_foreground_color_render_modifier.h"
#include "modifier_ng/geometry/rs_bounds_render_modifier.h"
#include "modifier_ng/geometry/rs_frame_clip_render_modifier.h"
#include "modifier_ng/rs_modifier_ng_type.h"
#include "modifier_ng/rs_render_modifier_ng.h"
#include "modifier/rs_render_property.h"
#include "pipeline/rs_context.h"
#include "pipeline/rs_render_node.h"
#include "pipeline/rs_surface_render_node.h"
#include "recording/draw_cmd.h"

using namespace testing;
using namespace testing::ext;
namespace OHOS {
namespace Rosen {

template<ModifierNG::RSModifierType ModType, bool isNull = false, bool isEmpty = false>
class RSTestStyleModifier : public ModifierNG::RSRenderModifier {
public:
    RSTestStyleModifier() = default;
    ~RSTestStyleModifier() override = default;
    static inline constexpr auto type = ModType;
    ModifierNG::RSModifierType GetType() const override { return type; }
    Drawing::DrawCmdListPtr GetPropertyDrawCmdList() const override
    {
        if constexpr (isNull) {
            return nullptr;
        } else if constexpr (isEmpty) {
            return std::make_shared<Drawing::DrawCmdList>();
        } else {
            // Use DEFERRED mode with drawOpItems to make IsEmpty() return false
            auto cmdList = std::make_shared<Drawing::DrawCmdList>(Drawing::DrawCmdList::UnmarshalMode::DEFERRED);
            cmdList->AddDrawOp(std::make_shared<Drawing::DrawColorOpItem>(0xFF000000, Drawing::BlendMode::SRC_OVER));
            return cmdList;
        }
    }
};

using RSTestContentStyleModifier = RSTestStyleModifier<ModifierNG::RSModifierType::CONTENT_STYLE, false, false>;
using RSTestBackgroundStyleModifier = RSTestStyleModifier<ModifierNG::RSModifierType::BACKGROUND_STYLE, false, true>;
using RSTestTransitionStyleModifier = RSTestStyleModifier<ModifierNG::RSModifierType::TRANSITION_STYLE, true, false>;

class RSSolidLayerTest : public testing::Test {
public:
    void SetUp() override { rsContext_ = std::make_shared<RSContext>(); }

private:
    static inline NodeId id_ = 0;
    std::shared_ptr<RSContext> rsContext_;
};

/**
 * @tc.name: HasValidDrawCmd001
 * @tc.desc: Test HasValidDrawCmd and IsPureBackgroundColor with various modifiers types
 * @tc.type: FUNC
 */
HWTEST_F(RSSolidLayerTest, HasValidDrawCmd001, TestSize.Level1)
{
    // Test 1: no modifier -> HasValidDrawCmd=false, IsPureBackgroundColor=true
    auto node1 = std::make_shared<RSSurfaceRenderNode>(id_, rsContext_);
    EXPECT_FALSE(node1->HasValidDrawCmd());
    EXPECT_TRUE(node1->IsPureBackgroundColor());

    // Test 2: BEHIND_WINDOW_FILTER -> HasValidDrawCmd=true, IsPureBackgroundColor=false
    auto node2 = std::make_shared<RSSurfaceRenderNode>(id_, rsContext_);
    node2->AddModifier(std::make_shared<ModifierNG::RSBehindWindowFilterRenderModifier>());
    EXPECT_TRUE(node2->HasValidDrawCmd());
    EXPECT_FALSE(node2->IsPureBackgroundColor());

    // Test 3: ENV_FOREGROUND_COLOR -> HasValidDrawCmd=true, IsPureBackgroundColor=false
    auto node3 = std::make_shared<RSSurfaceRenderNode>(id_, rsContext_);
    node3->AddModifier(std::make_shared<ModifierNG::RSEnvForegroundColorRenderModifier>());
    EXPECT_TRUE(node3->HasValidDrawCmd());
    EXPECT_FALSE(node3->IsPureBackgroundColor());

    // Test 4: HDR_BRIGHTNESS -> HasValidDrawCmd=true, IsPureBackgroundColor=false
    auto node4 = std::make_shared<RSSurfaceRenderNode>(id_, rsContext_);
    node4->AddModifier(std::make_shared<ModifierNG::RSHDRBrightnessRenderModifier>());
    EXPECT_TRUE(node4->HasValidDrawCmd());
    EXPECT_FALSE(node4->IsPureBackgroundColor());
}

/**
 * @tc.name: HasValidDrawCmd002
 * @tc.desc: Test HasValidDrawCmd with BOUNDS, CONTENT_STYLE, BACKGROUND_STYLE modifiers
 * @tc.type: FUNC
 */
HWTEST_F(RSSolidLayerTest, HasValidDrawCmd002, TestSize.Level1)
{
    // Test 1: BOUNDS only -> HasValidDrawCmd=false
    auto node1 = std::make_shared<RSSurfaceRenderNode>(id_, rsContext_);
    node1->AddModifier(std::make_shared<ModifierNG::RSBoundsRenderModifier>());
    EXPECT_FALSE(node1->HasValidDrawCmd());

    // Test 2: CONTENT_STYLE with non-empty cmdList -> HasValidDrawCmd=true, IsPureBackgroundColor=false
    auto node2 = std::make_shared<RSSurfaceRenderNode>(id_, rsContext_);
    node2->AddModifier(std::make_shared<RSTestContentStyleModifier>());
    EXPECT_TRUE(node2->HasValidDrawCmd());
    EXPECT_FALSE(node2->IsPureBackgroundColor());

    // Test 3: BACKGROUND_STYLE with empty cmdList -> HasValidDrawCmd=false
    auto node3 = std::make_shared<RSSurfaceRenderNode>(id_, rsContext_);
    node3->AddModifier(std::make_shared<RSTestBackgroundStyleModifier>());
    EXPECT_FALSE(node3->HasValidDrawCmd());

    // Test 4: TRANSITION_STYLE with nullptr cmdList -> HasValidDrawCmd=false
    auto node4 = std::make_shared<RSSurfaceRenderNode>(id_, rsContext_);
    node4->AddModifier(std::make_shared<RSTestTransitionStyleModifier>());
    EXPECT_FALSE(node4->HasValidDrawCmd());
}

/**
 * @tc.name: HasValidDrawCmd003
 * @tc.desc: Test HasValidDrawCmd with CLIP_TO_FRAME modifier
 * @tc.type: FUNC
 */
HWTEST_F(RSSolidLayerTest, HasValidDrawCmd003, TestSize.Level1)
{
    // Test 1: CLIP_TO_FRAME with CUSTOM_CLIP_TO_FRAME -> HasValidDrawCmd=true
    auto node1 = std::make_shared<RSSurfaceRenderNode>(id_, rsContext_);
    Vector4f clipRect { 1.0f, 1.0f, 1.0f, 1.0f };
    auto property = std::make_shared<RSRenderProperty<Vector4f>>(clipRect, id_);
    auto modifier = ModifierNG::RSRenderModifier::MakeRenderModifier(
        ModifierNG::RSModifierType::CLIP_TO_FRAME, property, id_,
        ModifierNG::RSPropertyType::CUSTOM_CLIP_TO_FRAME);
    node1->AddModifier(modifier);
    EXPECT_TRUE(node1->HasValidDrawCmd());

    // Test 2: CLIP_TO_FRAME without CUSTOM_CLIP_TO_FRAME -> HasValidDrawCmd=false
    auto node2 = std::make_shared<RSSurfaceRenderNode>(id_, rsContext_);
    node2->AddModifier(std::make_shared<ModifierNG::RSFrameClipRenderModifier>());
    EXPECT_FALSE(node2->HasValidDrawCmd());
}

} // namespace Rosen
} // namespace OHOS