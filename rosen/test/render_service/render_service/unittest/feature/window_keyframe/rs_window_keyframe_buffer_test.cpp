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

#include "gtest/gtest.h"
#include "drawable/rs_root_render_node_drawable.h"
#include "feature/window_keyframe/rs_window_keyframe_buffer.h"
#include "params/rs_render_params.h"
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

class RSWindowKeyframeBufferTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    std::shared_ptr<RSRootRenderNode> renderNode_;
    RSRenderNodeDrawableAdapter* drawable_ = nullptr;
    RSRootRenderNodeDrawable* rootNodeDrawable_ = nullptr;
    std::shared_ptr<Drawing::Canvas> drawingCanvas_;
    std::shared_ptr<RSPaintFilterCanvas> canvas_;
};

void RSWindowKeyframeBufferTest::SetUpTestCase() {}
void RSWindowKeyframeBufferTest::TearDownTestCase() {}
void RSWindowKeyframeBufferTest::SetUp()
{
    renderNode_ = std::make_shared<RSRootRenderNode>(DEFAULT_ID);
    if (renderNode_ == nullptr) {
        RS_LOGE("RSWindowKeyframeBufferTest: failed to create surface node");
    }

    drawable_ = RSRootRenderNodeDrawable::OnGenerate(renderNode_);
    if (drawable_ != nullptr) {
        drawable_->renderParams_ = std::make_unique<RSRenderParams>(DEFAULT_ID);
        rootNodeDrawable_ = static_cast<RSRootRenderNodeDrawable*>(drawable_);
        if (drawable_->renderParams_ == nullptr) {
            RS_LOGE("RSWindowKeyframeBufferTest: failed to init render params");
        }
    }

    drawingCanvas_ = std::make_unique<Drawing::Canvas>(DEFAULT_CANVAS_SIZE, DEFAULT_CANVAS_SIZE);
    if (drawingCanvas_ != nullptr) {
        canvas_ = std::make_shared<RSPaintFilterCanvas>(drawingCanvas_.get());
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
    ASSERT_NE(canvas_, nullptr);
    ASSERT_NE(drawingCanvas_, nullptr);
    ASSERT_NE(rootNodeDrawable_->GetRenderParams(), nullptr);

    RSWindowKeyframeBuffer windowKeyframeBuffer(*rootNodeDrawable_);
    auto ret = windowKeyframeBuffer.OnDraw(*drawingCanvas_, *rootNodeDrawable_->GetRenderParams());
    EXPECT_FALSE(ret);

    rootNodeDrawable_->GetRenderParams()->EnableWindowKeyFrame(true);
    ret = windowKeyframeBuffer.OnDraw(*drawingCanvas_, *rootNodeDrawable_->GetRenderParams());
    EXPECT_FALSE(ret);
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
    ASSERT_NE(canvas_, nullptr);

    RSWindowKeyframeBuffer windowKeyframeBuffer(*rootNodeDrawable_);
    Drawing::Rect bounds(1, 1, DEFAULT_CANVAS_SIZE, DEFAULT_CANVAS_SIZE);
    float alpha = 1.0f;
    auto ret = windowKeyframeBuffer.DrawOffscreenBuffer(*canvas_, bounds, alpha, false);
    EXPECT_FALSE(ret);

    Drawing::Bitmap bmp;
    Drawing::BitmapFormat format { Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_PREMUL };
    int32_t width = DEFAULT_CANVAS_SIZE;
    int32_t height = DEFAULT_CANVAS_SIZE;
    bmp.Build(width, height, format);
    bmp.ClearWithColor(Drawing::Color::COLOR_RED);
    windowKeyframeBuffer.preCachedOffscreenImg_ = bmp.MakeImage();
    ret = windowKeyframeBuffer.DrawOffscreenBuffer(*canvas_, bounds, alpha, true);
    EXPECT_TRUE(ret);
}

} // namespace OHOS::Rosen
