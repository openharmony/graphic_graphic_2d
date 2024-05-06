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
#include "ui/rs_root_node.h"
#include "ui/rs_surface_node.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSRootNodeTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSRootNodeTest::SetUpTestCase() {}
void RSRootNodeTest::TearDownTestCase() {}
void RSRootNodeTest::SetUp() {}
void RSRootNodeTest::TearDown() {}

/**
 * @tc.name: CreateAndDestroy001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSRootNodeTest, CreateAndDestroy001, TestSize.Level1)
{
    RSRootNode::Create();
}

/**
 * @tc.name: CreateAndDestroy002
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSRootNodeTest, CreateAndDestroy002, TestSize.Level1)
{
    RSRootNode::Create(true);
}

/**
 * @tc.name: CreateAndDestroy003
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSRootNodeTest, CreateAndDestroy003, TestSize.Level1)
{
    RSRootNode::Create(false);
}

/**
 * @tc.name: GetType001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSRootNodeTest, GetType001, TestSize.Level1)
{
    EXPECT_EQ(RSUINodeType::ROOT_NODE, RSRootNode::Create()->GetType());
}

/**
 * @tc.name: GetType002
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSRootNodeTest, GetType002, TestSize.Level1)
{
    EXPECT_EQ(RSUINodeType::ROOT_NODE, RSRootNode::Create(true)->GetType());
}

/**
 * @tc.name: GetType003
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSRootNodeTest, GetType003, TestSize.Level1)
{
    EXPECT_EQ(RSUINodeType::ROOT_NODE, RSRootNode::Create(false)->GetType());
}

/**
 * @tc.name: OnBoundsSizeChanged001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSRootNodeTest, OnBoundsSizeChanged001, TestSize.Level1)
{
    RSRootNode::Create()->OnBoundsSizeChanged();
    bool res = true;
    EXPECT_EQ(res, true);
}

/**
 * @tc.name: OnBoundsSizeChanged002
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSRootNodeTest, OnBoundsSizeChanged002, TestSize.Level1)
{
    RSRootNode::Create(true)->OnBoundsSizeChanged();
    bool res = true;
    EXPECT_EQ(res, true);
}

/**
 * @tc.name: OnBoundsSizeChanged003
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSRootNodeTest, OnBoundsSizeChanged003, TestSize.Level1)
{
    RSRootNode::Create(false)->OnBoundsSizeChanged();
    bool res = true;
    EXPECT_EQ(res, true);
}

/**
 * @tc.name: SetEnableRender001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSRootNodeTest, SetEnableRender001, TestSize.Level1)
{
    bool flag = true;
    auto node = std::make_shared<RSRootNode>(false);
    node->SetEnableRender(flag);
    bool res = true;
    EXPECT_EQ(res, true);
}

/**
 * @tc.name: SetEnableRender002
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSRootNodeTest, SetEnableRender002, TestSize.Level1)
{
    bool flag = true;
    auto node = std::make_shared<RSRootNode>(true);
    node->SetEnableRender(flag);
    bool res = true;
    EXPECT_EQ(res, true);
}

/**
 * @tc.name: AttachRSSurfaceNode001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSRootNodeTest, AttachRSSurfaceNode001, TestSize.Level1)
{
    Rosen::RSSurfaceNodeConfig config;
    config.SurfaceNodeName = "WindowScene_";
    std::shared_ptr<RSSurfaceNode> surfaceNode = std::make_shared<RSSurfaceNode>(config, true);
    auto node = std::make_shared<RSRootNode>(false);
    node->AttachRSSurfaceNode(surfaceNode);
    bool res = true;
    EXPECT_EQ(res, true);
}

/**
 * @tc.name: AttachRSSurfaceNode002
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSRootNodeTest, AttachRSSurfaceNode002, TestSize.Level1)
{
    Rosen::RSSurfaceNodeConfig config;
    config.SurfaceNodeName = "WindowScene_";
    std::shared_ptr<RSSurfaceNode> surfaceNode = std::make_shared<RSSurfaceNode>(config, true);
    auto node = std::make_shared<RSRootNode>(true);
    node->AttachRSSurfaceNode(surfaceNode);
    bool res = true;
    EXPECT_EQ(res, true);
}
} // namespace OHOS::Rosen