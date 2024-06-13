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

#include "drawable/rs_display_render_node_drawable.h"
#include "params/rs_display_render_params.h"
#include "params/rs_render_thread_params.h"
#include "pipeline/rs_display_render_node.h"
#include "pipeline/rs_uni_render_thread.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS::Rosen::DrawableV2;

namespace OHOS::Rosen {
constexpr int32_t DEFAULT_CANVAS_SIZE = 100;
constexpr NodeId DEFAULT_ID = 0xFFFF;

class RSDisplayRenderNodeDrawableTest : public testing::Test {
public:
    std::shared_ptr<RSDisplayRenderNode> renderNode_;
    std::shared_ptr<RSDisplayRenderNode> mirroredNode_;
    RSRenderNodeDrawableAdapter* drawable_ = nullptr;
    RSRenderNodeDrawableAdapter* mirroredDrawable_ = nullptr;
    RSDisplayRenderNodeDrawable* displayDrawable_ = nullptr;
    RSDisplayRenderNodeDrawable* mirroredDisplayDrawable_ = nullptr;

    std::shared_ptr<Drawing::Canvas> drawingCanvas_;
    std::shared_ptr<RSPaintFilterCanvas> canvas_;

    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSDisplayRenderNodeDrawableTest::SetUpTestCase() {}
void RSDisplayRenderNodeDrawableTest::TearDownTestCase() {}
void RSDisplayRenderNodeDrawableTest::SetUp()
{
    RSDisplayNodeConfig config;
    renderNode_ = std::make_shared<RSDisplayRenderNode>(DEFAULT_ID, config);
    mirroredNode_ = std::make_shared<RSDisplayRenderNode>(DEFAULT_ID + 1, config);
    if (!renderNode_ || !mirroredNode_) {
        RS_LOGE("RSDisplayRenderNodeDrawableTest: failed to create display node.");
    }
    drawable_ = RSDisplayRenderNodeDrawable::OnGenerate(renderNode_);
    mirroredDrawable_ = RSDisplayRenderNodeDrawable::OnGenerate(mirroredNode_);
    if (drawable_ && mirroredDrawable_) {
        displayDrawable_ = static_cast<RSDisplayRenderNodeDrawable*>(drawable_);
        mirroredDisplayDrawable_ = static_cast<RSDisplayRenderNodeDrawable*>(mirroredDrawable_);
        if (!drawable_->renderParams_ || !mirroredDrawable_->renderParams_) {
            RS_LOGE("RSDisplayRenderNodeDrawableTest: failed to init render params.");
        }
        drawingCanvas_ = std::make_unique<Drawing::Canvas>(DEFAULT_CANVAS_SIZE, DEFAULT_CANVAS_SIZE);
        if (drawingCanvas_) {
            canvas_ = std::make_shared<RSPaintFilterCanvas>(drawingCanvas_.get());
        }
        if (displayDrawable_) {
            displayDrawable_->curCanvas_ = canvas_;
        }
    } else {
        RS_LOGE("RSDisplayRenderNodeDrawableTest: failed to create drawable.");
    }
}
void RSDisplayRenderNodeDrawableTest::TearDown() {}

/**
 * @tc.name: CreateDisplayRenderNodeDrawableTest
 * @tc.desc: Test If DisplayRenderNodeDrawable Can Be Created
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSDisplayRenderNodeDrawableTest, CreateDisplayRenderNodeDrawable, TestSize.Level1)
{
    RSDisplayNodeConfig config;
    NodeId id = 1;
    auto displayNode = std::make_shared<RSDisplayRenderNode>(id, config);
    auto drawable = RSDisplayRenderNodeDrawable::OnGenerate(displayNode);
    ASSERT_NE(drawable, nullptr);
}

/**
 * @tc.name: PrepareOffscreenRender001
 * @tc.desc: Test PrepareOffscreenRender, if offscreenWidth/offscreenHeight were not initialized.
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSDisplayRenderNodeDrawableTest, PrepareOffscreenRender001, TestSize.Level1)
{
    ASSERT_NE(displayDrawable_, nullptr);
    ASSERT_NE(renderNode_, nullptr);
    displayDrawable_->PrepareOffscreenRender(*renderNode_);
}

/**
 * @tc.name: PrepareOffscreenRender002
 * @tc.desc: Test PrepareOffscreenRender, offscreenWidth/offscreenHeight is/are correctly initialized.
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSDisplayRenderNodeDrawableTest, PrepareOffscreenRender002, TestSize.Level1)
{
    ASSERT_NE(displayDrawable_, nullptr);
    ASSERT_NE(renderNode_, nullptr);
    renderNode_->GetMutableRenderProperties().SetFrameWidth(DEFAULT_CANVAS_SIZE);
    renderNode_->GetMutableRenderProperties().SetFrameHeight(DEFAULT_CANVAS_SIZE);
    displayDrawable_->PrepareOffscreenRender(*renderNode_);
}

/**
 * @tc.name: ClearTransparentBeforeSaveLayer
 * @tc.desc: Test ClearTransparentBeforeSaveLayer, with two surface with/without param initialization
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSDisplayRenderNodeDrawableTest, ClearTransparentBeforeSaveLayer, TestSize.Level1)
{
    ASSERT_NE(displayDrawable_, nullptr);
    auto& rtThread = RSUniRenderThread::Instance();
    if (!rtThread.renderThreadParams_) {
        rtThread.renderThreadParams_ = std::make_unique<RSRenderThreadParams>();
    }
    NodeId id = 1;
    auto surfaceNode1 = std::make_shared<RSSurfaceRenderNode>(id);
    id = 2;
    auto surfaceNode2 = std::make_shared<RSSurfaceRenderNode>(id);
    surfaceNode2->InitRenderParams();
    rtThread.renderThreadParams_->hardwareEnabledTypeNodes_.push_back(surfaceNode1);
    rtThread.renderThreadParams_->hardwareEnabledTypeNodes_.push_back(surfaceNode2);
    ASSERT_NE(renderNode_, nullptr);
    renderNode_->GetMutableRenderProperties().SetFrameWidth(DEFAULT_CANVAS_SIZE);
    renderNode_->GetMutableRenderProperties().SetFrameHeight(DEFAULT_CANVAS_SIZE);
    displayDrawable_->ClearTransparentBeforeSaveLayer();
}

/**
 * @tc.name: DrawCurtainScreen
 * @tc.desc: Test DrawCurtainScreen
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSDisplayRenderNodeDrawableTest, DrawCurtainScreen, TestSize.Level1)
{
    ASSERT_NE(displayDrawable_, nullptr);
    displayDrawable_->DrawCurtainScreen();
}

/**
 * @tc.name: DrawWatermarkIfNeed
 * @tc.desc: Test DrawWatermarkIfNeed
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSDisplayRenderNodeDrawableTest, DrawWatermarkIfNeed, TestSize.Level1)
{
    ASSERT_NE(displayDrawable_, nullptr);
    ASSERT_NE(canvas_, nullptr);
    auto params = static_cast<RSDisplayRenderParams*>(displayDrawable_->GetRenderParams().get());
    if (params) {
        displayDrawable_->DrawWatermarkIfNeed(*params, *canvas_);
    }
}

/**
 * @tc.name: RotateMirrorCanvasIfNeed001
 * @tc.desc: Test RotateMirrorCanvasIfNeed, if canvas doesn't need to be rotated.
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSDisplayRenderNodeDrawableTest, RotateMirrorCanvasIfNeed001, TestSize.Level1)
{
    ASSERT_NE(renderNode_, nullptr);
    ASSERT_NE(displayDrawable_, nullptr);
    renderNode_->mirrorSource_ = mirroredNode_;
    displayDrawable_->canvasRotation_ = true;
    displayDrawable_->exFoldScreen_ = true;
    displayDrawable_->RotateMirrorCanvasIfNeed(*renderNode_);

    displayDrawable_->canvasRotation_ = false;
    displayDrawable_->exFoldScreen_ = false;
    displayDrawable_->RotateMirrorCanvasIfNeed(*renderNode_);
}

/**
 * @tc.name: RotateMirrorCanvasIfNeed002
 * @tc.desc: Test RotateMirrorCanvasIfNeed, if canvas needs to be rotated.
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSDisplayRenderNodeDrawableTest, RotateMirrorCanvasIfNeed002, TestSize.Level1)
{
    ASSERT_NE(renderNode_, nullptr);
    ASSERT_NE(displayDrawable_, nullptr);
    renderNode_->mirrorSource_ = mirroredNode_;
    displayDrawable_->canvasRotation_ = true;
    displayDrawable_->exFoldScreen_ = false;
    displayDrawable_->RotateMirrorCanvasIfNeed(*renderNode_);

    displayDrawable_->canvasRotation_ = false;
    displayDrawable_->exFoldScreen_ = true;
    displayDrawable_->RotateMirrorCanvasIfNeed(*renderNode_);
}

/**
 * @tc.name: ScaleMirrorIfNeed001
 * @tc.desc: Test ScaleMirrorIfNeed001, with canvas rotation
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSDisplayRenderNodeDrawableTest, ScaleMirrorIfNeed001, TestSize.Level1)
{
    ASSERT_NE(renderNode_, nullptr);
    ASSERT_NE(displayDrawable_, nullptr);
    auto processor = std::make_shared<RSUniRenderVirtualProcessor>();
    renderNode_->mirrorSource_ = mirroredNode_;

    displayDrawable_->canvasRotation_ = true;
    displayDrawable_->exFoldScreen_ = true;
    displayDrawable_->ScaleMirrorIfNeed(*renderNode_, processor);

    displayDrawable_->canvasRotation_ = true;
    displayDrawable_->exFoldScreen_ = false;
    displayDrawable_->ScaleMirrorIfNeed(*renderNode_, processor);
}

/**
 * @tc.name: ScaleMirrorIfNeed002
 * @tc.desc: Test ScaleMirrorIfNeed, without canvas rotation
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSDisplayRenderNodeDrawableTest, ScaleMirrorIfNeed002, TestSize.Level1)
{
    ASSERT_NE(renderNode_, nullptr);
    ASSERT_NE(displayDrawable_, nullptr);
    auto processor = std::make_shared<RSUniRenderVirtualProcessor>();
    renderNode_->mirrorSource_ = mirroredNode_;

    displayDrawable_->canvasRotation_ = false;
    displayDrawable_->exFoldScreen_ = true;
    displayDrawable_->ScaleMirrorIfNeed(*renderNode_, processor);

    displayDrawable_->canvasRotation_ = false;
    displayDrawable_->exFoldScreen_ = false;
    displayDrawable_->ScaleMirrorIfNeed(*renderNode_, processor);
}
}
