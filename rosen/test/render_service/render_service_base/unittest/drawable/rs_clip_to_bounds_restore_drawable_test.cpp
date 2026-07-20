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

#include "gtest/gtest.h"

#include "common/rs_obj_geometry.h"
#include "draw/surface.h"
#include "drawable/rs_clip_to_bounds_restore_drawable.h"
#include "drawable/rs_property_drawable_utils.h"
#include "effect/rs_render_shape_base.h"
#include "ge_visual_effect_container.h"
#include "pipeline/rs_recording_canvas.h"
#include "pipeline/rs_render_node.h"
#include "render/rs_drawing_filter.h"
#include "render/rs_render_aibar_filter.h"
#include "render/rs_render_linear_gradient_blur_filter.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSClipToBoundsRestoreDrawableTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

class ConcreteRSRenderNodeDrawableAdapter : public DrawableV2::RSRenderNodeDrawableAdapter {
public:
    explicit ConcreteRSRenderNodeDrawableAdapter(std::shared_ptr<const RSRenderNode> node)
        : RSRenderNodeDrawableAdapter(std::move(node))
    {}
    void Draw(Drawing::Canvas& canvas) {}
};

void RSClipToBoundsRestoreDrawableTest::SetUpTestCase() {}
void RSClipToBoundsRestoreDrawableTest::TearDownTestCase() {}
void RSClipToBoundsRestoreDrawableTest::SetUp() {}
void RSClipToBoundsRestoreDrawableTest::TearDown() {}

/**
 * @tc.name: RSClipToBoundsRestoreDrawableOnGenerateTest
 * @tc.desc: Test RSClipToBoundsRestoreDrawable OnGenerate
 * @tc.type: FUNC
 */
HWTEST_F(RSClipToBoundsRestoreDrawableTest, RSClipToBoundsRestoreDrawableOnGenerateTest, TestSize.Level1)
{
    RSRenderNode node(0);
    auto content = std::make_shared<uint32_t>(1);
    auto drawable = DrawableV2::RSClipToBoundsRestoreDrawable::OnGenerate(node, content);
    EXPECT_NE(drawable, nullptr);
}

/**
 * @tc.name: RSClipToBoundsRestoreDrawableOnUpdateTest
 * @tc.desc: Test RSClipToBoundsRestoreDrawable OnUpdate branches (clipBounds/sdfShape/other)
 * @tc.type: FUNC
 */
HWTEST_F(RSClipToBoundsRestoreDrawableTest, RSClipToBoundsRestoreDrawableOnUpdateTest, TestSize.Level1)
{
    auto content = std::make_shared<uint32_t>(1);
    auto drawable = std::make_shared<DrawableV2::RSClipToBoundsRestoreDrawable>(content);
    EXPECT_NE(drawable, nullptr);

    // clipBounds set → sdfShape_ null (standard mode)
    RSRenderNode nodeClip(0);
    Drawing::Path path;
    path.MoveTo(10.0f, 20.0f);
    path.LineTo(100.0f, 200.0f);
    path.LineTo(150.0f, 250.0f);
    nodeClip.renderProperties_.clipPath_ = RSPath::CreateRSPath(path);
    drawable->OnUpdate(nodeClip);
    EXPECT_EQ(drawable->stagingSdfShape_, nullptr);

    // sdfShape set (no clipBounds) → sdfShape_ non-null (SDF mode)
    RSRenderNode nodeSdf(0);
    auto sdfShape = RSNGRenderShapeBase::Create(RSNGEffectType::SDF_UNION_OP_SHAPE);
    nodeSdf.GetMutableRenderProperties().SetSDFShape(sdfShape);
    drawable->OnUpdate(nodeSdf);
    EXPECT_NE(drawable->stagingSdfShape_, nullptr);

    // neither clipBounds nor sdfShape → sdfShape_ null (standard mode)
    RSRenderNode nodeEmpty(0);
    drawable->OnUpdate(nodeEmpty);
    EXPECT_EQ(drawable->stagingSdfShape_, nullptr);
}

/**
 * @tc.name: RSClipToBoundsRestoreDrawableOnSyncTest
 * @tc.desc: Test RSClipToBoundsRestoreDrawable OnSync
 * @tc.type: FUNC
 */
HWTEST_F(RSClipToBoundsRestoreDrawableTest, RSClipToBoundsRestoreDrawableOnSyncTest, TestSize.Level1)
{
    auto content = std::make_shared<uint32_t>(1);
    auto drawable = std::make_shared<DrawableV2::RSClipToBoundsRestoreDrawable>(content);
    drawable->needSync_ = false;
    drawable->OnSync();

    drawable->needSync_ = true;
    drawable->stagingSdfShape_ = RSNGRenderShapeBase::Create(RSNGEffectType::SDF_UNION_OP_SHAPE);
    drawable->OnSync();
    EXPECT_FALSE(drawable->needSync_);
    EXPECT_NE(drawable->sdfShape_, nullptr);
}

/**
 * @tc.name: RSClipToBoundsRestoreDrawableOnDrawTest
 * @tc.desc: Test RSClipToBoundsRestoreDrawable OnDraw (SDF mode and standard mode)
 * @tc.type: FUNC
 */
HWTEST_F(RSClipToBoundsRestoreDrawableTest, RSClipToBoundsRestoreDrawableOnDrawTest, TestSize.Level1)
{
    auto content = std::make_shared<uint32_t>(1);
    auto drawable = std::make_shared<DrawableV2::RSClipToBoundsRestoreDrawable>(content);
    Drawing::Canvas canvas;
    RSPaintFilterCanvas pfCanvas(&canvas);
    Drawing::Rect rect(0.0f, 0.0f, 1.0f, 1.0f);

    // standard mode (sdfShape_ = null) → RestoreToCount only, no DrawSdfClip
    drawable->stagingSdfShape_ = nullptr;
    drawable->needSync_ = true;
    drawable->OnSync();
    drawable->OnDraw(&pfCanvas, &rect);
    EXPECT_EQ(drawable->sdfShape_, nullptr);

    // SDF mode (sdfShape_ non-null) → DrawSdfClip (empty offscreen → early return, no crash)
    drawable->stagingSdfShape_ = RSNGRenderShapeBase::Create(RSNGEffectType::SDF_UNION_OP_SHAPE);
    drawable->needSync_ = true;
    drawable->OnSync();
    drawable->OnDraw(&pfCanvas, &rect);
    EXPECT_NE(drawable->sdfShape_, nullptr);
}

/**
 * @tc.name: RSClipToBoundsRestoreDrawableOnDrawNullGuardTest
 * @tc.desc: Test OnDraw null guards: null canvas → early return; null content_ → skip RestoreToCount
 * @tc.type: FUNC
 */
HWTEST_F(RSClipToBoundsRestoreDrawableTest, RSClipToBoundsRestoreDrawableOnDrawNullGuardTest, TestSize.Level1)
{
    Drawing::Rect rect(0.0f, 0.0f, 1.0f, 1.0f);
    // null canvas → paintFilterCanvas null → early return (no crash)
    auto drawable = std::make_shared<DrawableV2::RSClipToBoundsRestoreDrawable>(std::make_shared<uint32_t>(1));
    drawable->OnDraw(nullptr, &rect);
    EXPECT_NE(drawable->content_, nullptr);

    // null content_ → skip RestoreToCount (no crash)
    auto drawableNoContent = std::make_shared<DrawableV2::RSClipToBoundsRestoreDrawable>(nullptr);
    Drawing::Canvas canvas;
    RSPaintFilterCanvas pfCanvas(&canvas);
    drawableNoContent->OnDraw(&pfCanvas, &rect);
    EXPECT_EQ(drawableNoContent->content_, nullptr);
}
} // namespace OHOS::Rosen
