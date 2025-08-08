/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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
#include "drawable/rs_screen_render_node_drawable.h"
#include "foundation/graphic/graphic_2d/rosen/test/render_service/render_service/unittest/pipeline/rs_test_util.h"
#include "pipeline/render_thread/rs_uni_render_composer_adapter.h"
#include "pipeline/main_thread/rs_uni_render_listener.h"
#include "surface_buffer_impl.h"
#include "metadata_helper.h"
#include "screen_manager/rs_screen.h"

using namespace testing;
using namespace testing::ext;

namespace {
    constexpr uint32_t DEFAULT_CANVAS_WIDTH = 800;
    constexpr uint32_t DEFAULT_CANVAS_HEIGHT = 600;
}

namespace OHOS::Rosen {
class RSUniRenderComposerAdapterTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

public:
    const uint32_t screenId_ = 10;
    const int32_t offsetX = 0; // screenOffset on x axis equals to 0
    const int32_t offsetY = 0; // screenOffset on y axis equals to 0
    const float mirrorAdaptiveCoefficient = 1.0f;

    sptr<RSScreenManager> screenManager_;
    std::unique_ptr<RSUniRenderComposerAdapter> composerAdapter_;
};

void RSUniRenderComposerAdapterTest::SetUpTestCase() {}
void RSUniRenderComposerAdapterTest::TearDownTestCase() {}
void RSUniRenderComposerAdapterTest::TearDown()
{
    screenManager_ = OHOS::Rosen::impl::RSScreenManager::GetInstance();
    OHOS::Rosen::impl::RSScreenManager& screenManager =
        static_cast<OHOS::Rosen::impl::RSScreenManager&>(*screenManager_);
    screenManager.screens_.erase(screenId_);
}
void RSUniRenderComposerAdapterTest::SetUp()
{
    screenManager_ = CreateOrGetScreenManager();
    ASSERT_NE(screenManager_, nullptr);
    uint32_t width = 2560;
    uint32_t height = 1080;
    ScreenColorGamut colorGamut = ScreenColorGamut::COLOR_GAMUT_SRGB;
    ScreenState state = ScreenState::UNKNOWN;
    ScreenRotation rotation = ScreenRotation::ROTATION_0;
    auto rsScreen = std::make_shared<impl::RSScreen>(screenId_, true, HdiOutput::CreateHdiOutput(screenId_), nullptr);
    ASSERT_NE(rsScreen, nullptr);
    screenManager_->MockHdiScreenConnected(rsScreen);
    auto info = screenManager_->QueryScreenInfo(screenId_);
    info.width = width;
    info.height = height;
    info.phyWidth = width;
    info.phyHeight = height;
    info.colorGamut = colorGamut;
    info.state = state;
    info.rotation = rotation;
    composerAdapter_ = std::make_unique<RSUniRenderComposerAdapter>();
    ASSERT_NE(composerAdapter_, nullptr);
    composerAdapter_->Init(info, offsetX, offsetY);
}

/**
 * @tc.name: SrcRectRotateTransform001
 * @tc.desc: Test RSUniRenderComposerAdapterTest.SrcRectRotateTransform
 * @tc.type: FUNC
 * @tc.require: issueI6S774
 */
HWTEST_F(RSUniRenderComposerAdapterTest, SrcRectRotateTransform001, TestSize.Level1)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    surfaceNode->GetRSSurfaceHandler()->GetConsumer()->SetTransform(GraphicTransformType::GRAPHIC_FLIP_H_ROT90);
    ASSERT_NE(surfaceNode, nullptr);
    RectI dstRect{0, 0, DEFAULT_CANVAS_WIDTH, DEFAULT_CANVAS_HEIGHT};
    surfaceNode->SetSrcRect(dstRect);
    surfaceNode->SetDstRect(dstRect);
    auto rect = composerAdapter_->SrcRectRotateTransform(*surfaceNode);
    ASSERT_NE(0, rect.width_);
    ASSERT_NE(0, rect.height_);
}

/**
 * @tc.name: SrcRectRotateTransform002
 * @tc.desc: Test RSUniRenderComposerAdapterTest.SrcRectRotateTransform
 * @tc.type: FUNC
 * @tc.require: issueI6S774
 */
HWTEST_F(RSUniRenderComposerAdapterTest, SrcRectRotateTransform002, TestSize.Level1)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    surfaceNode->GetRSSurfaceHandler()->GetConsumer()->SetTransform(GraphicTransformType::GRAPHIC_FLIP_H_ROT180);
    ASSERT_NE(surfaceNode, nullptr);
    RectI dstRect{0, 0, DEFAULT_CANVAS_WIDTH, DEFAULT_CANVAS_HEIGHT};
    surfaceNode->SetSrcRect(dstRect);
    surfaceNode->SetDstRect(dstRect);
    auto rect = composerAdapter_->SrcRectRotateTransform(*surfaceNode);
    ASSERT_NE(0, rect.width_);
    ASSERT_NE(0, rect.height_);
}

/**
 * @tc.name: SrcRectRotateTransform003
 * @tc.desc: Test RSUniRenderComposerAdapterTest.SrcRectRotateTransform
 * @tc.type: FUNC
 * @tc.require: issueI6S774
 */
HWTEST_F(RSUniRenderComposerAdapterTest, SrcRectRotateTransform003, TestSize.Level1)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    surfaceNode->GetRSSurfaceHandler()->GetConsumer()->SetTransform(GraphicTransformType::GRAPHIC_FLIP_H_ROT270);
    ASSERT_NE(surfaceNode, nullptr);
    RectI dstRect{0, 0, DEFAULT_CANVAS_WIDTH, DEFAULT_CANVAS_HEIGHT};
    surfaceNode->SetSrcRect(dstRect);
    surfaceNode->SetDstRect(dstRect);
    auto rect = composerAdapter_->SrcRectRotateTransform(*surfaceNode);
    ASSERT_NE(0, rect.width_);
    ASSERT_NE(0, rect.height_);
}

/**
 * @tc.name: SrcRectRotateTransform004
 * @tc.desc: Test RSUniRenderComposerAdapterTest.SrcRectRotateTransform
 * @tc.type: FUNC
 * @tc.require: issueI6S774
 */
HWTEST_F(RSUniRenderComposerAdapterTest, SrcRectRotateTransform004, TestSize.Level1)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    surfaceNode->GetRSSurfaceHandler()->GetConsumer()->SetTransform(GraphicTransformType::GRAPHIC_ROTATE_NONE);
    ASSERT_NE(surfaceNode, nullptr);
    RectI dstRect{0, 0, DEFAULT_CANVAS_WIDTH, DEFAULT_CANVAS_HEIGHT};
    surfaceNode->SetSrcRect(dstRect);
    surfaceNode->SetDstRect(dstRect);
    auto rect = composerAdapter_->SrcRectRotateTransform(*surfaceNode);
    ASSERT_NE(0, rect.width_);
    ASSERT_NE(0, rect.height_);
}

/**
 * @tc.name: CheckStatusBeforeCreateLayer001
 * @tc.desc: Test RSUniRenderComposerAdapterTest.CheckStatusBeforeCreateLayer
 * @tc.type: FUNC
 * @tc.require: issueI6S774
 */
HWTEST_F(RSUniRenderComposerAdapterTest, CheckStatusBeforeCreateLayer001, TestSize.Level1)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    surfaceNode->GetRSSurfaceHandler()->GetConsumer()->SetTransform(GraphicTransformType::GRAPHIC_ROTATE_NONE);
    ASSERT_NE(surfaceNode, nullptr);
    composerAdapter_->output_ = nullptr;
    ASSERT_EQ(false, composerAdapter_->CheckStatusBeforeCreateLayer(*surfaceNode));
}

/**
 * @tc.name: CheckStatusBeforeCreateLayer002
 * @tc.desc: Test RSUniRenderComposerAdapterTest.CheckStatusBeforeCreateLayer
 * @tc.type: FUNC
 * @tc.require: issueI6S774
 */
HWTEST_F(RSUniRenderComposerAdapterTest, CheckStatusBeforeCreateLayer002, TestSize.Level1)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);
    const auto& buffer = surfaceNode->GetRSSurfaceHandler()->GetBuffer();
    ASSERT_EQ(buffer, nullptr);
    ASSERT_EQ(false, composerAdapter_->CheckStatusBeforeCreateLayer(*surfaceNode));
}

/**
 * @tc.name: CheckStatusBeforeCreateLayer003
 * @tc.desc: Test RSUniRenderComposerAdapterTest.CheckStatusBeforeCreateLayer
 * @tc.type: FUNC
 * @tc.require: issueI6S774
 */
HWTEST_F(RSUniRenderComposerAdapterTest, CheckStatusBeforeCreateLayer003, TestSize.Level1)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);
    RectI dstRect{0, 0, DEFAULT_CANVAS_WIDTH, DEFAULT_CANVAS_HEIGHT};
    surfaceNode->SetSrcRect(dstRect);
    surfaceNode->SetDstRect(dstRect);
    surfaceNode->dstRect_.width_ = 0;
    ASSERT_EQ(false, composerAdapter_->CheckStatusBeforeCreateLayer(*surfaceNode));
}

/**
 * @tc.name: CheckStatusBeforeCreateLayer004
 * @tc.desc: Test RSUniRenderComposerAdapterTest.CheckStatusBeforeCreateLayer
 * @tc.type: FUNC
 * @tc.require: issueI6S774
 */
HWTEST_F(RSUniRenderComposerAdapterTest, CheckStatusBeforeCreateLayer004, TestSize.Level1)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);
    RectI dstRect{0, 0, DEFAULT_CANVAS_WIDTH, DEFAULT_CANVAS_HEIGHT};
    surfaceNode->SetSrcRect(dstRect);
    surfaceNode->SetDstRect(dstRect);
    surfaceNode->dstRect_.height_ = 0;
    ASSERT_EQ(false, composerAdapter_->CheckStatusBeforeCreateLayer(*surfaceNode));
}

/**
 * @tc.name: CheckStatusBeforeCreateLayer005
 * @tc.desc: Test RSUniRenderComposerAdapterTest.CheckStatusBeforeCreateLayer
 * @tc.type: FUNC
 * @tc.require: issueI6S774
 */
HWTEST_F(RSUniRenderComposerAdapterTest, CheckStatusBeforeCreateLayer005, TestSize.Level1)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);
    RectI dstRect{0, 0, DEFAULT_CANVAS_WIDTH, DEFAULT_CANVAS_HEIGHT};
    surfaceNode->SetSrcRect(dstRect);
    surfaceNode->SetDstRect(dstRect);
    surfaceNode->srcRect_.width_ = 0;
    ASSERT_EQ(false, composerAdapter_->CheckStatusBeforeCreateLayer(*surfaceNode));
}

/**
 * @tc.name: CheckStatusBeforeCreateLayer006
 * @tc.desc: Test RSUniRenderComposerAdapterTest.CheckStatusBeforeCreateLayer
 * @tc.type: FUNC
 * @tc.require: issueI6S774
 */
HWTEST_F(RSUniRenderComposerAdapterTest, CheckStatusBeforeCreateLayer006, TestSize.Level1)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);
    RectI dstRect{0, 0, DEFAULT_CANVAS_WIDTH, DEFAULT_CANVAS_HEIGHT};
    surfaceNode->SetSrcRect(dstRect);
    surfaceNode->SetDstRect(dstRect);
    surfaceNode->srcRect_.height_ = 0;
    ASSERT_EQ(false, composerAdapter_->CheckStatusBeforeCreateLayer(*surfaceNode));
}

/**
 * @tc.name: CheckStatusBeforeCreateLayer007
 * @tc.desc: Test RSUniRenderComposerAdapterTest.CheckStatusBeforeCreateLayer
 * @tc.type: FUNC
 * @tc.require: issueI6S774
 */
HWTEST_F(RSUniRenderComposerAdapterTest, CheckStatusBeforeCreateLayer007, TestSize.Level1)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);
    RectI dstRect{0, 0, DEFAULT_CANVAS_WIDTH, DEFAULT_CANVAS_HEIGHT};
    surfaceNode->SetSrcRect(dstRect);
    surfaceNode->SetDstRect(dstRect);
    ASSERT_EQ(true, composerAdapter_->CheckStatusBeforeCreateLayer(*surfaceNode));
}

/**
 * @tc.name: SrcRectRotateTransform005
 * @tc.desc: Test RSUniRenderComposerAdapterTest.SrcRectRotateTransform for GRAPHIC_ROTATE_90
 * @tc.type: FUNC
 * @tc.require: issueI7O6WO
 */
HWTEST_F(RSUniRenderComposerAdapterTest, SrcRectRotateTransform005, TestSize.Level2)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);
    //let the left value of node's rect not equal to zero and the result of the founction isn't empty
    RectI rect{DEFAULT_CANVAS_WIDTH * 0.5, DEFAULT_CANVAS_HEIGHT * 0.5, DEFAULT_CANVAS_WIDTH, DEFAULT_CANVAS_HEIGHT};
    surfaceNode->SetSrcRect(rect);
    surfaceNode->SetDstRect(rect);
    surfaceNode->GetMutableRenderProperties().SetBoundsWidth(DEFAULT_CANVAS_WIDTH * 1.5);
    surfaceNode->GetMutableRenderProperties().SetBoundsHeight(DEFAULT_CANVAS_HEIGHT * 1.5);
    surfaceNode->GetRSSurfaceHandler()->GetConsumer()->SetTransform(GraphicTransformType::GRAPHIC_FLIP_H_ROT90);
    surfaceNode->GetRSSurfaceHandler()->GetBuffer()->SetSurfaceBufferTransform(
        GraphicTransformType::GRAPHIC_FLIP_H_ROT90);
    auto srcRect = composerAdapter_->SrcRectRotateTransform(*surfaceNode);
    ASSERT_EQ(srcRect.top_, DEFAULT_CANVAS_WIDTH * 0.5);
}

/**
 * @tc.name: CreateLayer002
 * @tc.desc: Test RSUniRenderComposerAdapterTest.CreateLayer002
 * @tc.type: FUNC
 */
HWTEST_F(RSUniRenderComposerAdapterTest, CreateLayer002, TestSize.Level2)
{
    NodeId id = 1;
    ScreenId screenId = 0;
    std::shared_ptr<RSContext> context = std::make_shared<RSContext>();
    auto rsScreenNode = std::make_shared<RSScreenRenderNode>(id, screenId, context->weak_from_this());
    LayerInfoPtr layer1 = composerAdapter_->CreateLayer(*rsScreenNode);
    ASSERT_EQ(layer1, nullptr);
    
    auto drawable = DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(rsScreenNode);
    ASSERT_NE(drawable, nullptr);
    auto screenDrawable = std::static_pointer_cast<DrawableV2::RSScreenRenderNodeDrawable>(drawable);
    rsScreenNode->renderDrawable_ = screenDrawable;
    screenDrawable->surfaceCreated_ = true;
    OHOS::sptr<SurfaceBuffer> cbuffer = new SurfaceBufferImpl();
    sptr<SyncFence> acquireFence = SyncFence::INVALID_FENCE;
    auto surfaceHandler = screenDrawable->GetMutableRSSurfaceHandlerOnDraw();
    ASSERT_NE(surfaceHandler, nullptr);
    surfaceHandler->SetBuffer(cbuffer, acquireFence, {}, 0);
    surfaceHandler->SetAvailableBufferCount(0);
    screenDrawable->surfaceCreated_ = true;
    LayerInfoPtr layer2 = composerAdapter_->CreateLayer(*rsScreenNode);
    ASSERT_EQ(layer2->GetNodeId(), id);
}

/**
 * @tc.name: SrcRectRotateTransform006
 * @tc.desc: Test RSUniRenderComposerAdapterTest.SrcRectRotateTransform while
 *           surface's consumer is nullptr
 * @tc.type: FUNC
 * @tc.require: issuesI7T9RE
 */
HWTEST_F(RSUniRenderComposerAdapterTest, SrcRectRotateTransform006, TestSize.Level2)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);
    RectI rect{0, 0, DEFAULT_CANVAS_WIDTH, DEFAULT_CANVAS_HEIGHT};
    surfaceNode->SetSrcRect(rect);
    surfaceNode->GetRSSurfaceHandler()->consumer_ = nullptr;
    ASSERT_EQ(composerAdapter_->SrcRectRotateTransform(*surfaceNode), rect);
}

/**
 * @tc.name: SetBufferColorSpace001
 * @tc.desc: Test RSUniRenderComposerAdapterTest.SetBufferColorSpace
 * @tc.type: FUNC
 * @tc.require: issuesI8C4I9
*/
HWTEST_F(RSUniRenderComposerAdapterTest, SetBufferColorSpace001, TestSize.Level2)
{
    SetUp();

    using namespace HDI::Display::Graphic::Common::V1_0;
    auto rsContext = std::make_shared<RSContext>();
    RSScreenRenderNode::SharedPtr nodePtr = std::make_shared<RSScreenRenderNode>(1, 0, rsContext->weak_from_this());
    auto screenDrawable =std::static_pointer_cast<DrawableV2::RSScreenRenderNodeDrawable>(
        DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(nodePtr));
    auto surfaceHandler = screenDrawable->GetRSSurfaceHandlerOnDraw();
    sptr<IBufferConsumerListener> listener = new RSUniRenderListener(surfaceHandler);
    screenDrawable->CreateSurface(listener);

    auto rsSurface = screenDrawable->GetRSSurface();
    rsSurface->SetColorSpace(GRAPHIC_COLOR_GAMUT_DISPLAY_P3);

    sptr<SurfaceBuffer> buffer = new SurfaceBufferImpl();
    BufferRequestConfig requestConfig = {
        .width = 0x100,
        .height = 0x100,
        .strideAlignment = 0x8,
        .format = GRAPHIC_PIXEL_FMT_RGBA_8888,
        .usage = BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA,
        .timeout = 0,
        .colorGamut = GraphicColorGamut::GRAPHIC_COLOR_GAMUT_DISPLAY_P3,
    };
    GSError ret = buffer->Alloc(requestConfig);
    ASSERT_EQ(ret, GSERROR_OK);

    surfaceHandler->SetBuffer(buffer, SyncFence::INVALID_FENCE, Rect(), 0);

    RSUniRenderComposerAdapter::SetBufferColorSpace(*screenDrawable);

    CM_ColorSpaceType colorSpaceType;
    ret = MetadataHelper::GetColorSpaceType(buffer, colorSpaceType);
    ASSERT_TRUE(ret == GSERROR_OK || GSErrorStr(ret) == "<500 api call failed>with low error <Not supported>");
    if (ret == GSERROR_OK) {
        ASSERT_EQ(colorSpaceType, CM_P3_FULL);
    }

    CM_ColorSpaceInfo colorSpaceInfo;
    ret = MetadataHelper::GetColorSpaceInfo(buffer, colorSpaceInfo);
    ASSERT_TRUE(ret == GSERROR_OK || GSErrorStr(ret) == "<500 api call failed>with low error <Not supported>");
    if (ret == GSERROR_OK) {
        ASSERT_EQ(colorSpaceInfo.primaries, COLORPRIMARIES_P3_D65);
        ASSERT_EQ(colorSpaceInfo.transfunc, TRANSFUNC_SRGB);
        ASSERT_EQ(colorSpaceInfo.matrix, MATRIX_P3);
        ASSERT_EQ(colorSpaceInfo.range, RANGE_FULL);
    }
}

/**
 * @tc.name: SetBufferColorSpace002
 * @tc.desc: Test RSUniRenderComposerAdapterTest.SetBufferColorSpace
 * @tc.type: FUNC
 * @tc.require: issuesI8C4I9
*/
HWTEST_F(RSUniRenderComposerAdapterTest, SetBufferColorSpace002, TestSize.Level2)
{
    SetUp();

    NodeId id = 0;
    auto rsContext = std::make_shared<RSContext>();
    auto node = std::make_shared<RSScreenRenderNode>(id, 0, rsContext->weak_from_this());
    auto drawable = DrawableV2::RSScreenRenderNodeDrawable::OnGenerate(node);
    ASSERT_NE(drawable, nullptr);
    auto screenDrawable = static_cast<DrawableV2::RSScreenRenderNodeDrawable*>(drawable);
    composerAdapter_->SetBufferColorSpace(*screenDrawable);
}

/**
 * @tc.name: SetBufferColorSpace003
 * @tc.desc: Test RSUniRenderComposerAdapterTest.SetBufferColorSpace
 * @tc.type: FUNC
 * @tc.require: issuesI8C4I9
*/
HWTEST_F(RSUniRenderComposerAdapterTest, SetBufferColorSpace003, TestSize.Level2)
{
    SetUp();

    NodeId id = 1;
    auto rsContext = std::make_shared<RSContext>();
    auto node = std::make_shared<RSScreenRenderNode>(id, 0, rsContext->weak_from_this());
    auto screenDrawable = std::static_pointer_cast<DrawableV2::RSScreenRenderNodeDrawable>(
        DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(node));
    ASSERT_NE(screenDrawable, nullptr);
    composerAdapter_->SetBufferColorSpace(*screenDrawable);
    sptr<SyncFence> acquireFence = SyncFence::INVALID_FENCE;
    int64_t timestamp = 0;
    Rect damage;
    sptr<OHOS::SurfaceBuffer> buffer = new SurfaceBufferImpl(0);
    auto surfaceHandler = screenDrawable->GetRSSurfaceHandlerOnDraw();
    surfaceHandler->SetBuffer(buffer, acquireFence, damage, timestamp);
    composerAdapter_->SetBufferColorSpace(*screenDrawable);
}
} // namespace