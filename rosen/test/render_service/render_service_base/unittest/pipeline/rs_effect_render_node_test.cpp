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

#include "pipeline/rs_effect_render_node.h"
#include "platform/common/rs_log.h"
using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {

class RSEffectRenderNodeTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    static inline NodeId id;
    static inline std::weak_ptr<RSContext> context = {};
    static inline Drawing::Canvas canvas;
};

void RSEffectRenderNodeTest::SetUpTestCase() {}
void RSEffectRenderNodeTest::TearDownTestCase() {}
void RSEffectRenderNodeTest::SetUp() {}
void RSEffectRenderNodeTest::TearDown() {}

/**
 * @tc.name: Prepare
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSEffectRenderNodeTest, Prepare, TestSize.Level1)
{
    std::shared_ptr<RSNodeVisitor> visitor = nullptr;
    NodeId nodeId = 0;
    std::weak_ptr<RSContext> context;
    RSEffectRenderNode rsEffectRenderNode(nodeId, context);
    rsEffectRenderNode.Prepare(visitor);
}
/**
 * @tc.name: Process
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSEffectRenderNodeTest, Process, TestSize.Level1)
{
    std::shared_ptr<RSNodeVisitor> visitor = nullptr;
    NodeId nodeId = 0;
    std::weak_ptr<RSContext> context;
    RSEffectRenderNode rsEffectRenderNode(nodeId, context);
    rsEffectRenderNode.Process(visitor);
}
/**
 * @tc.name: GetFilterRect
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSEffectRenderNodeTest, GetFilterRect, TestSize.Level1)
{
    NodeId nodeId = 0;
    std::weak_ptr<RSContext> context;
    RSEffectRenderNode rsEffectRenderNode(nodeId, context);
    Drawing::RectI path;
    rsEffectRenderNode.SetEffectRegion(path);
    rsEffectRenderNode.GetFilterRect();
}
} // namespace OHOS::Rosen