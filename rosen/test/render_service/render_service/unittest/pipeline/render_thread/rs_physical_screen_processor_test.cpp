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
#include "pipeline/rs_processor_factory.h"
#include "feature/round_corner_display/rs_rcd_surface_render_node.h"

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
    auto p = RSProcessorFactory::CreateProcessor(RSDisplayRenderNode::CompositeType::HARDWARE_COMPOSITE);
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
    auto p = factory.CreateProcessor(RSDisplayRenderNode::CompositeType::HARDWARE_COMPOSITE);
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
    RSDisplayNodeConfig config;
    NodeId id = 0;
    int32_t offsetX = 0;
    int32_t offsetY = 0;
    RSDisplayRenderNode rsDisplayRenderNode(id, config);
    auto rsHardwareProcessor = RSProcessorFactory::CreateProcessor(RSDisplayRenderNode::CompositeType::
        HARDWARE_COMPOSITE);
    auto& uniRenderThread = RSUniRenderThread::Instance();
    uniRenderThread.uniRenderEngine_ = std::make_shared<RSUniRenderEngine>();
    auto renderEngine = uniRenderThread.GetRenderEngine();
    ASSERT_NE(nullptr, rsHardwareProcessor);
    ASSERT_EQ(uniRenderThread.uniRenderEngine_, renderEngine);
    ASSERT_EQ(false, rsHardwareProcessor->Init(rsDisplayRenderNode, offsetX, offsetY, INVALID_SCREEN_ID, renderEngine));
}

/**
 * @tc.name: ProcessSurface001
 * @tc.desc: call ProcessSurface
 * @tc.type: FUNC
 * @tc.require: IssueI5VYCA
 */
HWTEST_F(RSPhysicalScreenProcessorTest, ProcessSurface001, TestSize.Level1)
{
    auto rsHardwareProcessor = RSProcessorFactory::CreateProcessor(RSDisplayRenderNode::CompositeType::
        HARDWARE_COMPOSITE);
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
 * @tc.desc: call ProcessDisplaySurface
 * @tc.type: FUNC
 * @tc.require: IssueI5VYCA
 */
HWTEST_F(RSPhysicalScreenProcessorTest, ProcessSurface002, TestSize.Level1)
{
    auto rsUniRenderProcessor = RSProcessorFactory::CreateProcessor(RSDisplayRenderNode::CompositeType::
        UNI_RENDER_COMPOSITE);
    ASSERT_NE(rsUniRenderProcessor, nullptr);
    NodeId id = 0;
    RSDisplayNodeConfig config;
    RSDisplayRenderNode rsDisplayRenderNode(id, config);
    rsUniRenderProcessor->ProcessDisplaySurface(rsDisplayRenderNode);
}

/**
 * @tc.name: ProcessSurface003
 * @tc.desc: call ProcessSurface
 * @tc.type: FUNC
 * @tc.require: issuesI6Q871
 */
HWTEST_F(RSPhysicalScreenProcessorTest, ProcessSurface003, TestSize.Level1)
{
    auto rsHardwareProcessor = RSProcessorFactory::CreateProcessor(RSDisplayRenderNode::CompositeType::
        HARDWARE_COMPOSITE);
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
    auto rsHardwareProcessor = RSProcessorFactory::CreateProcessor(RSDisplayRenderNode::CompositeType::
        HARDWARE_COMPOSITE);
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
    auto rsHardwareProcessor = RSProcessorFactory::CreateProcessor(RSDisplayRenderNode::CompositeType::
        HARDWARE_COMPOSITE);
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
    auto rsHardwareProcessor = RSProcessorFactory::CreateProcessor(RSDisplayRenderNode::CompositeType::
        HARDWARE_COMPOSITE);
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
    auto rsHardwareProcessor = RSProcessorFactory::CreateProcessor(RSDisplayRenderNode::CompositeType::
        HARDWARE_COMPOSITE);
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
    auto rsHardwareProcessor = RSProcessorFactory::CreateProcessor(RSDisplayRenderNode::CompositeType::
        HARDWARE_COMPOSITE);
    ASSERT_NE(rsHardwareProcessor, nullptr);
    RSSurfaceRenderNodeConfig config;
    RSSurfaceRenderNode rsSurfaceRenderNode(config);
    rsHardwareProcessor->ProcessSurface(rsSurfaceRenderNode);
}

/**
 * @tc.name: ProcessDisplaySurface
 * @tc.desc: call ProcessSurface
 * @tc.type: FUNC
 * @tc.require: issuesI6Q871
 */
HWTEST_F(RSPhysicalScreenProcessorTest, ProcessDisplaySurface, TestSize.Level1)
{
    RSDisplayNodeConfig config;
    NodeId id = 0;
    RSDisplayRenderNode rsDisplayRenderNode(id, config);
    auto rsHardwareProcessor = RSProcessorFactory::CreateProcessor(RSDisplayRenderNode::CompositeType::
        SOFTWARE_COMPOSITE);
    ASSERT_NE(nullptr, rsHardwareProcessor);
    rsHardwareProcessor->ProcessDisplaySurface(rsDisplayRenderNode);
}

/**
 * @tc.name: PostProcess001
 * @tc.desc: call PostProcess
 * @tc.type: FUNC
 * @tc.require: issuesI6Q871
 */
HWTEST_F(RSPhysicalScreenProcessorTest, PostProcess001, TestSize.Level1)
{
    auto rsHardwareProcessor = RSProcessorFactory::CreateProcessor(RSDisplayRenderNode::CompositeType::
        HARDWARE_COMPOSITE);
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
    auto rsHardwareProcessor = RSProcessorFactory::CreateProcessor(RSDisplayRenderNode::CompositeType::
        HARDWARE_COMPOSITE);
    ASSERT_NE(rsHardwareProcessor, nullptr);
    NodeId id = 1;
    RCDSurfaceType type = RCDSurfaceType::BOTTOM;
    auto node = RSRcdSurfaceRenderNode::Create(id, type);
    rsHardwareProcessor->ProcessRcdSurface(*node);
}

/**
 * @tc.name: Redraw
 * @tc.desc: call Redraw
 * @tc.type: FUNC
 * @tc.require: issueI9JY8B
 */
HWTEST_F(RSPhysicalScreenProcessorTest, Redraw, TestSize.Level1)
{
    std::shared_ptr<RSPhysicalScreenProcessor> rsHardwareProcessor = std::make_shared<RSPhysicalScreenProcessor>();
    sptr<Surface> surface;
    std::vector<LayerInfoPtr> layers;
    rsHardwareProcessor->Redraw(surface, layers);
    surface = Surface::CreateSurfaceAsConsumer();
    ASSERT_NE(surface, nullptr);
    rsHardwareProcessor->renderEngine_ = std::make_shared<RSUniRenderEngine>();
    rsHardwareProcessor->Redraw(surface, layers);
    ASSERT_NE(rsHardwareProcessor->renderEngine_, nullptr);
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
    auto rsHardwareProcessor = RSProcessorFactory::CreateProcessor(RSDisplayRenderNode::CompositeType::
        HARDWARE_COMPOSITE);
    RSDisplayNodeConfig config;
    NodeId id = 0;
    RSDisplayRenderNode node(id, config);
    rsHardwareProcessor->CalculateScreenTransformMatrix(node);
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
    auto processor = RSProcessorFactory::CreateProcessor(RSDisplayRenderNode::CompositeType::
        HARDWARE_COMPOSITE);
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
    auto processor = RSProcessorFactory::CreateProcessor(RSDisplayRenderNode::CompositeType::
        HARDWARE_COMPOSITE);
    processor->SetSecurityDisplay(true);
    ASSERT_TRUE(processor->isSecurityDisplay_);

    processor->SetDisplayHasSecSurface(true);
    ASSERT_TRUE(processor->displayHasSecSurface_);
}
} // namespace OHOS::Rosen
