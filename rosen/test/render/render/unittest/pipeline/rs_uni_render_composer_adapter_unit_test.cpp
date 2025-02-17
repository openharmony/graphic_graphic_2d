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
#include "drawable/rs_display_render_node_drawable.h"
#include "pipeline/render_thread/rs_uni_render_composer_adapter.h"
#include "pipeline/rs_uni_render_listener.h"
#include "surface_buffer_impl.h"
#include "rs_test_util.h"
#include "metadata_helper.h"

using namespace testing;
using namespace testing::ext;

namespace {
    constexpr uint32_t DEFAULT_CANVAS_WIDTH = 80;
    constexpr uint32_t DEFAULT_CANVAS_HEIGHT = 60;
    constexpr uint32_t DEFAULT_CANVAS_WIDTH_1K = 1920;
    constexpr uint32_t DEFAULT_CANVAS_HEIGHT_1K = 1080;
}

namespace OHOS::Rosen {
class RsUniRenderComposerTest : public testing::Test {
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

void RsUniRenderComposerTest::SetUpTestCase() {}
void RsUniRenderComposerTest::TearDownTestCase() {}
void RsUniRenderComposerTest::TearDown()
{
    screenManager_ = OHOS::Rosen::impl::RSScreenManager::GetInstance();
    OHOS::Rosen::impl::RSScreenManager& screenManager =
        static_cast<OHOS::Rosen::impl::RSScreenManager&>(*screenManager_);
    screenManager.screens_.erase(screenId_);
}
void RsUniRenderComposerTest::SetUp()
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
 * @tc.name: DealWithNodeGravity01
 * @tc.desc: Test RsUniRenderComposerTest.DealWithNodeGravity when frameGravity is RESIZE
 * @tc.type: FUNC
 * @tc.require: issue1I6S774
 */
HWTEST_F(RsUniRenderComposerTest, DealWithNodeGravity01, TestSize.Level1)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    surfaceNode->GetRSSurfaceHandler()->GetConsumer()->SetTransform(GraphicTransformType::GRAPHIC_FLIP_H_ROT180);
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
 * @tc.name: DealWithNodeGravity02
 * @tc.desc: Test RsUniRenderComposerTest.DealWithNodeGravity when frameGravity is TOP
 * @tc.type: FUNC
 * @tc.require: issue1I6S774
 */
HWTEST_F(RsUniRenderComposerTest, DealWithNodeGravity02, TestSize.Level1)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    surfaceNode->GetRSSurfaceHandler()->GetConsumer()->SetTransform(GraphicTransformType::GRAPHIC_FLIP_H_ROT180);
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
 * @tc.name: DealWithNodeGravity03
 * @tc.desc: Test RsUniRenderComposerTest.DealWithNodeGravity when rotation is ROTATION_90
 * @tc.type: FUNC
 * @tc.require: issue1I6S774
 */
HWTEST_F(RsUniRenderComposerTest, DealWithNodeGravity03, TestSize.Level1)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    surfaceNode->GetRSSurfaceHandler()->GetConsumer()->SetTransform(GraphicTransformType::GRAPHIC_FLIP_H_ROT180);
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
 * @tc.name: DealWithNodeGravity04
 * @tc.desc: Test RsUniRenderComposerTest.DealWithNodeGravity when rotation is ROTATION_270
 * @tc.type: FUNC
 * @tc.require: issue1I6S774
 */
HWTEST_F(RsUniRenderComposerTest, DealWithNodeGravity04, TestSize.Level1)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    surfaceNode->GetRSSurfaceHandler()->GetConsumer()->SetTransform(GraphicTransformType::GRAPHIC_FLIP_H_ROT180);
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
 * @tc.name: DealWithNodeGravity05
 * @tc.desc: Test RsUniRenderComposerTest.DealWithNodeGravity when rotation is ROTATION_180
 * @tc.type: FUNC
 * @tc.require: issue1I6S774
 */
HWTEST_F(RsUniRenderComposerTest, DealWithNodeGravity05, TestSize.Level1)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    surfaceNode->GetRSSurfaceHandler()->GetConsumer()->SetTransform(GraphicTransformType::GRAPHIC_FLIP_H_ROT180);
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
 * @tc.name: DealWithNodeGravity06
 * @tc.desc: Test RsUniRenderComposerTest.DealWithNodeGravity
 * @tc.type: FUNC
 * @tc.require: issue1I7FUVJ
 */
HWTEST_F(RsUniRenderComposerTest, DealWithNodeGravity06, TestSize.Level1)
{
    SetUp();
    RSSurfaceRenderNodeConfig config;
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);

    RectI dstRect{0, 0, 40, 60};
    surfaceNode->SetSrcRect(dstRect);
    surfaceNode->SetDstRect(dstRect);

    sptr<IConsumerSurface> csurf = IConsumerSurface::Create(config.name);
    ASSERT_NE(csurf, nullptr);
    surfaceNode->GetRSSurfaceHandler()->SetConsumer(csurf);

    composerAdapter_->CheckStatusBeforeCreateLayer(*surfaceNode);
    ComposeInfo info = composerAdapter_->BuildComposeInfo(*surfaceNode);
    composerAdapter_->DealWithNodeGravity(*surfaceNode, info);
}

/**
 * @tc.name: Start01
 * @tc.desc: Test RsUniRenderComposerTest.CommitLayers
 * @tc.type: FUNC
 * @tc.require: issue1I6S774
 */
HWTEST_F(RsUniRenderComposerTest, Start01, TestSize.Level1)
{
    SetUp();
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);
    RectI dstRect{0, 0, 40, 60};
    surfaceNode->SetSrcRect(dstRect);
    surfaceNode->SetDstRect(dstRect);
    auto surfaceDrawable = DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(surfaceNode);
    auto& params = surfaceDrawable->GetRenderParams();
    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(params.get());
    surfaceParams->SetBuffer(surfaceNode->GetRSSurfaceHandler()->GetBuffer(),
        surfaceNode->GetRSSurfaceHandler()->GetDamageRegion());
    RSLayerInfo layerInfo;
    layerInfo.srcRect.w = 1;
    layerInfo.srcRect.h = 1;
    layerInfo.dstRect.w = 1;
    layerInfo.dstRect.h = 1;
    surfaceParams->SetLayerInfo(layerInfo);
    ASSERT_NE(surfaceDrawable, nullptr);
    auto layer1 =
        composerAdapter_->CreateLayer(static_cast<DrawableV2::RSSurfaceRenderNodeDrawable&>(*surfaceDrawable));
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
 * @tc.name: BuildComposeInfo01
 * @tc.desc: Test RsUniRenderComposerTest.BuildComposeInfo
 * @tc.type: FUNC
 * @tc.require: issue1I6S774
 */
HWTEST_F(RsUniRenderComposerTest, BuildComposeInfo01, TestSize.Level1)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);
    SetUp();
    auto drawable = DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(surfaceNode);
    ASSERT_NE(drawable, nullptr);
    auto& params = drawable->GetRenderParams();
    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(params.get());
    surfaceParams->SetBuffer(surfaceNode->GetRSSurfaceHandler()->GetBuffer(),
        surfaceNode->GetRSSurfaceHandler()->GetDamageRegion());
    composerAdapter_->BuildComposeInfo(static_cast<DrawableV2::RSSurfaceRenderNodeDrawable&>(*drawable));
}

/**
 * @tc.name: BuildComposeInfo02
 * @tc.desc: Test RsUniRenderComposerTest.BuildComposeInfo
 * @tc.type: FUNC
 * @tc.require: issue1I7FUVJ
 */
HWTEST_F(RsUniRenderComposerTest, BuildComposeInfo02, TestSize.Level1)
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
    surfaceNode->GetRSSurfaceHandler()->SetConsumer(csurf);
    srcRect = composerAdapter_->SrcRectRotateTransform(*surfaceNode);
    ASSERT_EQ(srcRect.IsEmpty(), true);
    RectI dstRect{0, 0, 40, 60};
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
 * @tc.name: GetComposerInfoSrcRect01
 * @tc.desc: Test RsUniRenderComposerTest.GetComposerInfoSrcRect with GRAPHIC_FLIP_V_ROT90
 * @tc.type: FUNC
 * @tc.require: issue1I6S774
 */
HWTEST_F(RsUniRenderComposerTest, GetComposerInfoSrcRect01, TestSize.Level1)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    surfaceNode->GetRSSurfaceHandler()->GetConsumer()->SetTransform(GraphicTransformType::GRAPHIC_FLIP_V_ROT90);
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
    ASSERT_EQ(0, info.srcRect.x);
    ASSERT_EQ(0, info.srcRect.y);
}

/**
 * @tc.name: GetComposerInfoSrcRect02
 * @tc.desc: Test RsUniRenderComposerTest.GetComposerInfoSrcRect with GRAPHIC_FLIP_H_ROT180
 * @tc.type: FUNC
 * @tc.require: issue1I6S774
 */
HWTEST_F(RsUniRenderComposerTest, GetComposerInfoSrcRect02, TestSize.Level1)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    surfaceNode->GetRSSurfaceHandler()->GetConsumer()->SetTransform(GraphicTransformType::GRAPHIC_FLIP_H_ROT180);
    ASSERT_NE(surfaceNode, nullptr);
    RectI dstRect{0, 0, DEFAULT_CANVAS_WIDTH, DEFAULT_CANVAS_HEIGHT};
    surfaceNode->SetSrcRect(dstRect);
    surfaceNode->SetDstRect(dstRect);
    ComposeInfo info = composerAdapter_->BuildComposeInfo(*surfaceNode);
    composerAdapter_->GetComposerInfoSrcRect(info, *surfaceNode);
}

/**
 * @tc.name: GetComposerInfoSrcRect03
 * @tc.desc: Test RsUniRenderComposerTest.GetComposerInfoSrcRect with GRAPHIC_FLIP_H_ROT270
 * @tc.type: FUNC
 * @tc.require: issue1I6S774
 */
HWTEST_F(RsUniRenderComposerTest, GetComposerInfoSrcRect03, TestSize.Level1)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    surfaceNode->GetRSSurfaceHandler()->GetConsumer()->SetTransform(GraphicTransformType::GRAPHIC_FLIP_H_ROT270);
    ASSERT_NE(surfaceNode, nullptr);
    RectI dstRect{0, 0, DEFAULT_CANVAS_WIDTH, DEFAULT_CANVAS_HEIGHT};
    surfaceNode->SetSrcRect(dstRect);
    surfaceNode->SetDstRect(dstRect);
    ComposeInfo info = composerAdapter_->BuildComposeInfo(*surfaceNode);
    composerAdapter_->GetComposerInfoSrcRect(info, *surfaceNode);
}

/**
 * @tc.name: GetComposerInfoSrcRect04
 * @tc.desc: Test RsUniRenderComposerTest.GetComposerInfoSrcRect
 * @tc.type: FUNC
 * @tc.require: issue1I6S774
 */
HWTEST_F(RsUniRenderComposerTest, GetComposerInfoSrcRect04, TestSize.Level1)
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
 * @tc.name: GetComposerInfoSrcRect05
 * @tc.desc: GetComposerInfoSrcRect, Bounds size != Buffer size
 * @tc.type: FUNC
 * @tc.require: issue1I6S774
 */
HWTEST_F(RsUniRenderComposerTest, GetComposerInfoSrcRect05, TestSize.Level1)
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
 * @tc.name: GetComposerInfoSrcRect06
 * @tc.desc: GetComposerInfoSrcRect, Bounds size != Buffer size and FrameGravity != top left
 * @tc.type: FUNC
 * @tc.require: issue1I6S774
 */
HWTEST_F(RsUniRenderComposerTest, GetComposerInfoSrcRect06, TestSize.Level1)
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
    surfaceNode->GetRSSurfaceHandler()->GetConsumer()->SetScalingMode(info.buffer->GetSeqNum(), scalingMode);
    composerAdapter_->GetComposerInfoSrcRect(info, *surfaceNode);
}

/**
 * @tc.name: SrcRectRotateTransform01
 * @tc.desc: Test RsUniRenderComposerTest.SrcRectRotateTransform
 * @tc.type: FUNC
 * @tc.require: issue1I6S774
 */
HWTEST_F(RsUniRenderComposerTest, SrcRectRotateTransform01, TestSize.Level1)
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
 * @tc.name: SrcRectRotateTransform02
 * @tc.desc: Test RsUniRenderComposerTest.SrcRectRotateTransform
 * @tc.type: FUNC
 * @tc.require: issue1I6S774
 */
HWTEST_F(RsUniRenderComposerTest, SrcRectRotateTransform02, TestSize.Level1)
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
 * @tc.name: SrcRectRotateTransform03
 * @tc.desc: Test RsUniRenderComposerTest.SrcRectRotateTransform
 * @tc.type: FUNC
 * @tc.require: issue1I6S774
 */
HWTEST_F(RsUniRenderComposerTest, SrcRectRotateTransform03, TestSize.Level1)
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
 * @tc.name: SrcRectRotateTransform04
 * @tc.desc: Test RsUniRenderComposerTest.SrcRectRotateTransform
 * @tc.type: FUNC
 * @tc.require: issue1I6S774
 */
HWTEST_F(RsUniRenderComposerTest, SrcRectRotateTransform04, TestSize.Level1)
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
 * @tc.name: LayerCrop01
 * @tc.desc: Test RsUniRenderComposerTest.LayerCrop
 * @tc.type: FUNC
 * @tc.require: issue1I6S774
 */
HWTEST_F(RsUniRenderComposerTest, LayerCrop01, TestSize.Level1)
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
 * @tc.name: LayerCrop02
 * @tc.desc: Test RsUniRenderComposerTest.LayerCrop
 * @tc.type: FUNC
 * @tc.require: issue1I6S774
 */
HWTEST_F(RsUniRenderComposerTest, LayerCrop02, TestSize.Level1)
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
 * @tc.name: LayerScaleDown01
 * @tc.desc: Test RsUniRenderComposerTest.LayerScaleDown
 * @tc.type: FUNC
 * @tc.require: issue1I6S774
 */
HWTEST_F(RsUniRenderComposerTest, LayerScaleDown01, TestSize.Level1)
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
 * @tc.name: LayerScaleDown02
 * @tc.desc: Test RsUniRenderComposerTest.LayerScaleDown
 * @tc.type: FUNC
 * @tc.require: issue1I6S774
 */
HWTEST_F(RsUniRenderComposerTest, LayerScaleDown02, TestSize.Level1)
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
 * @tc.name: LayerScaleDown03
 * @tc.desc: Test RsUniRenderComposerTest.LayerScaleDown
 * @tc.type: FUNC
 * @tc.require: issue1I7FUVJ
 */
HWTEST_F(RsUniRenderComposerTest, LayerScaleDown03, TestSize.Level1)
{
    SetUp();
    RSSurfaceRenderNodeConfig config;
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);

    RectI dstRect{0, 0, 40, 60};
    surfaceNode->SetSrcRect(dstRect);
    surfaceNode->SetDstRect(dstRect);

    sptr<IConsumerSurface> csurf = IConsumerSurface::Create(config.name);
    ASSERT_NE(csurf, nullptr);
    surfaceNode->GetRSSurfaceHandler()->SetConsumer(csurf);

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
    composerAdapter_->SetComposeInfoToLayer(layer, info, surfaceNode->GetRSSurfaceHandler()->GetConsumer());

    composerAdapter_->LayerRotate(layer, *surfaceNode);
    composerAdapter_->LayerCrop(layer);
    composerAdapter_->LayerScaleDown(layer, *surfaceNode);
}

/**
 * @tc.name: LayerScaleFit01
 * @tc.desc: Test RsUniRenderComposerTest.LayerScaleFit
 * @tc.type: FUNC
 * @tc.require: issue1I6S774
 */
HWTEST_F(RsUniRenderComposerTest, LayerScaleFit01, TestSize.Level1)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);
    RectI dstRect{0, 0, DEFAULT_CANVAS_WIDTH, DEFAULT_CANVAS_HEIGHT};
    surfaceNode->SetSrcRect(dstRect);
    surfaceNode->SetDstRect(dstRect);
    auto layer = composerAdapter_->CreateLayer(*surfaceNode);
    ASSERT_NE(layer, nullptr);
    layer->sbuffer_ = nullptr;
    composerAdapter_->LayerScaleFit(layer);
}

/**
 * @tc.name: LayerScaleFit02
 * @tc.desc: Test RsUniRenderComposerTest.LayerScaleFit
 * @tc.type: FUNC
 * @tc.require: issue1I6S774
 */
HWTEST_F(RsUniRenderComposerTest, LayerScaleFit02, TestSize.Level1)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);
    RectI dstRect{0, 0, DEFAULT_CANVAS_WIDTH, DEFAULT_CANVAS_HEIGHT};
    surfaceNode->SetSrcRect(dstRect);
    surfaceNode->SetDstRect(dstRect);
    auto layer = composerAdapter_->CreateLayer(*surfaceNode);
    ASSERT_NE(layer, nullptr);
    layer->cSurface_ = nullptr;
    composerAdapter_->LayerScaleFit(layer);
}

/**
 * @tc.name: LayerScaleFit03
 * @tc.desc: Test RsUniRenderComposerTest.LayerScaleFit
 * @tc.type: FUNC
 * @tc.require: issue1I7FUVJ
 */
HWTEST_F(RsUniRenderComposerTest, LayerScaleFit03, TestSize.Level1)
{
    SetUp();
    RSSurfaceRenderNodeConfig config;
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);

    RectI dstRect{0, 0, 40, 60};
    surfaceNode->SetSrcRect(dstRect);
    surfaceNode->SetDstRect(dstRect);

    sptr<IConsumerSurface> csurf = IConsumerSurface::Create(config.name);
    ASSERT_NE(csurf, nullptr);
    surfaceNode->GetRSSurfaceHandler()->SetConsumer(csurf);

    bool statusReady = composerAdapter_->CheckStatusBeforeCreateLayer(*surfaceNode);
    ASSERT_EQ(statusReady, true);
    ComposeInfo info = composerAdapter_->BuildComposeInfo(*surfaceNode);

    LayerInfoPtr layer = HdiLayerInfo::CreateHdiLayerInfo();
    composerAdapter_->SetComposeInfoToLayer(layer, info, surfaceNode->GetRSSurfaceHandler()->GetConsumer());

    composerAdapter_->LayerRotate(layer, *surfaceNode);
    composerAdapter_->LayerCrop(layer);
    composerAdapter_->LayerScaleFit(layer);
}

/**
 * @tc.name: CheckStatusBeforeCreateLayer01
 * @tc.desc: Test RsUniRenderComposerTest.CheckStatusBeforeCreateLayer
 * @tc.type: FUNC
 * @tc.require: issue1I6S774
 */
HWTEST_F(RsUniRenderComposerTest, CheckStatusBeforeCreateLayer01, TestSize.Level1)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    surfaceNode->GetRSSurfaceHandler()->GetConsumer()->SetTransform(GraphicTransformType::GRAPHIC_ROTATE_NONE);
    ASSERT_NE(surfaceNode, nullptr);
    composerAdapter_->output_ = nullptr;
    ASSERT_EQ(false, composerAdapter_->CheckStatusBeforeCreateLayer(*surfaceNode));
}

/**
 * @tc.name: CheckStatusBeforeCreateLayer02
 * @tc.desc: Test RsUniRenderComposerTest.CheckStatusBeforeCreateLayer
 * @tc.type: FUNC
 * @tc.require: issue1I6S774
 */
HWTEST_F(RsUniRenderComposerTest, CheckStatusBeforeCreateLayer02, TestSize.Level1)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);
    const auto& buffer = surfaceNode->GetRSSurfaceHandler()->GetBuffer();
    ASSERT_EQ(buffer, nullptr);
    ASSERT_EQ(false, composerAdapter_->CheckStatusBeforeCreateLayer(*surfaceNode));
}

/**
 * @tc.name: CheckStatusBeforeCreateLayer03
 * @tc.desc: Test RsUniRenderComposerTest.CheckStatusBeforeCreateLayer
 * @tc.type: FUNC
 * @tc.require: issue1I6S774
 */
HWTEST_F(RsUniRenderComposerTest, CheckStatusBeforeCreateLayer03, TestSize.Level1)
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
 * @tc.name: CheckStatusBeforeCreateLayer04
 * @tc.desc: Test RsUniRenderComposerTest.CheckStatusBeforeCreateLayer
 * @tc.type: FUNC
 * @tc.require: issue1I6S774
 */
HWTEST_F(RsUniRenderComposerTest, CheckStatusBeforeCreateLayer04, TestSize.Level1)
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
 * @tc.name: CheckStatusBeforeCreateLayer05
 * @tc.desc: Test RsUniRenderComposerTest.CheckStatusBeforeCreateLayer
 * @tc.type: FUNC
 * @tc.require: issue1I6S774
 */
HWTEST_F(RsUniRenderComposerTest, CheckStatusBeforeCreateLayer05, TestSize.Level1)
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
 * @tc.name: CheckStatusBeforeCreateLayer06
 * @tc.desc: Test RsUniRenderComposerTest.CheckStatusBeforeCreateLayer
 * @tc.type: FUNC
 * @tc.require: issue1I6S774
 */
HWTEST_F(RsUniRenderComposerTest, CheckStatusBeforeCreateLayer06, TestSize.Level1)
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
 * @tc.name: CheckStatusBeforeCreateLayer07
 * @tc.desc: Test RsUniRenderComposerTest.CheckStatusBeforeCreateLayer
 * @tc.type: FUNC
 * @tc.require: issue1I6S774
 */
HWTEST_F(RsUniRenderComposerTest, CheckStatusBeforeCreateLayer07, TestSize.Level1)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);
    RectI dstRect{0, 0, DEFAULT_CANVAS_WIDTH, DEFAULT_CANVAS_HEIGHT};
    surfaceNode->SetSrcRect(dstRect);
    surfaceNode->SetDstRect(dstRect);
    ASSERT_EQ(true, composerAdapter_->CheckStatusBeforeCreateLayer(*surfaceNode));
}

/**
 * @tc.name: IsOutOfScreenRegion03
 * @tc.desc: Test RsUniRenderComposerTest.IsOutOfScreenRegion
 * @tc.type: FUNC
 * @tc.require: issue1I6S774
 */
HWTEST_F(RsUniRenderComposerTest, IsOutOfScreenRegion03, TestSize.Level1)
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
 * @tc.name: IsOutOfScreenRegion04
 * @tc.desc: Test RsUniRenderComposerTest.IsOutOfScreenRegion
 * @tc.type: FUNC
 * @tc.require: issue1I6S774
 */
HWTEST_F(RsUniRenderComposerTest, IsOutOfScreenRegion04, TestSize.Level1)
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
 * @tc.name: IsOutOfScreenRegion05
 * @tc.desc: Test RsUniRenderComposerTest.IsOutOfScreenRegion
 * @tc.type: FUNC
 * @tc.require: issue1I6S774
 */
HWTEST_F(RsUniRenderComposerTest, IsOutOfScreenRegion05, TestSize.Level1)
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
 * @tc.name: IsOutOfScreenRegion06
 * @tc.desc: Test RsUniRenderComposerTest.IsOutOfScreenRegion
 * @tc.type: FUNC
 * @tc.require: issue1I6S774
 */
HWTEST_F(RsUniRenderComposerTest, IsOutOfScreenRegion06, TestSize.Level1)
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
 * @tc.name: CreateBufferLayer01
 * @tc.desc: Test RsUniRenderComposerTest.CreateBufferLayer
 * @tc.type: FUNC
 * @tc.require: issue1I6S774
 */
HWTEST_F(RsUniRenderComposerTest, CreateBufferLayer01, TestSize.Level1)
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
 * @tc.name: CreateBufferLayer02
 * @tc.desc: Test RsUniRenderComposerTest.CreateBufferLayer
 * @tc.type: FUNC
 * @tc.require: issue1I6S774
 */
HWTEST_F(RsUniRenderComposerTest, CreateBufferLayer02, TestSize.Level1)
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
 * @tc.name: CreateBufferLayer03
 * @tc.desc: Test RsUniRenderComposerTest.CreateBufferLayer
 * @tc.type: FUNC
 * @tc.require: issue1I6S774
 */
HWTEST_F(RsUniRenderComposerTest, CreateBufferLayer03, TestSize.Level1)
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
 * @tc.name: CreateLayer01
 * @tc.desc: Test RsUniRenderComposerTest.CreateLayer
 * @tc.type: FUNC
 * @tc.require: issue1I6S774
 */
HWTEST_F(RsUniRenderComposerTest, CreateLayer01, TestSize.Level1)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);
    RectI dstRect{0, 0, DEFAULT_CANVAS_WIDTH, DEFAULT_CANVAS_HEIGHT};
    surfaceNode->SetSrcRect(dstRect);
    surfaceNode->SetDstRect(dstRect);
    composerAdapter_->screenInfo_.phyWidth = DEFAULT_CANVAS_WIDTH_1K;
    composerAdapter_->screenInfo_.phyHeight = DEFAULT_CANVAS_HEIGHT_1K;
    surfaceNode->GetRSSurfaceHandler()->consumer_ = nullptr;
    ASSERT_EQ(nullptr, composerAdapter_->CreateLayer(*surfaceNode));
}

/**
 * @tc.name: SetLayerSize01
 * @tc.desc: Test RsUniRenderComposerTest.SetLayerSize
 * @tc.type: FUNC
 * @tc.require: issue1I6S774
 */
HWTEST_F(RsUniRenderComposerTest, SetLayerSize01, TestSize.Level1)
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
    surfaceNode->GetRSSurfaceHandler()->consumer_ = nullptr;
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
 * @tc.name: GetComposerInfoNeedClient01
 * @tc.desc: Test RsUniRenderComposerTest.GetComposerInfoNeedClient while
 *           color gamut of screen and buffer are different
 * @tc.type: FUNC
 * @tc.require: issue1I7O6WO
 */
HWTEST_F(RsUniRenderComposerTest, GetComposerInfoNeedClient01, TestSize.Level2)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);
    ComposeInfo info = composerAdapter_->BuildComposeInfo(*surfaceNode);
    ASSERT_NE(info.buffer, nullptr);
    
    info.buffer->SetSurfaceBufferColorGamut(GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB);
    composerAdapter_->screenInfo_.colorGamut = ScreenColorGamut::COLOR_GAMUT_DISPLAY_BT2020;
    ASSERT_TRUE(composerAdapter_->GetComposerInfoNeedClient(info, *surfaceNode->GetStagingRenderParams()));
}

/**
 * @tc.name: GetComposerInfoNeedClient02
 * @tc.desc: Test RsUniRenderComposerTest.GetComposerInfoNeedClient with RSRenderNode
 * @tc.type: FUNC
 * @tc.require: issue1I7O6WO
 */
HWTEST_F(RsUniRenderComposerTest, GetComposerInfoNeedClient02, TestSize.Level2)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);
    ComposeInfo info = composerAdapter_->BuildComposeInfo(*surfaceNode);
    ASSERT_NE(info.buffer, nullptr);
    
    auto node = surfaceNode->ReinterpretCastTo<RSRenderNode>();
    info.buffer->SetSurfaceBufferColorGamut(GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB);
    composerAdapter_->screenInfo_.colorGamut = ScreenColorGamut::COLOR_GAMUT_DISPLAY_BT2020;
    ASSERT_TRUE(composerAdapter_->GetComposerInfoNeedClient(info, *node->GetStagingRenderParams()));
}

/**
 * @tc.name: LayerCrop03
 * @tc.desc: Test RsUniRenderComposerTest.LayerCrop while
 *           screen size is smaller than layer size
 * @tc.type: FUNC
 * @tc.require: issue1I7O6WO
 */
HWTEST_F(RsUniRenderComposerTest, LayerCrop03, TestSize.Level2)
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
 * @tc.name: SrcRectRotateTransform06
 * @tc.desc: Test RsUniRenderComposerTest.SrcRectRotateTransform while
 *           surface's consumer is nullptr
 * @tc.type: FUNC
 * @tc.require: issue11I7T9RE
 */
HWTEST_F(RsUniRenderComposerTest, SrcRectRotateTransform06, TestSize.Level2)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);
    RectI rect{0, 0, DEFAULT_CANVAS_WIDTH, DEFAULT_CANVAS_HEIGHT};
    surfaceNode->SetSrcRect(rect);
    surfaceNode->GetRSSurfaceHandler()->consumer_ = nullptr;
    ASSERT_EQ(composerAdapter_->SrcRectRotateTransform(*surfaceNode), rect);
}

/**
 * @tc.name: CheckStatusBeforeCreateLayer08
 * @tc.desc: Test RsUniRenderComposerTest.CheckStatusBeforeCreateLayer while
 *           bounds geometry is nullptr
 * @tc.type: FUNC
 * @tc.require: issue11I7T9RE
 */
HWTEST_F(RsUniRenderComposerTest, CheckStatusBeforeCreateLayer08, TestSize.Level2)
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
 * @tc.name: LayerScaleDown04
 * @tc.desc: Test RsUniRenderComposerTest.LayerScaleDown while
 *           scaling mode is SCALING_MODE_SCALE_CROP
 * @tc.type: FUNC
 * @tc.require: issue11I7T9RE
 */
HWTEST_F(RsUniRenderComposerTest, LayerScaleDown04, TestSize.Level2)
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
    surfaceNode->GetRSSurfaceHandler()->GetConsumer()->AttachBuffer(layer->sbuffer_);
    surfaceNode->GetRSSurfaceHandler()->GetConsumer()->SetScalingMode(layer->GetBuffer()->GetSeqNum(), scalingMode);
    
    composerAdapter_->LayerScaleDown(layer, *surfaceNode);
    ASSERT_TRUE(layer->GetDirtyRegions()[0].w == DEFAULT_CANVAS_WIDTH);
}

/**
 * @tc.name: SrcRectRotateTransform05
 * @tc.desc: Test RsUniRenderComposerTest.SrcRectRotateTransform for GRAPHIC_ROTATE_90
 * @tc.type: FUNC
 * @tc.require: issue1I7O6WO
 */
HWTEST_F(RsUniRenderComposerTest, SrcRectRotateTransform05, TestSize.Level2)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);
    // Let the left value of node's rect not equal to zero and the result of the founction isn't empty
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
 * @tc.name: LayerScaleDown05
 * @tc.desc: Test RsUniRenderComposerTest.LayerScaleDown while
 *           surface rotate angle is not a multiple of FLAT_ANGLE
 * @tc.type: FUNC
 * @tc.require: issue11I7T9RE
 */
HWTEST_F(RsUniRenderComposerTest, LayerScaleDown05, TestSize.Level2)
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
    surfaceNode->GetRSSurfaceHandler()->GetConsumer()->AttachBuffer(layer->sbuffer_);
    surfaceNode->GetRSSurfaceHandler()->GetConsumer()->SetScalingMode(layer->GetBuffer()->GetSeqNum(), scalingMode);

    surfaceNode->GetRSSurfaceHandler()->GetConsumer()->SetTransform(GraphicTransformType::GRAPHIC_ROTATE_90);
    surfaceNode->GetRSSurfaceHandler()->GetBuffer()->SetSurfaceBufferTransform(GraphicTransformType::GRAPHIC_ROTATE_90);
    composerAdapter_->LayerScaleDown(layer, *surfaceNode);
    ASSERT_FALSE(layer->GetDirtyRegions()[0].w == DEFAULT_CANVAS_WIDTH);
}

/**
 * @tc.name: LayerScaleDown06
 * @tc.desc: Test RsUniRenderComposerTest.LayerScaleDown while
 *           surface's width is smaller than surface's heigth
 * @tc.type: FUNC
 * @tc.require: issue11I7T9RE
 */
HWTEST_F(RsUniRenderComposerTest, LayerScaleDown06, TestSize.Level2)
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
    surfaceNode->GetRSSurfaceHandler()->GetConsumer()->AttachBuffer(layer->sbuffer_);
    surfaceNode->GetRSSurfaceHandler()->GetConsumer()->SetScalingMode(layer->GetBuffer()->GetSeqNum(), scalingMode);

    surfaceNode->GetRSSurfaceHandler()->GetConsumer()->SetTransform(GraphicTransformType::GRAPHIC_ROTATE_90);
    composerAdapter_->LayerScaleDown(layer, *surfaceNode);
    ASSERT_FALSE(layer->GetDirtyRegions()[0].h == DEFAULT_CANVAS_WIDTH);
}

/**
 * @tc.name: SetBufferColorSpace01
 * @tc.desc: Test RsUniRenderComposerTest.SetBufferColorSpace
 * @tc.type: FUNC
 * @tc.require: issue11I8C4I9
*/
HWTEST_F(RsUniRenderComposerTest, SetBufferColorSpace01, TestSize.Level2)
{
    SetUp();

    using namespace HDI::Display::Graphic::Common::V1_0;

    RSDisplayNodeConfig config;
    RSDisplayRenderNode::SharedPtr nodePtr = std::make_shared<RSDisplayRenderNode>(1, config);
    auto displayDrawable =std::static_pointer_cast<DrawableV2::RSDisplayRenderNodeDrawable>(
        DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(nodePtr));
    auto surfaceHandler = displayDrawable->GetRSSurfaceHandlerOnDraw();
    sptr<IBufferConsumerListener> listener = new RSUniRenderListener(surfaceHandler);
    displayDrawable->CreateSurface(listener);

    auto rsSurface = displayDrawable->GetRSSurface();
    rsSurface->SetColorSpace(GRAPHIC_COLOR_GAMUT_DISPLAY_P3);

    sptr<SurfaceBuffer> buffer = new SurfaceBufferImpl();
    BufferRequestConfig requestConfig = {
        .width = 0x10,
        .height = 0x10,
        .strideAlignment = 0x8,
        .format = GRAPHIC_PIXEL_FMT_RGBA_8888,
        .usage = BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA,
        .timeout = 0,
        .colorGamut = GraphicColorGamut::GRAPHIC_COLOR_GAMUT_DISPLAY_P3,
    };
    GSError ret = buffer->Alloc(requestConfig);
    ASSERT_EQ(ret, GSERROR_OK);

    surfaceHandler->SetBuffer(buffer, SyncFence::INVALID_FENCE, Rect(), 0);

    RSUniRenderComposerAdapter::SetBufferColorSpace(*displayDrawable);

    CM_ColorSpaceType colorSpaceType;
    ret = MetadataHelper::GetColorSpaceType(buffer, colorSpaceType);
    ASSERT_TRUE(ret == GSERROR_OK || GSErrorStr(ret) == "<50 api call failed>with low error <Not supported>");
    if (ret == GSERROR_OK) {
        ASSERT_EQ(colorSpaceType, CM_P3_FULL);
    }

    CM_ColorSpaceInfo colorSpaceInfo;
    ret = MetadataHelper::GetColorSpaceInfo(buffer, colorSpaceInfo);
    ASSERT_TRUE(ret == GSERROR_OK || GSErrorStr(ret) == "<50 api call failed>with low error <Not supported>");
    if (ret == GSERROR_OK) {
        ASSERT_EQ(colorSpaceInfo.primaries, COLORPRIMARIES_P3_D65);
        ASSERT_EQ(colorSpaceInfo.transfunc, TRANSFUNC_SRGB);
        ASSERT_EQ(colorSpaceInfo.matrix, MATRIX_P3);
        ASSERT_EQ(colorSpaceInfo.range, RANGE_FULL);
    }
}

/**
 * @tc.name: SetBufferColorSpace02
 * @tc.desc: Test RsUniRenderComposerTest.SetBufferColorSpace
 * @tc.type: FUNC
 * @tc.require: issue11I8C4I9
*/
HWTEST_F(RsUniRenderComposerTest, SetBufferColorSpace02, TestSize.Level2)
{
    SetUp();

    NodeId id = 0;
    RSDisplayNodeConfig config;
    auto node = std::make_shared<RSDisplayRenderNode>(id, config);
    auto drawable = DrawableV2::RSDisplayRenderNodeDrawable::OnGenerate(node);
    ASSERT_NE(drawable, nullptr);
    auto displayDrawable = static_cast<DrawableV2::RSDisplayRenderNodeDrawable*>(drawable);
    composerAdapter_->SetBufferColorSpace(*displayDrawable);
}

/**
 * @tc.name: SetBufferColorSpace03
 * @tc.desc: Test RsUniRenderComposerTest.SetBufferColorSpace
 * @tc.type: FUNC
 * @tc.require: issue11I8C4I9
*/
HWTEST_F(RsUniRenderComposerTest, SetBufferColorSpace03, TestSize.Level2)
{
    SetUp();

    NodeId id = 1;
    RSDisplayNodeConfig config;
    auto node = std::make_shared<RSDisplayRenderNode>(id, config);
    auto displayDrawable = std::static_pointer_cast<DrawableV2::RSDisplayRenderNodeDrawable>(
        DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(node));
    ASSERT_NE(displayDrawable, nullptr);
    composerAdapter_->SetBufferColorSpace(*displayDrawable);
    sptr<SyncFence> acquireFence = SyncFence::INVALID_FENCE;
    int64_t timestamp = 0;
    Rect damage;
    sptr<OHOS::SurfaceBuffer> buffer = new SurfaceBufferImpl(0);
    auto surfaceHandler = displayDrawable->GetRSSurfaceHandlerOnDraw();
    surfaceHandler->SetBuffer(buffer, acquireFence, damage, timestamp);
    composerAdapter_->SetBufferColorSpace(*displayDrawable);
}
} // namespace