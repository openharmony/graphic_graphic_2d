/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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
#include "common/rs_common_def.h"
#include "ui/rs_display_node.h"
#include "transaction/rs_transaction.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class RSDisplayModeTest : public testing::Test {
public:
    uint64_t screenId1 = 10; // To extinguish between the original screenIds and test screenIds
    uint64_t screenId2 = 11;
    static constexpr uint32_t TEST_SLEEP_S = 1;
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSDisplayModeTest::SetUpTestCase() {}
void RSDisplayModeTest::TearDownTestCase() {}
void RSDisplayModeTest::SetUp() {}
void RSDisplayModeTest::TearDown() {}

/**
 * @tc.name: CreateExpand
 * @tc.desc: Create expand display node
 * @tc.type:FUNC
 * @tc.require: issueI5CAAF
 */
HWTEST_F(RSDisplayModeTest, CreateExpand, Function | MediumTest  | Level2)
{
    /**
    * step1. create RSDisplayNode
    */
    RSDisplayNodeConfig config  = {screenId1, false, 0};
    RSDisplayNode::SharedPtr displayNode = RSDisplayNode::Create(config);
    ASSERT_TRUE(displayNode != nullptr);

    auto isMirrorDisplay = displayNode->IsMirrorDisplay();
    EXPECT_EQ(isMirrorDisplay, false);

    /**
    * step2. flush commands
    */
    RSTransaction::FlushImplicitTransaction();

    /**
    * step3. delete nodes
    */
    sleep(TEST_SLEEP_S);
    displayNode->RemoveFromTree();
}

/**
 * @tc.name: CreateMirror
 * @tc.desc: Create mirror display node
 * @tc.type:FUNC
 * @tc.require: issueI5CAAF
 */
HWTEST_F(RSDisplayModeTest, CreateMirror, Function | MediumTest  | Level2)
{
    /**
    * step1. create RSDisplayNode
    */
    RSDisplayNodeConfig configSrc = {screenId1, false, 0};
    RSDisplayNode::SharedPtr displayNodeSrc = RSDisplayNode::Create(configSrc);
    ASSERT_TRUE(displayNodeSrc != nullptr);

    RSDisplayNodeConfig config = {screenId2, true, displayNodeSrc->GetId()};
    RSDisplayNode::SharedPtr displayNode = RSDisplayNode::Create(config);
    ASSERT_TRUE(displayNode != nullptr);

    auto isMirrorDisplay = displayNode->IsMirrorDisplay();
    EXPECT_EQ(isMirrorDisplay, true);

    /**
    * step2. flush commands
    */
    RSTransaction::FlushImplicitTransaction();

    /**
    * step3. delete nodes
    */
    sleep(TEST_SLEEP_S);
    displayNodeSrc->RemoveFromTree();
    displayNode->RemoveFromTree();
}

/**
 * @tc.name: SetDisplayNodeMirrorConfigExpand
 * @tc.desc: Modify display mode type to expand display
 * @tc.type:FUNC
 * @tc.require: issueI5CAAF
 */
HWTEST_F(RSDisplayModeTest, SetDisplayNodeMirrorConfigExpand, Function | MediumTest  | Level2)
{
    /**
    * step1. create RSDisplayNode
    */
    RSDisplayNodeConfig configSrc = {screenId1, false, 0};
    RSDisplayNode::SharedPtr displayNodeSrc = RSDisplayNode::Create(configSrc);
    ASSERT_TRUE(displayNodeSrc != nullptr);

    RSDisplayNodeConfig config = {screenId2, true, screenId1};
    RSDisplayNode::SharedPtr displayNode = RSDisplayNode::Create(config);
    ASSERT_TRUE(displayNode != nullptr);
    auto isMirrorDisplay = displayNode->IsMirrorDisplay();
    EXPECT_EQ(isMirrorDisplay, true);

    /**
    * step2. set display mode
    */
    RSDisplayNodeConfig configNew = {screenId2, false, 0};
    displayNode->SetDisplayNodeMirrorConfig(configNew);
    isMirrorDisplay = displayNode->IsMirrorDisplay();
    EXPECT_EQ(isMirrorDisplay, false);

    /**
    * step3. flush commands
    */
    RSTransaction::FlushImplicitTransaction();

    /**
    * step4. delete nodes
    */
    sleep(TEST_SLEEP_S);
    displayNodeSrc->RemoveFromTree();
    displayNode->RemoveFromTree();
}

/**
 * @tc.name: SetDisplayNodeMirrorConfigMirror
 * @tc.desc: Modify display mode type to mirror display
 * @tc.type:FUNC
 * @tc.require: issueI5CAAF
 */
HWTEST_F(RSDisplayModeTest, SetDisplayNodeMirrorConfigMirror, Function | MediumTest  | Level2)
{
    /**
    * step1. create RSDisplayNode
    */
    RSDisplayNodeConfig configSrc = {screenId1, false, 0};
    RSDisplayNode::SharedPtr displayNodeSrc = RSDisplayNode::Create(configSrc);
    ASSERT_TRUE(displayNodeSrc != nullptr);

    RSDisplayNodeConfig config = {screenId2, false, 0};
    RSDisplayNode::SharedPtr displayNode = RSDisplayNode::Create(config);
    ASSERT_TRUE(displayNode != nullptr);
    auto isMirrorDisplay = displayNode->IsMirrorDisplay();
    EXPECT_EQ(isMirrorDisplay, false);

    /**
    * step2. set display mode
    */
    RSDisplayNodeConfig configNew = {screenId2, true, displayNodeSrc->GetId()};
    displayNode->SetDisplayNodeMirrorConfig(configNew);
    isMirrorDisplay = displayNode->IsMirrorDisplay();
    EXPECT_EQ(isMirrorDisplay, true);

    /**
    * step3. flush commands
    */
    RSTransaction::FlushImplicitTransaction();

    /**
    * step4. delete nodes
    */
    sleep(TEST_SLEEP_S);
    displayNodeSrc->RemoveFromTree();
    displayNode->RemoveFromTree();
}
} // namespace Rosen
} // namespace OHOS