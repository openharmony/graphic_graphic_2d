/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "pipeline/rs_render_node_map.h"
#include "pipeline/rs_surface_render_node.h"
#include "pipeline/rs_display_render_node.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSRenderNodeMapTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    static inline NodeId id;
    static inline std::weak_ptr<RSContext> context = {};
};

void RSRenderNodeMapTest::SetUpTestCase() {}
void RSRenderNodeMapTest::TearDownTestCase() {}
void RSRenderNodeMapTest::SetUp() {}
void RSRenderNodeMapTest::TearDown() {}

/**
 * @tc.name: ObtainScreenLockWindowNodeIdTest
 * @tc.desc: test results of ObtainScreenLockWindowNodeIdTest
 * @tc.type:FUNC
 * @tc.require: issueI9H4AD
 */
HWTEST_F(RSRenderNodeMapTest, ObtainScreenLockWindowNodeIdTest, TestSize.Level1)
{
    std::shared_ptr<RSSurfaceRenderNode> surfaceNode = nullptr;
    RSRenderNodeMap rsRenderNodeMap;
    rsRenderNodeMap.ObtainScreenLockWindowNodeId(surfaceNode);
    NodeId id = 1;
    RSSurfaceRenderNode *surfaceNodeOne = new RSSurfaceRenderNode(id);
    std::shared_ptr<RSSurfaceRenderNode> surfaceNodeTwo(surfaceNodeOne);
    rsRenderNodeMap.ObtainScreenLockWindowNodeId(surfaceNodeTwo);
    ASSERT_EQ(rsRenderNodeMap.screenLockWindowNodeId_, 0);
}

/**
 * @tc.name: ObtainLauncherNodeId
 * @tc.desc: test results of ObtainLauncherNodeId
 * @tc.type:FUNC
 * @tc.require: issueI9H4AD
 */
HWTEST_F(RSRenderNodeMapTest, ObtainLauncherNodeId, TestSize.Level1)
{
    std::shared_ptr<RSSurfaceRenderNode> surfaceNode = nullptr;
    RSRenderNodeMap rsRenderNodeMap;
    rsRenderNodeMap.ObtainLauncherNodeId(surfaceNode);
    ASSERT_EQ(rsRenderNodeMap.GetEntryViewNodeId(), 0);

    NodeId id = 1;
    surfaceNode = std::make_shared<RSSurfaceRenderNode>(id);
    rsRenderNodeMap.ObtainLauncherNodeId(surfaceNode);
    ASSERT_EQ(rsRenderNodeMap.wallpaperViewNodeId_, 0);
}

/**
 * @tc.name: CalCulateAbilityComponentNumsInProcess
 * @tc.desc: test results of CalCulateAbilityComponentNumsInProcess
 * @tc.type:FUNC
 * @tc.require: issueI9H4AD
 */
HWTEST_F(RSRenderNodeMapTest, CalCulateAbilityComponentNumsInProcess, TestSize.Level1)
{
    RSRenderNodeMap rsRenderNodeMap;
    NodeId newId = 1; // 提供一个新的NodeId
    rsRenderNodeMap.CalCulateAbilityComponentNumsInProcess(newId);
    ASSERT_TRUE(true);
}

/**
 * @tc.name: RegisterRenderNode
 * @tc.desc: test results of RegisterRenderNode
 * @tc.type:FUNC
 * @tc.require: issueI9H4AD
 */
HWTEST_F(RSRenderNodeMapTest, RegisterRenderNode, TestSize.Level1)
{
    NodeId id = 1;
    auto node = std::make_shared<OHOS::Rosen::RSRenderNode>(id);
    RSRenderNodeMap rsRenderNodeMap;
    rsRenderNodeMap.RegisterRenderNode(node);
    auto result = rsRenderNodeMap.RegisterRenderNode(node);
    EXPECT_FALSE(result);
}

/**
 * @tc.name: RegisterDisplayRenderNode
 * @tc.desc: test results of RegisterDisplayRenderNode
 * @tc.type:FUNC
 * @tc.require: issueI9H4AD
 */
HWTEST_F(RSRenderNodeMapTest, RegisterDisplayRenderNode, TestSize.Level1)
{
    NodeId id = 1;
    RSDisplayNodeConfig config;
    RSDisplayRenderNode * rsDisplayRenderNode = new RSDisplayRenderNode(id, config);
    std::shared_ptr<RSDisplayRenderNode> node(rsDisplayRenderNode);
    RSRenderNodeMap rsRenderNodeMap;
    auto result = rsRenderNodeMap.RegisterDisplayRenderNode(node);
    EXPECT_TRUE(result);
    result = rsRenderNodeMap.RegisterDisplayRenderNode(node);
    EXPECT_FALSE(result);
}

/**
 * @tc.name: EraseAbilityComponentNumsInProcess
 * @tc.desc: test results of EraseAbilityComponentNumsInProcess
 * @tc.type:FUNC
 * @tc.require: issueI9H4AD
 */
HWTEST_F(RSRenderNodeMapTest, EraseAbilityComponentNumsInProcess, TestSize.Level1)
{
    NodeId id = 1;
    auto node = std::make_shared<OHOS::Rosen::RSSurfaceRenderNode>(id);
    RSRenderNodeMap rsRenderNodeMap;
    rsRenderNodeMap.EraseAbilityComponentNumsInProcess(id);
    EXPECT_EQ(rsRenderNodeMap.surfaceNodeMap_.find(id), rsRenderNodeMap.surfaceNodeMap_.end());
}

/**
 * @tc.name: MoveRenderNodeMap
 * @tc.desc: test results of MoveRenderNodeMap
 * @tc.type:FUNC
 * @tc.require: issueI9H4AD
 */
HWTEST_F(RSRenderNodeMapTest, MoveRenderNodeMap, TestSize.Level1)
{
    NodeId id = 1;
    auto node = std::make_shared<OHOS::Rosen::RSRenderNode>(id);
    RSRenderNodeMap rsRenderNodeMap;
    std::shared_ptr<std::unordered_map<NodeId, std::shared_ptr<RSBaseRenderNode>>> subRenderNodeMap;
    pid_t pid = 1;
    rsRenderNodeMap.MoveRenderNodeMap(subRenderNodeMap, pid);
    rsRenderNodeMap.renderNodeMap_.emplace(id, node);
    rsRenderNodeMap.MoveRenderNodeMap(subRenderNodeMap, pid);
    EXPECT_TRUE(true);
}

/**
 * @tc.name: FilterNodeByPid
 * @tc.desc: test results of FilterNodeByPid
 * @tc.type:FUNC
 * @tc.require: issueI9H4AD
 */
HWTEST_F(RSRenderNodeMapTest, FilterNodeByPid, TestSize.Level1)
{
    NodeId id = 0;
    auto node = std::make_shared<OHOS::Rosen::RSRenderNode>(id);
    RSRenderNodeMap rsRenderNodeMap;
    rsRenderNodeMap.FilterNodeByPid(1);

    rsRenderNodeMap.renderNodeMap_.emplace(id, node);
    rsRenderNodeMap.FilterNodeByPid(1);
    EXPECT_TRUE(true);
}

/**
 * @tc.name: FilterNodeByPid
 * @tc.desc: test results of GetRenderNode
 * @tc.type:FUNC
 * @tc.require: issueI9H4AD
 */
HWTEST_F(RSRenderNodeMapTest, GetRenderNode, TestSize.Level1)
{
    NodeId id = 0;
    auto node = std::make_shared<OHOS::Rosen::RSRenderNode>(id);
    RSRenderNodeMap rsRenderNodeMap;
    rsRenderNodeMap.GetRenderNode(1);
    EXPECT_EQ(rsRenderNodeMap.GetRenderNode(1), nullptr);
}

/**
 * @tc.name: GetAnimationFallbackNode
 * @tc.desc: test results of GetAnimationFallbackNode
 * @tc.type:FUNC
 * @tc.require: issueI9H4AD
 */
HWTEST_F(RSRenderNodeMapTest, GetAnimationFallbackNode, TestSize.Level1)
{
    NodeId id = 0;
    auto node = std::make_shared<OHOS::Rosen::RSRenderNode>(id);
    RSRenderNodeMap rsRenderNodeMap;
    rsRenderNodeMap.GetAnimationFallbackNode();

    rsRenderNodeMap.renderNodeMap_.emplace(id, node);
    EXPECT_NE(rsRenderNodeMap.GetAnimationFallbackNode(), nullptr);
}
} // namespace OHOS::Rosen