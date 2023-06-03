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
#include "pipeline/rs_uni_render_engine.h"
#include "pipeline/rs_uni_render_processor.h"
#include "pipeline/rs_processor_factory.h"
#include "rs_test_util.h"

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
}