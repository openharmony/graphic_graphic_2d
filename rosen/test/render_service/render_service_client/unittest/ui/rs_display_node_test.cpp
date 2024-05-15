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

class TestSurfaceCapture : public SurfaceCaptureCallback {
public:
    TestSurfaceCapture()
    {
        showNode_ = nullptr;
    }
    explicit TestSurfaceCapture(std::shared_ptr<RSSurfaceNode> surfaceNode)
    {
        showNode_ = surfaceNode;
    }
    ~TestSurfaceCapture() override {}
    void OnSurfaceCapture(std::shared_ptr<Media::PixelMap> pixelmap) override {}
    bool IsTestSuccess()
    {
        return testSuccess;
    }
private:
    bool testSuccess = true;
    std::shared_ptr<RSSurfaceNode> showNode_;
}; // class TestSurfaceCapture

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
 * @tc.name: TakeSurfaceCapture001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSDisplayNodeTest, TakeSurfaceCapture001, TestSize.Level1)
{
    /**
    * @tc.steps: step1. create RSDisplayNode
    */
    RSDisplayNodeConfig c;
    RSDisplayNode::SharedPtr displayNode = RSDisplayNode::Create(c);
    ASSERT_TRUE(displayNode != nullptr);
    auto surfaceCaptureMock = std::make_shared<TestSurfaceCapture>();
    RSInterfaces::GetInstance().TakeSurfaceCapture(displayNode, surfaceCaptureMock);
}

/**
 * @tc.name: SetSecurityDisplay001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSDisplayNodeTest, SetSecurityDisplay001, TestSize.Level1)
{
    /**
    * @tc.steps: step1. create RSDisplayNode
    */
    RSDisplayNodeConfig c;
    RSDisplayNode::SharedPtr displayNode = RSDisplayNode::Create(c);
    ASSERT_TRUE(displayNode != nullptr);
    /**
    * @tc.steps: step2. set SecurityDisplay
    */
    displayNode->SetSecurityDisplay(true);
    EXPECT_TRUE(displayNode->GetSecurityDisplay());
}

/**
 * @tc.name: SetSecurityDisplay002
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSDisplayNodeTest, SetSecurityDisplay002, TestSize.Level1)
{
    /**
    * @tc.steps: step1. create RSDisplayNode
    */
    RSDisplayNodeConfig c;
    RSDisplayNode::SharedPtr displayNode = RSDisplayNode::Create(c);
    /**
    * @tc.steps: step2. set SecurityDisplay
    */
    ASSERT_TRUE(displayNode != nullptr);
    displayNode->SetSecurityDisplay(false);
    EXPECT_FALSE(displayNode->GetSecurityDisplay());
}


/**
 * @tc.name: GetSecurityDisplay001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSDisplayNodeTest, GetSecurityDisplay001, TestSize.Level1)
{
    /**
    * @tc.steps: step1. create RSDisplayNode
    */
    RSDisplayNodeConfig c;
    RSDisplayNode::SharedPtr displayNode = RSDisplayNode::Create(c);
    ASSERT_TRUE(displayNode != nullptr);
    EXPECT_FALSE(displayNode->GetSecurityDisplay());
}

/**
 * @tc.name: SetScreenId001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSDisplayNodeTest, SetScreenId001, TestSize.Level1)
{
    /**
    * @tc.steps: step1. create RSDisplayNode
    */
    RSDisplayNodeConfig c;
    RSDisplayNode::SharedPtr displayNode = RSDisplayNode::Create(c);
    ASSERT_TRUE(displayNode != nullptr);
    displayNode->SetScreenId(1);
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
}

/**
 * @tc.name: IsMirrorDisplayTest
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSDisplayNodeTest, IsMirrorDisplayTest, TestSize.Level1)
{
    RSDisplayNodeConfig config;
    RSDisplayNode::SharedPtr displayNode = RSDisplayNode::Create(config);
    ASSERT_FALSE(displayNode->IsMirrorDisplay());
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
 * @tc.name: SetDisplayOffset
 * @tc.desc: test results of SetDisplayOffset
 * @tc.type: FUNC
 * @tc.require: issueI9KDPI
 */
HWTEST_F(RSDisplayNodeTest, SetDisplayOffset, TestSize.Level1)
{
    RSDisplayNodeConfig c;
    RSDisplayNode::SharedPtr displayNode = RSDisplayNode::Create(c);
    displayNode->SetDisplayOffset(0, 1);
    EXPECT_NE(RSTransactionProxy::instance_, nullptr);
}

/**
 * @tc.name: SetDisplayNodeMirrorConfig
 * @tc.desc: test results of SetDisplayNodeMirrorConfig
 * @tc.type: FUNC
 * @tc.require: issueI9KDPI
 */
HWTEST_F(RSDisplayNodeTest, SetDisplayNodeMirrorConfig, TestSize.Level1)
{
    RSDisplayNodeConfig displayNodeConfig;
    RSDisplayNode::SharedPtr displayNode = RSDisplayNode::Create(displayNodeConfig);
    displayNode->SetDisplayNodeMirrorConfig(displayNodeConfig);
    EXPECT_NE(RSTransactionProxy::instance_, nullptr);
}

/**
 * @tc.name: SetScreenRotation
 * @tc.desc: test results of SetScreenRotation
 * @tc.type: FUNC
 * @tc.require: issueI9KDPI
 */
HWTEST_F(RSDisplayNodeTest, SetScreenRotation, TestSize.Level1)
{
    RSDisplayNodeConfig displayNodeConfig;
    RSDisplayNode::SharedPtr displayNode = RSDisplayNode::Create(displayNodeConfig);
    uint32_t rotation = 0;
    displayNode->SetScreenRotation(rotation);

    rotation = 1;
    displayNode->SetScreenRotation(rotation);

    rotation = 2;
    displayNode->SetScreenRotation(rotation);

    rotation = 3;
    displayNode->SetScreenRotation(rotation);

    rotation = 4;
    displayNode->SetScreenRotation(rotation);
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
 * @tc.name: SetScreenRotationTest003
 * @tc.desc: SetScreenRotation Test
 * @tc.type: FUNC
 * @tc.require: issueI9N1QF
 */
HWTEST_F(RSDisplayNodeTest, SetScreenRotationTest003, TestSize.Level1)
{
    RSDisplayNodeConfig config;
    RSDisplayNode::SharedPtr displayNode = RSDisplayNode::Create(config);
    EXPECT_TRUE(displayNode != nullptr);
    displayNode->SetId(0);
    EXPECT_EQ(displayNode->GetId(), 0);
    displayNode->SetScreenRotation(0);
    displayNode->SetScreenRotation(1);
    displayNode->SetScreenRotation(2);
    displayNode->SetScreenRotation(3);
    displayNode->SetScreenRotation(4);
    EXPECT_NE(RSTransactionProxy::GetInstance(), nullptr);
}
} // namespace OHOS::Rosen