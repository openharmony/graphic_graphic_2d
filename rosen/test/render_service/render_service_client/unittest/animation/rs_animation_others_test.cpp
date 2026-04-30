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

#include "animation/rs_interactive_implict_animator.h"
#include "ui/rs_ui_context.h"
#include "ui/rs_ui_context_manager.h"

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
};

void RSAnimationOthersClientTest::SetUpTestCase() {}
void RSAnimationOthersClientTest::TearDownTestCase() {}
void RSAnimationOthersClientTest::SetUp() {}
void RSAnimationOthersClientTest::TearDown() {}

/**
 * @tc.name: AddInteractiveImplictAnimator001
 * @tc.desc: Test AddInteractiveImplictAnimator with null animator
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimationOthersClientTest, AddInteractiveImplictAnimator001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationOthersClientTest AddInteractiveImplictAnimator001 start";
    OHOS::sptr<OHOS::IRemoteObject> connectToRenderRemote;
    auto uiContext = RSUIContextManager::MutableInstance().CreateRSUIContext(connectToRenderRemote);
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
    OHOS::sptr<OHOS::IRemoteObject> connectToRenderRemote;
    auto uiContext = RSUIContextManager::MutableInstance().CreateRSUIContext(connectToRenderRemote);
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
    OHOS::sptr<OHOS::IRemoteObject> connectToRenderRemote;
    auto uiContext = RSUIContextManager::MutableInstance().CreateRSUIContext(connectToRenderRemote);
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
    OHOS::sptr<OHOS::IRemoteObject> connectToRenderRemote;
    auto uiContext = RSUIContextManager::MutableInstance().CreateRSUIContext(connectToRenderRemote);
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
    OHOS::sptr<OHOS::IRemoteObject> connectToRenderRemote;
    auto uiContext = RSUIContextManager::MutableInstance().CreateRSUIContext(connectToRenderRemote);
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
    OHOS::sptr<OHOS::IRemoteObject> connectToRenderRemote;
    auto uiContext = RSUIContextManager::MutableInstance().CreateRSUIContext(connectToRenderRemote);
    ASSERT_NE(uiContext, nullptr);

    RSAnimationTimingProtocol timingProtocol;
    timingProtocol.SetDuration(1000);
    RSAnimationTimingCurve timingCurve;

    // Create group animator
    auto groupAnimatorWeak = RSInteractiveImplictAnimator::CreateGroup(uiContext, timingProtocol, timingCurve);
    auto groupAnimator = groupAnimatorWeak.lock();
    ASSERT_NE(groupAnimator, nullptr);
    EXPECT_TRUE(groupAnimator->isGroupAnimator_);

    auto groupId = groupAnimator->GetId();
    uiContext->AddInteractiveImplictAnimator(groupAnimator);

    // Create regular animators
    auto animator1 = RSInteractiveImplictAnimator::Create(uiContext, timingProtocol, timingCurve);
    auto animator2 = RSInteractiveImplictAnimator::Create(uiContext, timingProtocol, timingCurve);

    ASSERT_NE(animator1, nullptr);
    ASSERT_NE(animator2, nullptr);

    auto id1 = animator1->GetId();
    auto id2 = animator2->GetId();

    uiContext->AddInteractiveImplictAnimator(animator1);
    uiContext->AddInteractiveImplictAnimator(animator2);

    // Remove all animators
    uiContext->RemoveInteractiveImplictAnimator(groupId);
    uiContext->RemoveInteractiveImplictAnimator(id1);
    uiContext->RemoveInteractiveImplictAnimator(id2);
}

} // namespace Rosen
} // namespace OHOS
