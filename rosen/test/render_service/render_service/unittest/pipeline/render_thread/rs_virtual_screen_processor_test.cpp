/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "gtest/gtest.h"
#include "limit_number.h"
#include "pipeline/render_thread/rs_uni_render_engine.h"
#include "pipeline/main_thread/rs_main_thread.h"
#include "pipeline/rs_processor_factory.h"
#include "pipeline/render_thread/rs_virtual_screen_processor.h"
#include "pipeline/rs_logical_display_render_node.h"
#include "feature/round_corner_display/rs_rcd_surface_render_node.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSVirtualScreenProcessorTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSVirtualScreenProcessorTest::SetUpTestCase() {}
void RSVirtualScreenProcessorTest::TearDownTestCase() {}
void RSVirtualScreenProcessorTest::SetUp() {}
void RSVirtualScreenProcessorTest::TearDown() {}

/**
 * @tc.name: CreateAndDestroy001
 * @tc.desc: Test RSVirtualScreenProcessor can be created and destroyed.
 * @tc.type: FUNC
 * @tc.require: issueIATLPV
 */
HWTEST_F(RSVirtualScreenProcessorTest, CreateAndDestroy001, TestSize.Level1)
{
    // The best way to create RSSoftwareProcessor.
    auto p = RSProcessorFactory::CreateProcessor(CompositeType::SOFTWARE_COMPOSITE);
    ASSERT_NE(p, nullptr);
}

/**
 * @tc.name: CreateAndDestroy002
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSVirtualScreenProcessorTest, CreateAndDestroy002, TestSize.Level1)
{
    RSProcessorFactory factory;
    auto p = factory.CreateProcessor(CompositeType::SOFTWARE_COMPOSITE);
    EXPECT_NE(p.get(), nullptr);
}

/**
 * @tc.name: Init
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSVirtualScreenProcessorTest, Init, TestSize.Level1)
{
    NodeId id = 0;
    int32_t offsetX = 0;
    int32_t offsetY = 0;
    auto rsContext = std::make_shared<RSContext>();
    RSScreenRenderNode rsScreenRenderNode(id, 0, rsContext->weak_from_this());
    auto rsSoftwareProcessor = RSProcessorFactory::CreateProcessor(CompositeType::
        SOFTWARE_COMPOSITE);
    auto& uniRenderThread = RSUniRenderThread::Instance();
    uniRenderThread.uniRenderEngine_ = std::make_shared<RSUniRenderEngine>();
    auto renderEngine = uniRenderThread.GetRenderEngine();
    ASSERT_NE(nullptr, rsSoftwareProcessor);
    ASSERT_EQ(false, rsSoftwareProcessor->Init(rsScreenRenderNode, offsetX, offsetY, INVALID_SCREEN_ID, renderEngine));

    RSSurfaceRenderNodeConfig sConfig;
    RSSurfaceRenderNode rsSurfaceRenderNode(sConfig);
    rsSoftwareProcessor->ProcessSurface(rsSurfaceRenderNode);
}

/**
 * @tc.name: InitTest002
 * @tc.desc: test Init when mirroredId is not INVALID_SCREEN_ID
 * @tc.type: FUNC
 * @tc.require: issueI9KDPI
 */
HWTEST_F(RSVirtualScreenProcessorTest, InitTest002, TestSize.Level1)
{
    NodeId nodeId = 100;
    ScreenId screenId = 100;
    ScreenId mirroredId = 0;
    int32_t offsetX = 0;
    int32_t offsetY = 0;
    RSScreenRenderNode rsScreenRenderNode(nodeId, screenId);
    auto virtualScreenProcessor = RSProcessorFactory::CreateProcessor(CompositeType::SOFTWARE_COMPOSITE);
    auto renderEngine = std::make_shared<RSUniRenderEngine>();
    ASSERT_EQ(false, virtualScreenProcessor->Init(rsScreenRenderNode, offsetX, offsetY, mirroredId, renderEngine));
}

/**
 * @tc.name: InitTest003
 * @tc.desc: init test
 * @tc.type: FUNC
 * @tc.require: issuesI6Q871
 */
HWTEST_F(RSVirtualScreenProcessorTest, InitTest003, TestSize.Level1)
{
    auto virtualScreenProcessor = RSProcessorFactory::CreateProcessor(CompositeType::SOFTWARE_COMPOSITE);
    ASSERT_NE(nullptr, virtualScreenProcessor);
    NodeId screenNodeId = 0;
    ScreenId screenId = 1;
    int32_t offsetX = 0;
    int32_t offsetY = 0;
    auto context = std::make_shared<RSContext>();
    auto rsScreenRenderNode = std::make_shared<RSScreenRenderNode>(screenNodeId, screenId, context->weak_from_this());
    auto& uniRenderThread = RSUniRenderThread::Instance();
    uniRenderThread.uniRenderEngine_ = std::make_shared<RSUniRenderEngine>();
    auto renderEngine = uniRenderThread.GetRenderEngine();
    ASSERT_EQ(uniRenderThread.uniRenderEngine_, renderEngine);
    ASSERT_EQ(rsScreenRenderNode->GetChildrenCount(), 0);

    NodeId surfaceNodeId = 100;
    auto rsSurfaceRenderNode = std::make_shared<RSSurfaceRenderNode>(surfaceNodeId, context->weak_from_this());
    rsScreenRenderNode->AddChild(rsSurfaceRenderNode);
    ASSERT_EQ(rsScreenRenderNode->GetChildrenCount(), 1);
    ASSERT_FALSE(virtualScreenProcessor->Init(*rsScreenRenderNode, offsetX, offsetY, INVALID_SCREEN_ID, renderEngine));
}

/**
 * @tc.name: InitTest004
 * @tc.desc: init test
 * @tc.type: FUNC
 * @tc.require: issuesI6Q871
 */
HWTEST_F(RSVirtualScreenProcessorTest, InitTest004, TestSize.Level1)
{
    auto virtualScreenProcessor = RSProcessorFactory::CreateProcessor(CompositeType::SOFTWARE_COMPOSITE);
    ASSERT_NE(nullptr, virtualScreenProcessor);
    NodeId screenNodeId = 0;
    ScreenId screenId = 1;
    int32_t offsetX = 0;
    int32_t offsetY = 0;
    auto context = std::make_shared<RSContext>();
    auto rsScreenRenderNode = std::make_shared<RSScreenRenderNode>(screenNodeId, screenId, context->weak_from_this());
    auto& uniRenderThread = RSUniRenderThread::Instance();
    uniRenderThread.uniRenderEngine_ = std::make_shared<RSUniRenderEngine>();
    auto renderEngine = uniRenderThread.GetRenderEngine();
    ASSERT_EQ(uniRenderThread.uniRenderEngine_, renderEngine);
    ASSERT_EQ(rsScreenRenderNode->GetChildrenCount(), 0);

    RSDisplayNodeConfig config;
    NodeId displayNodeId = 100;
    auto rsLogicalDisplayRenderNode = std::make_shared<RSLogicalDisplayRenderNode>(displayNodeId, config,
        context->weak_from_this());
    rsScreenRenderNode->AddChild(rsLogicalDisplayRenderNode);
    ASSERT_EQ(rsScreenRenderNode->GetChildrenCount(), 1);
    ASSERT_FALSE(virtualScreenProcessor->Init(*rsScreenRenderNode, offsetX, offsetY, INVALID_SCREEN_ID, renderEngine));
}

/**
 * @tc.name: InitTest005
 * @tc.desc: init test
 * @tc.type: FUNC
 * @tc.require: issuesI6Q871
 */
HWTEST_F(RSVirtualScreenProcessorTest, InitTest005, TestSize.Level1)
{
    auto virtualScreenProcessor = RSProcessorFactory::CreateProcessor(CompositeType::SOFTWARE_COMPOSITE);
    ASSERT_NE(nullptr, virtualScreenProcessor);
    NodeId screenNodeId = 0;
    ScreenId screenId = 1;
    int32_t offsetX = 0;
    int32_t offsetY = 0;
    auto context = std::make_shared<RSContext>();
    auto rsScreenRenderNode = std::make_shared<RSScreenRenderNode>(screenNodeId, screenId, context->weak_from_this());
    auto& uniRenderThread = RSUniRenderThread::Instance();
    uniRenderThread.uniRenderEngine_ = std::make_shared<RSUniRenderEngine>();
    auto renderEngine = uniRenderThread.GetRenderEngine();
    ASSERT_EQ(uniRenderThread.uniRenderEngine_, renderEngine);
    ASSERT_EQ(rsScreenRenderNode->GetChildrenCount(), 0);

    RSDisplayNodeConfig config;
    NodeId displayNodeId = 100;
    auto rsLogicalDisplayRenderNode = std::make_shared<RSLogicalDisplayRenderNode>(displayNodeId, config,
        context->weak_from_this());
    rsLogicalDisplayRenderNode->SetBootAnimation(true);
    rsScreenRenderNode->AddChild(rsLogicalDisplayRenderNode);
    ASSERT_EQ(rsScreenRenderNode->GetChildrenCount(), 1);
    ASSERT_FALSE(virtualScreenProcessor->Init(*rsScreenRenderNode, offsetX, offsetY, INVALID_SCREEN_ID, renderEngine));
}

/**
 * @tc.name: InitTest006
 * @tc.desc: init test
 * @tc.type: FUNC
 * @tc.require: issuesI6Q871
 */
HWTEST_F(RSVirtualScreenProcessorTest, InitTest006, TestSize.Level1)
{
    auto virtualScreenProcessor = RSProcessorFactory::CreateProcessor(CompositeType::SOFTWARE_COMPOSITE);
    ASSERT_NE(nullptr, virtualScreenProcessor);
    NodeId screenNodeId = 0;
    ScreenId screenId = 1;
    int32_t offsetX = 0;
    int32_t offsetY = 0;
    auto context = std::make_shared<RSContext>();
    auto rsScreenRenderNode = std::make_shared<RSScreenRenderNode>(screenNodeId, screenId, context->weak_from_this());
    auto& uniRenderThread = RSUniRenderThread::Instance();
    uniRenderThread.uniRenderEngine_ = std::make_shared<RSUniRenderEngine>();
    auto renderEngine = uniRenderThread.GetRenderEngine();
    ASSERT_EQ(uniRenderThread.uniRenderEngine_, renderEngine);
    ASSERT_EQ(rsScreenRenderNode->GetChildrenCount(), 0);

    RSDisplayNodeConfig config;
    NodeId displayNodeId1 = 101;
    NodeId displayNodeId2 = 102;
    auto rsLogicalDisplayRenderNode1 = std::make_shared<RSLogicalDisplayRenderNode>(displayNodeId1, config,
        context->weak_from_this());
    auto rsLogicalDisplayRenderNode2 = std::make_shared<RSLogicalDisplayRenderNode>(displayNodeId2, config,
        context->weak_from_this());
    rsScreenRenderNode->AddChild(rsLogicalDisplayRenderNode1);
    rsScreenRenderNode->AddChild(rsLogicalDisplayRenderNode2);
    ASSERT_EQ(rsScreenRenderNode->GetChildrenCount(), 2);
    ASSERT_FALSE(virtualScreenProcessor->Init(*rsScreenRenderNode, offsetX, offsetY, INVALID_SCREEN_ID, renderEngine));
}

/**
 * @tc.name: ProcessSurface001
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSVirtualScreenProcessorTest, ProcessSurface001, TestSize.Level1)
{
    RSSurfaceRenderNodeConfig config;
    RSSurfaceRenderNode rsSurfaceRenderNode(config);
    auto rsSoftwareProcessor = RSProcessorFactory::CreateProcessor(CompositeType::
        SOFTWARE_COMPOSITE);
    ASSERT_NE(rsSoftwareProcessor, nullptr);
    rsSoftwareProcessor->ProcessSurface(rsSurfaceRenderNode);
}

/**
 * @tc.name: ProcessScreenSurface
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSVirtualScreenProcessorTest, ProcessScreenSurface, TestSize.Level1)
{
    NodeId id = 0;
    auto rsContext = std::make_shared<RSContext>();
    RSScreenRenderNode rsDisplayRenderNode(id, 0, rsContext->weak_from_this());
    auto rsSoftwareProcessor = RSProcessorFactory::CreateProcessor(CompositeType::
        SOFTWARE_COMPOSITE);
    ASSERT_NE(nullptr, rsSoftwareProcessor);
    rsSoftwareProcessor->ProcessScreenSurface(rsDisplayRenderNode);
}

/**
 * @tc.name: PostProcess001
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSVirtualScreenProcessorTest, PostProcess001, TestSize.Level1)
{
    auto rsSoftwareProcessor = RSProcessorFactory::CreateProcessor(CompositeType::
        SOFTWARE_COMPOSITE);
    ASSERT_NE(rsSoftwareProcessor, nullptr);
    rsSoftwareProcessor->PostProcess();
}

/**
 * @tc.name: ProcessSurfaceTest
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSVirtualScreenProcessorTest, ProcessSurfaceTest, TestSize.Level1)
{
    RSSurfaceRenderNodeConfig config;
    RSSurfaceRenderNode rsSurfaceRenderNode(config);
    auto rsVirtualScreenProcessor = std::make_shared<RSVirtualScreenProcessor>();
    ASSERT_NE(rsVirtualScreenProcessor, nullptr);
    rsVirtualScreenProcessor->ProcessSurface(rsSurfaceRenderNode);
}

/**
 * @tc.name: ProcessDisplaySurfaceTest
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSVirtualScreenProcessorTest, ProcessDisplaySurfaceTest, TestSize.Level1)
{
    NodeId id = 0;
    auto rsContext = std::make_shared<RSContext>();
    RSScreenRenderNode rsDisplayRenderNode(id, 0, rsContext->weak_from_this());
    auto rsVirtualScreenProcessor = std::make_shared<RSVirtualScreenProcessor>();
    ASSERT_NE(rsVirtualScreenProcessor, nullptr);
    rsVirtualScreenProcessor->ProcessScreenSurface(rsDisplayRenderNode);
}

/**
 * @tc.name: ProcessRcdSurfaceTest
 * @tc.desc: test results of ProcessRcdSurface
 * @tc.type:  FUNC
 * @tc.require: issueI9KDPI
 */
HWTEST_F(RSVirtualScreenProcessorTest, ProcessRcdSurfaceTest, TestSize.Level1)
{
    NodeId id = 0;
    RCDSurfaceType type = RCDSurfaceType::BOTTOM;
    RSRcdSurfaceRenderNode node(id, type);
    auto rsVirtualScreenProcessor = std::make_shared<RSVirtualScreenProcessor>();
    ASSERT_NE(rsVirtualScreenProcessor, nullptr);
    rsVirtualScreenProcessor->ProcessRcdSurface(node);
}
} // namespace OHOS::Rosen
