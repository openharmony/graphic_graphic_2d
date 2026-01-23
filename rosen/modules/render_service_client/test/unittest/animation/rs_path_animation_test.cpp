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

#include "rs_animation_base_test.h"
#include "rs_animation_test_utils.h"

#include "animation/rs_path_animation.h"
#include "modifier_ng/appearance/rs_background_filter_modifier.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
using namespace ANIMATIONTEST;
class RSPathAnimationTest : public RSAnimationBaseTest {
};

/**
 * @tc.name: SetRotationTest001
 * @tc.desc: Verify the SetRotation
 * @tc.type: FUNC
 */
HWTEST_F(RSPathAnimationTest, SetRotationTest001, TestSize.Level1)
{
    auto property = std::make_shared<RSAnimatableProperty<Vector2f>>(Vector2f(0.f, 0.f));
    auto startProperty = std::make_shared<RSAnimatableProperty<Vector2f>>(Vector2f(0.f, 0.f));
    auto endProperty = std::make_shared<RSAnimatableProperty<Vector2f>>(Vector2f(1.f, 1.f));
    auto pathAnimation = std::make_shared<RSPathAnimation>(property, ANIMATION_PATH, startProperty, endProperty);
    auto node = RSCanvasNode::Create();
    EXPECT_EQ(node->GetModifierCreatedBySetter(ModifierNG::RSModifierType::TRANSFORM), nullptr);
    pathAnimation->SetRotation(node, 1.0f);

    node->modifiersNGCreatedBySetter_.emplace(
        ModifierNG::RSModifierType::TRANSFORM, std::make_shared<ModifierNG::RSBackgroundFilterModifier>());
    EXPECT_NE(node->GetModifierCreatedBySetter(ModifierNG::RSModifierType::TRANSFORM), nullptr);
    pathAnimation->SetRotation(node, 1.0f);
}
} // namespace Rosen
} // namespace OHOS