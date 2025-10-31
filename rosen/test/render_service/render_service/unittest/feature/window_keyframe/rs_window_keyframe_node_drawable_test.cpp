/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include <gmock/gmock.h>
#include "gtest/gtest.h"
#include "drawable/rs_root_render_node_drawable.h"
#include "feature/window_keyframe/rs_window_keyframe_render_node.h"
#include "feature/window_keyframe/rs_window_keyframe_node_drawable.h"
#include "params/rs_render_params.h"
#include "params/rs_render_thread_params.h"
#include "pipeline/main_thread/rs_main_thread.h"
#include "pipeline/rs_context.h"
#include "pipeline/rs_root_render_node.h"
#include "pipeline/rs_surface_render_node.h"
#include "pipeline/rs_paint_filter_canvas.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS::Rosen::DrawableV2;

namespace OHOS::Rosen {

constexpr int32_t DEFAULT_CANVAS_SIZE = 100;
constexpr NodeId DEFAULT_ID = 0xFFFF;

class MockRSPaintFilterCanvas : public RSPaintFilterCanvas {
public:
    explicit MockRSPaintFilterCanvas(Drawing::Canvas* canvas) : RSPaintFilterCanvas(canvas) {}
    explicit MockRSPaintFilterCanvas(Drawing::Surface* surface) : RSPaintFilterCanvas(surface) {}
    MOCK_METHOD(Drawing::Surface*, GetSurface, (), (const));
};

class RSWindowKeyFrameDrawableTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    std::shared_ptr<RSWindowKeyFrameRenderNode> keyframeNode_;
    std::shared_ptr<RSWindowKeyFrameNodeDrawable> keyframeDrawable_ = nullptr;
};

void RSWindowKeyFrameDrawableTest::SetUpTestCase() {}
void RSWindowKeyFrameDrawableTest::TearDownTestCase() {}
void RSWindowKeyFrameDrawableTest::SetUp()
{
    keyframeNode_ = std::make_shared<RSWindowKeyFrameRenderNode>(DEFAULT_ID);
    ASSERT_NE(keyframeNode_, nullptr);

    keyframeDrawable_ = std::static_pointer_cast<RSWindowKeyFrameNodeDrawable>(
        DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(keyframeNode_));
    ASSERT_NE(keyframeDrawable_, nullptr);
    ASSERT_NE(keyframeDrawable_->renderParams_, nullptr);
}

void RSWindowKeyFrameDrawableTest::TearDown()
{
    keyframeDrawable_.reset();
    keyframeNode_.reset();
}

/**
 * @tc.name: OnDraw
 * @tc.desc: Test OnDraw
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSWindowKeyFrameDrawableTest, OnDraw, TestSize.Level1)
{
    auto surface = Drawing::Surface::MakeRasterN32Premul(DEFAULT_CANVAS_SIZE, DEFAULT_CANVAS_SIZE);
    ASSERT_NE(surface, nullptr);
    RSPaintFilterCanvas canvas(surface.get());

    keyframeDrawable_->renderParams_->shouldPaint_ = false;
    keyframeDrawable_->OnDraw(canvas);
    EXPECT_EQ(keyframeDrawable_->GetDrawSkipType(), DrawSkipType::SHOULD_NOT_PAINT);

    keyframeDrawable_->renderParams_->shouldPaint_ = true;
    keyframeDrawable_->OnDraw(canvas);
    EXPECT_EQ(keyframeDrawable_->GetDrawSkipType(), DrawSkipType::SHOULD_NOT_PAINT);

    Drawing::Bitmap bmp;
    Drawing::BitmapFormat format { Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_PREMUL };
    int32_t width = DEFAULT_CANVAS_SIZE;
    int32_t height = DEFAULT_CANVAS_SIZE;
    bmp.Build(width, height, format);
    bmp.ClearWithColor(Drawing::Color::COLOR_RED);
    keyframeDrawable_->preCachedOffscreenImg_ = bmp.MakeImage();
    keyframeDrawable_->OnDraw(canvas);
    EXPECT_EQ(keyframeDrawable_->GetDrawSkipType(), DrawSkipType::SHOULD_NOT_PAINT);

    keyframeDrawable_->renderParams_->SetBoundsRect(Drawing::RectF(0.0f, 0.0f, 1.0f, 2.0f));
    keyframeDrawable_->OnDraw(canvas);
    EXPECT_EQ(keyframeDrawable_->GetDrawSkipType(), DrawSkipType::NONE);

    keyframeDrawable_->renderParams_->SetBoundsRect(Drawing::RectF(0.0f, 0.0f, 2.0f, 1.0f));
    keyframeDrawable_->OnDraw(canvas);
    EXPECT_EQ(keyframeDrawable_->GetDrawSkipType(), DrawSkipType::NONE);
}

/**
 * @tc.name: CheckAndDrawToOffscreen
 * @tc.desc: Test CheckAndDrawToOffscreen
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSWindowKeyFrameDrawableTest, CheckAndDrawToOffscreen, TestSize.Level1)
{
    auto surface = Drawing::Surface::MakeRasterN32Premul(DEFAULT_CANVAS_SIZE, DEFAULT_CANVAS_SIZE);
    ASSERT_NE(surface, nullptr);
    RSPaintFilterCanvas canvas(surface.get());

    NodeId rootNodeId = DEFAULT_ID + 1;
    auto rootNode = std::make_shared<RSRootRenderNode>(rootNodeId);
    ASSERT_NE(rootNode, nullptr);
    auto rootDrawable = std::static_pointer_cast<RSRootRenderNodeDrawable>(
        DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(rootNode));
    ASSERT_NE(rootDrawable, nullptr);
    ASSERT_NE(rootDrawable->renderParams_, nullptr);

    auto rpBak = std::move(rootDrawable->renderParams_);
    rootDrawable->OnDraw(canvas);

    rootDrawable->renderParams_ = std::move(rpBak);
    rootDrawable->OnDraw(canvas);

    rootDrawable->renderParams_->SetWindowKeyFrameNodeDrawable(rootDrawable);
    EXPECT_FALSE(RSWindowKeyFrameNodeDrawable::CheckAndDrawToOffscreen(canvas, *rootDrawable));

    rootDrawable->renderParams_->SetWindowKeyFrameNodeDrawable(keyframeDrawable_);
    rootDrawable->OnDraw(canvas);
    EXPECT_FALSE(RSWindowKeyFrameNodeDrawable::CheckAndDrawToOffscreen(canvas, *rootDrawable));

    auto renderThreadParams = std::make_unique<RSRenderThreadParams>();
    ASSERT_NE(renderThreadParams, nullptr);
    RSUniRenderThread::Instance().Sync(std::move(renderThreadParams));
    rootDrawable->renderParams_->SetBoundsRect(Drawing::RectF(0.0f, 0.0f, 1.0f, 1.0f));
    rootDrawable->OnDraw(canvas);
    EXPECT_TRUE(RSWindowKeyFrameNodeDrawable::CheckAndDrawToOffscreen(canvas, *rootDrawable));
    RSUniRenderThread::Instance().Sync(std::unique_ptr<RSRenderThreadParams>());
}

/**
 * @tc.name: OnLinkedNodeDraw
 * @tc.desc: Test OnLinkedNodeDraw
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSWindowKeyFrameDrawableTest, OnLinkedNodeDraw, TestSize.Level1)
{
    auto surface = Drawing::Surface::MakeRasterN32Premul(DEFAULT_CANVAS_SIZE, DEFAULT_CANVAS_SIZE);
    ASSERT_NE(surface, nullptr);
    RSPaintFilterCanvas canvas(surface.get());

    NodeId rootNodeId = DEFAULT_ID + 1;
    auto rootNode = std::make_shared<RSRootRenderNode>(rootNodeId);
    ASSERT_NE(rootNode, nullptr);
    auto rootDrawable = std::static_pointer_cast<RSRootRenderNodeDrawable>(
        DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(rootNode));
    ASSERT_NE(rootDrawable, nullptr);
    ASSERT_NE(rootDrawable->renderParams_, nullptr);

    auto kfRpBak = std::move(keyframeDrawable_->renderParams_);
    EXPECT_FALSE(keyframeDrawable_->OnLinkedNodeDraw(canvas, *rootDrawable, *rootDrawable->renderParams_));

    keyframeDrawable_->renderParams_ = std::move(kfRpBak);
    EXPECT_FALSE(keyframeDrawable_->OnLinkedNodeDraw(canvas, *rootDrawable, *rootDrawable->renderParams_));

    auto renderThreadParams = std::make_unique<RSRenderThreadParams>();
    ASSERT_NE(renderThreadParams, nullptr);
    RSUniRenderThread::Instance().Sync(std::move(renderThreadParams));
    EXPECT_FALSE(keyframeDrawable_->OnLinkedNodeDraw(canvas, *rootDrawable, *rootDrawable->renderParams_));

    rootDrawable->renderParams_->SetBoundsRect(Drawing::RectF(0.0f, 0.0f, 1.0f, 1.0f));
    EXPECT_TRUE(keyframeDrawable_->OnLinkedNodeDraw(canvas, *rootDrawable, *rootDrawable->renderParams_));

    keyframeDrawable_->renderParams_->SetBoundsRect(Drawing::RectF(0.0f, 0.0f, 1.0f, 1.0f));
    EXPECT_TRUE(keyframeDrawable_->OnLinkedNodeDraw(canvas, *rootDrawable, *rootDrawable->renderParams_));

    keyframeDrawable_->renderParams_->freezeFlag_ = true;
    EXPECT_TRUE(keyframeDrawable_->OnLinkedNodeDraw(canvas, *rootDrawable, *rootDrawable->renderParams_));
}

/**
 * @tc.name: OnLinkedNodeDrawWithNullSurface
 * @tc.desc: Test OnLinkedNodeDraw with null surface
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSWindowKeyFrameDrawableTest, OnLinkedNodeDrawWithNullSurface, TestSize.Level1)
{
    NodeId rootNodeId = DEFAULT_ID + 1;
    auto rootNode = std::make_shared<RSRootRenderNode>(rootNodeId);
    ASSERT_NE(rootNode, nullptr);
    auto rootDrawable = std::static_pointer_cast<RSRootRenderNodeDrawable>(
        DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(rootNode));
    ASSERT_NE(rootDrawable, nullptr);
    ASSERT_NE(rootDrawable->renderParams_, nullptr);

    Drawing::Canvas canvas(DEFAULT_CANVAS_SIZE, DEFAULT_CANVAS_SIZE);
    MockRSPaintFilterCanvas mockRSPaintFilterCanvas(&canvas);
    EXPECT_CALL(mockRSPaintFilterCanvas, GetSurface()).Times(AnyNumber()).WillOnce(Return(nullptr));

    auto renderThreadParams = std::make_unique<RSRenderThreadParams>();
    ASSERT_NE(renderThreadParams, nullptr);
    RSUniRenderThread::Instance().Sync(std::move(renderThreadParams));
    EXPECT_FALSE(keyframeDrawable_->OnLinkedNodeDraw(mockRSPaintFilterCanvas,
        *rootDrawable, *rootDrawable->renderParams_));
}

/**
 * @tc.name: DrawOffscreenBuffer
 * @tc.desc: Test DrawOffscreenBuffer
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSWindowKeyFrameDrawableTest, DrawOffscreenBuffer, TestSize.Level1)
{
    Drawing::Canvas canvas;
    Drawing::Surface emptySurface;
    RSRenderParams rsParams(DEFAULT_ID);
    EXPECT_FALSE(keyframeDrawable_->DrawOffscreenBuffer(canvas, emptySurface, rsParams));
}

} // namespace OHOS::Rosen
