/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

class RSRenderNodeAutocacheTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSRenderNodeAutocacheTest::SetUpTestCase() {}
void RSRenderNodeAutocacheTest::TearDownTestCase() {}
void RSRenderNodeAutocacheTest::SetUp() {}
void RSRenderNodeAutocacheTest::TearDown() {}

/**
 * @tc.name: OpincSetInAppStateStart001
 * @tc.desc: test results of OpincSetInAppStateStart
 * @tc.type: FUNC
 * @tc.require: issueI9UX8W
 */
HWTEST_F(RSRenderNodeAutocacheTest, OpincSetInAppStateStart001, TestSize.Level1)
{
    RSRenderNode renderNode(0);
    bool unchangeMarkInApp = true;
    renderNode.OpincSetInAppStateStart(unchangeMarkInApp);
    EXPECT_TRUE(renderNode.isUnchangeMarkInApp_ == false);

    unchangeMarkInApp = false;
    renderNode.OpincSetInAppStateStart(unchangeMarkInApp);
    EXPECT_TRUE(renderNode.isUnchangeMarkInApp_);
}

/**
 * @tc.name: OpincSetInAppStateEnd001
 * @tc.desc: test results of OpincSetInAppStateEnd
 * @tc.type: FUNC
 * @tc.require: issueI9UX8W
 */
HWTEST_F(RSRenderNodeAutocacheTest, OpincSetInAppStateEnd001, TestSize.Level1)
{
    RSRenderNode renderNode(0);
    bool unchangeMarkInApp = true;
    renderNode.OpincSetInAppStateEnd(unchangeMarkInApp);
    EXPECT_TRUE(renderNode.isUnchangeMarkInApp_ == false);

    unchangeMarkInApp = false;
    renderNode.OpincSetInAppStateEnd(unchangeMarkInApp);
    EXPECT_TRUE(renderNode.isUnchangeMarkInApp_ == false);
}

/**
 * @tc.name: OpincQuickMarkStableNode001
 * @tc.desc: test results of OpincQuickMarkStableNode
 * @tc.type: FUNC
 * @tc.require: issueI9UX8W
 */
HWTEST_F(RSRenderNodeAutocacheTest, OpincQuickMarkStableNode001, TestSize.Level1)
{
    RSRenderNode renderNode(0);
    bool unchangeMarkInApp = false;
    bool unchangeMarkEnable = true;
    renderNode.OpincQuickMarkStableNode(unchangeMarkInApp, unchangeMarkEnable, false);
    EXPECT_TRUE(renderNode.isUnchangeMarkInApp_ == false);

    unchangeMarkInApp = true;
    renderNode.OpincQuickMarkStableNode(unchangeMarkInApp, unchangeMarkEnable, false);
    EXPECT_TRUE(renderNode.isUnchangeMarkInApp_ == false);

    renderNode.isSuggestOpincNode_ = true;
    renderNode.OpincQuickMarkStableNode(unchangeMarkInApp, unchangeMarkEnable, false);
    EXPECT_TRUE(renderNode.isSuggestOpincNode_);

    unchangeMarkEnable = false;
    renderNode.OpincQuickMarkStableNode(unchangeMarkInApp, unchangeMarkEnable, false);
    EXPECT_TRUE(renderNode.isUnchangeMarkEnable_);

    unchangeMarkEnable = true;
    renderNode.stagingRenderParams_ = std::make_unique<RSRenderParams>(0);
    renderNode.nodeCacheState_ = NodeCacheState::STATE_CHANGE;
    renderNode.OpincQuickMarkStableNode(unchangeMarkInApp, unchangeMarkEnable, false);
    EXPECT_TRUE(renderNode.isUnchangeMarkEnable_);

    renderNode.isSubTreeDirty_ = true;
    renderNode.OpincQuickMarkStableNode(unchangeMarkInApp, unchangeMarkEnable, false);
    EXPECT_TRUE(renderNode.isSubTreeDirty_);
}

/**
 * @tc.name: OpincQuickMarkStableNode002
 * @tc.desc: test results of OpincQuickMarkStableNode
 * @tc.type: FUNC
 * @tc.require: issueIB9E9A
 */
HWTEST_F(RSRenderNodeAutocacheTest, OpincQuickMarkStableNode002, TestSize.Level1)
{
    RSRenderNode renderNode(0);
    bool unchangeMarkInApp = false;
    bool unchangeMarkEnable = true;
    renderNode.OpincQuickMarkStableNode(unchangeMarkInApp, unchangeMarkEnable, true);
    EXPECT_TRUE(renderNode.isUnchangeMarkInApp_ == false);

    unchangeMarkInApp = true;
    renderNode.OpincQuickMarkStableNode(unchangeMarkInApp, unchangeMarkEnable, true);
    EXPECT_TRUE(renderNode.isUnchangeMarkInApp_ == false);

    renderNode.isSuggestOpincNode_ = true;
    renderNode.OpincQuickMarkStableNode(unchangeMarkInApp, unchangeMarkEnable, true);
    EXPECT_TRUE(renderNode.isSuggestOpincNode_);

    unchangeMarkEnable = false;
    renderNode.OpincQuickMarkStableNode(unchangeMarkInApp, unchangeMarkEnable, true);
    EXPECT_TRUE(renderNode.isUnchangeMarkEnable_);

    unchangeMarkEnable = true;
    renderNode.stagingRenderParams_ = std::make_unique<RSRenderParams>(0);
    renderNode.nodeCacheState_ = NodeCacheState::STATE_CHANGE;
    renderNode.OpincQuickMarkStableNode(unchangeMarkInApp, unchangeMarkEnable, true);
    EXPECT_TRUE(renderNode.isUnchangeMarkEnable_);

    renderNode.isSubTreeDirty_ = true;
    renderNode.OpincQuickMarkStableNode(unchangeMarkInApp, unchangeMarkEnable, true);
    EXPECT_TRUE(renderNode.isSubTreeDirty_);
}

/**
 * @tc.name: OpincUpdateRootFlag001
 * @tc.desc: test results of OpincUpdateRootFlag
 * @tc.type: FUNC
 * @tc.require: issueI9UX8W
 */
HWTEST_F(RSRenderNodeAutocacheTest, OpincUpdateRootFlag001, TestSize.Level1)
{
    RSRenderNode renderNode(0);
    bool unchangeMarkEnable = true;
    renderNode.OpincUpdateRootFlag(unchangeMarkEnable);
    EXPECT_TRUE(renderNode.isOpincRootFlag_ == false);

    renderNode.nodeCacheState_ = NodeCacheState::STATE_UNCHANGE;
    renderNode.isSuggestOpincNode_ = true;
    renderNode.stagingRenderParams_ = std::make_unique<RSRenderParams>(0);
    renderNode.OpincUpdateRootFlag(unchangeMarkEnable);
    EXPECT_TRUE(renderNode.IsOpincUnchangeState());

    renderNode.isUnchangeMarkEnable_ = true;
    renderNode.OpincUpdateRootFlag(unchangeMarkEnable);
    EXPECT_TRUE(renderNode.isUnchangeMarkEnable_ == false);
}

/**
 * @tc.name: IsOpincUnchangeState001
 * @tc.desc: test results of IsOpincUnchangeState
 * @tc.type: FUNC
 * @tc.require: issueI9UX8W
 */
HWTEST_F(RSRenderNodeAutocacheTest, IsOpincUnchangeState001, TestSize.Level1)
{
    RSRenderNode renderNode(0);
    bool res = renderNode.IsOpincUnchangeState();
    EXPECT_TRUE(res == false);

    renderNode.nodeCacheState_ = NodeCacheState::STATE_UNCHANGE;
    renderNode.isSuggestOpincNode_ = true;
    res = renderNode.IsOpincUnchangeState();
    EXPECT_TRUE(res);
}

/**
 * @tc.name: QuickGetNodeDebugInfo001
 * @tc.desc: test results of QuickGetNodeDebugInfo
 * @tc.type: FUNC
 * @tc.require: issueI9UX8W
 */
HWTEST_F(RSRenderNodeAutocacheTest, QuickGetNodeDebugInfo001, TestSize.Level1)
{
    RSRenderNode renderNode(0);
    renderNode.QuickGetNodeDebugInfo();
    EXPECT_TRUE(renderNode.isUnchangeMarkEnable_ == false);
}

/**
 * @tc.name: OpincUpdateNodeSupportFlag001
 * @tc.desc: test results of OpincUpdateNodeSupportFlag
 * @tc.type: FUNC
 * @tc.require: issueI9UX8W
 */
HWTEST_F(RSRenderNodeAutocacheTest, OpincUpdateNodeSupportFlag001, TestSize.Level1)
{
    RSRenderNode renderNode(0);
    renderNode.OpincUpdateNodeSupportFlag(true);
    EXPECT_TRUE(!renderNode.IsMarkedRenderGroup());
}

/**
 * @tc.name: IsMarkedRenderGroup001
 * @tc.desc: test results of IsMarkedRenderGroup
 * @tc.type: FUNC
 * @tc.require: issueI9UX8W
 */
HWTEST_F(RSRenderNodeAutocacheTest, IsMarkedRenderGroup001, TestSize.Level1)
{
    RSRenderNode renderNode(0);
    bool res = renderNode.IsMarkedRenderGroup();
    EXPECT_TRUE(!res);

    renderNode.nodeGroupType_ = RSRenderNode::NodeGroupType::GROUPED_BY_ANIM;
    renderNode.isOpincRootFlag_ = true;
    res = renderNode.IsMarkedRenderGroup();
    EXPECT_TRUE(res);
}

/**
 * @tc.name: OpincForcePrepareSubTree001
 * @tc.desc: test results of OpincForcePrepareSubTree
 * @tc.type: FUNC
 * @tc.require: issueI9UX8W
 */
HWTEST_F(RSRenderNodeAutocacheTest, OpincForcePrepareSubTree001, TestSize.Level1)
{
    RSRenderNode renderNode(0);
    bool autoCacheEnable = true;
    bool res = renderNode.OpincForcePrepareSubTree(autoCacheEnable);
    EXPECT_TRUE(!res);

    renderNode.isSuggestOpincNode_ = true;
    res = renderNode.OpincForcePrepareSubTree(autoCacheEnable);
    EXPECT_TRUE(res);
    autoCacheEnable = false;
    res = renderNode.OpincForcePrepareSubTree(autoCacheEnable);
    EXPECT_FALSE(res);
}

/**
 * @tc.name: OpincGetRootFlag001
 * @tc.desc: test results of OpincGetRootFlag
 * @tc.type: FUNC
 * @tc.require: issueI9UX8W
 */
HWTEST_F(RSRenderNodeAutocacheTest, OpincGetRootFlag001, TestSize.Level1)
{
    RSRenderNode renderNode(0);
    bool res = renderNode.OpincGetRootFlag();
    EXPECT_TRUE(!res);

    renderNode.isOpincRootFlag_ = true;
    res = renderNode.OpincGetRootFlag();
    EXPECT_TRUE(res);
}

/**
 * @tc.name: MarkSuggestOpincNode001
 * @tc.desc: test results of MarkSuggestOpincNode
 * @tc.type: FUNC
 * @tc.require: issueI9UX8W
 */
HWTEST_F(RSRenderNodeAutocacheTest, MarkSuggestOpincNode001, TestSize.Level1)
{
    RSRenderNode renderNode(0);
    renderNode.MarkSuggestOpincNode(true, true);
    EXPECT_TRUE(renderNode.isSuggestOpincNode_);
}

/**
 * @tc.name: GetSuggestOpincNode001
 * @tc.desc: test results of GetSuggestOpincNode
 * @tc.type: FUNC
 * @tc.require: issueI9UX8W
 */
HWTEST_F(RSRenderNodeAutocacheTest, GetSuggestOpincNode001, TestSize.Level1)
{
    RSRenderNode renderNode(0);
    EXPECT_TRUE(!renderNode.GetSuggestOpincNode());

    renderNode.isSuggestOpincNode_ = true;
    EXPECT_TRUE(renderNode.GetSuggestOpincNode());
}

/**
 * @tc.name: NodeCacheStateChange001
 * @tc.desc: test results of NodeCacheStateChange
 * @tc.type: FUNC
 * @tc.require: issueI9UX8W
 */
HWTEST_F(RSRenderNodeAutocacheTest, NodeCacheStateChange001, TestSize.Level1)
{
    RSRenderNode renderNode(0);
    renderNode.stagingRenderParams_ = std::make_unique<RSRenderParams>(0);
    renderNode.NodeCacheStateChange(NodeChangeType::KEEP_UNCHANGE);
    renderNode.NodeCacheStateChange(NodeChangeType::KEEP_UNCHANGE);
    renderNode.NodeCacheStateChange(NodeChangeType::KEEP_UNCHANGE);
    renderNode.NodeCacheStateChange(NodeChangeType::KEEP_UNCHANGE);
    EXPECT_TRUE(renderNode.nodeCacheState_ == NodeCacheState::STATE_UNCHANGE);

    renderNode.NodeCacheStateChange(NodeChangeType::SELF_DIRTY);
    EXPECT_TRUE(renderNode.nodeCacheState_ != NodeCacheState::STATE_UNCHANGE);
}

/**
 * @tc.name: SetCacheStateByRetrytime001
 * @tc.desc: test results of SetCacheStateByRetrytime
 * @tc.type: FUNC
 * @tc.require: issueI9UX8W
 */
HWTEST_F(RSRenderNodeAutocacheTest, SetCacheStateByRetrytime001, TestSize.Level1)
{
    RSRenderNode renderNode(0);
    renderNode.SetCacheStateByRetrytime();
    EXPECT_TRUE(renderNode.unchangeCountUpper_ == 13);

    renderNode.tryCacheTimes_ = 4;
    renderNode.unchangeCountUpper_ = 3;
    renderNode.SetCacheStateByRetrytime();
    EXPECT_TRUE(renderNode.unchangeCountUpper_ == 63);
}

/**
 * @tc.name: NodeCacheStateReset001
 * @tc.desc: test results of NodeCacheStateReset
 * @tc.type: FUNC
 * @tc.require: issueI9UX8W
 */
HWTEST_F(RSRenderNodeAutocacheTest, NodeCacheStateReset001, TestSize.Level1)
{
    RSRenderNode renderNode(0);
    renderNode.stagingRenderParams_ = std::make_unique<RSRenderParams>(0);
    renderNode.NodeCacheStateReset(NodeCacheState::STATE_INIT);
    EXPECT_TRUE(renderNode.nodeCacheState_ == NodeCacheState::STATE_INIT);

    renderNode.NodeCacheStateReset(NodeCacheState::STATE_CHANGE);
    EXPECT_TRUE(!renderNode.OpincGetRootFlag());

    renderNode.isOpincRootFlag_ = true;
    renderNode.NodeCacheStateReset(NodeCacheState::STATE_CHANGE);
    EXPECT_FALSE(renderNode.isOpincRootFlag_);
}
} // namespace Rosen
} // namespace OHOS
