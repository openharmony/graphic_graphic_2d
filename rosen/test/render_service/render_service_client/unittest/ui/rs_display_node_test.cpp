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
#include "transaction/rs_interfaces.h"
#include "ui/rs_display_node.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSDisplayNodeTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSDisplayNodeTest::SetUpTestCase() {}
void RSDisplayNodeTest::TearDownTestCase() {}
void RSDisplayNodeTest::SetUp() {}
void RSDisplayNodeTest::TearDown() {}

/**
 * @tc.name: Create001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSDisplayNodeTest, Create001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create RSDisplayNode
     */
    RSDisplayNodeConfig c;
    RSDisplayNode::SharedPtr displayNode = RSDisplayNode::Create(c);
    ASSERT_TRUE(displayNode != nullptr);
}

/**
 * @tc.name: AddDisplayNodeToTree001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSDisplayNodeTest, AddDisplayNodeToTree001, TestSize.Level1)
{
    /**
    * @tc.steps: step1. create RSDisplayNode
    */
    RSDisplayNodeConfig c;
    RSDisplayNode::SharedPtr displayNode = RSDisplayNode::Create(c);
    ASSERT_TRUE(displayNode != nullptr);
    displayNode->AddDisplayNodeToTree();
}

/**
 * @tc.name: RemoveDisplayNodeFromTree001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSDisplayNodeTest, RemoveDisplayNodeFromTree001, TestSize.Level1)
{
    /**
    * @tc.steps: step1. create RSDisplayNode
    */
    RSDisplayNodeConfig c;
    RSDisplayNode::SharedPtr displayNode = RSDisplayNode::Create(c);
    ASSERT_TRUE(displayNode != nullptr);
    displayNode->RemoveDisplayNodeFromTree();
}

/**
 * @tc.name: GetType001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSDisplayNodeTest, GetType001, TestSize.Level1)
{
    RSDisplayNodeConfig c;
    RSDisplayNode::SharedPtr displayNode = RSDisplayNode::Create(c);
    ASSERT_TRUE(displayNode != nullptr);
    ASSERT_TRUE(displayNode->GetType() == RSUINodeType::DISPLAY_NODE);
}

/**
 * @tc.name: SetScreenIdTest001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSDisplayNodeTest, SetScreenIdTest001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create RSDisplayNode
     */
    RSDisplayNodeConfig config;
    RSDisplayNode::SharedPtr displayNode = RSDisplayNode::Create(config);
    ASSERT_TRUE(displayNode != nullptr);
    displayNode->SetScreenId(1);

    delete RSTransactionProxy::instance_;
    RSTransactionProxy::instance_ = nullptr;
    displayNode->SetScreenId(1);
    ASSERT_TRUE(RSTransactionProxy::instance_ == nullptr);
    RSTransactionProxy::instance_ = new RSTransactionProxy();
}

/**
 * @tc.name: SetSecurityDisplayTest001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSDisplayNodeTest, SetSecurityDisplayTest001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create RSDisplayNode
     */
    RSDisplayNodeConfig config;
    RSDisplayNode::SharedPtr displayNode = RSDisplayNode::Create(config);
    ASSERT_TRUE(displayNode != nullptr);
    /**
     * @tc.steps: step2. set SecurityDisplay
     */
    displayNode->SetSecurityDisplay(true);
    EXPECT_TRUE(displayNode->GetSecurityDisplay());
}

/**
 * @tc.name: SetSecurityDisplayTest002
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSDisplayNodeTest, SetSecurityDisplayTest002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create RSDisplayNode
     */
    RSDisplayNodeConfig config;
    RSDisplayNode::SharedPtr displayNode = RSDisplayNode::Create(config);
    /**
     * @tc.steps: step2. set SecurityDisplay
     */
    ASSERT_TRUE(displayNode != nullptr);
    displayNode->SetSecurityDisplay(false);
    EXPECT_FALSE(displayNode->GetSecurityDisplay());
}

/**
 * @tc.name: GetSecurityDisplayTest001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSDisplayNodeTest, GetSecurityDisplayTest001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create RSDisplayNode
     */
    RSDisplayNodeConfig config;
    RSDisplayNode::SharedPtr displayNode = RSDisplayNode::Create(config);
    ASSERT_TRUE(displayNode != nullptr);
    EXPECT_FALSE(displayNode->GetSecurityDisplay());
}

/**
 * @tc.name: SetScreenRotationTest001
 * @tc.desc: SetScreenRotation Test
 * @tc.type: FUNC
 * @tc.require: issueI9N1QF
 */
HWTEST_F(RSDisplayNodeTest, SetScreenRotationTest001, TestSize.Level1)
{
    RSDisplayNodeConfig config;
    RSDisplayNode::SharedPtr displayNode = RSDisplayNode::Create(config);
    ASSERT_NE(displayNode, nullptr);
    displayNode->SetScreenRotation(0);
    displayNode->SetScreenRotation(1);
    displayNode->SetScreenRotation(2);
    displayNode->SetScreenRotation(3);
    displayNode->SetScreenRotation(4);
}

/**
 * @tc.name: SetDisplayNodeConfigTest001
 * @tc.desc: test results of SetDisplayNodeConfig
 * @tc.type: FUNC
 * @tc.require: issueI9KDPI
 */
HWTEST_F(RSDisplayNodeTest, SetDisplayNodeConfigTest001, TestSize.Level1)
{
    RSDisplayNodeConfig config;
    RSDisplayNode::SharedPtr displayNode = RSDisplayNode::Create(config);
    displayNode->SetDisplayNodeConfig(config);
    EXPECT_NE(RSTransactionProxy::instance_, nullptr);

    delete RSTransactionProxy::instance_;
    RSTransactionProxy::instance_ = nullptr;
    displayNode->SetDisplayNodeConfig(config);
    ASSERT_EQ(RSTransactionProxy::instance_, nullptr);
    RSTransactionProxy::instance_ = new RSTransactionProxy();
}

/**
 * @tc.name: IsMirrorDisplayTest001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSDisplayNodeTest, IsMirrorDisplayTest001, TestSize.Level1)
{
    RSDisplayNodeConfig config;
    RSDisplayNode::SharedPtr displayNode = RSDisplayNode::Create(config);
    ASSERT_FALSE(displayNode->IsMirrorDisplay());
}

/**
 * @tc.name: SetVirtualScreenMuteStatusTest001
 * @tc.desc: test SetVirtualScreenMuteStatus
 * @tc.type: FUNC
 * @tc.require: issueIBTNC3
 */
HWTEST_F(RSDisplayNodeTest, SetVirtualScreenMuteStatusTest001, TestSize.Level1)
{
    RSDisplayNodeConfig config;
    config.screenId = 6000;
    RSDisplayNode::SharedPtr displayNode = RSDisplayNode::Create(config);
    EXPECT_TRUE(displayNode != nullptr);

    displayNode->SetVirtualScreenMuteStatus(true);
    auto transactionProxy = RSTransactionProxy::GetInstance();
    if (transactionProxy != nullptr) {
        transactionProxy->FlushImplicitTransaction();
    }

    displayNode->SetVirtualScreenMuteStatus(false);
    if (transactionProxy != nullptr) {
        transactionProxy->FlushImplicitTransaction();
    }
}

/**
 * @tc.name: ClearChildrenTest
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSDisplayNodeTest, ClearChildrenTest, TestSize.Level1)
{
    RSDisplayNodeConfig config;
    RSDisplayNode::SharedPtr displayNode = RSDisplayNode::Create(config);
    displayNode->ClearChildren();

    std::weak_ptr<RSNode> child = std::make_shared<RSNode>(0);
    displayNode->children_.push_back(child);
    std::weak_ptr<RSNode> child1 = std::make_shared<RSNode>(1);
    displayNode->children_.push_back(child1);
    std::weak_ptr<RSNode> child2 = std::make_shared<RSNode>(2);
    displayNode->children_.push_back(child2);
    displayNode->ClearChildren();
    EXPECT_TRUE(!displayNode->children_.empty());
}

/**
 * @tc.name: SetBootAnimation Test
 * @tc.desc: SetBootAnimation and GetBootAnimation
 * @tc.type: FUNC
 * @tc.require:SR000HSUII
 */
HWTEST_F(RSDisplayNodeTest, SetBootAnimationTest, TestSize.Level1)
{
    RSDisplayNodeConfig c;
    RSDisplayNode::SharedPtr displayNode = RSDisplayNode::Create(c);
    displayNode->SetBootAnimation(true);
    ASSERT_EQ(true, displayNode->GetBootAnimation());
    displayNode->SetBootAnimation(false);
    ASSERT_EQ(false, displayNode->GetBootAnimation());

    delete RSTransactionProxy::instance_;
    RSTransactionProxy::instance_ = nullptr;
    displayNode->SetBootAnimation(false);
    ASSERT_TRUE(RSTransactionProxy::instance_ == nullptr);
    RSTransactionProxy::instance_ = new RSTransactionProxy();
}

/**
 * @tc.name: Marshalling
 * @tc.desc: test results of Marshalling
 * @tc.type: FUNC
 * @tc.require: issueI9KDPI
 */
HWTEST_F(RSDisplayNodeTest, Marshalling, TestSize.Level1)
{
    RSDisplayNodeConfig c;
    RSDisplayNode::SharedPtr displayNode = RSDisplayNode::Create(c);
    Parcel parcel;
    bool res = displayNode->Marshalling(parcel);
    EXPECT_EQ(res, true);
}

/**
 * @tc.name: Unmarshalling
 * @tc.desc: test results of Unmarshalling
 * @tc.type: FUNC
 * @tc.require: issueI9KDPI
 */
HWTEST_F(RSDisplayNodeTest, Unmarshalling, TestSize.Level1)
{
    RSDisplayNodeConfig c;
    RSDisplayNode::SharedPtr displayNode = RSDisplayNode::Create(c);
    Parcel parcel;
    auto res = displayNode->Unmarshalling(parcel);
    EXPECT_EQ(res, nullptr);
}

/**
 * @tc.name: OnBoundsSizeChanged
 * @tc.desc: test results of OnBoundsSizeChanged
 * @tc.type: FUNC
 * @tc.require: issueI9KDPI
 */
HWTEST_F(RSDisplayNodeTest, OnBoundsSizeChanged, TestSize.Level1)
{
    RSDisplayNodeConfig c;
    RSDisplayNode::SharedPtr displayNode = RSDisplayNode::Create(c);
    displayNode->OnBoundsSizeChanged();
    EXPECT_NE(RSTransactionProxy::instance_, nullptr);
}

/**
 * @tc.name: UnmarshallingTest001
 * @tc.desc: Unmarshalling Test
 * @tc.type: FUNC
 * @tc.require: issueI9N1QF
 */
HWTEST_F(RSDisplayNodeTest, UnmarshallingTest001, TestSize.Level1)
{
    RSDisplayNodeConfig config;
    RSDisplayNode::SharedPtr displayNode = RSDisplayNode::Create(config);

    Parcel parcel;
    auto displayNodeTest1 = displayNode->Unmarshalling(parcel);
    EXPECT_EQ(displayNodeTest1, nullptr);

    uint32_t id = 100;
    uint32_t screenId = 0;
    bool isMirrored = true;
    parcel.WriteUint64(id);
    parcel.WriteUint64(screenId);
    parcel.WriteBool(isMirrored);
    auto displayNodeTest2 = displayNode->Unmarshalling(parcel);
    EXPECT_TRUE(displayNodeTest2 != nullptr);
    EXPECT_EQ(displayNodeTest2->GetId(), id);
    EXPECT_EQ(displayNodeTest2->IsMirrorDisplay(), isMirrored);
}

/**
 * @tc.name: ServiceControlBlockTree001
 * @tc.desc: AddDisplayNodeToTree RemoveDisplayNodeFromTree SetScbNodePid Test
 * @tc.type: FUNC
 * @tc.require: issueI9TI4Y
 */
HWTEST_F(RSDisplayNodeTest, ServiceControlBlockTree001, TestSize.Level1)
{
    RSDisplayNodeConfig config;
    RSDisplayNode::SharedPtr displayNode = RSDisplayNode::Create(config);
    EXPECT_NE(displayNode, nullptr);

    // AddDisplayNodeToTree test
    displayNode->AddDisplayNodeToTree();
    // RemoveDisplayNodeFromTree test
    displayNode->RemoveDisplayNodeFromTree();
}

/**
 * @tc.name: SetDisplayContentRect001
 * @tc.desc: Test SetDisplayContentRect with normal rect values
 * @tc.type: FUNC
 */
HWTEST_F(RSDisplayNodeTest, SetDisplayContentRect001, TestSize.Level1)
{
    RSDisplayNodeConfig config;
    RSDisplayNode::SharedPtr displayNode = RSDisplayNode::Create(config);
    ASSERT_TRUE(displayNode != nullptr);

    Rect contentRect{0, 0, 1920, 1080};
    displayNode->SetDisplayContentRect(contentRect);
    EXPECT_NE(RSTransactionProxy::instance_, nullptr);
}

/**
 * @tc.name: SetDisplayContentRect002
 * @tc.desc: Test SetDisplayContentRect with different rect values
 * @tc.type: FUNC
 */
HWTEST_F(RSDisplayNodeTest, SetDisplayContentRect002, TestSize.Level1)
{
    RSDisplayNodeConfig config;
    RSDisplayNode::SharedPtr displayNode = RSDisplayNode::Create(config);
    ASSERT_TRUE(displayNode != nullptr);

    Rect contentRect{100, 50, 1280, 720};
    displayNode->SetDisplayContentRect(contentRect);
    EXPECT_NE(RSTransactionProxy::instance_, nullptr);

    Rect contentRect2{0, 0, 2560, 1440};
    displayNode->SetDisplayContentRect(contentRect2);
    EXPECT_NE(RSTransactionProxy::instance_, nullptr);
}

/**
 * @tc.name: SetDisplayContentRect003
 * @tc.desc: Test SetDisplayContentRect with null transaction proxy
 * @tc.type: FUNC
 */
HWTEST_F(RSDisplayNodeTest, SetDisplayContentRect003, TestSize.Level1)
{
    RSDisplayNodeConfig config;
    RSDisplayNode::SharedPtr displayNode = RSDisplayNode::Create(config);
    ASSERT_TRUE(displayNode != nullptr);

    delete RSTransactionProxy::instance_;
    RSTransactionProxy::instance_ = nullptr;

    Rect contentRect{0, 0, 1920, 1080};
    displayNode->SetDisplayContentRect(contentRect);
    ASSERT_TRUE(RSTransactionProxy::instance_ == nullptr);
    RSTransactionProxy::instance_ = new RSTransactionProxy();
}

#ifdef RS_ENABLE_UNI_RENDER
/**
 * @tc.name: ClearModifierByPid001
 * @tc.desc: Test ClearModifierByPid with valid pid
 * @tc.type: FUNC
 */
HWTEST_F(RSDisplayNodeTest, ClearModifierByPid001, TestSize.Level1)
{
    RSDisplayNodeConfig config;
    RSDisplayNode::SharedPtr displayNode = RSDisplayNode::Create(config);
    ASSERT_TRUE(displayNode != nullptr);

    pid_t testPid = getprocpid();
    displayNode->ClearModifierByPid(testPid);

    auto transactionProxy = RSTransactionProxy::GetInstance();
    if (transactionProxy != nullptr) {
        transactionProxy->FlushImplicitTransaction();
    }
}

/**
 * @tc.name: ClearModifierByPid002
 * @tc.desc: Test ClearModifierByPid with zero pid
 * @tc.type: FUNC
 */
HWTEST_F(RSDisplayNodeTest, ClearModifierByPid002, TestSize.Level1)
{
    RSDisplayNodeConfig config;
    RSDisplayNode::SharedPtr displayNode = RSDisplayNode::Create(config);
    ASSERT_TRUE(displayNode != nullptr);

    displayNode->ClearModifierByPid(0);

    auto transactionProxy = RSTransactionProxy::GetInstance();
    if (transactionProxy != nullptr) {
        transactionProxy->FlushImplicitTransaction();
    }
}

/**
 * @tc.name: ClearModifierByPid003
 * @tc.desc: Test ClearModifierByPid with negative pid
 * @tc.type: FUNC
 */
HWTEST_F(RSDisplayNodeTest, ClearModifierByPid003, TestSize.Level1)
{
    RSDisplayNodeConfig config;
    RSDisplayNode::SharedPtr displayNode = RSDisplayNode::Create(config);
    ASSERT_TRUE(displayNode != nullptr);

    displayNode->ClearModifierByPid(-1);

    auto transactionProxy = RSTransactionProxy::GetInstance();
    if (transactionProxy != nullptr) {
        transactionProxy->FlushImplicitTransaction();
    }
}
#endif
} // namespace OHOS::Rosen