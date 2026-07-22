/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "animation/rs_keyframe_animation.h"
#include "rs_animation_test_utils.h"
#include "ui/rs_canvas_node.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class RSKeyframeAnimationTest : public testing::Test {
};

/**
 * @tc.name: RebuildInRender001
 * @tc.desc: Verify RebuildInRender with empty keyframes
 * @tc.type: FUNC
 */
HWTEST_F(RSKeyframeAnimationTest, RebuildInRender001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSKeyframeAnimationTest RebuildInRender001 start";
    auto property = std::make_shared<RSAnimatableProperty<Vector4f>>(ANIMATIONTEST::ANIMATION_START_BOUNDS);
    OHOS::sptr<OHOS::IRemoteObject> connectToRenderRemote;
    auto rsUIContext = std::make_shared<RSUIContext>(0, connectToRenderRemote);
    rsUIContext->SetUITaskRunner([](const std::function<void()>& task, uint32_t delay) { task(); });
    auto keyframeAnimation = std::make_shared<RSKeyframeAnimation>(rsUIContext, property);
    keyframeAnimation->SetRebuildParam({0.5f, false});
    keyframeAnimation->RebuildInRender();
    GTEST_LOG_(INFO) << "RSKeyframeAnimationTest RebuildInRender001 end";
}

/**
 * @tc.name: CreateRenderAnimationNullOrigin001
 * @tc.desc: Verify CreateRenderAnimation returns nullptr when originValue_ is null
 * @tc.type:FUNC
 */
HWTEST_F(RSKeyframeAnimationTest, CreateRenderAnimationNullOrigin001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSKeyframeAnimationTest CreateRenderAnimationNullOrigin001 start";
    auto property = std::make_shared<RSAnimatableProperty<float>>(0.0f);
    OHOS::sptr<OHOS::IRemoteObject> connectToRenderRemote;
    auto rsUIContext = std::make_shared<RSUIContext>(0, connectToRenderRemote);
    rsUIContext->SetUITaskRunner([](const std::function<void()>& task, uint32_t delay) { task(); });
    auto keyframeAnimation = std::make_shared<RSKeyframeAnimation>(rsUIContext, property);
    keyframeAnimation->originValue_ = nullptr;
    auto result = keyframeAnimation->CreateRenderAnimation();
    EXPECT_EQ(result, nullptr);
    GTEST_LOG_(INFO) << "RSKeyframeAnimationTest CreateRenderAnimationNullOrigin001 end";
}

/**
 * @tc.name: OnStartAnimationNull001
 * @tc.desc: Verify OnStart handles CreateRenderAnimation returning nullptr
 * @tc.type:FUNC
 */
HWTEST_F(RSKeyframeAnimationTest, OnStartAnimationNull001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSKeyframeAnimationTest OnStartAnimationNull001 start";
    auto property = std::make_shared<RSAnimatableProperty<float>>(0.0f);
    OHOS::sptr<OHOS::IRemoteObject> connectToRenderRemote;
    auto rsUIContext = std::make_shared<RSUIContext>(0, connectToRenderRemote);
    rsUIContext->SetUITaskRunner([](const std::function<void()>& task, uint32_t delay) { task(); });
    auto canvasNode = RSCanvasNode::Create(false, false, rsUIContext);
    auto keyframeAnimation = std::make_shared<RSKeyframeAnimation>(rsUIContext, property);
    keyframeAnimation->target_ = canvasNode;
    keyframeAnimation->property_ = nullptr;
    keyframeAnimation->originValue_ = nullptr;
    keyframeAnimation->keyframes_.push_back(
        {0.5f, std::make_shared<RSAnimatableProperty<float>>(1.0f), RSAnimationTimingCurve::LINEAR});
    keyframeAnimation->OnStart();
    EXPECT_FALSE(keyframeAnimation->IsRunning());
    GTEST_LOG_(INFO) << "RSKeyframeAnimationTest OnStartAnimationNull001 end";
}

/**
 * @tc.name: RebuildInRenderAnimationNull001
 * @tc.desc: Verify RebuildInRender handles CreateRenderAnimation returning nullptr
 * @tc.type:FUNC
 */
HWTEST_F(RSKeyframeAnimationTest, RebuildInRenderAnimationNull001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSKeyframeAnimationTest RebuildInRenderAnimationNull001 start";
    auto property = std::make_shared<RSAnimatableProperty<float>>(0.0f);
    OHOS::sptr<OHOS::IRemoteObject> connectToRenderRemote;
    auto rsUIContext = std::make_shared<RSUIContext>(0, connectToRenderRemote);
    rsUIContext->SetUITaskRunner([](const std::function<void()>& task, uint32_t delay) { task(); });
    auto keyframeAnimation = std::make_shared<RSKeyframeAnimation>(rsUIContext, property);
    keyframeAnimation->originValue_ = nullptr;
    keyframeAnimation->RebuildInRender();
    EXPECT_FALSE(keyframeAnimation->IsRunning());
    GTEST_LOG_(INFO) << "RSKeyframeAnimationTest RebuildInRenderAnimationNull001 end";
}

/**
 * @tc.name: RebuildInRenderAnimationNullWithTarget001
 * @tc.desc: Verify RebuildInRender handles animation nullptr with valid target
 * @tc.type:FUNC
 */
HWTEST_F(RSKeyframeAnimationTest, RebuildInRenderAnimationNullWithTarget001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSKeyframeAnimationTest RebuildInRenderAnimationNullWithTarget001 start";
    auto property = std::make_shared<RSAnimatableProperty<float>>(0.0f);
    OHOS::sptr<OHOS::IRemoteObject> connectToRenderRemote;
    auto rsUIContext = std::make_shared<RSUIContext>(0, connectToRenderRemote);
    rsUIContext->SetUITaskRunner([](const std::function<void()>& task, uint32_t delay) { task(); });
    auto canvasNode = RSCanvasNode::Create(false, false, rsUIContext);
    auto keyframeAnimation = std::make_shared<RSKeyframeAnimation>(rsUIContext, property);
    keyframeAnimation->target_ = canvasNode;
    keyframeAnimation->originValue_ = nullptr;
    keyframeAnimation->keyframes_.push_back(
        {0.5f, std::make_shared<RSAnimatableProperty<float>>(1.0f), RSAnimationTimingCurve::LINEAR});
    keyframeAnimation->SetRebuildParam({0.5f, false});
    keyframeAnimation->RebuildInRender();
    EXPECT_FLOAT_EQ(keyframeAnimation->GetRebuildParam().fraction, 0.5f);
    EXPECT_FALSE(keyframeAnimation->IsRunning());
    GTEST_LOG_(INFO) << "RSKeyframeAnimationTest RebuildInRenderAnimationNullWithTarget001 end";
}

/**
 * @tc.name: OnStartPropertyNull001
 * @tc.desc: Verify OnStart handles property_ being null in isCustom_ branch
 * @tc.type:FUNC
 */
HWTEST_F(RSKeyframeAnimationTest, OnStartPropertyNull001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSKeyframeAnimationTest OnStartPropertyNull001 start";
    auto property = std::make_shared<RSAnimatableProperty<float>>(0.0f);
    OHOS::sptr<OHOS::IRemoteObject> connectToRenderRemote;
    auto rsUIContext = std::make_shared<RSUIContext>(0, connectToRenderRemote);
    rsUIContext->SetUITaskRunner([](const std::function<void()>& task, uint32_t delay) { task(); });
    auto canvasNode = RSCanvasNode::Create(false, false, rsUIContext);
    auto keyframeAnimation = std::make_shared<RSKeyframeAnimation>(rsUIContext, property);
    keyframeAnimation->target_ = canvasNode;
    keyframeAnimation->isCustom_ = true;
    keyframeAnimation->property_ = nullptr;
    keyframeAnimation->originValue_ = property;
    keyframeAnimation->keyframes_.push_back(
        {0.5f, std::make_shared<RSAnimatableProperty<float>>(1.0f), RSAnimationTimingCurve::LINEAR});
    keyframeAnimation->OnStart();
    EXPECT_FALSE(keyframeAnimation->IsRunning());
    GTEST_LOG_(INFO) << "RSKeyframeAnimationTest OnStartPropertyNull001 end";
}

} // namespace Rosen
} // namespace OHOS
