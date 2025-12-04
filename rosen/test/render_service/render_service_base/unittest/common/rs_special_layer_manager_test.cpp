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
class RSSpecialLayerManagerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSSpecialLayerManagerTest::SetUpTestCase() {}
void RSSpecialLayerManagerTest::TearDownTestCase() {}
void RSSpecialLayerManagerTest::SetUp() {}
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
    {
        AutoSpecialLayerStateRecover aslsr1(id1);
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
    for (uint32_t i = 0; i < MAX_IDS_SIZE; i++) {
        slManager.specialLayerIds_[SpecialLayerType::SKIP].insert(id++);
    }
    slManager.AddIds(SpecialLayerType::SKIP, id);
    ASSERT_EQ(slManager.specialLayerIds_[SpecialLayerType::SKIP].size(), MAX_IDS_SIZE);
    slManager.ClearSpecialLayerIds();
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
    std::deque<LeashPersistentId> deq(MAX_IDS_SIZE, persistentId);
    RSSpecialLayerManager::whiteListRootIds_ = std::stack<LeashPersistentId>(deq);

    RSSpecialLayerManager::SetWhiteListRootId(persistentId);
    ASSERT_EQ(RSSpecialLayerManager::whiteListRootIds_.size(), MAX_IDS_SIZE);
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
} // namespace OHOS::Rosen