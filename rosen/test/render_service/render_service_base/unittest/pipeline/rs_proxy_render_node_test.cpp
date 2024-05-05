/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, Hardware
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <gtest/gtest.h>

#include "pipeline/rs_proxy_render_node.h"
#include "pipeline/rs_render_thread_visitor.h"
#include "pipeline/rs_surface_render_node.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class RSProxyRenderNodeTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSProxyRenderNodeTest::SetUpTestCase() {}
void RSProxyRenderNodeTest::TearDownTestCase() {}
void RSProxyRenderNodeTest::SetUp() {}
void RSProxyRenderNodeTest::TearDown() {}

/**
 * @tc.name: LifeCycel001
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require: issueI9H4AD
 */
HWTEST_F(RSProxyRenderNodeTest, LifeCycel001, TestSize.Level1)
{
    NodeId id = 10;
    NodeId targetId = 11;
    RSSurfaceRenderNodeConfig config;
    auto target = std::make_shared<RSSurfaceRenderNode>(config);
    auto node = std::make_shared<RSProxyRenderNode>(id, target, targetId);
    std::shared_ptr<RSNodeVisitor> visitor = std::make_shared<RSRenderThreadVisitor>();
    node->Prepare(visitor);
    node->Process(visitor);
    auto matrix = Drawing::Matrix();
    auto clipRegion = Drawing::Rect();
    node->SetContextMatrix(matrix);
    node->SetContextClipRegion(clipRegion);
    node->ResetContextVariableCache();
    node->SetContextMatrix(matrix);
    node->SetContextAlpha(0.0f);
    clipRegion = Drawing::Rect(0.f, 0.f, 10.f, 10.f);
    node->SetContextClipRegion(clipRegion);
}

/**
 * @tc.name: Process
 * @tc.desc: test results of Process
 * @tc.type:FUNC
 * @tc.require: issueI9H4AD
 */
HWTEST_F(RSProxyRenderNodeTest, Process, TestSize.Level1)
{
    NodeId id = 10;
    NodeId targetId = 11;
    RSSurfaceRenderNodeConfig config;
    auto target = std::make_shared<RSSurfaceRenderNode>(config);
    auto node = std::make_shared<RSProxyRenderNode>(id, target, targetId);
    std::shared_ptr<RSNodeVisitor> visitor;
    node->Process(visitor);
    ASSERT_TRUE(true);
}

/**
 * @tc.name: Prepare
 * @tc.desc: test results of Prepare
 * @tc.type:FUNC
 * @tc.require: issueI9H4AD
 */
HWTEST_F(RSProxyRenderNodeTest, Prepare, TestSize.Level1)
{
    NodeId id = 10;
    NodeId targetId = 11;
    RSSurfaceRenderNodeConfig config;
    auto target = std::make_shared<RSSurfaceRenderNode>(config);
    auto node = std::make_shared<RSProxyRenderNode>(id, target, targetId);
    std::shared_ptr<RSNodeVisitor> visitor;
    node->Prepare(visitor);
    ASSERT_TRUE(true);
}

/**
 * @tc.name: SetContextMatrix
 * @tc.desc: test results of SetContextMatrix
 * @tc.type:FUNC
 * @tc.require: issueI9H4AD
 */
HWTEST_F(RSProxyRenderNodeTest, SetContextMatrix, TestSize.Level1)
{
    NodeId id = 10;
    NodeId targetId = 11;
    std::weak_ptr<RSSurfaceRenderNode> target;
    auto node = std::make_shared<RSProxyRenderNode>(id, target, targetId);
    node->SetContextMatrix(node->contextMatrix_);
    auto matrix = Drawing::Matrix();
    node->SetContextMatrix(matrix);
    ASSERT_EQ(node->contextMatrix_, matrix);
}

/**
 * @tc.name: SetContextAlpha
 * @tc.desc: test results of clipRegion
 * @tc.type:FUNC
 * @tc.require: issueI9H4AD
 */
HWTEST_F(RSProxyRenderNodeTest, SetContextAlpha, TestSize.Level1)
{
    NodeId id = 10;
    NodeId targetId = 11;
    RSSurfaceRenderNodeConfig config;
    auto target = std::make_shared<RSSurfaceRenderNode>(config);
    auto node = std::make_shared<RSProxyRenderNode>(id, target, targetId);

    node->SetContextAlpha(1.0f);
    node->target_.reset();
    node->SetContextAlpha(1.0f);
    ASSERT_EQ(node->contextAlpha_, 1.0f);
}

/**
 * @tc.name: SetContextClipRegion
 * @tc.desc: test results of SetContextClipRegion
 * @tc.type:FUNC
 * @tc.require: issueI9H4AD
 */
HWTEST_F(RSProxyRenderNodeTest, SetContextClipRegion, TestSize.Level1)
{
    NodeId id = 10;
    NodeId targetId = 11;
    RSSurfaceRenderNodeConfig config;
    auto target = std::make_shared<RSSurfaceRenderNode>(config);
    auto node = std::make_shared<RSProxyRenderNode>(id, target, targetId);
    auto clipRegion = Drawing::Rect();
    node->SetContextClipRegion(node->contextClipRect_);
    node->target_.reset();
    node->SetContextClipRegion(clipRegion);
    ASSERT_EQ(node->contextClipRect_, clipRegion);
}

/**
 * @tc.name: OnTreeStateChanged
 * @tc.desc: test results of OnTreeStateChanged
 * @tc.type:FUNC
 * @tc.require: issueI9H4AD
 */
HWTEST_F(RSProxyRenderNodeTest, OnTreeStateChanged, TestSize.Level1)
{
    NodeId id = 10;
    NodeId targetId = 11;
    RSSurfaceRenderNodeConfig config;
    auto target = std::make_shared<RSSurfaceRenderNode>(config);
    auto node = std::make_shared<RSProxyRenderNode>(id, target, targetId);
    auto clipRegion = Drawing::Rect();
    node->OnTreeStateChanged();
    node->isOnTheTree_ = true;
    node->OnTreeStateChanged();
    ASSERT_EQ(node->contextAlpha_, -1.0f);
}

/**
 * @tc.name: CleanUp
 * @tc.desc: test results of CleanUp
 * @tc.type:FUNC
 * @tc.require: issueI9H4AD
 */
HWTEST_F(RSProxyRenderNodeTest, CleanUp, TestSize.Level1)
{
    NodeId id = 10;
    NodeId targetId = 11;
    RSSurfaceRenderNodeConfig config;
    auto target = std::make_shared<RSSurfaceRenderNode>(config);
    auto node = std::make_shared<RSProxyRenderNode>(id, target, targetId);
    bool removeModifiers = true;
    node->CleanUp(removeModifiers);
    removeModifiers = false;
    node->CleanUp(removeModifiers);
    ASSERT_EQ(node->target_.lock()->contextAlpha_, 1.0f);
}
} // namespace Rosen
} // namespace OHOS