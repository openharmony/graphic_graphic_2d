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
#include "pipeline/rs_main_thread.h"
#include "pipeline/rs_processor_factory.h"
#include "pipeline/rs_uni_render_engine.h"
#include "pipeline/rs_virtual_screen_processor.h"
#include "pipeline/round_corner_display/rs_rcd_surface_render_node.h"

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
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSVirtualScreenProcessorTest, CreateAndDestroy001, TestSize.Level1)
{
    // The best way to create RSSoftwareProcessor.
    auto p = RSProcessorFactory::CreateProcessor(RSDisplayRenderNode::CompositeType::SOFTWARE_COMPOSITE);
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
    auto p = factory.CreateProcessor(RSDisplayRenderNode::CompositeType::SOFTWARE_COMPOSITE);
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
    RSDisplayNodeConfig config;
    NodeId id = 0;
    int32_t offsetX = 0;
    int32_t offsetY = 0;
    RSDisplayRenderNode rsDisplayRenderNode(id, config);
    auto rsSoftwareProcessor = RSProcessorFactory::CreateProcessor(RSDisplayRenderNode::CompositeType::
        SOFTWARE_COMPOSITE);
    auto& uniRenderThread = RSUniRenderThread::Instance();
    uniRenderThread.uniRenderEngine_ = std::make_shared<RSUniRenderEngine>();
    auto renderEngine = uniRenderThread.GetRenderEngine();
    ASSERT_NE(nullptr, rsSoftwareProcessor);
    ASSERT_EQ(false, rsSoftwareProcessor->Init(rsDisplayRenderNode, offsetX, offsetY, INVALID_SCREEN_ID, renderEngine));

    RSSurfaceRenderNodeConfig sConfig;
    RSSurfaceRenderNode rsSurfaceRenderNode(sConfig);
    rsSoftwareProcessor->ProcessSurface(rsSurfaceRenderNode);
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
    auto rsSoftwareProcessor = RSProcessorFactory::CreateProcessor(RSDisplayRenderNode::CompositeType::
        SOFTWARE_COMPOSITE);
    rsSoftwareProcessor->ProcessSurface(rsSurfaceRenderNode);
}

/**
 * @tc.name: ProcessDisplaySurface
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSVirtualScreenProcessorTest, ProcessDisplaySurface, TestSize.Level1)
{
    RSDisplayNodeConfig config;
    NodeId id = 0;
    RSDisplayRenderNode rsDisplayRenderNode(id, config);
    auto rsSoftwareProcessor = RSProcessorFactory::CreateProcessor(RSDisplayRenderNode::CompositeType::
        SOFTWARE_COMPOSITE);
    ASSERT_NE(nullptr, rsSoftwareProcessor);
    rsSoftwareProcessor->ProcessDisplaySurface(rsDisplayRenderNode);
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
    auto rsSoftwareProcessor = RSProcessorFactory::CreateProcessor(RSDisplayRenderNode::CompositeType::
        SOFTWARE_COMPOSITE);
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
    RSVirtualScreenProcessor rsVirtualScreenProcessor;
    rsVirtualScreenProcessor.ProcessSurface(rsSurfaceRenderNode);
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
    RSDisplayNodeConfig config;
    NodeId id = 0;
    RSDisplayRenderNode rsDisplayRenderNode(id, config);
    RSVirtualScreenProcessor rsVirtualScreenProcessor;
    rsVirtualScreenProcessor.ProcessDisplaySurface(rsDisplayRenderNode);
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
    RSVirtualScreenProcessor rsVirtualScreenProcessor;
    rsVirtualScreenProcessor.ProcessRcdSurface(node);
}
} // namespace OHOS::Rosen
