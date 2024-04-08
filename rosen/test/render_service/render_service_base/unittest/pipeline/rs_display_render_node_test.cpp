/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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
#include "pipeline/rs_display_render_node.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSDisplayRenderNodeTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    static inline NodeId id;
    RSDisplayNodeConfig config;
    static inline std::weak_ptr<RSContext> context = {};
};

void RSDisplayRenderNodeTest::SetUpTestCase() {}
void RSDisplayRenderNodeTest::TearDownTestCase() {}
void RSDisplayRenderNodeTest::SetUp() {}
void RSDisplayRenderNodeTest::TearDown() {}

/**
 * @tc.name: PrepareTest
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSDisplayRenderNodeTest, PrepareTest, TestSize.Level1)
{
    auto node = std::make_shared<RSDisplayRenderNode>(id, config, context);
    std::shared_ptr<RSNodeVisitor> visitor = nullptr;
    node->Prepare(visitor);
}


/**
 * @tc.name: SkipFrameTest
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSDisplayRenderNodeTest, SkipFrameTest, TestSize.Level1)
{
    auto node = std::make_shared<RSDisplayRenderNode>(id, config, context);
    uint32_t skipFrameInterval = 0;
    node->SkipFrame(skipFrameInterval);
    ASSERT_FALSE(node->SkipFrame(skipFrameInterval));
}
/**
 * @tc.name: SetMirrorSourceTest
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSDisplayRenderNodeTest, SetMirrorSourceTest, TestSize.Level1)
{
    std::shared_ptr<RSDisplayRenderNode> rsDisplayRenderNode = nullptr;
    auto node = std::make_shared<RSDisplayRenderNode>(id, config, context);
    node->SetMirrorSource(rsDisplayRenderNode);
}

/**
 * @tc.name: CreateSurfaceTest
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSDisplayRenderNodeTest, CreateSurfaceTest, TestSize.Level1)
{
    sptr<IBufferConsumerListener> listener;
    auto node = std::make_shared<RSDisplayRenderNode>(id, config, context);
    node->CreateSurface(listener);
}

/**
 * @tc.name: GetRotationTest
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSDisplayRenderNodeTest, GetRotationTest, TestSize.Level1)
{
    auto node = std::make_shared<RSDisplayRenderNode>(id, config, context);
    node->UpdateRotation();
    node->GetRotation();
}

/**
 * @tc.name: IsRotationChangedTest
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSDisplayRenderNodeTest, IsRotationChangedTest, TestSize.Level1)
{
    auto node = std::make_shared<RSDisplayRenderNode>(id, config, context);
    node->UpdateRotation();
    ASSERT_FALSE(node->IsRotationChanged());
}

/**
 * @tc.name: SetBootAnimationTest
 * @tc.desc: SetBootAnimation
 * @tc.type:FUNC
 * @tc.require:SR000HSUII
 */
HWTEST_F(RSDisplayRenderNodeTest, SetBootAnimationTest, TestSize.Level1)
{
    std::shared_ptr<RSRenderNode> node = std::make_shared<RSRenderNode>(id, context);
    auto childNode = std::make_shared<RSDisplayRenderNode>(id + 1, config, context);
    node->AddChild(childNode);
    childNode->SetBootAnimation(true);
    ASSERT_EQ(childNode->GetBootAnimation(), true);
    node->SetBootAnimation(false);
    ASSERT_FALSE(node->GetBootAnimation());
}

/**
 * @tc.name: GetBootAnimationTest
 * @tc.desc: GetBootAnimation
 * @tc.type:FUNC
 * @tc.require:SR000HSUII
 */
HWTEST_F(RSDisplayRenderNodeTest, GetBootAnimationTest, TestSize.Level1)
{
    auto node = std::make_shared<RSDisplayRenderNode>(id, config, context);
    node->SetBootAnimation(true);
    ASSERT_TRUE(node->GetBootAnimation());
    node->SetBootAnimation(false);
    ASSERT_FALSE(node->GetBootAnimation());
}

/**
 * @tc.name: SetRootIdOfCaptureWindow
 * @tc.desc: Test SetRootIdOfCaptureWindow
 * @tc.type:FUNC
 * @tc.require:issueI981R9
 */
HWTEST_F(RSDisplayRenderNodeTest, SetRootIdOfCaptureWindow, TestSize.Level2)
{
    auto childNode = std::make_shared<RSRenderNode>(id, context);
    auto displayNode = std::make_shared<RSDisplayRenderNode>(id + 1, config, context);
    ASSERT_NE(childNode, nullptr);
    ASSERT_NE(displayNode, nullptr);

    displayNode->SetRootIdOfCaptureWindow(childNode->GetId());
    ASSERT_EQ(displayNode->GetRootIdOfCaptureWindow(), childNode->GetId());
}
} // namespace OHOS::Rosen