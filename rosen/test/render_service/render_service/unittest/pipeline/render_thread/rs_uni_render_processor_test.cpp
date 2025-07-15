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
#include "limit_number.h"
#include "foundation/graphic/graphic_2d/rosen/test/render_service/render_service/unittest/pipeline/rs_test_util.h"
#include "dirty_region/rs_gpu_dirty_collector.h"
#include "drawable/rs_logical_display_render_node_drawable.h"
#include "drawable/rs_screen_render_node_drawable.h"
#include "metadata_helper.h"
#include "params/rs_screen_render_params.h"
#include "feature/round_corner_display/rs_rcd_surface_render_node.h"
#include "feature/round_corner_display/rs_rcd_surface_render_node_drawable.h"
#include "pipeline/render_thread/rs_uni_render_engine.h"
#include "pipeline/render_thread/rs_uni_render_processor.h"
#include "pipeline/render_thread/rs_render_engine.h"
#include "pipeline/rs_logical_display_render_node.h"
#include "pipeline/rs_screen_render_node.h"
#include "pipeline/rs_processor_factory.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSUniRenderProcessorTest : public testing::Test {
public:
    static inline BufferRequestConfig requestConfig = {
        .width = 200,
        .height = 200,
        .strideAlignment = 0x8,
        .format = GRAPHIC_PIXEL_FMT_RGBA_8888,
        .usage = BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA,
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

    static inline BlobDataType defaultBlobDataType = {
        .offset = 0,
        .length = 0,
        .capacity = 0,
        .vaddr = 0,
        .cacheop = CacheOption::CACHE_NOOP,
    };

    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSUniRenderProcessorTest::SetUpTestCase() {}
void RSUniRenderProcessorTest::TearDownTestCase() {}
void RSUniRenderProcessorTest::SetUp() {}
void RSUniRenderProcessorTest::TearDown() {}

/**
 * @tc.name: ProcessorInit001
 * @tc.desc: test ProcessorInit func with renderEngine nullptr
 * @tc.type: FUNC
 * @tc.require: issueI6QM6E
 */
HWTEST(RSUniRenderProcessorTest, ProcessorInit001, TestSize.Level1)
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
HWTEST(RSUniRenderProcessorTest, ProcessSurface001, TestSize.Level1)
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
 * @tc.name: InitTest
 * @tc.desc: Verify function Init
 * @tc.type:FUNC
 * @tc.require:issuesI9KRF1
 */
HWTEST(RSUniRenderProcessorTest, InitTest, TestSize.Level1)
{
    NodeId nodeId = 1;
    ScreenId screenId = 0;
    std::weak_ptr<RSContext> context = {};
    if (RSUniRenderJudgement::IsUniRender()) {
        auto renderProcessor = std::make_shared<RSUniRenderProcessor>();
        RSScreenRenderNode node(nodeId, screenId, context);
        auto renderEngine = std::make_shared<RSUniRenderEngine>();
        EXPECT_EQ(renderProcessor->Init(node, 0, 0, 0, renderEngine), false);
    }
}

/**
 * @tc.name: ProcessSurfaceTest
 * @tc.desc: Verify function ProcessSurface
 * @tc.type:FUNC
 * @tc.require:issuesI9KRF1
 */
HWTEST(RSUniRenderProcessorTest, ProcessSurfaceTest, TestSize.Level1)
{
    NodeId nodeId = 1;
    ScreenId screenId = 0;
    std::weak_ptr<RSContext> context = {};
    if (RSUniRenderJudgement::IsUniRender()) {
        auto renderProcessor = std::make_shared<RSUniRenderProcessor>();
        ASSERT_NE(renderProcessor, nullptr);
        RSScreenRenderNode node(nodeId, screenId, context);
        auto uniRenderEngine = std::make_shared<RSUniRenderEngine>();
        renderProcessor->Init(node, 0, 0, 0, uniRenderEngine);
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
HWTEST(RSUniRenderProcessorTest, CreateLayerTest, TestSize.Level1)
{
    if (RSUniRenderJudgement::IsUniRender()) {
        auto renderProcessor = std::make_shared<RSUniRenderProcessor>();
        HdiBackend hdiBackend;
        auto output = std::make_shared<HdiOutput>(1);
        renderProcessor->uniComposerAdapter_->hdiBackend_ = &hdiBackend;
        renderProcessor->uniComposerAdapter_->output_ = output;
        RSSurfaceRenderNode node(0);
        auto iConsumerSurface = IConsumerSurface::Create();
        node.GetRSSurfaceHandler()->SetConsumer(iConsumerSurface);
        sptr<SurfaceBuffer> buffer = OHOS::SurfaceBuffer::Create();
        sptr<SyncFence> acquireFence = SyncFence::InvalidFence();
        int64_t timestamp = 0;
        Rect damage;
        node.GetRSSurfaceHandler()->SetBuffer(buffer, acquireFence, damage, timestamp);
        RSSurfaceRenderParams params(0);
        RSLayerInfo layerInfo;
        sptr<SurfaceBuffer> bufferTest = OHOS::SurfaceBuffer::Create();
        sptr<SurfaceBuffer> preBufferTest = OHOS::SurfaceBuffer::Create();
        params.SetBuffer(bufferTest, {});
        params.SetPreBuffer(preBufferTest);
        layerInfo.zOrder = 0;
        params.SetLayerInfo(layerInfo);
        renderProcessor->CreateLayer(node, params);
        EXPECT_TRUE(params.GetLayerCreated());
    }
}

/**
 * @tc.name: ProcessScreenSurfaceTest
 * @tc.desc: Verify function ProcessScreenSurface
 * @tc.type:FUNC
 * @tc.require:issuesI9KRF1
 */
HWTEST(RSUniRenderProcessorTest, ProcessScreenSurfaceTest, TestSize.Level1)
{
    ScreenId screenId = 0;
    std::weak_ptr<RSContext> context = {};
    if (RSUniRenderJudgement::IsUniRender()) {
        auto renderProcessor = std::make_shared<RSUniRenderProcessor>();
        constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[0];
        RSScreenRenderNode node(nodeId, screenId, context);
        renderProcessor->ProcessScreenSurface(node);
        EXPECT_FALSE(renderProcessor->uniComposerAdapter_->CreateLayer(node));
    }
}

/**
 * @tc.name: ProcessRcdSurfaceTest
 * @tc.desc: Verify function ProcessRcdSurface
 * @tc.type:FUNC
 * @tc.require:issuesI9KRF1
 */
HWTEST(RSUniRenderProcessorTest, ProcessRcdSurfaceTest, TestSize.Level1)
{
    if (RSUniRenderJudgement::IsUniRender()) {
        auto renderProcessor = std::make_shared<RSUniRenderProcessor>();
        constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[0];
        RCDSurfaceType type = RCDSurfaceType::INVALID;
        auto node = std::make_shared<RSRcdSurfaceRenderNode>(nodeId, type);
        renderProcessor->ProcessRcdSurface(*node);
        EXPECT_FALSE(renderProcessor->uniComposerAdapter_->CreateLayer(*node));
        DrawableV2::RSRcdSurfaceRenderNodeDrawable drawable(node);
        renderProcessor->ProcessRcdSurfaceForRenderThread(drawable);
        EXPECT_FALSE(renderProcessor->uniComposerAdapter_->CreateLayer(drawable));
    }
}

/**
 * @tc.name: InitForRenderThread001
 * @tc.desc: Test RSUniRenderProcessorTest.InitForRenderThread when renderEngine is nullptr
 * @tc.type:FUNC
 * @tc.require: issueIAJ1RT
 */
HWTEST(RSUniRenderProcessorTest, InitForRenderThread001, TestSize.Level1)
{
    if (RSUniRenderJudgement::IsUniRender()) {
        NodeId nodeId = 1;
        auto node = std::make_shared<RSRenderNode>(nodeId);
        DrawableV2::RSScreenRenderNodeDrawable drawable(node);
        drawable.renderParams_ = std::make_unique<RSRenderParams>(nodeId);
        ASSERT_NE(drawable.renderParams_, nullptr);

        auto renderProcessor = std::make_shared<RSUniRenderProcessor>();
        ASSERT_NE(renderProcessor, nullptr);
        renderProcessor->InitForRenderThread(drawable, nullptr);
    }
}

/**
 * @tc.name: InitForRenderThread002
 * @tc.desc: Test RSUniRenderProcessorTest.InitForRenderThread with not nullptr
 * @tc.type:FUNC
 * @tc.require: issueIAJ1RT
 */
HWTEST(RSUniRenderProcessorTest, InitForRenderThread002, TestSize.Level1)
{
    if (RSUniRenderJudgement::IsUniRender()) {
        NodeId nodeId = 1;
        auto node = std::make_shared<RSRenderNode>(nodeId);
        DrawableV2::RSScreenRenderNodeDrawable drawable(node);
        drawable.renderParams_ = nullptr;
        ASSERT_EQ(drawable.renderParams_, nullptr);
        auto renderEngine = std::make_shared<RSRenderEngine>();
        ASSERT_NE(renderEngine, nullptr);
        
        auto renderProcessor = std::make_shared<RSUniRenderProcessor>();
        ASSERT_NE(renderProcessor, nullptr);
        renderProcessor->InitForRenderThread(drawable, renderEngine);
    }
}

/**
 * @tc.name: InitForRenderThread003
 * @tc.desc: Test RSUniRenderProcessorTest.InitForRenderThread with not nullptr
 * @tc.type:FUNC
 * @tc.require: issueIAJ1RT
 */
HWTEST(RSUniRenderProcessorTest, InitForRenderThread003, TestSize.Level1)
{
    if (RSUniRenderJudgement::IsUniRender()) {
        auto renderProcessor = std::make_shared<RSUniRenderProcessor>();
        NodeId nodeId = 0;
        ScreenId screenId = 1;
        auto context = std::make_shared<RSContext>();
        std::shared_ptr<RSScreenRenderNode> screenNode =
            std::make_shared<RSScreenRenderNode>(nodeId, screenId, context->weak_from_this());
        auto screenDrawable = std::static_pointer_cast<DrawableV2::RSScreenRenderNodeDrawable>(
            screenNode->GetRenderDrawable());
        auto renderEngine = std::make_shared<RSRenderEngine>();
        ASSERT_NE(renderEngine, nullptr);
        renderEngine = nullptr;
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
HWTEST(RSUniRenderProcessorTest, ProcessScreenSurfaceForRenderThread001, TestSize.Level1)
{
    if (RSUniRenderJudgement::IsUniRender()) {
        NodeId nodeId = 1;
        auto node = std::make_shared<RSRenderNode>(nodeId);
        DrawableV2::RSScreenRenderNodeDrawable drawable(node);
        
        auto renderProcessor = std::make_shared<RSUniRenderProcessor>();
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
HWTEST(RSUniRenderProcessorTest, ProcessScreenSurfaceForRenderThread002, TestSize.Level1)
{
    if (RSUniRenderJudgement::IsUniRender()) {
        NodeId nodeId = 1;
        auto node = std::make_shared<RSRenderNode>(nodeId);
        DrawableV2::RSScreenRenderNodeDrawable drawable(node);
        drawable.renderParams_= nullptr;
        
        auto renderProcessor = std::make_shared<RSUniRenderProcessor>();
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
HWTEST(RSUniRenderProcessorTest, ProcessScreenSurfaceForRenderThread003, TestSize.Level1)
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
HWTEST(RSUniRenderProcessorTest, ProcessScreenSurfaceForRenderThread004, TestSize.Level1)
{
    if (RSUniRenderJudgement::IsUniRender()) {
        NodeId nodeId = 1;
        auto node = std::make_shared<RSRenderNode>(nodeId);
        DrawableV2::RSScreenRenderNodeDrawable drawable(node);
        drawable.renderParams_= std::make_unique<RSRenderParams>(0);
        drawable.renderParams_->SetFingerprint(true);
        drawable.surfaceHandler_ = std::make_shared<RSSurfaceHandler>(0);
        ASSERT_NE(drawable.surfaceHandler_, nullptr);

        auto renderProcessor = std::make_shared<RSUniRenderProcessor>();
        ASSERT_NE(renderProcessor, nullptr);
        auto output = std::make_shared<HdiOutput>(1);
        ASSERT_NE(output, nullptr);
        renderProcessor->uniComposerAdapter_->output_ = output;
        renderProcessor->ProcessScreenSurfaceForRenderThread(drawable);
    }
}

/**
 * @tc.name: ProcessSurfaceForRenderThread001
 * @tc.desc: Test RSUniRenderProcessorTest.ProcessSurfaceForRenderThread with not nullptr
 * @tc.type:FUNC
 * @tc.require: issueIAIT5Z
 */
HWTEST(RSUniRenderProcessorTest, ProcessSurfaceForRenderThread001, TestSize.Level1)
{
    if (RSUniRenderJudgement::IsUniRender()) {
        NodeId id = 0;
        auto node = std::make_shared<RSSurfaceRenderNode>(id);
        ASSERT_NE(node, nullptr);
        auto surfaceDrawable = std::make_shared<DrawableV2::RSSurfaceRenderNodeDrawable>(node);
        ASSERT_NE(surfaceDrawable, nullptr);
        auto renderProcessor = std::make_shared<RSUniRenderProcessor>();
        ASSERT_NE(renderProcessor, nullptr);
        renderProcessor->ProcessSurfaceForRenderThread(*surfaceDrawable);
    }
}

/**
 * @tc.name: CreateLayerForRenderThread001
 * @tc.desc: Test RSUniRenderProcessorTest.CreateLayerForRenderThread with nullptr
 * @tc.type:FUNC
 * @tc.require: issueIAIT5Z
 */
HWTEST(RSUniRenderProcessorTest, CreateLayerForRenderThread001, TestSize.Level1)
{
    if (RSUniRenderJudgement::IsUniRender()) {
        NodeId id = 0;
        auto node = std::make_shared<RSSurfaceRenderNode>(id);
        ASSERT_NE(node, nullptr);
        auto surfaceDrawable = std::make_shared<DrawableV2::RSSurfaceRenderNodeDrawable>(node);
        ASSERT_NE(surfaceDrawable, nullptr);

        surfaceDrawable->renderParams_ = nullptr;
        auto renderProcessor = std::make_shared<RSUniRenderProcessor>();
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
HWTEST(RSUniRenderProcessorTest, CreateLayerForRenderThread002, TestSize.Level1)
{
    if (RSUniRenderJudgement::IsUniRender()) {
        RSSurfaceRenderParams params(0);
        sptr<SurfaceBuffer> bufferTest = OHOS::SurfaceBuffer::Create();
        params.SetBuffer(bufferTest, {});
        NodeId id = 1;
        auto node = std::make_shared<RSSurfaceRenderNode>(id);
        ASSERT_NE(node, nullptr);
        auto surfaceDrawable = std::make_shared<DrawableV2::RSSurfaceRenderNodeDrawable>(node);
        ASSERT_EQ(surfaceDrawable->renderParams_, nullptr);
        surfaceDrawable->renderParams_ = std::make_unique<RSRenderParams>(1);

        auto renderProcessor = std::make_shared<RSUniRenderProcessor>();
        ASSERT_NE(renderProcessor, nullptr);
        renderProcessor->CreateLayerForRenderThread(*surfaceDrawable);
    }
}

/**
 * @tc.name: GetForceClientForDRM001
 * @tc.desc: Test RSUniRenderProcessorTest.GetForceClientForDRM
 * @tc.type:FUNC
 * @tc.require: issueIAIT5Z
 */
HWTEST(RSUniRenderProcessorTest, GetForceClientForDRM001, TestSize.Level1)
{
    if (RSUniRenderJudgement::IsUniRender()) {
        auto renderProcessor = std::make_shared<RSUniRenderProcessor>();
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
HWTEST(RSUniRenderProcessorTest, GetForceClientForDRM002, TestSize.Level1)
{
    if (RSUniRenderJudgement::IsUniRender()) {
        auto renderProcessor = std::make_shared<RSUniRenderProcessor>();
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
HWTEST(RSUniRenderProcessorTest, GetForceClientForDRM003, TestSize.Level1)
{
    if (RSUniRenderJudgement::IsUniRender()) {
        auto renderProcessor = std::make_shared<RSUniRenderProcessor>();
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
HWTEST(RSUniRenderProcessorTest, GetForceClientForDRM004, TestSize.Level1)
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
HWTEST(RSUniRenderProcessorTest, GetForceClientForDRM005, TestSize.Level1)
{
    if (RSUniRenderJudgement::IsUniRender()) {
        auto renderProcessor = std::make_shared<RSUniRenderProcessor>();
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
HWTEST(RSUniRenderProcessorTest, HandleTunnelLayerParameters001, TestSize.Level1)
{
    auto renderProcessor = std::make_shared<RSUniRenderProcessor>();
    ASSERT_NE(renderProcessor, nullptr);
    RSSurfaceRenderParams params(0);
    LayerInfoPtr layer = nullptr;
    renderProcessor->HandleTunnelLayerParameters(params, layer);
    EXPECT_EQ(layer, nullptr);
}
 
/**
 * @tc.name: HandleTunnelLayerParameters002
 * @tc.desc: Test RSUniRenderProcessorTest.HandleTunnelLayerParameters
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST(RSUniRenderProcessorTest, HandleTunnelLayerParameters002, TestSize.Level1)
{
    auto renderProcessor = std::make_shared<RSUniRenderProcessor>();
    ASSERT_NE(renderProcessor, nullptr);
    RSSurfaceRenderParams params(0);
    LayerInfoPtr layer;
    params.SetTunnelLayerId(0);
    renderProcessor->HandleTunnelLayerParameters(params, layer);
    EXPECT_EQ(layer->GetTunnelLayerId(), 0);
    EXPECT_EQ(layer->GetTunnelLayerProperty(), 0);
}
 
/**
 * @tc.name: HandleTunnelLayerParameters003
 * @tc.desc: Test RSUniRenderProcessorTest.HandleTunnelLayerParameters
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST(RSUniRenderProcessorTest, HandleTunnelLayerParameters003, TestSize.Level1)
{
    auto renderProcessor = std::make_shared<RSUniRenderProcessor>();
    ASSERT_NE(renderProcessor, nullptr);
    RSSurfaceRenderParams params(0);
    LayerInfoPtr layer;
    params.SetTunnelLayerId(1);
    renderProcessor->HandleTunnelLayerParameters(params, layer);
    EXPECT_EQ(layer->GetTunnelLayerId(), 1);
    ASSERT_NE(layer->GetTunnelLayerProperty(), 0);
}
 
/**
 * @tc.name: GetLayerInfo001
 * @tc.desc: Test RSUniRenderProcessorTest.GetLayerInfo
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST(RSUniRenderProcessorTest, GetLayerInfo001, TestSize.Level1)
{
    auto renderProcessor = std::make_shared<RSUniRenderProcessor>();
    ASSERT_NE(renderProcessor, nullptr);
    RSSurfaceRenderParams params(0);
    params.SetTunnelLayerId(1);
    sptr<SurfaceBuffer> buffer = nullptr;
    sptr<SurfaceBuffer> preBuffer = nullptr;
    sptr<IConsumerSurface> consumer = nullptr;
    sptr<SyncFence> acquireFence = nullptr;
 
    LayerInfoPtr result = renderProcessor->GetLayerInfo(params, buffer, preBuffer, consumer, acquireFence);
    EXPECT_EQ(result->GetType(), GraphicLayerType::GRAPHIC_LAYER_TYPE_TUNNEL);
}

/**
 * @tc.name: GetLayerInfo002
 * @tc.desc: Test RSUniRenderProcessorTest.GetLayerInfo when layer have selfDrawingDirtyRegion
 * @tc.type:FUNC
 * @tc.require: issuesICA3L1
 */
HWTEST(RSUniRenderProcessorTest, GetLayerInfo002, TestSize.Level1)
{
    auto renderProcessor = std::make_shared<RSUniRenderProcessor>();
    ASSERT_NE(renderProcessor, nullptr);
    RSSurfaceRenderParams params(0);
    params.SetTunnelLayerId(1);
    sptr<SurfaceBuffer> preBuffer = nullptr;
    sptr<IConsumerSurface> consumer = nullptr;
    sptr<SyncFence> acquireFence = nullptr;
    auto buffer = SurfaceBuffer::Create();
    auto ret = buffer->Alloc(RSUniRenderProcessorTest::requestConfig);
    ASSERT_EQ(ret, GSERROR_OK);

    std::vector<uint8_t> metaData;
    BufferSelfDrawingData data = RSUniRenderProcessorTest::defaultSelfDrawingRect;
    BufferSelfDrawingData *src = &data;
    BlobDataType test = RSUniRenderProcessorTest::defaultBlobDataType;
    test.vaddr = reinterpret_cast<uintptr_t>(src);

    ret = MetadataHelper::ConvertMetadataToVec(test, metaData);
    ASSERT_EQ(ret, GSERROR_OK);
    ret = buffer->SetMetadata(RSGpuDirtyCollectorConst::ATTRKEY_GPU_DIRTY_REGION, metaData);
    ASSERT_EQ(ret, GSERROR_OK);

    EXPECT_EQ(params.GetTunnelLayerId(), 1);
    LayerInfoPtr result = renderProcessor->GetLayerInfo(params, buffer, preBuffer, consumer, acquireFence);
    EXPECT_EQ(result->GetType(), GraphicLayerType::GRAPHIC_LAYER_TYPE_TUNNEL);
}
}