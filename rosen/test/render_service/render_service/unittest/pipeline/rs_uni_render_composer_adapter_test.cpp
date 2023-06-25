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
#include "rs_test_util.h"

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
    const uint32_t screenId_ = 0;
    const int32_t offsetX = 0; // screenOffset on x axis equals to 0
    const int32_t offsetY = 0; // screenOffset on y axis equals to 0
    const float mirrorAdaptiveCoefficient = 1.0f;

    sptr<RSScreenManager> screenManager_;
    std::unique_ptr<RSUniRenderComposerAdapter> composerAdapter_;
};

void RSUniRenderComposerAdapterTest::SetUpTestCase() {}
void RSUniRenderComposerAdapterTest::TearDownTestCase() {}
void RSUniRenderComposerAdapterTest::TearDown() {}
void RSUniRenderComposerAdapterTest::SetUp()
{
    screenManager_ = CreateOrGetScreenManager();
    ASSERT_NE(screenManager_, nullptr);
    screenManager_->Init();
    uint32_t width = 2560;
    uint32_t height = 1080;
    ScreenColorGamut colorGamut = ScreenColorGamut::COLOR_GAMUT_SRGB;
    ScreenState state = ScreenState::UNKNOWN;
    ScreenRotation rotation = ScreenRotation::ROTATION_0;
    std::unique_ptr<impl::RSScreen> rsScreen =
        std::make_unique<impl::RSScreen>(screenId_, false, HdiOutput::CreateHdiOutput(screenId_), nullptr);
    ASSERT_NE(rsScreen, nullptr);
    screenManager_->MockHdiScreenConnected(rsScreen);
    auto info = screenManager_->QueryScreenInfo(screenId_);
    info.width = width;
    info.height = height;
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
    surfaceNode->renderProperties_.SetBoundsWidth(DEFAULT_CANVAS_WIDTH);
    surfaceNode->renderProperties_.SetBoundsHeight(DEFAULT_CANVAS_HEIGHT);
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
    surfaceNode->renderProperties_.SetBoundsWidth(DEFAULT_CANVAS_WIDTH);
    surfaceNode->renderProperties_.SetBoundsHeight(DEFAULT_CANVAS_HEIGHT);
    RectI dstRect{0, 0, DEFAULT_CANVAS_WIDTH, DEFAULT_CANVAS_HEIGHT};
    surfaceNode->SetSrcRect(dstRect);
    surfaceNode->SetDstRect(dstRect);
    ComposeInfo info = composerAdapter_->BuildComposeInfo(*surfaceNode);
    ASSERT_NE(info.buffer, nullptr);
    info.buffer->SetSurfaceBufferWidth(DEFAULT_CANVAS_WIDTH);
    info.buffer->SetSurfaceBufferHeight(DEFAULT_CANVAS_HEIGHT);
    composerAdapter_->GetComposerInfoSrcRect(info, *surfaceNode);

    surfaceNode->renderProperties_.SetBoundsWidth(DEFAULT_CANVAS_WIDTH_1K);
    surfaceNode->renderProperties_.SetBoundsHeight(DEFAULT_CANVAS_HEIGHT_1K);
    ComposeInfo info2 = composerAdapter_->BuildComposeInfo(*surfaceNode);
    composerAdapter_->GetComposerInfoSrcRect(info2, *surfaceNode);
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
    surfaceNode->renderProperties_.SetBoundsWidth(DEFAULT_CANVAS_WIDTH);
    surfaceNode->renderProperties_.SetBoundsHeight(DEFAULT_CANVAS_HEIGHT);
    ASSERT_NE(surfaceNode, nullptr);
    RectI dstRect{0, 0, DEFAULT_CANVAS_WIDTH, DEFAULT_CANVAS_HEIGHT};
    surfaceNode->SetSrcRect(dstRect);
    surfaceNode->SetDstRect(dstRect);
    ComposeInfo info = composerAdapter_->BuildComposeInfo(*surfaceNode);
    ASSERT_NE(info.buffer, nullptr);
    info.buffer->SetSurfaceBufferWidth(DEFAULT_CANVAS_WIDTH);
    info.buffer->SetSurfaceBufferHeight(DEFAULT_CANVAS_HEIGHT);

    surfaceNode->renderProperties_.frameGravity_ = Gravity::RESIZE;
    composerAdapter_->DealWithNodeGravity(*surfaceNode, info);

    surfaceNode->renderProperties_.frameGravity_ = Gravity::TOP;
    surfaceNode->renderProperties_.SetBoundsWidth(DEFAULT_CANVAS_WIDTH_1K);
    surfaceNode->renderProperties_.SetBoundsHeight(DEFAULT_CANVAS_HEIGHT_1K);
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
    surfaceNode->renderProperties_.SetBoundsWidth(DEFAULT_CANVAS_WIDTH);
    surfaceNode->renderProperties_.SetBoundsHeight(DEFAULT_CANVAS_HEIGHT);
    ASSERT_NE(surfaceNode, nullptr);
    RectI dstRect{0, 0, DEFAULT_CANVAS_WIDTH, DEFAULT_CANVAS_HEIGHT};
    surfaceNode->SetSrcRect(dstRect);
    surfaceNode->SetDstRect(dstRect);
    ComposeInfo info = composerAdapter_->BuildComposeInfo(*surfaceNode);
    ASSERT_NE(info.buffer, nullptr);
    info.buffer->SetSurfaceBufferWidth(DEFAULT_CANVAS_WIDTH);
    info.buffer->SetSurfaceBufferHeight(DEFAULT_CANVAS_HEIGHT);

    surfaceNode->renderProperties_.frameGravity_ = Gravity::TOP;
    composerAdapter_->DealWithNodeGravity(*surfaceNode, info);

    surfaceNode->renderProperties_.SetBoundsWidth(DEFAULT_CANVAS_WIDTH);
    surfaceNode->renderProperties_.SetBoundsHeight(DEFAULT_CANVAS_HEIGHT_1K);
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
    surfaceNode->renderProperties_.SetBoundsWidth(DEFAULT_CANVAS_WIDTH);
    surfaceNode->renderProperties_.SetBoundsHeight(DEFAULT_CANVAS_HEIGHT);
    ASSERT_NE(surfaceNode, nullptr);
    RectI dstRect{0, 0, DEFAULT_CANVAS_WIDTH, DEFAULT_CANVAS_HEIGHT};
    surfaceNode->SetSrcRect(dstRect);
    surfaceNode->SetDstRect(dstRect);
    ComposeInfo info = composerAdapter_->BuildComposeInfo(*surfaceNode);
    ASSERT_NE(info.buffer, nullptr);
    info.buffer->SetSurfaceBufferWidth(DEFAULT_CANVAS_WIDTH);
    info.buffer->SetSurfaceBufferHeight(DEFAULT_CANVAS_HEIGHT);

    surfaceNode->renderProperties_.frameGravity_ = Gravity::TOP;
    surfaceNode->renderProperties_.SetBoundsHeight(DEFAULT_CANVAS_HEIGHT_1K);
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
    surfaceNode->renderProperties_.SetBoundsWidth(DEFAULT_CANVAS_WIDTH);
    surfaceNode->renderProperties_.SetBoundsHeight(DEFAULT_CANVAS_HEIGHT);
    ASSERT_NE(surfaceNode, nullptr);
    RectI dstRect{0, 0, DEFAULT_CANVAS_WIDTH, DEFAULT_CANVAS_HEIGHT};
    surfaceNode->SetSrcRect(dstRect);
    surfaceNode->SetDstRect(dstRect);
    ComposeInfo info = composerAdapter_->BuildComposeInfo(*surfaceNode);
    ASSERT_NE(info.buffer, nullptr);
    info.buffer->SetSurfaceBufferWidth(DEFAULT_CANVAS_WIDTH);
    info.buffer->SetSurfaceBufferHeight(DEFAULT_CANVAS_HEIGHT);

    surfaceNode->renderProperties_.frameGravity_ = Gravity::TOP;
    surfaceNode->renderProperties_.SetBoundsHeight(DEFAULT_CANVAS_HEIGHT_1K);
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
    surfaceNode->renderProperties_.SetBoundsWidth(DEFAULT_CANVAS_WIDTH);
    surfaceNode->renderProperties_.SetBoundsHeight(DEFAULT_CANVAS_HEIGHT);
    ASSERT_NE(surfaceNode, nullptr);
    RectI dstRect{0, 0, DEFAULT_CANVAS_WIDTH, DEFAULT_CANVAS_HEIGHT};
    surfaceNode->SetSrcRect(dstRect);
    surfaceNode->SetDstRect(dstRect);
    ComposeInfo info = composerAdapter_->BuildComposeInfo(*surfaceNode);
    ASSERT_NE(info.buffer, nullptr);
    info.buffer->SetSurfaceBufferWidth(DEFAULT_CANVAS_WIDTH);
    info.buffer->SetSurfaceBufferHeight(DEFAULT_CANVAS_HEIGHT);

    surfaceNode->renderProperties_.frameGravity_ = Gravity::TOP;
    surfaceNode->renderProperties_.SetBoundsHeight(DEFAULT_CANVAS_HEIGHT_1K);
    composerAdapter_->screenInfo_.rotation = ScreenRotation::ROTATION_180;
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
    composerAdapter_->screenInfo_.width = DEFAULT_CANVAS_WIDTH;
    composerAdapter_->screenInfo_.height = DEFAULT_CANVAS_HEIGHT;
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
    composerAdapter_->screenInfo_.width = DEFAULT_CANVAS_WIDTH_1K;
    composerAdapter_->screenInfo_.height = DEFAULT_CANVAS_HEIGHT_1K;
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
 * @tc.name: IsOutOfScreenRegion001
 * @tc.desc: Test RSUniRenderComposerAdapterTest.IsOutOfScreenRegion
 * @tc.type: FUNC
 * @tc.require: issueI6S774
 */
HWTEST_F(RSUniRenderComposerAdapterTest, IsOutOfScreenRegion001, TestSize.Level1)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);
    ComposeInfo info = composerAdapter_->BuildComposeInfo(*surfaceNode);
    ASSERT_NE(info.buffer, nullptr);
    info.buffer->SetSurfaceBufferWidth(DEFAULT_CANVAS_WIDTH);
    info.buffer->SetSurfaceBufferHeight(DEFAULT_CANVAS_HEIGHT);
    composerAdapter_->screenInfo_.rotation = ScreenRotation::ROTATION_90;
    ASSERT_NE(true, composerAdapter_->IsOutOfScreenRegion(info));
}

/**
 * @tc.name: IsOutOfScreenRegion002
 * @tc.desc: Test RSUniRenderComposerAdapterTest.IsOutOfScreenRegion
 * @tc.type: FUNC
 * @tc.require: issueI6S774
 */
HWTEST_F(RSUniRenderComposerAdapterTest, IsOutOfScreenRegion002, TestSize.Level1)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);
    ComposeInfo info = composerAdapter_->BuildComposeInfo(*surfaceNode);
    ASSERT_NE(info.buffer, nullptr);
    info.buffer->SetSurfaceBufferWidth(DEFAULT_CANVAS_WIDTH);
    info.buffer->SetSurfaceBufferHeight(DEFAULT_CANVAS_HEIGHT);
    composerAdapter_->screenInfo_.rotation = ScreenRotation::ROTATION_270;
    ASSERT_NE(true, composerAdapter_->IsOutOfScreenRegion(info));
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
    composerAdapter_->screenInfo_.width = DEFAULT_CANVAS_WIDTH;
    composerAdapter_->screenInfo_.height = DEFAULT_CANVAS_HEIGHT;
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
    composerAdapter_->screenInfo_.width = DEFAULT_CANVAS_WIDTH_1K;
    composerAdapter_->screenInfo_.height = DEFAULT_CANVAS_HEIGHT;
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
    composerAdapter_->screenInfo_.width = DEFAULT_CANVAS_WIDTH;
    composerAdapter_->screenInfo_.height = DEFAULT_CANVAS_HEIGHT;
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
    composerAdapter_->screenInfo_.width = DEFAULT_CANVAS_WIDTH;
    composerAdapter_->screenInfo_.height = DEFAULT_CANVAS_HEIGHT;
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
    composerAdapter_->screenInfo_.width = DEFAULT_CANVAS_WIDTH_1K;
    composerAdapter_->screenInfo_.height = DEFAULT_CANVAS_HEIGHT_1K;
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
    composerAdapter_->screenInfo_.width = DEFAULT_CANVAS_WIDTH_1K;
    composerAdapter_->screenInfo_.height = DEFAULT_CANVAS_HEIGHT_1K;
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
    composerAdapter_->screenInfo_.width = DEFAULT_CANVAS_WIDTH_1K;
    composerAdapter_->screenInfo_.height = DEFAULT_CANVAS_HEIGHT_1K;
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
    composerAdapter_->screenInfo_.width = DEFAULT_CANVAS_WIDTH_1K;
    composerAdapter_->screenInfo_.height = DEFAULT_CANVAS_HEIGHT_1K;
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
} // namespace