/*
 * Copyright (c) 2024-2025 Huawei Device Co., Ltd.
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

#include "params/rs_render_params.h"
#include "pipeline/rs_render_node.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {

class RSOpincRootCacheTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSOpincRootCacheTest::SetUpTestCase() {}
void RSOpincRootCacheTest::TearDownTestCase() {}
void RSOpincRootCacheTest::SetUp() {}
void RSOpincRootCacheTest::TearDown() {}

/**
 * @tc.name: OpincQuickMarkStableNode001
 * @tc.desc: test results of OpincQuickMarkStableNode
 * @tc.type: FUNC
 * @tc.require: issueI9UX8W
 */
HWTEST_F(RSOpincRootCacheTest, OpincQuickMarkStableNode001, TestSize.Level1)
{
    RSRenderNode renderNode(0);
    auto& opincCache = renderNode.GetOpincRootCache();
    bool unchangeMarkInApp = false;
    bool unchangeMarkEnable = true;
    opincCache.OpincQuickMarkStableNode(unchangeMarkInApp, unchangeMarkEnable, false);

    unchangeMarkInApp = true;
    opincCache.OpincQuickMarkStableNode(unchangeMarkInApp, unchangeMarkEnable, false);

    opincCache.isSuggestOpincNode_ = true;
    opincCache.OpincQuickMarkStableNode(unchangeMarkInApp, unchangeMarkEnable, false);
    EXPECT_TRUE(opincCache.isSuggestOpincNode_);

    unchangeMarkEnable = false;
    opincCache.OpincQuickMarkStableNode(unchangeMarkInApp, unchangeMarkEnable, false);
    EXPECT_TRUE(opincCache.isUnchangeMarkEnable_);

    unchangeMarkEnable = true;
    renderNode.stagingRenderParams_ = std::make_unique<RSRenderParams>(0);
    opincCache.nodeCacheState_ = NodeCacheState::STATE_CHANGE;
    opincCache.OpincQuickMarkStableNode(unchangeMarkInApp, unchangeMarkEnable, false);
    EXPECT_TRUE(opincCache.isUnchangeMarkEnable_);

    renderNode.isSubTreeDirty_ = true;
    opincCache.OpincQuickMarkStableNode(unchangeMarkInApp, unchangeMarkEnable, false);
    EXPECT_TRUE(renderNode.isSubTreeDirty_);
}

/**
 * @tc.name: OpincQuickMarkStableNode002
 * @tc.desc: test results of OpincQuickMarkStableNode
 * @tc.type: FUNC
 * @tc.require: issueIB9E9A
 */
HWTEST_F(RSOpincRootCacheTest, OpincQuickMarkStableNode002, TestSize.Level1)
{
    RSRenderNode renderNode(0);
    auto& opincCache = renderNode.GetOpincRootCache();
    bool unchangeMarkInApp = false;
    bool unchangeMarkEnable = true;
    opincCache.OpincQuickMarkStableNode(unchangeMarkInApp, unchangeMarkEnable, false);

    unchangeMarkInApp = true;
    opincCache.OpincQuickMarkStableNode(unchangeMarkInApp, unchangeMarkEnable, false);

    opincCache.isSuggestOpincNode_ = true;
    opincCache.OpincQuickMarkStableNode(unchangeMarkInApp, unchangeMarkEnable, false);
    EXPECT_TRUE(opincCache.isSuggestOpincNode_);

    unchangeMarkEnable = false;
    opincCache.OpincQuickMarkStableNode(unchangeMarkInApp, unchangeMarkEnable, false);
    EXPECT_TRUE(opincCache.isUnchangeMarkEnable_);

    unchangeMarkEnable = true;
    renderNode.stagingRenderParams_ = std::make_unique<RSRenderParams>(0);
    opincCache.nodeCacheState_ = NodeCacheState::STATE_CHANGE;
    opincCache.OpincQuickMarkStableNode(unchangeMarkInApp, unchangeMarkEnable, false);
    EXPECT_TRUE(opincCache.isUnchangeMarkEnable_);

    opincCache.waitCount_ = 60;
    opincCache.OpincQuickMarkStableNode(unchangeMarkInApp, unchangeMarkEnable, false);
    EXPECT_TRUE(opincCache.waitCount_ == 59);
    opincCache.waitCount_ = 0;

    renderNode.isSubTreeDirty_ = true;
    opincCache.OpincQuickMarkStableNode(unchangeMarkInApp, unchangeMarkEnable, false);
    EXPECT_TRUE(renderNode.isSubTreeDirty_);

    opincCache.OpincQuickMarkStableNode(unchangeMarkInApp, unchangeMarkEnable, true);
    EXPECT_EQ(opincCache.nodeCacheState_, NodeCacheState::STATE_CHANGE);
}

/**
 * @tc.name: OpincQuickMarkStableNodeWaitCountBranch001
 * @tc.desc: cover waitCount decrement and min-unchange branch when state is unchange
 * @tc.type: FUNC
 * @tc.require: issueRootCacheBranch
 */
HWTEST_F(RSOpincRootCacheTest, OpincQuickMarkStableNodeWaitCountBranch001, TestSize.Level1)
{
    constexpr int32_t waitCountInit = 2;
    constexpr int32_t expectWaitCount = 1;
    constexpr int32_t nonMinUnchangeUpper = 11;
    constexpr int32_t minUnchangeUpper = 3;

    RSRenderNode renderNode(0);
    auto& opincCache = renderNode.GetOpincRootCache();
    bool unchangeMarkInApp = true;
    bool unchangeMarkEnable = true;

    opincCache.nodeCacheState_ = NodeCacheState::STATE_UNCHANGE;
    opincCache.waitCount_ = waitCountInit;
    opincCache.unchangeCountUpper_ = nonMinUnchangeUpper;
    opincCache.OpincQuickMarkStableNode(unchangeMarkInApp, unchangeMarkEnable, false);
    EXPECT_EQ(opincCache.waitCount_, expectWaitCount);
    EXPECT_EQ(opincCache.unchangeCountUpper_, nonMinUnchangeUpper);

    opincCache.waitCount_ = 0;
    opincCache.unchangeCountUpper_ = nonMinUnchangeUpper;
    opincCache.OpincQuickMarkStableNode(unchangeMarkInApp, unchangeMarkEnable, false);
    EXPECT_EQ(opincCache.unchangeCountUpper_, minUnchangeUpper);
}

/**
 * @tc.name: OpincUpdateRootFlag001
 * @tc.desc: test results of OpincUpdateRootFlag
 * @tc.type: FUNC
 * @tc.require: issueI9UX8W
 */
HWTEST_F(RSOpincRootCacheTest, OpincUpdateRootFlag001, TestSize.Level1)
{
    RSRenderNode renderNode(0);
    auto& opincCache = renderNode.GetOpincRootCache();
    bool unchangeMarkEnable = true;
    opincCache.OpincUpdateRootFlag(unchangeMarkEnable, true);
    EXPECT_TRUE(opincCache.isOpincRootFlag_ == false);

    opincCache.nodeCacheState_ = NodeCacheState::STATE_UNCHANGE;
    opincCache.isSuggestOpincNode_ = true;
    renderNode.stagingRenderParams_ = std::make_unique<RSRenderParams>(0);
    opincCache.OpincUpdateRootFlag(unchangeMarkEnable, true);
    EXPECT_TRUE(opincCache.IsOpincUnchangeState());

    opincCache.isUnchangeMarkEnable_ = true;
    opincCache.OpincUpdateRootFlag(unchangeMarkEnable, true);
    EXPECT_TRUE(opincCache.isUnchangeMarkEnable_ == false);
}

/**
 * @tc.name: OpincUpdateRootFlagSupportDisabled001
 * @tc.desc: cover root-flag branch when support flag is false
 * @tc.type: FUNC
 * @tc.require: issueRootCacheBranch
 */
HWTEST_F(RSOpincRootCacheTest, OpincUpdateRootFlagSupportDisabled001, TestSize.Level1)
{
    RSRenderNode renderNode(0);
    auto& opincCache = renderNode.GetOpincRootCache();
    bool unchangeMarkEnable = true;

    opincCache.nodeCacheState_ = NodeCacheState::STATE_UNCHANGE;
    opincCache.isSuggestOpincNode_ = true;
    opincCache.OpincUpdateRootFlag(unchangeMarkEnable, false);
    EXPECT_FALSE(opincCache.OpincGetRootFlag());

    opincCache.isUnchangeMarkEnable_ = true;
    unchangeMarkEnable = true;
    opincCache.OpincUpdateRootFlag(unchangeMarkEnable, false);
    EXPECT_FALSE(unchangeMarkEnable);
    EXPECT_FALSE(opincCache.isUnchangeMarkEnable_);
}

/**
 * @tc.name: IsOpincUnchangeState001
 * @tc.desc: test results of IsOpincUnchangeState
 * @tc.type: FUNC
 * @tc.require: issueI9UX8W
 */
HWTEST_F(RSOpincRootCacheTest, IsOpincUnchangeState001, TestSize.Level1)
{
    RSRenderNode renderNode(0);
    auto& opincCache = renderNode.GetOpincRootCache();
    bool res = opincCache.IsOpincUnchangeState();
    EXPECT_TRUE(res == false);

    opincCache.nodeCacheState_ = NodeCacheState::STATE_UNCHANGE;
    opincCache.isSuggestOpincNode_ = true;
    res = opincCache.IsOpincUnchangeState();
    EXPECT_TRUE(res);
}

/**
 * @tc.name: IsMarkedRenderGroup001
 * @tc.desc: test results of IsMarkedRenderGroup
 * @tc.type: FUNC
 * @tc.require: issueI9UX8W
 */
HWTEST_F(RSOpincRootCacheTest, IsMarkedRenderGroup001, TestSize.Level1)
{
    RSRenderNode renderNode(0);
    auto& opincCache = renderNode.GetOpincRootCache();
    bool res = opincCache.IsMarkedRenderGroup(renderNode.GetNodeGroupType() > RSRenderNode::NodeGroupType::NONE);
    EXPECT_TRUE(!res);

    renderNode.nodeGroupType_ = RSRenderNode::NodeGroupType::GROUPED_BY_ANIM;
    opincCache.isOpincRootFlag_ = true;
    res = opincCache.IsMarkedRenderGroup(renderNode.GetNodeGroupType() > RSRenderNode::NodeGroupType::NONE);
    EXPECT_TRUE(res);
}

/**
 * @tc.name: OpincForcePrepareSubTree001
 * @tc.desc: test results of OpincForcePrepareSubTree
 * @tc.type: FUNC
 * @tc.require: issueI9UX8W
 */
HWTEST_F(RSOpincRootCacheTest, OpincForcePrepareSubTree001, TestSize.Level1)
{
    RSRenderNode renderNode(0);
    auto& opincCache = renderNode.GetOpincRootCache();
    bool autoCacheEnable = true;
    bool res = opincCache.OpincForcePrepareSubTree(autoCacheEnable,
        renderNode.IsSubTreeDirty() || renderNode.IsContentDirty(), true);
    EXPECT_TRUE(!res);

    opincCache.isSuggestOpincNode_ = true;
    res = opincCache.OpincForcePrepareSubTree(autoCacheEnable,
        renderNode.IsSubTreeDirty() || renderNode.IsContentDirty(), true);
    EXPECT_TRUE(res);
    autoCacheEnable = false;
    res = opincCache.OpincForcePrepareSubTree(autoCacheEnable,
        renderNode.IsSubTreeDirty() || renderNode.IsContentDirty(), true);
    EXPECT_FALSE(res);
}

/**
 * @tc.name: OpincForcePrepareSubTreeBranchMatrix001
 * @tc.desc: cover dirty/support/root branches of force-prepare decision
 * @tc.type: FUNC
 * @tc.require: issueRootCacheBranch
 */
HWTEST_F(RSOpincRootCacheTest, OpincForcePrepareSubTreeBranchMatrix001, TestSize.Level1)
{
    RSRenderNode renderNode(0);
    auto& opincCache = renderNode.GetOpincRootCache();
    constexpr bool autoCacheEnable = true;

    opincCache.isSuggestOpincNode_ = true;

    EXPECT_FALSE(opincCache.OpincForcePrepareSubTree(autoCacheEnable, true, true));
    EXPECT_FALSE(opincCache.OpincForcePrepareSubTree(autoCacheEnable, false, false));

    opincCache.isOpincRootFlag_ = true;
    EXPECT_FALSE(opincCache.OpincForcePrepareSubTree(autoCacheEnable, false, true));

    opincCache.isOpincRootFlag_ = false;
    EXPECT_TRUE(opincCache.OpincForcePrepareSubTree(autoCacheEnable, false, true));
}

/**
 * @tc.name: OpincGetRootFlag001
 * @tc.desc: test results of OpincGetRootFlag
 * @tc.type: FUNC
 * @tc.require: issueI9UX8W
 */
HWTEST_F(RSOpincRootCacheTest, OpincGetRootFlag001, TestSize.Level1)
{
    RSRenderNode renderNode(0);
    auto& opincCache = renderNode.GetOpincRootCache();
    bool res = opincCache.OpincGetRootFlag();
    EXPECT_TRUE(!res);

    opincCache.isOpincRootFlag_ = true;
    res = opincCache.OpincGetRootFlag();
    EXPECT_TRUE(res);
}

/**
 * @tc.name: MarkSuggestOpincNode001
 * @tc.desc: test results of MarkSuggestOpincNode
 * @tc.type: FUNC
 * @tc.require: issueI9UX8W
 */
HWTEST_F(RSOpincRootCacheTest, MarkSuggestOpincNode001, TestSize.Level1)
{
    RSRenderNode renderNode(0);
    auto& opincCache = renderNode.GetOpincRootCache();
    opincCache.MarkSuggestOpincNode(true, true);
    EXPECT_TRUE(opincCache.isSuggestOpincNode_);
}

/**
 * @tc.name: IsSuggestOpincNode001
 * @tc.desc: test results of IsSuggestOpincNode
 * @tc.type: FUNC
 * @tc.require: issueI9UX8W
 */
HWTEST_F(RSOpincRootCacheTest, IsSuggestOpincNode001, TestSize.Level1)
{
    RSRenderNode renderNode(0);
    auto& opincCache = renderNode.GetOpincRootCache();
    EXPECT_TRUE(!opincCache.IsSuggestOpincNode());

    opincCache.isSuggestOpincNode_ = true;
    EXPECT_TRUE(opincCache.IsSuggestOpincNode());
}

/**
 * @tc.name: NodeCacheStateChange001
 * @tc.desc: test results of NodeCacheStateChange
 * @tc.type: FUNC
 * @tc.require: issueI9UX8W
 */
HWTEST_F(RSOpincRootCacheTest, NodeCacheStateChange001, TestSize.Level1)
{
    RSRenderNode renderNode(0);
    auto& opincCache = renderNode.GetOpincRootCache();
    renderNode.stagingRenderParams_ = std::make_unique<RSRenderParams>(0);
    opincCache.NodeCacheStateChange(NodeChangeType::KEEP_UNCHANGE);
    opincCache.NodeCacheStateChange(NodeChangeType::KEEP_UNCHANGE);
    opincCache.NodeCacheStateChange(NodeChangeType::KEEP_UNCHANGE);
    opincCache.NodeCacheStateChange(NodeChangeType::KEEP_UNCHANGE);
    EXPECT_TRUE(opincCache.nodeCacheState_ == NodeCacheState::STATE_UNCHANGE);

    opincCache.NodeCacheStateChange(NodeChangeType::SELF_DIRTY);
    EXPECT_TRUE(opincCache.nodeCacheState_ != NodeCacheState::STATE_UNCHANGE);
}

/**
 * @tc.name: SetCacheStateByRetrytime001
 * @tc.desc: test results of SetCacheStateByRetrytime
 * @tc.type: FUNC
 * @tc.require: issueI9UX8W
 */
HWTEST_F(RSOpincRootCacheTest, SetCacheStateByRetrytime001, TestSize.Level1)
{
    RSRenderNode renderNode(0);
    auto& opincCache = renderNode.GetOpincRootCache();
    opincCache.SetCacheStateByRetrytime();
    EXPECT_TRUE(opincCache.unchangeCountUpper_ == 13);

    opincCache.tryCacheTimes_ = 4;
    opincCache.unchangeCountUpper_ = 3;
    opincCache.SetCacheStateByRetrytime();
    EXPECT_TRUE(opincCache.unchangeCountUpper_ == 23);

    opincCache.unchangeCountUpper_ = 200;
    opincCache.SetCacheStateByRetrytime();
    EXPECT_TRUE(opincCache.unchangeCountUpper_ == 200);
}

/**
 * @tc.name: NodeCacheStateReset001
 * @tc.desc: test results of NodeCacheStateReset
 * @tc.type: FUNC
 * @tc.require: issueI9UX8W
 */
HWTEST_F(RSOpincRootCacheTest, NodeCacheStateReset001, TestSize.Level1)
{
    RSRenderNode renderNode(0);
    auto& opincCache = renderNode.GetOpincRootCache();
    renderNode.stagingRenderParams_ = std::make_unique<RSRenderParams>(0);
    opincCache.NodeCacheStateReset(NodeCacheState::STATE_INIT);
    EXPECT_TRUE(opincCache.nodeCacheState_ == NodeCacheState::STATE_INIT);

    opincCache.NodeCacheStateReset(NodeCacheState::STATE_CHANGE);
    EXPECT_TRUE(!opincCache.OpincGetRootFlag());

    opincCache.isOpincRootFlag_ = true;
    opincCache.NodeCacheStateReset(NodeCacheState::STATE_CHANGE);
    EXPECT_FALSE(opincCache.isOpincRootFlag_);
}

/**
 * @tc.name: NodeCacheStateResetFromUnchange001
 * @tc.desc: cover wait-count init branch and retry-time update when resetting from unchange
 * @tc.type: FUNC
 * @tc.require: issueRootCacheBranch
 */
HWTEST_F(RSOpincRootCacheTest, NodeCacheStateResetFromUnchange001, TestSize.Level1)
{
    constexpr int32_t initWaitCount = 60;
    constexpr int32_t initialUnchangeUpper = 3;
    constexpr int32_t expectedUnchangeUpper = 13;

    RSRenderNode renderNode(0);
    auto& opincCache = renderNode.GetOpincRootCache();

    opincCache.nodeCacheState_ = NodeCacheState::STATE_UNCHANGE;
    opincCache.waitCount_ = 0;
    opincCache.isOpincRootFlag_ = false;
    opincCache.NodeCacheStateReset(NodeCacheState::STATE_CHANGE);
    EXPECT_EQ(opincCache.waitCount_, initWaitCount);
    EXPECT_TRUE(opincCache.GetCacheChangeFlag());

    opincCache.nodeCacheState_ = NodeCacheState::STATE_UNCHANGE;
    opincCache.isOpincRootFlag_ = true;
    opincCache.tryCacheTimes_ = 0;
    opincCache.unchangeCountUpper_ = initialUnchangeUpper;
    opincCache.NodeCacheStateReset(NodeCacheState::STATE_CHANGE);
    EXPECT_EQ(opincCache.unchangeCountUpper_, expectedUnchangeUpper);
    EXPECT_FALSE(opincCache.isOpincRootFlag_);
}

/**
 * @tc.name: GetCacheChangeFlag001
 * @tc.desc: test results of GetCacheChangeFlag
 * @tc.type: FUNC
 * @tc.require: issueI9UX8W
 */
HWTEST_F(RSOpincRootCacheTest, GetCacheChangeFlag001, TestSize.Level1)
{
    RSRenderNode renderNode(0);
    auto& opincCache = renderNode.GetOpincRootCache();

    ASSERT_FALSE(opincCache.GetCacheChangeFlag());

    opincCache.MarkSuggestOpincNode(true, true);
    ASSERT_TRUE(opincCache.GetCacheChangeFlag());
}

/**
 * @tc.name: GetNodeCacheState001
 * @tc.desc: test results of GetNodeCacheState
 * @tc.type: FUNC
 * @tc.require: issueI9UX8W
 */
HWTEST_F(RSOpincRootCacheTest, GetNodeCacheState001, TestSize.Level1)
{
    RSRenderNode renderNode(0);
    auto& opincCache = renderNode.GetOpincRootCache();

    ASSERT_EQ(opincCache.GetNodeCacheState(), NodeCacheState::STATE_INIT);

    opincCache.MarkSuggestOpincNode(true, true);
    ASSERT_EQ(opincCache.GetNodeCacheState(), NodeCacheState::STATE_CHANGE);
}

/**
 * @tc.name: GetUnchangeCount001
 * @tc.desc: test results of GetUnchangeCount
 * @tc.type: FUNC
 * @tc.require: issueI9UX8W
 */
HWTEST_F(RSOpincRootCacheTest, GetUnchangeCount001, TestSize.Level1)
{
    RSRenderNode renderNode(0);
    auto& opincCache = renderNode.GetOpincRootCache();

    ASSERT_EQ(opincCache.GetUnchangeCount(), 0);
}
} // namespace Rosen
} // namespace OHOS
