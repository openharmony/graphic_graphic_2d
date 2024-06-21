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

#include "animation/rs_render_interactive_implict_animator_map.h"
#include "animation/rs_render_interactive_implict_animator.h"
#include "pipeline/rs_canvas_render_node.h"
#include "pipeline/rs_context.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class RSRenderInteractiveImplictAnimatorMapTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    static constexpr InteractiveImplictAnimatorId ANIMATOR_ID = 10001;
    static constexpr NodeId NODE_ID = 10002;
    static constexpr AnimationId ANIMATION_ID = 10003;
    static constexpr uint64_t PROPERTY_ID = 10004;
};

void RSRenderInteractiveImplictAnimatorMapTest::SetUpTestCase() {}
void RSRenderInteractiveImplictAnimatorMapTest::TearDownTestCase() {}
void RSRenderInteractiveImplictAnimatorMapTest::SetUp() {}
void RSRenderInteractiveImplictAnimatorMapTest::TearDown() {}

/**
 * @tc.name: RegisterInteractiveImplictAnimator001
 * @tc.desc: Verify the RegisterInteractiveImplictAnimator
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderInteractiveImplictAnimatorMapTest, RegisterInteractiveImplictAnimator001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderInteractiveImplictAnimatorMapTest RegisterInteractiveImplictAnimator001 start";
    RSContext context;
    auto animator = std::make_shared<RSRenderInteractiveImplictAnimator>(ANIMATOR_ID, context.weak_from_this());
    EXPECT_TRUE(animator != nullptr);
    context.GetInteractiveImplictAnimatorMap().RegisterInteractiveImplictAnimator(animator);
    auto result = context.GetInteractiveImplictAnimatorMap().GetInteractiveImplictAnimator(ANIMATOR_ID);
    EXPECT_TRUE(result != nullptr);

    context.GetInteractiveImplictAnimatorMap().RegisterInteractiveImplictAnimator(animator);
    GTEST_LOG_(INFO) << "RSRenderInteractiveImplictAnimatorMapTest RegisterInteractiveImplictAnimator001 end";
}

/**
 * @tc.name: UnregisterInteractiveImplictAnimator001
 * @tc.desc: Verify the UnregisterInteractiveImplictAnimator
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderInteractiveImplictAnimatorMapTest, UnregisterInteractiveImplictAnimator001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderInteractiveImplictAnimatorMapTest UnregisterInteractiveImplictAnimator001 start";
    RSContext context;
    auto animator = std::make_shared<RSRenderInteractiveImplictAnimator>(ANIMATOR_ID, context.weak_from_this());
    EXPECT_TRUE(animator != nullptr);
    context.GetInteractiveImplictAnimatorMap().RegisterInteractiveImplictAnimator(animator);
    auto result = context.GetInteractiveImplictAnimatorMap().GetInteractiveImplictAnimator(ANIMATOR_ID);
    EXPECT_TRUE(result != nullptr);

    context.GetInteractiveImplictAnimatorMap().UnregisterInteractiveImplictAnimator(ANIMATOR_ID);
    result = context.GetInteractiveImplictAnimatorMap().GetInteractiveImplictAnimator(ANIMATOR_ID);
    EXPECT_TRUE(result == nullptr);
    GTEST_LOG_(INFO) << "RSRenderInteractiveImplictAnimatorMapTest UnregisterInteractiveImplictAnimator001 end";
}
} // namespace Rosen
} // namespace OHOS
