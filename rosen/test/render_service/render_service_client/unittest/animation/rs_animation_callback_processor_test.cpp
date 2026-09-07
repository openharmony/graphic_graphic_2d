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

#include "animation/rs_animation.h"
#include "animation/rs_animation_callback.h"
#include "pipeline/rs_node_map.h"
#include "ui/rs_canvas_node.h"
#include "ui/rs_node.h"
#include "ui/rs_root_node.h"
#include "ui/rs_ui_context.h"
#include "ui/rs_ui_context_manager.h"
#include "ui/rs_ui_director.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {

class RSAnimationCallbackProcessorTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

class RSAnimationMock : public RSAnimation {
public:
    explicit RSAnimationMock(const std::shared_ptr<RSUIContext>& uiContext) : RSAnimation(uiContext) {}
    void RebuildInRender() override {}
};

void RSAnimationCallbackProcessorTest::SetUpTestCase() {}
void RSAnimationCallbackProcessorTest::TearDownTestCase() {}
void RSAnimationCallbackProcessorTest::SetUp() {}
void RSAnimationCallbackProcessorTest::TearDown() {}

/**
 * @tc.name: AnimationCallbackProcessor
 * @tc.desc: Verify AnimationCallbackProcessor with no matching context
 * @tc.type:FUNC
 */
HWTEST_F(RSAnimationCallbackProcessorTest, AnimationCallbackProcessor, TestSize.Level1)
{
    NodeId nodeId = 0;
    AnimationId animId = 0;
    uint64_t token = 0;
    AnimationCallbackEvent event = AnimationCallbackEvent::REPEAT_FINISHED;
    RSUIDirector::AnimationCallbackProcessor(nodeId, animId, token, event);
    EXPECT_EQ(nodeId, 0u);
}

/**
 * @tc.name: AnimationCallbackProcessorTest001
 * @tc.desc: test AnimationCallbackProcessor with node and animation on context
 * @tc.type:FUNC
 */
HWTEST_F(RSAnimationCallbackProcessorTest, AnimationCallbackProcessorTest001, TestSize.Level1)
{
    OHOS::sptr<OHOS::IRemoteObject> connectToRenderRemote;
    uint64_t token = 0;
    auto rsUIContext = std::make_shared<RSUIContext>(token, connectToRenderRemote);
    RSUIContextManager::MutableInstance().rsUIContextMap_[token] = rsUIContext;

    auto node = std::make_shared<RSRootNode>(false);
    NodeId nodeId = node->GetId();
    rsUIContext->GetMutableNodeMap().RegisterNode(node);

    AnimationCallbackEvent event = AnimationCallbackEvent::REPEAT_FINISHED;
    RSUIDirector::AnimationCallbackProcessor(nodeId, 0, token, event);

    auto animation = std::make_shared<RSAnimationMock>(rsUIContext);
    AnimationId animId = animation->GetId();
    node->animations_.emplace(animId, animation);
    RSUIDirector::AnimationCallbackProcessor(nodeId, animId, token, event);
    EXPECT_EQ(animation->state_, RSAnimation::AnimationState::INITIALIZED);

    RSUIDirector::AnimationCallbackProcessor(nodeId, animId, token, AnimationCallbackEvent::FINISHED);
    EXPECT_EQ(animation->state_, RSAnimation::AnimationState::FINISHED);

    RSUIContextManager::MutableInstance().rsUIContextMap_.erase(token);
}

/**
 * @tc.name: AnimationCallbackProcessorFallbackNodeFound
 * @tc.desc: Valid token, node not in self context, found in another context with animation,
 *           L957 true + L958 true, should execute callback and return
 * @tc.type:FUNC
 */
HWTEST_F(RSAnimationCallbackProcessorTest, AnimationCallbackProcessorFallbackNodeFound, TestSize.Level1)
{
    OHOS::sptr<OHOS::IRemoteObject> connectToRenderRemote;
    uint64_t selfToken = 100;
    auto selfContext = std::make_shared<RSUIContext>(selfToken, connectToRenderRemote);
    RSUIContextManager::MutableInstance().rsUIContextMap_[selfToken] = selfContext;
    RSUIContextManager::MutableInstance().isMultiInstanceOpen_ = true;

    auto otherContext = std::make_shared<RSUIContext>(999, connectToRenderRemote);
    uint64_t otherToken = 999;
    RSUIContextManager::MutableInstance().rsUIContextMap_[otherToken] = otherContext;

    auto node = std::make_shared<RSRootNode>(false);
    NodeId nodeId = node->GetId();
    otherContext->GetMutableNodeMap().RegisterNode(node);

    auto animation = std::make_shared<RSAnimationMock>(otherContext);
    AnimationId animId = animation->GetId();
    node->animations_.emplace(animId, animation);

    AnimationCallbackEvent event = AnimationCallbackEvent::FINISHED;
    RSUIDirector::AnimationCallbackProcessor(nodeId, animId, selfToken, event);
    EXPECT_EQ(animation->state_, RSAnimation::AnimationState::FINISHED);

    RSUIContextManager::MutableInstance().rsUIContextMap_.erase(otherToken);
    RSUIContextManager::MutableInstance().rsUIContextMap_.erase(selfToken);
    RSUIContextManager::MutableInstance().isMultiInstanceOpen_ = false;
}

/**
 * @tc.name: AnimationCallbackProcessorFallbackNodeNotFoundContextCallback
 * @tc.desc: Valid token, node not found in any context, but another context has the animation,
 *           L957 false for all + L968 true, should execute context-level AnimationCallback and return
 * @tc.type:FUNC
 */
HWTEST_F(RSAnimationCallbackProcessorTest, AnimationCallbackProcessorFallbackNodeNotFoundContextCallback,
    TestSize.Level1)
{
    OHOS::sptr<OHOS::IRemoteObject> connectToRenderRemote;
    uint64_t selfToken = 100;
    auto selfContext = std::make_shared<RSUIContext>(selfToken, connectToRenderRemote);
    RSUIContextManager::MutableInstance().rsUIContextMap_[selfToken] = selfContext;
    RSUIContextManager::MutableInstance().isMultiInstanceOpen_ = true;

    auto otherContext = std::make_shared<RSUIContext>(999, connectToRenderRemote);
    uint64_t otherToken = 999;
    RSUIContextManager::MutableInstance().rsUIContextMap_[otherToken] = otherContext;

    auto animation = std::make_shared<RSAnimationMock>(otherContext);
    AnimationId animId = animation->GetId();
    otherContext->animations_.emplace(animId, animation);

    NodeId nodeId = 99999;
    AnimationCallbackEvent event = AnimationCallbackEvent::FINISHED;
    RSUIDirector::AnimationCallbackProcessor(nodeId, animId, selfToken, event);
    EXPECT_EQ(animation->state_, RSAnimation::AnimationState::FINISHED);

    RSUIContextManager::MutableInstance().rsUIContextMap_.erase(otherToken);
    RSUIContextManager::MutableInstance().rsUIContextMap_.erase(selfToken);
    RSUIContextManager::MutableInstance().isMultiInstanceOpen_ = false;
}

/**
 * @tc.name: AnimationCallbackProcessorFallbackAllFail
 * @tc.desc: Valid token, node not found in any context, no context has the animation,
 *           should fall through to fallback node
 * @tc.type:FUNC
 */
HWTEST_F(RSAnimationCallbackProcessorTest, AnimationCallbackProcessorFallbackAllFail, TestSize.Level1)
{
    OHOS::sptr<OHOS::IRemoteObject> connectToRenderRemote;
    uint64_t selfToken = 100;
    auto selfContext = std::make_shared<RSUIContext>(selfToken, connectToRenderRemote);
    RSUIContextManager::MutableInstance().rsUIContextMap_[selfToken] = selfContext;
    RSUIContextManager::MutableInstance().isMultiInstanceOpen_ = true;

    auto otherContext = std::make_shared<RSUIContext>(999, connectToRenderRemote);
    uint64_t otherToken = 999;
    RSUIContextManager::MutableInstance().rsUIContextMap_[otherToken] = otherContext;

    NodeId nodeId = 99999;
    AnimationId animId = 88888;
    AnimationCallbackEvent event = AnimationCallbackEvent::FINISHED;
    RSUIDirector::AnimationCallbackProcessor(nodeId, animId, selfToken, event);
    EXPECT_TRUE(otherContext->animations_.empty());

    RSUIContextManager::MutableInstance().rsUIContextMap_.erase(otherToken);
    RSUIContextManager::MutableInstance().rsUIContextMap_.erase(selfToken);
    RSUIContextManager::MutableInstance().isMultiInstanceOpen_ = false;
}

/**
 * @tc.name: AnimationCallbackProcessorFallbackNodeAnimationFail
 * @tc.desc: Valid token, node found in another context but animation not on that node,
 *           L957 true + L958 false, nodeFound=true so return directly without context-level callback
 * @tc.type:FUNC
 */
HWTEST_F(RSAnimationCallbackProcessorTest, AnimationCallbackProcessorFallbackNodeAnimationFail, TestSize.Level1)
{
    OHOS::sptr<OHOS::IRemoteObject> connectToRenderRemote;
    uint64_t selfToken = 100;
    auto selfContext = std::make_shared<RSUIContext>(selfToken, connectToRenderRemote);
    RSUIContextManager::MutableInstance().rsUIContextMap_[selfToken] = selfContext;
    RSUIContextManager::MutableInstance().isMultiInstanceOpen_ = true;

    auto otherContext = std::make_shared<RSUIContext>(999, connectToRenderRemote);
    uint64_t otherToken = 999;
    RSUIContextManager::MutableInstance().rsUIContextMap_[otherToken] = otherContext;

    auto node = std::make_shared<RSRootNode>(false);
    NodeId nodeId = node->GetId();
    otherContext->GetMutableNodeMap().RegisterNode(node);

    auto otherAnimation = std::make_shared<RSAnimationMock>(otherContext);
    AnimationId otherAnimId = otherAnimation->GetId();
    otherContext->animations_.emplace(otherAnimId, otherAnimation);

    AnimationId animId = 88888;
    AnimationCallbackEvent event = AnimationCallbackEvent::FINISHED;
    RSUIDirector::AnimationCallbackProcessor(nodeId, animId, selfToken, event);
    EXPECT_EQ(otherAnimation->state_, RSAnimation::AnimationState::INITIALIZED);

    RSUIContextManager::MutableInstance().rsUIContextMap_.erase(otherToken);
    RSUIContextManager::MutableInstance().rsUIContextMap_.erase(selfToken);
    RSUIContextManager::MutableInstance().isMultiInstanceOpen_ = false;
}

} // namespace Rosen
} // namespace OHOS
