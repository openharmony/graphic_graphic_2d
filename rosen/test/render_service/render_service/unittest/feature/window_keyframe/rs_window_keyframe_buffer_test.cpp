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
#include "feature/window_keyframe/rs_window_keyframe_buffer.h"
#include "params/rs_render_params.h"
#include "params/rs_render_thread_params.h"
#include "pipeline/main_thread/rs_main_thread.h"
#include "pipeline/rs_context.h"
#include "pipeline/rs_root_render_node.h"
#include "pipeline/rs_surface_render_node.h"

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

class MockSurface : public Drawing::Surface {
public:
    explicit MockSurface() : Drawing::Surface() {}
    MOCK_METHOD(std::shared_ptr<Drawing::Surface>, MakeSurface, (int, int), (const));
};

class RSWindowKeyframeBufferTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    std::shared_ptr<RSRootRenderNode> renderNode_;
    std::shared_ptr<RSRootRenderNodeDrawable> rootNodeDrawable_ = nullptr;
};

void RSWindowKeyframeBufferTest::SetUpTestCase() {}
void RSWindowKeyframeBufferTest::TearDownTestCase() {}
void RSWindowKeyframeBufferTest::SetUp()
{
    renderNode_ = std::make_shared<RSRootRenderNode>(DEFAULT_ID);
    if (renderNode_ == nullptr) {
        RS_LOGE("RSWindowKeyframeBufferTest: failed to create surface node");
        return;
    }
    rootNodeDrawable_ = std::static_pointer_cast<RSRootRenderNodeDrawable>(
        DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(renderNode_));
    if (rootNodeDrawable_) {
        rootNodeDrawable_->renderParams_ = std::make_unique<RSRenderParams>(DEFAULT_ID);
    }
}
void RSWindowKeyframeBufferTest::TearDown() {}

/**
 * @tc.name: NeedDrawWindowKeyFrame
 * @tc.desc: Test NeedDrawWindowKeyFrame
 * @tc.type: FUNC
 * @tc.require: #IBPVN9
 */
HWTEST_F(RSWindowKeyframeBufferTest, NeedDrawWindowKeyFrame, TestSize.Level1)
{
    ASSERT_NE(rootNodeDrawable_, nullptr);

    RSWindowKeyframeBuffer windowKeyframeBuffer(*rootNodeDrawable_);
    auto ret = windowKeyframeBuffer.NeedDrawWindowKeyFrame(nullptr);
    EXPECT_FALSE(ret);

    auto params = std::make_unique<RSRenderParams>(DEFAULT_ID);
    ret = windowKeyframeBuffer.NeedDrawWindowKeyFrame(params);
    EXPECT_FALSE(ret);

    params->EnableWindowKeyFrame(true);
    ret = windowKeyframeBuffer.NeedDrawWindowKeyFrame(params);
    EXPECT_TRUE(ret);
}

/**
 * @tc.name: OnDraw
 * @tc.desc: Test OnDraw
 * @tc.type: FUNC
 * @tc.require: #IBPVN9
 */
HWTEST_F(RSWindowKeyframeBufferTest, OnDraw, TestSize.Level1)
{
    ASSERT_NE(rootNodeDrawable_, nullptr);
    ASSERT_NE(rootNodeDrawable_->GetRenderParams(), nullptr);

    Drawing::Canvas canvas(DEFAULT_CANVAS_SIZE, DEFAULT_CANVAS_SIZE);
    RSWindowKeyframeBuffer windowKeyframeBuffer(*rootNodeDrawable_);
    auto ret = windowKeyframeBuffer.OnDraw(canvas, *rootNodeDrawable_->GetRenderParams());
    EXPECT_FALSE(ret);

    rootNodeDrawable_->GetRenderParams()->EnableWindowKeyFrame(true);
    ret = windowKeyframeBuffer.OnDraw(canvas, *rootNodeDrawable_->GetRenderParams());
    EXPECT_FALSE(ret);
}

/**
 * @tc.name: OnDraw
 * @tc.desc: Test OnDraw GetSurface return nullptr
 * @tc.type: FUNC
 * @tc.require: #IC1LJK
 */
HWTEST_F(RSWindowKeyframeBufferTest, OnDraw_001, TestSize.Level1)
{
    ASSERT_NE(rootNodeDrawable_, nullptr);
    ASSERT_NE(rootNodeDrawable_->GetRenderParams(), nullptr);

    auto renderThreadParams = std::make_unique<RSRenderThreadParams>();
    ASSERT_NE(renderThreadParams, nullptr);
    RSUniRenderThread::Instance().Sync(std::move(renderThreadParams));

    Drawing::Canvas canvas(DEFAULT_CANVAS_SIZE, DEFAULT_CANVAS_SIZE);
    MockRSPaintFilterCanvas mockRSPaintFilterCanvas(&canvas);
    EXPECT_CALL(mockRSPaintFilterCanvas, GetSurface()).Times(AnyNumber()).WillOnce(Return(nullptr));
    RSWindowKeyframeBuffer windowKeyframeBuffer(*rootNodeDrawable_);
    auto ret = windowKeyframeBuffer.OnDraw(mockRSPaintFilterCanvas, *rootNodeDrawable_->GetRenderParams());
    EXPECT_FALSE(ret);

    RSUniRenderThread::Instance().Sync(nullptr);
}

/**
 * @tc.name: OnDraw
 * @tc.desc: Test OnDraw MakeSurface return nullptr
 * @tc.type: FUNC
 * @tc.require: #IC1LJK
 */
HWTEST_F(RSWindowKeyframeBufferTest, OnDraw_002, TestSize.Level1)
{
    ASSERT_NE(rootNodeDrawable_, nullptr);
    ASSERT_NE(rootNodeDrawable_->GetRenderParams(), nullptr);

    auto renderThreadParams = std::make_unique<RSRenderThreadParams>();
    ASSERT_NE(renderThreadParams, nullptr);
    RSUniRenderThread::Instance().Sync(std::move(renderThreadParams));

    Drawing::Canvas canvas(DEFAULT_CANVAS_SIZE, DEFAULT_CANVAS_SIZE);
    MockRSPaintFilterCanvas mockRSPaintFilterCanvas(&canvas);
    MockSurface mockSurface1;
    EXPECT_CALL(mockRSPaintFilterCanvas, GetSurface()).Times(AnyNumber()).WillOnce(Return(&mockSurface1));
    EXPECT_CALL(mockSurface1, MakeSurface(A<int>(), A<int>())).Times(AnyNumber()).WillOnce(Return(nullptr));
    RSWindowKeyframeBuffer windowKeyframeBuffer1(*rootNodeDrawable_);
    auto ret1 = windowKeyframeBuffer1.OnDraw(mockRSPaintFilterCanvas, *rootNodeDrawable_->GetRenderParams());
    EXPECT_FALSE(ret1);

    MockSurface mockSurface2;
    RSPaintFilterCanvas filterCanvas(&mockSurface2);
    EXPECT_CALL(mockSurface2, MakeSurface(A<int>(), A<int>())).Times(AnyNumber()).WillOnce(Return(nullptr));
    RSWindowKeyframeBuffer windowKeyframeBuffer2(*rootNodeDrawable_);
    auto ret2 = windowKeyframeBuffer2.OnDraw(filterCanvas, *rootNodeDrawable_->GetRenderParams());
    EXPECT_FALSE(ret2);

    RSUniRenderThread::Instance().Sync(nullptr);
}

/**
 * @tc.name: OnDraw
 * @tc.desc: Test OnDraw snapshot is nullptr and return true
 * @tc.type: FUNC
 * @tc.require: #IC1LJK
 */
HWTEST_F(RSWindowKeyframeBufferTest, OnDraw_003, TestSize.Level1)
{
    ASSERT_NE(rootNodeDrawable_, nullptr);
    ASSERT_NE(rootNodeDrawable_->GetRenderParams(), nullptr);

    auto renderThreadParams = std::make_unique<RSRenderThreadParams>();
    ASSERT_NE(renderThreadParams, nullptr);
    RSUniRenderThread::Instance().Sync(std::move(renderThreadParams));

    rootNodeDrawable_->GetRenderParams()->SetNeedSwapBuffer(true);
    rootNodeDrawable_->GetRenderParams()->SetFrameRect({0, 0, DEFAULT_CANVAS_SIZE, DEFAULT_CANVAS_SIZE});

    auto surface = Drawing::Surface::MakeRasterN32Premul(1, 1);
    ASSERT_NE(surface, nullptr);
    RSPaintFilterCanvas filterCanvas(surface.get());

    RSWindowKeyframeBuffer windowKeyframeBuffer(*rootNodeDrawable_);
    auto ret = windowKeyframeBuffer.OnDraw(filterCanvas, *rootNodeDrawable_->GetRenderParams());
    EXPECT_TRUE(ret);

    rootNodeDrawable_->GetRenderParams()->SetCacheNodeFrameRect({0, 0, 1, 1});
    ret = windowKeyframeBuffer.OnDraw(filterCanvas, *rootNodeDrawable_->GetRenderParams());
    EXPECT_TRUE(ret);

    RSUniRenderThread::Instance().Sync(nullptr);
}

/**
 * @tc.name: DrawOffscreenBuffer
 * @tc.desc: Test DrawOffscreenBuffer
 * @tc.type: FUNC
 * @tc.require: #IBPVN9
 */
HWTEST_F(RSWindowKeyframeBufferTest, DrawOffscreenBuffer, TestSize.Level1)
{
    ASSERT_NE(rootNodeDrawable_, nullptr);

    auto surface = Drawing::Surface::MakeRasterN32Premul(1, 1);
    ASSERT_NE(surface, nullptr);
    RSPaintFilterCanvas filterCanvas(surface.get());

    RSWindowKeyframeBuffer windowKeyframeBuffer(*rootNodeDrawable_);
    Drawing::Rect bounds1(1, 1, 1, 1);
    float alpha = 1.0f;
    auto ret = windowKeyframeBuffer.DrawOffscreenBuffer(filterCanvas, bounds1, alpha, false);
    EXPECT_FALSE(ret);

    Drawing::Rect bounds2(1, 1, 0, 0);
    Drawing::Bitmap bmp;
    Drawing::BitmapFormat format { Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_PREMUL };
    int32_t width = DEFAULT_CANVAS_SIZE;
    int32_t height = DEFAULT_CANVAS_SIZE;
    bmp.Build(width, height, format);
    bmp.ClearWithColor(Drawing::Color::COLOR_RED);
    windowKeyframeBuffer.preCachedOffscreenImg_ = bmp.MakeImage();
    ret = windowKeyframeBuffer.DrawOffscreenBuffer(filterCanvas, bounds2, alpha, false);
    EXPECT_FALSE(ret);

    Drawing::Rect bounds3(0, 0, DEFAULT_CANVAS_SIZE * 2, DEFAULT_CANVAS_SIZE);
    ret = windowKeyframeBuffer.DrawOffscreenBuffer(filterCanvas, bounds3, alpha, true);
    EXPECT_TRUE(ret);

    Drawing::Rect bounds4(0, 0, DEFAULT_CANVAS_SIZE, DEFAULT_CANVAS_SIZE * 2);
    ret = windowKeyframeBuffer.DrawOffscreenBuffer(filterCanvas, bounds4, alpha, true);
    EXPECT_TRUE(ret);
}

} // namespace OHOS::Rosen
