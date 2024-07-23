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

#include "gtest/gtest.h"
#include "drawable/dfx/rs_dirty_rects_dfx.h"
#include "drawable/rs_display_render_node_drawable.h"
#include "params/rs_render_thread_params.h"
#include "pipeline/rs_display_render_node.h"
#include "pipeline/rs_render_node.h"
#include "pipeline/rs_uni_render_thread.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS::Rosen::DrawableV2;

namespace OHOS::Rosen {
constexpr int32_t DEFAULT_CANVAS_SIZE = 100;
constexpr NodeId DEFAULT_ID = 0xFFFF;

class RSDirtyRectsDFXTest : public testing::Test {
public:
    std::shared_ptr<RSSurfaceRenderNode> renderNode_;
    std::shared_ptr<RSDisplayRenderNode> displayRenderNode_;
    RSRenderNodeDrawableAdapter* drawable_ = nullptr;
    std::shared_ptr<RSDisplayRenderNodeDrawable> displayDrawable_ = nullptr;
    std::shared_ptr<RSDirtyRectsDfx> rsDirtyRectsDfx_;
    std::shared_ptr<RSPaintFilterCanvas> canvas_;
    std::shared_ptr<Drawing::Canvas> drawingCanvas_;

    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSDirtyRectsDFXTest::SetUpTestCase() {}
void RSDirtyRectsDFXTest::TearDownTestCase() {}
void RSDirtyRectsDFXTest::SetUp()
{
    renderNode_ = std::make_shared<RSSurfaceRenderNode>(DEFAULT_ID);
    RSDisplayNodeConfig config;
    displayRenderNode_ = std::make_shared<RSDisplayRenderNode>(DEFAULT_ID, config);
    if (!renderNode_) {
        RS_LOGE("RSSurfaceRenderNodeDrawableTest: failed to create surface node.");
        return;
    }
    displayDrawable_ = std::static_pointer_cast<RSDisplayRenderNodeDrawable>(
        DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(displayRenderNode_));
    if (!displayDrawable_->renderParams_) {
        RS_LOGE("RSSurfaceRenderNodeDrawableTest: failed to init displayDrawable_.");
        return;
    }
    auto displayRenderParams = static_cast<RSDisplayRenderParams*>(displayDrawable_->GetRenderParams().get());
    if (!displayRenderParams) {
        RS_LOGE("RSSurfaceRenderNodeDrawableTest: failed to init displayRenderParams.");
        return;
    }
    rsDirtyRectsDfx_ = std::make_shared<RSDirtyRectsDfx>(*displayDrawable_);
    if (!rsDirtyRectsDfx_) {
        RS_LOGE("RSSurfaceRenderNodeDrawableTest: failed to create RSDirtyRectsDfx.");
        return;
    }
    drawingCanvas_ = std::make_unique<Drawing::Canvas>(DEFAULT_CANVAS_SIZE, DEFAULT_CANVAS_SIZE);
    if (drawingCanvas_) {
        canvas_ = std::make_shared<RSPaintFilterCanvas>(drawingCanvas_.get());
    }
    auto& rtThread = RSUniRenderThread::Instance();
    if (!rtThread.renderThreadParams_) {
        rtThread.renderThreadParams_ = std::make_unique<RSRenderThreadParams>();
    }
}
void RSDirtyRectsDFXTest::TearDown() {}

/**
 * @tc.name: OnDrawest
 * @tc.desc: Test If OnDraw Can Run
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSDirtyRectsDFXTest, OnDraw, TestSize.Level1)
{
    ASSERT_NE(rsDirtyRectsDfx_, nullptr);
    rsDirtyRectsDfx_->OnDraw(canvas_);
}

/**
 * @tc.name: OnDrawVirtualTest
 * @tc.desc: Test If OnDrawVirtual Can Run
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSDirtyRectsDFXTest, OnDrawVirtual, TestSize.Level1)
{
    ASSERT_NE(rsDirtyRectsDfx_, nullptr);
    rsDirtyRectsDfx_->OnDrawVirtual(canvas_);
}

/**
 * @tc.name: DrawDirtyRegionInVirtual
 * @tc.desc: Test If DrawCurrentRefreshRate Can Run
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSDirtyRectsDFXTest, DrawDirtyRegionInVirtual, TestSize.Level1)
{
    ASSERT_NE(rsDirtyRectsDfx_, nullptr);
    rsDirtyRectsDfx_->DrawDirtyRegionInVirtual();
    ASSERT_EQ(rsDirtyRectsDfx_->canvas_, nullptr);
}

/**
 * @tc.name: DrawDirtyRegionForDFX
 * @tc.desc: Test If DrawDirtyRegionForDFX Can Run
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSDirtyRectsDFXTest, DrawDirtyRegionForDFX, TestSize.Level1)
{
    ASSERT_NE(rsDirtyRectsDfx_, nullptr);
    const auto& visibleDirtyRects = rsDirtyRectsDfx_->dirtyRegion_.GetRegionRects();
    std::vector<RectI> rects;
    for (auto& rect : visibleDirtyRects) {
        rects.emplace_back(rect.left_, rect.top_, rect.right_ - rect.left_, rect.bottom_ - rect.top_);
    }
    rsDirtyRectsDfx_->DrawDirtyRegionForDFX(rects);

    auto& targetDrawable = rsDirtyRectsDfx_->targetDrawable_;
    auto dirtyManager = targetDrawable.GetSyncDirtyManager();
    ASSERT_NE(dirtyManager, nullptr);
    rects = dirtyManager->GetMergedDirtyRegions();
    rsDirtyRectsDfx_->DrawDirtyRegionForDFX(rects);
}

/**
 * @tc.name: DrawAllSurfaceOpaqueRegionForDFX
 * @tc.desc: Test If DrawAllSurfaceOpaqueRegionForDFX Can Run
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSDirtyRectsDFXTest, DrawAllSurfaceOpaqueRegionForDFX, TestSize.Level1)
{
    ASSERT_NE(rsDirtyRectsDfx_, nullptr);
    rsDirtyRectsDfx_->DrawAllSurfaceOpaqueRegionForDFX();
    auto& targetDrawable = rsDirtyRectsDfx_->targetDrawable_;
    ASSERT_NE(targetDrawable.GetRenderParams(), nullptr);
}

/**
 * @tc.name: DrawTargetSurfaceDirtyRegionForDFX
 * @tc.desc: Test If DrawTargetSurfaceDirtyRegionForDFX Can Run
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSDirtyRectsDFXTest, DrawTargetSurfaceDirtyRegionForDFX, TestSize.Level1)
{
    ASSERT_NE(rsDirtyRectsDfx_, nullptr);
    rsDirtyRectsDfx_->DrawTargetSurfaceDirtyRegionForDFX();
    ASSERT_NE(rsDirtyRectsDfx_->targetDrawable_.GetRenderParams(), nullptr);
}

/**
 * @tc.name: DrawTargetSurfaceVisibleRegionForDFX
 * @tc.desc: Test If DrawTargetSurfaceVisibleRegionForDFX Can Run
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSDirtyRectsDFXTest, DrawTargetSurfaceVisibleRegionForDFX, TestSize.Level1)
{
    ASSERT_NE(rsDirtyRectsDfx_, nullptr);
    rsDirtyRectsDfx_->DrawTargetSurfaceVisibleRegionForDFX();
}
}
