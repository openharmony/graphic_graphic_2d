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

#include <parameters.h>
#include "dirty_region/rs_gpu_dirty_collector.h"
#include "drawable/rs_logical_display_render_node_drawable.h"
#include "drawable/rs_screen_render_node_drawable.h"
#include "drawable/rs_surface_render_node_drawable.h"
#include "feature/round_corner_display/rs_rcd_surface_render_node.h"
#include "limit_number.h"
#include "metadata_helper.h"
#include "params/rs_screen_render_params.h"
#include "engine/rs_uni_render_engine.h"
#include "pipeline/render_thread/rs_uni_render_processor.h"
#include "pipeline/render_thread/rs_render_engine.h"
#include "pipeline/rs_logical_display_render_node.h"
#include "pipeline/rs_processor_factory.h"
#include "pipeline/rs_test_util.h"
#include "rs_surface_layer.h"
#include "screen_manager/rs_screen.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
namespace {
constexpr Rect DEFAULT_RECT = {0, 0, 200, 200};
constexpr uint64_t BUFFER_USAGE_GPU_RENDER_DIRTY = BUFFER_USAGE_HW_RENDER | BUFFER_USAGE_AUXILLARY_BUFFER0;
}
class RSUniRenderProcessorTest : public testing::Test {
public:
    static inline BufferRequestConfig requestConfig = {
        .width = 200,
        .height = 200,
        .strideAlignment = 0x8,
        .format = GRAPHIC_PIXEL_FMT_RGBA_8888,
        .usage = BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA | BUFFER_USAGE_GPU_RENDER_DIRTY,
        .timeout = 0,
        .colorGamut = GraphicColorGamut::GRAPHIC_COLOR_GAMUT_DCI_P3,
    };

    static inline BufferSelfDrawingData defaultSelfDrawingRect = {
        .gpuDirtyEnable = true,
        .curFrameDirtyEnable = true,
        .left = 0,
        .top = 0,
        .right = 100,
        .bottom = 100,
    };

    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    static inline uint32_t screenId_ = 0;
    static inline std::shared_ptr<RSUniRenderProcessor> renderProcessor = nullptr;
    static inline std::unique_ptr<RSScreen> screen = nullptr;
};

void RSUniRenderProcessorTest::SetUpTestCase()
{
#ifdef RS_ENABLE_VK
    RsVulkanContext::SetRecyclable(false);
#endif
    RSUniRenderThread::Instance().composerClientManager_ = std::make_shared<RSComposerClientManager>();
    RSTestUtil::InitRenderNodeGC();
    auto output = std::make_shared<HdiOutput>(screenId_);
    std::shared_ptr<AppExecFwk::EventHandler> handler = nullptr;
    screen = std::make_unique<RSScreen>(screenId_);
    auto screenManager = sptr<RSScreenManager>::MakeSptr();
    screenManager->screens_.insert(std::make_pair(screenId_, std::move(screen)));

    renderProcessor = std::make_shared<RSUniRenderProcessor>(screenId_);
    renderProcessor->screenInfo_.id = screenId_;
    NodeId nodeId = 1;
    RSScreenRenderNode node(nodeId, screenId_);
    auto renderEngine = std::make_shared<RSUniRenderEngine>();
    auto composerClient = RSComposerClient::Create(nullptr, nullptr);
    renderProcessor->composerClient_ = composerClient;
    renderProcessor->Init(node, renderEngine);
    EXPECT_NE(renderProcessor->composerClient_, nullptr);
}

void RSUniRenderProcessorTest::TearDownTestCase()
{
    renderProcessor = nullptr;
}
void RSUniRenderProcessorTest::SetUp()
{
    if (renderProcessor != nullptr) {
        renderProcessor->layers_.clear();
    }
}
void RSUniRenderProcessorTest::TearDown() {}

/**
 * @tc.name: ProcessorInit001
 * @tc.desc: test ProcessorInit func with renderEngine nullptr
 * @tc.type: FUNC
 * @tc.require: issueI6QM6E
 */
HWTEST_F(RSUniRenderProcessorTest, ProcessorInit001, TestSize.Level1)
{
    NodeId nodeId = 1;
    ScreenId screenId = 0;
    std::weak_ptr<RSContext> context = {};
    if (RSUniRenderJudgement::IsUniRender()) {
        std::shared_ptr<RSComposerClientManager> rsComposerClientMgr = std::make_shared<RSComposerClientManager>();
        RSUniRenderThread::Instance().composerClientManager_ = rsComposerClientMgr;
        auto processor = RSProcessorFactory::CreateProcessor(CompositeType::UNI_RENDER_COMPOSITE, screenId);
        RSScreenRenderNode node(nodeId, screenId, context);
        EXPECT_EQ(processor->Init(node, nullptr), false);
    }
}

/**
 * @tc.name: ProcessSurface001
 * @tc.desc: test ProcessSurface func with invalid layer info
 * @tc.type: FUNC
 * @tc.require: issueI6QM6E
 */
HWTEST_F(RSUniRenderProcessorTest, ProcessSurface001, TestSize.Level1)
{
    NodeId nodeId = 1;
    ScreenId screenId = 0;
    std::weak_ptr<RSContext> context = {};
    if (RSUniRenderJudgement::IsUniRender()) {
        std::shared_ptr<RSComposerClientManager> rsComposerClientMgr = std::make_shared<RSComposerClientManager>();
        RSUniRenderThread::Instance().composerClientManager_ = rsComposerClientMgr;
        auto processor = RSProcessorFactory::CreateProcessor(CompositeType::UNI_RENDER_COMPOSITE, screenId);
        ASSERT_NE(processor, nullptr);
        RSScreenRenderNode node(nodeId, screenId, context);
        auto uniRenderEngine = std::make_shared<RSUniRenderEngine>();
        processor->Init(node, uniRenderEngine);
        RSSurfaceRenderNode surfaceNode(2);
        processor->ProcessSurface(surfaceNode);
    }
}

/**
 * @tc.name: ProcessSurfaceTest
 * @tc.desc: Verify function ProcessSurface
 * @tc.type:FUNC
 * @tc.require:issuesI9KRF1
 */
HWTEST_F(RSUniRenderProcessorTest, ProcessSurfaceTest, TestSize.Level1)
{
    if (RSUniRenderJudgement::IsUniRender()) {
        ASSERT_NE(renderProcessor, nullptr);
        // for test
        RSSurfaceRenderNode surfaceNode(2);
        renderProcessor->ProcessSurface(surfaceNode);
    }
}

/**
 * @tc.name: CreateLayerTest
 * @tc.desc: Verify function CreateLayer
 * @tc.type:FUNC
 * @tc.require:issuesI9KRF1
 */
HWTEST_F(RSUniRenderProcessorTest, CreateLayerTest, TestSize.Level1)
{
    if (RSUniRenderJudgement::IsUniRender()) {
        auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
        auto surfaceDrawable = std::static_pointer_cast<DrawableV2::RSSurfaceRenderNodeDrawable>(
            DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(surfaceNode));
        auto params = static_cast<RSSurfaceRenderParams*>(surfaceDrawable->GetRenderParams().get());
        RSLayerInfo layerInfo;
        sptr<SurfaceBuffer> bufferTest = OHOS::SurfaceBuffer::Create();
        sptr<SurfaceBuffer> preBufferTest = OHOS::SurfaceBuffer::Create();
        params->SetBuffer(bufferTest, {}, DEFAULT_RECT);
        params->SetPreBuffer(preBufferTest, nullptr);
        layerInfo.zOrder = 0;
        params->SetLayerInfo(layerInfo);
        renderProcessor->composerClient_ = nullptr;
        renderProcessor->CreateLayer(*surfaceNode, *params);

        NodeId nodeId = 1;
        RSScreenRenderNode screenNode(nodeId, screenId_);
        auto renderEngine = std::make_shared<RSUniRenderEngine>();
        renderProcessor->Init(screenNode, renderEngine);
        auto composerClient = RSComposerClient::Create(nullptr, nullptr);
        renderProcessor->composerClient_ = composerClient;
        renderProcessor->CreateLayer(*surfaceNode, *params);
        EXPECT_TRUE(params->GetLayerCreated());
    }
}

/**
 * @tc.name: ProcessScreenSurfaceTest
 * @tc.desc: Verify function ProcessScreenSurface
 * @tc.type:FUNC
 * @tc.require:issuesI9KRF1
 */
HWTEST_F(RSUniRenderProcessorTest, ProcessScreenSurfaceTest, TestSize.Level1)
{
    ScreenId screenId = 0;
    std::weak_ptr<RSContext> context = {};
    if (RSUniRenderJudgement::IsUniRender()) {
        constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[0];
        RSScreenRenderNode node(nodeId, screenId, context);
        auto size = renderProcessor->layers_.size();
        renderProcessor->ProcessScreenSurface(node);
        EXPECT_EQ(renderProcessor->layers_.size(), size);
    }
}

/**
 * @tc.name: ProcessScreenSurfaceTest002
 * @tc.desc: Verify function ProcessScreenSurface
 * @tc.type:FUNC
 * @tc.require:issuesI9KRF1
 */
HWTEST_F(RSUniRenderProcessorTest, ProcessScreenSurfaceTest002, TestSize.Level1)
{
    ScreenId screenId = 0;
    std::weak_ptr<RSContext> context = {};
    if (RSUniRenderJudgement::IsUniRender()) {
        constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[0];
        RSScreenRenderNode node(nodeId, screenId, context);
        auto size = renderProcessor->layers_.size();
        auto layerSkipController = node.GetDynamicLayerSkipController();
        layerSkipController->screenLayerInvalid_ = true;
        renderProcessor->ProcessScreenSurface(node);
        EXPECT_EQ(renderProcessor->layers_.size(), size);
    }
}

/**
 * @tc.name: ProcessRcdSurfaceTest
 * @tc.desc: Verify function ProcessRcdSurface
 * @tc.type:FUNC
 * @tc.require:issuesI9KRF1
 */
HWTEST_F(RSUniRenderProcessorTest, ProcessRcdSurfaceTest, TestSize.Level1)
{
    if (RSUniRenderJudgement::IsUniRender()) {
        constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[0];
        RCDSurfaceType type = RCDSurfaceType::INVALID;
        RSRcdSurfaceRenderNode node(nodeId, type);
        auto size = renderProcessor->layers_.size();
        renderProcessor->ProcessRcdSurface(node);
        EXPECT_EQ(renderProcessor->layers_.size(), size);
    }
}

/**
 * @tc.name: InitForRenderThread001
 * @tc.desc: Test RSUniRenderProcessorTest.InitForRenderThread when renderEngine is nullptr
 * @tc.type:FUNC
 * @tc.require: issueIAJ1RT
 */
HWTEST_F(RSUniRenderProcessorTest, InitForRenderThread001, TestSize.Level1)
{
    if (RSUniRenderJudgement::IsUniRender()) {
        // case1: renderEngine is nullptr
        NodeId nodeId = 1;
        auto screenNode = std::make_shared<RSRenderNode>(nodeId);
        auto screenDrawable = std::static_pointer_cast<DrawableV2::RSScreenRenderNodeDrawable>(
            DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(screenNode));
        ASSERT_NE(renderProcessor, nullptr);
        auto ret = renderProcessor->InitForRenderThread(*screenDrawable, nullptr);
        EXPECT_EQ(ret, false);

        // case2: renderParams is nullptr
        auto renderEngine = std::make_shared<RSRenderEngine>();
        screenDrawable->renderParams_ = nullptr;
        ret = renderProcessor->InitForRenderThread(*screenDrawable, renderEngine);
        EXPECT_EQ(ret, false);

        // case3: renderEngine not nullptr, renderParams not nullptr
        screenDrawable->renderParams_ = std::make_unique<RSScreenRenderParams>(screenNode->GetId());
        ret = renderProcessor->InitForRenderThread(*screenDrawable, renderEngine);
        EXPECT_EQ(ret, true);
    }
}

/**
 * @tc.name: InitForRenderThread002
 * @tc.desc: Test RSUniRenderProcessorTest.InitForRenderThread with not nullptr
 * @tc.type:FUNC
 * @tc.require: issueIAJ1RT
 */
HWTEST_F(RSUniRenderProcessorTest, InitForRenderThread002, TestSize.Level1)
{
    if (RSUniRenderJudgement::IsUniRender()) {
        NodeId nodeId = 1;
        ScreenId screenId = 0;
        auto context = std::make_shared<RSContext>();
        std::shared_ptr<RSScreenRenderNode> screenNode =
            std::make_shared<RSScreenRenderNode>(nodeId, screenId, context->weak_from_this());
        auto screenDrawable = std::static_pointer_cast<DrawableV2::RSScreenRenderNodeDrawable>(
            DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(screenNode));
        auto params = static_cast<RSScreenRenderParams*>(screenDrawable->GetRenderParams().get());
        params->screenInfo_.id = screenId;
        auto renderEngine = std::make_shared<RSRenderEngine>();
        ASSERT_NE(renderEngine, nullptr);
        bool result = renderProcessor->InitForRenderThread(*screenDrawable, renderEngine);
        ASSERT_EQ(result, false);
    }
}

/**
 * @tc.name: ProcessScreenSurfaceForRenderThread001
 * @tc.desc: Test RSUniRenderProcessorTest.ProcessScreenSurfaceForRenderThread when layer is nullptr
 * @tc.type:FUNC
 * @tc.require: issueIAJ1RT
 */
HWTEST_F(RSUniRenderProcessorTest, ProcessScreenSurfaceForRenderThread001, TestSize.Level1)
{
    if (RSUniRenderJudgement::IsUniRender()) {
        NodeId nodeId = 1;
        auto node = std::make_shared<RSRenderNode>(nodeId);
        DrawableV2::RSScreenRenderNodeDrawable drawable(node);
        ASSERT_NE(renderProcessor, nullptr);
        renderProcessor->ProcessScreenSurfaceForRenderThread(drawable);
    }
}

/**
 * @tc.name: ProcessScreenSurfaceForRenderThread002
 * @tc.desc: Test RSUniRenderProcessorTest.ProcessScreenSurfaceForRenderThread when params is nullptr
 * @tc.type:FUNC
 * @tc.require: issueIAJ1RT
 */
HWTEST_F(RSUniRenderProcessorTest, ProcessScreenSurfaceForRenderThread002, TestSize.Level1)
{
    if (RSUniRenderJudgement::IsUniRender()) {
        NodeId nodeId = 1;
        auto node = std::make_shared<RSRenderNode>(nodeId);
        DrawableV2::RSScreenRenderNodeDrawable drawable(node);
        drawable.renderParams_= nullptr;
        ASSERT_NE(renderProcessor, nullptr);
        renderProcessor->ProcessScreenSurfaceForRenderThread(drawable);
    }
}

/**
 * @tc.name: ProcessScreenSurfaceForRenderThread003
 * @tc.desc: Test RSUniRenderProcessorTest.ProcessScreenSurfaceForRenderThread when Fingerprint_ is false
 * @tc.type:FUNC
 * @tc.require: issueIAJ1RT
 */
HWTEST_F(RSUniRenderProcessorTest, ProcessScreenSurfaceForRenderThread003, TestSize.Level1)
{
    if (RSUniRenderJudgement::IsUniRender()) {
        NodeId nodeId = 1;
        auto node = std::make_shared<RSRenderNode>(nodeId);
        DrawableV2::RSScreenRenderNodeDrawable drawable(node);
        drawable.renderParams_= std::make_unique<RSRenderParams>(0);

        ASSERT_NE(renderProcessor, nullptr);
        renderProcessor->ProcessScreenSurfaceForRenderThread(drawable);
    }
}

/**
 * @tc.name: ProcessScreenSurfaceForRenderThread004
 * @tc.desc: Test RSUniRenderProcessorTest.ProcessScreenSurfaceForRenderThread when Fingerprint_ is true
 * @tc.type:FUNC
 * @tc.require: issueIAJ1RT
 */
HWTEST_F(RSUniRenderProcessorTest, ProcessScreenSurfaceForRenderThread004, TestSize.Level1)
{
    if (RSUniRenderJudgement::IsUniRender()) {
        NodeId nodeId = 1;
        auto node = std::make_shared<RSRenderNode>(nodeId);
        DrawableV2::RSScreenRenderNodeDrawable drawable(node);
        drawable.renderParams_= std::make_unique<RSRenderParams>(0);
        drawable.renderParams_->SetFingerprint(true);
        drawable.surfaceHandler_ = std::make_shared<RSSurfaceHandler>(0);
        ASSERT_NE(drawable.surfaceHandler_, nullptr);

        ASSERT_NE(renderProcessor, nullptr);
        renderProcessor->ProcessScreenSurfaceForRenderThread(drawable);
    }
}

// Test helper class for buffer consumer listener
class TestBufferConsumerListener : public IBufferConsumerListener {
public:
    void OnBufferAvailable() override {}
};

/**
 * @tc.name: ProcessScreenSurface_FingerprintTrueTest001
 * @tc.desc: Test ProcessScreenSurface when GetFingerprint() is true
 *           The if (node.GetFingerprint()) branch at line 520 should be true
 * @tc.type: FUNC
 * @tc.require: issue41
 */
HWTEST_F(RSUniRenderProcessorTest, ProcessScreenSurface_FingerprintTrueTest001, TestSize.Level2)
{
    if (RSUniRenderJudgement::IsUniRender()) {
        NodeId nodeId = 1;
        auto node = std::make_shared<RSScreenRenderNode>(nodeId, screenId_);
        node->SetFingerprint(true);

        // Initialize render drawable
        auto drawable = node->GetRenderDrawable();
        if (drawable == nullptr) {
            drawable = DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(
                std::static_pointer_cast<RSRenderNode>(node));
        }
        ASSERT_NE(drawable, nullptr);

        auto screenDrawable = std::static_pointer_cast<DrawableV2::RSScreenRenderNodeDrawable>(drawable);
        ASSERT_NE(screenDrawable, nullptr);

        // Set surfaceHandler
        screenDrawable->surfaceHandler_ = std::make_shared<RSSurfaceHandler>(0);
        ASSERT_NE(screenDrawable->surfaceHandler_, nullptr);

        // Create consumer and call CreateSurface to set surfaceCreated_ = true
        // Use a simple test listener since RSRenderServiceListener requires RSSurfaceRenderNode
        sptr<IBufferConsumerListener> listener = new TestBufferConsumerListener();
        screenDrawable->CreateSurface(listener);
        ASSERT_TRUE(screenDrawable->IsSurfaceCreated());

        // Request and flush buffer
        auto consumer = screenDrawable->surfaceHandler_->GetConsumer();
        ASSERT_NE(consumer, nullptr);
        auto producer = consumer->GetProducer();
        auto surface = Surface::CreateSurfaceAsProducer(producer);
        ASSERT_NE(surface, nullptr);
        surface->SetQueueSize(1);

        sptr<SurfaceBuffer> buffer;
        sptr<SyncFence> requestFence = SyncFence::INVALID_FENCE;
        [[maybe_unused]] GSError ret = surface->RequestBuffer(buffer, requestFence, requestConfig);
        ASSERT_EQ(ret, GSERROR_OK);

        sptr<SyncFence> flushFence = SyncFence::INVALID_FENCE;
        BufferFlushConfig flushConfig = { .damage = { .w = 200, .h = 200, } };
        ret = surface->FlushBuffer(buffer, flushFence, flushConfig);
        ASSERT_EQ(ret, GSERROR_OK);

        // Acquire buffer to consumer
        OHOS::sptr<SurfaceBuffer> cbuffer;
        Rect damage;
        sptr<SyncFence> acquireFence = SyncFence::INVALID_FENCE;
        int64_t timestamp = 0;
        ret = consumer->AcquireBuffer(cbuffer, acquireFence, timestamp, damage);
        ASSERT_EQ(ret, GSERROR_OK);
        ASSERT_NE(cbuffer, nullptr);

        screenDrawable->surfaceHandler_->SetBuffer(cbuffer, acquireFence, damage, timestamp, nullptr);

        // Call ProcessScreenSurface with Fingerprint = true
        auto layerSize = renderProcessor->layers_.size();
        EXPECT_NO_FATAL_FAILURE(renderProcessor->ProcessScreenSurface(*node));

        // Verify that a layer was created
        EXPECT_GT(renderProcessor->layers_.size(), layerSize);
    }
}

/**
 * @tc.name: ProcessScreenSurface_FingerprintFalseTest001
 * @tc.desc: Test ProcessScreenSurface when GetFingerprint() is false
 *           The if (node.GetFingerprint()) branch at line 520 should be false
 * @tc.type: FUNC
 * @tc.require: issue41
 */
HWTEST_F(RSUniRenderProcessorTest, ProcessScreenSurface_FingerprintFalseTest001, TestSize.Level2)
{
    if (RSUniRenderJudgement::IsUniRender()) {
        NodeId nodeId = 2;
        auto node = std::make_shared<RSScreenRenderNode>(nodeId, screenId_);
        node->SetFingerprint(false);  // Explicitly set to false

        // Initialize render drawable
        auto drawable = node->GetRenderDrawable();
        if (drawable == nullptr) {
            drawable = DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(
                std::static_pointer_cast<RSRenderNode>(node));
        }
        ASSERT_NE(drawable, nullptr);

        auto screenDrawable = std::static_pointer_cast<DrawableV2::RSScreenRenderNodeDrawable>(drawable);
        ASSERT_NE(screenDrawable, nullptr);

        // Set surfaceHandler
        screenDrawable->surfaceHandler_ = std::make_shared<RSSurfaceHandler>(0);
        ASSERT_NE(screenDrawable->surfaceHandler_, nullptr);

        // Create consumer and call CreateSurface to set surfaceCreated_ = true
        // Use a simple test listener since RSRenderServiceListener requires RSSurfaceRenderNode
        sptr<IBufferConsumerListener> listener = new TestBufferConsumerListener();
        screenDrawable->CreateSurface(listener);
        ASSERT_TRUE(screenDrawable->IsSurfaceCreated());

        // Request and flush buffer
        auto consumer = screenDrawable->surfaceHandler_->GetConsumer();
        ASSERT_NE(consumer, nullptr);
        auto producer = consumer->GetProducer();
        auto surface = Surface::CreateSurfaceAsProducer(producer);
        ASSERT_NE(surface, nullptr);
        surface->SetQueueSize(1);

        sptr<SurfaceBuffer> buffer;
        sptr<SyncFence> requestFence = SyncFence::INVALID_FENCE;
        [[maybe_unused]] GSError ret = surface->RequestBuffer(buffer, requestFence, requestConfig);
        ASSERT_EQ(ret, GSERROR_OK);

        sptr<SyncFence> flushFence = SyncFence::INVALID_FENCE;
        BufferFlushConfig flushConfig = { .damage = { .w = 200, .h = 200, } };
        ret = surface->FlushBuffer(buffer, flushFence, flushConfig);
        ASSERT_EQ(ret, GSERROR_OK);

        // Acquire buffer to consumer
        OHOS::sptr<SurfaceBuffer> cbuffer;
        Rect damage;
        sptr<SyncFence> acquireFence = SyncFence::INVALID_FENCE;
        int64_t timestamp = 0;
        ret = consumer->AcquireBuffer(cbuffer, acquireFence, timestamp, damage);
        ASSERT_EQ(ret, GSERROR_OK);
        ASSERT_NE(cbuffer, nullptr);

        screenDrawable->surfaceHandler_->SetBuffer(cbuffer, acquireFence, damage, timestamp, nullptr);

        // Call ProcessScreenSurface with Fingerprint = false
        auto layerSize = renderProcessor->layers_.size();
        EXPECT_NO_FATAL_FAILURE(renderProcessor->ProcessScreenSurface(*node));

        // Verify that a layer was created
        EXPECT_GT(renderProcessor->layers_.size(), layerSize);
    }
}

/**
 * @tc.name: CreateLayerForRenderThread001
 * @tc.desc: Test RSUniRenderProcessorTest.CreateLayerForRenderThread with nullptr
 * @tc.type:FUNC
 * @tc.require: issueIAIT5Z
 */
HWTEST_F(RSUniRenderProcessorTest, CreateLayerForRenderThread001, TestSize.Level1)
{
    if (RSUniRenderJudgement::IsUniRender()) {
        auto node = RSTestUtil::CreateSurfaceNodeWithBuffer();
        ASSERT_NE(node, nullptr);
        auto surfaceDrawable = std::static_pointer_cast<DrawableV2::RSSurfaceRenderNodeDrawable>(
            DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(node));
        ASSERT_NE(surfaceDrawable, nullptr);

        surfaceDrawable->renderParams_ = nullptr;
        ASSERT_NE(renderProcessor, nullptr);
        renderProcessor->CreateLayerForRenderThread(*surfaceDrawable);
        surfaceDrawable->renderParams_= std::make_unique<RSRenderParams>(0);
        renderProcessor->CreateLayerForRenderThread(*surfaceDrawable);
    }
}

/**
 * @tc.name: CreateLayerForRenderThread002
 * @tc.desc: Test RSUniRenderProcessorTest.CreateLayerForRenderThread with not nullptr
 * @tc.type:FUNC
 * @tc.require: issueIAIT5Z
 */
HWTEST_F(RSUniRenderProcessorTest, CreateLayerForRenderThread002, TestSize.Level1)
{
    if (RSUniRenderJudgement::IsUniRender()) {
        auto node = RSTestUtil::CreateSurfaceNodeWithBuffer();
        ASSERT_NE(node, nullptr);
        auto surfaceDrawable = std::static_pointer_cast<DrawableV2::RSSurfaceRenderNodeDrawable>(
            DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(node));
        ASSERT_NE(surfaceDrawable->renderParams_, nullptr);
        sptr<SurfaceBuffer> buffer = OHOS::SurfaceBuffer::Create();
        surfaceDrawable->renderParams_->SetBuffer(buffer, {}, DEFAULT_RECT);
        ASSERT_NE(renderProcessor, nullptr);
        renderProcessor->composerClient_ = nullptr;
        renderProcessor->CreateLayerForRenderThread(*surfaceDrawable);

        NodeId nodeId = 1;
        RSScreenRenderNode screenNode(nodeId, screenId_);
        auto renderEngine = std::make_shared<RSUniRenderEngine>();
        renderProcessor->Init(screenNode, renderEngine);
        renderProcessor->CreateLayerForRenderThread(*surfaceDrawable);
    }
}

/**
 * @tc.name: GetForceClientForDRM001
 * @tc.desc: Test RSUniRenderProcessorTest.GetForceClientForDRM
 * @tc.type:FUNC
 * @tc.require: issueIAIT5Z
 */
HWTEST_F(RSUniRenderProcessorTest, GetForceClientForDRM001, TestSize.Level1)
{
    if (RSUniRenderJudgement::IsUniRender()) {
        ASSERT_NE(renderProcessor, nullptr);
        RSSurfaceRenderParams params(0);
        params.GetMultableSpecialLayerMgr().Set(SpecialLayerType::PROTECTED, false);
        ASSERT_FALSE(renderProcessor->GetForceClientForDRM(params));
    }
}

/**
 * @tc.name: GetForceClientForDRM002
 * @tc.desc: Test RSUniRenderProcessorTest.GetForceClientForDRM
 * @tc.type:FUNC
 * @tc.require: issueIAIT5Z
 */
HWTEST_F(RSUniRenderProcessorTest, GetForceClientForDRM002, TestSize.Level1)
{
    if (RSUniRenderJudgement::IsUniRender()) {
        ASSERT_NE(renderProcessor, nullptr);
        RSSurfaceRenderParams params(0);
        params.GetMultableSpecialLayerMgr().Set(SpecialLayerType::PROTECTED, true);
        params.animateState_ = true;
        ASSERT_TRUE(renderProcessor->GetForceClientForDRM(params));
    }
}

/**
 * @tc.name: GetForceClientForDRM003
 * @tc.desc: Test RSUniRenderProcessorTest.GetForceClientForDRM
 * @tc.type:FUNC
 * @tc.require: issueIAIT5Z
 */
HWTEST_F(RSUniRenderProcessorTest, GetForceClientForDRM003, TestSize.Level1)
{
    if (RSUniRenderJudgement::IsUniRender()) {
        ASSERT_NE(renderProcessor, nullptr);
        RSSurfaceRenderParams params(0);
        params.GetMultableSpecialLayerMgr().Set(SpecialLayerType::PROTECTED, true);
        params.animateState_ = false;
        ASSERT_FALSE(renderProcessor->GetForceClientForDRM(params));
        // set totalMatrix to 30 degrees
        params.totalMatrix_.PostRotate(30.0f);
        ASSERT_TRUE(renderProcessor->GetForceClientForDRM(params));
    }
}

/**
 * @tc.name: GetForceClientForDRM004
 * @tc.desc: Test RSUniRenderProcessorTest.GetForceClientForDRM
 * @tc.type:FUNC
 * @tc.require: issueIAIT5Z
 */
HWTEST_F(RSUniRenderProcessorTest, GetForceClientForDRM004, TestSize.Level1)
{
    NodeId nodeId = 1;
    ScreenId screenId = 0;
    std::weak_ptr<RSContext> context = {};
    if (RSUniRenderJudgement::IsUniRender()) {
        ASSERT_NE(renderProcessor, nullptr);
        RSSurfaceRenderParams params(0);
        params.GetMultableSpecialLayerMgr().Set(SpecialLayerType::PROTECTED, true);
        params.animateState_ = false;
        auto node = std::make_shared<RSScreenRenderNode>(nodeId, screenId, context);
        std::shared_ptr<DrawableV2::RSScreenRenderNodeDrawable> screenDrawable(
            static_cast<DrawableV2::RSScreenRenderNodeDrawable*>(
            DrawableV2::RSScreenRenderNodeDrawable::OnGenerate(node)));
        ASSERT_NE(screenDrawable, nullptr);
        params.ancestorScreenDrawable_ = screenDrawable;
        screenDrawable->renderParams_ = std::make_unique<RSScreenRenderParams>(nodeId);
        ASSERT_NE(screenDrawable->GetRenderParams(), nullptr);
        ASSERT_FALSE(renderProcessor->GetForceClientForDRM(params));
    }
}

/**
 * @tc.name: HandleTunnelLayerParameters001
 * @tc.desc: Test RSUniRenderProcessorTest.HandleTunnelLayerParameters
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSUniRenderProcessorTest, HandleTunnelLayerParameters001, TestSize.Level1)
{
    ASSERT_NE(renderProcessor, nullptr);
    RSSurfaceRenderParams params(0);

    RSLayerPtr layer = nullptr;
    params.SetTunnelLayerId(0);
    renderProcessor->HandleTunnelLayerParameters(params, layer);

    params.SetTunnelLayerId(0);
    renderProcessor->HandleTunnelLayerParameters(params, layer);

    layer = std::make_shared<RSSurfaceLayer>(0, nullptr);
    renderProcessor->HandleTunnelLayerParameters(params, layer);

    ASSERT_EQ(layer->GetTunnelLayerId(), params.GetTunnelLayerId());
}

/**
 * @tc.name: GetLayerInfo001
 * @tc.desc: Test RSUniRenderProcessorTest.GetLayerInfo
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSUniRenderProcessorTest, GetLayerInfo001, TestSize.Level1)
{
    ASSERT_NE(renderProcessor, nullptr);
    RSSurfaceRenderParams params(0);
    params.SetTunnelLayerId(1);
    sptr<SurfaceBuffer> buffer = nullptr;
    sptr<SurfaceBuffer> preBuffer = nullptr;
    sptr<IConsumerSurface> consumer = nullptr;
    sptr<SyncFence> acquireFence = nullptr;
    EXPECT_EQ(params.GetTunnelLayerId(), 1);
    RSLayerPtr result = renderProcessor->GetLayerInfo(params, buffer, preBuffer, consumer, acquireFence);
    EXPECT_EQ(result, nullptr);
}

/**
 * @tc.name: GetLayerInfo002
 * @tc.desc: Test RSUniRenderProcessorTest.GetLayerInfo
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSUniRenderProcessorTest, GetLayerInfo002, TestSize.Level1)
{
    ASSERT_NE(renderProcessor, nullptr);
    RSSurfaceRenderParams params(0);
    params.SetTunnelLayerId(1);
    params.SetHwcGlobalPositionEnabled(true);
    sptr<SurfaceBuffer> buffer = nullptr;
    sptr<SurfaceBuffer> preBuffer = nullptr;
    sptr<IConsumerSurface> consumer = IConsumerSurface::Create("test");
    sptr<SyncFence> acquireFence = nullptr;
    auto composerClient = RSComposerClient::Create(nullptr, nullptr);
    renderProcessor->composerClient_ = composerClient;
    RSLayerPtr result = renderProcessor->GetLayerInfo(params, buffer, preBuffer, consumer, acquireFence);
    EXPECT_EQ(result->GetType(), GraphicLayerType::GRAPHIC_LAYER_TYPE_TUNNEL);
}

/**
 * @tc.name: GetLayerInfo003
 * @tc.desc: Test RSUniRenderProcessorTest.GetLayerInfo
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSUniRenderProcessorTest, GetLayerInfo003, TestSize.Level1)
{
    ScreenInfo screenInfo;
    screenInfo.isSamplingOn = false;

    ASSERT_NE(renderProcessor, nullptr);
    renderProcessor->screenInfo_ = screenInfo;
    RSSurfaceRenderParams params(0);
    params.SetTunnelLayerId(1);
    params.SetHwcGlobalPositionEnabled(true);
    sptr<SurfaceBuffer> buffer = nullptr;
    sptr<SurfaceBuffer> preBuffer = nullptr;
    sptr<IConsumerSurface> consumer = IConsumerSurface::Create("test");
    sptr<SyncFence> acquireFence = nullptr;
    auto composerClient = RSComposerClient::Create(nullptr, nullptr);
    renderProcessor->composerClient_ = composerClient;
    RSLayerPtr result = renderProcessor->GetLayerInfo(params, buffer, preBuffer, consumer, acquireFence);
    EXPECT_EQ(result->GetType(), GraphicLayerType::GRAPHIC_LAYER_TYPE_TUNNEL);
}

/**
 * @tc.name: GetLayerInfo004
 * @tc.desc: Test RSUniRenderProcessorTest.GetLayerInfo
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSUniRenderProcessorTest, GetLayerInfo004, TestSize.Level1)
{
    ScreenInfo screenInfo;
    screenInfo.isSamplingOn = true;

    ASSERT_NE(renderProcessor, nullptr);
    renderProcessor->screenInfo_ = screenInfo;
    RSSurfaceRenderParams params(0);
    params.SetTunnelLayerId(1);
    params.SetHwcGlobalPositionEnabled(true);
    sptr<SurfaceBuffer> buffer = nullptr;
    sptr<SurfaceBuffer> preBuffer = nullptr;
    sptr<IConsumerSurface> consumer = IConsumerSurface::Create("test");
    sptr<SyncFence> acquireFence = nullptr;
    auto composerClient = RSComposerClient::Create(nullptr, nullptr);
    renderProcessor->composerClient_ = composerClient;
    RSLayerPtr result = renderProcessor->GetLayerInfo(params, buffer, preBuffer, consumer, acquireFence);
    EXPECT_EQ(result->GetType(), GraphicLayerType::GRAPHIC_LAYER_TYPE_TUNNEL);
}

/**
 * @tc.name: GetLayerInfo005
 * @tc.desc: Test RSUniRenderProcessorTest.GetLayerInfo
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSUniRenderProcessorTest, GetLayerInfo005, TestSize.Level1)
{
    ScreenInfo screenInfo;
    screenInfo.isSamplingOn = false;

    ASSERT_NE(renderProcessor, nullptr);
    renderProcessor->screenInfo_ = screenInfo;
    RSSurfaceRenderParams params(0);
    params.SetTunnelLayerId(1);
    params.SetHwcGlobalPositionEnabled(true);
    params.GetMultableSpecialLayerMgr().Set(SpecialLayerType::PROTECTED, true);
    sptr<SurfaceBuffer> buffer = nullptr;
    sptr<SurfaceBuffer> preBuffer = nullptr;
    sptr<IConsumerSurface> consumer = IConsumerSurface::Create("test");
    sptr<SyncFence> acquireFence = nullptr;
    auto composerClient = RSComposerClient::Create(nullptr, nullptr);
    renderProcessor->composerClient_ = composerClient;
    RSLayerPtr result = renderProcessor->GetLayerInfo(params, buffer, preBuffer, consumer, acquireFence);
    EXPECT_EQ(result->GetType(), GraphicLayerType::GRAPHIC_LAYER_TYPE_TUNNEL);
}

/**
 * @tc.name: GetLayerInfo006
 * @tc.desc: Test RSUniRenderProcessorTest.GetLayerInfo when layer have selfDrawingDirtyRegion
 * @tc.type:FUNC
 * @tc.require: issuesICA3L1
 */
HWTEST_F(RSUniRenderProcessorTest, GetLayerInfo006, TestSize.Level1)
{
    ASSERT_NE(renderProcessor, nullptr);
    auto composerClient = RSComposerClient::Create(nullptr, nullptr);
    renderProcessor->composerClient_ = composerClient;
    RSSurfaceRenderParams params(0);
    params.SetTunnelLayerId(1);
    sptr<SurfaceBuffer> preBuffer = nullptr;
    sptr<IConsumerSurface> consumer = IConsumerSurface::Create("test");
    sptr<SyncFence> acquireFence = nullptr;
    auto buffer = SurfaceBuffer::Create();
    auto ret = buffer->Alloc(RSUniRenderProcessorTest::requestConfig);
    ASSERT_EQ(ret, GSERROR_OK);

    auto src = RSGpuDirtyCollector::GetBufferSelfDrawingData(buffer);
    ASSERT_EQ(src, nullptr);

    EXPECT_EQ(params.GetTunnelLayerId(), 1);
    params.SetBuffer(buffer, nullptr, DEFAULT_RECT);
    auto param = system::GetParameter("rosen.graphic.selfdrawingdirtyregion.enabled", "");
    system::SetParameter("rosen.graphic.selfdrawingdirtyregion.enabled", "1");
    RSLayerPtr result = renderProcessor->GetLayerInfo(params, buffer, preBuffer, consumer, acquireFence);
    EXPECT_EQ(result->GetType(), GraphicLayerType::GRAPHIC_LAYER_TYPE_TUNNEL);
    system::SetParameter("rosen.graphic.selfdrawingdirtyregion.enabled", param);
    result = renderProcessor->GetLayerInfo(params, buffer, preBuffer, consumer, acquireFence);
    EXPECT_EQ(result->GetType(), GraphicLayerType::GRAPHIC_LAYER_TYPE_TUNNEL);
}

/**
 * @tc.name: CreateSolidColorLayerTest
 * @tc.desc: Test CreateSolidColorLayer
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSUniRenderProcessorTest, CreateSolidColorLayerTest, TestSize.Level1)
{
    RSLayerPtr layer = std::make_shared<RSSurfaceLayer>(0, nullptr);
    layer->SetZorder(5);
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    auto surfaceDrawable = std::static_pointer_cast<DrawableV2::RSSurfaceRenderNodeDrawable>(
        DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(surfaceNode));
    auto params = static_cast<RSSurfaceRenderParams*>(surfaceDrawable->renderParams_.get());
    params->SetIsHwcEnabledBySolidLayer(true);
    params->SetSolidLayerColor(Color(255, 0, 0, 128));
    RSLayerInfo layerInfo;
    layerInfo.dstRect = {10, 10, 100, 100};
    params->SetLayerInfo(layerInfo);
    ASSERT_NE(renderProcessor, nullptr);
    renderProcessor->composerClient_ = nullptr;
    renderProcessor->CreateSolidColorLayer(layer, *params);

    NodeId nodeId = 1;
    RSScreenRenderNode screenNode(nodeId, screenId_);
    auto renderEngine = std::make_shared<RSUniRenderEngine>();
    renderProcessor->Init(screenNode, renderEngine);
    renderProcessor->CreateSolidColorLayer(layer, *params);
    auto property = layer->GetSolidColorLayerProperty();
    ASSERT_EQ(property.zOrder, layer->GetZorder() - 1);
}

/**
 * @tc.name: CreateSolidColorLayer001
 * @tc.desc: Test CreateSolidColorLayer when GetIsHwcEnabledBySolidLayer returns true (layer created)
 * @tc.type:FUNC
 * @tc.require: issuesI9KRF1
 */
HWTEST_F(RSUniRenderProcessorTest, CreateSolidColorLayer001, TestSize.Level1)
{
    if (RSUniRenderJudgement::IsUniRender()) {
        auto ctx = std::make_shared<RSComposerContext>(nullptr);
        RSLayerPtr layer = RSSurfaceLayer::Create(0, ctx);
        ASSERT_NE(layer, nullptr);
        layer->SetZorder(5);
        auto surfaceNode = RSTestUtil::CreateSurfaceNode();
        auto surfaceDrawable = std::static_pointer_cast<DrawableV2::RSSurfaceRenderNodeDrawable>(
            DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(surfaceNode));
        auto params = static_cast<RSSurfaceRenderParams*>(surfaceDrawable->GetRenderParams().get());

        // Set IsHwcEnabledBySolidLayer to true - should create solid color layer
        params->SetIsHwcEnabledBySolidLayer(true);
        params->SetSolidLayerColor(Color(255, 0, 0, 128));
        RSLayerInfo layerInfo;
        layerInfo.dstRect = {10, 10, 100, 100};
        params->SetLayerInfo(layerInfo);

        ASSERT_NE(renderProcessor, nullptr);
        NodeId nodeId = 1;
        RSScreenRenderNode screenNode(nodeId, screenId_);
        auto renderEngine = std::make_shared<RSUniRenderEngine>();
        renderProcessor->Init(screenNode, renderEngine);

        renderProcessor->CreateSolidColorLayer(layer, *params);
        auto property = layer->GetSolidColorLayerProperty();
        ASSERT_EQ(property.zOrder, layer->GetZorder() - 1);
    }
}

/**
 * @tc.name: CreateSolidColorLayer002
 * @tc.desc: Test CreateSolidColorLayer when composerClient is nullptr (layer creation fails)
 * @tc.type:FUNC
 * @tc.require: issuesI9KRF1
 */
HWTEST_F(RSUniRenderProcessorTest, CreateSolidColorLayer002, TestSize.Level1)
{
    if (RSUniRenderJudgement::IsUniRender()) {
        auto ctx = std::make_shared<RSComposerContext>(nullptr);
        RSLayerPtr layer = RSSurfaceLayer::Create(0, ctx);
        ASSERT_NE(layer, nullptr);
        layer->SetZorder(5);
        auto surfaceNode = RSTestUtil::CreateSurfaceNode();
        auto surfaceDrawable = std::static_pointer_cast<DrawableV2::RSSurfaceRenderNodeDrawable>(
            DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(surfaceNode));
        auto params = static_cast<RSSurfaceRenderParams*>(surfaceDrawable->GetRenderParams().get());

        // Set IsHwcEnabledBySolidLayer to true (should create layer, but will fail due to null composerClient)
        params->SetIsHwcEnabledBySolidLayer(true);
        params->SetSolidLayerColor(Color(0, 255, 0, 200));

        // Create processor with null composerClient
        auto processor = std::make_shared<RSUniRenderProcessor>();
        processor->composerClient_ = nullptr;

        processor->CreateSolidColorLayer(layer, *params);
        auto property = layer->GetSolidColorLayerProperty();
        ASSERT_EQ(property.zOrder, layer->GetZorder() - 1);
    }
}

/**
 * @tc.name: CreateSolidColorLayer003
 * @tc.desc: Test CreateSolidColorLayer with normal conditions and zorder > 0
 * @tc.type:FUNC
 * @tc.require: issuesI9KRF1
 */
HWTEST_F(RSUniRenderProcessorTest, CreateSolidColorLayer003, TestSize.Level1)
{
    if (RSUniRenderJudgement::IsUniRender()) {
        auto ctx = std::make_shared<RSComposerContext>(nullptr);
        RSLayerPtr layer = RSSurfaceLayer::Create(0, ctx);
        ASSERT_NE(layer, nullptr);
        layer->SetZorder(5);
        auto surfaceNode = RSTestUtil::CreateSurfaceNode();
        auto surfaceDrawable = std::static_pointer_cast<DrawableV2::RSSurfaceRenderNodeDrawable>(
            DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(surfaceNode));
        auto params = static_cast<RSSurfaceRenderParams*>(surfaceDrawable->GetRenderParams().get());

        // Set IsHwcEnabledBySolidLayer to true and set color
        params->SetIsHwcEnabledBySolidLayer(true);
        params->SetSolidLayerColor(Color(128, 128, 128, 255));
        RSLayerInfo layerInfo;
        layerInfo.dstRect = {10, 10, 100, 100};
        params->SetLayerInfo(layerInfo);

        ASSERT_NE(renderProcessor, nullptr);
        NodeId nodeId = 1;
        RSScreenRenderNode screenNode(nodeId, screenId_);
        auto renderEngine = std::make_shared<RSUniRenderEngine>();
        renderProcessor->Init(screenNode, renderEngine);

        renderProcessor->CreateSolidColorLayer(layer, *params);
        auto property = layer->GetSolidColorLayerProperty();
        ASSERT_EQ(property.zOrder, layer->GetZorder() - 1);
    }
}

/**
 * @tc.name: CreateSolidColorLayer004
 * @tc.desc: Test CreateSolidColorLayer when zorder is 0 (boundary condition)
 * @tc.type:FUNC
 * @tc.require: issuesI9KRF1
 */
HWTEST_F(RSUniRenderProcessorTest, CreateSolidColorLayer004, TestSize.Level1)
{
    if (RSUniRenderJudgement::IsUniRender()) {
        auto ctx = std::make_shared<RSComposerContext>(nullptr);
        RSLayerPtr layer = RSSurfaceLayer::Create(0, ctx);
        ASSERT_NE(layer, nullptr);
        layer->SetZorder(0);
        auto surfaceNode = RSTestUtil::CreateSurfaceNode();
        auto surfaceDrawable = std::static_pointer_cast<DrawableV2::RSSurfaceRenderNodeDrawable>(
            DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(surfaceNode));
        auto params = static_cast<RSSurfaceRenderParams*>(surfaceDrawable->GetRenderParams().get());

        // Set IsHwcEnabledBySolidLayer to true
        params->SetIsHwcEnabledBySolidLayer(true);
        params->SetSolidLayerColor(Color(255, 255, 0, 100));
        RSLayerInfo layerInfo;
        layerInfo.dstRect = {10, 10, 100, 100};
        params->SetLayerInfo(layerInfo);

        ASSERT_NE(renderProcessor, nullptr);
        NodeId nodeId = 1;
        RSScreenRenderNode screenNode(nodeId, screenId_);
        auto renderEngine = std::make_shared<RSUniRenderEngine>();
        renderProcessor->Init(screenNode, renderEngine);

        renderProcessor->CreateSolidColorLayer(layer, *params);
        auto property = layer->GetSolidColorLayerProperty();
        ASSERT_NE(property.zOrder, layer->GetZorder() - 1);
    }
}

/**
 * @tc.name: CreateSolidColorLayer005
 * @tc.desc: Test CreateSolidColorLayer when zorder is negative (boundary condition)
 * @tc.type:FUNC
 * @tc.require: issuesI9KRF1
 */
HWTEST_F(RSUniRenderProcessorTest, CreateSolidColorLayer005, TestSize.Level1)
{
    if (RSUniRenderJudgement::IsUniRender()) {
        auto ctx = std::make_shared<RSComposerContext>(nullptr);
        RSLayerPtr layer = RSSurfaceLayer::Create(0, ctx);
        ASSERT_NE(layer, nullptr);
        layer->SetZorder(-1);
        auto surfaceNode = RSTestUtil::CreateSurfaceNode();
        auto surfaceDrawable = std::static_pointer_cast<DrawableV2::RSSurfaceRenderNodeDrawable>(
            DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(surfaceNode));
        auto params = static_cast<RSSurfaceRenderParams*>(surfaceDrawable->GetRenderParams().get());

        // Set IsHwcEnabledBySolidLayer to true
        params->SetIsHwcEnabledBySolidLayer(true);
        params->SetSolidLayerColor(Color(0, 0, 255, 180));
        RSLayerInfo layerInfo;
        layerInfo.dstRect = {10, 10, 100, 100};
        params->SetLayerInfo(layerInfo);

        ASSERT_NE(renderProcessor, nullptr);
        NodeId nodeId = 1;
        RSScreenRenderNode screenNode(nodeId, screenId_);
        auto renderEngine = std::make_shared<RSUniRenderEngine>();
        renderProcessor->Init(screenNode, renderEngine);

        renderProcessor->CreateSolidColorLayer(layer, *params);
        auto property = layer->GetSolidColorLayerProperty();
        ASSERT_EQ(property.zOrder, layer->GetZorder() - 1);
    }
}

/**
 * @tc.name: CreateSolidColorLayer006
 * @tc.desc: Test CreateSolidColorLayer with various color values
 * @tc.type:FUNC
 * @tc.require: issuesI9KRF1
 */
HWTEST_F(RSUniRenderProcessorTest, CreateSolidColorLayer006, TestSize.Level1)
{
    if (RSUniRenderJudgement::IsUniRender()) {
        struct ColorTestCase {
            uint32_t argb;
            std::string name;
        };
        std::vector<ColorTestCase> testColors = {
            {0xFF000000, "Black"},
            {0xFFFFFFFF, "White"},
            {0xFFFF0000, "Red"},
            {0xFF00FF00, "Green"},
            {0xFF0000FF, "Blue"},
            {0x80FFFFFF, "Semi-transparent white"}
        };

        ASSERT_NE(renderProcessor, nullptr);
        NodeId nodeId = 1;
        RSScreenRenderNode screenNode(nodeId, screenId_);
        auto renderEngine = std::make_shared<RSUniRenderEngine>();
        renderProcessor->Init(screenNode, renderEngine);

        for (const auto& colorCase : testColors) {
            // Create a new layer for each iteration
            auto ctx = std::make_shared<RSComposerContext>(nullptr);
            RSLayerPtr layer = RSSurfaceLayer::Create(0, ctx);
            ASSERT_NE(layer, nullptr);
            layer->SetZorder(3);

            auto surfaceNode = RSTestUtil::CreateSurfaceNode();
            auto surfaceDrawable = std::static_pointer_cast<DrawableV2::RSSurfaceRenderNodeDrawable>(
                DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(surfaceNode));
            auto params = static_cast<RSSurfaceRenderParams*>(surfaceDrawable->GetRenderParams().get());

            params->SetIsHwcEnabledBySolidLayer(true);
            Color color(colorCase.argb);
            params->SetSolidLayerColor(color);
            RSLayerInfo layerInfo;
            layerInfo.dstRect = {10, 10, 100, 100};
            params->SetLayerInfo(layerInfo);

            renderProcessor->CreateSolidColorLayer(layer, *params);
            auto property = layer->GetSolidColorLayerProperty();
            ASSERT_EQ(property.zOrder, layer->GetZorder() - 1);
        }
    }
}

/**
 * @tc.name: CreateSolidColorLayer007
 * @tc.desc: Test CreateSolidColorLayer with different dstRect values
 * @tc.type:FUNC
 * @tc.require: issuesI9KRF1
 */
HWTEST_F(RSUniRenderProcessorTest, CreateSolidColorLayer007, TestSize.Level1)
{
    if (RSUniRenderJudgement::IsUniRender()) {
        struct RectTestCase {
            GraphicIRect dstRect;
            std::string name;
        };
        std::vector<RectTestCase> testRects = {
            {{0, 0, 100, 100}, "Top-left origin"},
            {{1920, 1080, 100, 100}, "Bottom-right area"},
            {{-100, -100, 50, 50}, "Negative coordinates"},
            {{0, 0, 1, 1}, "Minimum size"},
            {{0, 0, 3840, 2160}, "Full HD size"}
        };

        ASSERT_NE(renderProcessor, nullptr);
        NodeId nodeId = 1;
        RSScreenRenderNode screenNode(nodeId, screenId_);
        auto renderEngine = std::make_shared<RSUniRenderEngine>();
        renderProcessor->Init(screenNode, renderEngine);

        for (const auto& rectCase : testRects) {
            // Create a new layer for each iteration
            auto ctx = std::make_shared<RSComposerContext>(nullptr);
            RSLayerPtr layer = RSSurfaceLayer::Create(0, ctx);
            ASSERT_NE(layer, nullptr);
            layer->SetZorder(2);

            auto surfaceNode = RSTestUtil::CreateSurfaceNode();
            auto surfaceDrawable = std::static_pointer_cast<DrawableV2::RSSurfaceRenderNodeDrawable>(
                DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(surfaceNode));
            auto params = static_cast<RSSurfaceRenderParams*>(surfaceDrawable->GetRenderParams().get());

            params->SetIsHwcEnabledBySolidLayer(true);
            params->SetSolidLayerColor(Color(100, 150, 200, 255));
            RSLayerInfo layerInfo;
            layerInfo.dstRect = rectCase.dstRect;
            params->SetLayerInfo(layerInfo);

            renderProcessor->CreateSolidColorLayer(layer, *params);
            auto property = layer->GetSolidColorLayerProperty();
            ASSERT_EQ(property.zOrder, layer->GetZorder() - 1);
        }
    }
}

/**
 * @tc.name: CreateSolidColorLayer008
 * @tc.desc: Test CreateSolidColorLayer with layer pointer nullptr
 * @tc.type:FUNC
 * @tc.require: issuesI9KRF1
 */
HWTEST_F(RSUniRenderProcessorTest, CreateSolidColorLayer008, TestSize.Level1)
{
    if (RSUniRenderJudgement::IsUniRender()) {
        RSLayerPtr layer = nullptr;
        auto surfaceNode = RSTestUtil::CreateSurfaceNode();
        auto surfaceDrawable = std::static_pointer_cast<DrawableV2::RSSurfaceRenderNodeDrawable>(
            DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(surfaceNode));
        auto params = static_cast<RSSurfaceRenderParams*>(surfaceDrawable->GetRenderParams().get());

        params->SetIsHwcEnabledBySolidLayer(false);
        params->SetSolidLayerColor(Color(255, 0, 0, 128));

        ASSERT_NE(renderProcessor, nullptr);
        NodeId nodeId = 1;
        RSScreenRenderNode screenNode(nodeId, screenId_);
        auto renderEngine = std::make_shared<RSUniRenderEngine>();
        renderProcessor->Init(screenNode, renderEngine);

        // Should handle null layer gracefully
        renderProcessor->CreateSolidColorLayer(layer, *params);
    }
}

/**
 * @tc.name: PostProcess_UniLayerNullTest001
 * @tc.desc: Test PostProcess when uniLayer_ is nullptr
 *           The first if (uniLayer_) branch at line 103 should be false
 * @tc.type: FUNC
 * @tc.require: issue41
 */
HWTEST_F(RSUniRenderProcessorTest, PostProcess_UniLayerNullTest001, TestSize.Level2)
{
    if (RSUniRenderJudgement::IsUniRender()) {
        ASSERT_NE(renderProcessor, nullptr);
        // Ensure uniLayer_ is nullptr (default state after initialization)
        renderProcessor->uniLayer_ = nullptr;
        renderProcessor->layers_.clear();

        // Call PostProcess, should not crash and skip uniLayer_ branch
        EXPECT_NO_FATAL_FAILURE(renderProcessor->PostProcess());
    }
}

/**
 * @tc.name: PostProcess_UniBufferOwnerCountNullTest001
 * @tc.desc: Test PostProcess when uniBufferOwnerCount is nullptr
 *           When uniLayer_ exists but GetBufferOwnerCount() returns nullptr,
 *           the if (uniBufferOwnerCount) branch at line 106 should be false
 * @tc.type: FUNC
 * @tc.require: issue41
 */
HWTEST_F(RSUniRenderProcessorTest, PostProcess_UniBufferOwnerCountNullTest001, TestSize.Level2)
{
    if (RSUniRenderJudgement::IsUniRender()) {
        ASSERT_NE(renderProcessor, nullptr);

        // Create uniLayer_ without setting bufferOwnerCount
        auto ctx = std::make_shared<RSComposerContext>(nullptr);
        auto layer = RSSurfaceLayer::Create(0, ctx);
        ASSERT_NE(layer, nullptr);
        renderProcessor->uniLayer_ = layer;
        renderProcessor->layers_.clear();

        // Call PostProcess, uniBufferOwnerCount will be nullptr since no bufferOwnerCount is set
        EXPECT_NO_FATAL_FAILURE(renderProcessor->PostProcess());
    }
}

/**
 * @tc.name: PostProcess_LayerNullInLoopTest001
 * @tc.desc: Test PostProcess when layer pointer in layers_ is expired (weak_ptr.lock() returns nullptr)
 *           The if (layer == nullptr ...) branch at line 109 should be true
 * @tc.type: FUNC
 * @tc.require: issue41
 */
HWTEST_F(RSUniRenderProcessorTest, PostProcess_LayerNullInLoopTest001, TestSize.Level2)
{
    if (RSUniRenderJudgement::IsUniRender()) {
        ASSERT_NE(renderProcessor, nullptr);

        // Create uniLayer_ with bufferOwnerCount
        auto ctx = std::make_shared<RSComposerContext>(nullptr);
        auto uniLayer = RSSurfaceLayer::Create(0, ctx);
        ASSERT_NE(uniLayer, nullptr);
        auto owner = std::make_shared<RSSurfaceHandler::BufferOwnerCount>();
        owner->bufferId_ = 1000ULL;
        uniLayer->SetBufferOwnerCount(owner, true);
        renderProcessor->uniLayer_ = uniLayer;

        // Add an expired weak_ptr to layers_
        {
            auto tempLayer = RSSurfaceLayer::Create(1, ctx);
            renderProcessor->layers_.emplace_back(tempLayer);
            // tempLayer goes out of scope, weak_ptr becomes expired
        }

        // Call PostProcess, should skip expired layer
        EXPECT_NO_FATAL_FAILURE(renderProcessor->PostProcess());
    }
}

/**
 * @tc.name: PostProcess_LayerEqualsUniLayerTest001
 * @tc.desc: Test PostProcess when layer in layers_ is the same as uniLayer_
 *           The if (... layer == uniLayer_ ...) branch at line 109 should be true
 * @tc.type: FUNC
 * @tc.require: issue41
 */
HWTEST_F(RSUniRenderProcessorTest, PostProcess_LayerEqualsUniLayerTest001, TestSize.Level2)
{
    if (RSUniRenderJudgement::IsUniRender()) {
        ASSERT_NE(renderProcessor, nullptr);

        // Create uniLayer_ with bufferOwnerCount
        auto ctx = std::make_shared<RSComposerContext>(nullptr);
        auto uniLayer = RSSurfaceLayer::Create(0, ctx);
        ASSERT_NE(uniLayer, nullptr);
        auto owner = std::make_shared<RSSurfaceHandler::BufferOwnerCount>();
        owner->bufferId_ = 1000ULL;
        uniLayer->SetBufferOwnerCount(owner, true);
        renderProcessor->uniLayer_ = uniLayer;

        // Add same layer to layers_
        renderProcessor->layers_.emplace_back(uniLayer);

        // Call PostProcess, should skip layer that equals uniLayer_
        EXPECT_NO_FATAL_FAILURE(renderProcessor->PostProcess());
    }
}

/**
 * @tc.name: PostProcess_LayerBufferNullTest001
 * @tc.desc: Test PostProcess when layer->GetBuffer() returns nullptr
 *           The if (... layer->GetBuffer() == nullptr) branch at line 109 should be true
 * @tc.type: FUNC
 * @tc.require: issue41
 */
HWTEST_F(RSUniRenderProcessorTest, PostProcess_LayerBufferNullTest001, TestSize.Level2)
{
    if (RSUniRenderJudgement::IsUniRender()) {
        ASSERT_NE(renderProcessor, nullptr);

        // Create uniLayer_ with bufferOwnerCount
        auto ctx = std::make_shared<RSComposerContext>(nullptr);
        auto uniLayer = RSSurfaceLayer::Create(0, ctx);
        ASSERT_NE(uniLayer, nullptr);
        auto owner = std::make_shared<RSSurfaceHandler::BufferOwnerCount>();
        owner->bufferId_ = 1000ULL;
        uniLayer->SetBufferOwnerCount(owner, true);
        renderProcessor->uniLayer_ = uniLayer;

        // Add a layer without buffer to layers_
        auto layerWithoutBuffer = RSSurfaceLayer::Create(1, ctx);
        ASSERT_NE(layerWithoutBuffer, nullptr);
        renderProcessor->layers_.emplace_back(layerWithoutBuffer);

        // Call PostProcess, should skip layer without buffer
        EXPECT_NO_FATAL_FAILURE(renderProcessor->PostProcess());
    }
}

/**
 * @tc.name: PostProcess_BufferOwnerCountNullInLoopTest001
 * @tc.desc: Test PostProcess when layer->GetBufferOwnerCount() returns nullptr
 *           The if (bufferOwnerCount == nullptr) branch at line 114 should be true
 * @tc.type: FUNC
 * @tc.require: issue41
 */
HWTEST_F(RSUniRenderProcessorTest, PostProcess_BufferOwnerCountNullInLoopTest001, TestSize.Level2)
{
    if (RSUniRenderJudgement::IsUniRender()) {
        ASSERT_NE(renderProcessor, nullptr);

        // Create uniLayer_ with bufferOwnerCount
        auto ctx = std::make_shared<RSComposerContext>(nullptr);
        auto uniLayer = RSSurfaceLayer::Create(0, ctx);
        ASSERT_NE(uniLayer, nullptr);
        auto uniOwner = std::make_shared<RSSurfaceHandler::BufferOwnerCount>();
        uniOwner->bufferId_ = 1000ULL;
        uniLayer->SetBufferOwnerCount(uniOwner, true);
        renderProcessor->uniLayer_ = uniLayer;

        // Add a layer with buffer but without bufferOwnerCount to layers_
        auto layerWithoutOwner = RSSurfaceLayer::Create(1, ctx);
        ASSERT_NE(layerWithoutOwner, nullptr);
        auto buffer = SurfaceBuffer::Create();
        BufferRequestConfig cfg { 100, 100, 8, GRAPHIC_PIXEL_FMT_RGBA_8888,
            BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA, 0 };
        auto ret = buffer->Alloc(cfg);
        ASSERT_EQ(ret, GSERROR_OK);
        layerWithoutOwner->SetBuffer(buffer, nullptr);
        // Do NOT set bufferOwnerCount, so GetBufferOwnerCount() will return nullptr
        renderProcessor->layers_.emplace_back(layerWithoutOwner);

        // Call PostProcess, should skip layer without bufferOwnerCount
        EXPECT_NO_FATAL_FAILURE(renderProcessor->PostProcess());
    }
}

/**
 * @tc.name: PostProcess_AllBranchesCoveredTest001
 * @tc.desc: Test PostProcess with all branches covered
 *           Covers: uniLayer_ != nullptr, uniBufferOwnerCount != nullptr,
 *           layers_ with valid layer, buffer, and bufferOwnerCount
 * @tc.type: FUNC
 * @tc.require: issue41
 */
HWTEST_F(RSUniRenderProcessorTest, PostProcess_AllBranchesCoveredTest001, TestSize.Level2)
{
    if (RSUniRenderJudgement::IsUniRender()) {
        ASSERT_NE(renderProcessor, nullptr);

        // Create uniLayer_ with bufferOwnerCount
        auto ctx = std::make_shared<RSComposerContext>(nullptr);
        auto uniLayer = RSSurfaceLayer::Create(0, ctx);
        ASSERT_NE(uniLayer, nullptr);
        auto uniOwner = std::make_shared<RSSurfaceHandler::BufferOwnerCount>();
        uniOwner->bufferId_ = 1000ULL;
        uniLayer->SetBufferOwnerCount(uniOwner, true);
        renderProcessor->uniLayer_ = uniLayer;

        // Add a valid layer with buffer and bufferOwnerCount to layers_
        auto validLayer = RSSurfaceLayer::Create(1, ctx);
        ASSERT_NE(validLayer, nullptr);
        auto buffer = SurfaceBuffer::Create();
        BufferRequestConfig cfg { 100, 100, 8, GRAPHIC_PIXEL_FMT_RGBA_8888,
            BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA, 0 };
        auto ret = buffer->Alloc(cfg);
        ASSERT_EQ(ret, GSERROR_OK);
        validLayer->SetBuffer(buffer, nullptr);
        auto layerOwner = std::make_shared<RSSurfaceHandler::BufferOwnerCount>();
        layerOwner->bufferId_ = buffer->GetBufferId();
        validLayer->SetBufferOwnerCount(layerOwner, true);
        renderProcessor->layers_.emplace_back(validLayer);

        // Call PostProcess with all valid conditions
        EXPECT_NO_FATAL_FAILURE(renderProcessor->PostProcess());
    }
}

/**
 * @tc.name: PostProcess_MixedLayersTest001
 * @tc.desc: Test PostProcess with mixed layer conditions
 *           Including: null layer, same as uniLayer, null buffer, null bufferOwnerCount, and valid layer
 * @tc.type: FUNC
 * @tc.require: issue41
 */
HWTEST_F(RSUniRenderProcessorTest, PostProcess_MixedLayersTest001, TestSize.Level2)
{
    if (RSUniRenderJudgement::IsUniRender()) {
        ASSERT_NE(renderProcessor, nullptr);

        // Create uniLayer_ with bufferOwnerCount
        auto ctx = std::make_shared<RSComposerContext>(nullptr);
        auto uniLayer = RSSurfaceLayer::Create(0, ctx);
        ASSERT_NE(uniLayer, nullptr);
        auto uniOwner = std::make_shared<RSSurfaceHandler::BufferOwnerCount>();
        uniOwner->bufferId_ = 1000ULL;
        uniLayer->SetBufferOwnerCount(uniOwner, true);
        renderProcessor->uniLayer_ = uniLayer;
        renderProcessor->layers_.clear();

        // Add expired weak_ptr (null layer after lock)
        {
            auto tempLayer = RSSurfaceLayer::Create(1, ctx);
            renderProcessor->layers_.emplace_back(tempLayer);
        }

        // Add same as uniLayer
        renderProcessor->layers_.emplace_back(uniLayer);

        // Add layer without buffer
        auto layerNoBuffer = RSSurfaceLayer::Create(2, ctx);
        renderProcessor->layers_.emplace_back(layerNoBuffer);

        // Add layer without bufferOwnerCount
        auto layerNoOwner = RSSurfaceLayer::Create(3, ctx);
        auto buffer = SurfaceBuffer::Create();
        BufferRequestConfig cfg { 100, 100, 8, GRAPHIC_PIXEL_FMT_RGBA_8888,
            BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA, 0 };
        auto ret = buffer->Alloc(cfg);
        ASSERT_EQ(ret, GSERROR_OK);
        layerNoOwner->SetBuffer(buffer, nullptr);
        renderProcessor->layers_.emplace_back(layerNoOwner);

        // Add valid layer
        auto validLayer = RSSurfaceLayer::Create(4, ctx);
        auto buffer2 = SurfaceBuffer::Create();
        ret = buffer2->Alloc(cfg);
        ASSERT_EQ(ret, GSERROR_OK);
        validLayer->SetBuffer(buffer2, nullptr);
        auto layerOwner = std::make_shared<RSSurfaceHandler::BufferOwnerCount>();
        layerOwner->bufferId_ = buffer2->GetBufferId();
        validLayer->SetBufferOwnerCount(layerOwner, true);
        renderProcessor->layers_.emplace_back(validLayer);

        // Call PostProcess, should handle all cases
        EXPECT_NO_FATAL_FAILURE(renderProcessor->PostProcess());
    }
}

/**
 * @tc.name: CreateLayer_NullBufferTest001
 * @tc.desc: Test CreateLayer when buffer is nullptr
 *           The if (buffer == nullptr || consumer == nullptr) branch at line 131 should be true
 * @tc.type: FUNC
 * @tc.require: issue41
 */
HWTEST_F(RSUniRenderProcessorTest, CreateLayer_NullBufferTest001, TestSize.Level2)
{
    if (RSUniRenderJudgement::IsUniRender()) {
        auto surfaceNode = RSTestUtil::CreateSurfaceNode();
        auto surfaceDrawable = std::static_pointer_cast<DrawableV2::RSSurfaceRenderNodeDrawable>(
            DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(surfaceNode));
        auto params = static_cast<RSSurfaceRenderParams*>(surfaceDrawable->GetRenderParams().get());

        // Ensure buffer is nullptr
        sptr<SurfaceBuffer> nullBuffer = nullptr;
        params->SetBuffer(nullBuffer, {}, DEFAULT_RECT);

        NodeId nodeId = 1;
        RSScreenRenderNode screenNode(nodeId, screenId_);
        auto renderEngine = std::make_shared<RSUniRenderEngine>();
        renderProcessor->Init(screenNode, renderEngine);
        auto composerClient = RSComposerClient::Create(nullptr, nullptr);
        renderProcessor->composerClient_ = composerClient;

        // Call CreateLayer with null buffer, should return early
        EXPECT_NO_FATAL_FAILURE(renderProcessor->CreateLayer(*surfaceNode, *params));
        EXPECT_FALSE(params->GetLayerCreated());
    }
}

/**
 * @tc.name: CreateLayer_NullConsumerTest001
 * @tc.desc: Test CreateLayer when consumer is nullptr
 *           The if (buffer == nullptr || consumer == nullptr) branch at line 131 should be true
 * @tc.type: FUNC
 * @tc.require: issue41
 */
HWTEST_F(RSUniRenderProcessorTest, CreateLayer_NullConsumerTest001, TestSize.Level2)
{
    if (RSUniRenderJudgement::IsUniRender()) {
        auto surfaceNode = RSTestUtil::CreateSurfaceNode();
        // Do not request buffer, consumer will be nullptr

        auto surfaceDrawable = std::static_pointer_cast<DrawableV2::RSSurfaceRenderNodeDrawable>(
            DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(surfaceNode));
        auto params = static_cast<RSSurfaceRenderParams*>(surfaceDrawable->GetRenderParams().get());

        NodeId nodeId = 1;
        RSScreenRenderNode screenNode(nodeId, screenId_);
        auto renderEngine = std::make_shared<RSUniRenderEngine>();
        renderProcessor->Init(screenNode, renderEngine);
        auto composerClient = RSComposerClient::Create(nullptr, nullptr);
        renderProcessor->composerClient_ = composerClient;

        // Call CreateLayer with null consumer, should return early
        EXPECT_NO_FATAL_FAILURE(renderProcessor->CreateLayer(*surfaceNode, *params));
        EXPECT_FALSE(params->GetLayerCreated());
    }
}

/**
 * @tc.name: CreateLayer_NullLayerFromGetLayerInfoTest001
 * @tc.desc: Test CreateLayer when GetLayerInfo returns nullptr
 *           The if (layer == nullptr) branch at line 140 should be true
 * @tc.type: FUNC
 * @tc.require: issue41
 */
HWTEST_F(RSUniRenderProcessorTest, CreateLayer_NullLayerFromGetLayerInfoTest001, TestSize.Level2)
{
    if (RSUniRenderJudgement::IsUniRender()) {
        auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
        auto surfaceDrawable = std::static_pointer_cast<DrawableV2::RSSurfaceRenderNodeDrawable>(
            DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(surfaceNode));
        auto params = static_cast<RSSurfaceRenderParams*>(surfaceDrawable->GetRenderParams().get());

        // Set composerClient_ to nullptr so GetLayerInfo returns nullptr
        renderProcessor->composerClient_ = nullptr;

        // Call CreateLayer with null composerClient, GetLayerInfo will return nullptr
        EXPECT_NO_FATAL_FAILURE(renderProcessor->CreateLayer(*surfaceNode, *params));
        EXPECT_FALSE(params->GetLayerCreated());
    }
}

/**
 * @tc.name: CreateLayer_NullBufferOwnerCountTest001
 * @tc.desc: Test CreateLayer when bufferOwnerCount is nullptr
 *           The if (bufferOwnerCount) branch at line 150 should be false
 * @tc.type: FUNC
 * @tc.require: issue41
 */
HWTEST_F(RSUniRenderProcessorTest, CreateLayer_NullBufferOwnerCountTest001, TestSize.Level2)
{
    if (RSUniRenderJudgement::IsUniRender()) {
        auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
        auto surfaceDrawable = std::static_pointer_cast<DrawableV2::RSSurfaceRenderNodeDrawable>(
            DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(surfaceNode));
        auto params = static_cast<RSSurfaceRenderParams*>(surfaceDrawable->GetRenderParams().get());

        // Do NOT set bufferOwnerCount in params
        // params->GetBufferOwnerCount() will return nullptr

        NodeId nodeId = 1;
        RSScreenRenderNode screenNode(nodeId, screenId_);
        auto renderEngine = std::make_shared<RSUniRenderEngine>();
        renderProcessor->Init(screenNode, renderEngine);
        auto composerClient = RSComposerClient::Create(nullptr, nullptr);
        renderProcessor->composerClient_ = composerClient;

        // Call CreateLayer with null bufferOwnerCount
        EXPECT_NO_FATAL_FAILURE(renderProcessor->CreateLayer(*surfaceNode, *params));
        EXPECT_TRUE(params->GetLayerCreated());
    }
}

/**
 * @tc.name: CreateLayer_WithBufferOwnerCountTest001
 * @tc.desc: Test CreateLayer when bufferOwnerCount is not nullptr
 *           The if (bufferOwnerCount) branch at line 150 should be true
 * @tc.type: FUNC
 * @tc.require: issue41
 */
HWTEST_F(RSUniRenderProcessorTest, CreateLayer_WithBufferOwnerCountTest001, TestSize.Level2)
{
    if (RSUniRenderJudgement::IsUniRender()) {
        auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
        auto surfaceDrawable = std::static_pointer_cast<DrawableV2::RSSurfaceRenderNodeDrawable>(
            DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(surfaceNode));
        auto params = static_cast<RSSurfaceRenderParams*>(surfaceDrawable->GetRenderParams().get());

        // Set bufferOwnerCount in params
        auto bufferOwnerCount = std::make_shared<RSSurfaceHandler::BufferOwnerCount>();
        bufferOwnerCount->bufferId_ = 9999ULL;
        params->bufferOwnerCount_ = bufferOwnerCount;

        NodeId nodeId = 1;
        RSScreenRenderNode screenNode(nodeId, screenId_);
        auto renderEngine = std::make_shared<RSUniRenderEngine>();
        renderProcessor->Init(screenNode, renderEngine);
        auto composerClient = RSComposerClient::Create(nullptr, nullptr);
        renderProcessor->composerClient_ = composerClient;

        // Call CreateLayer with non-null bufferOwnerCount
        EXPECT_NO_FATAL_FAILURE(renderProcessor->CreateLayer(*surfaceNode, *params));
        EXPECT_TRUE(params->GetLayerCreated());
    }
}

/**
 * @tc.name: CreateLayer_WithOfflineResultTest001
 * @tc.desc: Test CreateLayer with offlineResult parameter
 *           Covers the offlineResult != nullptr path
 * @tc.type: FUNC
 * @tc.require: issue41
 */
HWTEST_F(RSUniRenderProcessorTest, CreateLayer_WithOfflineResultTest001, TestSize.Level2)
{
    if (RSUniRenderJudgement::IsUniRender()) {
        auto surfaceNode = RSTestUtil::CreateSurfaceNode();
        auto surfaceDrawable = std::static_pointer_cast<DrawableV2::RSSurfaceRenderNodeDrawable>(
            DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(surfaceNode));
        auto params = static_cast<RSSurfaceRenderParams*>(surfaceDrawable->GetRenderParams().get());

        // Create offlineResult with buffer and consumer
        auto offlineResult = std::make_shared<ProcessOfflineResult>();
        offlineResult->buffer = SurfaceBuffer::Create();
        BufferRequestConfig cfg { 100, 100, 8, GRAPHIC_PIXEL_FMT_RGBA_8888,
            BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA, 0 };
        auto ret = offlineResult->buffer->Alloc(cfg);
        ASSERT_EQ(ret, GSERROR_OK);
        offlineResult->consumer = IConsumerSurface::Create("test-offline");
        offlineResult->damageRect = {0, 0, 100, 100};
        offlineResult->bufferRect = {0, 0, 100, 100};
        offlineResult->taskSuccess = true;

        NodeId nodeId = 1;
        RSScreenRenderNode screenNode(nodeId, screenId_);
        auto renderEngine = std::make_shared<RSUniRenderEngine>();
        renderProcessor->Init(screenNode, renderEngine);
        auto composerClient = RSComposerClient::Create(nullptr, nullptr);
        renderProcessor->composerClient_ = composerClient;

        // Call CreateLayer with offlineResult
        EXPECT_NO_FATAL_FAILURE(renderProcessor->CreateLayer(*surfaceNode, *params, offlineResult));
        EXPECT_TRUE(params->GetLayerCreated());
    }
}

/**
 * @tc.name: CreateLayer_WithOfflineResultNullBufferTest001
 * @tc.desc: Test CreateLayer with offlineResult but buffer is nullptr
 *           The if (buffer == nullptr || consumer == nullptr) branch at line 131 should be true
 * @tc.type: FUNC
 * @tc.require: issue41
 */
HWTEST_F(RSUniRenderProcessorTest, CreateLayer_WithOfflineResultNullBufferTest001, TestSize.Level2)
{
    if (RSUniRenderJudgement::IsUniRender()) {
        auto surfaceNode = RSTestUtil::CreateSurfaceNode();
        auto surfaceDrawable = std::static_pointer_cast<DrawableV2::RSSurfaceRenderNodeDrawable>(
            DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(surfaceNode));
        auto params = static_cast<RSSurfaceRenderParams*>(surfaceDrawable->GetRenderParams().get());

        // Create offlineResult with null buffer
        auto offlineResult = std::make_shared<ProcessOfflineResult>();
        offlineResult->buffer = nullptr;
        offlineResult->consumer = IConsumerSurface::Create("test-offline");

        NodeId nodeId = 1;
        RSScreenRenderNode screenNode(nodeId, screenId_);
        auto renderEngine = std::make_shared<RSUniRenderEngine>();
        renderProcessor->Init(screenNode, renderEngine);
        auto composerClient = RSComposerClient::Create(nullptr, nullptr);
        renderProcessor->composerClient_ = composerClient;

        // Call CreateLayer with offlineResult null buffer, should return early
        EXPECT_NO_FATAL_FAILURE(renderProcessor->CreateLayer(*surfaceNode, *params, offlineResult));
        EXPECT_FALSE(params->GetLayerCreated());
    }
}

/**
 * @tc.name: CreateLayer_WithOfflineResultNullConsumerTest001
 * @tc.desc: Test CreateLayer with offlineResult but consumer is nullptr
 *           The if (buffer == nullptr || consumer == nullptr) branch at line 131 should be true
 * @tc.type: FUNC
 * @tc.require: issue41
 */
HWTEST_F(RSUniRenderProcessorTest, CreateLayer_WithOfflineResultNullConsumerTest001, TestSize.Level2)
{
    if (RSUniRenderJudgement::IsUniRender()) {
        auto surfaceNode = RSTestUtil::CreateSurfaceNode();
        auto surfaceDrawable = std::static_pointer_cast<DrawableV2::RSSurfaceRenderNodeDrawable>(
            DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(surfaceNode));
        auto params = static_cast<RSSurfaceRenderParams*>(surfaceDrawable->GetRenderParams().get());

        // Create offlineResult with null consumer
        auto offlineResult = std::make_shared<ProcessOfflineResult>();
        offlineResult->buffer = SurfaceBuffer::Create();
        BufferRequestConfig cfg { 100, 100, 8, GRAPHIC_PIXEL_FMT_RGBA_8888,
            BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA, 0 };
        auto ret = offlineResult->buffer->Alloc(cfg);
        ASSERT_EQ(ret, GSERROR_OK);
        offlineResult->consumer = nullptr;

        NodeId nodeId = 1;
        RSScreenRenderNode screenNode(nodeId, screenId_);
        auto renderEngine = std::make_shared<RSUniRenderEngine>();
        renderProcessor->Init(screenNode, renderEngine);
        auto composerClient = RSComposerClient::Create(nullptr, nullptr);
        renderProcessor->composerClient_ = composerClient;

        // Call CreateLayer with offlineResult null consumer, should return early
        EXPECT_NO_FATAL_FAILURE(renderProcessor->CreateLayer(*surfaceNode, *params, offlineResult));
        EXPECT_FALSE(params->GetLayerCreated());
    }
}

/**
 * @tc.name: CreateLayer_AllBranchesCoveredTest001
 * @tc.desc: Test CreateLayer with all branches covered
 *           Covers: buffer != nullptr, consumer != nullptr, layer != nullptr, bufferOwnerCount != nullptr
 * @tc.type: FUNC
 * @tc.require: issue41
 */
HWTEST_F(RSUniRenderProcessorTest, CreateLayer_AllBranchesCoveredTest001, TestSize.Level2)
{
    if (RSUniRenderJudgement::IsUniRender()) {
        auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
        auto surfaceDrawable = std::static_pointer_cast<DrawableV2::RSSurfaceRenderNodeDrawable>(
            DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(surfaceNode));
        auto params = static_cast<RSSurfaceRenderParams*>(surfaceDrawable->GetRenderParams().get());

        // Set bufferOwnerCount
        auto bufferOwnerCount = std::make_shared<RSSurfaceHandler::BufferOwnerCount>();
        bufferOwnerCount->bufferId_ = 8888ULL;
        params->bufferOwnerCount_ = bufferOwnerCount;

        NodeId nodeId = 1;
        RSScreenRenderNode screenNode(nodeId, screenId_);
        auto renderEngine = std::make_shared<RSUniRenderEngine>();
        renderProcessor->Init(screenNode, renderEngine);
        auto composerClient = RSComposerClient::Create(nullptr, nullptr);
        renderProcessor->composerClient_ = composerClient;

        // Call CreateLayer with all valid conditions
        EXPECT_NO_FATAL_FAILURE(renderProcessor->CreateLayer(*surfaceNode, *params));
        EXPECT_TRUE(params->GetLayerCreated());
        EXPECT_EQ(renderProcessor->layers_.size(), 1);
    }
}

/**
 * @tc.name: CreateLayerForRenderThread_NullBufferTest001
 * @tc.desc: Test CreateLayerForRenderThread when buffer is nullptr
 *           The if (buffer == nullptr || consumer == nullptr) branch at line 199 should be true
 * @tc.type: FUNC
 * @tc.require: issue41
 */
HWTEST_F(RSUniRenderProcessorTest, CreateLayerForRenderThread_NullBufferTest001, TestSize.Level2)
{
    if (RSUniRenderJudgement::IsUniRender()) {
        auto node = RSTestUtil::CreateSurfaceNode();
        auto surfaceDrawable = std::static_pointer_cast<DrawableV2::RSSurfaceRenderNodeDrawable>(
            DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(node));
        ASSERT_NE(surfaceDrawable, nullptr);

        // Ensure buffer is nullptr
        sptr<SurfaceBuffer> nullBuffer = nullptr;
        auto params = static_cast<RSSurfaceRenderParams*>(surfaceDrawable->GetRenderParams().get());
        params->SetBuffer(nullBuffer, {}, DEFAULT_RECT);

        NodeId nodeId = 1;
        RSScreenRenderNode screenNode(nodeId, screenId_);
        auto renderEngine = std::make_shared<RSUniRenderEngine>();
        renderProcessor->Init(screenNode, renderEngine);

        // Call CreateLayerForRenderThread with null buffer, should return early
        EXPECT_NO_FATAL_FAILURE(renderProcessor->CreateLayerForRenderThread(*surfaceDrawable));
        EXPECT_FALSE(params->GetLayerCreated());
    }
}

/**
 * @tc.name: CreateLayerForRenderThread_NullConsumerTest001
 * @tc.desc: Test CreateLayerForRenderThread when consumer is nullptr
 *           The if (buffer == nullptr || consumer == nullptr) branch at line 199 should be true
 * @tc.type: FUNC
 * @tc.require: issue41
 */
HWTEST_F(RSUniRenderProcessorTest, CreateLayerForRenderThread_NullConsumerTest001, TestSize.Level2)
{
    if (RSUniRenderJudgement::IsUniRender()) {
        auto node = RSTestUtil::CreateSurfaceNode();
        auto surfaceDrawable = std::static_pointer_cast<DrawableV2::RSSurfaceRenderNodeDrawable>(
            DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(node));
        ASSERT_NE(surfaceDrawable, nullptr);

        // consumer will be nullptr since no consumer is set
        auto params = static_cast<RSSurfaceRenderParams*>(surfaceDrawable->GetRenderParams().get());

        NodeId nodeId = 1;
        RSScreenRenderNode screenNode(nodeId, screenId_);
        auto renderEngine = std::make_shared<RSUniRenderEngine>();
        renderProcessor->Init(screenNode, renderEngine);

        // Call CreateLayerForRenderThread with null consumer, should return early
        EXPECT_NO_FATAL_FAILURE(renderProcessor->CreateLayerForRenderThread(*surfaceDrawable));
        EXPECT_FALSE(params->GetLayerCreated());
    }
}

/**
 * @tc.name: CreateLayerForRenderThread_NullLayerFromGetLayerInfoTest001
 * @tc.desc: Test CreateLayerForRenderThread when GetLayerInfo returns nullptr
 *           The if (layer == nullptr) branch at line 204 should be true
 * @tc.type: FUNC
 * @tc.require: issue41
 */
HWTEST_F(RSUniRenderProcessorTest, CreateLayerForRenderThread_NullLayerFromGetLayerInfoTest001, TestSize.Level2)
{
    if (RSUniRenderJudgement::IsUniRender()) {
        auto node = RSTestUtil::CreateSurfaceNodeWithBuffer();
        auto surfaceDrawable = std::static_pointer_cast<DrawableV2::RSSurfaceRenderNodeDrawable>(
            DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(node));
        ASSERT_NE(surfaceDrawable, nullptr);

        // Set composerClient_ to nullptr so GetLayerInfo returns nullptr
        renderProcessor->composerClient_ = nullptr;

        auto params = static_cast<RSSurfaceRenderParams*>(surfaceDrawable->GetRenderParams().get());

        // Call CreateLayerForRenderThread with null composerClient, GetLayerInfo will return nullptr
        EXPECT_NO_FATAL_FAILURE(renderProcessor->CreateLayerForRenderThread(*surfaceDrawable));
        EXPECT_FALSE(params->GetLayerCreated());
    }
}

/**
 * @tc.name: CreateLayerForRenderThread_NullBufferOwnerCountTest001
 * @tc.desc: Test CreateLayerForRenderThread when bufferOwnerCount is nullptr
 *           The if (bufferOwnerCount) branch at line 216 should be false
 * @tc.type: FUNC
 * @tc.require: issue41
 */
HWTEST_F(RSUniRenderProcessorTest, CreateLayerForRenderThread_NullBufferOwnerCountTest001, TestSize.Level2)
{
    if (RSUniRenderJudgement::IsUniRender()) {
        auto node = RSTestUtil::CreateSurfaceNodeWithBuffer();
        auto surfaceDrawable = std::static_pointer_cast<DrawableV2::RSSurfaceRenderNodeDrawable>(
            DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(node));
        ASSERT_NE(surfaceDrawable, nullptr);

        // Do NOT set bufferOwnerCount in params
        // params->GetBufferOwnerCount() will return nullptr
        auto params = static_cast<RSSurfaceRenderParams*>(surfaceDrawable->GetRenderParams().get());

        // Set buffer in params from node's surfaceHandler
        auto surfaceHandler = node->GetRSSurfaceHandler();
        auto buffer = surfaceHandler->GetBuffer();
        ASSERT_NE(buffer, nullptr);
        auto bufferOwnerCount = std::make_shared<RSSurfaceHandler::BufferOwnerCount>();
        params->SetBuffer(buffer, bufferOwnerCount, DEFAULT_RECT);
        NodeId nodeId = 1;
        RSScreenRenderNode screenNode(nodeId, screenId_);
        auto renderEngine = std::make_shared<RSUniRenderEngine>();
        renderProcessor->Init(screenNode, renderEngine);
        auto composerClient = RSComposerClient::Create(nullptr, nullptr);
        renderProcessor->composerClient_ = composerClient;


        // Call CreateLayerForRenderThread with null bufferOwnerCount
        EXPECT_NO_FATAL_FAILURE(renderProcessor->CreateLayerForRenderThread(*surfaceDrawable));
        EXPECT_TRUE(params->GetLayerCreated());
    }
}

/**
 * @tc.name: CreateLayerForRenderThread_WithBufferOwnerCountTest001
 * * @tc.desc: Test CreateLayerForRenderThread when bufferOwnerCount is not nullptr
 *           The if (bufferOwnerCount) branch at line 220 should be true
 * @tc.type: FUNC
 * @tc.require: issue41
 */
HWTEST_F(RSUniRenderProcessorTest, CreateLayerForRenderThread_WithBufferOwnerCountTest001, TestSize.Level2)
{
    if (RSUniRenderJudgement::IsUniRender()) {
        auto node = RSTestUtil::CreateSurfaceNodeWithBuffer();
        auto surfaceDrawable = std::static_pointer_cast<DrawableV2::RSSurfaceRenderNodeDrawable>(
            DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(node));
        ASSERT_NE(surfaceDrawable, nullptr);

        // Set bufferOwnerCount in params
        auto bufferOwnerCount = std::make_shared<RSSurfaceHandler::BufferOwnerCount>();
        bufferOwnerCount->bufferId_ = 7777ULL;
        auto params = static_cast<RSSurfaceRenderParams*>(surfaceDrawable->GetRenderParams().get());
        params->bufferOwnerCount_ = bufferOwnerCount;

        // Set buffer in params from node's surfaceHandler
        auto surfaceHandler = node->GetRSSurfaceHandler();
        auto buffer = surfaceHandler->GetBuffer();
        ASSERT_NE(buffer, nullptr);
        params->SetBuffer(buffer, bufferOwnerCount, DEFAULT_RECT);

        NodeId nodeId = 1;
        RSScreenRenderNode screenNode(nodeId, screenId_);
        auto renderEngine = std::make_shared<RSUniRenderEngine>();
        renderProcessor->Init(screenNode, renderEngine);
        auto composerClient = RSComposerClient::Create(nullptr, nullptr);
        renderProcessor->composerClient_ = composerClient;

        // Call CreateLayerForRenderThread with non-null bufferOwnerCount
        EXPECT_NO_FATAL_FAILURE(renderProcessor->CreateLayerForRenderThread(*surfaceDrawable));
        EXPECT_TRUE(params->GetLayerCreated());
    }
}

/**
 * @tc.name: CreateLayerForRenderThread_WithOfflineResultTest001
 * @tc.desc: Test CreateLayerForRenderThread with offlineResult parameter
 *           Covers the offlineResult != nullptr path
 * @tc.type: FUNC
 * @tc.require: issue41
 */
HWTEST_F(RSUniRenderProcessorTest, CreateLayerForRenderThread_WithOfflineResultTest001, TestSize.Level2)
{
    if (RSUniRenderJudgement::IsUniRender()) {
        auto node = RSTestUtil::CreateSurfaceNode();
        auto surfaceDrawable = std::static_pointer_cast<DrawableV2::RSSurfaceRenderNodeDrawable>(
            DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(node));
        ASSERT_NE(surfaceDrawable, nullptr);

        // Create offlineResult with buffer and consumer
        auto offlineResult = std::make_shared<ProcessOfflineResult>();
        offlineResult->buffer = SurfaceBuffer::Create();
        BufferRequestConfig cfg { 100, 100, 8, GRAPHIC_PIXEL_FMT_RGBA_8888,
            BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA, 0 };
        auto ret = offlineResult->buffer->Alloc(cfg);
        ASSERT_EQ(ret, GSERROR_OK);
        offlineResult->consumer = IConsumerSurface::Create("test-offline-render");
        offlineResult->damageRect = {0, 0, 100, 100};
        offlineResult->bufferRect = {0, 0, 100, 100};
        offlineResult->taskSuccess = true;

        NodeId nodeId = 1;
        RSScreenRenderNode screenNode(nodeId, screenId_);
        auto renderEngine = std::make_shared<RSUniRenderEngine>();
        renderProcessor->Init(screenNode, renderEngine);
        auto composerClient = RSComposerClient::Create(nullptr, nullptr);
        renderProcessor->composerClient_ = composerClient;

        auto params = static_cast<RSSurfaceRenderParams*>(surfaceDrawable->GetRenderParams().get());

        // Call CreateLayerForRenderThread with offlineResult
        EXPECT_NO_FATAL_FAILURE(renderProcessor->CreateLayerForRenderThread(*surfaceDrawable, offlineResult));
        EXPECT_TRUE(params->GetLayerCreated());
    }
}

/**
 * @tc.name: CreateLayerForRenderThread_WithOfflineResultNullBufferTest001
 * @tc.desc: Test CreateLayerForRenderThread with offlineResult but buffer is nullptr
 *           The if (buffer == nullptr || consumer == nullptr) branch at line 199 should be true
 * @tc.type: FUNC
 * @tc.require: issue41
 */
HWTEST_F(RSUniRenderProcessorTest, CreateLayerForRenderThread_WithOfflineResultNullBufferTest001, TestSize.Level2)
{
    if (RSUniRenderJudgement::IsUniRender()) {
        auto node = RSTestUtil::CreateSurfaceNode();
        auto surfaceDrawable = std::static_pointer_cast<DrawableV2::RSSurfaceRenderNodeDrawable>(
            DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(node));
        ASSERT_NE(surfaceDrawable, nullptr);

        // Create offlineResult with null buffer
        auto offlineResult = std::make_shared<ProcessOfflineResult>();
        offlineResult->buffer = nullptr;
        offlineResult->consumer = IConsumerSurface::Create("test-offline-render");

        NodeId nodeId = 1;
        RSScreenRenderNode screenNode(nodeId, screenId_);
        auto renderEngine = std::make_shared<RSUniRenderEngine>();
        renderProcessor->Init(screenNode, renderEngine);
        auto composerClient = RSComposerClient::Create(nullptr, nullptr);
        renderProcessor->composerClient_ = composerClient;

        auto params = static_cast<RSSurfaceRenderParams*>(surfaceDrawable->GetRenderParams().get());

        // Call CreateLayerForRenderThread with offlineResult null buffer, should return early
        EXPECT_NO_FATAL_FAILURE(renderProcessor->CreateLayerForRenderThread(*surfaceDrawable, offlineResult));
        EXPECT_FALSE(params->GetLayerCreated());
    }
}

/**
 * @tc.name: CreateLayerForRenderThread_WithOfflineResultNullConsumerTest001
 * @tc.desc: Test CreateLayerForRenderThread with offlineResult but consumer is nullptr
 *           The if (buffer == nullptr || consumer == nullptr) branch at line 199 should be true
 * @tc.type: FUNC
 * @tc.require: issue41
 */
HWTEST_F(RSUniRenderProcessorTest, CreateLayerForRenderThread_WithOfflineResultNullConsumerTest001, TestSize.Level2)
{
    if (RSUniRenderJudgement::IsUniRender()) {
        auto node = RSTestUtil::CreateSurfaceNode();
        auto surfaceDrawable = std::static_pointer_cast<DrawableV2::RSSurfaceRenderNodeDrawable>(
            DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(node));
        ASSERT_NE(surfaceDrawable, nullptr);

        // Create offlineResult with null consumer
        auto offlineResult = std::make_shared<ProcessOfflineResult>();
        offlineResult->buffer = SurfaceBuffer::Create();
        BufferRequestConfig cfg { 100, 100, 8, GRAPHIC_PIXEL_FMT_RGBA_8888,
            BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA, 0 };
        auto ret = offlineResult->buffer->Alloc(cfg);
        ASSERT_EQ(ret, GSERROR_OK);
        offlineResult->consumer = nullptr;

        NodeId nodeId = 1;
        RSScreenRenderNode screenNode(nodeId, screenId_);
        auto renderEngine = std::make_shared<RSUniRenderEngine>();
        renderProcessor->Init(screenNode, renderEngine);
        auto composerClient = RSComposerClient::Create(nullptr, nullptr);
        renderProcessor->composerClient_ = composerClient;

        auto params = static_cast<RSSurfaceRenderParams*>(surfaceDrawable->GetRenderParams().get());

        // Call CreateLayerForRenderThread with offlineResult null consumer, should return early
        EXPECT_NO_FATAL_FAILURE(renderProcessor->CreateLayerForRenderThread(*surfaceDrawable, offlineResult));
        EXPECT_FALSE(params->GetLayerCreated());
    }
}

/**
 * @tc.name: CreateLayerForRenderThread_AllBranchesCoveredTest001
 * @tc.desc: Test CreateLayerForRenderThread with all branches covered
 *           Covers: paramsSp != nullptr, buffer != nullptr, consumer != nullptr,
 *           layer != nullptr, bufferOwnerCount != nullptr
 * @tc.type: FUNC
 * @tc.require: issue41
 */
HWTEST_F(RSUniRenderProcessorTest, CreateLayerForRenderThread_AllBranchesCoveredTest001, TestSize.Level2)
{
    if (RSUniRenderJudgement::IsUniRender()) {
        auto node = RSTestUtil::CreateSurfaceNodeWithBuffer();
        auto surfaceDrawable = std::static_pointer_cast<DrawableV2::RSSurfaceRenderNodeDrawable>(
            DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(node));
        ASSERT_NE(surfaceDrawable, nullptr);

        // Set bufferOwnerCount
        auto bufferOwnerCount = std::make_shared<RSSurfaceHandler::BufferOwnerCount>();
        bufferOwnerCount->bufferId_ = 6666ULL;
        auto params = static_cast<RSSurfaceRenderParams*>(surfaceDrawable->GetRenderParams().get());
        params->bufferOwnerCount_ = bufferOwnerCount;

        // Set buffer in params from node's surfaceHandler
        auto surfaceHandler = node->GetRSSurfaceHandler();
        auto buffer = surfaceHandler->GetBuffer();
        ASSERT_NE(buffer, nullptr);
        params->SetBuffer(buffer, bufferOwnerCount, DEFAULT_RECT);

        NodeId nodeId = 1;
        RSScreenRenderNode screenNode(nodeId, screenId_);
        auto renderEngine = std::make_shared<RSUniRenderEngine>();
        renderProcessor->Init(screenNode, renderEngine);
        auto composerClient = RSComposerClient::Create(nullptr, nullptr);
        renderProcessor->composerClient_ = composerClient;

        // Call CreateLayerForRenderThread with all valid conditions
        EXPECT_NO_FATAL_FAILURE(renderProcessor->CreateLayerForRenderThread(*surfaceDrawable));
        EXPECT_TRUE(params->GetLayerCreated());
        EXPECT_EQ(renderProcessor->layers_.size(), 1);
    }
}

/**
 * @tc.name: GetLayerInfo_ComposerClientNullTest001
 * @tc.desc: Test GetLayerInfo when composerClient_ is nullptr
 *           The if (composerClient_ == nullptr) branch at line 361 should be true
 * @tc.type: FUNC
 * @tc.require: issue41
 */
HWTEST_F(RSUniRenderProcessorTest, GetLayerInfo_ComposerClientNullTest001, TestSize.Level2)
{
    ASSERT_NE(renderProcessor, nullptr);
    RSSurfaceRenderParams params(0);
    sptr<SurfaceBuffer> buffer = SurfaceBuffer::Create();
    BufferRequestConfig cfg { 100, 100, 8, GRAPHIC_PIXEL_FMT_RGBA_8888,
        BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA, 0 };
    auto ret = buffer->Alloc(cfg);
    ASSERT_EQ(ret, GSERROR_OK);
    sptr<IConsumerSurface> consumer = IConsumerSurface::Create("test");
    sptr<SurfaceBuffer> preBuffer = nullptr;
    sptr<SyncFence> acquireFence = nullptr;

    // Set composerClient_ to nullptr
    renderProcessor->composerClient_ = nullptr;

    RSLayerPtr result = renderProcessor->GetLayerInfo(params, buffer, preBuffer, consumer, acquireFence);
    EXPECT_EQ(result, nullptr);
}

/**
 * @tc.name: GetLayerInfo_HwcGlobalPositionDisabledTest001
 * @tc.desc: Test GetLayerInfo when HwcGlobalPositionEnabled is false
 *           The if (params.GetHwcGlobalPositionEnabled()) branch at line 372 should be false
 * @tc.type: FUNC
 * @tc.require: issue41
 */
HWTEST_F(RSUniRenderProcessorTest, GetLayerInfo_HwcGlobalPositionDisabledTest001, TestSize.Level2)
{
    ASSERT_NE(renderProcessor, nullptr);
    auto composerClient = RSComposerClient::Create(nullptr, nullptr);
    renderProcessor->composerClient_ = composerClient;

    RSSurfaceRenderParams params(0);
    params.SetHwcGlobalPositionEnabled(false);  // Disabled
    sptr<SurfaceBuffer> buffer = SurfaceBuffer::Create();
    BufferRequestConfig cfg { 100, 100, 8, GRAPHIC_PIXEL_FMT_RGBA_8888,
        BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA, 0 };
    auto ret = buffer->Alloc(cfg);
    ASSERT_EQ(ret, GSERROR_OK);
    sptr<IConsumerSurface> consumer = IConsumerSurface::Create("test");
    sptr<SurfaceBuffer> preBuffer = nullptr;
    sptr<SyncFence> acquireFence = nullptr;

    RSLayerPtr result = renderProcessor->GetLayerInfo(params, buffer, preBuffer, consumer, acquireFence);
    EXPECT_NE(result, nullptr);
}

/**
 * @tc.name: GetLayerInfo_TunnelLayerIdDisabledTest001
 * @tc.desc: Test GetLayerInfo when TunnelLayerId is 0
 *           The if (params.GetTunnelLayerId()) branch at line 387 should be false
 * @tc.type: FUNC
 * @tc.require: issue41
 */
HWTEST_F(RSUniRenderProcessorTest, GetLayerInfo_TunnelLayerIdDisabledTest001, TestSize.Level2)
{
    ASSERT_NE(renderProcessor, nullptr);
    auto composerClient = RSComposerClient::Create(nullptr, nullptr);
    renderProcessor->composerClient_ = composerClient;

    RSSurfaceRenderParams params(0);
    params.SetTunnelLayerId(0);  // Disabled
    sptr<SurfaceBuffer> buffer = SurfaceBuffer::Create();
    BufferRequestConfig cfg { 100, 100, 8, GRAPHIC_PIXEL_FMT_RGBA_8888,
        BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA, 0 };
    auto ret = buffer->Alloc(cfg);
    ASSERT_EQ(ret, GSERROR_OK);
    sptr<IConsumerSurface> consumer = IConsumerSurface::Create("test");
    sptr<SurfaceBuffer> preBuffer = nullptr;
    sptr<SyncFence> acquireFence = nullptr;

    RSLayerPtr result = renderProcessor->GetLayerInfo(params, buffer, preBuffer, consumer, acquireFence);
    EXPECT_NE(result, nullptr);
    EXPECT_NE(result->GetType(), GraphicLayerType::GRAPHIC_LAYER_TYPE_TUNNEL);
}

/**
 * @tc.name: GetLayerInfo_CursorLayerTypeTest001
 * @tc.desc: Test GetLayerInfo when layerType is CURSOR
 *           Covers the cursor layer specific branches at line 400 and 408
 * @tc.type: FUNC
 * @tc.require: issue41
 */
HWTEST_F(RSUniRenderProcessorTest, GetLayerInfo_CursorLayerTypeTest001, TestSize.Level2)
{
    ASSERT_NE(renderProcessor, nullptr);
    auto composerClient = RSComposerClient::Create(nullptr, nullptr);
    renderProcessor->composerClient_ = composerClient;

    RSSurfaceRenderParams params(0);
    RSLayerInfo layerInfo;
    layerInfo.layerType = GraphicLayerType::GRAPHIC_LAYER_TYPE_CURSOR;
    layerInfo.dstRect = {0, 0, 200, 200};
    layerInfo.srcRect = {0, 0, 100, 100};  // Different from dstRect to trigger cursor branch
    layerInfo.zOrder = 100;
    params.SetLayerInfo(layerInfo);

    sptr<SurfaceBuffer> buffer = SurfaceBuffer::Create();
    BufferRequestConfig cfg { 100, 100, 8, GRAPHIC_PIXEL_FMT_RGBA_8888,
        BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA, 0 };
    auto ret = buffer->Alloc(cfg);
    ASSERT_EQ(ret, GSERROR_OK);
    sptr<IConsumerSurface> consumer = IConsumerSurface::Create("test");
    sptr<SurfaceBuffer> preBuffer = nullptr;
    sptr<SyncFence> acquireFence = nullptr;

    RSLayerPtr result = renderProcessor->GetLayerInfo(params, buffer, preBuffer, consumer, acquireFence);
    EXPECT_NE(result, nullptr);
    EXPECT_EQ(result->GetType(), GraphicLayerType::GRAPHIC_LAYER_TYPE_CURSOR);
    // Cursor layer should have rotation set to NONE
    EXPECT_EQ(result->GetTransform(), GraphicTransformType::GRAPHIC_ROTATE_NONE);
}

/**
 * @tc.name: GetLayerInfo_WithOfflineResultTest001
 * @tc.desc: Test GetLayerInfo with offlineResult parameter
 *           Covers the offlineResult branches at line 382, 437, 478
 * @tc.type: FUNC
 * @tc.require: issue41
 */
HWTEST_F(RSUniRenderProcessorTest, GetLayerInfo_WithOfflineResultTest001, TestSize.Level2)
{
    ASSERT_NE(renderProcessor, nullptr);
    auto composerClient = RSComposerClient::Create(nullptr, nullptr);
    renderProcessor->composerClient_ = composerClient;

    RSSurfaceRenderParams params(0);
    RSLayerInfo layerInfo;
    layerInfo.srcRect = {0, 0, 100, 100};
    params.SetLayerInfo(layerInfo);

    sptr<SurfaceBuffer> buffer = SurfaceBuffer::Create();
    BufferRequestConfig cfg { 100, 100, 8, GRAPHIC_PIXEL_FMT_RGBA_8888,
        BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA, 0 };
    auto ret = buffer->Alloc(cfg);
    ASSERT_EQ(ret, GSERROR_OK);
    sptr<IConsumerSurface> consumer = IConsumerSurface::Create("test");

    auto offlineResult = std::make_shared<ProcessOfflineResult>();
    offlineResult->buffer = buffer;
    offlineResult->consumer = consumer;
    offlineResult->damageRect = {0, 0, 50, 50};
    offlineResult->bufferRect = {0, 0, 100, 100};
    offlineResult->taskSuccess = true;

    sptr<SurfaceBuffer> preBuffer = nullptr;
    sptr<SyncFence> acquireFence = nullptr;

    RSLayerPtr result = renderProcessor->GetLayerInfo(params, buffer, preBuffer, consumer, acquireFence, offlineResult);
    EXPECT_NE(result, nullptr);
}

/**
 * @tc.name: GetLayerInfo_NonCursorLayerTypeTest001
 * @tc.desc: Test GetLayerInfo when layerType is not CURSOR
 *           The if (layerInfo.layerType != GRAPHIC_LAYER_TYPE_CURSOR) branch at line 400 should be true
 *           Covers ROG ratio adjustment path
 * @tc.type: FUNC
 * @tc.require: issue41
 */
HWTEST_F(RSUniRenderProcessorTest, GetLayerInfo_NonCursorLayerTypeTest001, TestSize.Level2)
{
    ASSERT_NE(renderProcessor, nullptr);
    auto composerClient = RSComposerClient::Create(nullptr, nullptr);
    renderProcessor->composerClient_ = composerClient;

    // Set screen info with ROG ratios
    ScreenInfo screenInfo;
    screenInfo.isSamplingOn = false;
    renderProcessor->screenInfo_ = screenInfo;

    RSSurfaceRenderParams params(0);
    RSLayerInfo layerInfo;
    layerInfo.layerType = GraphicLayerType::GRAPHIC_LAYER_TYPE_GRAPHIC;
    layerInfo.dstRect = {0, 0, 100, 100};
    params.SetLayerInfo(layerInfo);

    sptr<SurfaceBuffer> buffer = SurfaceBuffer::Create();
    BufferRequestConfig cfg { 100, 100, 8, GRAPHIC_PIXEL_FMT_RGBA_8888,
        BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA, 0 };
    auto ret = buffer->Alloc(cfg);
    ASSERT_EQ(ret, GSERROR_OK);
    sptr<IConsumerSurface> consumer = IConsumerSurface::Create("test");
    sptr<SurfaceBuffer> preBuffer = nullptr;
    sptr<SyncFence> acquireFence = nullptr;

    RSLayerPtr result = renderProcessor->GetLayerInfo(params, buffer, preBuffer, consumer, acquireFence);
    EXPECT_NE(result, nullptr);
    EXPECT_NE(result->GetType(), GraphicLayerType::GRAPHIC_LAYER_TYPE_CURSOR);
}

/**
 * @tc.name: GetLayerInfo_ProtectedLayerTest001
 * @tc.desc: Test GetLayerInfo with protected layer type
 *           Covers GetForceClientForDRM branch with PROTECTED layer
 * @tc.type: FUNC
 * @tc.require: issue41
 */
HWTEST_F(RSUniRenderProcessorTest, GetLayerInfo_ProtectedLayerTest001, TestSize.Level2)
{
    ASSERT_NE(renderProcessor, nullptr);
    auto composerClient = RSComposerClient::Create(nullptr, nullptr);
    renderProcessor->composerClient_ = composerClient;

    RSSurfaceRenderParams params(0);
    params.GetMultableSpecialLayerMgr().Set(SpecialLayerType::PROTECTED, true);
    params.animateState_ = true;  // This triggers forceClientForDRM

    sptr<SurfaceBuffer> buffer = SurfaceBuffer::Create();
    BufferRequestConfig cfg { 100, 100, 8, GRAPHIC_PIXEL_FMT_RGBA_8888,
        BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA, 0 };
    auto ret = buffer->Alloc(cfg);
    ASSERT_EQ(ret, GSERROR_OK);
    sptr<IConsumerSurface> consumer = IConsumerSurface::Create("test");
    sptr<SurfaceBuffer> preBuffer = nullptr;
    sptr<SyncFence> acquireFence = nullptr;

    RSLayerPtr result = renderProcessor->GetLayerInfo(params, buffer, preBuffer, consumer, acquireFence);
    EXPECT_NE(result, nullptr);
}

/**
 * @tc.name: GetLayerInfo_CornerRadiusInfoForDRMTest001
 * @tc.desc: Test GetLayerInfo with CornerRadiusInfoForDRM set
 * @tc.type: FUNC
 * @tc.require: issue41
 */
HWTEST_F(RSUniRenderProcessorTest, GetLayerInfo_CornerRadiusInfoForDRMTest001, TestSize.Level2)
{
    ASSERT_NE(renderProcessor, nullptr);
    auto composerClient = RSComposerClient::Create(nullptr, nullptr);
    renderProcessor->composerClient_ = composerClient;

    RSSurfaceRenderParams params(0);
    std::vector<float> cornerRadius = {10.0f, 10.0f, 10.0f, 10.0f};
    params.SetCornerRadiusInfoForDRM(cornerRadius);

    sptr<SurfaceBuffer> buffer = SurfaceBuffer::Create();
    BufferRequestConfig cfg { 100, 100, 8, GRAPHIC_PIXEL_FMT_RGBA_8888,
        BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA, 0 };
    auto ret = buffer->Alloc(cfg);
    ASSERT_EQ(ret, GSERROR_OK);
    sptr<IConsumerSurface> consumer = IConsumerSurface::Create("test");
    sptr<SurfaceBuffer> preBuffer = nullptr;
    sptr<SyncFence> acquireFence = nullptr;

    RSLayerPtr result = renderProcessor->GetLayerInfo(params, buffer, preBuffer, consumer, acquireFence);
    EXPECT_NE(result, nullptr);
    EXPECT_FALSE(result->GetCornerRadiusInfoForDRM().empty());
}

/**
 * @tc.name: GetLayerInfo_AllBranchesCoveredTest001
 * @tc.desc: Test GetLayerInfo with all main branches covered
 *           Normal path with all valid parameters
 * @tc.type: FUNC
 * @tc.require: issue41
 */
HWTEST_F(RSUniRenderProcessorTest, GetLayerInfo_AllBranchesCoveredTest001, TestSize.Level2)
{
    ASSERT_NE(renderProcessor, nullptr);
    auto composerClient = RSComposerClient::Create(nullptr, nullptr);
    renderProcessor->composerClient_ = composerClient;

    // Set screen info
    ScreenInfo screenInfo;
    screenInfo.isSamplingOn = false;
    renderProcessor->screenInfo_ = screenInfo;

    RSSurfaceRenderParams params(0);
    RSLayerInfo layerInfo;
    layerInfo.layerType = GraphicLayerType::GRAPHIC_LAYER_TYPE_GRAPHIC;
    layerInfo.dstRect = {0, 0, 100, 100};
    layerInfo.srcRect = {0, 0, 100, 100};
    layerInfo.zOrder = 1;
    layerInfo.alpha = 1.0f;
    params.SetLayerInfo(layerInfo);
    params.SetHwcGlobalPositionEnabled(false);
    params.SetTunnelLayerId(0);

    sptr<SurfaceBuffer> buffer = SurfaceBuffer::Create();
    BufferRequestConfig cfg { 100, 100, 8, GRAPHIC_PIXEL_FMT_RGBA_8888,
        BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA, 0 };
    auto ret = buffer->Alloc(cfg);
    ASSERT_EQ(ret, GSERROR_OK);
    sptr<IConsumerSurface> consumer = IConsumerSurface::Create("test");
    sptr<SurfaceBuffer> preBuffer = nullptr;
    sptr<SyncFence> acquireFence = new SyncFence(dup(STDOUT_FILENO));

    RSLayerPtr result = renderProcessor->GetLayerInfo(params, buffer, preBuffer, consumer, acquireFence);
    EXPECT_NE(result, nullptr);
    EXPECT_EQ(result->GetType(), GraphicLayerType::GRAPHIC_LAYER_TYPE_GRAPHIC);
    EXPECT_TRUE(result->GetIsNeedComposition());
}
}