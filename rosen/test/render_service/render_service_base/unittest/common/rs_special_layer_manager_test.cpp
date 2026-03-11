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

#include "common/rs_special_layer_manager.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
namespace {
NodeId GenerateNodeId()
{
    static NodeId nextId = 1;
    return nextId++;
}

ScreenId GenerateScreenId()
{
    static ScreenId nextId = 1;
    return nextId++;
}
}
class RSSpecialLayerManagerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSSpecialLayerManagerTest::SetUpTestCase() {}
void RSSpecialLayerManagerTest::TearDownTestCase() {}
void RSSpecialLayerManagerTest::SetUp()
{
    ScreenSpecialLayerInfo::screenSpecialLayerInfoByNode_ = {};
    ScreenSpecialLayerInfo::SetGlobalBlackList({});
}
void RSSpecialLayerManagerTest::TearDown() {}

/**
 * @tc.name: Set001
 * @tc.desc: test RSSpecialLayerManager::Set with mul-specialType
 * @tc.type: FUNC
 * @tc.require: issueIB18WF1
 */
HWTEST_F(RSSpecialLayerManagerTest, Set001, TestSize.Level1)
{
    RSSpecialLayerManager slManager;
    slManager.specialLayerType_ = SpecialLayerType::SECURITY;
    ASSERT_TRUE(slManager.Set(SpecialLayerType::SECURITY, false));
    ASSERT_FALSE(slManager.Set(SpecialLayerType::SECURITY, false));
    ASSERT_TRUE(slManager.Set(SpecialLayerType::PROTECTED, true));
}

/**
 * @tc.name: Find001
 * @tc.desc: test RSSpecialLayerManager::Find with security layer
 * @tc.type: FUNC
 * @tc.require: issueIB18WF1
 */
HWTEST_F(RSSpecialLayerManagerTest, Find001, TestSize.Level1)
{
    RSSpecialLayerManager slManager;
    slManager.specialLayerType_ = SpecialLayerType::SECURITY;
    ASSERT_TRUE(slManager.Find(SpecialLayerType::SECURITY));
}

/**
 * @tc.name: Get001
 * @tc.desc: test RSSpecialLayerManager::Get when has skip layer
 * @tc.type: FUNC
 * @tc.require: issueIB18WF1
 */
HWTEST_F(RSSpecialLayerManagerTest, Get001, TestSize.Level1)
{
    RSSpecialLayerManager slManager;
    slManager.specialLayerType_ = SpecialLayerType::SKIP;
    ASSERT_EQ(slManager.Get(), SpecialLayerType::SKIP);
}

/**
 * @tc.name: AddIds001
 * @tc.desc: test RSSpecialLayerManager::AddIds add skip layer to set
 * @tc.type:FUNC
 * @tc.require:issueIB18WF1
 */
HWTEST_F(RSSpecialLayerManagerTest, AddIds001, TestSize.Level1)
{
    RSSpecialLayerManager slManager;
    NodeId id = 1;
    slManager.AddIds(SpecialLayerType::SKIP, id);
    ASSERT_FALSE(slManager.specialLayerIds_[SpecialLayerType::SKIP].empty());
}

/**
 * @tc.name: RemoveIds001
 * @tc.desc: test RSSpecialLayerManager::RemoveIds remove skip layer from set
 * @tc.type: FUNC
 * @tc.require: issueIB18WF1
 */
HWTEST_F(RSSpecialLayerManagerTest, RemoveIds001, TestSize.Level1)
{
    RSSpecialLayerManager slManager;
    slManager.specialLayerType_ = SpecialLayerType::SKIP;
    NodeId id = 1;
    slManager.AddIds(SpecialLayerType::SKIP, id);
    slManager.RemoveIds(SpecialLayerType::SKIP, id);
    ASSERT_TRUE(slManager.specialLayerIds_[SpecialLayerType::SKIP].empty());
}

/**
 * @tc.name: SetWithScreen001
 * @tc.desc: test RSSpecialLayerManager::SetWithScreen with blackListNode type
 * @tc.type: FUNC
 * @tc.require: issueICTXOD
 */
HWTEST_F(RSSpecialLayerManagerTest, SetWithScreen001, TestSize.Level1)
{
    RSSpecialLayerManager slManager;
    uint64_t screenId1 = 1;
    uint64_t screenId2 = 2;
    ASSERT_TRUE(slManager.SetWithScreen(screenId1, SpecialLayerType::IS_BLACK_LIST, true));
    ASSERT_TRUE(slManager.FindWithScreen(screenId1, SpecialLayerType::IS_BLACK_LIST));
    ASSERT_FALSE(slManager.FindWithScreen(screenId2, SpecialLayerType::IS_BLACK_LIST));
    ASSERT_FALSE(slManager.FindWithScreen(screenId1, SpecialLayerType::HAS_BLACK_LIST));
    ASSERT_FALSE(slManager.SetWithScreen(screenId1, SpecialLayerType::IS_BLACK_LIST, true));
    ASSERT_TRUE(slManager.SetWithScreen(screenId1, SpecialLayerType::IS_BLACK_LIST, false));
    ASSERT_TRUE(slManager.SetWithScreen(screenId2, SpecialLayerType::IS_BLACK_LIST, false));
    slManager.ClearScreenSpecialLayer();
}

/**
 * @tc.name: AutoSpecialLayerStateRecover
 * @tc.desc: test class AutoSpecialLayerStateRecover RAII
 * @tc.type: FUNC
 * @tc.require: issueICWXJS
 */
HWTEST_F(RSSpecialLayerManagerTest, AutoSpecialLayerStateRecover, TestSize.Level1)
{
    LeashPersistentId id1 = 1;
    LeashPersistentId id2 = 2;
    RSSpecialLayerManager::ResetWhiteListRootId(id1);
    {
        AutoSpecialLayerStateRecover aslsr1(id1);
        ASSERT_EQ(RSSpecialLayerManager::GetCurWhiteListRootId(), id1);
        RSSpecialLayerManager::ResetWhiteListRootId(id2);
        ASSERT_EQ(RSSpecialLayerManager::GetCurWhiteListRootId(), id1);
        AutoSpecialLayerStateRecover aslsr2(id2);
        ASSERT_EQ(RSSpecialLayerManager::GetCurWhiteListRootId(), id2);
    }
    ASSERT_EQ(RSSpecialLayerManager::GetCurWhiteListRootId(), INVALID_LEASH_PERSISTENTID);
}

/**
 * @tc.name: AddIds002
 * @tc.desc: test RSSpecialLayerManager::AddIds add skip layer to set
 * @tc.type: FUNC
 * @tc.require: issue20168
 */
HWTEST_F(RSSpecialLayerManagerTest, AddIds002, TestSize.Level1)
{
    RSSpecialLayerManager slManager;
    NodeId id = 1;
    for (uint32_t i = 0; i < MAX_SPECIAL_LAYER_NUM; i++) {
        slManager.specialLayerIds_[SpecialLayerType::SKIP].insert(id++);
    }
    slManager.AddIds(SpecialLayerType::SKIP, id);
    ASSERT_EQ(slManager.specialLayerIds_[SpecialLayerType::SKIP].size(), MAX_SPECIAL_LAYER_NUM);
    slManager.Clear();
}

/**
 * @tc.name: ResetWhiteListRootId001
 * @tc.desc: test ResetWhiteListRootId while whiteListRootIds_ is empty
 * @tc.type: FUNC
 * @tc.require: issue19858
 */
HWTEST_F(RSSpecialLayerManagerTest, ResetWhiteListRootId001, TestSize.Level2)
{
    // test whiteListRootIds_ is empty
    RSSpecialLayerManager::ClearWhiteListRootIds();
    LeashPersistentId persistentId = 1;
    RSSpecialLayerManager::ResetWhiteListRootId(persistentId);

    // test top of whiteListRootIds_ mismatch
    RSSpecialLayerManager::SetWhiteListRootId(persistentId);
    RSSpecialLayerManager::ResetWhiteListRootId(persistentId + 1);
    ASSERT_FALSE(RSSpecialLayerManager::whiteListRootIds_.empty());

    // restore
    RSSpecialLayerManager::ClearWhiteListRootIds();
}

/**
 * @tc.name: SetWhiteListRootId001
 * @tc.desc: test ResetWhiteListRootId while whiteListRootIds_ exceeds size limit
 * @tc.type: FUNC
 * @tc.require: issue19858
 */
HWTEST_F(RSSpecialLayerManagerTest, SetWhiteListRootId001, TestSize.Level2)
{
    LeashPersistentId persistentId = 1;
    std::deque<LeashPersistentId> deq(MAX_SPECIAL_LAYER_NUM, persistentId);
    RSSpecialLayerManager::whiteListRootIds_ = std::stack<LeashPersistentId>(deq);

    RSSpecialLayerManager::SetWhiteListRootId(persistentId);
    ASSERT_EQ(RSSpecialLayerManager::whiteListRootIds_.size(), MAX_SPECIAL_LAYER_NUM);
    // restore
    RSSpecialLayerManager::ClearWhiteListRootIds();
}

/**
 * @tc.name: AddIdsWithScreen001
 * @tc.desc: test AddIdsWithScreen for different type
 * @tc.type: FUNC
 * @tc.require: issue20875
 */
HWTEST_F(RSSpecialLayerManagerTest, AddIdsWithScreen001, TestSize.Level2)
{
    RSSpecialLayerManager slManager;
    NodeId nodeId = 0;
    ScreenId screenId = 0;
    slManager.AddIdsWithScreen(screenId, SpecialLayerType::NONE, nodeId);
    slManager.RemoveIdsWithScreen(screenId, SpecialLayerType::NONE, nodeId);
    ASSERT_FALSE(slManager.FindWithScreen(screenId, SpecialLayerType::HAS_SKIP));

    slManager.AddIdsWithScreen(screenId, SpecialLayerType::SKIP, nodeId);
    slManager.AddIdsWithScreen(screenId, SpecialLayerType::SECURITY, nodeId);
    ASSERT_TRUE(slManager.FindWithScreen(screenId, SpecialLayerType::HAS_SKIP));
    
    slManager.RemoveIdsWithScreen(screenId, SpecialLayerType::SKIP, nodeId);
    slManager.RemoveIdsWithScreen(screenId, SpecialLayerType::SECURITY, nodeId);
    ASSERT_FALSE(slManager.FindWithScreen(screenId, SpecialLayerType::HAS_SKIP));
}

/**
 * @tc.name: AddIdsWithScreen002
 * @tc.desc: test a node has multiple child nodes as special layers
 * @tc.type: FUNC
 * @tc.require: issue20875
 */
HWTEST_F(RSSpecialLayerManagerTest, AddIdsWithScreen002, TestSize.Level2)
{
    RSSpecialLayerManager slManager;
    NodeId nodeId1 = 1;
    NodeId nodeId2 = 2;
    ScreenId screenId = 0;
    slManager.AddIdsWithScreen(screenId, SpecialLayerType::SKIP, nodeId1);
    slManager.AddIdsWithScreen(screenId, SpecialLayerType::SKIP, nodeId2);
    ASSERT_TRUE(slManager.FindWithScreen(screenId, SpecialLayerType::HAS_SKIP));

    slManager.RemoveIdsWithScreen(screenId, SpecialLayerType::SKIP, nodeId1);
    slManager.RemoveIdsWithScreen(screenId, SpecialLayerType::SKIP, nodeId2);
    ASSERT_FALSE(slManager.FindWithScreen(screenId, SpecialLayerType::HAS_SKIP));
}

/**
 * @tc.name: GetHasSlInVisibleRect
 * @tc.desc: test GetHasSlInVisibleRect records valid screenId
 * @tc.type: FUNC
 * @tc.require: issue21104
 */
HWTEST_F(RSSpecialLayerManagerTest, GetHasSlInVisibleRect, TestSize.Level2)
{
    RSSpecialLayerManager slManager;
    ScreenId screenId = 1;
    slManager.SetHasSlInVisibleRect(screenId, true);
    ASSERT_TRUE(slManager.GetHasSlInVisibleRect(screenId));
}

/**
 * @tc.name: GetIds001
 * @tc.desc: test GetIds while type is skip
 * @tc.require: issue21104
 */
HWTEST_F(RSSpecialLayerManagerTest, GetIds001, TestSize.Level1)
{
    RSSpecialLayerManager slManager;
    NodeId id = 1;
    slManager.AddIds(SpecialLayerType::SKIP, id);
    ASSERT_FALSE(slManager.GetIds(SpecialLayerType::HAS_SKIP).empty());
}

/**
 * @tc.name: GetIds002
 * @tc.desc: test GetIdsWithScreen while type is skip
 * @tc.type: FUNC
 * @tc.require: issue21104
 */
HWTEST_F(RSSpecialLayerManagerTest, GetIds002, TestSize.Level2)
{
    RSSpecialLayerManager slManager;
    NodeId nodeId = 1;
    slManager.AddIds(SpecialLayerType::SKIP, nodeId);
    ScreenId screenId = 1;
    ASSERT_TRUE(slManager.GetIdsWithScreen(screenId, SpecialLayerType::HAS_SKIP).empty());
}

/**
 * @tc.name: GetIds003
 * @tc.desc: test GetIdsWithScreen while type mismatch
 * @tc.type: FUNC
 * @tc.require: issue21104
 */
HWTEST_F(RSSpecialLayerManagerTest, GetIds003, TestSize.Level2)
{
    RSSpecialLayerManager slManager;
    NodeId nodeId = 1;
    slManager.AddIds(SpecialLayerType::SKIP, nodeId);
    ScreenId screenId = 1;
    ASSERT_TRUE(slManager.GetIdsWithScreen(screenId, SpecialLayerType::HAS_SECURITY).empty());
}

/**
 * @tc.name: GetIdsWithScreen001
 * @tc.desc: test GetIdsWithScreen while type is empty
 * @tc.type: FUNC
 * @tc.require: issue21104
 */
HWTEST_F(RSSpecialLayerManagerTest, GetIdsWithScreen001, TestSize.Level2)
{
    RSSpecialLayerManager slManager;
    NodeId nodeId = 1;
    ScreenId screenId = 0;
    slManager.AddIdsWithScreen(screenId, SpecialLayerType::SKIP, nodeId);
    ASSERT_TRUE(slManager.GetIdsWithScreen(screenId, SpecialLayerType::NONE).empty());
}

/**
 * @tc.name: GetIdsWithScreen002
 * @tc.desc: test GetIdsWithScreen while type is skip
 * @tc.type: FUNC
 * @tc.require: issue21104
 */
HWTEST_F(RSSpecialLayerManagerTest, GetIdsWithScreen002, TestSize.Level2)
{
    RSSpecialLayerManager slManager;
    NodeId nodeId = 1;
    ScreenId screenId = 0;
    slManager.AddIdsWithScreen(screenId, SpecialLayerType::SKIP, nodeId);
    ASSERT_FALSE(slManager.GetIdsWithScreen(screenId, SpecialLayerType::HAS_SKIP).empty());
}

/**
 * @tc.name: GetIdsWithScreen003
 * @tc.desc: test GetIdsWithScreen while type mismatch
 * @tc.type: FUNC
 * @tc.require: issue21104
 */
HWTEST_F(RSSpecialLayerManagerTest, GetIdsWithScreen003, TestSize.Level2)
{
    RSSpecialLayerManager slManager;
    NodeId nodeId = 1;
    ScreenId screenId = 0;
    slManager.AddIdsWithScreen(screenId, SpecialLayerType::SKIP, nodeId);
    ASSERT_TRUE(slManager.GetIdsWithScreen(screenId, SpecialLayerType::HAS_SECURITY).empty());
}

/**
 * @tc.name: AddIdsWithScreen003
 * @tc.desc: test while black list overflows
 * @tc.type: FUNC
 * @tc.require: issue21114
 */
HWTEST_F(RSSpecialLayerManagerTest, AddIdsWithScreen003, TestSize.Level2)
{
    RSSpecialLayerManager slManager;
    ScreenId screenId = 1;
    NodeId id = 1;
    for (uint32_t i = 0; i < MAX_SPECIAL_LAYER_NUM + 1; i++) {
        slManager.AddIdsWithScreen(screenId, SpecialLayerType::IS_BLACK_LIST, id++);
    }
    ASSERT_EQ(
        slManager.screenSpecialLayerIds_[screenId][SpecialLayerType::IS_BLACK_LIST].size(), MAX_SPECIAL_LAYER_NUM);
}

/**
 * @tc.name: GetWithScreen001
 * @tc.desc: test RSSpecialLayerManager::GetWithScreen when screen exists
 * @tc.type: FUNC
 * @tc.require: issue20875
 */
HWTEST_F(RSSpecialLayerManagerTest, GetWithScreen001, TestSize.Level2)
{
    RSSpecialLayerManager slManager;
    uint64_t screenId1 = GenerateScreenId();
    uint64_t screenId2 = GenerateScreenId();

    // Set screen 1 with BLACK_LIST type
    slManager.SetWithScreen(screenId1, SpecialLayerType::IS_BLACK_LIST, true);

    // Test GetWithScreen for existing screen
    uint32_t result = slManager.GetWithScreen(screenId1);
    ASSERT_NE(result, SpecialLayerType::NONE);

    // Test GetWithScreen for non-existing screen
    uint32_t result2 = slManager.GetWithScreen(screenId2);
    ASSERT_EQ(result2, SpecialLayerType::NONE);
}

/**
 * @tc.name: FindScreenHasType001
 * @tc.desc: test RSSpecialLayerManager::FindScreenHasType find screens with BLACK_LIST type
 * @tc.type: FUNC
 * @tc.require: issue20875
 */
HWTEST_F(RSSpecialLayerManagerTest, FindScreenHasType001, TestSize.Level2)
{
    RSSpecialLayerManager slManager;
    uint64_t screenId = GenerateScreenId();
    slManager.SetWithScreen(screenId, SpecialLayerType::IS_BLACK_LIST, true);

    auto screenIds = slManager.FindScreenHasType(SpecialLayerType::IS_BLACK_LIST);
    ASSERT_FALSE(screenIds.empty());
    screenIds = slManager.FindScreenHasType(SpecialLayerType::IS_WHITE_LIST);
    ASSERT_TRUE(screenIds.empty());
}

/**
 * @tc.name: Update001
 * @tc.desc: test ScreenSpecialLayerInfo::Update with SECURITY type
 * @tc.type: FUNC
 * @tc.require: issue20875
 */
HWTEST_F(RSSpecialLayerManagerTest, Update001, TestSize.Level2)
{
    SpecialLayerType type = SpecialLayerType::SECURITY;
    ScreenId screenId = GenerateScreenId();
    std::unordered_set<NodeId> nodeIds = {GenerateNodeId(), GenerateNodeId(), GenerateNodeId()};

    // Update screen special layer info
    ScreenSpecialLayerInfo::Update(type, screenId, nodeIds);

    // Clear by screen ID to verify update worked
    ScreenSpecialLayerInfo::ClearByScreenId(screenId);

    // Verify no more info for this screen
    auto screens = ScreenSpecialLayerInfo::QueryEnableScreen(type, {0, 0});
    ASSERT_TRUE(screens.empty());
}

/**
 * @tc.name: Update002
 * @tc.desc: test ScreenSpecialLayerInfo::Update with empty node IDs
 * @tc.type: FUNC
 * @tc.require: issue20875
 */
HWTEST_F(RSSpecialLayerManagerTest, Update002, TestSize.Level2)
{
    SpecialLayerType type = SpecialLayerType::SKIP;
    ScreenId screenId = GenerateScreenId();
    std::unordered_set<NodeId> nodeIds = {};

    // Update with empty node IDs
    ScreenSpecialLayerInfo::Update(type, screenId, nodeIds);

    // Verify no screens are enabled
    bool exists = ScreenSpecialLayerInfo::ExistEnableScreen(type);
    ASSERT_FALSE(exists);
}

/**
 * @tc.name: QueryEnableScreen001
 * @tc.desc: test ScreenSpecialLayerInfo::QueryEnableScreen find screens by NodeId
 * @tc.type: FUNC
 * @tc.require: issue20875
 */
HWTEST_F(RSSpecialLayerManagerTest, QueryEnableScreen001, TestSize.Level2)
{
    SpecialLayerType type = SpecialLayerType::SECURITY;
    ScreenId screenId = GenerateScreenId();
    NodeId nodeId = GenerateNodeId();

    ScreenSpecialLayerInfo::Update(type, screenId, {nodeId});
    auto screens = ScreenSpecialLayerInfo::QueryEnableScreen(type, {nodeId, nodeId});
    ASSERT_FALSE(screens.empty());
}

/**
 * @tc.name: QueryEnableScreen002
 * @tc.desc: test ScreenSpecialLayerInfo::QueryEnableScreen find screens by invalid id
 * @tc.type: FUNC
 * @tc.require: issue20875
 */
HWTEST_F(RSSpecialLayerManagerTest, QueryEnableScreen002, TestSize.Level2)
{
    SpecialLayerType type = SpecialLayerType::SECURITY;
    ScreenId screenId = GenerateScreenId();
    NodeId nodeId = GenerateNodeId();

    ScreenSpecialLayerInfo::Update(type, screenId, {nodeId});
    auto screens = ScreenSpecialLayerInfo::QueryEnableScreen(type, {INVALID_NODEID, INVALID_NODEID});
    ASSERT_TRUE(screens.empty());
}

/**
 * @tc.name: SetGlobalBlackList001
 * @tc.desc: test ScreenSpecialLayerInfo::SetGlobalBlackList and GetGlobalBlackList
 * @tc.type: FUNC
 * @tc.require: issue20875
 */
HWTEST_F(RSSpecialLayerManagerTest, SetGlobalBlackList001, TestSize.Level2)
{
    // Set global black list
    ScreenSpecialLayerInfo::SetGlobalBlackList({GenerateNodeId()});

    // Get and verify
    const auto& blacklist = ScreenSpecialLayerInfo::GetGlobalBlackList();
    ASSERT_EQ(blacklist.size(), 1);
    
    // Clean up
    ScreenSpecialLayerInfo::SetGlobalBlackList({});
}
} // namespace OHOS::Rosen