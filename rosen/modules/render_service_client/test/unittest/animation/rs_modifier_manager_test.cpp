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

#include "animation/rs_render_animation.h"
#include "modifier/rs_modifier_manager.h"
#include "ui/rs_canvas_node.h"
#include "ui/rs_ui_context.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {

class RSRenderAnimationMock : public RSRenderAnimation {
public:
    RSRenderAnimationMock() : RSRenderAnimation() {}
    explicit RSRenderAnimationMock(AnimationId id) : RSRenderAnimation(id) {}
    ~RSRenderAnimationMock() override = default;
    void RebuildPropertyValue(float fraction) override {}
};

class RSModifierManagerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    static constexpr uint64_t NODE_ID = 67890;
    static constexpr uint64_t ANIMATION_ID = 12345;
};

void RSModifierManagerTest::SetUpTestCase() {}
void RSModifierManagerTest::TearDownTestCase() {}
void RSModifierManagerTest::SetUp() {}
void RSModifierManagerTest::TearDown() {}

/**
 * @tc.name: GetNodeTreeStatus_NullContext001
 * @tc.desc: Verify GetNodeTreeStatus returns true when rsUIContext is null.
 *           This is the conservative behavior to avoid affecting normal animations.
 * @tc.type:FUNC
 */
HWTEST_F(RSModifierManagerTest, GetNodeTreeStatus_NullContext001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSModifierManagerTest GetNodeTreeStatus_NullContext001 start";

    RSModifierManager manager;
    bool result = manager.GetNodeTreeStatus(NODE_ID);

    EXPECT_TRUE(result);

    GTEST_LOG_(INFO) << "RSModifierManagerTest GetNodeTreeStatus_NullContext001 end";
}

/**
 * @tc.name: GetNodeTreeStatus_NullNode001
 * @tc.desc: Verify GetNodeTreeStatus returns false when node is not found.
 *           Node not found means node has been destructed, treat as off-tree for power optimization.
 * @tc.type:FUNC
 */
HWTEST_F(RSModifierManagerTest, GetNodeTreeStatus_NullNode001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSModifierManagerTest GetNodeTreeStatus_NullNode001 start";

    OHOS::sptr<OHOS::IRemoteObject> connectToRenderRemote;
    auto rsUIContext = std::make_shared<RSUIContext>(0, connectToRenderRemote);
    RSModifierManager manager;
    manager.SetRSUIContext(rsUIContext);

    bool result = manager.GetNodeTreeStatus(NODE_ID);

    EXPECT_FALSE(result);

    GTEST_LOG_(INFO) << "RSModifierManagerTest GetNodeTreeStatus_NullNode001 end";
}

/**
 * @tc.name: GetNodeTreeStatus_NodeOnTree001
 * @tc.desc: Verify GetNodeTreeStatus returns true when node is on the tree.
 *           Test case where node exists and GetIsOnTheTree() returns true.
 * @tc.type:FUNC
 */
HWTEST_F(RSModifierManagerTest, GetNodeTreeStatus_NodeOnTree001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSModifierManagerTest GetNodeTreeStatus_NodeOnTree001 start";

    OHOS::sptr<OHOS::IRemoteObject> connectToRenderRemote;
    auto rsUIContext = std::make_shared<RSUIContext>(0, connectToRenderRemote);
    RSModifierManager manager;
    manager.SetRSUIContext(rsUIContext);

    auto node = RSCanvasNode::Create(false, false, rsUIContext);
    node->SetIsOnTheTree(true);
    rsUIContext->GetMutableNodeMap().RegisterNode(node);

    bool result = manager.GetNodeTreeStatus(node->GetId());

    EXPECT_TRUE(result);

    GTEST_LOG_(INFO) << "RSModifierManagerTest GetNodeTreeStatus_NodeOnTree001 end";
}

/**
 * @tc.name: GetNodeTreeStatus_NodeOffTree001
 * @tc.desc: Verify GetNodeTreeStatus returns false when node is not on the tree.
 *           Test case where node exists but GetIsOnTheTree() returns false.
 * @tc.type:FUNC
 */
HWTEST_F(RSModifierManagerTest, GetNodeTreeStatus_NodeOffTree001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSModifierManagerTest GetNodeTreeStatus_NodeOffTree001 start";

    OHOS::sptr<OHOS::IRemoteObject> connectToRenderRemote;
    auto rsUIContext = std::make_shared<RSUIContext>(0, connectToRenderRemote);
    RSModifierManager manager;
    manager.SetRSUIContext(rsUIContext);

    auto node = RSCanvasNode::Create(false, false, rsUIContext);
    node->SetIsOnTheTree(false);
    rsUIContext->GetMutableNodeMap().RegisterNode(node);

    bool result = manager.GetNodeTreeStatus(node->GetId());

    EXPECT_FALSE(result);

    GTEST_LOG_(INFO) << "RSModifierManagerTest GetNodeTreeStatus_NodeOffTree001 end";
}

/**
 * @tc.name: RequestDelayedVSync_NullContext001
 * @tc.desc: Verify RequestDelayedVSync when rsUIContext is null.
 *           Branch: if (auto rsUIContext = rsUIContext_.lock()) -> false, hasRunningAnimation remains true.
 * @tc.type:FUNC
 */
HWTEST_F(RSModifierManagerTest, RequestDelayedVSync_NullContext001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSModifierManagerTest RequestDelayedVSync_NullContext001 start";

    RSModifierManager manager;  // No context set
    bool hasRunningAnimation = true;
    int64_t minLeftDelayTime = 100;  // > 17, passes first check

    manager.RequestDelayedVSync(minLeftDelayTime, hasRunningAnimation);

    EXPECT_TRUE(hasRunningAnimation);  // Context null, hasRunningAnimation remains true

    GTEST_LOG_(INFO) << "RSModifierManagerTest RequestDelayedVSync_NullContext001 end";
}

/**
 * @tc.name: RequestDelayedVSync_NoRunningAnimation001
 * @tc.desc: Verify RequestDelayedVSync returns early when hasRunningAnimation is false.
 *           Branch: if (!hasRunningAnimation || ...)
 * @tc.type:FUNC
 */
HWTEST_F(RSModifierManagerTest, RequestDelayedVSync_NoRunningAnimation001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSModifierManagerTest RequestDelayedVSync_NoRunningAnimation001 start";

    RSModifierManager manager;
    bool hasRunningAnimation = false;
    int64_t minLeftDelayTime = 100;

    manager.RequestDelayedVSync(minLeftDelayTime, hasRunningAnimation);

    EXPECT_FALSE(hasRunningAnimation);

    GTEST_LOG_(INFO) << "RSModifierManagerTest RequestDelayedVSync_NoRunningAnimation001 end";
}

/**
 * @tc.name: RequestDelayedVSync_SmallDelay001
 * @tc.desc: Verify RequestDelayedVSync returns early when minLeftDelayTime <= 17ms.
 *           Branch: if (... || minLeftDelayTime <= oneFrameTimeInFPS60 || ...)
 * @tc.type:FUNC
 */
HWTEST_F(RSModifierManagerTest, RequestDelayedVSync_SmallDelay001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSModifierManagerTest RequestDelayedVSync_SmallDelay001 start";

    RSModifierManager manager;
    bool hasRunningAnimation = true;
    int64_t minLeftDelayTime = 15;

    manager.RequestDelayedVSync(minLeftDelayTime, hasRunningAnimation);

    EXPECT_TRUE(hasRunningAnimation);

    GTEST_LOG_(INFO) << "RSModifierManagerTest RequestDelayedVSync_SmallDelay001 end";
}

/**
 * @tc.name: RequestDelayedVSync_MaxValue001
 * @tc.desc: Verify RequestDelayedVSync returns early when minLeftDelayTime >= INT64_MAX.
 *           Branch: if (... || minLeftDelayTime >= INT64_MAX)
 * @tc.type:FUNC
 */
HWTEST_F(RSModifierManagerTest, RequestDelayedVSync_MaxValue001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSModifierManagerTest RequestDelayedVSync_MaxValue001 start";

    RSModifierManager manager;
    bool hasRunningAnimation = true;
    int64_t minLeftDelayTime = INT64_MAX;

    manager.RequestDelayedVSync(minLeftDelayTime, hasRunningAnimation);

    EXPECT_TRUE(hasRunningAnimation);

    GTEST_LOG_(INFO) << "RSModifierManagerTest RequestDelayedVSync_MaxValue001 end";
}

/**
 * @tc.name: RequestDelayedVSync_NormalDelay001
 * @tc.desc: Verify RequestDelayedVSync handles normal delay correctly.
 *           Branch: valid parameters (100ms delay)
 * @tc.type:FUNC
 */
HWTEST_F(RSModifierManagerTest, RequestDelayedVSync_NormalDelay001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSModifierManagerTest RequestDelayedVSync_NormalDelay001 start";

    OHOS::sptr<OHOS::IRemoteObject> connectToRenderRemote;
    auto rsUIContext = std::make_shared<RSUIContext>(0, connectToRenderRemote);
    RSModifierManager manager;
    manager.SetRSUIContext(rsUIContext);

    bool hasRunningAnimation = true;
    int64_t minLeftDelayTime = 100;

    manager.RequestDelayedVSync(minLeftDelayTime, hasRunningAnimation);

    EXPECT_FALSE(hasRunningAnimation);

    GTEST_LOG_(INFO) << "RSModifierManagerTest RequestDelayedVSync_NormalDelay001 end";
}

/**
 * @tc.name: RequestDelayedVSync_LargeDelay001
 * @tc.desc: Verify RequestDelayedVSync clamps delay to max 60000ms.
 *           Branch: delayTimeMs exceeds maxDelayTimeMs
 * @tc.type:FUNC
 */
HWTEST_F(RSModifierManagerTest, RequestDelayedVSync_LargeDelay001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSModifierManagerTest RequestDelayedVSync_LargeDelay001 start";

    OHOS::sptr<OHOS::IRemoteObject> connectToRenderRemote;
    auto rsUIContext = std::make_shared<RSUIContext>(0, connectToRenderRemote);
    RSModifierManager manager;
    manager.SetRSUIContext(rsUIContext);

    bool hasRunningAnimation = true;
    int64_t minLeftDelayTime = 70000;

    manager.RequestDelayedVSync(minLeftDelayTime, hasRunningAnimation);

    EXPECT_FALSE(hasRunningAnimation);

    GTEST_LOG_(INFO) << "RSModifierManagerTest RequestDelayedVSync_LargeDelay001 end";
}

/**
 * @tc.name: RequestDelayedVSync_ExactOneFrame001
 * @tc.desc: Verify RequestDelayedVSync at boundary value (exactly 17ms).
 *           Branch: minLeftDelayTime == oneFrameTimeInFPS60
 * @tc.type:FUNC
 */
HWTEST_F(RSModifierManagerTest, RequestDelayedVSync_ExactOneFrame001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSModifierManagerTest RequestDelayedVSync_ExactOneFrame001 start";

    RSModifierManager manager;
    bool hasRunningAnimation = true;
    int64_t minLeftDelayTime = 17;

    manager.RequestDelayedVSync(minLeftDelayTime, hasRunningAnimation);

    EXPECT_TRUE(hasRunningAnimation);

    GTEST_LOG_(INFO) << "RSModifierManagerTest RequestDelayedVSync_ExactOneFrame001 end";
}

/**
 * @tc.name: HasUIRunningAnimation_NoAnimations001
 * @tc.desc: Verify HasUIRunningAnimation returns false when no animations.
 *           Branch: animations_ is empty
 * @tc.type:FUNC
 */
HWTEST_F(RSModifierManagerTest, HasUIRunningAnimation_NoAnimations001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSModifierManagerTest HasUIRunningAnimation_NoAnimations001 start";

    RSModifierManager manager;

    bool result = manager.HasUIRunningAnimation();

    EXPECT_FALSE(result);

    GTEST_LOG_(INFO) << "RSModifierManagerTest HasUIRunningAnimation_NoAnimations001 end";
}

/**
 * @tc.name: HasUIRunningAnimation_AnimationExpired001
 * @tc.desc: Verify HasUIRunningAnimation handles expired animation correctly.
 *           Animation weak_ptr lock returns nullptr.
 *           Branch: if (auto animation = iter.second.lock(); ...) -> animation = nullptr (line 87)
 * @tc.type:FUNC
 */
HWTEST_F(RSModifierManagerTest, HasUIRunningAnimation_AnimationExpired001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSModifierManagerTest HasUIRunningAnimation_AnimationExpired001 start";

    RSModifierManager manager;
    
    // Add an animation and let it expire (weak_ptr becomes nullptr)
    auto animation = std::make_shared<RSRenderAnimationMock>(ANIMATION_ID);
    manager.AddAnimation(animation);
    animation.reset();  // Destroy animation, weak_ptr in manager.lock() returns nullptr

    bool result = manager.HasUIRunningAnimation();

    EXPECT_FALSE(result);  // Animation expired, not running

    GTEST_LOG_(INFO) << "RSModifierManagerTest HasUIRunningAnimation_AnimationExpired001 end";
}

/**
 * @tc.name: HasUIRunningAnimation_AnimationNotRunning001
 * @tc.desc: Verify HasUIRunningAnimation returns false when animation exists but not running.
 *           Animation exists but IsRunning() returns false.
 *           Branch: if (... && animation->IsRunning()) -> false (line 87)
 * @tc.type:FUNC
 */
HWTEST_F(RSModifierManagerTest, HasUIRunningAnimation_AnimationNotRunning001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSModifierManagerTest HasUIRunningAnimation_AnimationNotRunning001 start";

    RSModifierManager manager;
    
    auto animation = std::make_shared<RSRenderAnimationMock>(ANIMATION_ID);
    animation->Finish();  // Animation is finished, not running
    manager.AddAnimation(animation);

    bool result = manager.HasUIRunningAnimation();

    EXPECT_FALSE(result);  // Animation exists but not running

    GTEST_LOG_(INFO) << "RSModifierManagerTest HasUIRunningAnimation_AnimationNotRunning001 end";
}

/**
 * @tc.name: HasUIRunningAnimation_AnimationRunning001
 * @tc.desc: Verify HasUIRunningAnimation returns true when animation is running.
 *           Animation exists and IsRunning() returns true.
 *           Branch: if (... && animation->IsRunning()) -> true (line 87)
 * @tc.type:FUNC
 */
HWTEST_F(RSModifierManagerTest, HasUIRunningAnimation_AnimationRunning001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSModifierManagerTest HasUIRunningAnimation_AnimationRunning001 start";

    system("param set rosen.animationdelay.optimize.enabled 0");  // Optimization disabled

    RSModifierManager manager;
    
    auto animation = std::make_shared<RSRenderAnimationMock>(ANIMATION_ID);
    animation->Start();
    manager.AddAnimation(animation);

    bool result = manager.HasUIRunningAnimation();

    EXPECT_TRUE(result);  // Optimization disabled -> return true (line 89)

    system("param set rosen.animationdelay.optimize.enabled 1");  // Restore default

    GTEST_LOG_(INFO) << "RSModifierManagerTest HasUIRunningAnimation_AnimationRunning001 end";
}

/**
 * @tc.name: HasUIRunningAnimation_OptimizationEnabled_OnTree001
 * @tc.desc: Verify HasUIRunningAnimation with optimization enabled and node on tree.
 *           Branch: GetAnimationDelayOptimizeEnabled() = true (line 88) + GetNodeTreeStatus() = true (line 91)
 * @tc.type:FUNC
 */
HWTEST_F(RSModifierManagerTest, HasUIRunningAnimation_OptimizationEnabled_OnTree001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSModifierManagerTest HasUIRunningAnimation_OptimizationEnabled_OnTree001 start";

    OHOS::sptr<OHOS::IRemoteObject> connectToRenderRemote;
    auto rsUIContext = std::make_shared<RSUIContext>(0, connectToRenderRemote);
    RSModifierManager manager;
    manager.SetRSUIContext(rsUIContext);

    auto node = RSCanvasNode::Create(false, false, rsUIContext);
    node->SetIsOnTheTree(true);
    rsUIContext->GetMutableNodeMap().RegisterNode(node);

    auto animation = std::make_shared<RSRenderAnimationMock>(ANIMATION_ID);
    animation->targetId_ = node->GetId();
    animation->Start();
    manager.AddAnimation(animation);

    bool result = manager.HasUIRunningAnimation();

    EXPECT_TRUE(result);

    GTEST_LOG_(INFO) << "RSModifierManagerTest HasUIRunningAnimation_OptimizationEnabled_OnTree001 end";
}

/**
 * @tc.name: HasUIRunningAnimation_OptimizationEnabled_OffTree001
 * @tc.desc: Verify HasUIRunningAnimation with optimization enabled and node off tree.
 *           Branch: GetAnimationDelayOptimizeEnabled() = true (line 88) + GetNodeTreeStatus() = false (line 91 skip)
 * @tc.type:FUNC
 */
HWTEST_F(RSModifierManagerTest, HasUIRunningAnimation_OptimizationEnabled_OffTree001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSModifierManagerTest HasUIRunningAnimation_OptimizationEnabled_OffTree001 start";

    OHOS::sptr<OHOS::IRemoteObject> connectToRenderRemote;
    auto rsUIContext = std::make_shared<RSUIContext>(0, connectToRenderRemote);
    RSModifierManager manager;
    manager.SetRSUIContext(rsUIContext);

    auto node = RSCanvasNode::Create(false, false, rsUIContext);
    node->SetIsOnTheTree(false);
    rsUIContext->GetMutableNodeMap().RegisterNode(node);

    auto animation = std::make_shared<RSRenderAnimationMock>(ANIMATION_ID);
    animation->targetId_ = node->GetId();
    animation->Start();
    manager.AddAnimation(animation);

    bool result = manager.HasUIRunningAnimation();

    EXPECT_FALSE(result);

    GTEST_LOG_(INFO) << "RSModifierManagerTest HasUIRunningAnimation_OptimizationEnabled_OffTree001 end";
}

/**
 * @tc.name: RequestDelayedVSync_CallbackExecuted001
 * @tc.desc: Verify RequestDelayedVSync PostDelayTask callback is executed after delay time.
 *           Use mock TaskRunner to capture and execute the delayed task.
 *           Branch: PostDelayTask lambda -> if (auto context = rsUIContext_.lock()) -> true
 * @tc.type:FUNC
 */
HWTEST_F(RSModifierManagerTest, RequestDelayedVSync_CallbackExecuted001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSModifierManagerTest RequestDelayedVSync_CallbackExecuted001 start";

    OHOS::sptr<OHOS::IRemoteObject> connectToRenderRemote;
    auto rsUIContext = std::make_shared<RSUIContext>(0, connectToRenderRemote);
    RSModifierManager manager;
    manager.SetRSUIContext(rsUIContext);

    bool vsyncCallbackCalled = false;
    rsUIContext->SetRequestVsyncCallback([&vsyncCallbackCalled]() {
        vsyncCallbackCalled = true;
    });

    std::function<void()> capturedTask;
    rsUIContext->SetUITaskRunner([&capturedTask](const std::function<void()>& task, uint32_t delay) {
        capturedTask = task;
    });

    bool hasRunningAnimation = true;
    int64_t minLeftDelayTime = 100;

    manager.RequestDelayedVSync(minLeftDelayTime, hasRunningAnimation);
    EXPECT_FALSE(hasRunningAnimation);

    ASSERT_TRUE(capturedTask);
    capturedTask();

    EXPECT_TRUE(vsyncCallbackCalled);

    GTEST_LOG_(INFO) << "RSModifierManagerTest RequestDelayedVSync_CallbackExecuted001 end";
}

/**
 * @tc.name: RequestDelayedVSync_ContextDestroyed001
 * @tc.desc: Verify RequestDelayedVSync lambda handles destroyed context correctly.
 *           Context is destroyed before the delayed callback executes.
 *           Branch: PostDelayTask lambda -> if (auto context = rsUIContext_.lock()) -> false (line 192)
 * @tc.type:FUNC
 */
HWTEST_F(RSModifierManagerTest, RequestDelayedVSync_ContextDestroyed001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSModifierManagerTest RequestDelayedVSync_ContextDestroyed001 start";

    OHOS::sptr<OHOS::IRemoteObject> connectToRenderRemote;
    auto rsUIContext = std::make_shared<RSUIContext>(0, connectToRenderRemote);
    RSModifierManager manager;
    manager.SetRSUIContext(rsUIContext);

    bool vsyncCallbackCalled = false;
    rsUIContext->SetRequestVsyncCallback([&vsyncCallbackCalled]() {
        vsyncCallbackCalled = true;
    });

    std::function<void()> capturedTask;
    rsUIContext->SetUITaskRunner([&capturedTask](const std::function<void()>& task, uint32_t delay) {
        capturedTask = task;
    });

    bool hasRunningAnimation = true;
    int64_t minLeftDelayTime = 100;

    manager.RequestDelayedVSync(minLeftDelayTime, hasRunningAnimation);
    EXPECT_FALSE(hasRunningAnimation);

    rsUIContext.reset();

    ASSERT_TRUE(capturedTask);
    capturedTask();

    EXPECT_FALSE(vsyncCallbackCalled);

    GTEST_LOG_(INFO) << "RSModifierManagerTest RequestDelayedVSync_ContextDestroyed001 end";
}

} // namespace Rosen
} // namespace OHOS
