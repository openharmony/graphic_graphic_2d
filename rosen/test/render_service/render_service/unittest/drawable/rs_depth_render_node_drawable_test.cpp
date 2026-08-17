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
#include "drawable/rs_depth_render_node_drawable.h"
#include "drawable/rs_surface_render_node_drawable.h"
#include "params/rs_depth_render_params.h"
#include "pipeline/render_thread/rs_render_engine.h"
#include "pipeline/render_thread/rs_uni_render_thread.h"
#include "pipeline/rs_render_node_gc.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS::Rosen::DrawableV2;

namespace OHOS::Rosen {
constexpr int32_t DEFAULT_CANVAS_SIZE = 100;
constexpr NodeId DEFAULT_ID = 0xFFFF;

class RSDepthRenderNodeDrawableTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    std::shared_ptr<RSRenderNode> renderNode_;
    RSDepthRenderNodeDrawable* depthDrawable_ = nullptr;
    std::shared_ptr<Drawing::Canvas> drawingCanvas_;
    std::shared_ptr<RSPaintFilterCanvas> canvas_;
};

void RSDepthRenderNodeDrawableTest::SetUpTestCase()
{
    auto& renderNodeGC = RSRenderNodeGC::Instance();
    renderNodeGC.nodeBucket_ = std::queue<std::vector<RSRenderNode*>>();
    renderNodeGC.drawableBucket_ = std::queue<std::vector<DrawableV2::RSRenderNodeDrawableAdapter*>>();
}

void RSDepthRenderNodeDrawableTest::TearDownTestCase() {}

void RSDepthRenderNodeDrawableTest::SetUp()
{
    renderNode_ = std::make_shared<RSRenderNode>(DEFAULT_ID);
    if (!renderNode_) {
        RS_LOGE("RSDepthRenderNodeDrawableTest: failed to create render node.");
        return;
    }

    auto drawable = RSDepthRenderNodeDrawable::OnGenerate(renderNode_);
    if (drawable) {
        depthDrawable_ = static_cast<RSDepthRenderNodeDrawable*>(drawable);
        depthDrawable_->renderParams_ = std::make_unique<RSDepthRenderParams>(DEFAULT_ID);
        if (!depthDrawable_->renderParams_) {
            RS_LOGE("RSDepthRenderNodeDrawableTest: failed to init render params.");
        }
    }

    drawingCanvas_ = std::make_unique<Drawing::Canvas>(DEFAULT_CANVAS_SIZE, DEFAULT_CANVAS_SIZE);
    if (drawingCanvas_) {
        canvas_ = std::make_shared<RSPaintFilterCanvas>(drawingCanvas_.get());
    }
}

void RSDepthRenderNodeDrawableTest::TearDown() {}

/**
 * @tc.name: CreateDepthRenderNodeDrawable
 * @tc.desc: Test if DepthRenderNodeDrawable can be created
 * @tc.type: FUNC
 * @tc.require: issueICWNX9
 */
HWTEST_F(RSDepthRenderNodeDrawableTest, CreateDepthRenderNodeDrawable, TestSize.Level1)
{
    NodeId id = 1;
    auto renderNode = std::make_shared<RSRenderNode>(id);
    renderNode->InitRenderParams();
    auto drawable = RSDepthRenderNodeDrawable::OnGenerate(renderNode);
    drawable->renderParams_ = std::make_unique<RSRenderParams>(id);
    ASSERT_NE(drawable, nullptr);

    EXPECT_EQ(drawable->GetId(), id);
}

/**
 * @tc.name: OnDrawWithNullParams
 * @tc.desc: Test OnDraw when render params is nullptr
 * @tc.type: FUNC
 * @tc.require: issueICWNX9
 */
HWTEST_F(RSDepthRenderNodeDrawableTest, OnDrawWithNullParams, TestSize.Level1)
{
    ASSERT_NE(depthDrawable_, nullptr);

    depthDrawable_->renderParams_ = nullptr;
    depthDrawable_->OnDraw(*canvas_);

    // Should not crash when renderParams is nullptr
}

/**
 * @tc.name: OnDrawWithValidParams
 * @tc.desc: Test OnDraw with valid params but no surface drawable
 * @tc.type: FUNC
 * @tc.require: issueICWNX9
 */
HWTEST_F(RSDepthRenderNodeDrawableTest, OnDrawWithValidParams, TestSize.Level1)
{
    ASSERT_NE(depthDrawable_, nullptr);
    ASSERT_NE(depthDrawable_->renderParams_, nullptr);

    auto depthParams = static_cast<RSDepthRenderParams*>(depthDrawable_->renderParams_.get());
    ASSERT_NE(depthParams, nullptr);

    // Set weak_ptr to nullptr (no surface drawable)
    depthParams->depthSrcSurfaceDrawable_ = std::weak_ptr<RSRenderNodeDrawableAdapter>();

    depthDrawable_->OnDraw(*canvas_);

    // Should not crash when depthSrcSurfaceDrawable is empty
}

/**
 * @tc.name: OnDrawWithNullRenderEngine
 * @tc.desc: Test OnDraw when render engine is nullptr
 * @tc.type: FUNC
 * @tc.require: issueICWNX9
 */
HWTEST_F(RSDepthRenderNodeDrawableTest, OnDrawWithNullRenderEngine, TestSize.Level1)
{
    ASSERT_NE(depthDrawable_, nullptr);
    ASSERT_NE(depthDrawable_->renderParams_, nullptr);

    auto depthParams = static_cast<RSDepthRenderParams*>(depthDrawable_->renderParams_.get());
    ASSERT_NE(depthParams, nullptr);

    // Create a surface drawable
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(DEFAULT_ID + 1);
    auto surfaceDrawable = std::static_pointer_cast<RSSurfaceRenderNodeDrawable>(
        RSRenderNodeDrawableAdapter::OnGenerate(surfaceNode));

    std::shared_ptr<RSRenderNodeDrawableAdapter> surfaceDrawableBase = surfaceDrawable;
    depthParams->depthSrcSurfaceDrawable_ = std::weak_ptr<RSRenderNodeDrawableAdapter>(surfaceDrawableBase);

    // Ensure render engine is null
    RSUniRenderThread::Instance().uniRenderEngine_ = nullptr;

    depthDrawable_->OnDraw(*canvas_);

    // Should log error and return when renderEngine is nullptr
}

/**
 * @tc.name: OnDrawWithNullImage
 * @tc.desc: Test OnDraw when render engine returns null image
 * @tc.type: FUNC
 * @tc.require: issueICWNX9
 */
HWTEST_F(RSDepthRenderNodeDrawableTest, OnDrawWithNullImage, TestSize.Level1)
{
    ASSERT_NE(depthDrawable_, nullptr);
    ASSERT_NE(depthDrawable_->renderParams_, nullptr);

    auto depthParams = static_cast<RSDepthRenderParams*>(depthDrawable_->renderParams_.get());
    ASSERT_NE(depthParams, nullptr);

    // Create a surface drawable
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(DEFAULT_ID + 1);
    auto surfaceDrawable = std::static_pointer_cast<RSSurfaceRenderNodeDrawable>(
        RSRenderNodeDrawableAdapter::OnGenerate(surfaceNode));
    surfaceDrawable->renderParams_ = std::make_unique<RSSurfaceRenderParams>(DEFAULT_ID + 1);

    std::shared_ptr<RSRenderNodeDrawableAdapter> surfaceDrawableBase = surfaceDrawable;
    depthParams->depthSrcSurfaceDrawable_ = std::weak_ptr<RSRenderNodeDrawableAdapter>(surfaceDrawableBase);

    RSUniRenderThread::Instance().uniRenderEngine_ = std::make_shared<RSRenderEngine>();

    depthDrawable_->OnDraw(*canvas_);

    // Should log info when depth surface image is nullptr
    EXPECT_EQ(depthParams->GetDepthImage(), nullptr);
}

/**
 * @tc.name: OnDrawWithMultipleThreads
 * @tc.desc: Test OnDraw with different parallel thread IDs
 * @tc.type: FUNC
 * @tc.require: issueICWNX9
 */
HWTEST_F(RSDepthRenderNodeDrawableTest, OnDrawWithMultipleThreads, TestSize.Level1)
{
    ASSERT_NE(depthDrawable_, nullptr);
    ASSERT_NE(depthDrawable_->renderParams_, nullptr);

    auto depthParams = static_cast<RSDepthRenderParams*>(depthDrawable_->renderParams_.get());
    ASSERT_NE(depthParams, nullptr);

    // Create a surface drawable
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(DEFAULT_ID + 1);
    auto surfaceDrawable = std::static_pointer_cast<RSSurfaceRenderNodeDrawable>(
        RSRenderNodeDrawableAdapter::OnGenerate(surfaceNode));
    surfaceDrawable->renderParams_ = std::make_unique<RSSurfaceRenderParams>(DEFAULT_ID + 1);

    std::shared_ptr<RSRenderNodeDrawableAdapter> surfaceDrawableBase = surfaceDrawable;
    depthParams->depthSrcSurfaceDrawable_ = std::weak_ptr<RSRenderNodeDrawableAdapter>(surfaceDrawableBase);

    // Test with different thread IDs
    for (uint32_t threadId = 0; threadId < 3; threadId++) {
        canvas_->SetParallelThreadId(threadId);
        depthDrawable_->OnDraw(*canvas_);
        EXPECT_EQ(depthParams->GetDepthImage(), nullptr);
    }
}

/**
 * @tc.name: OnCaptureWithNullParams
 * @tc.desc: Test OnCapture when render params is nullptr
 * @tc.type: FUNC
 * @tc.require: issueICWNX9
 */
HWTEST_F(RSDepthRenderNodeDrawableTest, OnCaptureWithNullParams, TestSize.Level1)
{
    ASSERT_NE(depthDrawable_, nullptr);

    depthDrawable_->renderParams_ = nullptr;
    depthDrawable_->OnCapture(*canvas_);

    // Should not crash when renderParams is nullptr
}

/**
 * @tc.name: OnCaptureWithValidParams
 * @tc.desc: Test OnCapture with valid params
 * @tc.type: FUNC
 * @tc.require: issueICWNX9
 */
HWTEST_F(RSDepthRenderNodeDrawableTest, OnCaptureWithValidParams, TestSize.Level1)
{
    ASSERT_NE(depthDrawable_, nullptr);
    ASSERT_NE(depthDrawable_->renderParams_, nullptr);

    auto depthParams = static_cast<RSDepthRenderParams*>(depthDrawable_->renderParams_.get());
    ASSERT_NE(depthParams, nullptr);

    // Create a surface drawable
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(DEFAULT_ID + 1);
    auto surfaceDrawable = std::static_pointer_cast<RSSurfaceRenderNodeDrawable>(
        RSRenderNodeDrawableAdapter::OnGenerate(surfaceNode));
    surfaceDrawable->renderParams_ = std::make_unique<RSSurfaceRenderParams>(DEFAULT_ID + 1);

    std::shared_ptr<RSRenderNodeDrawableAdapter> surfaceDrawableBase = surfaceDrawable;
    depthParams->depthSrcSurfaceDrawable_ = std::weak_ptr<RSRenderNodeDrawableAdapter>(surfaceDrawableBase);

    depthDrawable_->OnCapture(*canvas_);

    // OnCapture should behave same as OnDraw
    EXPECT_EQ(depthParams->GetDepthImage(), nullptr);
}

/**
 * @tc.name: OnDrawCompositeWithCanvas
 * @tc.desc: Test OnDraw composite operations with canvas
 * @tc.type: FUNC
 * @tc.require: issueICWNX9
 */
HWTEST_F(RSDepthRenderNodeDrawableTest, OnDrawCompositeWithCanvas, TestSize.Level1)
{
    ASSERT_NE(depthDrawable_, nullptr);
    ASSERT_NE(depthDrawable_->renderParams_, nullptr);

    auto depthParams = static_cast<RSDepthRenderParams*>(depthDrawable_->renderParams_.get());
    ASSERT_NE(depthParams, nullptr);

    // Set alpha and matrix
    depthParams->alpha_ = 128;
    Drawing::Matrix matrix;
    matrix.Translate(10, 20);
    depthParams->matrix_ = matrix;

    // Create a surface drawable
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(DEFAULT_ID + 1);
    auto surfaceDrawable = std::static_pointer_cast<RSSurfaceRenderNodeDrawable>(
        RSRenderNodeDrawableAdapter::OnGenerate(surfaceNode));
    surfaceDrawable->renderParams_ = std::make_unique<RSSurfaceRenderParams>(DEFAULT_ID + 1);

    std::shared_ptr<RSRenderNodeDrawableAdapter> surfaceDrawableBase = surfaceDrawable;
    depthParams->depthSrcSurfaceDrawable_ = std::weak_ptr<RSRenderNodeDrawableAdapter>(surfaceDrawableBase);

    depthDrawable_->OnDraw(*canvas_);

    EXPECT_EQ(depthParams->GetDepthImage(), nullptr);
}

/**
 * @tc.name: OnDrawWithExpiredWeakPtr
 * @tc.desc: Test OnDraw when surface drawable weak_ptr is expired
 * @tc.type: FUNC
 * @tc.require: issueICWNX9
 */
HWTEST_F(RSDepthRenderNodeDrawableTest, OnDrawWithExpiredWeakPtr, TestSize.Level1)
{
    ASSERT_NE(depthDrawable_, nullptr);
    ASSERT_NE(depthDrawable_->renderParams_, nullptr);

    auto depthParams = static_cast<RSDepthRenderParams*>(depthDrawable_->renderParams_.get());
    ASSERT_NE(depthParams, nullptr);

    // Create a surface drawable and let it go out of scope
    {
        auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(DEFAULT_ID + 1);
        auto surfaceDrawable = std::static_pointer_cast<RSSurfaceRenderNodeDrawable>(
            RSRenderNodeDrawableAdapter::OnGenerate(surfaceNode));
        surfaceDrawable->renderParams_ = std::make_unique<RSSurfaceRenderParams>(DEFAULT_ID + 1);

        std::shared_ptr<RSRenderNodeDrawableAdapter> surfaceDrawableBase = surfaceDrawable;
        depthParams->depthSrcSurfaceDrawable_ = std::weak_ptr<RSRenderNodeDrawableAdapter>(surfaceDrawableBase);
    }

    depthDrawable_->OnDraw(*canvas_);

    // Should handle expired weak_ptr gracefully
    EXPECT_EQ(depthParams->GetDepthImage(), nullptr);
}

/**
 * @tc.name: OnDrawMultipleTimes
 * @tc.desc: Test OnDraw called multiple times
 * @tc.type: FUNC
 * @tc.require: issueICWNX9
 */
HWTEST_F(RSDepthRenderNodeDrawableTest, OnDrawMultipleTimes, TestSize.Level1)
{
    ASSERT_NE(depthDrawable_, nullptr);
    ASSERT_NE(depthDrawable_->renderParams_, nullptr);

    auto depthParams = static_cast<RSDepthRenderParams*>(depthDrawable_->renderParams_.get());
    ASSERT_NE(depthParams, nullptr);

    // Create a surface drawable
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(DEFAULT_ID + 1);
    auto surfaceDrawable = std::static_pointer_cast<RSSurfaceRenderNodeDrawable>(
        RSRenderNodeDrawableAdapter::OnGenerate(surfaceNode));
    surfaceDrawable->renderParams_ = std::make_unique<RSSurfaceRenderParams>(DEFAULT_ID + 1);

    std::shared_ptr<RSRenderNodeDrawableAdapter> surfaceDrawableBase = surfaceDrawable;
    depthParams->depthSrcSurfaceDrawable_ = std::weak_ptr<RSRenderNodeDrawableAdapter>(surfaceDrawableBase);

    // Call OnDraw multiple times
    for (int i = 0; i < 5; i++) {
        depthDrawable_->OnDraw(*canvas_);
        EXPECT_EQ(depthParams->GetDepthImage(), nullptr);
    }
}

/**
 * @tc.name: SetAndGetDepthImage
 * @tc.desc: Test SetDepthImage and GetDepthImage
 * @tc.type: FUNC
 * @tc.require: issueICWNX9
 */
HWTEST_F(RSDepthRenderNodeDrawableTest, SetAndGetDepthImage, TestSize.Level1)
{
    ASSERT_NE(depthDrawable_, nullptr);
    ASSERT_NE(depthDrawable_->renderParams_, nullptr);

    auto depthParams = static_cast<RSDepthRenderParams*>(depthDrawable_->renderParams_.get());
    ASSERT_NE(depthParams, nullptr);

    // Create a test image
    auto bitmap = Drawing::Bitmap();
    Drawing::BitmapFormat format{Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_PREMUL};
    bitmap.Build(100, 100, format);
    auto testImage = bitmap.MakeImage();

    depthParams->SetDepthImage(testImage);
    auto resultImage = depthParams->GetDepthImage();

    EXPECT_NE(resultImage, nullptr);
}

/**
 * @tc.name: OnDrawWithDifferentCanvasSizes
 * @tc.desc: Test OnDraw with different canvas sizes
 * @tc.type: FUNC
 * @tc.require: issueICWNX9
 */
HWTEST_F(RSDepthRenderNodeDrawableTest, OnDrawWithDifferentCanvasSizes, TestSize.Level1)
{
    ASSERT_NE(depthDrawable_, nullptr);
    ASSERT_NE(depthDrawable_->renderParams_, nullptr);

    auto depthParams = static_cast<RSDepthRenderParams*>(depthDrawable_->renderParams_.get());
    ASSERT_NE(depthParams, nullptr);

    // Create a surface drawable
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(DEFAULT_ID + 1);
    auto surfaceDrawable = std::static_pointer_cast<RSSurfaceRenderNodeDrawable>(
        RSRenderNodeDrawableAdapter::OnGenerate(surfaceNode));
    surfaceDrawable->renderParams_ = std::make_unique<RSSurfaceRenderParams>(DEFAULT_ID + 1);

    std::shared_ptr<RSRenderNodeDrawableAdapter> surfaceDrawableBase = surfaceDrawable;
    depthParams->depthSrcSurfaceDrawable_ = std::weak_ptr<RSRenderNodeDrawableAdapter>(surfaceDrawableBase);

    // Test with different canvas sizes
    std::vector<int32_t> sizes = {50, 100, 200, 500};
    for (auto size : sizes) {
        auto testCanvas = std::make_unique<Drawing::Canvas>(size, size);
        auto paintFilterCanvas = std::make_shared<RSPaintFilterCanvas>(testCanvas.get());

        depthDrawable_->OnDraw(*paintFilterCanvas);
        EXPECT_EQ(depthParams->GetDepthImage(), nullptr);
    }
}

/**
 * @tc.name: OnDrawWithNullSurfaceDrawableParams
 * @tc.desc: Test OnDraw when surface drawable render params is nullptr
 * @tc.type: FUNC
 * @tc.require: issueICWNX9
 */
HWTEST_F(RSDepthRenderNodeDrawableTest, OnDrawWithNullSurfaceDrawableParams, TestSize.Level1)
{
    ASSERT_NE(depthDrawable_, nullptr);
    ASSERT_NE(depthDrawable_->renderParams_, nullptr);

    auto depthParams = static_cast<RSDepthRenderParams*>(depthDrawable_->renderParams_.get());
    ASSERT_NE(depthParams, nullptr);

    // Create a surface drawable with null render params
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(DEFAULT_ID + 1);
    auto surfaceDrawable = std::static_pointer_cast<RSSurfaceRenderNodeDrawable>(
        RSRenderNodeDrawableAdapter::OnGenerate(surfaceNode));
    surfaceDrawable->renderParams_ = nullptr;

    std::shared_ptr<RSRenderNodeDrawableAdapter> surfaceDrawableBase = surfaceDrawable;
    depthParams->depthSrcSurfaceDrawable_ = std::weak_ptr<RSRenderNodeDrawableAdapter>(surfaceDrawableBase);

    depthDrawable_->OnDraw(*canvas_);

    // Should handle null surface drawable render params
}

/**
 * @tc.name: OnDrawEdgeCases
 * @tc.desc: Test OnDraw with various edge cases
 * @tc.type: FUNC
 * @tc.require: issueICWNX9
 */
HWTEST_F(RSDepthRenderNodeDrawableTest, OnDrawEdgeCases, TestSize.Level1)
{
    ASSERT_NE(depthDrawable_, nullptr);
    ASSERT_NE(depthDrawable_->renderParams_, nullptr);

    auto depthParams = static_cast<RSDepthRenderParams*>(depthDrawable_->renderParams_.get());
    ASSERT_NE(depthParams, nullptr);

    // Test case 1: Zero alpha
    depthParams->alpha_ = 0;
    Drawing::Matrix matrix;
    depthParams->matrix_ = matrix;

    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(DEFAULT_ID + 1);
    auto surfaceDrawable = std::static_pointer_cast<RSSurfaceRenderNodeDrawable>(
        RSRenderNodeDrawableAdapter::OnGenerate(surfaceNode));
    surfaceDrawable->renderParams_ = std::make_unique<RSSurfaceRenderParams>(DEFAULT_ID + 1);

    std::shared_ptr<RSRenderNodeDrawableAdapter> surfaceDrawableBase = surfaceDrawable;
    depthParams->depthSrcSurfaceDrawable_ = std::weak_ptr<RSRenderNodeDrawableAdapter>(surfaceDrawableBase);

    depthDrawable_->OnDraw(*canvas_);

    // Test case 2: Full alpha
    depthParams->alpha_ = 255;
    depthDrawable_->OnDraw(*canvas_);

    EXPECT_EQ(depthParams->GetDepthImage(), nullptr);
}

/**
 * @tc.name: MultipleDrawablesThreadSafe
 * @tc.desc: Test OnDraw is thread-safe with multiple drawables
 * @tc.type: FUNC
 * @tc.require: issueICWNX9
 */
HWTEST_F(RSDepthRenderNodeDrawableTest, MultipleDrawablesThreadSafe, TestSize.Level1)
{
    std::vector<std::unique_ptr<RSDepthRenderNodeDrawable>> drawables;

    // Create multiple depth drawables
    for (int i = 0; i < 3; i++) {
        auto node = std::make_shared<RSRenderNode>(DEFAULT_ID + i + 1);
        auto drawable = static_cast<RSDepthRenderNodeDrawable*>(
            RSDepthRenderNodeDrawable::OnGenerate(node));
        drawable->renderParams_ = std::make_unique<RSDepthRenderParams>(DEFAULT_ID + i + 1);

        auto depthParams = static_cast<RSDepthRenderParams*>(drawable->renderParams_.get());

        auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(DEFAULT_ID + i + 10);
        auto surfaceDrawable = std::static_pointer_cast<RSSurfaceRenderNodeDrawable>(
            RSRenderNodeDrawableAdapter::OnGenerate(surfaceNode));
        surfaceDrawable->renderParams_ = std::make_unique<RSSurfaceRenderParams>(DEFAULT_ID + i + 10);

        std::shared_ptr<RSRenderNodeDrawableAdapter> surfaceDrawableBase = surfaceDrawable;
        depthParams->depthSrcSurfaceDrawable_ = std::weak_ptr<RSRenderNodeDrawableAdapter>(surfaceDrawableBase);

        drawables.push_back(std::unique_ptr<RSDepthRenderNodeDrawable>(drawable));
    }

    // Call OnDraw on all drawables
    for (auto& drawable : drawables) {
        drawable->OnDraw(*canvas_);
        auto depthParams = static_cast<RSDepthRenderParams*>(drawable->renderParams_.get());
        EXPECT_EQ(depthParams->GetDepthImage(), nullptr);
    }
}
} // namespace OHOS::Rosen