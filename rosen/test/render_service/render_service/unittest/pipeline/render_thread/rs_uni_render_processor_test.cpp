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
#include "pipeline/render_thread/rs_uni_render_engine.h"
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
    static inline std::unique_ptr<OHOS::Rosen::impl::RSScreen> screen = nullptr;
};

void RSUniRenderProcessorTest::SetUpTestCase()
{
#ifdef RS_ENABLE_VK
    RsVulkanContext::SetRecyclable(false);
#endif
    RSTestUtil::InitRenderNodeGC();
    auto output = std::make_shared<HdiOutput>(screenId_);
    RSRenderComposerManager::GetInstance().OnScreenConnected(output);
    screen = std::make_unique<OHOS::Rosen::impl::RSScreen>(screenId_, false, output, nullptr);
    auto screenManagerPtr = impl::RSScreenManager::GetInstance();
    auto* screenManager = static_cast<impl::RSScreenManager*>(screenManagerPtr.GetRefPtr());
    screenManager->screens_.insert(std::make_pair(0, std::move(screen)));

    renderProcessor = std::make_shared<RSUniRenderProcessor>();
    renderProcessor->screenInfo_.id = screenId_;
    NodeId nodeId = 1;
    RSScreenRenderNode node(nodeId, screenId_);
    auto renderEngine = std::make_shared<RSUniRenderEngine>();
    renderProcessor->Init(node, 0, 0, 0, renderEngine);
    EXPECT_NE(renderProcessor->composerClient_, nullptr);
}
void RSUniRenderProcessorTest::TearDownTestCase()
{
    RSRenderComposerManager::GetInstance().rsRenderComposerMap_[screenId_]->uniRenderEngine_ = nullptr;
    renderProcessor = nullptr;
}
void RSUniRenderProcessorTest::SetUp() {}
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
        auto processor = RSProcessorFactory::CreateProcessor(CompositeType::UNI_RENDER_COMPOSITE);
        RSScreenRenderNode node(nodeId, screenId, context);
        EXPECT_EQ(processor->Init(node, 0, 0, 0, nullptr), false);
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
        auto processor = RSProcessorFactory::CreateProcessor(CompositeType::UNI_RENDER_COMPOSITE);
        ASSERT_NE(processor, nullptr);
        RSScreenRenderNode node(nodeId, screenId, context);
        auto uniRenderEngine = std::make_shared<RSUniRenderEngine>();
        processor->Init(node, 0, 0, 0, uniRenderEngine);
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
        params->SetBuffer(bufferTest, {});
        params->SetPreBuffer(preBufferTest);
        layerInfo.zOrder = 0;
        params->SetLayerInfo(layerInfo);
        renderProcessor->composerClient_ = nullptr;
        renderProcessor->CreateLayer(*surfaceNode, *params);

        NodeId nodeId = 1;
        RSScreenRenderNode screenNode(nodeId, screenId_);
        auto renderEngine = std::make_shared<RSUniRenderEngine>();
        renderProcessor->Init(screenNode, 0, 0, 0, renderEngine);
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
        // case1:renderEngine is nullptr
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
        screenDrawable->renderParams_ = std::make_unique<RSRenderParams>(screenNode->GetId());
        ret = renderProcessor->InitForRenderThread(*screenDrawable, renderEngine);
        EXPECT_EQ(ret, false);
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
        ASSERT_EQ(result, true);
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
        auto output = std::make_shared<HdiOutput>(1);
        renderProcessor->uniComposerAdapter_->output_ = output;
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

        auto renderProcessor = std::make_shared<RSUniRenderProcessor>();
        ASSERT_NE(renderProcessor, nullptr);
        auto output = std::make_shared<HdiOutput>(1);
        ASSERT_NE(output, nullptr);
        renderProcessor->uniComposerAdapter_->output_ = output;
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
        auto output = std::make_shared<HdiOutput>(1);
        ASSERT_NE(output, nullptr);
        renderProcessor->uniComposerAdapter_->output_ = output;
        renderProcessor->ProcessScreenSurfaceForRenderThread(drawable);
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
        surfaceDrawable->renderParams_->SetBuffer(buffer, {});
        ASSERT_EQ(renderProcessor, nullptr);
        renderProcessor->composerClient_ = nullptr;
        renderProcessor->CreateLayerForRenderThread(*surfaceDrawable);
        
        NodeId nodeId = 1;
        RSScreenRenderNode screenNode(nodeId, screenId_);
        auto renderEngine = std::make_shared<RSUniRenderEngine>();
        renderProcessor->Init(screenNode, 0, 0, 0, renderEngine);
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
        auto renderProcessor = std::make_shared<RSUniRenderProcessor>();
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
 * @tc.name: GetForceClientForDRM005
 * @tc.desc: Test RSUniRenderProcessorTest.GetForceClientForDRM
 * @tc.type:FUNC
 * @tc.require: issueIAIT5Z
 */
HWTEST_F(RSUniRenderProcessorTest, GetForceClientForDRM005, TestSize.Level1)
{
    if (RSUniRenderJudgement::IsUniRender()) {
        ASSERT_NE(renderProcessor, nullptr);
        RSSurfaceRenderParams params(0);
        params.GetMultableSpecialLayerMgr().Set(SpecialLayerType::PROTECTED, true);
        params.animateState_ = false;
        ASSERT_FALSE(renderProcessor->GetForceClientForDRM(params));
        // set drm out of screen
        params.isOutOfScreen_ = false;
        ASSERT_FALSE(renderProcessor->GetForceClientForDRM(params));
        params.isOutOfScreen_ = true;
        ASSERT_TRUE(renderProcessor->GetForceClientForDRM(params));
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

    layer = std::make_shared<RSSurfaceLayer>();
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
    EXPECT_EQ(result->GetType(), GraphicLayerType::GRAPHIC_LAYER_TYPE_TUNNEL);
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
    sptr<IConsumerSurface> consumer = nullptr;
    sptr<SyncFence> acquireFence = nullptr;
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
    sptr<IConsumerSurface> consumer = nullptr;
    sptr<SyncFence> acquireFence = nullptr;
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
    sptr<IConsumerSurface> consumer = nullptr;
    sptr<SyncFence> acquireFence = nullptr;
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
    sptr<IConsumerSurface> consumer = nullptr;
    sptr<SyncFence> acquireFence = nullptr;
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
    RSSurfaceRenderParams params(0);
    params.SetTunnelLayerId(1);
    sptr<SurfaceBuffer> preBuffer = nullptr;
    sptr<IConsumerSurface> consumer = nullptr;
    sptr<SyncFence> acquireFence = nullptr;
    auto buffer = SurfaceBuffer::Create();
    auto ret = buffer->Alloc(RSUniRenderProcessorTest::requestConfig);
    ASSERT_EQ(ret, GSERROR_OK);
 
    auto src = RSGpuDirtyCollector::GetBufferSelfDrawingData(buffer);
    ASSERT_NE(src, nullptr);
    (*src) = RSUniRenderProcessorTest::defaultSelfDrawingRect;

    EXPECT_EQ(params.GetTunnelLayerId(), 1);
    params.SetBuffer(buffer, DEFAULT_RECT);
    auto param = system::GetParameter("rosen.graphic.selfdrawingdirtyregion.enabled", "");
    system::SetParameter("rosen.graphic.selfdrawingdirtyregion.enabled", "1");
    RSLayerPtr result = renderProcessor->GetLayerInfo(params, buffer, preBuffer, consumer, acquireFence);
    EXPECT_EQ(result->GetType(), GraphicLayerType::GRAPHIC_LAYER_TYPE_TUNNEL);
    system::SetParameter("rosen.graphic.selfdrawingdirtyregion.enabled", param);
    result = renderProcessor->GetLayerInfo(params, buffer, preBuffer, consumer, acquireFence);
    EXPECT_EQ(result->GetType(), GraphicLayerType::GRAPHIC_LAYER_TYPE_TUNNEL);
}

/**
 * @tc.name: ScaleLayerIfNeeded001
 * @tc.desc: Test RSUniRenderProcessorTest.ScaleLayerIfNeeded
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSUniRenderProcessorTest, ScaleLayerIfNeeded001, TestSize.Level1)
{
    ScreenInfo screenInfo;
    screenInfo.isSamplingOn = false;
    auto renderProcessor = std::make_shared<RSUniRenderProcessor>();
    ASSERT_NE(renderProcessor, nullptr);
    renderProcessor->screenInfo_ = screenInfo;

    RSLayerInfo layerInfo;
    layerInfo.dstRect.x = 1;

    renderProcessor->ScaleLayerIfNeeded(layerInfo);
    EXPECT_EQ(layerInfo.dstRect.x, 1);

    screenInfo.isSamplingOn = true;
    screenInfo.samplingTranslateX = 1.f;
    screenInfo.samplingTranslateY = 1.f;
    renderProcessor->screenInfo_ = screenInfo;
    renderProcessor->ScaleLayerIfNeeded(layerInfo);
    EXPECT_EQ(layerInfo.dstRect.x, 2);
}

/**
 * @tc.name: CreateSolidColorLayerTest
 * @tc.desc: Test CreateSolidColorLayer
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSUniRenderProcessorTest, CreateSolidColorLayerTest, TestSize.Level1)
{
    RSLayerPtr layer = std::make_shared<RSSurfaceLayer>();
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    auto surfaceDrawable = std::static_pointer_cast<DrawableV2::RSSurfaceRenderNodeDrawable>(
        DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(surfaceNode));
    auto params = static_cast<RSSurfaceRenderParams*>(surfaceDrawable->GetRenderParams().get());
    params->SetIsHwcEnabledBySolidLayer(true);
    ASSERT_NE(renderProcessor, nullptr);
    renderProcessor->composerClient_ = nullptr;
    renderProcessor->CreateSolidColorLayer(layer, *params);

    NodeId nodeId = 1;
    RSScreenRenderNode screenNode(nodeId, screenId_);
    auto renderEngine = std::make_shared<RSUniRenderEngine>();
    renderProcessor->Init(screenNode, 0, 0, 0, renderEngine);
    renderProcessor->CreateSolidColorLayer(layer, *params);
    ASSERT_NE(renderProcessor->layers_.size(), 0);
}
}