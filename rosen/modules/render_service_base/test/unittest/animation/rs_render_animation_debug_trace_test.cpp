/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "animation/rs_animation_trace_utils.h"
#include "pipeline/rs_root_render_node.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class RSRenderAnimationDebugTraceTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    static constexpr uint64_t NODE_ID = 123456;
    static constexpr uint64_t ANIMATION_ID = 12345;
    static constexpr uint64_t PROPERTY_ID = 54321;
    const std::string NODE_NAME = "test";
};

void RSRenderAnimationDebugTraceTest::SetUpTestCase() {}
void RSRenderAnimationDebugTraceTest::TearDownTestCase() {}
void RSRenderAnimationDebugTraceTest::SetUp() {}
void RSRenderAnimationDebugTraceTest::TearDown() {}

/**
 * @tc.name: AnimationDebugTrace001
 * @tc.desc: Verify the AnimationDebugTrace
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderAnimationDebugTraceTest, AnimationDebugTrace001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderSpringAnimationTest AnimationDebugTrace001 start";

    RSAnimationTraceUtils::GetInstance().addAnimationNameTrace(NODE_NAME);
    RSAnimationTraceUtils::GetInstance().addAnimationFinishTrace(NODE_NAME, NODE_ID, ANIMATION_ID, false);
    auto startValue = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto endValue = std::make_shared<RSRenderAnimatableProperty<float>>(100.0f);
    RSAnimationTraceUtils::GetInstance().addAnimationCreateTrace(
        NODE_ID, NODE_NAME, PROPERTY_ID, ANIMATION_ID, 0, 0, startValue, endValue, 0, 0, 1);
    RSAnimationTraceUtils::GetInstance().addAnimationFrameTrace(
        NODE_ID, NODE_NAME, ANIMATION_ID, PROPERTY_ID, 0, startValue, 0);
    RSRenderNode renderNode(NODE_ID);
    RSAnimationTraceUtils::GetInstance().addRenderNodeTrace(renderNode);
    RSAnimationTraceUtils::GetInstance().addSpringInitialVelocityTrace(PROPERTY_ID, ANIMATION_ID, startValue, endValue);

    GTEST_LOG_(INFO) << "RSRenderSpringAnimationTest AnimationDebugTrace001 end";
}

/**
 * @tc.name: AnimationDebugTrace002
 * @tc.desc: Verify the AnimationDebugTrace
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderAnimationDebugTraceTest, AnimationDebugTrace002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderSpringAnimationTest AnimationDebugTrace002 start";

    system("param set persist.rosen.animationtrace.enabled 1");
    RSAnimationTraceUtils::GetInstance().addAnimationNameTrace(NODE_NAME);
    RSAnimationTraceUtils::GetInstance().addAnimationFinishTrace(NODE_NAME, NODE_ID, ANIMATION_ID, true);
    auto startValue = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto endValue = std::make_shared<RSRenderAnimatableProperty<float>>(100.0f);
    RSAnimationTraceUtils::GetInstance().addAnimationCreateTrace(
        NODE_ID, NODE_NAME, PROPERTY_ID, ANIMATION_ID, 0, 0, startValue, endValue, 0, 0, 1);
    RSAnimationTraceUtils::GetInstance().addAnimationFrameTrace(
        NODE_ID, NODE_NAME, ANIMATION_ID, PROPERTY_ID, 0, startValue, 0);
    RSRenderNode renderNode(NODE_ID);
    RSAnimationTraceUtils::GetInstance().addRenderNodeTrace(renderNode);
    RSAnimationTraceUtils::GetInstance().addSpringInitialVelocityTrace(PROPERTY_ID, ANIMATION_ID, startValue, endValue);
    system("param set persist.rosen.animationtrace.enabled 0");

    GTEST_LOG_(INFO) << "RSRenderSpringAnimationTest AnimationDebugTrace002 end";
}
} // namespace Rosen
} // namespace OHOS