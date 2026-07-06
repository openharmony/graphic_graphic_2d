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

#include "animation/rs_curve_animation.h"
#include "animation/rs_particle_animation.h"
#include "modifier/rs_property.h"
#include "ui/rs_ui_context.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class RSParticleAnimationTest : public testing::Test {
public:
    std::shared_ptr<RSUIContext> CreateRSUIContext();
    std::vector<std::shared_ptr<ParticleRenderParams>> CreateParticleParams();
    static constexpr uint64_t MODIFIER_ID = 54321;
    const Vector4f ANIMATION_START_BOUNDS = Vector4f(100.f, 100.f, 200.f, 300.f);
    const Vector4f ANIMATION_END_BOUNDS = Vector4f(100.f, 100.f, 300.f, 300.f);
};

std::shared_ptr<RSUIContext> RSParticleAnimationTest::CreateRSUIContext()
{
    OHOS::sptr<OHOS::IRemoteObject> connectToRenderRemote;
    auto rsUIContext = std::make_shared<RSUIContext>(0, connectToRenderRemote);
    rsUIContext->SetUITaskRunner([](const std::function<void()>& task, uint32_t delay) { task(); });
    return rsUIContext;
}

std::vector<std::shared_ptr<ParticleRenderParams>> RSParticleAnimationTest::CreateParticleParams()
{
    std::vector<std::shared_ptr<ParticleRenderParams>> particlesRenderParams;
    particlesRenderParams.push_back(std::make_shared<ParticleRenderParams>());
    return particlesRenderParams;
}

/**
 * @tc.name: IsParticleAnimation001
 * @tc.desc: Verify RSParticleAnimation::IsParticleAnimation returns true
 * @tc.type: FUNC
 */
HWTEST_F(RSParticleAnimationTest, IsParticleAnimation001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSParticleAnimationTest IsParticleAnimation001 start";
    auto rsUIContext = CreateRSUIContext();
    auto particleAnimation =
        std::make_shared<RSParticleAnimation>(rsUIContext, CreateParticleParams(), MODIFIER_ID);
    ASSERT_TRUE(particleAnimation != nullptr);
    EXPECT_TRUE(particleAnimation->IsParticleAnimation());
    GTEST_LOG_(INFO) << "RSParticleAnimationTest IsParticleAnimation001 end";
}

/**
 * @tc.name: IsParticleAnimation002
 * @tc.desc: Verify the RSAnimation base-class default IsParticleAnimation returns false
 * @tc.type: FUNC
 */
HWTEST_F(RSParticleAnimationTest, IsParticleAnimation002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSParticleAnimationTest IsParticleAnimation002 start";
    auto rsUIContext = CreateRSUIContext();
    auto property = std::make_shared<RSAnimatableProperty<Vector4f>>(ANIMATION_START_BOUNDS);
    auto startProperty = std::make_shared<RSAnimatableProperty<Vector4f>>(ANIMATION_START_BOUNDS);
    auto endProperty = std::make_shared<RSAnimatableProperty<Vector4f>>(ANIMATION_END_BOUNDS);
    auto curveAnimation =
        std::make_shared<RSCurveAnimation>(rsUIContext, property, startProperty, endProperty);
    ASSERT_TRUE(curveAnimation != nullptr);
    EXPECT_FALSE(curveAnimation->IsParticleAnimation());
    GTEST_LOG_(INFO) << "RSParticleAnimationTest IsParticleAnimation002 end";
}

/**
 * @tc.name: IsSupportInteractiveAnimator001
 * @tc.desc: Verify RSParticleAnimation does not support the interactive animator
 * @tc.type: FUNC
 */
HWTEST_F(RSParticleAnimationTest, IsSupportInteractiveAnimator001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSParticleAnimationTest IsSupportInteractiveAnimator001 start";
    auto rsUIContext = CreateRSUIContext();
    auto particleAnimation =
        std::make_shared<RSParticleAnimation>(rsUIContext, CreateParticleParams(), MODIFIER_ID);
    ASSERT_TRUE(particleAnimation != nullptr);
    EXPECT_FALSE(particleAnimation->IsSupportInteractiveAnimator());
    GTEST_LOG_(INFO) << "RSParticleAnimationTest IsSupportInteractiveAnimator001 end";
}
} // namespace Rosen
} // namespace OHOS
