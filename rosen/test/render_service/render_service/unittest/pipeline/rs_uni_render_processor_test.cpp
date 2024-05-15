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
#include "rs_test_util.h"

#include "pipeline/round_corner_display/rs_rcd_surface_render_node.h"
#include "pipeline/rs_processor_factory.h"
#include "pipeline/rs_uni_render_engine.h"
#include "pipeline/rs_uni_render_processor.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSUniRenderProcessorTest : public testing::Test {
public:
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
    auto processor = RSProcessorFactory::CreateProcessor(RSDisplayRenderNode::CompositeType::UNI_RENDER_COMPOSITE);
    RSDisplayNodeConfig config;
    RSDisplayRenderNode node(1, config);
    EXPECT_EQ(processor->Init(node, 0, 0, 0, nullptr), false);
}

/**
 * @tc.name: ProcessSurface001
 * @tc.desc: test ProcessSurface func with invalid layer info
 * @tc.type: FUNC
 * @tc.require: issueI6QM6E
 */
HWTEST(RSUniRenderProcessorTest, ProcessSurface001, TestSize.Level1)
{
    auto processor = RSProcessorFactory::CreateProcessor(RSDisplayRenderNode::CompositeType::UNI_RENDER_COMPOSITE);
    RSDisplayNodeConfig config;
    RSDisplayRenderNode node(1, config);
    auto uniRenderEngine = std::make_shared<RSUniRenderEngine>();
    processor->Init(node, 0, 0, 0, uniRenderEngine);
    RSSurfaceRenderNode surfaceNode(2);
    processor->ProcessSurface(surfaceNode);
}

/**
 * @tc.name: InitTest
 * @tc.desc: Verify function Init
 * @tc.type:FUNC
 * @tc.require:issuesI9KRF1
 */
HWTEST(RSUniRenderProcessorTest, InitTest, TestSize.Level1)
{
    auto renderProcessor = std::make_shared<RSUniRenderProcessor>();
    RSDisplayNodeConfig config;
    RSDisplayRenderNode node(1, config);
    auto renderEngine = std::make_shared<RSUniRenderEngine>();
    EXPECT_EQ(renderProcessor->Init(node, 0, 0, 0, renderEngine), false);
}

/**
 * @tc.name: ProcessSurfaceTest
 * @tc.desc: Verify function ProcessSurface
 * @tc.type:FUNC
 * @tc.require:issuesI9KRF1
 */
HWTEST(RSUniRenderProcessorTest, ProcessSurfaceTest, TestSize.Level1)
{
    auto renderProcessor = std::make_shared<RSUniRenderProcessor>();
    RSDisplayNodeConfig config;
    RSDisplayRenderNode node(1, config);
    auto uniRenderEngine = std::make_shared<RSUniRenderEngine>();
    renderProcessor->Init(node, 0, 0, 0, uniRenderEngine);
    // for test
    RSSurfaceRenderNode surfaceNode(2);
    renderProcessor->ProcessSurface(surfaceNode);
}

/**
 * @tc.name: PostProcessTest
 * @tc.desc: Verify function PostProcess
 * @tc.type:FUNC
 * @tc.require:issuesI9KRF1
 */
HWTEST(RSUniRenderProcessorTest, PostProcessTest, TestSize.Level1)
{
    auto renderProcessor = std::make_shared<RSUniRenderProcessor>();
    renderProcessor->PostProcess();
    EXPECT_FALSE(renderProcessor->isPhone_);
}

/**
 * @tc.name: CreateLayerTest
 * @tc.desc: Verify function CreateLayer
 * @tc.type:FUNC
 * @tc.require:issuesI9KRF1
 */
HWTEST(RSUniRenderProcessorTest, CreateLayerTest, TestSize.Level1)
{
    auto renderProcessor = std::make_shared<RSUniRenderProcessor>();
    HdiBackend hdiBackend;
    auto output = std::make_shared<HdiOutput>(1);
    renderProcessor->uniComposerAdapter_->hdiBackend_ = &hdiBackend;
    renderProcessor->uniComposerAdapter_->output_ = output;
    RSSurfaceRenderNode node(0);
    auto iConsumerSurface = IConsumerSurface::Create();
    node.SetConsumer(iConsumerSurface);
    RSSurfaceRenderParams params(0);
    RSLayerInfo layerInfo;
    sptr<SurfaceBuffer> bufferTest = OHOS::SurfaceBuffer::Create();
    sptr<SurfaceBuffer> preBufferTest = OHOS::SurfaceBuffer::Create();
    layerInfo.buffer = bufferTest;
    layerInfo.preBuffer = preBufferTest;
    layerInfo.zOrder = 0;
    params.SetLayerInfo(layerInfo);
    renderProcessor->CreateLayer(node, params);
    EXPECT_FALSE(renderProcessor->isPhone_);
}

/**
 * @tc.name: ProcessDisplaySurfaceTest
 * @tc.desc: Verify function ProcessDisplaySurface
 * @tc.type:FUNC
 * @tc.require:issuesI9KRF1
 */
HWTEST(RSUniRenderProcessorTest, ProcessDisplaySurfaceTest, TestSize.Level1)
{
    auto renderProcessor = std::make_shared<RSUniRenderProcessor>();
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[0];
    RSDisplayNodeConfig config;
    RSDisplayRenderNode node(nodeId, config);
    renderProcessor->ProcessDisplaySurface(node);
    EXPECT_FALSE(renderProcessor->uniComposerAdapter_->CreateLayer(node));
}

/**
 * @tc.name: ProcessRcdSurfaceTest
 * @tc.desc: Verify function ProcessRcdSurface
 * @tc.type:FUNC
 * @tc.require:issuesI9KRF1
 */
HWTEST(RSUniRenderProcessorTest, ProcessRcdSurfaceTest, TestSize.Level1)
{
    auto renderProcessor = std::make_shared<RSUniRenderProcessor>();
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[0];
    RCDSurfaceType type = RCDSurfaceType::INVALID;
    RSRcdSurfaceRenderNode node(nodeId, type);
    renderProcessor->ProcessRcdSurface(node);
    EXPECT_FALSE(renderProcessor->uniComposerAdapter_->CreateLayer(node));
}
}