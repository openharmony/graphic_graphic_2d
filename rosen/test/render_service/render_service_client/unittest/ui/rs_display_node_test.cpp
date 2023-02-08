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
} // namespace OHOS::Rosen