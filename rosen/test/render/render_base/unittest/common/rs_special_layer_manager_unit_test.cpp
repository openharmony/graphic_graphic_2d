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
 * @tc.type:FUNC
 * @tc.require:issueIB18WF1
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
 * @tc.type:FUNC
 * @tc.require:issueIB18WF1
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
 * @tc.type:FUNC
 * @tc.require:issueIB18WF1
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
 * @tc.desc: test RSSpecialLayerManager::RemoveIds remopve skip layer from set
 * @tc.type:FUNC
 * @tc.require:issueIB18WF1
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
} // namespace OHOS::Rosen