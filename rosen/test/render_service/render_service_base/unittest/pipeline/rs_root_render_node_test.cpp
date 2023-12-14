/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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
#include "pipeline/rs_root_render_node.h"
#include "platform/common/rs_log.h"
using namespace testing;
using namespace testing::ext;
namespace OHOS {
namespace Rosen {
class RSRootRenderNodeTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    static inline NodeId id;
    static inline std::weak_ptr<RSContext> context = {};
};

void RSRootRenderNodeTest::SetUpTestCase() {}
void RSRootRenderNodeTest::TearDownTestCase() {}
void RSRootRenderNodeTest::SetUp() {}
void RSRootRenderNodeTest::TearDown() {}

/**
 * @tc.name: settings
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSRootRenderNodeTest, settings, TestSize.Level1)
{
    NodeId nodeId = 0;
    RSRootRenderNode rsRootRenderNode(nodeId, context);

    rsRootRenderNode.AttachRSSurfaceNode(nodeId);
    EXPECT_EQ(rsRootRenderNode.GetRSSurfaceNodeId(), 0);

    float width = 1.0;
    float height = 2.0;
    rsRootRenderNode.UpdateSuggestedBufferSize(width, height);
    EXPECT_EQ(rsRootRenderNode.GetSuggestedBufferWidth(), 1.0);
    EXPECT_EQ(rsRootRenderNode.GetSuggestedBufferHeight(), 2.0);
}

/**
 * @tc.name: Prepare
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSRootRenderNodeTest, Prepare, TestSize.Level1)
{
    std::shared_ptr<RSNodeVisitor> visitor = nullptr;
    NodeId nodeId = 0;
    std::weak_ptr<RSContext> context;
    RSRootRenderNode rsRootRenderNode(nodeId, context);
    rsRootRenderNode.Prepare(visitor);
}
/**
 * @tc.name: Process
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSRootRenderNodeTest, Process, TestSize.Level1)
{
    std::shared_ptr<RSNodeVisitor> visitor = nullptr;
    NodeId nodeId = 0;
    std::weak_ptr<RSContext> context;
    RSRootRenderNode rsRootRenderNode(nodeId, context);
    rsRootRenderNode.Process(visitor);
}
} // namespace Rosen
} // namespace OHOS