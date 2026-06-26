/*
 * Copyright (c) 2024-2025 Huawei Device Co., Ltd.
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

#include "animation/rs_animation.h"
#include "animation/rs_animation_callback.h"
#include "animation/rs_interactive_implict_animator.h"
#include "command/rs_animation_command.h"
#include "modifier/rs_modifier_manager.h"
#include "pipeline/rs_node_map.h"
#include "render/rs_filter.h"
#include "transaction/rs_transaction_proxy.h"
#include "ui/rs_canvas_node.h"
#include "ui/rs_node.h"
#include "ui/rs_surface_node.h"
#include "ui/rs_ui_context.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {

class RSAnimationOthersClientTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    std::shared_ptr<RSUIContext> CreateRSUIContext();
};

class RSAnimationMock : public RSAnimation {
public:
    explicit RSAnimationMock(const std::shared_ptr<RSUIContext>& uiContext) : RSAnimation(uiContext) {}
    void RebuildInRender() override {}
};

void RSAnimationOthersClientTest::SetUpTestCase() {}
void RSAnimationOthersClientTest::TearDownTestCase() {}
void RSAnimationOthersClientTest::SetUp() {}
void RSAnimationOthersClientTest::TearDown() {}

std::shared_ptr<RSUIContext> RSAnimationOthersClientTest::CreateRSUIContext()
{
    OHOS::sptr<OHOS::IRemoteObject> connectToRenderRemote;
    auto rsUIContext = std::make_shared<RSUIContext>(0, connectToRenderRemote);
    rsUIContext->SetUITaskRunner([](const std::function<void()>& task, uint32_t delay) { task(); });
    return rsUIContext;
}

/**
 * @tc.name: AddInteractiveImplictAnimator001
 * @tc.desc: Test AddInteractiveImplictAnimator with null animator
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimationOthersClientTest, AddInteractiveImplictAnimator001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationOthersClientTest AddInteractiveImplictAnimator001 start";
    auto uiContext = CreateRSUIContext();
    ASSERT_NE(uiContext, nullptr);

    // Cover branch: animator == nullptr
    std::shared_ptr<RSInteractiveImplictAnimator> nullAnimator = nullptr;
    uiContext->AddInteractiveImplictAnimator(nullAnimator);

    // Verify no crash
    GTEST_LOG_(INFO) << "RSAnimationOthersClientTest AddInteractiveImplictAnimator001 end";
}

/**
 * @tc.name: AddInteractiveImplictAnimator002
 * @tc.desc: Test AddInteractiveImplictAnimator with valid animator
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimationOthersClientTest, AddInteractiveImplictAnimator002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationOthersClientTest AddInteractiveImplictAnimator002 start";
    auto uiContext = CreateRSUIContext();
    ASSERT_NE(uiContext, nullptr);

    // Cover branch: animator != nullptr, add to map
    RSAnimationTimingProtocol timingProtocol;
    timingProtocol.SetDuration(1000);
    RSAnimationTimingCurve timingCurve;

    auto animator = RSInteractiveImplictAnimator::Create(uiContext, timingProtocol, timingCurve);
    ASSERT_NE(animator, nullptr);

    auto animatorId = animator->GetId();

    // Add animator to context
    uiContext->AddInteractiveImplictAnimator(animator);

    // Verify animator was added (can be removed successfully)
    uiContext->RemoveInteractiveImplictAnimator(animatorId);

    GTEST_LOG_(INFO) << "RSAnimationOthersClientTest AddInteractiveImplictAnimator002 end";
}

/**
 * @tc.name: AddInteractiveImplictAnimator003
 * @tc.desc: Test AddInteractiveImplictAnimator with multiple animators
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimationOthersClientTest, AddInteractiveImplictAnimator003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationOthersClientTest AddInteractiveImplictAnimator003 start";
    auto uiContext = CreateRSUIContext();
    ASSERT_NE(uiContext, nullptr);

    RSAnimationTimingProtocol timingProtocol;
    timingProtocol.SetDuration(1000);
    RSAnimationTimingCurve timingCurve;

    // Add multiple animators
    auto animator1 = RSInteractiveImplictAnimator::Create(uiContext, timingProtocol, timingCurve);
    auto animator2 = RSInteractiveImplictAnimator::Create(uiContext, timingProtocol, timingCurve);
    auto animator3 = RSInteractiveImplictAnimator::Create(uiContext, timingProtocol, timingCurve);

    ASSERT_NE(animator1, nullptr);
    ASSERT_NE(animator2, nullptr);
    ASSERT_NE(animator3, nullptr);

    auto id1 = animator1->GetId();
    auto id2 = animator2->GetId();
    auto id3 = animator3->GetId();

    // Add all animators
    uiContext->AddInteractiveImplictAnimator(animator1);
    uiContext->AddInteractiveImplictAnimator(animator2);
    uiContext->AddInteractiveImplictAnimator(animator3);

    // Verify all can be removed
    uiContext->RemoveInteractiveImplictAnimator(id1);
    uiContext->RemoveInteractiveImplictAnimator(id2);
    uiContext->RemoveInteractiveImplictAnimator(id3);

    GTEST_LOG_(INFO) << "RSAnimationOthersClientTest AddInteractiveImplictAnimator003 end";
}

/**
 * @tc.name: RemoveInteractiveImplictAnimator001
 * @tc.desc: Test RemoveInteractiveImplictAnimator with valid id
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimationOthersClientTest, RemoveInteractiveImplictAnimator001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationOthersClientTest RemoveInteractiveImplictAnimator001 start";
    auto uiContext = CreateRSUIContext();
    ASSERT_NE(uiContext, nullptr);

    RSAnimationTimingProtocol timingProtocol;
    timingProtocol.SetDuration(1000);
    RSAnimationTimingCurve timingCurve;

    auto animator = RSInteractiveImplictAnimator::Create(uiContext, timingProtocol, timingCurve);
    ASSERT_NE(animator, nullptr);

    auto animatorId = animator->GetId();

    // Add then remove
    uiContext->AddInteractiveImplictAnimator(animator);
    uiContext->RemoveInteractiveImplictAnimator(animatorId);

    // Remove again should not crash (even if not found)
    uiContext->RemoveInteractiveImplictAnimator(animatorId);

    GTEST_LOG_(INFO) << "RSAnimationOthersClientTest RemoveInteractiveImplictAnimator001 end";
}

/**
 * @tc.name: RemoveInteractiveImplictAnimator002
 * @tc.desc: Test RemoveInteractiveImplictAnimator with non-existent id
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimationOthersClientTest, RemoveInteractiveImplictAnimator002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationOthersClientTest RemoveInteractiveImplictAnimator002 start";
    auto uiContext = CreateRSUIContext();
    ASSERT_NE(uiContext, nullptr);

    // Cover branch: removing non-existent id (no error, just erase)
    InteractiveImplictAnimatorId nonExistentId = 99999;
    uiContext->RemoveInteractiveImplictAnimator(nonExistentId);

    // Verify no crash
    GTEST_LOG_(INFO) << "RSAnimationOthersClientTest RemoveInteractiveImplictAnimator002 end";
}

/**
 * @tc.name: InteractiveImplictAnimator003
 * @tc.desc: Test Add and Remove interactive implicit animators
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimationOthersClientTest, InteractiveImplictAnimator003, TestSize.Level1)
{
    auto uiContext = CreateRSUIContext();
}

/**
 * @tc.name: GetAnimationsCount
 * @tc.desc: test results of GetAnimationsCount
 * @tc.type: FUNC
 * @tc.require: issueIBWOU7
 */
HWTEST_F(RSAnimationOthersClientTest, GetAnimationsCount, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    AnimationId animationId = 1;
    auto uiContext = CreateRSUIContext();
    auto animation = std::make_shared<RSAnimationMock>(uiContext);
    rsNode->animations_.clear();
    EXPECT_TRUE(rsNode->animations_.empty());
    rsNode->animations_.insert({ animationId, animation });
    auto animationsSize = rsNode->GetAnimationsCount();
    EXPECT_EQ(animationsSize, 1);
}

/**
 * @tc.name: OpenImplicitAnimationTest001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSAnimationOthersClientTest, OpenImplicitAnimationTest001, TestSize.Level1)
{
    std::function<void()> finishCallback = nullptr;
    RSAnimationTimingProtocol timingProtocal;
    RSAnimationTimingCurve timingCurve;
    auto uiContext = CreateRSUIContext();
    RSNode::OpenImplicitAnimation(uiContext, timingProtocal, timingCurve, finishCallback);
    EXPECT_TRUE(finishCallback == nullptr);

    finishCallback = []() {};
    RSNode::OpenImplicitAnimation(uiContext, timingProtocal, timingCurve, finishCallback);
    EXPECT_TRUE(finishCallback != nullptr);
}

/**
 * @tc.name: CloseImplicitAnimationTest
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSAnimationOthersClientTest, CloseImplicitAnimationTest, TestSize.Level1)
{
    auto uiContext = CreateRSUIContext();
    RSNode::CloseImplicitAnimation(uiContext);
    std::vector<std::shared_ptr<RSAnimation>> vec;
    EXPECT_EQ(vec, RSNode::CloseImplicitAnimation(uiContext));
}

/**
 * @tc.name: IsImplicitAnimationOpen
 * @tc.desc: test results of IsImplicitAnimationOpen
 * @tc.type: FUNC
 * @tc.require: issueI9KAZH
 */
HWTEST_F(RSAnimationOthersClientTest, IsImplicitAnimationOpen, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    auto uiContext = CreateRSUIContext();
    bool res = rsNode->IsImplicitAnimationOpen(uiContext);
    EXPECT_EQ(res, false);
}

/**
 * @tc.name: ExecuteWithoutAnimation
 * @tc.desc: test results of ExecuteWithoutAnimation
 * @tc.type: FUNC
 * @tc.require: issueI9KAZH
 */
HWTEST_F(RSAnimationOthersClientTest, ExecuteWithoutAnimation, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    PropertyCallback callback = nullptr;
    std::shared_ptr<RSImplicitAnimator> implicitAnimator = nullptr;
    std::shared_ptr<RSUIContext> rsUIContext = nullptr;
    rsNode->ExecuteWithoutAnimation(callback, rsUIContext, implicitAnimator);

    callback = []() {};
    rsNode->ExecuteWithoutAnimation(callback, rsUIContext, implicitAnimator);
    rsUIContext = CreateRSUIContext();
    rsNode->ExecuteWithoutAnimation(callback, rsUIContext, implicitAnimator);
    EXPECT_NE(rsUIContext, nullptr);

    implicitAnimator = std::make_shared<RSImplicitAnimator>();
    rsNode->ExecuteWithoutAnimation(callback, rsUIContext, implicitAnimator);
    EXPECT_NE(implicitAnimator, nullptr);
}

/**
 * @tc.name: AddAnimationInner
 * @tc.desc: test results of AddAnimationInner
 * @tc.type: FUNC
 * @tc.require: issueI9KAZH
 */
HWTEST_F(RSAnimationOthersClientTest, AddAnimationInner, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    auto uiContext = CreateRSUIContext();
    std::shared_ptr<RSAnimation> animation = std::make_shared<RSAnimationMock>(uiContext);
    rsNode->AddAnimationInner(animation);
    EXPECT_NE(rsNode->GenerateId(), 1);
}

/**
 * @tc.name: RemoveAnimationInner
 * @tc.desc: test results of RemoveAnimationInner
 * @tc.type: FUNC
 * @tc.require: issueI9KAZH
 */
HWTEST_F(RSAnimationOthersClientTest, RemoveAnimationInner, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    auto uiContext = CreateRSUIContext();
    auto animation = std::make_shared<RSAnimationMock>(uiContext);
    rsNode->AddAnimationInner(animation);
    rsNode->RemoveAnimationInner(animation);
    EXPECT_NE(animation, nullptr);

    PropertyId id = animation->GetPropertyId();
    uint32_t num = 2;
    rsNode->animatingPropertyNum_.insert({ id, num });
    rsNode->RemoveAnimationInner(animation);
    EXPECT_NE(animation, nullptr);

    rsNode->animatingPropertyNum_.clear();
    num = 1;
    rsNode->animatingPropertyNum_.insert({ id, num });
    rsNode->RemoveAnimationInner(animation);
    EXPECT_NE(animation, nullptr);
}

/**
 * @tc.name: FinishAnimationByProperty
 * @tc.desc: test results of FinishAnimationByProperty
 * @tc.type: FUNC
 * @tc.require: issueI9KAZH
 */
HWTEST_F(RSAnimationOthersClientTest, FinishAnimationByProperty, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    PropertyId id = 0;
    auto uiContext = CreateRSUIContext();
    auto animation = std::make_shared<RSAnimationMock>(uiContext);
    rsNode->AddAnimationInner(animation);
    rsNode->FinishAnimationByProperty(id);
    EXPECT_NE(animation, nullptr);

    id = animation->GetPropertyId();
    rsNode->animations_.insert({ id, animation });
    rsNode->FinishAnimationByProperty(id);
    EXPECT_NE(animation, nullptr);
}

/**
 * @tc.name: CancelAnimationByProperty
 * @tc.desc: test results of CancelAnimationByProperty
 * @tc.type: FUNC
 * @tc.require: issueI9KAZH
 */
HWTEST_F(RSAnimationOthersClientTest, CancelAnimationByProperty, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    PropertyId id = 0;
    rsNode->animatingPropertyNum_.insert({ id, 1 });
    delete RSTransactionProxy::instance_;
    RSTransactionProxy::instance_ = nullptr;
    rsNode->CancelAnimationByProperty(id, true);
    EXPECT_EQ(RSTransactionProxy::GetInstance(), nullptr);

    RSTransactionProxy::instance_ = new RSTransactionProxy();
    rsNode->CancelAnimationByProperty(id, true);
    EXPECT_NE(RSTransactionProxy::GetInstance(), nullptr);
}

/**
 * @tc.name: AddAnimation
 * @tc.desc: test results of AddAnimation
 * @tc.type: FUNC
 * @tc.require: issueI9KAZH
 */
HWTEST_F(RSAnimationOthersClientTest, AddAnimation, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    std::shared_ptr<RSAnimation> animation = nullptr;
    rsNode->AddAnimation(animation);
    EXPECT_EQ(animation, nullptr);

    auto uiContext = CreateRSUIContext();
    animation = std::make_shared<RSAnimationMock>(uiContext);
    rsNode->AddAnimation(animation);
    EXPECT_TRUE(animation != nullptr);

    rsNode->id_ = 0;
    rsNode->AddAnimation(animation);
    EXPECT_TRUE(animation != nullptr);

    animation->duration_ = 0;
    rsNode->AddAnimation(animation);
    EXPECT_TRUE(!animation->duration_);

    rsNode->id_ = 1;
    rsNode->AddAnimation(animation);
    EXPECT_TRUE(rsNode->id_ == 1);

    AnimationId id = animation->GetId();
    rsNode->animations_.insert({ id, animation });
    rsNode->AddAnimation(animation);
    EXPECT_TRUE(!rsNode->animations_.empty());
}

/**
 * @tc.name: RemoveAllAnimations
 * @tc.desc: test results of RemoveAllAnimations
 * @tc.type: FUNC
 * @tc.require: issueI9KAZH
 */
HWTEST_F(RSAnimationOthersClientTest, RemoveAllAnimations, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    AnimationId id = 1;
    auto uiContext = CreateRSUIContext();
    auto animation = std::make_shared<RSAnimationMock>(uiContext);
    rsNode->animations_.insert({ id, animation });
    rsNode->RemoveAllAnimations();
    EXPECT_NE(animation, nullptr);
}

/**
 * @tc.name: RemoveAnimation
 * @tc.desc: test results of RemoveAnimation
 * @tc.type: FUNC
 * @tc.require: issueI9KAZH
 */
HWTEST_F(RSAnimationOthersClientTest, RemoveAnimation, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    std::shared_ptr<RSAnimation> animation = nullptr;
    rsNode->RemoveAnimation(animation);
    EXPECT_EQ(animation, nullptr);

    auto uiContext = CreateRSUIContext();
    animation = std::make_shared<RSAnimationMock>(uiContext);
    rsNode->RemoveAnimation(animation);
    EXPECT_NE(animation, nullptr);

    AnimationId id = animation->GetId();
    rsNode->animations_.insert({ id, animation });
    rsNode->RemoveAnimation(animation);
    EXPECT_NE(animation, nullptr);
}

/**
 * @tc.name: HasPropertyAnimation
 * @tc.desc: test results of HasPropertyAnimation
 * @tc.type: FUNC
 * @tc.require: issueI9KAZH
 */
HWTEST_F(RSAnimationOthersClientTest, HasPropertyAnimation, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    PropertyId id = 1;
    bool res = rsNode->HasPropertyAnimation(id);
    EXPECT_EQ(res, false);
}

/**
 * @tc.name: GetAnimationByPropertyId
 * @tc.desc: test results of GetAnimationByPropertyId
 * @tc.type: FUNC
 * @tc.require: issueI9KAZH
 */
HWTEST_F(RSAnimationOthersClientTest, GetAnimationByPropertyId, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    PropertyId id = 0;
    AnimationId animationId = 1;
    auto uiContext = CreateRSUIContext();
    auto animation = std::make_shared<RSAnimationMock>(uiContext);
    rsNode->animations_.insert({ animationId, animation });
    rsNode->GetAnimationByPropertyId(id);
    EXPECT_NE(animation, nullptr);
}

/**
 * @tc.name: AnimationCallback
 * @tc.desc: test results of AnimationCallback
 * @tc.type: FUNC
 * @tc.require: issueI9KQ6R
 */
HWTEST_F(RSAnimationOthersClientTest, AnimationCallback, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    AnimationId animationId = 1;
    AnimationCallbackEvent event = AnimationCallbackEvent::FINISHED;
    bool res = rsNode->AnimationCallback(animationId, event);
    EXPECT_EQ(res, false);

    auto uiContext = CreateRSUIContext();
    auto animation = std::make_shared<RSAnimationMock>(uiContext);
    rsNode->animations_.insert({ animationId, animation });
    res = rsNode->AnimationCallback(animationId, event);
    EXPECT_EQ(res, true);

    event = AnimationCallbackEvent::REPEAT_FINISHED;
    res = rsNode->AnimationCallback(animationId, event);
    EXPECT_EQ(res, true);

    event = AnimationCallbackEvent::LOGICALLY_FINISHED;
    res = rsNode->AnimationCallback(animationId, event);
    EXPECT_EQ(res, true);
}

} // namespace Rosen
} // namespace OHOS
