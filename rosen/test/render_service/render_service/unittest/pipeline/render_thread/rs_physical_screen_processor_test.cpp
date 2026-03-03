/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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
#include "pipeline/render_thread/rs_physical_screen_processor.h"
#include "pipeline/render_thread/rs_uni_render_engine.h"
#include "pipeline/render_thread/rs_uni_render_util.h"
#include "pipeline/main_thread/rs_main_thread.h"
#include "pipeline/main_thread/rs_render_service_listener.h"
#include "pipeline/rs_processor_factory.h"
#include "composer/composer_client/pipeline/rs_composer_client_manager.h"
#include "composer/composer_client/pipeline/rs_composer_client.h"
#include "hdi_output.h"
#include "drawable/rs_screen_render_node_drawable.h"
#include "drawable/rs_logical_display_render_node_drawable.h"
#include "feature/round_corner_display/rs_rcd_surface_render_node.h"
#include "pipeline/rs_logical_display_render_node.h"
#include "pipeline/rs_processor.h"
#include "pipeline/rs_test_util.h"
#include "screen_manager/rs_screen_info.h"
#ifdef RS_ENABLE_VK
#include "platform/ohos/backend/rs_vulkan_context.h"
#endif
#include "pipeline/rs_render_node_gc.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSPhysicalScreenProcessorTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSPhysicalScreenProcessorTest::SetUpTestCase()
{
#ifdef RS_ENABLE_VK
    RsVulkanContext::SetRecyclable(false);
#endif
    auto& renderNodeGC = RSRenderNodeGC::Instance();
    renderNodeGC.nodeBucket_ = std::queue<std::vector<RSRenderNode*>>();
    renderNodeGC.drawableBucket_ = std::queue<std::vector<DrawableV2::RSRenderNodeDrawableAdapter*>>();
    auto& memTrack = MemoryTrack::Instance();
    memTrack.memNodeMap_ = std::unordered_map<NodeId, MemoryInfo>();
    auto& memorySnapshot = MemorySnapshot::Instance();
    memorySnapshot.appMemorySnapshots_ = std::unordered_map<pid_t, MemorySnapshotInfo>();
}
void RSPhysicalScreenProcessorTest::TearDownTestCase() {}
void RSPhysicalScreenProcessorTest::SetUp() {}
void RSPhysicalScreenProcessorTest::TearDown() {}

/**
 * @tc.name: CreateAndDestroy001
 * @tc.desc: create hardware processor
 * @tc.type: FUNC
 * @tc.require: issuesI6Q871
 */
HWTEST_F(RSPhysicalScreenProcessorTest, CreateAndDestroy001, TestSize.Level1)
{
    // The best way to create RSHardwareProcessor.
    RSUniRenderThread::Instance().composerClientManager_ = std::make_shared<RSComposerClientManager>();
    auto p = RSProcessorFactory::CreateProcessor(CompositeType::HARDWARE_COMPOSITE, 0);
    EXPECT_NE(p.get(), nullptr);
}

/**
 * @tc.name: CreateAndDestroy002
 * @tc.desc: create hardware processor
 * @tc.type: FUNC
 * @tc.require: issuesI6Q871
 */
HWTEST_F(RSPhysicalScreenProcessorTest, CreateAndDestroy002, TestSize.Level1)
{
    RSProcessorFactory factory;
    auto p = factory.CreateProcessor(CompositeType::HARDWARE_COMPOSITE, 0);
    EXPECT_NE(p.get(), nullptr);
}

/**
 * @tc.name: Init
 * @tc.desc: init hardware processor
 * @tc.type: FUNC
 * @tc.require: issuesI6Q871
 */
HWTEST_F(RSPhysicalScreenProcessorTest, Init, TestSize.Level1)
{
    NodeId id = 0;
    ScreenId screenId = 1;
    auto context = std::make_shared<RSContext>();
    RSScreenRenderNode rsDisplayRenderNode(id, screenId, context);
    auto rsHardwareProcessor = RSProcessorFactory::CreateProcessor(CompositeType::HARDWARE_COMPOSITE, 0);
    auto& uniRenderThread = RSUniRenderThread::Instance();
    uniRenderThread.uniRenderEngine_ = std::make_shared<RSUniRenderEngine>();
    auto renderEngine = uniRenderThread.GetRenderEngine();
    ASSERT_NE(nullptr, rsHardwareProcessor);
    ASSERT_EQ(uniRenderThread.uniRenderEngine_, renderEngine);
    ASSERT_EQ(false, rsHardwareProcessor->Init(rsDisplayRenderNode, renderEngine));
}

/**
 * @tc.name: Init_ComposerClientPresent_OutputNull_ReturnsFalse
 * @tc.desc: With a composer client present but null output, Init should return false (adapter init fails)
 * @tc.type: FUNC
 */
HWTEST_F(RSPhysicalScreenProcessorTest, Init_ComposerClientPresent_OutputNull_ReturnsFalse, TestSize.Level1)
{
    // Prepare composer client manager and add a client for screenId
    auto &uniThread = RSUniRenderThread::Instance();
    uniThread.uniRenderEngine_ = std::make_shared<RSUniRenderEngine>();
    auto renderEngine = uniThread.GetRenderEngine();
    uniThread.composerClientManager_ = std::make_shared<RSComposerClientManager>();
    ScreenId screenId = 2;
    sptr<IRSRenderToComposerConnection> conn = nullptr;
    auto client = std::make_shared<RSComposerClient>(conn);
    uniThread.composerClientManager_->AddComposerClient(screenId, client);

    // Create processor bound to the same screenId
    auto processor = RSProcessorFactory::CreateProcessor(CompositeType::HARDWARE_COMPOSITE, screenId);
    ASSERT_NE(processor, nullptr);
    NodeId nodeId = 10;
    auto context = std::make_shared<RSContext>();
    RSScreenRenderNode rsDisplayRenderNode(nodeId, screenId, context);
    // Expect Init returns false when renderEngine is nullptr
    bool ret = processor->Init(rsDisplayRenderNode, nullptr);
    EXPECT_FALSE(ret);
    // Expect Init returns false when RSComposerClient::GetOutput() is nullptr
    ret = processor->Init(rsDisplayRenderNode, renderEngine);
    EXPECT_FALSE(ret);
    // Verify composer client exists via manager
    auto &mgr = RSUniRenderThread::Instance().composerClientManager_;
    ASSERT_NE(mgr->GetComposerClient(screenId), nullptr);
}

/**
 * @tc.name: ProcessSurface001
 * @tc.desc: call ProcessSurface
 * @tc.type: FUNC
 * @tc.require: IssueI5VYCA
 */
HWTEST_F(RSPhysicalScreenProcessorTest, ProcessSurface001, TestSize.Level1)
{
    auto rsHardwareProcessor = RSProcessorFactory::CreateProcessor(CompositeType::HARDWARE_COMPOSITE, 0);
    ASSERT_NE(rsHardwareProcessor, nullptr);
    RSSurfaceRenderNodeConfig config;
    RSSurfaceRenderNode rsSurfaceRenderNode(config);
    rsHardwareProcessor->ProcessSurface(rsSurfaceRenderNode);
    sptr<IConsumerSurface> surface = IConsumerSurface::Create(config.name);
    rsSurfaceRenderNode.GetRSSurfaceHandler()->SetConsumer(surface);
    rsHardwareProcessor->ProcessSurface(rsSurfaceRenderNode);
    auto consumer = rsSurfaceRenderNode.GetRSSurfaceHandler()->GetConsumer();
    GraphicExtDataHandle handle;
    handle.fd = -1;
    handle.reserveInts = 1;
    consumer->SetTunnelHandle(&handle);
    rsHardwareProcessor->ProcessSurface(rsSurfaceRenderNode);
}

/**
 * @tc.name: ProcessSurface002
 * @tc.desc: call ProcessScreenSurface
 * @tc.type: FUNC
 * @tc.require: IssueI5VYCA
 */
HWTEST_F(RSPhysicalScreenProcessorTest, ProcessSurface002, TestSize.Level1)
{
    auto rsUniRenderProcessor = RSProcessorFactory::CreateProcessor(CompositeType::UNI_RENDER_COMPOSITE, 0);
    ASSERT_NE(rsUniRenderProcessor, nullptr);
    NodeId id = 0;
    ScreenId screenId = 1;
    auto context = std::make_shared<RSContext>();
    RSScreenRenderNode rsDisplayRenderNode(id, screenId, context);
    rsUniRenderProcessor->ProcessScreenSurface(rsDisplayRenderNode);
}

/**
 * @tc.name: ProcessSurface003
 * @tc.desc: call ProcessSurface
 * @tc.type: FUNC
 * @tc.require: issuesI6Q871
 */
HWTEST_F(RSPhysicalScreenProcessorTest, ProcessSurface003, TestSize.Level1)
{
    auto rsHardwareProcessor = RSProcessorFactory::CreateProcessor(CompositeType::HARDWARE_COMPOSITE, 0);
    ASSERT_NE(rsHardwareProcessor, nullptr);
    RSSurfaceRenderNodeConfig config;
    RSSurfaceRenderNode rsSurfaceRenderNode(config);
    rsHardwareProcessor->ProcessSurface(rsSurfaceRenderNode);
}

/**
 * @tc.name: ProcessSurface004
 * @tc.desc: call ProcessSurface
 * @tc.type: FUNC
 * @tc.require: issuesI6Q871
 */
HWTEST_F(RSPhysicalScreenProcessorTest, ProcessSurface004, TestSize.Level1)
{
    auto rsHardwareProcessor = RSProcessorFactory::CreateProcessor(CompositeType::HARDWARE_COMPOSITE, 0);
    ASSERT_NE(rsHardwareProcessor, nullptr);
    RSSurfaceRenderNodeConfig config;
    RSSurfaceRenderNode rsSurfaceRenderNode(config);
    rsHardwareProcessor->ProcessSurface(rsSurfaceRenderNode);
}

/**
 * @tc.name: ProcessSurface005
 * @tc.desc: call ProcessSurface
 * @tc.type: FUNC
 * @tc.require: issuesI6Q871
 */
HWTEST_F(RSPhysicalScreenProcessorTest, ProcessSurface005, TestSize.Level1)
{
    auto rsHardwareProcessor = RSProcessorFactory::CreateProcessor(CompositeType::HARDWARE_COMPOSITE, 0);
    ASSERT_NE(rsHardwareProcessor, nullptr);
    RSSurfaceRenderNodeConfig config;
    RSSurfaceRenderNode rsSurfaceRenderNode(config);
    rsHardwareProcessor->ProcessSurface(rsSurfaceRenderNode);
}

/**
 * @tc.name: ProcessSurface006
 * @tc.desc: call ProcessSurface
 * @tc.type: FUNC
 * @tc.require: issuesI6Q871
 */
HWTEST_F(RSPhysicalScreenProcessorTest, ProcessSurface006, TestSize.Level1)
{
    auto rsHardwareProcessor = RSProcessorFactory::CreateProcessor(CompositeType::HARDWARE_COMPOSITE, 0);
    ASSERT_NE(rsHardwareProcessor, nullptr);
    RSSurfaceRenderNodeConfig config;
    RSSurfaceRenderNode rsSurfaceRenderNode(config);
    rsHardwareProcessor->ProcessSurface(rsSurfaceRenderNode);
}

/**
 * @tc.name: ProcessSurface007
 * @tc.desc: call ProcessSurface
 * @tc.type: FUNC
 * @tc.require: issuesI6Q871
 */
HWTEST_F(RSPhysicalScreenProcessorTest, ProcessSurface007, TestSize.Level1)
{
    auto rsHardwareProcessor = RSProcessorFactory::CreateProcessor(CompositeType::HARDWARE_COMPOSITE, 0);
    ASSERT_NE(rsHardwareProcessor, nullptr);
    RSSurfaceRenderNodeConfig config;
    RSSurfaceRenderNode rsSurfaceRenderNode(config);
    rsHardwareProcessor->ProcessSurface(rsSurfaceRenderNode);
}

/**
 * @tc.name: ProcessSurface008
 * @tc.desc: call ProcessSurface
 * @tc.type: FUNC
 * @tc.require: issuesI6Q871
 */
HWTEST_F(RSPhysicalScreenProcessorTest, ProcessSurface008, TestSize.Level1)
{
    auto rsHardwareProcessor = RSProcessorFactory::CreateProcessor(CompositeType::HARDWARE_COMPOSITE, 0);
    ASSERT_NE(rsHardwareProcessor, nullptr);
    RSSurfaceRenderNodeConfig config;
    RSSurfaceRenderNode rsSurfaceRenderNode(config);
    rsHardwareProcessor->ProcessSurface(rsSurfaceRenderNode);
}

/**
 * @tc.name: ProcessScreenSurface
 * @tc.desc: call ProcessSurface
 * @tc.type: FUNC
 * @tc.require: issuesI6Q871
 */
HWTEST_F(RSPhysicalScreenProcessorTest, ProcessScreenSurface, TestSize.Level1)
{
    NodeId id = 0;
    ScreenId screenId = 1;
    auto context = std::make_shared<RSContext>();
    RSScreenRenderNode rsDisplayRenderNode(id, screenId, context);
    auto rsHardwareProcessor = RSProcessorFactory::CreateProcessor(CompositeType::SOFTWARE_COMPOSITE, 0);
    ASSERT_NE(nullptr, rsHardwareProcessor);
    rsHardwareProcessor->ProcessScreenSurface(rsDisplayRenderNode);
}

/**
 * @tc.name: PostProcess001
 * @tc.desc: call PostProcess
 * @tc.type: FUNC
 * @tc.require: issuesI6Q871
 */
HWTEST_F(RSPhysicalScreenProcessorTest, PostProcess001, TestSize.Level1)
{
    auto rsHardwareProcessor = RSProcessorFactory::CreateProcessor(CompositeType::HARDWARE_COMPOSITE, 0);
    ASSERT_NE(rsHardwareProcessor, nullptr);
    rsHardwareProcessor->PostProcess();
}

/**
 * @tc.name: ProcessRcdSurface
 * @tc.desc: call ProcessRcdSurface
 * @tc.type: FUNC
 * @tc.require: issueI9JY8B
 */
HWTEST_F(RSPhysicalScreenProcessorTest, ProcessRcdSurface, TestSize.Level1)
{
    auto rsHardwareProcessor = RSProcessorFactory::CreateProcessor(CompositeType::HARDWARE_COMPOSITE, 0);
    ASSERT_NE(rsHardwareProcessor, nullptr);
    NodeId id = 1;
    RCDSurfaceType type = RCDSurfaceType::BOTTOM;
    RSRcdSurfaceRenderNode node(id, type);
    rsHardwareProcessor->ProcessRcdSurface(node);
}

/**
 * @tc.name: Redraw
 * @tc.desc: call Redraw
 * @tc.type: FUNC
 * @tc.require: issueI9JY8B
 */
HWTEST_F(RSPhysicalScreenProcessorTest, Redraw, TestSize.Level1)
{
    std::shared_ptr<RSPhysicalScreenProcessor> rsHardwareProcessor = std::make_shared<RSPhysicalScreenProcessor>(0);
    sptr<Surface> surface;
    std::vector<RSLayerPtr> layers;
    rsHardwareProcessor->Redraw(surface, layers);
    surface = Surface::CreateSurfaceAsConsumer();
    ASSERT_NE(surface, nullptr);
    rsHardwareProcessor->renderEngine_ = std::make_shared<RSUniRenderEngine>();
    rsHardwareProcessor->Redraw(surface, layers);
    ASSERT_NE(rsHardwareProcessor->renderEngine_, nullptr);
}

/**
 * @tc.name: Redraw_SurfaceNonNull_EngineNull_EarlyReturn
 * @tc.desc: Redraw should early return when renderEngine_ is nullptr even if surface is valid
 * @tc.type: FUNC
 */
HWTEST_F(RSPhysicalScreenProcessorTest, Redraw_SurfaceNonNull_EngineNull_EarlyReturn, TestSize.Level1)
{
    auto processor = std::make_shared<RSPhysicalScreenProcessor>(0);
    std::vector<RSLayerPtr> layers;
    auto surface = Surface::CreateSurfaceAsConsumer();
    ASSERT_NE(surface, nullptr);
    processor->renderEngine_ = nullptr;
    processor->Redraw(surface, layers);
    ASSERT_EQ(processor->renderEngine_, nullptr);
}

/**
 * @tc.name: Redraw_RequestFrameNull_EarlyReturn
 * @tc.desc: With non-null engine but not initialized, RequestFrame returns nullptr and Redraw early returns
 * @tc.type: FUNC
 */
HWTEST_F(RSPhysicalScreenProcessorTest, Redraw_RequestFrameNull_EarlyReturn, TestSize.Level1)
{
    auto processor = std::make_shared<RSPhysicalScreenProcessor>(0);
    std::vector<RSLayerPtr> layers;
    auto consumer = IConsumerSurface::Create("rf-ut");
    auto producer = consumer->GetProducer();
    auto pSurface = Surface::CreateSurfaceAsProducer(producer);
    ASSERT_NE(pSurface, nullptr);
    // Set a render engine without calling Init to make RequestFrame return nullptr
    processor->renderEngine_ = std::make_shared<RSUniRenderEngine>();
    processor->Redraw(pSurface, layers);
}

/**
 * @tc.name: Redraw_ScreenMatrix_Path
 * @tc.desc: When mirroredScreenInfo is invalid, screenTransformMatrix is used and drawing proceeds
 * @tc.type: FUNC
 */
HWTEST_F(RSPhysicalScreenProcessorTest, Redraw_ScreenMatrix_Path, TestSize.Level1)
{
    auto processor = std::make_shared<RSPhysicalScreenProcessor>(0);
    std::vector<RSLayerPtr> layers;
    auto consumer = IConsumerSurface::Create("rf-ut2");
    auto producer = consumer->GetProducer();
    auto pSurface = Surface::CreateSurfaceAsProducer(producer);
    ASSERT_NE(pSurface, nullptr);
    // Initialize render engine to obtain a valid frame and canvas
    auto engine = std::make_shared<RSUniRenderEngine>();
    engine->Init();
    processor->renderEngine_ = engine;
    // Ensure mirroredScreenInfo invalid so screenTransformMatrix_ path is taken
    processor->mirroredScreenInfo_.id = INVALID_SCREEN_ID;
    processor->Redraw(pSurface, layers);
    ASSERT_NE(processor->renderEngine_, nullptr);
}

/**
 * @tc.name: Redraw_MirrorMatrix_Path
 * @tc.desc: When mirroredScreenInfo is valid, mirrorAdaptiveMatrix is used during drawing
 * @tc.type: FUNC
 */
HWTEST_F(RSPhysicalScreenProcessorTest, Redraw_MirrorMatrix_Path, TestSize.Level1)
{
    auto processor = std::make_shared<RSPhysicalScreenProcessor>(0);
    std::vector<RSLayerPtr> layers;
    auto consumer = IConsumerSurface::Create("rf-ut3");
    auto producer = consumer->GetProducer();
    auto pSurface = Surface::CreateSurfaceAsProducer(producer);
    ASSERT_NE(pSurface, nullptr);
    auto engine = std::make_shared<RSUniRenderEngine>();
    engine->Init();
    processor->renderEngine_ = engine;
    // Configure screen and mirrored info then calculate matrix
    processor->screenInfo_.width = 100;
    processor->screenInfo_.height = 80;
    processor->mirroredScreenInfo_.id = 1;
    processor->mirroredScreenInfo_.width = 50;
    processor->mirroredScreenInfo_.height = 40;
    processor->CalculateMirrorAdaptiveMatrix();
    processor->Redraw(pSurface, layers);
    ASSERT_NE(processor->renderEngine_, nullptr);
}

/**
 * @tc.name: RequestPerf
 * @tc.desc: test results of RequestPerf
 * @tc.type: FUNC
 * @tc.require: issueI9JY8B
 */
HWTEST_F(RSPhysicalScreenProcessorTest, RequestPerf, TestSize.Level1)
{
    uint32_t layerLevel[] = { 0, 1, 2, 3 };
    bool onOffTag = true;
    int total = 4;
    int num = 0;
    for (uint32_t level : layerLevel) {
        RSUniRenderUtil::RequestPerf(level, onOffTag);
        num++;
    }
    EXPECT_TRUE(num == total);
}

/**
 * @tc.name: CalculateScreenTransformMatrix
 * @tc.desc: test results of CalculateScreenTransformMatrix
 * @tc.type: FUNC
 * @tc.require: issueI9JY8B
 */
HWTEST_F(RSPhysicalScreenProcessorTest, CalculateScreenTransformMatrix, TestSize.Level1)
{
    auto rsHardwareProcessor = RSProcessorFactory::CreateProcessor(CompositeType::HARDWARE_COMPOSITE, 0);
    NodeId displayNodeId = 0;
    RSDisplayNodeConfig config;
    auto context = std::make_shared<RSContext>();
    auto rsLogicalDisplayRenderNode = std::make_shared<RSLogicalDisplayRenderNode>(displayNodeId, config,
        context->weak_from_this());
    rsHardwareProcessor->CalculateScreenTransformMatrix(*rsLogicalDisplayRenderNode);
    ASSERT_EQ(rsHardwareProcessor->screenInfo_.width, 0);
}

/**
 * @tc.name: CalculateMirrorAdaptiveCoefficient
 * @tc.desc: test results of CalculateMirrorAdaptiveCoefficient
 * @tc.type: FUNC
 * @tc.require: issueI9JY8B
 */
HWTEST_F(RSPhysicalScreenProcessorTest, CalculateMirrorAdaptiveCoefficient, TestSize.Level1)
{
    auto processor = RSProcessorFactory::CreateProcessor(CompositeType::HARDWARE_COMPOSITE, 0);
    float curWidth = 2.0f;
    float curHeight = 1.0f;
    float mirroredWidth = .0f;
    float mirroredHeight = .0f;
    processor->CalculateMirrorAdaptiveCoefficient(curWidth, curHeight, mirroredWidth, mirroredHeight);
    mirroredHeight =1.0f;
    processor->CalculateMirrorAdaptiveCoefficient(curWidth, curHeight, mirroredWidth, mirroredHeight);
    mirroredWidth = 1.0f;
    processor->CalculateMirrorAdaptiveCoefficient(curWidth, curHeight, mirroredWidth, mirroredHeight);
    ASSERT_EQ(processor->mirrorAdaptiveCoefficient_, 1);
}

/**
 * @tc.name: SetDisplayHasSecSurface
 * @tc.desc: test results of SetDisplayHasSecSurface
 * @tc.type: FUNC
 * @tc.require: issueI9JY8B
 */
HWTEST_F(RSPhysicalScreenProcessorTest, SetDisplayHasSecSurface, TestSize.Level1)
{
    auto processor = RSProcessorFactory::CreateProcessor(CompositeType::HARDWARE_COMPOSITE, 0);
    processor->SetSecurityDisplay(true);
    ASSERT_TRUE(processor->isSecurityDisplay_);

    processor->SetDisplayHasSecSurface(true);
    ASSERT_TRUE(processor->displayHasSecSurface_);
}

/**
 * @tc.name: UpdateMirrorInfo_BaseBehavior
 * @tc.desc: Verify base UpdateMirrorInfo returns false on null params and true otherwise
 * @tc.type: FUNC
 */
HWTEST_F(RSPhysicalScreenProcessorTest, UpdateMirrorInfo_BaseBehavior, TestSize.Level1)
{
    NodeId nodeId = 2;
    RSDisplayNodeConfig config;
    auto context = std::make_shared<RSContext>();
    auto displayNode = std::make_shared<RSLogicalDisplayRenderNode>(nodeId, config, context->weak_from_this());
    auto drawableBase = DrawableV2::RSRenderNodeDrawable::OnGenerate(displayNode);
    auto displayDrawable = static_cast<DrawableV2::RSLogicalDisplayRenderNodeDrawable*>(drawableBase);
    auto processor = RSProcessorFactory::CreateProcessor(CompositeType::HARDWARE_COMPOSITE, 0);
    ASSERT_NE(displayDrawable, nullptr);
    bool ret = processor->UpdateMirrorInfo(*displayDrawable);
    EXPECT_FALSE(ret);
}

/**
 * @tc.name: MirrorAdaptiveMatrix_Rotation0
 * @tc.desc: Verify mirror adaptive translation and scale for ROTATION_0
 * @tc.type: FUNC
 */
HWTEST_F(RSPhysicalScreenProcessorTest, MirrorAdaptiveMatrix_Rotation0, TestSize.Level1)
{
    auto processor = RSProcessorFactory::CreateProcessor(CompositeType::HARDWARE_COMPOSITE, 0);
    processor->screenInfo_.width = 100;
    processor->screenInfo_.height = 80;
    processor->screenInfo_.rotation = ScreenRotation::ROTATION_0;
    processor->mirroredScreenInfo_.width = 50;
    processor->mirroredScreenInfo_.height = 40;
    processor->CalculateMirrorAdaptiveMatrix();
    EXPECT_NEAR(processor->mirrorAdaptiveMatrix_.Get(Drawing::Matrix::TRANS_X), 0.0f, 1e-3f);
    EXPECT_NEAR(processor->mirrorAdaptiveMatrix_.Get(Drawing::Matrix::TRANS_Y), 0.0f, 1e-3f);
    EXPECT_NEAR(processor->mirrorAdaptiveMatrix_.Get(Drawing::Matrix::SCALE_X), 2.0f, 1e-3f);
    EXPECT_NEAR(processor->mirrorAdaptiveMatrix_.Get(Drawing::Matrix::SCALE_Y), 2.0f, 1e-3f);
}

/**
 * @tc.name: MirrorAdaptiveMatrix_Rotation90
 * @tc.desc: Verify mirror adaptive translation for ROTATION_90
 * @tc.type: FUNC
 */
HWTEST_F(RSPhysicalScreenProcessorTest, MirrorAdaptiveMatrix_Rotation90, TestSize.Level1)
{
    auto processor = RSProcessorFactory::CreateProcessor(CompositeType::HARDWARE_COMPOSITE, 0);
    processor->screenInfo_.width = 100;
    processor->screenInfo_.height = 80;
    processor->screenInfo_.rotation = ScreenRotation::ROTATION_90;
    processor->mirroredScreenInfo_.width = 50;
    processor->mirroredScreenInfo_.height = 40;
    processor->CalculateMirrorAdaptiveMatrix();
    EXPECT_NEAR(processor->mirrorAdaptiveMatrix_.Get(Drawing::Matrix::TRANS_X), -90.0f, 1e-3f);
    EXPECT_NEAR(processor->mirrorAdaptiveMatrix_.Get(Drawing::Matrix::TRANS_Y), 10.0f, 1e-3f);
}

/**
 * @tc.name: MirrorAdaptiveMatrix_Rotation180
 * @tc.desc: Verify mirror adaptive translation for ROTATION_180
 * @tc.type: FUNC
 */
HWTEST_F(RSPhysicalScreenProcessorTest, MirrorAdaptiveMatrix_Rotation180, TestSize.Level1)
{
    auto processor = RSProcessorFactory::CreateProcessor(CompositeType::HARDWARE_COMPOSITE, 0);
    processor->screenInfo_.width = 100;
    processor->screenInfo_.height = 80;
    processor->screenInfo_.rotation = ScreenRotation::ROTATION_180;
    processor->mirroredScreenInfo_.width = 50;
    processor->mirroredScreenInfo_.height = 40;
    processor->CalculateMirrorAdaptiveMatrix();
    EXPECT_NEAR(processor->mirrorAdaptiveMatrix_.Get(Drawing::Matrix::TRANS_X), -100.0f, 1e-3f);
    EXPECT_NEAR(processor->mirrorAdaptiveMatrix_.Get(Drawing::Matrix::TRANS_Y), -80.0f, 1e-3f);
}

/**
 * @tc.name: MirrorAdaptiveMatrix_Rotation270
 * @tc.desc: Verify mirror adaptive translation for ROTATION_270
 * @tc.type: FUNC
 */
HWTEST_F(RSPhysicalScreenProcessorTest, MirrorAdaptiveMatrix_Rotation270, TestSize.Level1)
{
    auto processor = RSProcessorFactory::CreateProcessor(CompositeType::HARDWARE_COMPOSITE, 0);
    processor->screenInfo_.width = 100;
    processor->screenInfo_.height = 80;
    processor->screenInfo_.rotation = ScreenRotation::ROTATION_270;
    processor->mirroredScreenInfo_.width = 50;
    processor->mirroredScreenInfo_.height = 40;
    processor->CalculateMirrorAdaptiveMatrix();
    EXPECT_NEAR(processor->mirrorAdaptiveMatrix_.Get(Drawing::Matrix::TRANS_X), -10.0f, 1e-3f);
    EXPECT_NEAR(processor->mirrorAdaptiveMatrix_.Get(Drawing::Matrix::TRANS_Y), -90.0f, 1e-3f);
}

/**
 * @tc.name: ProcessScreenSurface_Physical_NoCrash
 * @tc.desc: Call RSPhysicalScreenProcessor::ProcessScreenSurface to cover logging path
 * @tc.type: FUNC
 */
HWTEST_F(RSPhysicalScreenProcessorTest, ProcessScreenSurface_Physical_NoCrash, TestSize.Level1)
{
    auto processor = RSProcessorFactory::CreateProcessor(CompositeType::HARDWARE_COMPOSITE, 0);
    ASSERT_NE(processor, nullptr);
    NodeId id = 10;
    ScreenId screenId = 0;
    auto context = std::make_shared<RSContext>();
    RSScreenRenderNode rsDisplayRenderNode(id, screenId, context);
    processor->ProcessScreenSurface(rsDisplayRenderNode);
}

/**
 * @tc.name: ProcessSurface_RenderEngineNotNull_And_LayerNull
 * @tc.desc: Ensure SetColorFilterMode branch executes when renderEngine_ is not null, and layer==nullptr path taken
 * @tc.type: FUNC
 */
HWTEST_F(RSPhysicalScreenProcessorTest, ProcessSurface_RenderEngineNotNull_And_LayerNull, TestSize.Level1)
{
    auto processor = RSProcessorFactory::CreateProcessor(CompositeType::HARDWARE_COMPOSITE, 999);
    ASSERT_NE(processor, nullptr);
    // Force renderEngine_ non-null to hit SetColorFilterMode path
    processor->renderEngine_ = std::make_shared<RSUniRenderEngine>();
    EXPECT_NE(processor->renderEngine_, nullptr);

    // Create a surface node with minimal setup to make CreateLayer return nullptr
    RSSurfaceRenderNodeConfig config;
    RSSurfaceRenderNode node(config);
    auto physical = RSProcessor::ReinterpretCast<RSPhysicalScreenProcessor>(processor);
    ASSERT_NE(physical, nullptr);
    physical->ProcessSurface(node);
    // layer is nullptr path should be taken, so layers_ remains unchanged
    EXPECT_EQ(physical->layers_.size(), 0U);
}

/**
 * @tc.name: ProcessSurface_LayerNonNull_PositivePath
 * @tc.desc: Ensure CreateLayer returns non-null and layers_ grows under positive path (tunnel handle + valid rects)
 * @tc.type: FUNC
 */
HWTEST_F(RSPhysicalScreenProcessorTest, ProcessSurface_LayerNonNull_PositivePath, TestSize.Level1)
{
    // Prepare composer client with a valid HdiOutput and register into manager
    ScreenId screenId = 7;
    auto &uniThread = RSUniRenderThread::Instance();
    uniThread.composerClientManager_ = std::make_shared<RSComposerClientManager>();
    sptr<IRSRenderToComposerConnection> conn = nullptr;
    auto client = std::make_shared<RSComposerClient>(conn);
    client->SetOutput(std::make_shared<HdiOutput>(screenId));
    uniThread.composerClientManager_->AddComposerClient(screenId, client);

    // Create hardware processor bound to the same screenId
    auto processor = RSProcessorFactory::CreateProcessor(CompositeType::HARDWARE_COMPOSITE, screenId);
    ASSERT_NE(processor, nullptr);

    // Init processor to pass output into adapter
    auto renderEngine = std::make_shared<RSUniRenderEngine>();
    NodeId nodeId = 1001;
    auto context = std::make_shared<RSContext>();
    RSScreenRenderNode screenNode(nodeId, screenId, context);
    (void)processor->Init(screenNode, renderEngine);

    // Prepare a surface node with consumer, tunnel handle and valid src/dst rects
    RSSurfaceRenderNodeConfig cfg;
    cfg.id = 1;
    cfg.name = "surface";
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    sptr<IBufferConsumerListener> listener =
        new RSRenderServiceListener(std::weak_ptr<RSSurfaceRenderNode>(surfaceNode), nullptr);
    auto consumer = IConsumerSurface::Create(cfg.name);
    consumer->RegisterConsumerListener(listener);
    surfaceNode->GetRSSurfaceHandler()->SetConsumer(consumer);
    GraphicExtDataHandle handle;
    handle.fd = -1;
    handle.reserveInts = 1;
    consumer->SetTunnelHandle(&handle);
    surfaceNode->SetSrcRect(RectI(0, 80, 2000, 1000));
    surfaceNode->SetDstRect(RectI(0, 80, 2000, 1000));

    // Execute ProcessSurface and verify layer is created and appended
    auto physical2 = RSProcessor::ReinterpretCast<RSPhysicalScreenProcessor>(processor);
    ASSERT_NE(physical2, nullptr);
    size_t before = physical2->layers_.size();
    EXPECT_NE(surfaceNode->GetRSSurfaceHandler()->GetConsumer(), nullptr);
    ScreenInfo info;
    info.width = 2160;
    info.height = 1080;
    info.phyWidth = 2160;
    info.phyHeight = 1080;
    info.colorGamut = ScreenColorGamut::COLOR_GAMUT_SRGB;
    info.state = ScreenState::UNKNOWN;
    info.rotation = ScreenRotation::ROTATION_0;
    auto hdiOutput = HdiOutput::CreateHdiOutput(0);
    hdiOutput->Init();
    physical2->composerAdapter_->Init(info, 1.0f, nullptr, hdiOutput);
    physical2->ProcessSurface(*surfaceNode);
    EXPECT_EQ(physical2->layers_.size(), before + 1);
    ASSERT_GT(physical2->layers_.size(), 0);
    EXPECT_NE(physical2->layers_.back(), nullptr);
    consumer->UnregisterConsumerListener();
}

/**
 * @tc.name: Init_RenderEngineNull_ReturnsFalse
 * @tc.desc: Test Init when renderEngine is nullptr under GPU enabled build
 *           The RSProcessor::Init should fail when renderEngine is null
 * @tc.type: FUNC
 * @tc.require: issue41
 */
HWTEST_F(RSPhysicalScreenProcessorTest, Init_RenderEngineNull_ReturnsFalse, TestSize.Level2)
{
#ifdef RS_ENABLE_GPU
    // Prepare composer client manager and client
    ScreenId screenId = 10;
    auto& uniThread = RSUniRenderThread::Instance();
    uniThread.composerClientManager_ = std::make_shared<RSComposerClientManager>();
    sptr<IRSRenderToComposerConnection> conn = nullptr;
    auto client = std::make_shared<RSComposerClient>(conn);
    client->SetOutput(std::make_shared<HdiOutput>(screenId));
    uniThread.composerClientManager_->AddComposerClient(screenId, client);

    auto processor = RSProcessorFactory::CreateProcessor(CompositeType::HARDWARE_COMPOSITE, screenId);
    ASSERT_NE(processor, nullptr);

    NodeId nodeId = 100;
    auto context = std::make_shared<RSContext>();
    RSScreenRenderNode screenNode(nodeId, screenId, context);

    // Init with nullptr renderEngine should return false under RS_ENABLE_GPU
    bool ret = processor->Init(screenNode, nullptr);
    EXPECT_FALSE(ret);
#endif
}

/**
 * @tc.name: Init_ComposerClientNull_ReturnsFalse
 * @tc.desc: Test Init when composerClient_ is nullptr
 *           The if (composerClient_ == nullptr) branch at line 37 should be true
 * @tc.type: FUNC
 * @tc.require: issue41
 */
HWTEST_F(RSPhysicalScreenProcessorTest, Init_ComposerClientNull_ReturnsFalse, TestSize.Level2)
{
    // Don't set up composerClientManager, so composerClient_ will be nullptr
    ScreenId screenId = 11;
    auto processor = RSProcessorFactory::CreateProcessor(CompositeType::HARDWARE_COMPOSITE, screenId);
    ASSERT_NE(processor, nullptr);

    NodeId nodeId = 101;
    auto context = std::make_shared<RSContext>();
    RSScreenRenderNode screenNode(nodeId, screenId, context);
    auto renderEngine = std::make_shared<RSUniRenderEngine>();

    // Init should return false when composerClient_ is nullptr
    bool ret = processor->Init(screenNode, renderEngine);
    EXPECT_FALSE(ret);
}

/**
 * @tc.name: Init_WithValidComposerClient_And_NullOutput
 * @tc.desc: Test Init when composerClient_ exists but GetOutput() returns nullptr
 *           The composerAdapter_->Init should return false
 * @tc.type: FUNC
 * @tc.require: issue41
 */
HWTEST_F(RSPhysicalScreenProcessorTest, Init_WithValidComposerClient_And_NullOutput, TestSize.Level2)
{
    // Prepare composer client manager and client without setting output
    ScreenId screenId = 12;
    auto& uniThread = RSUniRenderThread::Instance();
    uniThread.composerClientManager_ = std::make_shared<RSComposerClientManager>();
    sptr<IRSRenderToComposerConnection> conn = nullptr;
    auto client = std::make_shared<RSComposerClient>(conn);
    // Don't set output - client->GetOutput() will return nullptr
    uniThread.composerClientManager_->AddComposerClient(screenId, client);

    auto processor = RSProcessorFactory::CreateProcessor(CompositeType::HARDWARE_COMPOSITE, screenId);
    ASSERT_NE(processor, nullptr);

    NodeId nodeId = 102;
    auto context = std::make_shared<RSContext>();
    RSScreenRenderNode screenNode(nodeId, screenId, context);
    auto renderEngine = std::make_shared<RSUniRenderEngine>();

    // Init should return false when GetOutput() returns nullptr
    bool ret = processor->Init(screenNode, renderEngine);
    EXPECT_FALSE(ret);
}
} // namespace OHOS::Rosen