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
#include "pipeline/rs_physical_screen_processor.h"
#include "pipeline/rs_processor_factory.h"

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

void RSPhysicalScreenProcessorTest::SetUpTestCase() {}
void RSPhysicalScreenProcessorTest::TearDownTestCase() {}
void RSPhysicalScreenProcessorTest::SetUp() {}
void RSPhysicalScreenProcessorTest::TearDown() {}

/**
 * @tc.name: CreateAndDestroy001
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSPhysicalScreenProcessorTest, CreateAndDestroy001, TestSize.Level1)
{
    // The best way to create RSHardwareProcessor.
    auto p = RSProcessorFactory::CreateProcessor(RSDisplayRenderNode::CompositeType::HARDWARE_COMPOSITE);
    EXPECT_NE(p.get(), nullptr);
}

/**
 * @tc.name: CreateAndDestroy002
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSPhysicalScreenProcessorTest, CreateAndDestroy002, TestSize.Level1)
{
    RSProcessorFactory factory;
    auto p = factory.CreateProcessor(RSDisplayRenderNode::CompositeType::HARDWARE_COMPOSITE);
    EXPECT_NE(p.get(), nullptr);
}

/**
 * @tc.name: Init
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
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
    ASSERT_NE(nullptr, rsHardwareProcessor);
    ASSERT_EQ(false, rsHardwareProcessor->Init(rsDisplayRenderNode, offsetX, offsetY, INVALID_SCREEN_ID));
}

/*
* Function: ProcessSurface001
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: call ProcessSurface
* @tc.require: IssueI5VYCA
*/
HWTEST_F(RSPhysicalScreenProcessorTest, ProcessSurface001, TestSize.Level1)
{
    auto rsHardwareProcessor = RSProcessorFactory::CreateProcessor(RSDisplayRenderNode::CompositeType::
        HARDWARE_COMPOSITE);
    RSSurfaceRenderNodeConfig config;
    RSSurfaceRenderNode rsSurfaceRenderNode(config);
    rsHardwareProcessor->ProcessSurface(rsSurfaceRenderNode);
    sptr<Surface> surface = Surface::CreateSurfaceAsConsumer(config.name);
    rsSurfaceRenderNode.SetConsumer(surface);
    rsHardwareProcessor->ProcessSurface(rsSurfaceRenderNode);
    auto& consumer = rsSurfaceRenderNode.GetConsumer();
    GraphicExtDataHandle handle;
    handle.fd = -1;
    handle.reserveInts = 1;
    consumer->SetTunnelHandle(&handle);
    rsHardwareProcessor->ProcessSurface(rsSurfaceRenderNode);
}

/*
* Function: ProcessSurface002
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: call ProcessDisplaySurface
* @tc.require: IssueI5VYCA
*/
HWTEST_F(RSPhysicalScreenProcessorTest, ProcessSurface002, TestSize.Level1)
{
    auto rsUniRenderProcessor = RSProcessorFactory::CreateProcessor(RSDisplayRenderNode::CompositeType::
        UNI_RENDER_COMPOSITE);
    NodeId id = 0;
    RSDisplayNodeConfig config;
    RSDisplayRenderNode rsDisplayRenderNode(id, config);
    rsUniRenderProcessor->ProcessDisplaySurface(rsDisplayRenderNode);
}

/**
 * @tc.name: ProcessSurface003
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSPhysicalScreenProcessorTest, ProcessSurface003, TestSize.Level1)
{
    auto rsHardwareProcessor = RSProcessorFactory::CreateProcessor(RSDisplayRenderNode::CompositeType::
        HARDWARE_COMPOSITE);
    RSSurfaceRenderNodeConfig config;
    RSSurfaceRenderNode rsSurfaceRenderNode(config);
    rsHardwareProcessor->ProcessSurface(rsSurfaceRenderNode);
}

/**
 * @tc.name: ProcessSurface004
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSPhysicalScreenProcessorTest, ProcessSurface004, TestSize.Level1)
{
    auto rsHardwareProcessor = RSProcessorFactory::CreateProcessor(RSDisplayRenderNode::CompositeType::
        HARDWARE_COMPOSITE);
    RSSurfaceRenderNodeConfig config;
    RSSurfaceRenderNode rsSurfaceRenderNode(config);
    rsHardwareProcessor->ProcessSurface(rsSurfaceRenderNode);
}

/**
 * @tc.name: ProcessSurface005
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSPhysicalScreenProcessorTest, ProcessSurface005, TestSize.Level1)
{
    auto rsHardwareProcessor = RSProcessorFactory::CreateProcessor(RSDisplayRenderNode::CompositeType::
        HARDWARE_COMPOSITE);
    RSSurfaceRenderNodeConfig config;
    RSSurfaceRenderNode rsSurfaceRenderNode(config);
    rsHardwareProcessor->ProcessSurface(rsSurfaceRenderNode);
}

/**
 * @tc.name: ProcessSurface006
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSPhysicalScreenProcessorTest, ProcessSurface006, TestSize.Level1)
{
    auto rsHardwareProcessor = RSProcessorFactory::CreateProcessor(RSDisplayRenderNode::CompositeType::
        HARDWARE_COMPOSITE);
    RSSurfaceRenderNodeConfig config;
    RSSurfaceRenderNode rsSurfaceRenderNode(config);
    rsHardwareProcessor->ProcessSurface(rsSurfaceRenderNode);
}

/**
 * @tc.name: ProcessSurface007
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSPhysicalScreenProcessorTest, ProcessSurface007, TestSize.Level1)
{
    auto rsHardwareProcessor = RSProcessorFactory::CreateProcessor(RSDisplayRenderNode::CompositeType::
        HARDWARE_COMPOSITE);
    RSSurfaceRenderNodeConfig config;
    RSSurfaceRenderNode rsSurfaceRenderNode(config);
    rsHardwareProcessor->ProcessSurface(rsSurfaceRenderNode);
}

/**
 * @tc.name: ProcessSurface008
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSPhysicalScreenProcessorTest, ProcessSurface008, TestSize.Level1)
{
    auto rsHardwareProcessor = RSProcessorFactory::CreateProcessor(RSDisplayRenderNode::CompositeType::
        HARDWARE_COMPOSITE);
    RSSurfaceRenderNodeConfig config;
    RSSurfaceRenderNode rsSurfaceRenderNode(config);
    rsHardwareProcessor->ProcessSurface(rsSurfaceRenderNode);
}

/**
 * @tc.name: ProcessDisplaySurface
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
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
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSPhysicalScreenProcessorTest, PostProcess001, TestSize.Level1)
{
    auto rsHardwareProcessor = RSProcessorFactory::CreateProcessor(RSDisplayRenderNode::CompositeType::
        HARDWARE_COMPOSITE);
    rsHardwareProcessor->PostProcess();
}
} // namespace OHOS::Rosen
