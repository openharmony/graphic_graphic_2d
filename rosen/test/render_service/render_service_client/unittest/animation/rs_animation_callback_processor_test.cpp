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
 * @tc.desc: Verify AnimationCallbackProcessor
 * @tc.type:FUNC
 */
HWTEST_F(RSAnimationCallbackProcessorTest, AnimationCallbackProcessor, TestSize.Level1)
{
    OHOS::sptr<OHOS::IRemoteObject> connectToRenderRemote;
    auto rsUIContext = std::make_shared<RSUIContext>(0, connectToRenderRemote);
    rsUIContext->SetUITaskRunner([](const std::function<void()>& task, uint32_t delay) { task(); });
    std::shared_ptr<RSUIDirector> director = RSUIDirector::Create(connectToRenderRemote, rsUIContext);
    ASSERT_TRUE(director != nullptr);
    NodeId nodeId = 0;
    AnimationId animId = 0;
    uint64_t token = 0;
    AnimationCallbackEvent event = AnimationCallbackEvent::REPEAT_FINISHED;
    director->AnimationCallbackProcessor(nodeId, animId, token, event);
}

/**
 * @tc.name: AnimationCallbackProcessorTest001
 * @tc.desc: test results of AnimationCallbackProcessor
 * @tc.type:FUNC
 */
HWTEST_F(RSAnimationCallbackProcessorTest, AnimationCallbackProcessorTest001, TestSize.Level1)
{
    OHOS::sptr<OHOS::IRemoteObject> connectToRenderRemote;
    auto rsUIContext = std::make_shared<RSUIContext>(0, connectToRenderRemote);
    rsUIContext->SetUITaskRunner([](const std::function<void()>& task, uint32_t delay) { task(); });
    std::shared_ptr<RSUIDirector> director = RSUIDirector::Create(connectToRenderRemote, rsUIContext);
    ASSERT_TRUE(director != nullptr);
    NodeId nodeId = 0;
    AnimationId animId = 0;
    uint64_t token = 0;
    AnimationCallbackEvent event = AnimationCallbackEvent::REPEAT_FINISHED;
    director->AnimationCallbackProcessor(nodeId, animId, token, event);

    auto node = std::make_shared<RSRootNode>(false);
    nodeId = node->GetId();
    director->SetRoot(nodeId);
    RSRootNode::SharedPtr nodePtr = std::make_shared<RSRootNode>(nodeId);
    bool res = RSNodeMap::MutableInstance().RegisterNode(nodePtr);
    director->AnimationCallbackProcessor(nodeId, animId, token, event);
    auto animation = std::make_shared<RSAnimationMock>(director->GetRSUIContext());
    animId = animation->GetId();
    director->rsUIContext_->animations_.emplace(animId, animation);
    director->AnimationCallbackProcessor(nodeId, animId, token, event);
    director->rsUIContext_ = nullptr;
    director->AnimationCallbackProcessor(nodeId, animId, token, event);
    ASSERT_TRUE(res);
}

} // namespace Rosen
} // namespace OHOS
