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
#include "display_engine/rs_luminance_control.h"
#include "params/rs_screen_render_params.h"
#include "pipeline/rs_canvas_render_node.h"
#include "pipeline/rs_screen_render_node.h"
#include "render_thread/rs_render_thread_visitor.h"
#include "pipeline/rs_surface_render_node.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSScreenRenderNodeTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    static inline NodeId id;
    static inline ScreenId screenId;
    static inline std::weak_ptr<RSContext> context = {};
};

void RSScreenRenderNodeTest::SetUpTestCase() {}
void RSScreenRenderNodeTest::TearDownTestCase() {}
void RSScreenRenderNodeTest::SetUp() {}
void RSScreenRenderNodeTest::TearDown() {}

/**
 * @tc.name: PrepareTest
 * @tc.desc: test results of Prepare
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSScreenRenderNodeTest, PrepareTest, TestSize.Level1)
{
    auto node = std::make_shared<RSScreenRenderNode>(id, screenId, context);
    std::shared_ptr<RSNodeVisitor> visitor = nullptr;
    node->QuickPrepare(visitor);
    node->Prepare(visitor);

    visitor = std::make_shared<RSRenderThreadVisitor>();
    node->QuickPrepare(visitor);
    node->Prepare(visitor);
    ASSERT_TRUE(true);
}

/**
 * @tc.name: SkipFrameTest001
 * @tc.desc: test SkipFrame for refreshRate 0 and skipFrameInterval 0
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSScreenRenderNodeTest, SkipFrameTest001, TestSize.Level1)
{
    auto node = std::make_shared<RSScreenRenderNode>(id, screenId, context);
    uint32_t refreshRate = 0;
    uint32_t skipFrameInterval = 0;
    ASSERT_FALSE(node->SkipFrame(refreshRate, skipFrameInterval));
}

/**
 * @tc.name: SkipFrameTest002
 * @tc.desc: test SkipFrame for skipFrameInterval 0
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSScreenRenderNodeTest, SkipFrameTest002, TestSize.Level1)
{
    auto node = std::make_shared<RSScreenRenderNode>(id, screenId, context);
    uint32_t refreshRate = 60; // 60hz
    uint32_t skipFrameInterval = 0;
    ASSERT_FALSE(node->SkipFrame(refreshRate, skipFrameInterval));
}

/**
 * @tc.name: SkipFrameTest003
 * @tc.desc: test SkipFrame for skipFrameInterval 1
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSScreenRenderNodeTest, SkipFrameTest003, TestSize.Level1)
{
    auto node = std::make_shared<RSScreenRenderNode>(id, screenId, context);
    uint32_t refreshRate = 60; // 60hz
    uint32_t skipFrameInterval = 1;
    ASSERT_FALSE(node->SkipFrame(refreshRate, skipFrameInterval));
}

/**
 * @tc.name: SkipFrameTest004
 * @tc.desc: test SkipFrame for time within skipFrameInterval 2
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSScreenRenderNodeTest, SkipFrameTest004, TestSize.Level1)
{
    auto node = std::make_shared<RSScreenRenderNode>(id, screenId, context);
    uint32_t refreshRate = 60; // 60hz
    uint32_t skipFrameInterval = 2; // skipFrameInterval 2
    node->SkipFrame(refreshRate, skipFrameInterval);
    ASSERT_TRUE(node->SkipFrame(refreshRate, skipFrameInterval));
}

/**
 * @tc.name: SkipFrameTest005
 * @tc.desc: test SkipFrame for time over skipFrameInterval 2
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSScreenRenderNodeTest, SkipFrameTest005, TestSize.Level1)
{
    auto node = std::make_shared<RSScreenRenderNode>(id, screenId, context);
    uint32_t refreshRate = 60; // 60hz
    uint32_t skipFrameInterval = 2; // skipFrameInterval 2
    node->SkipFrame(refreshRate, skipFrameInterval);
    usleep(50000); // 50000us == 50ms
    ASSERT_FALSE(node->SkipFrame(refreshRate, skipFrameInterval));
}

/**
 * @tc.name: SkipFrameTest006
 * @tc.desc: test SkipFrame for time within skipFrameInterval 6
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSScreenRenderNodeTest, SkipFrameTest006, TestSize.Level1)
{
    auto node = std::make_shared<RSScreenRenderNode>(id, screenId, context);
    uint32_t refreshRate = 60; // 60hz
    uint32_t skipFrameInterval = 6; // skipFrameInterval 6
    node->SkipFrame(refreshRate, skipFrameInterval);
    usleep(50000); // 50000us == 50ms
    ASSERT_TRUE(node->SkipFrame(refreshRate, skipFrameInterval));
}

/**
 * @tc.name: SkipFrameTest007
 * @tc.desc: test SkipFrame for time over skipFrameInterval 6
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSScreenRenderNodeTest, SkipFrameTest007, TestSize.Level1)
{
    auto node = std::make_shared<RSScreenRenderNode>(id, screenId, context);
    uint32_t refreshRate = 60; // 60hz
    uint32_t skipFrameInterval = 6; // skipFrameInterval 6
    node->SkipFrame(refreshRate, skipFrameInterval);
    usleep(150000); // 150000us == 150ms
    ASSERT_FALSE(node->SkipFrame(refreshRate, skipFrameInterval));
}

/**
 * @tc.name: SkipFrameTest008
 * @tc.desc: test SkipFrame for time over skipFrameInterval 55
 * @tc.type:FUNC
 * @tc.require: issuesIAVK8D
 */
HWTEST_F(RSScreenRenderNodeTest, SkipFrameTest008, TestSize.Level1)
{
    auto node = std::make_shared<RSScreenRenderNode>(id, screenId, context);
    uint32_t refreshRate = 60; // 60hz
    uint32_t skipFrameInterval = 55; // skipFrameInterval 55
    node->SkipFrame(refreshRate, skipFrameInterval);
    usleep(16666); // 16666us == 16.666ms
    ASSERT_FALSE(node->SkipFrame(refreshRate, skipFrameInterval));
}

/**
 * @tc.name: SkipFrameTest009
 * @tc.desc: test SkipFrame for time over skipFrameInterval 45
 * @tc.type:FUNC
 * @tc.require: issuesIAVK8D
 */
HWTEST_F(RSScreenRenderNodeTest, SkipFrameTest009, TestSize.Level1)
{
    auto node = std::make_shared<RSScreenRenderNode>(id, screenId, context);
    uint32_t refreshRate = 60; // 60hz
    uint32_t skipFrameInterval = 45; // skipFrameInterval 45
    node->SkipFrame(refreshRate, skipFrameInterval);
    usleep(16666); // 16666us == 16.666ms
    ASSERT_TRUE(node->SkipFrame(refreshRate, skipFrameInterval));
}

/**
 * @tc.name: SkipFrameTest010
 * @tc.desc: test SkipFrame for time over skipFrameInterval 25
 * @tc.type:FUNC
 * @tc.require: issuesIAVK8D
 */
HWTEST_F(RSScreenRenderNodeTest, SkipFrameTest010, TestSize.Level1)
{
    auto node = std::make_shared<RSScreenRenderNode>(id, screenId, context);
    uint32_t refreshRate = 60; // 60hz
    uint32_t skipFrameInterval = 25; // skipFrameInterval 25
    node->SkipFrame(refreshRate, skipFrameInterval);
    usleep(16666); // 16666us == 16.666ms
    ASSERT_TRUE(node->SkipFrame(refreshRate, skipFrameInterval));
}

/**
 * @tc.name: IsMirrorScreenTest001
 * @tc.desc: test result for IsMirrorScreenTest001
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSScreenRenderNodeTest, IsMirrorScreenTest001, TestSize.Level1)
{
    auto node = std::make_shared<RSScreenRenderNode>(id, 0, context);
    EXPECT_NE(node, nullptr);
    EXPECT_EQ(node->IsMirrorScreen(), false);
    node->isMirroredScreen_ = true;
    EXPECT_EQ(node->IsMirrorScreen(), true);
}

/**
 * @tc.name: IsMirrorScreenTest002
 * @tc.desc: test result for IsMirrorScreenTest002
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSScreenRenderNodeTest, IsMirrorScreenTest002, TestSize.Level1)
{
    auto node = std::make_shared<RSScreenRenderNode>(id, 0, context);
    node->SetIsMirrorScreen(true);
    EXPECT_EQ(node->IsMirrorScreen(), true);
    // test Repeat settings
    node->SetIsMirrorScreen(true);
    EXPECT_EQ(node->IsMirrorScreen(), true);
    // Test set to false
    node->SetIsMirrorScreen(false);
    EXPECT_EQ(node->IsMirrorScreen(), false);
}

/**
 * @tc.name: SetIsMirrorScreenTest
 * @tc.desc: test result for SetIsMirrorScreen
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSScreenRenderNodeTest, SetIsMirrorScreenTest, TestSize.Level1)
{
    auto node = std::make_shared<RSScreenRenderNode>(id, 0, context);
    EXPECT_NE(node, nullptr);
    node->isMirroredScreen_ = false;
    node->SetIsMirrorScreen(true);
    EXPECT_EQ(node->IsMirrorScreen(), true);
    node->SetIsMirrorScreen(false);
    EXPECT_EQ(node->IsMirrorScreen(), false);
}

/**
 * @tc.name: SetReleaseTaskTest
 * @tc.desc: test result for SetReleaseTask
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSScreenRenderNodeTest, SetReleaseTaskTest, TestSize.Level1)
{
    auto node = std::make_shared<RSScreenRenderNode>(id, 0, context);
    EXPECT_NE(node, nullptr);
    node->releaseScreenDmaBufferTask_ = nullptr;
    node->SetReleaseTask([](ScreenId id) {});
    node->SetReleaseTask(nullptr);

    auto task = [](ScreenId id) {};
    node->releaseScreenDmaBufferTask_ = task;
    node->SetReleaseTask([](ScreenId id) {});
    node->SetReleaseTask(task);
}

/**
 * @tc.name: ResetMirrorSourceTest
 * @tc.desc: test results of ResetMirrorSource
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSScreenRenderNodeTest, ResetMirrorSourceTest, TestSize.Level1)
{
    auto node = std::make_shared<RSScreenRenderNode>(id, screenId, context);
    node->SetMirrorSource(nullptr);
    EXPECT_EQ(node->mirrorSource_.lock(), nullptr);
    node->ResetMirrorSource();
    EXPECT_EQ(node->mirrorSource_.lock(), nullptr);

    auto rsScreenRenderNode = std::make_shared<RSScreenRenderNode>(id + 1, screenId, context);
    node->isMirroredScreen_ = true;
    node->SetMirrorSource(rsScreenRenderNode);
    EXPECT_NE(node->mirrorSource_.lock(), nullptr);
    node->ResetMirrorSource();
    EXPECT_EQ(node->mirrorSource_.lock(), nullptr);
}

/**
 * @tc.name: SetDisplayGlobalZorderTest
 * @tc.desc: test results of SetDisplayGlobalZorder
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSScreenRenderNodeTest, SetDisplayGlobalZorderTest, TestSize.Level1)
{
    auto node = std::make_shared<RSScreenRenderNode>(id, 0, context);
    node->SetDisplayGlobalZOrder(1.0);
    node->stagingRenderParams_ = std::make_unique<RSScreenRenderParams>(node->GetId());
    ASSERT_NE(node->stagingRenderParams_, nullptr);
    node->SetDisplayGlobalZOrder(1.0);
    ASSERT_NE(node->stagingRenderParams_, nullptr);
}

/**
 * @tc.name: SetNeedForceUpdateHwcNodesTest
 * @tc.desc: test results of SetNeedForceUpdateHwcNodes
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSScreenRenderNodeTest, SetNeedForceUpdateHwcNodesTest, TestSize.Level1)
{
    auto node = std::make_shared<RSScreenRenderNode>(id, 0, context);
    node->stagingRenderParams_ = std::make_unique<RSScreenRenderParams>(node->GetId());
    ASSERT_NE(node->stagingRenderParams_, nullptr);
    node->SetNeedForceUpdateHwcNodes(true, true);
    ASSERT_NE(node->stagingRenderParams_, nullptr);
}

/**
 * @tc.name: SetFingerprintTest
 * @tc.desc: test results of SetFingerprint
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSScreenRenderNodeTest, SetFingerprintTest, TestSize.Level1)
{
    auto node = std::make_shared<RSScreenRenderNode>(id, 0, context);
    node->SetFingerprint(true);
    node->stagingRenderParams_ = std::make_unique<RSScreenRenderParams>(node->GetId());
    ASSERT_NE(node->stagingRenderParams_, nullptr);
    node->SetFingerprint(true);
    node->stagingRenderParams_->SetNeedSync(true);
    node->SetFingerprint(true);
    ASSERT_NE(node->stagingRenderParams_, nullptr);
}

/**
 * @tc.name: FixVirtualBuffer10BitTest
 * @tc.desc: test results of SetFixVirtualBuffer10Bit, GetFixVirtualBuffer10Bit
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSScreenRenderNodeTest, FixVirtualBuffer10BitTest, TestSize.Level1)
{
    auto node = std::make_shared<RSScreenRenderNode>(id, 0, context);
    node->SetFixVirtualBuffer10Bit(false);
    node->SetFixVirtualBuffer10Bit(true);
    EXPECT_EQ(node->GetFixVirtualBuffer10Bit(), false);
    node->stagingRenderParams_ = std::make_unique<RSScreenRenderParams>(node->GetId());
    ASSERT_NE(node->stagingRenderParams_, nullptr);
    node->SetFixVirtualBuffer10Bit(true);
    EXPECT_EQ(node->GetFixVirtualBuffer10Bit(), true);
    node->stagingRenderParams_->SetNeedSync(false);
    node->SetFixVirtualBuffer10Bit(false);
    node->stagingRenderParams_->SetNeedSync(true);
    node->SetFixVirtualBuffer10Bit(true);
    EXPECT_EQ(node->GetFixVirtualBuffer10Bit(), true);
}

/**
 * @tc.name: ExistHWCNodeTest
 * @tc.desc: test results of SetExistHWCNode, GetExistHWCNode
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSScreenRenderNodeTest, ExistHWCNodeTest, TestSize.Level1)
{
    auto node = std::make_shared<RSScreenRenderNode>(id, 0, context);
    node->SetExistHWCNode(false);
    node->SetExistHWCNode(true);
    EXPECT_EQ(node->GetExistHWCNode(), false);
    node->stagingRenderParams_ = std::make_unique<RSScreenRenderParams>(node->GetId());
    ASSERT_NE(node->stagingRenderParams_, nullptr);
    node->SetExistHWCNode(true);
    EXPECT_EQ(node->GetExistHWCNode(), true);
    node->stagingRenderParams_->SetNeedSync(false);
    node->SetExistHWCNode(false);
    node->stagingRenderParams_->SetNeedSync(true);
    node->SetExistHWCNode(true);
    EXPECT_EQ(node->GetExistHWCNode(), true);
}

/**
 * @tc.name: SetHDRPresentTest
 * @tc.desc: test results of SetHDRPresent
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSScreenRenderNodeTest, SetHDRPresentTest, TestSize.Level1)
{
    auto node = std::make_shared<RSScreenRenderNode>(id, 0, context);
    node->SetHDRPresent(true);
    node->stagingRenderParams_ = std::make_unique<RSScreenRenderParams>(node->GetId());
    ASSERT_NE(node->stagingRenderParams_, nullptr);
    node->SetHDRPresent(true);
    node->stagingRenderParams_->SetNeedSync(true);
    node->SetHDRPresent(true);
    ASSERT_NE(node->stagingRenderParams_, nullptr);
}

/**
 * @tc.name: SetBrightnessRatioTest
 * @tc.desc: test results of SetBrightnessRatioTest
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSScreenRenderNodeTest, SetBrightnessRatioTest, TestSize.Level1)
{
    auto node = std::make_shared<RSScreenRenderNode>(id, 0, context);
    node->SetBrightnessRatio(1.0);
    node->stagingRenderParams_ = std::make_unique<RSScreenRenderParams>(node->GetId());
    ASSERT_NE(node->stagingRenderParams_, nullptr);
    node->SetBrightnessRatio(1.0);
    node->stagingRenderParams_->SetNeedSync(true);
    node->SetBrightnessRatio(1.0);
    ASSERT_NE(node->stagingRenderParams_, nullptr);
}

/**
 * @tc.name: SetColorSpaceTest002
 * @tc.desc: test results of SetColorSpace
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSScreenRenderNodeTest, SetColorSpaceTest002, TestSize.Level1)
{
    auto node = std::make_shared<RSScreenRenderNode>(id, screenId, context);
    node->stagingRenderParams_ = std::make_unique<RSScreenRenderParams>(node->GetId());
    ASSERT_NE(node->stagingRenderParams_, nullptr);
    node->stagingRenderParams_->SetNeedSync(true);
    node->SetColorSpace(GraphicColorGamut::GRAPHIC_COLOR_GAMUT_DISPLAY_P3);
    auto colorSpace = node->GetColorSpace();
    ASSERT_EQ(GraphicColorGamut::GRAPHIC_COLOR_GAMUT_DISPLAY_P3, colorSpace);
}

/**
 * @tc.name: SetTargetSurfaceRenderNodeDrawableTest
 * @tc.desc: test results of SetTargetSurfaceRenderNodeDrawable
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSScreenRenderNodeTest, SetTargetSurfaceRenderNodeDrawableTest, TestSize.Level1)
{
    auto node = std::make_shared<RSScreenRenderNode>(id, screenId, context);
    node->SetTargetSurfaceRenderNodeDrawable(std::weak_ptr<DrawableV2::RSRenderNodeDrawableAdapter>());
    node->stagingRenderParams_ = std::make_unique<RSScreenRenderParams>(node->GetId());
    ASSERT_NE(node->stagingRenderParams_, nullptr);
    node->SetTargetSurfaceRenderNodeDrawable(std::weak_ptr<DrawableV2::RSRenderNodeDrawableAdapter>());
    node->stagingRenderParams_->SetNeedSync(true);
    node->SetTargetSurfaceRenderNodeDrawable(std::weak_ptr<DrawableV2::RSRenderNodeDrawableAdapter>());
}

/**
 * @tc.name: SetHasMirrorScreenTest
 * @tc.desc: test results of SetHasMirrorScreen
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSScreenRenderNodeTest, SetHasMirrorScreenTest, TestSize.Level1)
{
    auto node = std::make_shared<RSScreenRenderNode>(id, screenId, context);
    node->stagingRenderParams_ = std::make_unique<RSScreenRenderParams>(node->GetId());
    ASSERT_NE(node->stagingRenderParams_, nullptr);
    auto screenParams = static_cast<RSScreenRenderParams*>(node->stagingRenderParams_.get());
    node->stagingRenderParams_->SetNeedSync(true);
    node->SetHasMirrorScreen(true);
    EXPECT_TRUE(screenParams->HasMirrorScreen());
    node->stagingRenderParams_->SetNeedSync(true);
    node->SetHasMirrorScreen(false);
    EXPECT_FALSE(screenParams->HasMirrorScreen());
}

/**
 * @tc.name: SetBootAnimationTest
 * @tc.desc:  test results of SetBootAnimation
 * @tc.type:FUNC
 * @tc.require:SR000HSUII
 */
HWTEST_F(RSScreenRenderNodeTest, SetBootAnimationTest, TestSize.Level1)
{
    NodeId id = 0;
    std::shared_ptr<RSRenderNode> node = std::make_shared<RSRenderNode>(id);
    auto childNode = std::make_shared<RSScreenRenderNode>(id + 1, screenId);
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
HWTEST_F(RSScreenRenderNodeTest, GetBootAnimationTest, TestSize.Level1)
{
    NodeId id = 0;
    auto node = std::make_shared<RSScreenRenderNode>(id, screenId, context);
    node->SetBootAnimation(true);
    ASSERT_TRUE(node->GetBootAnimation());
    node->SetBootAnimation(false);
    ASSERT_FALSE(node->GetBootAnimation());
}

/**
 * @tc.name: CollectSurface
 * @tc.desc:  test results of CollectSurface
 * @tc.type:FUNC
 * @tc.require:issueI981R9
 */
HWTEST_F(RSScreenRenderNodeTest, CollectSurface, TestSize.Level2)
{
    NodeId id = 0;
    auto screenNode = std::make_shared<RSScreenRenderNode>(id, screenId, context);
    RSContext* rsContext = new RSContext();
    std::shared_ptr<RSContext> sharedContext(rsContext);
    std::shared_ptr<RSBaseRenderNode> node = std::make_shared<RSRenderNode>(1, sharedContext);
    std::vector<RSBaseRenderNode::SharedPtr> vec;
    bool isUniRender = true;
    bool onlyFirstLevel = true;
    screenNode->CollectSurface(node, vec, isUniRender, onlyFirstLevel);
    auto fullChildrenList = std::make_shared<std::vector<std::shared_ptr<RSRenderNode>>>();
    auto screenNode1 = std::make_shared<RSScreenRenderNode>(id + 1, screenId + 1, context);
    fullChildrenList->emplace_back(std::move(screenNode1));
    screenNode->fullChildrenList_ = fullChildrenList;
    ASSERT_TRUE(true);
}

/**
 * @tc.name: ProcessTest
 * @tc.desc: test results of Process
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSScreenRenderNodeTest, ProcessTest, TestSize.Level1)
{
    std::shared_ptr<RSNodeVisitor> visitor = nullptr;
    NodeId id = 0;
    auto screenNode = std::make_shared<RSScreenRenderNode>(id, screenId, context);
    screenNode->Process(visitor);

    visitor = std::make_shared<RSRenderThreadVisitor>();
    screenNode->Process(visitor);
    ASSERT_TRUE(true);
}

/**
 * @tc.name: SetIsOnTheTree
 * @tc.desc: test results of SetIsOnTheTree
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSScreenRenderNodeTest, SetIsOnTheTree, TestSize.Level1)
{
    NodeId id = 0;
    auto screenNode = std::make_shared<RSScreenRenderNode>(id, screenId, context);
    bool flag = true;
    screenNode->SetIsOnTheTree(flag, id, id, id, id);
    ASSERT_TRUE(true);
}

/**
 * @tc.name: SetCompositeType
 * @tc.desc: test results of SetCompositeType
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSScreenRenderNodeTest, SetCompositeType, TestSize.Level1)
{
    NodeId id = 0;
    auto screenNode = std::make_shared<RSScreenRenderNode>(id, screenId, context);
    CompositeType type = CompositeType::UNI_RENDER_COMPOSITE;
    screenNode->SetCompositeType(type);
    ASSERT_EQ(screenNode->GetCompositeType(), type);
}

/**
 * @tc.name: SetForceSoftComposite
 * @tc.desc: test results of SetForceSoftComposite
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSScreenRenderNodeTest, SetForceSoftComposite, TestSize.Level1)
{
    NodeId id = 0;
    auto screenNode = std::make_shared<RSScreenRenderNode>(id, 1);
    bool flag = true;
    screenNode->SetForceSoftComposite(flag);
    ASSERT_EQ(screenNode->IsForceSoftComposite(), flag);
}

/**
 * @tc.name: UpdateRenderParams
 * @tc.desc: test results of UpdateRenderParams
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSScreenRenderNodeTest, UpdateRenderParams, TestSize.Level1)
{
    NodeId id = 0;

    auto screenNode = std::make_shared<RSScreenRenderNode>(id, screenId, context);
    screenNode->UpdateRenderParams();
    ASSERT_TRUE(true);
    auto rsScreenRenderNode = std::make_shared<RSScreenRenderNode>(id + 1, 0, context);
    screenNode->isMirroredScreen_ = true;
    screenNode->SetMirrorSource(rsScreenRenderNode);
    EXPECT_NE(screenNode->mirrorSource_.lock(), nullptr);
    screenNode->UpdateRenderParams();
    ASSERT_TRUE(true);
}

/**
 * @tc.name: UpdateScreenRenderParams
 * @tc.desc: test results of UpdateRenderParams
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSScreenRenderNodeTest, UpdateScreenRenderParams, TestSize.Level1)
{
    NodeId id = 0;
    auto screenNode = std::make_shared<RSScreenRenderNode>(id, 1, context);
    ASSERT_TRUE(true);
}

/**
 * @tc.name: UpdatePartialRenderParams
 * @tc.desc: test results of UpdatePartialRenderParams
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSScreenRenderNodeTest, UpdatePartialRenderParams, TestSize.Level1)
{
    NodeId id = 0;
    auto screenNode = std::make_shared<RSScreenRenderNode>(id, screenId, context);
    screenNode->UpdateRenderParams();
    ASSERT_TRUE(true);
}

/**
 * @tc.name: UpdateDisplayDirtyManager
 * @tc.desc: test results of UpdateDisplayDirtyManager
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSScreenRenderNodeTest, UpdateDisplayDirtyManager, TestSize.Level1)
{
    NodeId id = 0;
    auto screenNode = std::make_shared<RSScreenRenderNode>(id, 1, context);
    int32_t bufferage = 1;
    bool useAlignedDirtyRegion = false;
    screenNode->UpdateDisplayDirtyManager(bufferage, useAlignedDirtyRegion);
    ASSERT_TRUE(true);
}

/**
 * @tc.name: GetSortedChildren
 * @tc.desc: test results of GetSortedChildren
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSScreenRenderNodeTest, GetSortedChildren, TestSize.Level1)
{
    auto screenNode = std::make_shared<RSScreenRenderNode>(id, screenId, context);
    screenNode->GetSortedChildren();
    screenNode->isNeedWaitNewScbPid_ = true;
    screenNode->GetSortedChildren();
    ASSERT_TRUE(true);
}

/**
 * @tc.name: GetDisappearedSurfaceRegionBelowCurrent001
 * @tc.desc: test results of GetDisappearedSurfaceRegionBelowCurrent
 * @tc.type:FUNC
 * @tc.require: issuesIA8LNR
 */
HWTEST_F(RSScreenRenderNodeTest, GetDisappearedSurfaceRegionBelowCurrent001, TestSize.Level1)
{
    NodeId id = 0;
    auto screenNode = std::make_shared<RSScreenRenderNode>(id, screenId, context);
    ASSERT_NE(screenNode, nullptr);
}

/**
 * @tc.name: GetDisappearedSurfaceRegionBelowCurrent002
 * @tc.desc: test results of GetDisappearedSurfaceRegionBelowCurrent
 * @tc.type:FUNC
 * @tc.require: issuesIA8LNR
 */
HWTEST_F(RSScreenRenderNodeTest, GetDisappearedSurfaceRegionBelowCurrent002, TestSize.Level1)
{
    auto screenNode = std::make_shared<RSScreenRenderNode>(id, screenId, context);
    ASSERT_NE(screenNode, nullptr);

    constexpr NodeId bottomSurfaceNodeId = 1;
    const RectI bottomSurfacePos(0, 0, 1, 1);
    const std::pair<NodeId, RectI> bottomSurface{ bottomSurfaceNodeId, bottomSurfacePos };
    constexpr NodeId topSurfaceNodeId = 2;
    const RectI topSurfacePos(0, 0, 2, 2);
    const std::pair<NodeId, RectI> topSurface{ topSurfaceNodeId, topSurfacePos };
}

/**
 * @tc.name: RecordMainAndLeashSurfaces001
 * @tc.desc: test RecordMainAndLeashSurfaces
 * @tc.type:FUNC
 * @tc.require: issueIANDBE
 */
HWTEST_F(RSScreenRenderNodeTest, RecordMainAndLeashSurfaces001, TestSize.Level2)
{
    auto screenNode = std::make_shared<RSScreenRenderNode>(id, 1, context);
    ASSERT_NE(screenNode, nullptr);
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(id + 1, context);
    ASSERT_NE(surfaceNode, nullptr);

    screenNode->RecordMainAndLeashSurfaces(surfaceNode);
}

/**
 * @tc.name: HandleCurMainAndLeashSurfaceNodes001
 * @tc.desc: test HandleCurMainAndLeashSurfaceNodes while curMainAndLeashSurfaceNodes_ is empty
 * @tc.type:FUNC
 * @tc.require: issueIANDBE
 */
HWTEST_F(RSScreenRenderNodeTest, HandleCurMainAndLeashSurfaceNodes001, TestSize.Level2)
{
    auto screenNode = std::make_shared<RSScreenRenderNode>(id, screenId, context);
    ASSERT_NE(screenNode, nullptr);
}

/**
 * @tc.name: HandleCurMainAndLeashSurfaceNodes002
 * @tc.desc: test HandleCurMainAndLeashSurfaceNodes while the node isn't surface node
 * @tc.type:FUNC
 * @tc.require: issueIANDBE
 */
HWTEST_F(RSScreenRenderNodeTest, HandleCurMainAndLeashSurfaceNodes002, TestSize.Level2)
{
    auto screenNode = std::make_shared<RSScreenRenderNode>(id, screenId, context);
    ASSERT_NE(screenNode, nullptr);
    auto canvasNode = std::make_shared<RSCanvasRenderNode>(id + 1);
    ASSERT_NE(canvasNode, nullptr);
}

/**
 * @tc.name: HandleCurMainAndLeashSurfaceNodes003
 * @tc.desc: test HandleCurMainAndLeashSurfaceNodes while the node is leash window
 * @tc.type:FUNC
 * @tc.require: issueIANDBE
 */
HWTEST_F(RSScreenRenderNodeTest, HandleCurMainAndLeashSurfaceNodes003, TestSize.Level2)
{
    auto screenNode = std::make_shared<RSScreenRenderNode>(id, screenId, context);
    ASSERT_NE(screenNode, nullptr);
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(id + 1, context);
    ASSERT_NE(surfaceNode, nullptr);
    surfaceNode->SetSurfaceNodeType(RSSurfaceNodeType::LEASH_WINDOW_NODE);
}

/**
 * @tc.name: HandleCurMainAndLeashSurfaceNodes004
 * @tc.desc: test HandleCurMainAndLeashSurfaceNodes while the node isn't leash window and on the tree
 * @tc.type:FUNC
 * @tc.require: issueIANDBE
 */
HWTEST_F(RSScreenRenderNodeTest, HandleCurMainAndLeashSurfaceNodes004, TestSize.Level2)
{
    auto screenNode = std::make_shared<RSScreenRenderNode>(id, screenId, context);
    ASSERT_NE(screenNode, nullptr);
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(id + 1, context);
    ASSERT_NE(surfaceNode, nullptr);
    surfaceNode->isOnTheTree_ = true;
}

/**
 * @tc.name: HandleCurMainAndLeashSurfaceNodes005
 * @tc.desc: test HandleCurMainAndLeashSurfaceNodes while the node isn't leash window and not on the tree
 * @tc.type:FUNC
 * @tc.require: issueIANDBE
 */
HWTEST_F(RSScreenRenderNodeTest, HandleCurMainAndLeashSurfaceNodes005, TestSize.Level2)
{
    auto screenNode = std::make_shared<RSScreenRenderNode>(id, screenId, context);
    ASSERT_NE(screenNode, nullptr);
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(id + 1, context);
    ASSERT_NE(surfaceNode, nullptr);
    surfaceNode->isOnTheTree_ = false;
}

/**
 * @tc.name: ForceCloseHdr
 * @tc.desc: test results of Get and Set ForceCloseHdr
 * @tc.type:FUNC
 * @tc.require: issuesIB6QKS
 */
HWTEST_F(RSScreenRenderNodeTest, ForceCloseHdrTest, TestSize.Level1)
{
    auto node = std::make_shared<RSScreenRenderNode>(id, 1, context);
    ASSERT_NE(node, nullptr);
    node->InitRenderParams();
    node->SetForceCloseHdr(false);
    EXPECT_EQ(node->GetForceCloseHdr(), false);
}

/**
 * @tc.name: HasUniRenderHdrSurface
 * @tc.desc: test results of Get and Set HasUniRenderHdrSurface
 * @tc.type:FUNC
 * @tc.require: issuesIB6QKS
 */
HWTEST_F(RSScreenRenderNodeTest, HasUniRenderHdrSurfaceTest, TestSize.Level1)
{
    auto node = std::make_shared<RSScreenRenderNode>(id, 1, context);
    ASSERT_NE(node, nullptr);
    node->InitRenderParams();
    EXPECT_EQ(node->GetHasUniRenderHdrSurface(), false);
    node->SetHasUniRenderHdrSurface(true);
    EXPECT_EQ(node->GetHasUniRenderHdrSurface(), true);
}

/**
 * @tc.name: IsLuminanceStatusChange
 * @tc.desc: test results of Get and Set IsLuminanceStatusChange
 * @tc.type:FUNC
 * @tc.require: issuesIB6QKS
 */
HWTEST_F(RSScreenRenderNodeTest, IsLuminanceStatusChangeTest, TestSize.Level1)
{
    auto node = std::make_shared<RSScreenRenderNode>(id, 1, context);
    ASSERT_NE(node, nullptr);
    node->InitRenderParams();
    EXPECT_EQ(node->GetIsLuminanceStatusChange(), false);
    node->SetIsLuminanceStatusChange(true);
    EXPECT_EQ(node->GetIsLuminanceStatusChange(), true);
}

/**
 * @tc.name: GetColorSpaceTest
 * @tc.desc: test results of GetColorSpace
 * @tc.type:FUNC
 * @tc.require: issuesIB6QKS
 */
HWTEST_F(RSScreenRenderNodeTest, GetColorSpaceTest, TestSize.Level1)
{
    auto node = std::make_shared<RSScreenRenderNode>(id, 1, context);
    node->InitRenderParams();
    auto colorSpace = node->GetColorSpace();
    ASSERT_EQ(GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB, colorSpace);
}

/**
 * @tc.name: SetColorSpaceTest
 * @tc.desc: test results of SetColorSpace
 * @tc.type:FUNC
 * @tc.require: issuesIB6QKS
 */
HWTEST_F(RSScreenRenderNodeTest, SetColorSpaceTest, TestSize.Level1)
{
    auto node = std::make_shared<RSScreenRenderNode>(id, 1, context);
    node->InitRenderParams();
    node->SetColorSpace(GraphicColorGamut::GRAPHIC_COLOR_GAMUT_DISPLAY_P3);
    auto colorSpace = node->GetColorSpace();
    ASSERT_EQ(GraphicColorGamut::GRAPHIC_COLOR_GAMUT_DISPLAY_P3, colorSpace);
}

/**
 * @tc.name: UpdateColorSpaceTest
 * @tc.desc: test results of UpdateColorSpace
 * @tc.type:FUNC
 * @tc.require: issueICGKPE
 */
HWTEST_F(RSScreenRenderNodeTest, UpdateColorSpaceTest, TestSize.Level1)
{
    auto node = std::make_shared<RSScreenRenderNode>(id, 1, context);
    node->InitRenderParams();
    node->UpdateColorSpace(GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB);
    ASSERT_EQ(GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB, node->GetColorSpace());

    node->UpdateColorSpace(GraphicColorGamut::GRAPHIC_COLOR_GAMUT_DISPLAY_P3);
    ASSERT_EQ(GraphicColorGamut::GRAPHIC_COLOR_GAMUT_DISPLAY_P3, node->GetColorSpace());

    node->UpdateColorSpace(GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB);
    ASSERT_EQ(GraphicColorGamut::GRAPHIC_COLOR_GAMUT_DISPLAY_P3, node->GetColorSpace());

    node->UpdateColorSpace(GraphicColorGamut::GRAPHIC_COLOR_GAMUT_DISPLAY_BT2020);
    ASSERT_EQ(GraphicColorGamut::GRAPHIC_COLOR_GAMUT_DISPLAY_BT2020, node->GetColorSpace());

    node->UpdateColorSpace(GraphicColorGamut::GRAPHIC_COLOR_GAMUT_DISPLAY_P3);
    ASSERT_EQ(GraphicColorGamut::GRAPHIC_COLOR_GAMUT_DISPLAY_BT2020, node->GetColorSpace());
}

/**
 * @tc.name: SetPixelFormat GetPixelFormat
 * @tc.desc: test results of SetPixelFormat GetPixelFormat
 * @tc.type:FUNC
 * @tc.require: issuesIB6QKS
 */
HWTEST_F(RSScreenRenderNodeTest, PixelFormatTest, TestSize.Level1)
{
    auto node = std::make_shared<RSScreenRenderNode>(id, 1, context);
    node->InitRenderParams();
    node->SetPixelFormat(GraphicPixelFormat::GRAPHIC_PIXEL_FMT_RGBA_8888);
    ASSERT_EQ(GraphicPixelFormat::GRAPHIC_PIXEL_FMT_RGBA_8888, node->GetPixelFormat());
    node->SetPixelFormat(GraphicPixelFormat::GRAPHIC_PIXEL_FMT_RGBA_1010102);
    ASSERT_EQ(GraphicPixelFormat::GRAPHIC_PIXEL_FMT_RGBA_1010102, node->GetPixelFormat());
}

/**
 * @tc.name: HdrStatusTest
 * @tc.desc: test results of CollectHdrStatus, GetDisplayHdrStatus, ResetDisplayHdrStatus
 * @tc.type: FUNC
 * @tc.require: issuesIBANP9
 */
HWTEST_F(RSScreenRenderNodeTest, HdrStatusTest, TestSize.Level1)
{
    auto screenNode = std::make_shared<RSScreenRenderNode>(id, 1, context);
    screenNode->CollectHdrStatus(HdrStatus::HDR_PHOTO);
    screenNode->CollectHdrStatus(HdrStatus::HDR_VIDEO);
    screenNode->CollectHdrStatus(HdrStatus::AI_HDR_VIDEO);
    EXPECT_EQ(screenNode->GetDisplayHdrStatus(), HdrStatus::HDR_PHOTO | HdrStatus::HDR_VIDEO |
        HdrStatus::AI_HDR_VIDEO);
}

/**
 * @tc.name: GetTargetSurfaceRenderNodeId
 * @tc.desc: test results of Set/GetTargetSurfaceRenderNodeId
 * @tc.type: FUNC
 * @tc.require: issuesIBIK1X
 */
HWTEST_F(RSScreenRenderNodeTest, GetTargetSurfaceRenderNodeId, TestSize.Level1)
{
    NodeId id = 1;
    auto screenNode = std::make_shared<RSScreenRenderNode>(id, 1, context);
    NodeId targetSurfaceRenderNodeId = 2;
    screenNode->SetTargetSurfaceRenderNodeId(targetSurfaceRenderNodeId);
    ASSERT_EQ(screenNode->GetTargetSurfaceRenderNodeId(), targetSurfaceRenderNodeId);
}

/**
 * @tc.name: SetForceFreeze
 * @tc.desc: test results of Set/GetForceFreeze
 * @tc.type: FUNC
 * @tc.require: issuesICPMFO
 */
HWTEST_F(RSScreenRenderNodeTest, SetForceFreeze, TestSize.Level1)
{
    NodeId id = 1;
    auto screenNode = std::make_shared<RSScreenRenderNode>(id, 1, context);
    ASSERT_NE(screenNode, nullptr);
    ASSERT_FALSE(screenNode->GetForceFreeze());

    screenNode->SetForceFreeze(false);
    ASSERT_FALSE(screenNode->GetForceFreeze());

    screenNode->stagingRenderParams_ = std::make_unique<RSScreenRenderParams>(screenNode->GetId());
    ASSERT_NE(screenNode->stagingRenderParams_, nullptr);
    screenNode->stagingRenderParams_->needSync_ = false;
    screenNode->SetForceFreeze(true);
    ASSERT_TRUE(screenNode->GetForceFreeze());

    screenNode->stagingRenderParams_->needSync_ = true;
    screenNode->SetForceFreeze(true);
    ASSERT_TRUE(screenNode->GetForceFreeze());
}
} // namespace OHOS::Rosen