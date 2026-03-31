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

#include "feature/opinc/rs_opinc_cache.h"
#include "params/rs_render_params.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {

class RSOpincCacheTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSOpincCacheTest::SetUpTestCase() {}
void RSOpincCacheTest::TearDownTestCase() {}
void RSOpincCacheTest::SetUp() {}
void RSOpincCacheTest::TearDown() {}

/**
 * @tc.name: OpincSetInAppStateStart001
 * @tc.desc: test results of OpincSetInAppStateStart
 * @tc.type: FUNC
 * @tc.require: issueI9UX8W
 */
HWTEST_F(RSOpincCacheTest, OpincSetInAppStateStart001, TestSize.Level1)
{
    RSRenderNode renderNode(0);
    auto& opincCache = renderNode.GetOpincCache();
    bool unchangeMarkInApp = true;
    opincCache.OpincSetInAppStateStart(unchangeMarkInApp);
    EXPECT_TRUE(opincCache.isUnchangeMarkInApp_ == false);

    unchangeMarkInApp = false;
    opincCache.OpincSetInAppStateStart(unchangeMarkInApp);
    EXPECT_TRUE(opincCache.isUnchangeMarkInApp_);
}

/**
 * @tc.name: OpincSetInAppStateEnd001
 * @tc.desc: test results of OpincSetInAppStateEnd
 * @tc.type: FUNC
 * @tc.require: issueI9UX8W
 */
HWTEST_F(RSOpincCacheTest, OpincSetInAppStateEnd001, TestSize.Level1)
{
    RSRenderNode renderNode(0);
    auto& opincCache = renderNode.GetOpincCache();
    bool unchangeMarkInApp = true;
    opincCache.OpincSetInAppStateEnd(unchangeMarkInApp);
    EXPECT_TRUE(opincCache.isUnchangeMarkInApp_ == false);

    unchangeMarkInApp = false;
    opincCache.OpincSetInAppStateEnd(unchangeMarkInApp);
    EXPECT_TRUE(opincCache.isUnchangeMarkInApp_ == false);
}

/**
 * @tc.name: OpincQuickMarkStableNode001
 * @tc.desc: test results of OpincQuickMarkStableNode
 * @tc.type: FUNC
 * @tc.require: issueI9UX8W
 */
HWTEST_F(RSOpincCacheTest, OpincQuickMarkStableNode001, TestSize.Level1)
{
    RSRenderNode renderNode(0);
    auto& opincCache = renderNode.GetOpincCache();
    bool unchangeMarkInApp = false;
    bool unchangeMarkEnable = true;
    opincCache.OpincQuickMarkStableNode(unchangeMarkInApp, unchangeMarkEnable, false);
    EXPECT_TRUE(opincCache.isUnchangeMarkInApp_ == false);

    unchangeMarkInApp = true;
    opincCache.OpincQuickMarkStableNode(unchangeMarkInApp, unchangeMarkEnable, false);
    EXPECT_TRUE(opincCache.isUnchangeMarkInApp_ == false);

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
HWTEST_F(RSOpincCacheTest, OpincQuickMarkStableNode002, TestSize.Level1)
{
    RSRenderNode renderNode(0);
    auto& opincCache = renderNode.GetOpincCache();
    bool unchangeMarkInApp = false;
    bool unchangeMarkEnable = true;
    opincCache.OpincQuickMarkStableNode(unchangeMarkInApp, unchangeMarkEnable, false);
    EXPECT_TRUE(opincCache.isUnchangeMarkInApp_ == false);

    unchangeMarkInApp = true;
    opincCache.OpincQuickMarkStableNode(unchangeMarkInApp, unchangeMarkEnable, false);
    EXPECT_TRUE(opincCache.isUnchangeMarkInApp_ == false);

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
    EXPECT_TRUE(opincCache.subTreeSupportFlag_);
}

/**
 * @tc.name: OpincUpdateRootFlag001
 * @tc.desc: test results of OpincUpdateRootFlag
 * @tc.type: FUNC
 * @tc.require: issueI9UX8W
 */
HWTEST_F(RSOpincCacheTest, OpincUpdateRootFlag001, TestSize.Level1)
{
    RSRenderNode renderNode(0);
    auto& opincCache = renderNode.GetOpincCache();
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
 * @tc.name: IsOpincUnchangeState001
 * @tc.desc: test results of IsOpincUnchangeState
 * @tc.type: FUNC
 * @tc.require: issueI9UX8W
 */
HWTEST_F(RSOpincCacheTest, IsOpincUnchangeState001, TestSize.Level1)
{
    RSRenderNode renderNode(0);
    auto& opincCache = renderNode.GetOpincCache();
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
HWTEST_F(RSOpincCacheTest, IsMarkedRenderGroup001, TestSize.Level1)
{
    RSRenderNode renderNode(0);
    auto& opincCache = renderNode.GetOpincCache();
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
HWTEST_F(RSOpincCacheTest, OpincForcePrepareSubTree001, TestSize.Level1)
{
    RSRenderNode renderNode(0);
    auto& opincCache = renderNode.GetOpincCache();
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
 * @tc.name: OpincGetRootFlag001
 * @tc.desc: test results of OpincGetRootFlag
 * @tc.type: FUNC
 * @tc.require: issueI9UX8W
 */
HWTEST_F(RSOpincCacheTest, OpincGetRootFlag001, TestSize.Level1)
{
    RSRenderNode renderNode(0);
    auto& opincCache = renderNode.GetOpincCache();
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
HWTEST_F(RSOpincCacheTest, MarkSuggestOpincNode001, TestSize.Level1)
{
    RSRenderNode renderNode(0);
    auto& opincCache = renderNode.GetOpincCache();
    opincCache.MarkSuggestOpincNode(true, true);
    EXPECT_TRUE(opincCache.isSuggestOpincNode_);
}

/**
 * @tc.name: IsSuggestOpincNode001
 * @tc.desc: test results of IsSuggestOpincNode
 * @tc.type: FUNC
 * @tc.require: issueI9UX8W
 */
HWTEST_F(RSOpincCacheTest, IsSuggestOpincNode001, TestSize.Level1)
{
    RSRenderNode renderNode(0);
    auto& opincCache = renderNode.GetOpincCache();
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
HWTEST_F(RSOpincCacheTest, NodeCacheStateChange001, TestSize.Level1)
{
    RSRenderNode renderNode(0);
    auto& opincCache = renderNode.GetOpincCache();
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
HWTEST_F(RSOpincCacheTest, SetCacheStateByRetrytime001, TestSize.Level1)
{
    RSRenderNode renderNode(0);
    auto& opincCache = renderNode.GetOpincCache();
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
HWTEST_F(RSOpincCacheTest, NodeCacheStateReset001, TestSize.Level1)
{
    RSRenderNode renderNode(0);
    auto& opincCache = renderNode.GetOpincCache();
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
 * @tc.name: UpdateSubTreeSupportFlag
 * @tc.desc: test results of UpdateSubTreeSupportFlag
 * @tc.type: FUNC
 * @tc.require: issueI9UX8W
 */
HWTEST_F(RSOpincCacheTest, UpdateSubTreeSupportFlag, TestSize.Level1)
{
    RSRenderNode renderNode(0);
    auto& opincCache = renderNode.GetOpincCache();
    opincCache.subTreeSupportFlag_ = true;
    opincCache.UpdateSubTreeSupportFlag(false, false, false);
    ASSERT_FALSE(opincCache.GetSubTreeSupportFlag());

    opincCache.subTreeSupportFlag_ = true;
    opincCache.UpdateSubTreeSupportFlag(false, false, true);
    ASSERT_FALSE(opincCache.GetSubTreeSupportFlag());

    opincCache.subTreeSupportFlag_ = true;
    opincCache.UpdateSubTreeSupportFlag(false, true, false);
    ASSERT_FALSE(opincCache.GetSubTreeSupportFlag());

    opincCache.subTreeSupportFlag_ = true;
    opincCache.UpdateSubTreeSupportFlag(true, false, false);
    ASSERT_FALSE(opincCache.GetSubTreeSupportFlag());

    opincCache.subTreeSupportFlag_ = true;
    opincCache.UpdateSubTreeSupportFlag(true, true, false);
    ASSERT_FALSE(opincCache.GetSubTreeSupportFlag());

    opincCache.subTreeSupportFlag_ = true;
    opincCache.UpdateSubTreeSupportFlag(false, true, true);
    ASSERT_FALSE(opincCache.GetSubTreeSupportFlag());

    opincCache.subTreeSupportFlag_ = true;
    opincCache.UpdateSubTreeSupportFlag(true, false, true);
    ASSERT_TRUE(opincCache.GetSubTreeSupportFlag());

    opincCache.subTreeSupportFlag_ = true;
    opincCache.UpdateSubTreeSupportFlag(true, true, true);
    ASSERT_FALSE(opincCache.GetSubTreeSupportFlag());
}

/**
 * @tc.name: OpincSubTreeSkip001
 * @tc.desc: test results of OpincSubTreeSkip
 * @tc.type: FUNC
 * @tc.require: issue22996
 */
HWTEST_F(RSOpincCacheTest, OpincSubTreeSkip001, TestSize.Level1)
{
    RSRenderNode renderNode(0);
    auto& opincCache = renderNode.GetOpincCache();
    opincCache.unchangeCount_ = 0;
    opincCache.unchangeCountUpper_ = 3;
    {
        opincCache.isSuggestOpincNode_ = false;
        opincCache.OpincSubTreeSkip();
        EXPECT_EQ(opincCache.unchangeCount_, 0);
    }

    {
        opincCache.isSuggestOpincNode_ = true;
        opincCache.nodeCacheState_ = NodeCacheState::STATE_UNCHANGE;
        opincCache.OpincSubTreeSkip();
        EXPECT_EQ(opincCache.unchangeCount_, 0);
    }

    {
        opincCache.isSuggestOpincNode_ = true;
        opincCache.nodeCacheState_ = NodeCacheState::STATE_CHANGE;
        opincCache.unchangeCount_ = 4;
        opincCache.unchangeCountUpper_ = 3;
        opincCache.OpincSubTreeSkip();
        EXPECT_EQ(opincCache.unchangeCount_, 4);
    }

    {
        opincCache.isSuggestOpincNode_ = true;
        opincCache.nodeCacheState_ = NodeCacheState::STATE_CHANGE;
        opincCache.unchangeCount_ = 0;
        opincCache.unchangeCountUpper_ = 3;
        opincCache.OpincSubTreeSkip();
        EXPECT_EQ(opincCache.unchangeCount_, opincCache.unchangeCountUpper_);
    }
}

/**
 * @tc.name: SetCurNodeTreeSupportFlag
 * @tc.desc: test results of SetCurNodeTreeSupportFlag
 * @tc.type: FUNC
 * @tc.require: issueI9UX8W
 */
HWTEST_F(RSOpincCacheTest, SetCurNodeTreeSupportFlag, TestSize.Level1)
{
    RSRenderNode renderNode(0);
    auto& opincCache = renderNode.GetOpincCache();
    opincCache.SetCurNodeTreeSupportFlag(true);
    ASSERT_TRUE(opincCache.GetCurNodeTreeSupportFlag());
}

/**
 * @tc.name: MarkSuggestLayerPartRenderNode001
 * @tc.desc: test results of MarkSuggestLayerPartRenderNode
 * @tc.type: FUNC
 * @tc.require: issueLayerPart
 */
HWTEST_F(RSOpincCacheTest, MarkSuggestLayerPartRenderNode001, TestSize.Level1)
{
    RSRenderNode renderNode(0);
    auto& opincCache = renderNode.GetOpincCache();

    opincCache.MarkSuggestLayerPartRenderNode(true);
    ASSERT_TRUE(opincCache.IsSuggestLayerPartRenderNode());

    opincCache.MarkSuggestLayerPartRenderNode(false);
    ASSERT_FALSE(opincCache.IsSuggestLayerPartRenderNode());
}

/**
 * @tc.name: IsSuggestLayerPartRenderNode001
 * @tc.desc: test results of IsSuggestLayerPartRenderNode
 * @tc.type: FUNC
 * @tc.require: issueLayerPart
 */
HWTEST_F(RSOpincCacheTest, IsSuggestLayerPartRenderNode001, TestSize.Level1)
{
    RSRenderNode renderNode(0);
    auto& opincCache = renderNode.GetOpincCache();

    ASSERT_FALSE(opincCache.IsSuggestLayerPartRenderNode());

    opincCache.MarkSuggestLayerPartRenderNode(true);
    ASSERT_TRUE(opincCache.IsSuggestLayerPartRenderNode());
}

/**
 * @tc.name: SetLayerPartRender001
 * @tc.desc: test results of SetLayerPartRender
 * @tc.type: FUNC
 * @tc.require: issueLayerPart
 */
HWTEST_F(RSOpincCacheTest, SetLayerPartRender001, TestSize.Level1)
{
    RSRenderNode renderNode(0);
    auto& opincCache = renderNode.GetOpincCache();

    opincCache.SetLayerPartRender(true);
    ASSERT_TRUE(opincCache.IsLayerPartRender());

    opincCache.SetLayerPartRender(false);
    ASSERT_FALSE(opincCache.IsLayerPartRender());
}

/**
 * @tc.name: IsLayerPartRender001
 * @tc.desc: test results of IsLayerPartRender
 * @tc.type: FUNC
 * @tc.require: issueLayerPart
 */
HWTEST_F(RSOpincCacheTest, IsLayerPartRender001, TestSize.Level1)
{
    RSRenderNode renderNode(0);
    auto& opincCache = renderNode.GetOpincCache();

    ASSERT_FALSE(opincCache.IsLayerPartRender());

    opincCache.SetLayerPartRender(true);
    ASSERT_TRUE(opincCache.IsLayerPartRender());
}

/**
 * @tc.name: IsLayerPartRenderUnchangeState001
 * @tc.desc: test results of IsLayerPartRenderUnchangeState
 * @tc.type: FUNC
 * @tc.require: issueLayerPart
 */
HWTEST_F(RSOpincCacheTest, IsLayerPartRenderUnchangeState001, TestSize.Level1)
{
    RSRenderNode renderNode(0);
    auto& opincCache = renderNode.GetOpincCache();

    // count=0, 0<=3 -> false, count=1
    ASSERT_FALSE(opincCache.IsLayerPartRenderUnchangeState());
    // count=1, 1<=3 -> false, count=2
    ASSERT_FALSE(opincCache.IsLayerPartRenderUnchangeState());
    // count=2, 2<=3 -> false, count=3
    ASSERT_FALSE(opincCache.IsLayerPartRenderUnchangeState());
    // count=3, 3<=3 -> false, count=4
    ASSERT_FALSE(opincCache.IsLayerPartRenderUnchangeState());
    // count=4, 4>3 -> true
    ASSERT_TRUE(opincCache.IsLayerPartRenderUnchangeState());
}

/**
 * @tc.name: IsLayerPartRenderUnchangeState002
 * @tc.desc: test results of IsLayerPartRenderUnchangeState after reset
 * @tc.type: FUNC
 * @tc.require: issueLayerPart
 */
HWTEST_F(RSOpincCacheTest, IsLayerPartRenderUnchangeState002, TestSize.Level1)
{
    RSRenderNode renderNode(0);
    auto& opincCache = renderNode.GetOpincCache();

    for (int i = 0; i < 5; i++) {
        opincCache.IsLayerPartRenderUnchangeState();
    }
    ASSERT_TRUE(opincCache.IsLayerPartRenderUnchangeState());

    opincCache.ResetLayerPartRenderUnchangeState();

    ASSERT_FALSE(opincCache.IsLayerPartRenderUnchangeState());
}

/**
 * @tc.name: ResetLayerPartRenderUnchangeState001
 * @tc.desc: test results of ResetLayerPartRenderUnchangeState
 * @tc.type: FUNC
 * @tc.require: issueLayerPart
 */
HWTEST_F(RSOpincCacheTest, ResetLayerPartRenderUnchangeState001, TestSize.Level1)
{
    RSRenderNode renderNode(0);
    auto& opincCache = renderNode.GetOpincCache();

    for (int i = 0; i < 5; i++) {
        opincCache.IsLayerPartRenderUnchangeState();
    }
    ASSERT_TRUE(opincCache.IsLayerPartRenderUnchangeState());

    opincCache.ResetLayerPartRenderUnchangeState();

    // After reset, count=0, need 4 calls to reach count=4
    ASSERT_FALSE(opincCache.IsLayerPartRenderUnchangeState());
    ASSERT_FALSE(opincCache.IsLayerPartRenderUnchangeState());
    ASSERT_FALSE(opincCache.IsLayerPartRenderUnchangeState());
    ASSERT_FALSE(opincCache.IsLayerPartRenderUnchangeState());
    ASSERT_TRUE(opincCache.IsLayerPartRenderUnchangeState());
}

/**
 * @tc.name: GetLayerPartRenderDirtyManager001
 * @tc.desc: test results of GetLayerPartRenderDirtyManager
 * @tc.type: FUNC
 * @tc.require: issueLayerPart
 */
HWTEST_F(RSOpincCacheTest, GetLayerPartRenderDirtyManager001, TestSize.Level1)
{
    RSRenderNode renderNode(0);
    auto& opincCache = renderNode.GetOpincCache();

    auto& dirtyManager = opincCache.GetLayerPartRenderDirtyManager();
    ASSERT_NE(dirtyManager, nullptr);

    auto& dirtyManager2 = opincCache.GetLayerPartRenderDirtyManager();
    ASSERT_EQ(dirtyManager, dirtyManager2);
}

/**
 * @tc.name: GetLayerPartRenderDirtyManager002
 * @tc.desc: test GetLayerPartRenderDirtyManager returns same instance
 * @tc.type: FUNC
 * @tc.require: issueLayerPart
 */
HWTEST_F(RSOpincCacheTest, GetLayerPartRenderDirtyManager002, TestSize.Level1)
{
    RSRenderNode renderNode(0);
    auto& opincCache = renderNode.GetOpincCache();

    auto& dirtyManager1 = opincCache.GetLayerPartRenderDirtyManager();
    ASSERT_NE(dirtyManager1, nullptr);

    auto& dirtyManager2 = opincCache.GetLayerPartRenderDirtyManager();
    ASSERT_EQ(dirtyManager1.get(), dirtyManager2.get());
}

/**
 * @tc.name: SetLayerPartRenderDirtyFlag001
 * @tc.desc: test results of SetLayerPartRenderDirtyFlag and GetLayerPartRenderDirtyFlag
 * @tc.type: FUNC
 * @tc.require: issueLayerPart
 */
HWTEST_F(RSOpincCacheTest, SetLayerPartRenderDirtyFlag001, TestSize.Level1)
{
    RSRenderNode renderNode(0);
    auto& opincCache = renderNode.GetOpincCache();

    opincCache.SetLayerPartRenderDirtyFlag(true);
    ASSERT_TRUE(opincCache.GetLayerPartRenderDirtyFlag());

    opincCache.SetLayerPartRenderDirtyFlag(false);
    ASSERT_FALSE(opincCache.GetLayerPartRenderDirtyFlag());
}

/**
 * @tc.name: SetLayerPartRenderOldAbsDrawRect001
 * @tc.desc: test results of SetLayerPartRenderOldAbsDrawRect and GetLayerPartRenderOldAbsDrawRect
 * @tc.type: FUNC
 * @tc.require: issueLayerPart
 */
HWTEST_F(RSOpincCacheTest, SetLayerPartRenderOldAbsDrawRect001, TestSize.Level1)
{
    constexpr int32_t rectLeft = 10;
    constexpr int32_t rectTop = 10;
    constexpr int32_t rectWidth = 100;
    constexpr int32_t rectHeight = 100;

    RSRenderNode renderNode(0);
    auto& opincCache = renderNode.GetOpincCache();
    RectI absDrawRect = { rectLeft, rectTop, rectWidth, rectHeight };

    opincCache.SetLayerPartRenderOldAbsDrawRect(absDrawRect);

    ASSERT_EQ(opincCache.GetLayerPartRenderOldAbsDrawRect(), absDrawRect);
}

/**
 * @tc.name: GetCacheChangeFlag001
 * @tc.desc: test results of GetCacheChangeFlag
 * @tc.type: FUNC
 * @tc.require: issueI9UX8W
 */
HWTEST_F(RSOpincCacheTest, GetCacheChangeFlag001, TestSize.Level1)
{
    RSRenderNode renderNode(0);
    auto& opincCache = renderNode.GetOpincCache();

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
HWTEST_F(RSOpincCacheTest, GetNodeCacheState001, TestSize.Level1)
{
    RSRenderNode renderNode(0);
    auto& opincCache = renderNode.GetOpincCache();

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
HWTEST_F(RSOpincCacheTest, GetUnchangeCount001, TestSize.Level1)
{
    RSRenderNode renderNode(0);
    auto& opincCache = renderNode.GetOpincCache();

    ASSERT_EQ(opincCache.GetUnchangeCount(), 0);
}
} // namespace Rosen
} // namespace OHOS
