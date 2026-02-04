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
#include "pipeline/rs_test_util.h"
#include "pipeline/render_thread/rs_uni_render_composer_adapter.h"
#include "pipeline/main_thread/rs_uni_render_listener.h"
#include "pipeline/rs_screen_render_node.h"
#include "surface_buffer_impl.h"
#include "metadata_helper.h"
#include "screen_manager/rs_screen.h"
#include "rs_surface_layer.h"

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
    static inline uint32_t screenId_ = 0;
    static inline int32_t offsetX = 0; // screenOffset on x axis equals to 0
    static inline int32_t offsetY = 0; // screenOffset on y axis equals to 0
    static inline float mirrorAdaptiveCoefficient = 1.0f;

    static inline sptr<RSScreenManager> screenManager_;
    static inline std::shared_ptr<RSUniRenderComposerAdapter> composerAdapter_;
    static inline std::shared_ptr<HdiOutput> output_;
    static inline ScreenInfo info = {};
};

void RSUniRenderComposerAdapterTest::SetUpTestCase()
{
#ifdef RS_ENABLE_VK
    RsVulkanContext::SetRecyclable(false);
#endif
    RSTestUtil::InitRenderNodeGC();

    output_ = HdiOutput::CreateHdiOutput(screenId_);
    auto screen = std::make_shared<RSScreen>(output_);
    screenManager_ = CreateOrGetScreenManager();
    screenManager_->MockHdiScreenConnected(screen);

    composerAdapter_ = std::make_unique<RSUniRenderComposerAdapter>();
    ASSERT_NE(composerAdapter_, nullptr);
}

void RSUniRenderComposerAdapterTest::TearDownTestCase()
{
    RSRenderComposerManager::GetInstance().rsRenderComposerMap_[screenId_]->uniRenderEngine_ = nullptr;
}

void RSUniRenderComposerAdapterTest::TearDown() {}

void RSUniRenderComposerAdapterTest::SetUp() {}

// must be first UTTest
HWTEST_F(RSUniRenderComposerAdapterTest, InitTest, TestSize.Level1)
{
    uint32_t width = 2560;
    uint32_t height = 1080;
    ScreenColorGamut colorGamut = ScreenColorGamut::COLOR_GAMUT_SRGB;
    ScreenState state = ScreenState::UNKNOWN;
    ScreenRotation rotation = ScreenRotation::ROTATION_0;
    info = screenManager_->QueryScreenInfo(screenId_);
    composerAdapter_->Init(info, offsetX, offsetY);
    EXPECT_EQ(composerAdapter_->composerClient_, nullptr);
    info.width = width;
    info.height = height;
    info.phyWidth = width;
    info.phyHeight = height;
    info.colorGamut = colorGamut;
    info.state = state;
    info.rotation = rotation;
    RSRenderComposerManager::GetInstance().OnScreenConnected(output_);
    composerAdapter_->Init(info, offsetX, offsetY);
    EXPECT_NE(composerAdapter_->composerClient_, nullptr);
    EXPECT_NE(composerAdapter_->output_, nullptr);
}

// ==================== CheckStatusBeforeCreateLayer ====================

/**
 * @tc.name: CheckStatusBeforeCreateLayer001
 * @tc.desc: Test RSUniRenderComposerAdapterTest.CheckStatusBeforeCreateLayer
 * @tc.type: FUNC
 * @tc.require: issueI6S774
 */
HWTEST_F(RSUniRenderComposerAdapterTest, CheckStatusBeforeCreateLayer001, TestSize.Level1)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);
    const auto& buffer = surfaceNode->GetRSSurfaceHandler()->GetBuffer();
    ASSERT_EQ(buffer, nullptr);
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
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);
    RectI dstRect{0, 0, DEFAULT_CANVAS_WIDTH, DEFAULT_CANVAS_HEIGHT};
    surfaceNode->SetSrcRect(dstRect);
    surfaceNode->SetDstRect(dstRect);
    surfaceNode->dstRect_.width_ = 0;
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
    surfaceNode->dstRect_.height_ = 0;
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
    surfaceNode->srcRect_.width_ = 0;
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
    surfaceNode->srcRect_.height_ = 0;
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
    ASSERT_EQ(true, composerAdapter_->CheckStatusBeforeCreateLayer(*surfaceNode));
}

/**
 * @tc.name: CheckStatusBeforeCreateLayer007
 * @tc.desc: Test RSUniRenderComposerAdapterTest.CheckStatusBeforeCreateLayer for drawable with null output
 * @tc.type: FUNC
 * @tc.require: issuesI7T9RE
 */
HWTEST_F(RSUniRenderComposerAdapterTest, CheckStatusBeforeCreateLayer007, TestSize.Level2)
{
    auto adapter = std::make_unique<RSUniRenderComposerAdapter>();
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);

    auto drawable = std::static_pointer_cast<DrawableV2::RSSurfaceRenderNodeDrawable>(
        DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(surfaceNode));
    ASSERT_NE(drawable, nullptr);

    // output_ is nullptr
    ASSERT_EQ(false, adapter->CheckStatusBeforeCreateLayer(*drawable));
}

/**
 * @tc.name: CheckStatusBeforeCreateLayer008
 * @tc.desc: Test RSUniRenderComposerAdapterTest.CheckStatusBeforeCreateLayer for drawable with null params
 * @tc.type: FUNC
 * @tc.require: issuesI7T9RE
 */
HWTEST_F(RSUniRenderComposerAdapterTest, CheckStatusBeforeCreateLayer008, TestSize.Level2)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);

    auto drawable = std::static_pointer_cast<DrawableV2::RSSurfaceRenderNodeDrawable>(
        DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(surfaceNode));
    ASSERT_NE(drawable, nullptr);

    // Directly set private renderParams_ to nullptr to test null params branch
    drawable->renderParams_ = nullptr;

    // params is nullptr, should return false
    ASSERT_EQ(false, composerAdapter_->CheckStatusBeforeCreateLayer(*drawable));
}

/**
 * @tc.name: CheckStatusBeforeCreateLayer009
 * @tc.desc: Test RSUniRenderComposerAdapterTest.CheckStatusBeforeCreateLayer for drawable with null buffer
 * @tc.type: FUNC
 * @tc.require: issuesI7T9RE
 */
HWTEST_F(RSUniRenderComposerAdapterTest, CheckStatusBeforeCreateLayer009, TestSize.Level2)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);

    auto drawable = std::static_pointer_cast<DrawableV2::RSSurfaceRenderNodeDrawable>(
        DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(surfaceNode));
    ASSERT_NE(drawable, nullptr);

    // buffer is nullptr (OnGenerate creates params but no buffer)
    ASSERT_EQ(false, composerAdapter_->CheckStatusBeforeCreateLayer(*drawable));
}

// ==================== CheckStatusBeforeCreateLayerFailed ====================

/**
 * @tc.name: CheckStatusBeforeCreateLayerFailed001
 * @tc.desc: Test RSUniRenderComposerAdapterTest.CheckStatusBeforeCreateLayer with null output
 * @tc.type: FUNC
 * @tc.require: issuesI7T9RE
 */
HWTEST_F(RSUniRenderComposerAdapterTest, CheckStatusBeforeCreateLayerFailed001, TestSize.Level2)
{
    auto adapter = std::make_unique<RSUniRenderComposerAdapter>();
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);
    RectI dstRect{0, 0, DEFAULT_CANVAS_WIDTH, DEFAULT_CANVAS_HEIGHT};
    surfaceNode->SetSrcRect(dstRect);
    surfaceNode->SetDstRect(dstRect);
    // output_ is nullptr
    ASSERT_EQ(false, adapter->CheckStatusBeforeCreateLayer(*surfaceNode));
}

/**
 * @tc.name: CheckStatusBeforeCreateLayerFailed002
 * @tc.desc: Test RSUniRenderComposerAdapterTest.CheckStatusBeforeCreateLayer with null surfaceHandler
 * @tc.type: FUNC
 * @tc.require: issuesI7T9RE
 */
HWTEST_F(RSUniRenderComposerAdapterTest, CheckStatusBeforeCreateLayerFailed002, TestSize.Level2)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);
    // surfaceHandler is nullptr
    ASSERT_EQ(false, composerAdapter_->CheckStatusBeforeCreateLayer(*surfaceNode));
}

// ==================== CommitLayersFailed ====================

/**
 * @tc.name: CommitLayersFailed001
 * @tc.desc: Test RSUniRenderComposerAdapterTest.CommitLayers with null backend
 * @tc.type: FUNC
 * @tc.require: issuesI7T9RE
 */
HWTEST_F(RSUniRenderComposerAdapterTest, CommitLayersFailed001, TestSize.Level2)
{
    auto adapter = std::make_unique<RSUniRenderComposerAdapter>();
    // hdiBackend_ is nullptr, should return without crash
    adapter->CommitLayers();
    ASSERT_EQ(adapter->hdiBackend_, nullptr);
}

/**
 * @tc.name: CommitLayersFailed002
 * @tc.desc: Test RSUniRenderComposerAdapterTest.CommitLayers with null output
 * @tc.type: FUNC
 * @tc.require: issuesI7T9RE
 */
HWTEST_F(RSUniRenderComposerAdapterTest, CommitLayersFailed002, TestSize.Level2)
{
    auto adapter = std::make_unique<RSUniRenderComposerAdapter>();
    // Set hdiBackend_ but not output_
    adapter->hdiBackend_ = HdiBackend::GetInstance();
    // Should return early when output_ is nullptr
    adapter->CommitLayers();
    // Verify output_ is still nullptr (no state change)
    ASSERT_EQ(adapter->output_, nullptr);
}

// ==================== CreateLayer ====================

/**
 * @tc.name: CreateLayer001
 * @tc.desc: Test RSUniRenderComposerAdapterTest.CreateLayer
 * @tc.type: FUNC
 */
HWTEST_F(RSUniRenderComposerAdapterTest, CreateLayer001, TestSize.Level2)
{
    NodeId id = 1;
    ScreenId screenId = 0;
    std::shared_ptr<RSContext> context = std::make_shared<RSContext>();
    auto rsScreenNode = std::make_shared<RSScreenRenderNode>(id, screenId, context->weak_from_this());
    RSLayerPtr layer = composerAdapter_->CreateLayer(*rsScreenNode);
    ASSERT_EQ(layer, nullptr);

    auto screenDrawable = std::static_pointer_cast<DrawableV2::RSScreenRenderNodeDrawable>(
        DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(rsScreenNode));
    ASSERT_NE(screenDrawable, nullptr);
    screenDrawable->surfaceCreated_ = true;
    OHOS::sptr<SurfaceBuffer> cbuffer = new SurfaceBufferImpl();
    sptr<SyncFence> acquireFence = SyncFence::INVALID_FENCE;
    auto surfaceHandler = screenDrawable->GetMutableRSSurfaceHandlerOnDraw();
    ASSERT_NE(surfaceHandler, nullptr);
    surfaceHandler->SetBuffer(cbuffer, acquireFence, {}, 0);
    surfaceHandler->SetAvailableBufferCount(0);
    composerAdapter_->composerClient_ = nullptr;
    layer = composerAdapter_->CreateLayer(*rsScreenNode);
    ASSERT_EQ(layer, nullptr);
    composerAdapter_->Init(info, offsetX, offsetY);
    layer = composerAdapter_->CreateLayer(*rsScreenNode);
    ASSERT_NE(layer, nullptr);

    composerAdapter_->composerClient_ = nullptr;
    layer = composerAdapter_->CreateLayer(*screenDrawable);
    ASSERT_EQ(layer, nullptr);
    composerAdapter_->Init(info, offsetX, offsetY);
    layer = composerAdapter_->CreateLayer(*screenDrawable);
    ASSERT_NE(layer, nullptr);
}

/**
 * @tc.name: CreateLayer002
 * @tc.desc: Test RSUniRenderComposerAdapterTest.CreateLayer
 * @tc.type: FUNC
 */
HWTEST_F(RSUniRenderComposerAdapterTest, CreateLayer002, TestSize.Level2)
{
    auto rcdSurfaceRenderNode = RSRcdSurfaceRenderNode::Create(1, RCDSurfaceType::TOP);
    sptr<SurfaceBuffer> buffer = OHOS::SurfaceBuffer::Create();
    sptr<SyncFence> acquireFence = SyncFence::InvalidFence();
    int64_t timestamp = 0;
    Rect damage;
    rcdSurfaceRenderNode->SetBuffer(buffer, acquireFence, damage, timestamp);
    composerAdapter_->composerClient_ = nullptr;
    RSLayerPtr layer = composerAdapter_->CreateLayer(*rcdSurfaceRenderNode);
    ASSERT_EQ(layer, nullptr);
    composerAdapter_->Init(info, offsetX, offsetY);
    layer = composerAdapter_->CreateLayer(*rcdSurfaceRenderNode);
    ASSERT_NE(layer, nullptr);
}

/**
 * @tc.name: CreateLayer003
 * @tc.desc: Test RSUniRenderComposerAdapterTest.CreateLayer with null output
 * @tc.type: FUNC
 * @tc.require: issuesI7T9RE
 */
HWTEST_F(RSUniRenderComposerAdapterTest, CreateLayer003, TestSize.Level2)
{
    auto adapter = std::make_unique<RSUniRenderComposerAdapter>();
    NodeId id = 1;
    ScreenId screenId = 0;
    std::shared_ptr<RSContext> context = std::make_shared<RSContext>();
    auto rsScreenNode = std::make_shared<RSScreenRenderNode>(id, screenId, context->weak_from_this());
    // output_ is nullptr, should return nullptr
    RSLayerPtr layer = adapter->CreateLayer(*rsScreenNode);
    ASSERT_EQ(layer, nullptr);
}

/**
 * @tc.name: CreateLayer004
 * @tc.desc: Test RSUniRenderComposerAdapterTest.CreateLayer for RcdSurfaceRenderNode with null output
 * @tc.type: FUNC
 * @tc.require: issuesI7T9RE
 */
HWTEST_F(RSUniRenderComposerAdapterTest, CreateLayer004, TestSize.Level2)
{
    auto adapter = std::make_unique<RSUniRenderComposerAdapter>();
    auto rcdSurfaceRenderNode = RSRcdSurfaceRenderNode::Create(1, RCDSurfaceType::TOP);
    // output_ is nullptr, should return nullptr
    RSLayerPtr layer = adapter->CreateLayer(*rcdSurfaceRenderNode);
    ASSERT_EQ(layer, nullptr);
}

/**
 * @tc.name: CreateLayer005
 * @tc.desc: Test RSUniRenderComposerAdapterTest.CreateLayer for RcdSurfaceRenderNode with null buffer
 * @tc.type: FUNC
 * @tc.require: issuesI7T9RE
 */
HWTEST_F(RSUniRenderComposerAdapterTest, CreateLayer005, TestSize.Level2)
{
    auto rcdSurfaceRenderNode = RSRcdSurfaceRenderNode::Create(1, RCDSurfaceType::TOP);
    ASSERT_NE(rcdSurfaceRenderNode, nullptr);
    // buffer is nullptr, should return nullptr
    RSLayerPtr layer = composerAdapter_->CreateLayer(*rcdSurfaceRenderNode);
    ASSERT_EQ(layer, nullptr);
}

/**
 * @tc.name: CreateLayer006
 * @tc.desc: Test RSUniRenderComposerAdapterTest.CreateLayer for drawable with null output
 * @tc.type: FUNC
 * @tc.require: issuesI7T9RE
 */
HWTEST_F(RSUniRenderComposerAdapterTest, CreateLayer006, TestSize.Level2)
{
    auto adapter = std::make_unique<RSUniRenderComposerAdapter>();
    NodeId id = 1;
    ScreenId screenId = 0;
    auto context = std::make_shared<RSContext>();
    auto rsScreenNode = std::make_shared<RSScreenRenderNode>(id, screenId, context->weak_from_this());
    auto screenDrawable = std::static_pointer_cast<DrawableV2::RSScreenRenderNodeDrawable>(
        DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(rsScreenNode));
    ASSERT_NE(screenDrawable, nullptr);

    // output_ is nullptr - should return nullptr
    RSLayerPtr layer = adapter->CreateLayer(*screenDrawable);
    ASSERT_EQ(layer, nullptr);
}

/**
 * @tc.name: CreateLayer007
 * @tc.desc: Test RSUniRenderComposerAdapterTest.CreateLayer for drawable with null surfaceHandler
 * @tc.type: FUNC
 * @tc.require: issuesI7T9RE
 */
HWTEST_F(RSUniRenderComposerAdapterTest, CreateLayer007, TestSize.Level2)
{
    NodeId id = 1;
    ScreenId screenId = 0;
    auto context = std::make_shared<RSContext>();
    auto rsScreenNode = std::make_shared<RSScreenRenderNode>(id, screenId, context->weak_from_this());
    auto screenDrawable = std::static_pointer_cast<DrawableV2::RSScreenRenderNodeDrawable>(
        DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(rsScreenNode));
    ASSERT_NE(screenDrawable, nullptr);

    composerAdapter_->Init(info, offsetX, offsetY);
    // surfaceHandler is nullptr - should return nullptr
    RSLayerPtr layer = composerAdapter_->CreateLayer(*screenDrawable);
    ASSERT_EQ(layer, nullptr);
}

// ==================== DealWithNodeGravity ====================

/**
 * @tc.name: DealWithNodeGravity001
 * @tc.desc: Test RSUniRenderComposerAdapterTest.DealWithNodeGravity with RESIZE gravity
 * @tc.type: FUNC
 * @tc.require: issuesI7T9RE
 */
HWTEST_F(RSUniRenderComposerAdapterTest, DealWithNodeGravity001, TestSize.Level2)
{
    NodeId id = 1;
    ScreenId screenId = 0;
    auto context = std::make_shared<RSContext>();
    auto rsScreenNode = std::make_shared<RSScreenRenderNode>(id, screenId, context->weak_from_this());
    auto screenDrawable = std::static_pointer_cast<DrawableV2::RSScreenRenderNodeDrawable>(
        DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(rsScreenNode));
    ASSERT_NE(screenDrawable, nullptr);

    sptr<SyncFence> acquireFence = SyncFence::INVALID_FENCE;
    auto buffer = new SurfaceBufferImpl(0);
    auto surfaceHandler = screenDrawable->GetMutableRSSurfaceHandlerOnDraw();
    ASSERT_NE(surfaceHandler, nullptr);
    surfaceHandler->SetBuffer(buffer, acquireFence, {}, 0);

    composerAdapter_->Init(info, offsetX, offsetY);
    ComposeInfo composeInfo = composerAdapter_->BuildComposeInfo(*screenDrawable, screenDrawable->GetDirtyRects());

    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);
    RectI rect{0, 0, DEFAULT_CANVAS_WIDTH, DEFAULT_CANVAS_HEIGHT};
    surfaceNode->SetSrcRect(rect);
    surfaceNode->SetDstRect(rect);

    // Set gravity to RESIZE - should return early after setting gravity
    surfaceNode->GetMutableRenderProperties().SetFrameGravity(Gravity::RESIZE);

    composerAdapter_->DealWithNodeGravity(*surfaceNode, composeInfo);
    // Verify gravity was set even though function returns early
    ASSERT_EQ(composeInfo.gravity, static_cast<int32_t>(Gravity::RESIZE));

    if (buffer) {
        delete buffer;
        buffer = nullptr;
    }
}

/**
 * @tc.name: DealWithNodeGravity002
 * @tc.desc: Test RSUniRenderComposerAdapterTest.DealWithNodeGravity with TOP_LEFT gravity
 * @tc.type: FUNC
 * @tc.require: issuesI7T9RE
 */
HWTEST_F(RSUniRenderComposerAdapterTest, DealWithNodeGravity002, TestSize.Level2)
{
    NodeId id = 1;
    ScreenId screenId = 0;
    auto context = std::make_shared<RSContext>();
    auto rsScreenNode = std::make_shared<RSScreenRenderNode>(id, screenId, context->weak_from_this());
    auto screenDrawable = std::static_pointer_cast<DrawableV2::RSScreenRenderNodeDrawable>(
        DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(rsScreenNode));
    ASSERT_NE(screenDrawable, nullptr);

    sptr<SyncFence> acquireFence = SyncFence::INVALID_FENCE;
    auto buffer = new SurfaceBufferImpl(0);
    auto surfaceHandler = screenDrawable->GetMutableRSSurfaceHandlerOnDraw();
    ASSERT_NE(surfaceHandler, nullptr);
    surfaceHandler->SetBuffer(buffer, acquireFence, {}, 0);

    composerAdapter_->Init(info, offsetX, offsetY);
    ComposeInfo composeInfo = composerAdapter_->BuildComposeInfo(*screenDrawable, screenDrawable->GetDirtyRects());

    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);
    RectI rect{0, 0, DEFAULT_CANVAS_WIDTH, DEFAULT_CANVAS_HEIGHT};
    surfaceNode->SetSrcRect(rect);
    surfaceNode->SetDstRect(rect);

    // Set gravity to TOP_LEFT - should return early after setting gravity
    surfaceNode->GetMutableRenderProperties().SetFrameGravity(Gravity::TOP_LEFT);

    composerAdapter_->DealWithNodeGravity(*surfaceNode, composeInfo);
    // Verify gravity was set even though function returns early
    ASSERT_EQ(composeInfo.gravity, static_cast<int32_t>(Gravity::TOP_LEFT));

    if (buffer) {
        delete buffer;
        buffer = nullptr;
    }
}

/**
 * @tc.name: DealWithNodeGravity003
 * @tc.desc: Test RSUniRenderComposerAdapterTest.DealWithNodeGravity for drawable with RESIZE gravity
 * @tc.type: FUNC
 * @tc.require: issuesI7T9RE
 */
HWTEST_F(RSUniRenderComposerAdapterTest, DealWithNodeGravity003, TestSize.Level2)
{
    NodeId id = 1;
    ScreenId screenId = 0;
    auto context = std::make_shared<RSContext>();
    auto rsScreenNode = std::make_shared<RSScreenRenderNode>(id, screenId, context->weak_from_this());
    auto screenDrawable = std::static_pointer_cast<DrawableV2::RSScreenRenderNodeDrawable>(
        DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(rsScreenNode));
    ASSERT_NE(screenDrawable, nullptr);

    sptr<SyncFence> acquireFence = SyncFence::INVALID_FENCE;
    auto buffer = new SurfaceBufferImpl(0);
    auto surfaceHandler = screenDrawable->GetMutableRSSurfaceHandlerOnDraw();
    ASSERT_NE(surfaceHandler, nullptr);
    surfaceHandler->SetBuffer(buffer, acquireFence, {}, 0);

    composerAdapter_->Init(info, offsetX, offsetY);
    ComposeInfo composeInfo = composerAdapter_->BuildComposeInfo(*screenDrawable, screenDrawable->GetDirtyRects());

    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);
    RectI rect{0, 0, DEFAULT_CANVAS_WIDTH, DEFAULT_CANVAS_HEIGHT};
    surfaceNode->SetSrcRect(rect);
    surfaceNode->SetDstRect(rect);

    auto drawable = std::static_pointer_cast<DrawableV2::RSSurfaceRenderNodeDrawable>(
        DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(surfaceNode));
    ASSERT_NE(drawable, nullptr);

    // Set gravity to RESIZE - should return early after setting gravity
    drawable->GetRenderParams()->SetFrameGravity(Gravity::RESIZE);

    composerAdapter_->DealWithNodeGravity(*drawable, composeInfo);
    // Verify gravity was set even though function returns early
    ASSERT_EQ(composeInfo.gravity, static_cast<int32_t>(Gravity::RESIZE));

    if (buffer) {
        delete buffer;
        buffer = nullptr;
    }
}

/**
 * @tc.name: DealWithNodeGravity004
 * @tc.desc: Test RSUniRenderComposerAdapterTest.DealWithNodeGravity for drawable with null params
 * @tc.type: FUNC
 * @tc.require: issuesI7T9RE
 */
HWTEST_F(RSUniRenderComposerAdapterTest, DealWithNodeGravity004, TestSize.Level2)
{
    NodeId id = 1;
    ScreenId screenId = 0;
    auto context = std::make_shared<RSContext>();
    auto rsScreenNode = std::make_shared<RSScreenRenderNode>(id, screenId, context->weak_from_this());
    auto screenDrawable = std::static_pointer_cast<DrawableV2::RSScreenRenderNodeDrawable>(
        DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(rsScreenNode));
    ASSERT_NE(screenDrawable, nullptr);

    sptr<SyncFence> acquireFence = SyncFence::INVALID_FENCE;
    auto buffer = new SurfaceBufferImpl(0);
    auto surfaceHandler = screenDrawable->GetMutableRSSurfaceHandlerOnDraw();
    ASSERT_NE(surfaceHandler, nullptr);
    surfaceHandler->SetBuffer(buffer, acquireFence, {}, 0);

    composerAdapter_->Init(info, offsetX, offsetY);
    ComposeInfo composeInfo = composerAdapter_->BuildComposeInfo(*screenDrawable, screenDrawable->GetDirtyRects());

    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);

    auto drawable = std::static_pointer_cast<DrawableV2::RSSurfaceRenderNodeDrawable>(
        DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(surfaceNode));
    ASSERT_NE(drawable, nullptr);

    // params is nullptr, function should return early without crash
    int32_t originalGravity = composeInfo.gravity;  // Save original gravity value
    composerAdapter_->DealWithNodeGravity(*drawable, composeInfo);
    // Verify function completed without modifying composeInfo (gravity should remain unchanged)
    ASSERT_EQ(composeInfo.gravity, originalGravity);

    if (buffer) {
        delete buffer;
        buffer = nullptr;
    }
}

// ==================== GetComposerInfoNeedClient ====================

/**
 * @tc.name: GetComposerInfoNeedClient001
 * @tc.desc: Test RSUniRenderComposerAdapterTest.GetComposerInfoNeedClient with color gamut mismatch
 * @tc.type: FUNC
 * @tc.require: issuesI7T9RE
 */
HWTEST_F(RSUniRenderComposerAdapterTest, GetComposerInfoNeedClient001, TestSize.Level2)
{
    NodeId id = 1;
    ScreenId screenId = 0;
    auto context = std::make_shared<RSContext>();
    auto rsScreenNode = std::make_shared<RSScreenRenderNode>(id, screenId, context->weak_from_this());
    auto screenDrawable = std::static_pointer_cast<DrawableV2::RSScreenRenderNodeDrawable>(
        DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(rsScreenNode));
    ASSERT_NE(screenDrawable, nullptr);

    sptr<SyncFence> acquireFence = SyncFence::INVALID_FENCE;
    auto buffer = new SurfaceBufferImpl(0);
    auto surfaceHandler = screenDrawable->GetMutableRSSurfaceHandlerOnDraw();
    ASSERT_NE(surfaceHandler, nullptr);
    surfaceHandler->SetBuffer(buffer, acquireFence, {}, 0);

    composerAdapter_->Init(info, offsetX, offsetY);
    ComposeInfo composeInfo = composerAdapter_->BuildComposeInfo(*screenDrawable, screenDrawable->GetDirtyRects());

    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);

    // Set buffer color gamut different from screen info to trigger needClient = true
    if (composeInfo.buffer) {
        composeInfo.buffer->SetSurfaceBufferColorGamut(GraphicColorGamut::GRAPHIC_COLOR_GAMUT_DISPLAY_P3);
    }

    auto params = static_cast<RSSurfaceRenderParams*>(surfaceNode->GetRenderParams().get());
    if (params) {
        // Set needClient to false initially
        params->SetNeedClient(false);
        bool needClient = composerAdapter_->GetComposerInfoNeedClient(composeInfo, *params);
        // Should return true when color gamut doesn't match
        ASSERT_EQ(needClient, true);
    }

    if (buffer) {
        delete buffer;
        buffer = nullptr;
    }
}

// ==================== GetComposerInfoSrcRect ====================

/**
 * @tc.name: GetComposerInfoSrcRect001
 * @tc.desc: Test RSUniRenderComposerAdapterTest.GetComposerInfoSrcRect
 * @tc.type: FUNC
 * @tc.require: issues20101
*/
HWTEST_F(RSUniRenderComposerAdapterTest, GetComposerInfoSrcRect001, TestSize.Level2)
{
    NodeId id = 1;
    ScreenId screenId = 0;
    auto context = std::make_shared<RSContext>();
    auto rsScreenNode = std::make_shared<RSScreenRenderNode>(id, screenId, context->weak_from_this());
    auto screenDrawable = std::static_pointer_cast<DrawableV2::RSScreenRenderNodeDrawable>(
        DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(rsScreenNode));
    ASSERT_NE(screenDrawable, nullptr);
    sptr<SyncFence> acquireFence = SyncFence::INVALID_FENCE;
    auto surfaceHandler = screenDrawable->GetMutableRSSurfaceHandlerOnDraw();
    ASSERT_NE(surfaceHandler, nullptr);
    auto buffer = new SurfaceBufferImpl(0);
    surfaceHandler->SetBuffer(buffer, acquireFence, {}, 0);
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    RectI rect{0, 0, DEFAULT_CANVAS_WIDTH, DEFAULT_CANVAS_HEIGHT};
    surfaceNode->SetSrcRect(rect);
    // Initialize composerAdapter_ before calling BuildComposeInfo
    composerAdapter_->Init(info, offsetX, offsetY);
    ComposeInfo composeInfo = composerAdapter_->BuildComposeInfo(*screenDrawable, screenDrawable->GetDirtyRects());
    // This is a void function that modifies composeInfo.srcRect
    composerAdapter_->GetComposerInfoSrcRect(composeInfo, *surfaceNode);
    // Verify function executed successfully (srcRect should be in valid bounds after function call)
    ASSERT_GE(composeInfo.srcRect.w, 0);
    ASSERT_GE(composeInfo.srcRect.h, 0);
    if (buffer) {
        delete buffer;
        buffer = nullptr;
    }
}

/**
 * @tc.name: GetComposerInfoSrcRect002
 * @tc.desc: Test RSUniRenderComposerAdapterTest.GetComposerInfoSrcRect with null consumer
 * @tc.type: FUNC
 * @tc.require: issues20101
 */
HWTEST_F(RSUniRenderComposerAdapterTest, GetComposerInfoSrcRect002, TestSize.Level2)
{
    NodeId id = 1;
    ScreenId screenId = 0;
    auto context = std::make_shared<RSContext>();
    auto rsScreenNode = std::make_shared<RSScreenRenderNode>(id, screenId, context->weak_from_this());
    auto screenDrawable = std::static_pointer_cast<DrawableV2::RSScreenRenderNodeDrawable>(
        DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(rsScreenNode));
    ASSERT_NE(screenDrawable, nullptr);

    sptr<SyncFence> acquireFence = SyncFence::INVALID_FENCE;
    auto buffer = new SurfaceBufferImpl(0);
    auto surfaceHandler = screenDrawable->GetMutableRSSurfaceHandlerOnDraw();
    ASSERT_NE(surfaceHandler, nullptr);
    surfaceHandler->SetBuffer(buffer, acquireFence, {}, 0);

    composerAdapter_->Init(info, offsetX, offsetY);
    ComposeInfo composeInfo = composerAdapter_->BuildComposeInfo(*screenDrawable, screenDrawable->GetDirtyRects());

    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);

    // Set consumer to nullptr
    surfaceNode->GetRSSurfaceHandler()->consumer_ = nullptr;

    // Function should return early when consumer is nullptr
    int32_t originalX = composeInfo.srcRect.x;
    int32_t originalY = composeInfo.srcRect.y;
    int32_t originalW = composeInfo.srcRect.w;
    int32_t originalH = composeInfo.srcRect.h;
    composerAdapter_->GetComposerInfoSrcRect(composeInfo, *surfaceNode);
    // Verify srcRect was not modified (function returns early when consumer is nullptr)
    ASSERT_EQ(composeInfo.srcRect.x, originalX);
    ASSERT_EQ(composeInfo.srcRect.y, originalY);
    ASSERT_EQ(composeInfo.srcRect.w, originalW);
    ASSERT_EQ(composeInfo.srcRect.h, originalH);

    if (buffer) {
        delete buffer;
        buffer = nullptr;
    }
}

/**
 * @tc.name: GetComposerInfoSrcRect003
 * @tc.desc: Test RSUniRenderComposerAdapterTest.GetComposerInfoSrcRect with null buffer
 * @tc.type: FUNC
 * @tc.require: issues20101
 */
HWTEST_F(RSUniRenderComposerAdapterTest, GetComposerInfoSrcRect003, TestSize.Level2)
{
    ComposeInfo composeInfo {};
    composeInfo.buffer = nullptr;

    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);

    // Function should return early when buffer is nullptr
    composerAdapter_->GetComposerInfoSrcRect(composeInfo, *surfaceNode);
    // Test passes if function completes without crash (void function)
    ASSERT_EQ(composeInfo.buffer, nullptr);  // Verify no state change
}

/**
 * @tc.name: GetComposerInfoSrcRect004
 * @tc.desc: Test RSUniRenderComposerAdapterTest.GetComposerInfoSrcRect for drawable with null buffer
 * @tc.type: FUNC
 * @tc.require: issues20101
 */
HWTEST_F(RSUniRenderComposerAdapterTest, GetComposerInfoSrcRect004, TestSize.Level2)
{
    ComposeInfo composeInfo {};
    composeInfo.buffer = nullptr;

    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);

    auto drawable = std::static_pointer_cast<DrawableV2::RSSurfaceRenderNodeDrawable>(
        DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(surfaceNode));
    ASSERT_NE(drawable, nullptr);

    // Function should return early when buffer is nullptr
    composerAdapter_->GetComposerInfoSrcRect(composeInfo, *drawable);
    ASSERT_EQ(composeInfo.buffer, nullptr);  // Verify no state change
}

/**
 * @tc.name: GetComposerInfoSrcRect005
 * @tc.desc: Test RSUniRenderComposerAdapterTest.GetComposerInfoSrcRect for drawable with null params
 * @tc.type: FUNC
 * @tc.require: issues20101
 */
HWTEST_F(RSUniRenderComposerAdapterTest, GetComposerInfoSrcRect005, TestSize.Level2)
{
    NodeId id = 1;
    ScreenId screenId = 0;
    auto context = std::make_shared<RSContext>();
    auto rsScreenNode = std::make_shared<RSScreenRenderNode>(id, screenId, context->weak_from_this());
    auto screenDrawable = std::static_pointer_cast<DrawableV2::RSScreenRenderNodeDrawable>(
        DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(rsScreenNode));
    ASSERT_NE(screenDrawable, nullptr);

    sptr<SyncFence> acquireFence = SyncFence::INVALID_FENCE;
    auto buffer = new SurfaceBufferImpl(0);
    auto surfaceHandler = screenDrawable->GetMutableRSSurfaceHandlerOnDraw();
    ASSERT_NE(surfaceHandler, nullptr);
    surfaceHandler->SetBuffer(buffer, acquireFence, {}, 0);

    composerAdapter_->Init(info, offsetX, offsetY);
    ComposeInfo composeInfo = composerAdapter_->BuildComposeInfo(*screenDrawable, screenDrawable->GetDirtyRects());

    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);

    auto drawable = std::static_pointer_cast<DrawableV2::RSSurfaceRenderNodeDrawable>(
        DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(surfaceNode));
    ASSERT_NE(drawable, nullptr);

    // params is nullptr, function should return early without crash
    int32_t originalX = composeInfo.srcRect.x;
    int32_t originalY = composeInfo.srcRect.y;
    int32_t originalW = composeInfo.srcRect.w;
    int32_t originalH = composeInfo.srcRect.h;
    composerAdapter_->GetComposerInfoSrcRect(composeInfo, *drawable);
    // Verify composeInfo was not modified (srcRect should remain unchanged)
    ASSERT_EQ(composeInfo.srcRect.x, originalX);
    ASSERT_EQ(composeInfo.srcRect.y, originalY);
    ASSERT_EQ(composeInfo.srcRect.w, originalW);
    ASSERT_EQ(composeInfo.srcRect.h, originalH);

    if (buffer) {
        delete buffer;
        buffer = nullptr;
    }
}

// ==================== InitFailed ====================

/**
 * @tc.name: InitFailed001
 * @tc.desc: Test RSUniRenderComposerAdapterTest.Init with null hdiBackend
 * @tc.type: FUNC
 * @tc.require: issuesI7T9RE
 */
HWTEST_F(RSUniRenderComposerAdapterTest, InitFailed001, TestSize.Level2)
{
    auto adapter = std::make_unique<RSUniRenderComposerAdapter>();
    // Create a ScreenInfo with invalid screen id to cause hdiBackend to be nullptr
    ScreenInfo invalidInfo;
    invalidInfo.id = 9999;
    bool result = adapter->Init(invalidInfo, offsetX, offsetY);
    // Expected to fail due to invalid screen configuration
    ASSERT_EQ(result, false);
}

// ==================== LayerCrop ====================

/**
 * @tc.name: LayerCrop001
 * @tc.desc: Test RSUniRenderComposerAdapterTest.LayerCrop with layer that equals screen rect
 * @tc.type: FUNC
 * @tc.require: issuesI7T9RE
 */
HWTEST_F(RSUniRenderComposerAdapterTest, LayerCrop001, TestSize.Level2)
{
    NodeId id = 1;
    ScreenId screenId = 0;
    auto context = std::make_shared<RSContext>();
    auto rsScreenNode = std::make_shared<RSScreenRenderNode>(id, screenId, context->weak_from_this());
    auto screenDrawable = std::static_pointer_cast<DrawableV2::RSScreenRenderNodeDrawable>(
        DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(rsScreenNode));
    ASSERT_NE(screenDrawable, nullptr);

    sptr<SyncFence> acquireFence = SyncFence::INVALID_FENCE;
    auto buffer = new SurfaceBufferImpl(0);
    auto surfaceHandler = screenDrawable->GetMutableRSSurfaceHandlerOnDraw();
    ASSERT_NE(surfaceHandler, nullptr);
    surfaceHandler->SetBuffer(buffer, acquireFence, {}, 0);

    composerAdapter_->Init(info, offsetX, offsetY);

    RSLayerPtr layer = std::make_shared<RSSurfaceLayer>();
    ASSERT_NE(layer, nullptr);

    // Set layer size to equal screen size - should return early
    layer->SetLayerSize(GraphicIRect {0, 0, static_cast<int32_t>(info.width), static_cast<int32_t>(info.height)});
    layer->SetCropRect(GraphicIRect {0, 0, 100, 100});

    // Save original cropRect to verify no changes
    GraphicIRect originalCropRect = layer->GetCropRect();

    composerAdapter_->LayerCrop(layer);
    // Verify cropRect was not modified (function returns early)
    ASSERT_EQ(layer->GetCropRect().x, originalCropRect.x);
    ASSERT_EQ(layer->GetCropRect().y, originalCropRect.y);
    ASSERT_EQ(layer->GetCropRect().w, originalCropRect.w);
    ASSERT_EQ(layer->GetCropRect().h, originalCropRect.h);

    if (buffer) {
        delete buffer;
        buffer = nullptr;
    }
}

// ==================== LayerRotate ====================

/**
 * @tc.name: LayerRotate001
 * @tc.desc: Test RSUniRenderComposerAdapterTest.LayerRotate with null surface
 * @tc.type: FUNC
 * @tc.require: issuesI7T9RE
 */
HWTEST_F(RSUniRenderComposerAdapterTest, LayerRotate001, TestSize.Level2)
{
    NodeId id = 1;
    ScreenId screenId = 0;
    auto context = std::make_shared<RSContext>();
    auto rsScreenNode = std::make_shared<RSScreenRenderNode>(id, screenId, context->weak_from_this());
    auto screenDrawable = std::static_pointer_cast<DrawableV2::RSScreenRenderNodeDrawable>(
        DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(rsScreenNode));
    ASSERT_NE(screenDrawable, nullptr);

    sptr<SyncFence> acquireFence = SyncFence::INVALID_FENCE;
    auto buffer = new SurfaceBufferImpl(0);
    auto surfaceHandler = screenDrawable->GetMutableRSSurfaceHandlerOnDraw();
    ASSERT_NE(surfaceHandler, nullptr);
    surfaceHandler->SetBuffer(buffer, acquireFence, {}, 0);

    composerAdapter_->Init(info, offsetX, offsetY);

    RSLayerPtr layer = std::make_shared<RSSurfaceLayer>();
    ASSERT_NE(layer, nullptr);

    // Create a surface node for LayerRotate which expects RSSurfaceRenderNode
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);

    // surface is nullptr - function should return early without crash
    // Save layer state before call
    GraphicTransformType originalTransform = layer->GetTransformType();
    composerAdapter_->LayerRotate(layer, *surfaceNode);
    // Verify transform was not modified (function returns early when surface is nullptr)
    ASSERT_EQ(layer->GetTransformType(), originalTransform);

    if (buffer) {
        delete buffer;
        buffer = nullptr;
    }
}

// ==================== LayerScaleDown ====================

/**
 * @tc.name: LayerScaleDown001
 * @tc.desc: Test RSUniRenderComposerAdapterTest.LayerScaleDown with null buffer
 * @tc.type: FUNC
 * @tc.require: issuesI7T9RE
 */
HWTEST_F(RSUniRenderComposerAdapterTest, LayerScaleDown001, TestSize.Level2)
{
    RSLayerPtr layer = std::make_shared<RSSurfaceLayer>();
    ASSERT_NE(layer, nullptr);

    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);
    RectI rect{0, 0, DEFAULT_CANVAS_WIDTH, DEFAULT_CANVAS_HEIGHT};
    surfaceNode->SetSrcRect(rect);
    surfaceNode->SetDstRect(rect);

    // Layer buffer is nullptr - function should return early without crash
    // Save layer state before call
    GraphicIRect originalCropRect = layer->GetCropRect();
    composerAdapter_->LayerScaleDown(layer, *surfaceNode);
    // Verify cropRect was not modified (function returns early when buffer is nullptr)
    ASSERT_EQ(layer->GetCropRect().x, originalCropRect.x);
    ASSERT_EQ(layer->GetCropRect().y, originalCropRect.y);
    ASSERT_EQ(layer->GetCropRect().w, originalCropRect.w);
    ASSERT_EQ(layer->GetCropRect().h, originalCropRect.h);
}

/**
 * @tc.name: LayerScaleDown002
 * @tc.desc: Test RSUniRenderComposerAdapterTest.LayerScaleDown for drawable with null params
 * @tc.type: FUNC
 * @tc.require: issuesI7T9RE
 */
HWTEST_F(RSUniRenderComposerAdapterTest, LayerScaleDown002, TestSize.Level2)
{
    RSLayerPtr layer = std::make_shared<RSSurfaceLayer>();
    ASSERT_NE(layer, nullptr);

    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);

    auto drawable = std::static_pointer_cast<DrawableV2::RSSurfaceRenderNodeDrawable>(
        DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(surfaceNode));
    ASSERT_NE(drawable, nullptr);

    // params is nullptr - function should return early without crash
    // Save layer state before call
    GraphicIRect originalCropRect = layer->GetCropRect();
    composerAdapter_->LayerScaleDown(layer, *drawable);
    // Verify cropRect was not modified (function returns early when params is nullptr)
    ASSERT_EQ(layer->GetCropRect().x, originalCropRect.x);
    ASSERT_EQ(layer->GetCropRect().y, originalCropRect.y);
    ASSERT_EQ(layer->GetCropRect().w, originalCropRect.w);
    ASSERT_EQ(layer->GetCropRect().h, originalCropRect.h);
}

// ==================== LayerScaleFit ====================

/**
 * @tc.name: LayerScaleFit001
 * @tc.desc: Test RSUniRenderComposerAdapterTest.LayerScaleFit
 * @tc.type: FUNC
 * @tc.require: issuesI7T9RE
 */
HWTEST_F(RSUniRenderComposerAdapterTest, LayerScaleFit001, TestSize.Level2)
{
    NodeId id = 1;
    ScreenId screenId = 0;
    auto context = std::make_shared<RSContext>();
    auto rsScreenNode = std::make_shared<RSScreenRenderNode>(id, screenId, context->weak_from_this());
    auto screenDrawable = std::static_pointer_cast<DrawableV2::RSScreenRenderNodeDrawable>(
        DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(rsScreenNode));
    ASSERT_NE(screenDrawable, nullptr);

    sptr<SyncFence> acquireFence = SyncFence::INVALID_FENCE;
    auto buffer = new SurfaceBufferImpl(0);
    auto surfaceHandler = screenDrawable->GetMutableRSSurfaceHandlerOnDraw();
    ASSERT_NE(surfaceHandler, nullptr);
    surfaceHandler->SetBuffer(buffer, acquireFence, {}, 0);

    composerAdapter_->Init(info, offsetX, offsetY);

    RSLayerPtr layer = std::make_shared<RSSurfaceLayer>();
    ASSERT_NE(layer, nullptr);

    layer->SetLayerSize(GraphicIRect {0, 0, 100, 100});
    layer->SetCropRect(GraphicIRect {0, 0, 50, 50});

    // Should process without crash
    composerAdapter_->LayerScaleFit(layer);

    if (buffer) {
        delete buffer;
        buffer = nullptr;
    }
}

// ==================== SetBufferColorSpace ====================

/**
 * @tc.name: SetBufferColorSpace001
 * @tc.desc: Test RSUniRenderComposerAdapterTest.SetBufferColorSpace
 * @tc.type: FUNC
 * @tc.require: issuesI8C4I9
*/
HWTEST_F(RSUniRenderComposerAdapterTest, SetBufferColorSpace001, TestSize.Level2)
{
    using namespace HDI::Display::Graphic::Common::V1_0;

    auto rsContext = std::make_shared<RSContext>();
    RSScreenRenderNode::SharedPtr nodePtr = std::make_shared<RSScreenRenderNode>(1, 0, rsContext->weak_from_this());
    auto screenDrawable = std::static_pointer_cast<DrawableV2::RSScreenRenderNodeDrawable>(
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
}

/**
 * @tc.name: SetBufferColorSpace002
 * @tc.desc: Test RSUniRenderComposerAdapterTest.SetBufferColorSpace
 * @tc.type: FUNC
 * @tc.require: issuesI8C4I9
*/
HWTEST_F(RSUniRenderComposerAdapterTest, SetBufferColorSpace002, TestSize.Level2)
{
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

/**
 * @tc.name: SetBufferColorSpace004
 * @tc.desc: Test RSUniRenderComposerAdapterTest.SetBufferColorSpace with null rsSurface
 * @tc.type: FUNC
 * @tc.require: issuesI8C4I9
 */
HWTEST_F(RSUniRenderComposerAdapterTest, SetBufferColorSpace004, TestSize.Level2)
{
    NodeId id = 1;
    auto rsContext = std::make_shared<RSContext>();
    auto node = std::make_shared<RSScreenRenderNode>(id, 0, rsContext->weak_from_this());
    auto screenDrawable = std::static_pointer_cast<DrawableV2::RSScreenRenderNodeDrawable>(
        DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(node));
    ASSERT_NE(screenDrawable, nullptr);

    sptr<SyncFence> acquireFence = SyncFence::INVALID_FENCE;
    auto surfaceHandler = screenDrawable->GetMutableRSSurfaceHandlerOnDraw();
    ASSERT_NE(surfaceHandler, nullptr);

    auto buffer = new SurfaceBufferImpl(0);
    surfaceHandler->SetBuffer(buffer, acquireFence, {}, 0);
    // rsSurface will be nullptr, should return without crash
    RSUniRenderComposerAdapter::SetBufferColorSpace(*screenDrawable);

    if (buffer) {
        delete buffer;
        buffer = nullptr;
    }
}

/**
 * @tc.name: SetBufferColorSpace005
 * @tc.desc: Test RSUniRenderComposerAdapterTest.SetBufferColorSpace with null buffer
 * @tc.type: FUNC
 * @tc.require: issuesI8C4I9
 */
HWTEST_F(RSUniRenderComposerAdapterTest, SetBufferColorSpace005, TestSize.Level2)
{
    NodeId id = 1;
    auto rsContext = std::make_shared<RSContext>();
    auto node = std::make_shared<RSScreenRenderNode>(id, 0, rsContext->weak_from_this());
    auto screenDrawable = std::static_pointer_cast<DrawableV2::RSScreenRenderNodeDrawable>(
        DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(node));
    ASSERT_NE(screenDrawable, nullptr);

    // buffer is nullptr, should return without crash
    RSUniRenderComposerAdapter::SetBufferColorSpace(*screenDrawable);
}

// ==================== SetComposeInfoToLayer ====================

/**
 * @tc.name: SetComposeInfoToLayer001
 * @tc.desc: Test RSUniRenderComposerAdapterTest.SetComposeInfoToLayer with null layer
 * @tc.type: FUNC
 * @tc.require: issuesI7T9RE
 */
HWTEST_F(RSUniRenderComposerAdapterTest, SetComposeInfoToLayer001, TestSize.Level2)
{
    NodeId id = 1;
    ScreenId screenId = 0;
    auto context = std::make_shared<RSContext>();
    auto rsScreenNode = std::make_shared<RSScreenRenderNode>(id, screenId, context->weak_from_this());
    auto screenDrawable = std::static_pointer_cast<DrawableV2::RSScreenRenderNodeDrawable>(
        DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(rsScreenNode));
    ASSERT_NE(screenDrawable, nullptr);

    sptr<SyncFence> acquireFence = SyncFence::INVALID_FENCE;
    auto buffer = new SurfaceBufferImpl(0);
    auto surfaceHandler = screenDrawable->GetMutableRSSurfaceHandlerOnDraw();
    ASSERT_NE(surfaceHandler, nullptr);
    surfaceHandler->SetBuffer(buffer, acquireFence, {}, 0);

    composerAdapter_->Init(info, offsetX, offsetY);
    ComposeInfo composeInfo = composerAdapter_->BuildComposeInfo(*screenDrawable, screenDrawable->GetDirtyRects());

    RSLayerPtr layer = nullptr;
    // Function should return early when layer is nullptr without crash
    // This is a void function with no return value; testing early return path
    composerAdapter_->SetComposeInfoToLayer(layer, composeInfo, surfaceHandler->GetConsumer());
    // Test passes if function completes without crash (void function, early return)

    if (buffer) {
        delete buffer;
        buffer = nullptr;
    }
}

// ==================== SetMetaDataInfoToLayer ====================

/**
 * @tc.name: SetMetaDataInfoToLayer001
 * @tc.desc: Test RSUniRenderComposerAdapterTest.SetMetaDataInfoToLayer
 * @tc.type: FUNC
 * @tc.require: issues20101
*/
HWTEST_F(RSUniRenderComposerAdapterTest, SetMetaDataInfoToLayer001, TestSize.Level2)
{
    NodeId id = 1;
    ScreenId screenId = 0;
    auto context = std::make_shared<RSContext>();
    auto rsScreenNode = std::make_shared<RSScreenRenderNode>(id, screenId, context->weak_from_this());
    auto screenDrawable = std::static_pointer_cast<DrawableV2::RSScreenRenderNodeDrawable>(
        DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(rsScreenNode));
    ASSERT_NE(screenDrawable, nullptr);
    sptr<SyncFence> acquireFence = SyncFence::INVALID_FENCE;
    auto surfaceHandler = screenDrawable->GetMutableRSSurfaceHandlerOnDraw();
    ASSERT_NE(surfaceHandler, nullptr);
    auto buffer = new SurfaceBufferImpl(0);
    surfaceHandler->SetBuffer(buffer, acquireFence, {}, 0);
    // Initialize composerAdapter_ before calling BuildComposeInfo
    composerAdapter_->Init(info, offsetX, offsetY);
    ComposeInfo composeInfo = composerAdapter_->BuildComposeInfo(*screenDrawable, screenDrawable->GetDirtyRects());
    RSLayerPtr layer = std::make_shared<RSSurfaceLayer>();
    layer->SetUniRenderFlag(true);
    // This is a void function with no return value; testing it completes without crash
    composerAdapter_->SetMetaDataInfoToLayer(layer, composeInfo.buffer, surfaceHandler->GetConsumer());
    // Test passes if function completes without crash (void function)
    if (buffer) {
        delete buffer;
        buffer = nullptr;
    }
}

/**
 * @tc.name: SetMetaDataInfoToLayer002
 * @tc.desc: Test RSUniRenderComposerAdapterTest.SetMetaDataInfoToLayer with null layer
 * @tc.type: FUNC
 * @tc.require: issues20101
 */
HWTEST_F(RSUniRenderComposerAdapterTest, SetMetaDataInfoToLayer002, TestSize.Level2)
{
    RSLayerPtr layer = nullptr;
    sptr<SurfaceBuffer> buffer = new SurfaceBufferImpl(0);
    sptr<IConsumerSurface> surface = nullptr;
    // Function should return early when layer is null without crash
    ASSERT_NE(buffer, nullptr);  // Verify buffer is valid
    composerAdapter_->SetMetaDataInfoToLayer(layer, buffer, surface);
    // Test passes if function completes without crash (void function, early return)
    if (buffer) {
        delete buffer;
        buffer = nullptr;
    }
}

/**
 * @tc.name: SetMetaDataInfoToLayer003
 * @tc.desc: Test RSUniRenderComposerAdapterTest.SetMetaDataInfoToLayer with null surface
 * @tc.type: FUNC
 * @tc.require: issues20101
 */
HWTEST_F(RSUniRenderComposerAdapterTest, SetMetaDataInfoToLayer003, TestSize.Level2)
{
    RSLayerPtr layer = std::make_shared<RSSurfaceLayer>();
    sptr<SurfaceBuffer> buffer = new SurfaceBufferImpl(0);
    sptr<IConsumerSurface> surface = nullptr;
    // Function should return early when surface is null without crash
    ASSERT_NE(layer, nullptr);  // Verify layer is valid
    ASSERT_NE(buffer, nullptr);  // Verify buffer is valid
    composerAdapter_->SetMetaDataInfoToLayer(layer, buffer, surface);
    // Test passes if function completes without crash (void function, early return)
    if (buffer) {
        delete buffer;
        buffer = nullptr;
    }
}

/**
 * @tc.name: SetMetaDataInfoToLayer004
 * @tc.desc: Test RSUniRenderComposerAdapterTest.SetMetaDataInfoToLayer with null buffer
 * @tc.type: FUNC
 * @tc.require: issues20101
 */
HWTEST_F(RSUniRenderComposerAdapterTest, SetMetaDataInfoToLayer004, TestSize.Level2)
{
    RSLayerPtr layer = std::make_shared<RSSurfaceLayer>();
    sptr<SurfaceBuffer> buffer = nullptr;
    sptr<IConsumerSurface> surface = nullptr;
    // Function should return early when buffer is null without crash
    ASSERT_NE(layer, nullptr);  // Verify layer is valid
    composerAdapter_->SetMetaDataInfoToLayer(layer, buffer, surface);
}

// ==================== SrcRectRotateTransform ====================

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
 * @tc.name: SrcRectRotateTransform007
 * @tc.desc: Test RSUniRenderComposerAdapterTest.SrcRectRotateTransform for drawable
 *           with null renderParams (access private member directly)
 * @tc.type: FUNC
 * @tc.require: issuesI7T9RE
 */
HWTEST_F(RSUniRenderComposerAdapterTest, SrcRectRotateTransform007, TestSize.Level2)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);

    auto drawable = std::static_pointer_cast<DrawableV2::RSSurfaceRenderNodeDrawable>(
        DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(surfaceNode));
    ASSERT_NE(drawable, nullptr);

    // Directly set private renderParams_ to nullptr to test null params path
    drawable->renderParams_ = nullptr;

    auto resultRect = composerAdapter_->SrcRectRotateTransform(*drawable);
    // Should return empty RectI when params is nullptr
    ASSERT_EQ(resultRect.width_, 0);
    ASSERT_EQ(resultRect.height_, 0);
}

// ==================== UpdateMirrorInfo ====================

/**
 * @tc.name: UpdateMirrorInfo001
 * @tc.desc: Test RSUniRenderComposerAdapterTest.UpdateMirrorInfo
 * @tc.type: FUNC
 * @tc.require: issuesI7T9RE
 */
HWTEST_F(RSUniRenderComposerAdapterTest, UpdateMirrorInfo001, TestSize.Level2)
{
    float testCoefficient = 1.5f;
    bool result = composerAdapter_->UpdateMirrorInfo(testCoefficient);
    ASSERT_EQ(result, true);
    ASSERT_EQ(composerAdapter_->mirrorAdaptiveCoefficient_, testCoefficient);
}

} // namespace
