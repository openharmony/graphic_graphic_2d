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
class RSDmaBufferSurfaceRenderNodeDrawableTest : public testing::Test {
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

void RSDmaBufferSurfaceRenderNodeDrawableTest::SetUpTestCase() {}
void RSDmaBufferSurfaceRenderNodeDrawableTest::TearDownTestCase() {}
void RSDmaBufferSurfaceRenderNodeDrawableTest::SetUp()
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
void RSDmaBufferSurfaceRenderNodeDrawableTest::TearDown() {}

/**
 * @tc.name: UseDmaBuffer
 * @tc.desc: Test If UseDmaBuffer Can Run
 * @tc.type: FUNC
 * @tc.require: issueIAH6OI
 */
HWTEST_F(RSDmaBufferSurfaceRenderNodeDrawableTest, UseDmaBufferTest, TestSize.Level1)
{
    ASSERT_NE(surfaceDrawable_, nullptr);
    bool result = surfaceDrawable_->UseDmaBuffer();
    ASSERT_FALSE(result);
}

/**
 * @tc.name: GetFrameBufferRequestConfig
 * @tc.desc: Test If GetFrameBufferRequestConfig Can Run
 * @tc.type: FUNC
 * @tc.require: issueIAH6OI
 */
HWTEST_F(RSDmaBufferSurfaceRenderNodeDrawableTest, GetFrameBufferRequestConfigTest, TestSize.Level1)
{
    ASSERT_NE(surfaceDrawable_, nullptr);
    BufferRequestConfig config = surfaceDrawable_->GetFrameBufferRequestConfig();
    ASSERT_FALSE(config.timeout);
}

/**
 * @tc.name: RequestFrame
 * @tc.desc: Test If RequestFrame Can Run
 * @tc.type: FUNC
 * @tc.require: issueIAH6OI
 */
HWTEST_F(RSDmaBufferSurfaceRenderNodeDrawableTest, RequestFrameTest, TestSize.Level1)
{
    ASSERT_NE(surfaceDrawable_, nullptr);
    RenderContext* renderContext = nullptr;
    std::shared_ptr<Drawing::GPUContext> skContext = std::make_shared<Drawing::GPUContext>();
    auto result = surfaceDrawable_->RequestFrame(renderContext, skContext);
    ASSERT_FALSE(result);

    surfaceDrawable_->CreateSurface();
    ASSERT_TRUE(surfaceDrawable_->surfaceHandlerUiFirst_->GetConsumer());
    ASSERT_TRUE(surfaceDrawable_->surface_);
    result = surfaceDrawable_->RequestFrame(renderContext, skContext);
    ASSERT_FALSE(result);
}

/**
 * @tc.name: CreateSurface
 * @tc.desc: Test If CreateSurface Can Run
 * @tc.type: FUNC
 * @tc.require: issueIAH6OI
 */
HWTEST_F(RSDmaBufferSurfaceRenderNodeDrawableTest, CreateSurfaceTest, TestSize.Level1)
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
 * @tc.name: DrawUIFirstCacheWithDma
 * @tc.desc: Test If DrawUIFirstCacheWithDma Can Run
 * @tc.type: FUNC
 * @tc.require: issueIAH6OI
 */
HWTEST_F(RSDmaBufferSurfaceRenderNodeDrawableTest, DrawUIFirstCacheWithDmaTest, TestSize.Level1)
{
    ASSERT_NE(surfaceDrawable_, nullptr);
    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(surfaceDrawable_->renderParams_.get());
    ASSERT_NE(surfaceParams, nullptr);
    bool result = surfaceDrawable_->DrawUIFirstCacheWithDma(*canvas_, *surfaceParams);
    ASSERT_FALSE(result);

    surfaceDrawable_->surfaceHandlerUiFirst_->bufferAvailableCount_ = DEFAULT_CANVAS_SIZE;
    result = surfaceDrawable_->DrawUIFirstCacheWithDma(*canvas_, *surfaceParams);
    ASSERT_FALSE(result);
    surfaceDrawable_->surfaceHandlerUiFirst_->buffer_.buffer = OHOS::SurfaceBuffer::Create();
    result = surfaceDrawable_->DrawUIFirstCacheWithDma(*canvas_, *surfaceParams);
    ASSERT_FALSE(result);

    surfaceDrawable_->surfaceHandlerUiFirst_->bufferAvailableCount_ = 0;
    RSUniRenderThread::Instance().uniRenderEngine_ = std::make_shared<RSUniRenderEngine>();
    result = surfaceDrawable_->DrawUIFirstCacheWithDma(*canvas_, *surfaceParams);
    ASSERT_TRUE(result);
    RSUniRenderThread::Instance().uniRenderEngine_ = nullptr;
    surfaceParams->isHardwareEnabled_ = true;
    result = surfaceDrawable_->DrawUIFirstCacheWithDma(*canvas_, *surfaceParams);
    ASSERT_TRUE(result);
}

/**
 * @tc.name: DrawDmaBufferWithGPU
 * @tc.desc: Test If DrawDmaBufferWithGPU Can Run
 * @tc.type: FUNC
 * @tc.require: issueIAH6OI
 */
HWTEST_F(RSDmaBufferSurfaceRenderNodeDrawableTest, DrawDmaBufferWithGPUTest, TestSize.Level1)
{
    ASSERT_NE(surfaceDrawable_, nullptr);
    surfaceDrawable_->surfaceHandlerUiFirst_->buffer_.buffer = OHOS::SurfaceBuffer::Create();
    RSUniRenderThread::Instance().uniRenderEngine_ = std::make_shared<RSUniRenderEngine>();
    surfaceDrawable_->DrawDmaBufferWithGPU(*canvas_);
    ASSERT_TRUE(surfaceDrawable_->surfaceHandlerUiFirst_->GetBuffer());
    RSUniRenderThread::Instance().uniRenderEngine_ = nullptr;
}

/**
 * @tc.name: ClipRoundRect
 * @tc.desc: Test If ClipRoundRect Can Run
 * @tc.type: FUNC
 * @tc.require: issueIAH6OI
 */
HWTEST_F(RSDmaBufferSurfaceRenderNodeDrawableTest, ClipRoundRectTest, TestSize.Level1)
{
    ASSERT_NE(surfaceDrawable_, nullptr);
    ASSERT_NE(drawingCanvas_, nullptr);
    ASSERT_FALSE(!surfaceDrawable_->uifirstRenderParams_);
    surfaceDrawable_->ClipRoundRect(*drawingCanvas_);
    surfaceDrawable_->uifirstRenderParams_ = nullptr;
    surfaceDrawable_->ClipRoundRect(*drawingCanvas_);
    ASSERT_TRUE(!surfaceDrawable_->uifirstRenderParams_);
}

/**
 * @tc.name: ClearBufferQueue
 * @tc.desc: Test If ClearBufferQueue Can Run
 * @tc.type: FUNC
 * @tc.require: issueIAH6OI
 */
HWTEST_F(RSDmaBufferSurfaceRenderNodeDrawableTest, ClearBufferQueueTest, TestSize.Level1)
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
}
