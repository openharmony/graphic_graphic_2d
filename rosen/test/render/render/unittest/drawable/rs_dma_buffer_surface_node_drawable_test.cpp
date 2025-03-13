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

#include "drawable/rs_render_node_drawable.h"
#include "drawable/rs_surface_render_node_drawable.h"
#include "pipeline/render_thread/rs_uni_render_engine.h"
#include "pipeline/render_thread/rs_uni_render_thread.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS::Rosen::DrawableV2;

namespace OHOS::Rosen {
constexpr int32_t DEFAULT_CANVAS_SIZE = 100;
constexpr NodeId DEFAULT_ID = 0xFFFF;
class RSDmaBufferSurfaceNodeDrawableTest : public testing::Test {
public:
    std::shared_ptr<RSSurfaceRenderNode> renderNode_;
    std::shared_ptr<RSSurfaceRenderNodeDrawable> surfaceDrawable_;
    std::shared_ptr<RSPaintFilterCanvas> canvas_;
    std::shared_ptr<Drawing::Canvas> drawingCanvas_;

    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSDmaBufferSurfaceNodeDrawableTest::SetUp()
{
    renderNode_ = std::make_shared<RSSurfaceRenderNode>(DEFAULT_ID);
    if (!renderNode_) {
        RS_LOGE("RSSurfaceRenderNodeDrawableTest: failed to create surface node.");
        return;
    }
    surfaceDrawable_ = std::static_pointer_cast<RSSurfaceRenderNodeDrawable>(
        DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(renderNode_));
    if (!surfaceDrawable_ || !surfaceDrawable_->renderParams_) {
        RS_LOGE("RSSurfaceRenderNodeDrawableTest: failed to init render params.");
        return;
    }
    drawingCanvas_ = std::make_unique<Drawing::Canvas>(DEFAULT_CANVAS_SIZE, DEFAULT_CANVAS_SIZE);
    if (drawingCanvas_) {
        canvas_ = std::make_shared<RSPaintFilterCanvas>(drawingCanvas_.get());
    }
}
void RSDmaBufferSurfaceNodeDrawableTest::SetUpTestCase() {}
void RSDmaBufferSurfaceNodeDrawableTest::TearDownTestCase() {}
void RSDmaBufferSurfaceNodeDrawableTest::TearDown() {}

/**
 * @tc.name: GetFrameBufferRequestConfigExtTest
 * @tc.desc: Test If GetFrameBufferRequestConfig Can Run
 * @tc.type: FUNC
 * @tc.require: issueIAH6OI
 */
HWTEST_F(RSDmaBufferSurfaceNodeDrawableTest, GetFrameBufferRequestConfigExtTest, TestSize.Level1)
{
    ASSERT_NE(surfaceDrawable_, nullptr);
    BufferRequestConfig config = surfaceDrawable_->GetFrameBufferRequestConfig();
    ASSERT_FALSE(config.timeout);
}

/**
 * @tc.name: UseDmaBufferExtTest
 * @tc.desc: Test If UseDmaBuffer Can Run
 * @tc.type: FUNC
 * @tc.require: issueIAH6OI
 */
HWTEST_F(RSDmaBufferSurfaceNodeDrawableTest, UseDmaBufferExtTest, TestSize.Level1)
{
    ASSERT_NE(surfaceDrawable_, nullptr);
    bool result = surfaceDrawable_->UseDmaBuffer();
    ASSERT_FALSE(result);
}

/**
 * @tc.name: CreateSurfaceExtTest
 * @tc.desc: Test If CreateSurface Can Run
 * @tc.type: FUNC
 * @tc.require: issueIAH6OI
 */
HWTEST_F(RSDmaBufferSurfaceNodeDrawableTest, CreateSurfaceExtTest, TestSize.Level1)
{
    ASSERT_NE(surfaceDrawable_, nullptr);
    ASSERT_FALSE(surfaceDrawable_->surfaceHandlerUiFirst_->GetConsumer());
    ASSERT_FALSE(surfaceDrawable_->surface_);
    bool result = surfaceDrawable_->CreateSurface();
    ASSERT_TRUE(result);
    ASSERT_TRUE(surfaceDrawable_->surfaceHandlerUiFirst_->GetConsumer());
    ASSERT_TRUE(surfaceDrawable_->surface_);

    surfaceDrawable_->surface_ = nullptr;
    result = surfaceDrawable_->CreateSurface();
    ASSERT_TRUE(result);
    ASSERT_TRUE(surfaceDrawable_->surfaceHandlerUiFirst_->GetConsumer());
    ASSERT_TRUE(surfaceDrawable_->surface_);
    result = surfaceDrawable_->CreateSurface();
    ASSERT_TRUE(result);
}

/**
 * @tc.name: DrawDmaBufferWithGPUExtTest
 * @tc.desc: Test If DrawDmaBufferWithGPU Can Run
 * @tc.type: FUNC
 * @tc.require: issueIAH6OI
 */
HWTEST_F(RSDmaBufferSurfaceNodeDrawableTest, DrawDmaBufferWithGPUExtTest, TestSize.Level1)
{
    ASSERT_NE(surfaceDrawable_, nullptr);
    surfaceDrawable_->surfaceHandlerUiFirst_->buffer_.buffer = OHOS::SurfaceBuffer::Create();
    RSUniRenderThread::Instance().uniRenderEngine_ = std::make_shared<RSUniRenderEngine>();
    surfaceDrawable_->DrawDmaBufferWithGPU(*canvas_);
    ASSERT_TRUE(surfaceDrawable_->surfaceHandlerUiFirst_->GetBuffer());
    RSUniRenderThread::Instance().uniRenderEngine_ = nullptr;
}

/**
 * @tc.name: ClearBufferQueueExtTest
 * @tc.desc: Test If ClearBufferQueue Can Run
 * @tc.type: FUNC
 * @tc.require: issueIAH6OI
 */
HWTEST_F(RSDmaBufferSurfaceNodeDrawableTest, ClearBufferQueueExtTest, TestSize.Level1)
{
    ASSERT_NE(surfaceDrawable_, nullptr);
    surfaceDrawable_->ClearBufferQueue();
    surfaceDrawable_->CreateSurface();
    ASSERT_TRUE(surfaceDrawable_->surfaceHandlerUiFirst_->GetConsumer());
    ASSERT_TRUE(surfaceDrawable_->surface_);
    surfaceDrawable_->ClearBufferQueue();
    ASSERT_FALSE(surfaceDrawable_->surfaceHandlerUiFirst_->GetConsumer());
    ASSERT_FALSE(surfaceDrawable_->surface_);
}

/**
 * @tc.name: ClipRoundRectExtTest
 * @tc.desc: Test If ClipRoundRect Can Run
 * @tc.type: FUNC
 * @tc.require: issueIAH6OI
 */
HWTEST_F(RSDmaBufferSurfaceNodeDrawableTest, ClipRoundRectExtTest, TestSize.Level1)
{
    ASSERT_NE(surfaceDrawable_, nullptr);
    ASSERT_NE(drawingCanvas_, nullptr);
    ASSERT_FALSE(!surfaceDrawable_->uifirstRenderParams_);
    surfaceDrawable_->ClipRoundRect(*drawingCanvas_);
    surfaceDrawable_->uifirstRenderParams_ = nullptr;
    surfaceDrawable_->ClipRoundRect(*drawingCanvas_);
    ASSERT_TRUE(!surfaceDrawable_->uifirstRenderParams_);
}
}
