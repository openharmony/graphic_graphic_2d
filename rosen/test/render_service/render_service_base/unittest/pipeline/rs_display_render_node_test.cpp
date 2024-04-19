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

#include "common/rs_obj_abs_geometry.h"
#include "pipeline/rs_display_render_node.h"
#include "pipeline/rs_render_thread_visitor.h"

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
 * @tc.desc: test results of Prepare
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSDisplayRenderNodeTest, PrepareTest, TestSize.Level1)
{
    auto node = std::make_shared<RSDisplayRenderNode>(id, config, context);
    std::shared_ptr<RSNodeVisitor> visitor = nullptr;
    node->Prepare(visitor);

    visitor = std::make_shared<RSRenderThreadVisitor>();
    node->Prepare(visitor);
    ASSERT_TRUE(true);
}

/**
 * @tc.name: SkipFrameTest
 * @tc.desc: test results of SkipFrame
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSDisplayRenderNodeTest, SkipFrameTest, TestSize.Level1)
{
    auto node = std::make_shared<RSDisplayRenderNode>(id, config, context);
    uint32_t skipFrameInterval = 0;
    node->SkipFrame(skipFrameInterval);
    skipFrameInterval = 10;
    node->frameCount_ = 0;
    ASSERT_FALSE(node->SkipFrame(skipFrameInterval));
    node->frameCount_ = 6;
    ASSERT_TRUE(node->SkipFrame(skipFrameInterval));
}
/**
 * @tc.name: SetMirrorSourceTest
 * @tc.desc: test results of SetMirrorSource
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSDisplayRenderNodeTest, SetMirrorSourceTest, TestSize.Level1)
{
    std::shared_ptr<RSDisplayRenderNode> rsDisplayRenderNode = nullptr;
    auto node = std::make_shared<RSDisplayRenderNode>(id, config, context);
    node->SetMirrorSource(rsDisplayRenderNode);

    node->isMirroredDisplay_ = true;
    node->SetMirrorSource(rsDisplayRenderNode);

    rsDisplayRenderNode = std::make_shared<RSDisplayRenderNode>(id + 1, config, context);
    node->SetMirrorSource(rsDisplayRenderNode);
    ASSERT_NE(node->mirrorSource_.lock(), nullptr);
}

/**
 * @tc.name: CreateSurfaceTest
 * @tc.desc: test results of CreateSurface
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSDisplayRenderNodeTest, CreateSurfaceTest, TestSize.Level1)
{
    sptr<IBufferConsumerListener> listener;
    auto node = std::make_shared<RSDisplayRenderNode>(id, config, context);
    ASSERT_TRUE(node->CreateSurface(listener));
    ASSERT_TRUE(node->CreateSurface(listener));
}

/**
 * @tc.name: GetRotationTest
 * @tc.desc: test results of GetRotation
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSDisplayRenderNodeTest, GetRotationTest, TestSize.Level1)
{
    auto node = std::make_shared<RSDisplayRenderNode>(id, config, context);
    node->UpdateRotation();
    node->GetRotation();
    RSProperties& properties = const_cast<RSProperties&>(node->GetRenderProperties());
    properties.boundsGeo_.reset(new RSObjAbsGeometry());
    node->GetRotation();
    ASSERT_TRUE(true);
}

/**
 * @tc.name: IsRotationChangedTest
 * @tc.desc: test results of IsRotationChanged
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSDisplayRenderNodeTest, IsRotationChangedTest, TestSize.Level1)
{
    auto node = std::make_shared<RSDisplayRenderNode>(id, config, context);
    node->UpdateRotation();
    ASSERT_FALSE(node->IsRotationChanged());
    RSProperties& properties = const_cast<RSProperties&>(node->GetRenderProperties());
    properties.boundsGeo_.reset(new RSObjAbsGeometry());
    node->IsRotationChanged();
    ASSERT_TRUE(true);
}

/**
 * @tc.name: SetBootAnimationTest
 * @tc.desc:  test results of SetBootAnimation
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
    childNode->SetBootAnimation(false);
    ASSERT_FALSE(node->GetBootAnimation());
}

/**
 * @tc.name: GetBootAnimationTest
 * @tc.desc:  test results of GetBootAnimation
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
 * @tc.desc:  test results of SetRootIdOfCaptureWindow
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

/**
 * @tc.name: CollectSurface
 * @tc.desc:  test results of CollectSurface
 * @tc.type:FUNC
 * @tc.require:issueI981R9
 */
HWTEST_F(RSDisplayRenderNodeTest, CollectSurface, TestSize.Level2)
{
    auto displayNode = std::make_shared<RSDisplayRenderNode>(id, config, context);
    RSContext* rsContext = new RSContext();
    std::shared_ptr<RSContext> sharedContext(rsContext);
    std::shared_ptr<RSBaseRenderNode> node = std::make_shared<RSRenderNode>(1, sharedContext);
    std::vector<RSBaseRenderNode::SharedPtr> vec;
    bool isUniRender = true;
    bool onlyFirstLevel = true;
    displayNode->CollectSurface(node, vec, isUniRender, onlyFirstLevel);
    ASSERT_TRUE(true);
}

/**
 * @tc.name: ProcessTest
 * @tc.desc: test results of Process
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSDisplayRenderNodeTest, ProcessTest, TestSize.Level1)
{
    std::shared_ptr<RSNodeVisitor> visitor = nullptr;
    auto displayNode = std::make_shared<RSDisplayRenderNode>(id, config, context);
    displayNode->Process(visitor);

    visitor = std::make_shared<RSRenderThreadVisitor>();
    displayNode->Process(visitor);
    ASSERT_TRUE(true);
}
} // namespace OHOS::Rosen