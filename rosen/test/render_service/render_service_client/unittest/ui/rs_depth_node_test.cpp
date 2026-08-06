/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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
#include "ui/rs_depth_node.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSDepthNodeClientTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSDepthNodeClientTest::SetUpTestCase() {}
void RSDepthNodeClientTest::TearDownTestCase() {}
void RSDepthNodeClientTest::SetUp() {}
void RSDepthNodeClientTest::TearDown() {}

/**
 * @tc.name: Create001
 * @tc.desc: Verify RSDepthNode::Create returns a valid node
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSDepthNodeClientTest, Create001, TestSize.Level1)
{
    auto node = RSDepthNode::Create(true, false, nullptr);
    ASSERT_NE(node, nullptr);
}

/**
 * @tc.name: Create002
 * @tc.desc: Verify RSDepthNode::Create with isTextureExportNode = true
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSDepthNodeClientTest, Create002, TestSize.Level1)
{
    auto node = RSDepthNode::Create(true, true, nullptr);
    ASSERT_NE(node, nullptr);
    EXPECT_EQ(node->GetType(), RSUINodeType::DEPTH_NODE);
}

/**
 * @tc.name: GetType001
 * @tc.desc: Verify GetType returns DEPTH_NODE
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSDepthNodeClientTest, GetType001, TestSize.Level1)
{
    auto node = RSDepthNode::Create(true, false, nullptr);
    ASSERT_NE(node, nullptr);
    EXPECT_EQ(node->GetType(), RSUINodeType::DEPTH_NODE);
}

/**
 * @tc.name: GetType002
 * @tc.desc: Verify static Type constant
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSDepthNodeClientTest, GetType002, TestSize.Level1)
{
    EXPECT_EQ(RSDepthNode::Type, RSUINodeType::DEPTH_NODE);
}

/**
 * @tc.name: SetDepthSpaceType001
 * @tc.desc: Verify SetDepthSpaceType does not crash
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSDepthNodeClientTest, SetDepthSpaceType001, TestSize.Level1)
{
    auto node = RSDepthNode::Create(true, false, nullptr);
    ASSERT_NE(node, nullptr);
    EXPECT_NO_FATAL_FAILURE(node->SetDepthSpaceType(DepthSpaceType::INSTANCE));
}

/**
 * @tc.name: SetDepthSpaceType002
 * @tc.desc: Verify SetDepthSpaceType with GLOBAL type
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSDepthNodeClientTest, SetDepthSpaceType002, TestSize.Level1)
{
    auto node = RSDepthNode::Create(true, false, nullptr);
    ASSERT_NE(node, nullptr);
    EXPECT_NO_FATAL_FAILURE(node->SetDepthSpaceType(DepthSpaceType::GLOBAL));
}

/**
 * @tc.name: SetDepthImage001
 * @tc.desc: Verify SetDepthImage with nullptr does not crash
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSDepthNodeClientTest, SetDepthImage001, TestSize.Level1)
{
    auto node = RSDepthNode::Create(true, false, nullptr);
    ASSERT_NE(node, nullptr);
    EXPECT_NO_FATAL_FAILURE(node->SetDepthImage(nullptr));
}

/**
 * @tc.name: SetDepthCameraPara001
 * @tc.desc: Verify SetDepthCameraPara does not crash
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSDepthNodeClientTest, SetDepthCameraPara001, TestSize.Level1)
{
    auto node = RSDepthNode::Create(true, false, nullptr);
    ASSERT_NE(node, nullptr);
    DepthCameraPara para;
    para.position = {0.0f, 0.0f, 5.0f};
    para.quaternion = {0.0f, 0.0f, 0.0f, 1.0f};
    para.yFov = 60.0f;
    para.zNear = 0.1f;
    para.zFar = 100.0f;
    EXPECT_NO_FATAL_FAILURE(node->SetDepthCameraPara(para));
}

/**
 * @tc.name: SetDepthLightPara001
 * @tc.desc: Verify SetDepthLightPara does not crash
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSDepthNodeClientTest, SetDepthLightPara001, TestSize.Level1)
{
    auto node = RSDepthNode::Create(true, false, nullptr);
    ASSERT_NE(node, nullptr);
    DepthLightPara para;
    para.direction = {0.0f, -1.0f, 0.0f};
    para.color = {1.0f, 1.0f, 1.0f};
    para.intensity = 1.0f;
    EXPECT_NO_FATAL_FAILURE(node->SetDepthLightPara(para));
}

/**
 * @tc.name: SetDepthImageMatrix001
 * @tc.desc: Verify SetDepthImageMatrix
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSDepthNodeClientTest, SetDepthImageMatrix001, TestSize.Level1)
{
    auto node = RSDepthNode::Create(true, false, nullptr);
    ASSERT_NE(node, nullptr);

    Matrix3f matrix(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f);
    EXPECT_NO_FATAL_FAILURE(node->SetDepthImageMatrix(matrix));
}
} // namespace OHOS::Rosen
