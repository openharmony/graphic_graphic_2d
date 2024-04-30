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
#include "pipeline/rs_render_thread_visitor.h"
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
 * @tc.desc: test results of Prepare
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

    std::shared_ptr<RSNodeVisitor> visitorTwo = std::make_shared<RSRenderThreadVisitor>();
    rsEffectRenderNode.Prepare(visitorTwo);
    ASSERT_TRUE(true);
}

/**
 * @tc.name: Process
 * @tc.desc: test results of Process
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

    std::shared_ptr<RSNodeVisitor> visitorTwo = std::make_shared<RSRenderThreadVisitor>();
    rsEffectRenderNode.Prepare(visitorTwo);
    ASSERT_TRUE(true);
}

/**
 * @tc.name: GetFilterRect
 * @tc.desc: test results of GetFilterRect
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

/**
 * @tc.name: ProcessRenderBeforeChildren
 * @tc.desc: test results of ProcessRenderBeforeChildren
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSEffectRenderNodeTest, ProcessRenderBeforeChildren, TestSize.Level1)
{
    NodeId nodeId = 0;
    std::weak_ptr<RSContext> context;
    RSEffectRenderNode rsEffectRenderNode(nodeId, context);
    Drawing::Canvas canvas(1, 1);
    RSPaintFilterCanvas paintFilterCanvas(&canvas);
    rsEffectRenderNode.ProcessRenderBeforeChildren(paintFilterCanvas);
}

/**
 * @tc.name: SetEffectRegion
 * @tc.desc: test results of SetEffectRegion
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSEffectRenderNodeTest, SetEffectRegion, TestSize.Level1)
{
    NodeId nodeId = 0;
    std::weak_ptr<RSContext> context;
    RSEffectRenderNode rsEffectRenderNode(nodeId, context);
    Drawing::RectI path(1, 1, 1, 1);
    rsEffectRenderNode.SetEffectRegion(path);
}

/**
 * @tc.name: UpdateFilterCacheWithBelowDirty
 * @tc.desc: test results of UpdateFilterCacheWithBelowDirty
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSEffectRenderNodeTest, UpdateFilterCacheWithBelowDirty, TestSize.Level1)
{
    NodeId nodeId = 0;
    std::weak_ptr<RSContext> context;
    RSEffectRenderNode rsEffectRenderNode(nodeId, context);
    RSDirtyRegionManager dirtyManager;
    bool isForeground = true;
    rsEffectRenderNode.UpdateFilterCacheWithBelowDirty(dirtyManager, isForeground);

    rsEffectRenderNode.isRotationChanged_ = true;
    rsEffectRenderNode.preRotationStatus_ = true;
    rsEffectRenderNode.UpdateFilterCacheWithBelowDirty(dirtyManager, isForeground);
    ASSERT_TRUE(true);
}
} // namespace OHOS::Rosen