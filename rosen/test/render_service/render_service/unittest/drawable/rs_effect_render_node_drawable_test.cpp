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
#include "drawable/rs_effect_render_node_drawable.h"
#include "params/rs_render_thread_params.h"
#include "pipeline/rs_context.h"
#include "pipeline/rs_effect_render_node.h"
#include "pipeline/rs_render_node_gc.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS::Rosen::DrawableV2;

namespace OHOS::Rosen {
constexpr int32_t DEFAULT_CANVAS_SIZE = 100;
constexpr NodeId DEFAULT_ID = 0xFFFF;

class RSEffectRenderNodeDrawableTest : public testing::Test {
public:
    std::shared_ptr<RSEffectRenderNode> renderNode_;
    RSRenderNodeDrawableAdapter* drawable_ = nullptr;
    RSEffectRenderNodeDrawable* effectDrawable_ = nullptr;

    std::shared_ptr<Drawing::Canvas> drawingCanvas_;
    std::shared_ptr<RSPaintFilterCanvas> canvas_;
      
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSEffectRenderNodeDrawableTest::SetUpTestCase()
{
    auto& renderNodeGC = RSRenderNodeGC::Instance();
    renderNodeGC.nodeBucket_ = std::queue<std::vector<RSRenderNode*>>();
    renderNodeGC.drawableBucket_ = std::queue<std::vector<DrawableV2::RSRenderNodeDrawableAdapter*>>();
}
void RSEffectRenderNodeDrawableTest::TearDownTestCase() {}
void RSEffectRenderNodeDrawableTest::SetUp()
{
    renderNode_ = std::make_shared<RSEffectRenderNode>(DEFAULT_ID);
    if (!renderNode_) {
        RS_LOGE("RSEffectRenderNodeDrawableTest:failed to create effect node.");
    }
    drawable_ = RSEffectRenderNodeDrawable::OnGenerate(renderNode_);
    if (drawable_) {
        drawable_->renderParams_ = std::make_unique<RSEffectRenderParams>(DEFAULT_ID);
        effectDrawable_ = static_cast<RSEffectRenderNodeDrawable*>(drawable_);
        if (!drawable_->renderParams_) {
            RS_LOGE("RSEffectRenderNodeDrawableTest:failed to init effect render params.");
        }
    }
    drawingCanvas_ = std::make_unique<Drawing::Canvas>(DEFAULT_CANVAS_SIZE, DEFAULT_CANVAS_SIZE);
    if (drawingCanvas_) {
        canvas_ = std::make_shared<RSPaintFilterCanvas>(drawingCanvas_.get());
    }
}
void RSEffectRenderNodeDrawableTest::TearDown() {}

#ifdef SUBTREE_PARALLEL_ENABLE
/**
 * @tc.name: OnDrawTest
 * @tc.desc: Test OnDraw
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSEffectRenderNodeDrawableTest, OnDrawTest, TestSize.Level1)
{
    auto effectNode = std::make_shared<RSEffectRenderNode>(DEFAULT_ID);
    ASSERT_NE(effectNode, nullptr);
    auto effectDrawable = static_cast<RSEffectRenderNodeDrawable*>(
        RSRenderNodeDrawableAdapter::OnGenerate(effectNode).get());
    ASSERT_NE(effectDrawable, nullptr);

    auto canvas = std::make_shared<Drawing::Canvas>(DEFAULT_CANVAS_SIZE, DEFAULT_CANVAS_SIZE);
    auto rsPaintFilterCanvas = std::make_shared<RSPaintFilterCanvas>(canvas.get());

    // default case, shouldpaint == false
    effectDrawable->OnDraw(*rsPaintFilterCanvas);

    //if should paint
    effectDrawable->renderParams_->shouldPaint_ = true;
    effectDrawable->renderParams_->contentEmpty_ = true;
    effectDrawable->OnDraw(*rsPaintFilterCanvas);
}
#endif

/**
 * @tc.name: OnCaptureTest
 * @tc.desc: Test If OnCapture Can Run
 * @tc.type: FUNC
 * @tc.require: issueICF7P6
 */
HWTEST_F(RSEffectRenderNodeDrawableTest, OnCapture001, TestSize.Level1)
{
    NodeId nodeId = 1;
    auto node = std::make_shared<RSRenderNode>(nodeId);
    auto drawable = std::make_shared<RSEffectRenderNodeDrawable>(std::move(node));
    int width = 1024;
    int height = 1920;
    Drawing::Canvas canvas(width, height);
    drawable->renderParams_ = nullptr;
    drawable->OnCapture(canvas);
    ASSERT_FALSE(drawable->ShouldPaint());
    drawable->renderParams_ = std::make_unique<RSRenderParams>(nodeId);
    drawable->OnCapture(canvas);
    ASSERT_FALSE(drawable->ShouldPaint());
}

/**
 * @tc.name: OnDraw
 * @tc.desc: Test OnDraw
 * @tc.type: FUNC
 * @tc.require: #ICEF7K
 */
HWTEST_F(RSEffectRenderNodeDrawableTest, OnDraw, TestSize.Level1)
{
    ASSERT_NE(effectDrawable_, nullptr);
    ASSERT_NE(drawable_->renderParams_, nullptr);
    // default case, shouldpaint == false
    effectDrawable_->OnDraw(*drawingCanvas_);

    // if should paint
    drawable_->renderParams_->shouldPaint_ = true;
    drawable_->renderParams_->contentEmpty_ = false;
    effectDrawable_->OnDraw(*drawingCanvas_);
}

/**
 * @tc.name: OnDraw002
 * @tc.desc: Test OnDraw
 * @tc.type: FUNC
 * @tc.require: #ICEF7K
 */
HWTEST_F(RSEffectRenderNodeDrawableTest, OnDrawTest002, TestSize.Level1)
{
    ASSERT_NE(effectDrawable_, nullptr);
    ASSERT_NE(drawable_->renderParams_, nullptr);
    // default case, shouldpaint == false
    effectDrawable_->OnDraw(*drawingCanvas_);

    // if should paint
    drawable_->renderParams_->shouldPaint_ = true;
    drawable_->renderParams_->contentEmpty_ = false;
    effectDrawable_->OnDraw(*drawingCanvas_);
}
}
