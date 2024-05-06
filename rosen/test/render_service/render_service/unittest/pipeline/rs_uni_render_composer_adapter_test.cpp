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
#include "pipeline/rs_uni_render_composer_adapter.h"
#include "pipeline/rs_uni_render_listener.h"
#include "surface_buffer_impl.h"
#include "rs_test_util.h"
#include "metadata_helper.h"

using namespace testing;
using namespace testing::ext;

namespace {
    constexpr uint32_t DEFAULT_CANVAS_WIDTH = 800;
    constexpr uint32_t DEFAULT_CANVAS_HEIGHT = 600;
    constexpr uint32_t DEFAULT_CANVAS_WIDTH_1K = 1920;
    constexpr uint32_t DEFAULT_CANVAS_HEIGHT_1K = 1080;
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
    std::unique_ptr<impl::RSScreen> rsScreen =
        std::make_unique<impl::RSScreen>(screenId_, true, HdiOutput::CreateHdiOutput(screenId_), nullptr);
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
    composerAdapter_->Init(info, offsetX, offsetY, mirrorAdaptiveCoefficient);
}

/**
 * @tc.name: Start001
 * @tc.desc: Test RSUniRenderComposerAdapterTest.CommitLayers
 * @tc.type: FUNC
 * @tc.require: issueI6S774
 */
HWTEST_F(RSUniRenderComposerAdapterTest, Start001, TestSize.Level1)
{
    SetUp();
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);
    RectI dstRect{0, 0, 400, 600};
    surfaceNode->SetSrcRect(dstRect);
    surfaceNode->SetDstRect(dstRect);
    auto layer1 = composerAdapter_->CreateLayer(*surfaceNode);
    ASSERT_NE(layer1, nullptr);
    std::vector<LayerInfoPtr> layers;
    layers.emplace_back(layer1);
    composerAdapter_->CommitLayers(layers);
    auto hdiBackend = composerAdapter_->hdiBackend_;
    composerAdapter_->hdiBackend_ = nullptr;
    composerAdapter_->CommitLayers(layers);
    composerAdapter_->hdiBackend_ = hdiBackend;
    composerAdapter_->output_ = nullptr;
    composerAdapter_->CommitLayers(layers);
}

/**
 * @tc.name: BuildComposeInfo001
 * @tc.desc: Test RSUniRenderComposerAdapterTest.BuildComposeInfo
 * @tc.type: FUNC
 * @tc.require: issueI6S774
 */
HWTEST_F(RSUniRenderComposerAdapterTest, BuildComposeInfo001, TestSize.Level1)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);
    SetUp();
    composerAdapter_->BuildComposeInfo(*surfaceNode);
}

/**
 * @tc.name: BuildComposeInfo002
 * @tc.desc: Test RSUniRenderComposerAdapterTest.BuildComposeInfo
 * @tc.type: FUNC
 * @tc.require: issueI7FUVJ
 */
HWTEST_F(RSUniRenderComposerAdapterTest, BuildComposeInfo002, TestSize.Level1)
{
    SetUp();
    RSSurfaceRenderNodeConfig config;
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);
    sptr<IConsumerSurface> csurf = IConsumerSurface::Create(config.name);
    ASSERT_NE(csurf, nullptr);

    csurf->SetTransform(GRAPHIC_ROTATE_270);
    auto srcRect = composerAdapter_->SrcRectRotateTransform(*surfaceNode);
    ASSERT_EQ(srcRect.IsEmpty(), true);
    surfaceNode->SetConsumer(csurf);
    srcRect = composerAdapter_->SrcRectRotateTransform(*surfaceNode);
    ASSERT_EQ(srcRect.IsEmpty(), true);
    RectI dstRect{0, 0, 400, 600};
    surfaceNode->SetSrcRect(dstRect);
    srcRect = composerAdapter_->SrcRectRotateTransform(*surfaceNode);
    ASSERT_EQ(srcRect.IsEmpty(), false);
    csurf->SetTransform(GRAPHIC_ROTATE_180);
    srcRect = composerAdapter_->SrcRectRotateTransform(*surfaceNode);
    ASSERT_EQ(srcRect.IsEmpty(), false);
    csurf->SetTransform(GRAPHIC_ROTATE_90);
    srcRect = composerAdapter_->SrcRectRotateTransform(*surfaceNode);
    ASSERT_EQ(srcRect.IsEmpty(), false);

    surfaceNode->SetDstRect(dstRect);
    auto layer = composerAdapter_->CreateLayer(*surfaceNode);
    ASSERT_NE(layer, nullptr);
}

/**
 * @tc.name: GetComposerInfoSrcRect001
 * @tc.desc: Test RSUniRenderComposerAdapterTest.GetComposerInfoSrcRect with GRAPHIC_FLIP_V_ROT90
 * @tc.type: FUNC
 * @tc.require: issueI6S774
 */
HWTEST_F(RSUniRenderComposerAdapterTest, GetComposerInfoSrcRect001, TestSize.Level1)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    surfaceNode->GetConsumer()->SetTransform(GraphicTransformType::GRAPHIC_FLIP_V_ROT90);
    ASSERT_NE(surfaceNode, nullptr);
    RectI dstRect{0, 0, DEFAULT_CANVAS_WIDTH, DEFAULT_CANVAS_HEIGHT};
    surfaceNode->SetSrcRect(dstRect);
    surfaceNode->SetDstRect(dstRect);
    ComposeInfo info = composerAdapter_->BuildComposeInfo(*surfaceNode);
    info.srcRect.x = DEFAULT_CANVAS_WIDTH;
    info.srcRect.y = DEFAULT_CANVAS_HEIGHT;
    surfaceNode->GetMutableRenderProperties().SetBoundsWidth(DEFAULT_CANVAS_WIDTH);
    surfaceNode->GetMutableRenderProperties().SetBoundsHeight(DEFAULT_CANVAS_HEIGHT);
    composerAdapter_->GetComposerInfoSrcRect(info, *surfaceNode);
    ASSERT_NE(0, info.srcRect.x);
    ASSERT_NE(0, info.srcRect.y);
}

/**
 * @tc.name: GetComposerInfoSrcRect002
 * @tc.desc: Test RSUniRenderComposerAdapterTest.GetComposerInfoSrcRect with GRAPHIC_FLIP_H_ROT180
 * @tc.type: FUNC
 * @tc.require: issueI6S774
 */
HWTEST_F(RSUniRenderComposerAdapterTest, GetComposerInfoSrcRect002, TestSize.Level1)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    surfaceNode->GetConsumer()->SetTransform(GraphicTransformType::GRAPHIC_FLIP_H_ROT180);
    ASSERT_NE(surfaceNode, nullptr);
    RectI dstRect{0, 0, DEFAULT_CANVAS_WIDTH, DEFAULT_CANVAS_HEIGHT};
    surfaceNode->SetSrcRect(dstRect);
    surfaceNode->SetDstRect(dstRect);
    ComposeInfo info = composerAdapter_->BuildComposeInfo(*surfaceNode);
    composerAdapter_->GetComposerInfoSrcRect(info, *surfaceNode);
}

/**
 * @tc.name: GetComposerInfoSrcRect003
 * @tc.desc: Test RSUniRenderComposerAdapterTest.GetComposerInfoSrcRect with GRAPHIC_FLIP_H_ROT270
 * @tc.type: FUNC
 * @tc.require: issueI6S774
 */
HWTEST_F(RSUniRenderComposerAdapterTest, GetComposerInfoSrcRect003, TestSize.Level1)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    surfaceNode->GetConsumer()->SetTransform(GraphicTransformType::GRAPHIC_FLIP_H_ROT270);
    ASSERT_NE(surfaceNode, nullptr);
    RectI dstRect{0, 0, DEFAULT_CANVAS_WIDTH, DEFAULT_CANVAS_HEIGHT};
    surfaceNode->SetSrcRect(dstRect);
    surfaceNode->SetDstRect(dstRect);
    ComposeInfo info = composerAdapter_->BuildComposeInfo(*surfaceNode);
    composerAdapter_->GetComposerInfoSrcRect(info, *surfaceNode);
}

/**
 * @tc.name: GetComposerInfoSrcRect004
 * @tc.desc: Test RSUniRenderComposerAdapterTest.GetComposerInfoSrcRect
 * @tc.type: FUNC
 * @tc.require: issueI6S774
 */
HWTEST_F(RSUniRenderComposerAdapterTest, GetComposerInfoSrcRect004, TestSize.Level1)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);
    surfaceNode->GetMutableRenderProperties().SetBoundsWidth(DEFAULT_CANVAS_WIDTH);
    surfaceNode->GetMutableRenderProperties().SetBoundsHeight(DEFAULT_CANVAS_HEIGHT);
    RectI dstRect{0, 0, DEFAULT_CANVAS_WIDTH, DEFAULT_CANVAS_HEIGHT};
    surfaceNode->SetSrcRect(dstRect);
    surfaceNode->SetDstRect(dstRect);
    ComposeInfo info = composerAdapter_->BuildComposeInfo(*surfaceNode);
    ASSERT_NE(info.buffer, nullptr);
    info.buffer->SetSurfaceBufferWidth(DEFAULT_CANVAS_WIDTH);
    info.buffer->SetSurfaceBufferHeight(DEFAULT_CANVAS_HEIGHT);
    composerAdapter_->GetComposerInfoSrcRect(info, *surfaceNode);

    surfaceNode->GetMutableRenderProperties().SetBoundsWidth(DEFAULT_CANVAS_WIDTH_1K);
    surfaceNode->GetMutableRenderProperties().SetBoundsHeight(DEFAULT_CANVAS_HEIGHT_1K);
    ComposeInfo info2 = composerAdapter_->BuildComposeInfo(*surfaceNode);
    composerAdapter_->GetComposerInfoSrcRect(info2, *surfaceNode);
}

/**
 * @tc.name: GetComposerInfoSrcRect005
 * @tc.desc: GetComposerInfoSrcRect, Bounds size != Buffer size
 * @tc.type: FUNC
 * @tc.require: issueI6S774
 */
HWTEST_F(RSUniRenderComposerAdapterTest, GetComposerInfoSrcRect005, TestSize.Level1)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);
    surfaceNode->GetMutableRenderProperties().SetFrameGravity(Gravity::TOP_RIGHT);
    surfaceNode->GetMutableRenderProperties().SetBoundsWidth(DEFAULT_CANVAS_WIDTH_1K);
    surfaceNode->GetMutableRenderProperties().SetBoundsHeight(DEFAULT_CANVAS_HEIGHT_1K);
    RectI dstRect{0, 0, DEFAULT_CANVAS_WIDTH, DEFAULT_CANVAS_HEIGHT};
    surfaceNode->SetSrcRect(dstRect);
    surfaceNode->SetDstRect(dstRect);
    ComposeInfo info = composerAdapter_->BuildComposeInfo(*surfaceNode);
    ASSERT_NE(info.buffer, nullptr);
    info.buffer->SetSurfaceBufferWidth(DEFAULT_CANVAS_WIDTH);
    info.buffer->SetSurfaceBufferHeight(DEFAULT_CANVAS_HEIGHT);
    composerAdapter_->GetComposerInfoSrcRect(info, *surfaceNode);
}

/**
 * @tc.name: GetComposerInfoSrcRect006
 * @tc.desc: GetComposerInfoSrcRect, Bounds size != Buffer size and FrameGravity != top left
 * @tc.type: FUNC
 * @tc.require: issueI6S774
 */
HWTEST_F(RSUniRenderComposerAdapterTest, GetComposerInfoSrcRect006, TestSize.Level1)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);
    surfaceNode->GetMutableRenderProperties().SetFrameGravity(Gravity::TOP_RIGHT);
    surfaceNode->GetMutableRenderProperties().SetBoundsWidth(DEFAULT_CANVAS_WIDTH_1K);
    surfaceNode->GetMutableRenderProperties().SetBoundsHeight(DEFAULT_CANVAS_HEIGHT_1K);
    RectI dstRect{0, 0, DEFAULT_CANVAS_WIDTH, DEFAULT_CANVAS_HEIGHT};
    surfaceNode->SetSrcRect(dstRect);
    surfaceNode->SetDstRect(dstRect);
    ComposeInfo info = composerAdapter_->BuildComposeInfo(*surfaceNode);
    ASSERT_NE(info.buffer, nullptr);
    info.buffer->SetSurfaceBufferWidth(DEFAULT_CANVAS_WIDTH);
    info.buffer->SetSurfaceBufferHeight(DEFAULT_CANVAS_HEIGHT);
    ScalingMode scalingMode = ScalingMode::SCALING_MODE_SCALE_CROP;
    surfaceNode->GetConsumer()->SetScalingMode(info.buffer->GetSeqNum(), scalingMode);
    composerAdapter_->GetComposerInfoSrcRect(info, *surfaceNode);
}

/**
 * @tc.name: DealWithNodeGravity001
 * @tc.desc: Test RSUniRenderComposerAdapterTest.DealWithNodeGravity when frameGravity is RESIZE
 * @tc.type: FUNC
 * @tc.require: issueI6S774
 */
HWTEST_F(RSUniRenderComposerAdapterTest, DealWithNodeGravity001, TestSize.Level1)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    surfaceNode->GetConsumer()->SetTransform(GraphicTransformType::GRAPHIC_FLIP_H_ROT180);
    surfaceNode->GetMutableRenderProperties().SetBoundsWidth(DEFAULT_CANVAS_WIDTH);
    surfaceNode->GetMutableRenderProperties().SetBoundsHeight(DEFAULT_CANVAS_HEIGHT);
    ASSERT_NE(surfaceNode, nullptr);
    RectI dstRect{0, 0, DEFAULT_CANVAS_WIDTH, DEFAULT_CANVAS_HEIGHT};
    surfaceNode->SetSrcRect(dstRect);
    surfaceNode->SetDstRect(dstRect);
    ComposeInfo info = composerAdapter_->BuildComposeInfo(*surfaceNode);
    ASSERT_NE(info.buffer, nullptr);
    info.buffer->SetSurfaceBufferWidth(DEFAULT_CANVAS_WIDTH);
    info.buffer->SetSurfaceBufferHeight(DEFAULT_CANVAS_HEIGHT);

    surfaceNode->GetMutableRenderProperties().frameGravity_ = Gravity::RESIZE;
    composerAdapter_->DealWithNodeGravity(*surfaceNode, info);

    surfaceNode->GetMutableRenderProperties().frameGravity_ = Gravity::TOP;
    surfaceNode->GetMutableRenderProperties().SetBoundsWidth(DEFAULT_CANVAS_WIDTH_1K);
    surfaceNode->GetMutableRenderProperties().SetBoundsHeight(DEFAULT_CANVAS_HEIGHT_1K);
    composerAdapter_->DealWithNodeGravity(*surfaceNode, info);
}

/**
 * @tc.name: DealWithNodeGravity002
 * @tc.desc: Test RSUniRenderComposerAdapterTest.DealWithNodeGravity when frameGravity is TOP
 * @tc.type: FUNC
 * @tc.require: issueI6S774
 */
HWTEST_F(RSUniRenderComposerAdapterTest, DealWithNodeGravity002, TestSize.Level1)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    surfaceNode->GetConsumer()->SetTransform(GraphicTransformType::GRAPHIC_FLIP_H_ROT180);
    surfaceNode->GetMutableRenderProperties().SetBoundsWidth(DEFAULT_CANVAS_WIDTH);
    surfaceNode->GetMutableRenderProperties().SetBoundsHeight(DEFAULT_CANVAS_HEIGHT);
    ASSERT_NE(surfaceNode, nullptr);
    RectI dstRect{0, 0, DEFAULT_CANVAS_WIDTH, DEFAULT_CANVAS_HEIGHT};
    surfaceNode->SetSrcRect(dstRect);
    surfaceNode->SetDstRect(dstRect);
    ComposeInfo info = composerAdapter_->BuildComposeInfo(*surfaceNode);
    ASSERT_NE(info.buffer, nullptr);
    info.buffer->SetSurfaceBufferWidth(DEFAULT_CANVAS_WIDTH);
    info.buffer->SetSurfaceBufferHeight(DEFAULT_CANVAS_HEIGHT);

    surfaceNode->GetMutableRenderProperties().frameGravity_ = Gravity::TOP;
    composerAdapter_->DealWithNodeGravity(*surfaceNode, info);

    surfaceNode->GetMutableRenderProperties().SetBoundsWidth(DEFAULT_CANVAS_WIDTH);
    surfaceNode->GetMutableRenderProperties().SetBoundsHeight(DEFAULT_CANVAS_HEIGHT_1K);
    composerAdapter_->DealWithNodeGravity(*surfaceNode, info);
}

/**
 * @tc.name: DealWithNodeGravity003
 * @tc.desc: Test RSUniRenderComposerAdapterTest.DealWithNodeGravity when rotation is ROTATION_90
 * @tc.type: FUNC
 * @tc.require: issueI6S774
 */
HWTEST_F(RSUniRenderComposerAdapterTest, DealWithNodeGravity003, TestSize.Level1)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    surfaceNode->GetConsumer()->SetTransform(GraphicTransformType::GRAPHIC_FLIP_H_ROT180);
    surfaceNode->GetMutableRenderProperties().SetBoundsWidth(DEFAULT_CANVAS_WIDTH);
    surfaceNode->GetMutableRenderProperties().SetBoundsHeight(DEFAULT_CANVAS_HEIGHT);
    ASSERT_NE(surfaceNode, nullptr);
    RectI dstRect{0, 0, DEFAULT_CANVAS_WIDTH, DEFAULT_CANVAS_HEIGHT};
    surfaceNode->SetSrcRect(dstRect);
    surfaceNode->SetDstRect(dstRect);
    ComposeInfo info = composerAdapter_->BuildComposeInfo(*surfaceNode);
    ASSERT_NE(info.buffer, nullptr);
    info.buffer->SetSurfaceBufferWidth(DEFAULT_CANVAS_WIDTH);
    info.buffer->SetSurfaceBufferHeight(DEFAULT_CANVAS_HEIGHT);

    surfaceNode->GetMutableRenderProperties().frameGravity_ = Gravity::TOP;
    surfaceNode->GetMutableRenderProperties().SetBoundsHeight(DEFAULT_CANVAS_HEIGHT_1K);
    composerAdapter_->screenInfo_.rotation = ScreenRotation::ROTATION_90;
    composerAdapter_->DealWithNodeGravity(*surfaceNode, info);
}

/**
 * @tc.name: DealWithNodeGravity004
 * @tc.desc: Test RSUniRenderComposerAdapterTest.DealWithNodeGravity when rotation is ROTATION_270
 * @tc.type: FUNC
 * @tc.require: issueI6S774
 */
HWTEST_F(RSUniRenderComposerAdapterTest, DealWithNodeGravity004, TestSize.Level1)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    surfaceNode->GetConsumer()->SetTransform(GraphicTransformType::GRAPHIC_FLIP_H_ROT180);
    surfaceNode->GetMutableRenderProperties().SetBoundsWidth(DEFAULT_CANVAS_WIDTH);
    surfaceNode->GetMutableRenderProperties().SetBoundsHeight(DEFAULT_CANVAS_HEIGHT);
    ASSERT_NE(surfaceNode, nullptr);
    RectI dstRect{0, 0, DEFAULT_CANVAS_WIDTH, DEFAULT_CANVAS_HEIGHT};
    surfaceNode->SetSrcRect(dstRect);
    surfaceNode->SetDstRect(dstRect);
    ComposeInfo info = composerAdapter_->BuildComposeInfo(*surfaceNode);
    ASSERT_NE(info.buffer, nullptr);
    info.buffer->SetSurfaceBufferWidth(DEFAULT_CANVAS_WIDTH);
    info.buffer->SetSurfaceBufferHeight(DEFAULT_CANVAS_HEIGHT);

    surfaceNode->GetMutableRenderProperties().frameGravity_ = Gravity::TOP;
    surfaceNode->GetMutableRenderProperties().SetBoundsHeight(DEFAULT_CANVAS_HEIGHT_1K);
    composerAdapter_->screenInfo_.rotation = ScreenRotation::ROTATION_270;
    composerAdapter_->DealWithNodeGravity(*surfaceNode, info);
}

/**
 * @tc.name: DealWithNodeGravity005
 * @tc.desc: Test RSUniRenderComposerAdapterTest.DealWithNodeGravity when rotation is ROTATION_180
 * @tc.type: FUNC
 * @tc.require: issueI6S774
 */
HWTEST_F(RSUniRenderComposerAdapterTest, DealWithNodeGravity005, TestSize.Level1)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    surfaceNode->GetConsumer()->SetTransform(GraphicTransformType::GRAPHIC_FLIP_H_ROT180);
    surfaceNode->GetMutableRenderProperties().SetBoundsWidth(DEFAULT_CANVAS_WIDTH);
    surfaceNode->GetMutableRenderProperties().SetBoundsHeight(DEFAULT_CANVAS_HEIGHT);
    ASSERT_NE(surfaceNode, nullptr);
    RectI dstRect{0, 0, DEFAULT_CANVAS_WIDTH, DEFAULT_CANVAS_HEIGHT};
    surfaceNode->SetSrcRect(dstRect);
    surfaceNode->SetDstRect(dstRect);
    ComposeInfo info = composerAdapter_->BuildComposeInfo(*surfaceNode);
    ASSERT_NE(info.buffer, nullptr);
    info.buffer->SetSurfaceBufferWidth(DEFAULT_CANVAS_WIDTH);
    info.buffer->SetSurfaceBufferHeight(DEFAULT_CANVAS_HEIGHT);

    surfaceNode->GetMutableRenderProperties().frameGravity_ = Gravity::TOP;
    surfaceNode->GetMutableRenderProperties().SetBoundsHeight(DEFAULT_CANVAS_HEIGHT_1K);
    composerAdapter_->screenInfo_.rotation = ScreenRotation::ROTATION_180;
    composerAdapter_->DealWithNodeGravity(*surfaceNode, info);
}

/**
 * @tc.name: DealWithNodeGravity006
 * @tc.desc: Test RSUniRenderComposerAdapterTest.DealWithNodeGravity
 * @tc.type: FUNC
 * @tc.require: issueI7FUVJ
 */
HWTEST_F(RSUniRenderComposerAdapterTest, DealWithNodeGravity006, TestSize.Level1)
{
    SetUp();
    RSSurfaceRenderNodeConfig config;
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);

    RectI dstRect{0, 0, 400, 600};
    surfaceNode->SetSrcRect(dstRect);
    surfaceNode->SetDstRect(dstRect);

    sptr<IConsumerSurface> csurf = IConsumerSurface::Create(config.name);
    ASSERT_NE(csurf, nullptr);
    surfaceNode->SetConsumer(csurf);

    composerAdapter_->CheckStatusBeforeCreateLayer(*surfaceNode);
    ComposeInfo info = composerAdapter_->BuildComposeInfo(*surfaceNode);
    composerAdapter_->DealWithNodeGravity(*surfaceNode, info);
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
    surfaceNode->GetConsumer()->SetTransform(GraphicTransformType::GRAPHIC_FLIP_H_ROT90);
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
    surfaceNode->GetConsumer()->SetTransform(GraphicTransformType::GRAPHIC_FLIP_H_ROT180);
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
    surfaceNode->GetConsumer()->SetTransform(GraphicTransformType::GRAPHIC_FLIP_H_ROT270);
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
    surfaceNode->GetConsumer()->SetTransform(GraphicTransformType::GRAPHIC_ROTATE_NONE);
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
    surfaceNode->GetConsumer()->SetTransform(GraphicTransformType::GRAPHIC_ROTATE_NONE);
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
    const auto& buffer = surfaceNode->GetBuffer();
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
 * @tc.name: LayerCrop001
 * @tc.desc: Test RSUniRenderComposerAdapterTest.LayerCrop
 * @tc.type: FUNC
 * @tc.require: issueI6S774
 */
HWTEST_F(RSUniRenderComposerAdapterTest, LayerCrop001, TestSize.Level1)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);
    RectI dstRect{0, 0, DEFAULT_CANVAS_WIDTH, DEFAULT_CANVAS_HEIGHT};
    surfaceNode->SetSrcRect(dstRect);
    surfaceNode->SetDstRect(dstRect);
    LayerInfoPtr layer = composerAdapter_->CreateLayer(*surfaceNode);
    ASSERT_NE(layer, nullptr);
    composerAdapter_->screenInfo_.phyWidth = DEFAULT_CANVAS_WIDTH;
    composerAdapter_->screenInfo_.phyHeight = DEFAULT_CANVAS_HEIGHT;
    composerAdapter_->LayerCrop(layer);
}

/**
 * @tc.name: LayerCrop002
 * @tc.desc: Test RSUniRenderComposerAdapterTest.LayerCrop
 * @tc.type: FUNC
 * @tc.require: issueI6S774
 */
HWTEST_F(RSUniRenderComposerAdapterTest, LayerCrop002, TestSize.Level1)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);
    RectI dstRect{0, 0, DEFAULT_CANVAS_WIDTH, DEFAULT_CANVAS_HEIGHT};
    surfaceNode->SetSrcRect(dstRect);
    surfaceNode->SetDstRect(dstRect);
    auto layer = composerAdapter_->CreateLayer(*surfaceNode);
    ASSERT_NE(layer, nullptr);
    composerAdapter_->screenInfo_.phyWidth = DEFAULT_CANVAS_WIDTH_1K;
    composerAdapter_->screenInfo_.phyHeight = DEFAULT_CANVAS_HEIGHT_1K;
    composerAdapter_->LayerCrop(layer);
}

/**
 * @tc.name: LayerScaleDown001
 * @tc.desc: Test RSUniRenderComposerAdapterTest.LayerScaleDown
 * @tc.type: FUNC
 * @tc.require: issueI6S774
 */
HWTEST_F(RSUniRenderComposerAdapterTest, LayerScaleDown001, TestSize.Level1)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);
    RectI dstRect{0, 0, DEFAULT_CANVAS_WIDTH, DEFAULT_CANVAS_HEIGHT};
    surfaceNode->SetSrcRect(dstRect);
    surfaceNode->SetDstRect(dstRect);
    auto layer = composerAdapter_->CreateLayer(*surfaceNode);
    ASSERT_NE(layer, nullptr);
    layer->sbuffer_ = nullptr;
    composerAdapter_->LayerScaleDown(layer, *surfaceNode);
}

/**
 * @tc.name: LayerScaleDown002
 * @tc.desc: Test RSUniRenderComposerAdapterTest.LayerScaleDown
 * @tc.type: FUNC
 * @tc.require: issueI6S774
 */
HWTEST_F(RSUniRenderComposerAdapterTest, LayerScaleDown002, TestSize.Level1)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);
    RectI dstRect{0, 0, DEFAULT_CANVAS_WIDTH, DEFAULT_CANVAS_HEIGHT};
    surfaceNode->SetSrcRect(dstRect);
    surfaceNode->SetDstRect(dstRect);
    auto layer = composerAdapter_->CreateLayer(*surfaceNode);
    ASSERT_NE(layer, nullptr);
    layer->cSurface_ = nullptr;
    composerAdapter_->LayerScaleDown(layer, *surfaceNode);
}

/**
 * @tc.name: LayerScaleDown003
 * @tc.desc: Test RSUniRenderComposerAdapterTest.LayerScaleDown
 * @tc.type: FUNC
 * @tc.require: issueI7FUVJ
 */
HWTEST_F(RSUniRenderComposerAdapterTest, LayerScaleDown003, TestSize.Level1)
{
    SetUp();
    RSSurfaceRenderNodeConfig config;
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);

    RectI dstRect{0, 0, 400, 600};
    surfaceNode->SetSrcRect(dstRect);
    surfaceNode->SetDstRect(dstRect);

    sptr<IConsumerSurface> csurf = IConsumerSurface::Create(config.name);
    ASSERT_NE(csurf, nullptr);
    surfaceNode->SetConsumer(csurf);

    bool statusReady = composerAdapter_->CheckStatusBeforeCreateLayer(*surfaceNode);
    ASSERT_EQ(statusReady, true);
    ComposeInfo info = composerAdapter_->BuildComposeInfo(*surfaceNode);
    auto screenInfo = screenManager_->QueryScreenInfo(screenId_);

    screenInfo.rotation = ScreenRotation::ROTATION_90;
    bool isOutOfRegion = composerAdapter_->IsOutOfScreenRegion(info);
    ASSERT_EQ(isOutOfRegion, false);
    screenInfo.rotation = ScreenRotation::ROTATION_180;
    isOutOfRegion = composerAdapter_->IsOutOfScreenRegion(info);
    ASSERT_EQ(isOutOfRegion, false);
    screenInfo.rotation = ScreenRotation::ROTATION_270;
    ASSERT_EQ(isOutOfRegion, false);
    isOutOfRegion = composerAdapter_->IsOutOfScreenRegion(info);

    LayerInfoPtr layer = HdiLayerInfo::CreateHdiLayerInfo();
    composerAdapter_->SetComposeInfoToLayer(layer, info, surfaceNode->GetConsumer(), &(*surfaceNode));

    composerAdapter_->LayerRotate(layer, *surfaceNode);
    composerAdapter_->LayerCrop(layer);
    composerAdapter_->LayerScaleDown(layer, *surfaceNode);
}

/**
 * @tc.name: LayerScaleFit001
 * @tc.desc: Test RSUniRenderComposerAdapterTest.LayerScaleFit
 * @tc.type: FUNC
 * @tc.require: issueI6S774
 */
HWTEST_F(RSUniRenderComposerAdapterTest, LayerScaleFit001, TestSize.Level1)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);
    RectI dstRect{0, 0, DEFAULT_CANVAS_WIDTH, DEFAULT_CANVAS_HEIGHT};
    surfaceNode->SetSrcRect(dstRect);
    surfaceNode->SetDstRect(dstRect);
    auto layer = composerAdapter_->CreateLayer(*surfaceNode);
    ASSERT_NE(layer, nullptr);
    layer->sbuffer_ = nullptr;
    composerAdapter_->LayerScaleFit(layer, *surfaceNode);
}

/**
 * @tc.name: LayerScaleFit002
 * @tc.desc: Test RSUniRenderComposerAdapterTest.LayerScaleFit
 * @tc.type: FUNC
 * @tc.require: issueI6S774
 */
HWTEST_F(RSUniRenderComposerAdapterTest, LayerScaleFit002, TestSize.Level1)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);
    RectI dstRect{0, 0, DEFAULT_CANVAS_WIDTH, DEFAULT_CANVAS_HEIGHT};
    surfaceNode->SetSrcRect(dstRect);
    surfaceNode->SetDstRect(dstRect);
    auto layer = composerAdapter_->CreateLayer(*surfaceNode);
    ASSERT_NE(layer, nullptr);
    layer->cSurface_ = nullptr;
    composerAdapter_->LayerScaleFit(layer, *surfaceNode);
}

/**
 * @tc.name: LayerScaleFit003
 * @tc.desc: Test RSUniRenderComposerAdapterTest.LayerScaleFit
 * @tc.type: FUNC
 * @tc.require: issueI7FUVJ
 */
HWTEST_F(RSUniRenderComposerAdapterTest, LayerScaleFit003, TestSize.Level1)
{
    SetUp();
    RSSurfaceRenderNodeConfig config;
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);

    RectI dstRect{0, 0, 400, 600};
    surfaceNode->SetSrcRect(dstRect);
    surfaceNode->SetDstRect(dstRect);

    sptr<IConsumerSurface> csurf = IConsumerSurface::Create(config.name);
    ASSERT_NE(csurf, nullptr);
    surfaceNode->SetConsumer(csurf);

    bool statusReady = composerAdapter_->CheckStatusBeforeCreateLayer(*surfaceNode);
    ASSERT_EQ(statusReady, true);
    ComposeInfo info = composerAdapter_->BuildComposeInfo(*surfaceNode);

    LayerInfoPtr layer = HdiLayerInfo::CreateHdiLayerInfo();
    composerAdapter_->SetComposeInfoToLayer(layer, info, surfaceNode->GetConsumer(), &(*surfaceNode));

    composerAdapter_->LayerRotate(layer, *surfaceNode);
    composerAdapter_->LayerCrop(layer);
    composerAdapter_->LayerScaleFit(layer, *surfaceNode);
}

/**
 * @tc.name: IsOutOfScreenRegion003
 * @tc.desc: Test RSUniRenderComposerAdapterTest.IsOutOfScreenRegion
 * @tc.type: FUNC
 * @tc.require: issueI6S774
 */
HWTEST_F(RSUniRenderComposerAdapterTest, IsOutOfScreenRegion003, TestSize.Level1)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);
    ComposeInfo info = composerAdapter_->BuildComposeInfo(*surfaceNode);
    ASSERT_NE(info.buffer, nullptr);
    info.buffer->SetSurfaceBufferWidth(DEFAULT_CANVAS_WIDTH);
    info.buffer->SetSurfaceBufferHeight(DEFAULT_CANVAS_HEIGHT);
    info.dstRect.x = 0;
    info.dstRect.w = 0;
    ASSERT_EQ(true, composerAdapter_->IsOutOfScreenRegion(info));
}

/**
 * @tc.name: IsOutOfScreenRegion004
 * @tc.desc: Test RSUniRenderComposerAdapterTest.IsOutOfScreenRegion
 * @tc.type: FUNC
 * @tc.require: issueI6S774
 */
HWTEST_F(RSUniRenderComposerAdapterTest, IsOutOfScreenRegion004, TestSize.Level1)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);
    ComposeInfo info = composerAdapter_->BuildComposeInfo(*surfaceNode);
    ASSERT_NE(info.buffer, nullptr);
    info.buffer->SetSurfaceBufferWidth(DEFAULT_CANVAS_WIDTH);
    info.buffer->SetSurfaceBufferHeight(DEFAULT_CANVAS_HEIGHT);
    info.dstRect.x = DEFAULT_CANVAS_WIDTH_1K;
    info.dstRect.y = DEFAULT_CANVAS_HEIGHT_1K;
    composerAdapter_->screenInfo_.phyWidth = DEFAULT_CANVAS_WIDTH;
    composerAdapter_->screenInfo_.phyHeight = DEFAULT_CANVAS_HEIGHT;
    ASSERT_EQ(true, composerAdapter_->IsOutOfScreenRegion(info));
}

/**
 * @tc.name: IsOutOfScreenRegion005
 * @tc.desc: Test RSUniRenderComposerAdapterTest.IsOutOfScreenRegion
 * @tc.type: FUNC
 * @tc.require: issueI6S774
 */
HWTEST_F(RSUniRenderComposerAdapterTest, IsOutOfScreenRegion005, TestSize.Level1)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);
    ComposeInfo info = composerAdapter_->BuildComposeInfo(*surfaceNode);
    ASSERT_NE(info.buffer, nullptr);
    info.buffer->SetSurfaceBufferWidth(DEFAULT_CANVAS_WIDTH);
    info.buffer->SetSurfaceBufferHeight(DEFAULT_CANVAS_HEIGHT);
    info.dstRect.x = DEFAULT_CANVAS_WIDTH;
    info.dstRect.y = DEFAULT_CANVAS_HEIGHT_1K;
    composerAdapter_->screenInfo_.phyWidth = DEFAULT_CANVAS_WIDTH_1K;
    composerAdapter_->screenInfo_.phyHeight = DEFAULT_CANVAS_HEIGHT;
    ASSERT_EQ(true, composerAdapter_->IsOutOfScreenRegion(info));
}

/**
 * @tc.name: IsOutOfScreenRegion006
 * @tc.desc: Test RSUniRenderComposerAdapterTest.IsOutOfScreenRegion
 * @tc.type: FUNC
 * @tc.require: issueI6S774
 */
HWTEST_F(RSUniRenderComposerAdapterTest, IsOutOfScreenRegion006, TestSize.Level1)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);
    ComposeInfo info = composerAdapter_->BuildComposeInfo(*surfaceNode);
    ASSERT_NE(info.buffer, nullptr);
    info.buffer->SetSurfaceBufferWidth(DEFAULT_CANVAS_WIDTH);
    info.buffer->SetSurfaceBufferHeight(DEFAULT_CANVAS_HEIGHT);
    info.dstRect.x = DEFAULT_CANVAS_WIDTH;
    info.dstRect.y = 0;
    info.dstRect.h = 0;
    composerAdapter_->screenInfo_.phyWidth = DEFAULT_CANVAS_WIDTH;
    composerAdapter_->screenInfo_.phyHeight = DEFAULT_CANVAS_HEIGHT;
    ASSERT_EQ(true, composerAdapter_->IsOutOfScreenRegion(info));
}

/**
 * @tc.name: CreateBufferLayer001
 * @tc.desc: Test RSUniRenderComposerAdapterTest.CreateBufferLayer
 * @tc.type: FUNC
 * @tc.require: issueI6S774
 */
HWTEST_F(RSUniRenderComposerAdapterTest, CreateBufferLayer001, TestSize.Level1)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);
    RectI dstRect{0, 0, DEFAULT_CANVAS_WIDTH_1K, DEFAULT_CANVAS_HEIGHT_1K};
    surfaceNode->SetSrcRect(dstRect);
    surfaceNode->SetDstRect(dstRect);
    composerAdapter_->screenInfo_.phyWidth = DEFAULT_CANVAS_WIDTH;
    composerAdapter_->screenInfo_.phyHeight = DEFAULT_CANVAS_HEIGHT;
    ASSERT_NE(nullptr, composerAdapter_->CreateBufferLayer(*surfaceNode));
}

/**
 * @tc.name: CreateBufferLayer002
 * @tc.desc: Test RSUniRenderComposerAdapterTest.CreateBufferLayer
 * @tc.type: FUNC
 * @tc.require: issueI6S774
 */
HWTEST_F(RSUniRenderComposerAdapterTest, CreateBufferLayer002, TestSize.Level1)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);
    RectI dstRect{0, 0, DEFAULT_CANVAS_WIDTH, DEFAULT_CANVAS_HEIGHT};
    surfaceNode->SetSrcRect(dstRect);
    surfaceNode->SetDstRect(dstRect);
    composerAdapter_->screenInfo_.phyWidth = DEFAULT_CANVAS_WIDTH_1K;
    composerAdapter_->screenInfo_.phyHeight = DEFAULT_CANVAS_HEIGHT_1K;
    composerAdapter_->output_ = nullptr;
    ASSERT_EQ(nullptr, composerAdapter_->CreateBufferLayer(*surfaceNode));
}

/**
 * @tc.name: CreateBufferLayer003
 * @tc.desc: Test RSUniRenderComposerAdapterTest.CreateBufferLayer
 * @tc.type: FUNC
 * @tc.require: issueI6S774
 */
HWTEST_F(RSUniRenderComposerAdapterTest, CreateBufferLayer003, TestSize.Level1)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);
    RectI dstRect{0, 0, DEFAULT_CANVAS_WIDTH, DEFAULT_CANVAS_HEIGHT};
    surfaceNode->SetSrcRect(dstRect);
    surfaceNode->SetDstRect(dstRect);
    composerAdapter_->screenInfo_.phyWidth = DEFAULT_CANVAS_WIDTH_1K;
    composerAdapter_->screenInfo_.phyHeight = DEFAULT_CANVAS_HEIGHT_1K;
    ASSERT_NE(nullptr, composerAdapter_->CreateBufferLayer(*surfaceNode));
}

/**
 * @tc.name: CreateLayer001
 * @tc.desc: Test RSUniRenderComposerAdapterTest.CreateLayer
 * @tc.type: FUNC
 * @tc.require: issueI6S774
 */
HWTEST_F(RSUniRenderComposerAdapterTest, CreateLayer001, TestSize.Level1)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);
    RectI dstRect{0, 0, DEFAULT_CANVAS_WIDTH, DEFAULT_CANVAS_HEIGHT};
    surfaceNode->SetSrcRect(dstRect);
    surfaceNode->SetDstRect(dstRect);
    composerAdapter_->screenInfo_.phyWidth = DEFAULT_CANVAS_WIDTH_1K;
    composerAdapter_->screenInfo_.phyHeight = DEFAULT_CANVAS_HEIGHT_1K;
    surfaceNode->consumer_ = nullptr;
    ASSERT_EQ(nullptr, composerAdapter_->CreateLayer(*surfaceNode));
}

/**
 * @tc.name: SetLayerSize001
 * @tc.desc: Test RSUniRenderComposerAdapterTest.SetLayerSize
 * @tc.type: FUNC
 * @tc.require: issueI6S774
 */
HWTEST_F(RSUniRenderComposerAdapterTest, SetLayerSize001, TestSize.Level1)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);
    RectI dstRect{0, 0, DEFAULT_CANVAS_WIDTH, DEFAULT_CANVAS_HEIGHT};
    surfaceNode->SetSrcRect(dstRect);
    surfaceNode->SetDstRect(dstRect);
    composerAdapter_->screenInfo_.phyWidth = DEFAULT_CANVAS_WIDTH_1K;
    composerAdapter_->screenInfo_.phyHeight = DEFAULT_CANVAS_HEIGHT_1K;
    auto layer = composerAdapter_->CreateLayer(*surfaceNode);
    ASSERT_NE(layer, nullptr);
    surfaceNode->consumer_ = nullptr;
    composerAdapter_->screenInfo_.rotation = ScreenRotation::ROTATION_90;
    composerAdapter_->LayerRotate(layer, *surfaceNode);

    composerAdapter_->screenInfo_.rotation = ScreenRotation::ROTATION_180;
    composerAdapter_->LayerRotate(layer, *surfaceNode);

    composerAdapter_->screenInfo_.rotation = ScreenRotation::ROTATION_270;
    composerAdapter_->LayerRotate(layer, *surfaceNode);

    composerAdapter_->screenInfo_.rotation = ScreenRotation::ROTATION_0;
    composerAdapter_->LayerRotate(layer, *surfaceNode);
}

/**
 * @tc.name: GetComposerInfoNeedClient001
 * @tc.desc: Test RSUniRenderComposerAdapterTest.GetComposerInfoNeedClient while
 *           color gamut of screen and buffer are different
 * @tc.type: FUNC
 * @tc.require: issueI7O6WO
 */
HWTEST_F(RSUniRenderComposerAdapterTest, GetComposerInfoNeedClient001, TestSize.Level2)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);
    ComposeInfo info = composerAdapter_->BuildComposeInfo(*surfaceNode);
    ASSERT_NE(info.buffer, nullptr);
    
    info.buffer->SetSurfaceBufferColorGamut(GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB);
    composerAdapter_->screenInfo_.colorGamut = ScreenColorGamut::COLOR_GAMUT_DISPLAY_BT2020;
    ASSERT_TRUE(composerAdapter_->GetComposerInfoNeedClient(info, *surfaceNode));
}

/**
 * @tc.name: GetComposerInfoNeedClient002
 * @tc.desc: Test RSUniRenderComposerAdapterTest.GetComposerInfoNeedClient with RSRenderNode
 * @tc.type: FUNC
 * @tc.require: issueI7O6WO
 */
HWTEST_F(RSUniRenderComposerAdapterTest, GetComposerInfoNeedClient002, TestSize.Level2)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);
    ComposeInfo info = composerAdapter_->BuildComposeInfo(*surfaceNode);
    ASSERT_NE(info.buffer, nullptr);
    
    auto node = surfaceNode->ReinterpretCastTo<RSRenderNode>();
    info.buffer->SetSurfaceBufferColorGamut(GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB);
    composerAdapter_->screenInfo_.colorGamut = ScreenColorGamut::COLOR_GAMUT_DISPLAY_BT2020;
    ASSERT_TRUE(composerAdapter_->GetComposerInfoNeedClient(info, *node));
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
    surfaceNode->GetConsumer()->SetTransform(GraphicTransformType::GRAPHIC_FLIP_H_ROT90);
    auto srcRect = composerAdapter_->SrcRectRotateTransform(*surfaceNode);
    ASSERT_EQ(srcRect.top_, DEFAULT_CANVAS_WIDTH * 0.5);
}

/**
 * @tc.name: LayerCrop003
 * @tc.desc: Test RSUniRenderComposerAdapterTest.LayerCrop while
 *           screen size is smaller than layer size
 * @tc.type: FUNC
 * @tc.require: issueI7O6WO
 */
HWTEST_F(RSUniRenderComposerAdapterTest, LayerCrop003, TestSize.Level2)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);
    RectI rect{0, 0, DEFAULT_CANVAS_WIDTH, DEFAULT_CANVAS_HEIGHT};
    surfaceNode->SetSrcRect(rect);
    surfaceNode->SetDstRect(rect);
    LayerInfoPtr layer = composerAdapter_->CreateLayer(*surfaceNode);
    ASSERT_NE(layer, nullptr);

    layer->layerRect_ = {0, 0, DEFAULT_CANVAS_WIDTH, DEFAULT_CANVAS_HEIGHT};
    layer->cropRect_ = {0, 0, DEFAULT_CANVAS_WIDTH, DEFAULT_CANVAS_HEIGHT};
    composerAdapter_->screenInfo_.phyWidth = DEFAULT_CANVAS_WIDTH * 0.5;
    composerAdapter_->screenInfo_.phyHeight = DEFAULT_CANVAS_HEIGHT * 0.5;
    
    composerAdapter_->LayerCrop(layer);
    ASSERT_EQ(layer->layerRect_.w, DEFAULT_CANVAS_WIDTH * 0.5);
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
    surfaceNode->consumer_ = nullptr;
    ASSERT_EQ(composerAdapter_->SrcRectRotateTransform(*surfaceNode), rect);
}

/**
 * @tc.name: CheckStatusBeforeCreateLayer008
 * @tc.desc: Test RSUniRenderComposerAdapterTest.CheckStatusBeforeCreateLayer while
 *           bounds geometry is nullptr
 * @tc.type: FUNC
 * @tc.require: issuesI7T9RE
 */
HWTEST_F(RSUniRenderComposerAdapterTest, CheckStatusBeforeCreateLayer008, TestSize.Level2)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);
    surfaceNode->GetMutableRenderProperties().boundsGeo_ = nullptr;
    RectI dstRect{0, 0, DEFAULT_CANVAS_WIDTH, DEFAULT_CANVAS_HEIGHT};
    surfaceNode->SetSrcRect(dstRect);
    surfaceNode->SetDstRect(dstRect);
    ASSERT_FALSE(composerAdapter_->CheckStatusBeforeCreateLayer(*surfaceNode));
}

/**
 * @tc.name: LayerScaleDown004
 * @tc.desc: Test RSUniRenderComposerAdapterTest.LayerScaleDown while
 *           scaling mode is SCALING_MODE_SCALE_CROP
 * @tc.type: FUNC
 * @tc.require: issuesI7T9RE
 */
HWTEST_F(RSUniRenderComposerAdapterTest, LayerScaleDown004, TestSize.Level2)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);
    RectI rect{0, 0, DEFAULT_CANVAS_WIDTH, DEFAULT_CANVAS_HEIGHT};
    surfaceNode->SetSrcRect(rect);
    surfaceNode->SetDstRect(rect);

    LayerInfoPtr layer = composerAdapter_->CreateLayer(*surfaceNode);
    ASSERT_NE(layer, nullptr);
    GraphicIRect gRect = {0, 0, DEFAULT_CANVAS_WIDTH, DEFAULT_CANVAS_HEIGHT};
    layer->SetCropRect(gRect);
    layer->SetLayerSize(gRect);
    std::vector<GraphicIRect> dirtyRegions;
    dirtyRegions.emplace_back(gRect);
    layer->SetDirtyRegions(dirtyRegions);

    ScalingMode scalingMode = ScalingMode::SCALING_MODE_SCALE_CROP;
    surfaceNode->GetConsumer()->AttachBuffer(layer->sbuffer_);
    surfaceNode->GetConsumer()->SetScalingMode(layer->GetBuffer()->GetSeqNum(), scalingMode);
    
    composerAdapter_->LayerScaleDown(layer, *surfaceNode);
    ASSERT_TRUE(layer->GetDirtyRegions()[0].w == DEFAULT_CANVAS_WIDTH);
}

/**
 * @tc.name: LayerScaleDown005
 * @tc.desc: Test RSUniRenderComposerAdapterTest.LayerScaleDown while
 *           surface rotate angle is not a multiple of FLAT_ANGLE
 * @tc.type: FUNC
 * @tc.require: issuesI7T9RE
 */
HWTEST_F(RSUniRenderComposerAdapterTest, LayerScaleDown005, TestSize.Level2)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);
    RectI rect{0, 0, DEFAULT_CANVAS_WIDTH, DEFAULT_CANVAS_HEIGHT};
    surfaceNode->SetSrcRect(rect);
    surfaceNode->SetDstRect(rect);

    LayerInfoPtr layer = composerAdapter_->CreateLayer(*surfaceNode);
    ASSERT_NE(layer, nullptr);
    GraphicIRect gRect = {0, 0, DEFAULT_CANVAS_WIDTH, DEFAULT_CANVAS_HEIGHT};
    layer->SetCropRect(gRect);
    layer->SetLayerSize(gRect);
    std::vector<GraphicIRect> dirtyRegions;
    dirtyRegions.emplace_back(gRect);
    layer->SetDirtyRegions(dirtyRegions);

    ScalingMode scalingMode = ScalingMode::SCALING_MODE_SCALE_CROP;
    surfaceNode->GetConsumer()->AttachBuffer(layer->sbuffer_);
    surfaceNode->GetConsumer()->SetScalingMode(layer->GetBuffer()->GetSeqNum(), scalingMode);

    surfaceNode->GetConsumer()->SetTransform(GraphicTransformType::GRAPHIC_ROTATE_90);
    composerAdapter_->LayerScaleDown(layer, *surfaceNode);
    ASSERT_FALSE(layer->GetDirtyRegions()[0].w == DEFAULT_CANVAS_WIDTH);
}

/**
 * @tc.name: LayerScaleDown006
 * @tc.desc: Test RSUniRenderComposerAdapterTest.LayerScaleDown while
 *           surface's width is smaller than surface's heigth
 * @tc.type: FUNC
 * @tc.require: issuesI7T9RE
 */
HWTEST_F(RSUniRenderComposerAdapterTest, LayerScaleDown006, TestSize.Level2)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);
    RectI rect{0, 0, DEFAULT_CANVAS_HEIGHT, DEFAULT_CANVAS_WIDTH};
    surfaceNode->SetSrcRect(rect);
    surfaceNode->SetDstRect(rect);
    
    LayerInfoPtr layer = composerAdapter_->CreateLayer(*surfaceNode);
    ASSERT_NE(layer, nullptr);
    GraphicIRect gRect = {0, 0, DEFAULT_CANVAS_WIDTH, DEFAULT_CANVAS_HEIGHT};
    layer->SetCropRect(gRect);
    layer->SetLayerSize(gRect);
    std::vector<GraphicIRect> dirtyRegions;
    dirtyRegions.emplace_back(gRect);
    layer->SetDirtyRegions(dirtyRegions);

    ScalingMode scalingMode = ScalingMode::SCALING_MODE_SCALE_CROP;
    surfaceNode->GetConsumer()->AttachBuffer(layer->sbuffer_);
    surfaceNode->GetConsumer()->SetScalingMode(layer->GetBuffer()->GetSeqNum(), scalingMode);

    surfaceNode->GetConsumer()->SetTransform(GraphicTransformType::GRAPHIC_ROTATE_90);
    composerAdapter_->LayerScaleDown(layer, *surfaceNode);
    ASSERT_FALSE(layer->GetDirtyRegions()[0].h == DEFAULT_CANVAS_WIDTH);
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

    RSDisplayNodeConfig config;
    RSDisplayRenderNode::SharedPtr nodePtr = std::make_shared<RSDisplayRenderNode>(1, config);

    sptr<IBufferConsumerListener> listener = new RSUniRenderListener(nodePtr);
    nodePtr->CreateSurface(listener);

    auto rsSurface = nodePtr->GetRSSurface();
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

    nodePtr->SetBuffer(buffer, SyncFence::INVALID_FENCE, Rect(), 0);

    RSUniRenderComposerAdapter::SetBufferColorSpace(*nodePtr);

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
    RSDisplayNodeConfig config;
    auto node = std::make_shared<RSDisplayRenderNode>(id, config);
    composerAdapter_->SetBufferColorSpace(*node);
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
    RSDisplayNodeConfig config;
    auto node = std::make_shared<RSDisplayRenderNode>(id, config);
    composerAdapter_->SetBufferColorSpace(*node);
    sptr<SyncFence> acquireFence = SyncFence::INVALID_FENCE;
    int64_t timestamp = 0;
    Rect damage;
    sptr<OHOS::SurfaceBuffer> buffer = new SurfaceBufferImpl(0);
    node->SetBuffer(buffer, acquireFence, damage, timestamp);
    composerAdapter_->SetBufferColorSpace(*node);
}
} // namespace