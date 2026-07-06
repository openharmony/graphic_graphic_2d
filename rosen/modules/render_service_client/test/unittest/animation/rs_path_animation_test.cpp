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
#include "rs_animation_test_utils.h"

#include "animation/rs_path_animation.h"
#include "modifier_ng/appearance/rs_background_filter_modifier.h"
#include "ui/rs_canvas_node.h"
#include "ui/rs_ui_context.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
using namespace ANIMATIONTEST;

class RSPathAnimationTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    std::shared_ptr<RSUIContext> rsUIContext;
};

void RSPathAnimationTest::SetUpTestCase() {}
void RSPathAnimationTest::TearDownTestCase() {}

void RSPathAnimationTest::SetUp()
{
    OHOS::sptr<OHOS::IRemoteObject> connectToRenderRemote;
    rsUIContext = std::make_shared<RSUIContext>(0, connectToRenderRemote);
    rsUIContext->SetUITaskRunner([](const std::function<void()>& task, uint32_t delay) { task(); });
}

void RSPathAnimationTest::TearDown() {}

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
    auto pathAnimation =
        std::make_shared<RSPathAnimation>(rsUIContext, property, ANIMATION_PATH, startProperty, endProperty);
    auto node = RSCanvasNode::Create(false, false, rsUIContext);
    EXPECT_EQ(node->GetModifierCreatedBySetter(ModifierNG::RSModifierType::TRANSFORM), nullptr);
    pathAnimation->SetRotation(node, 1.0f);

    node->modifiersNGCreatedBySetter_.emplace(
        ModifierNG::RSModifierType::TRANSFORM, std::make_shared<ModifierNG::RSBackgroundFilterModifier>());
    EXPECT_NE(node->GetModifierCreatedBySetter(ModifierNG::RSModifierType::TRANSFORM), nullptr);
    pathAnimation->SetRotation(node, 1.0f);
}

/**
 * @tc.name: RebuildInRender001
 * @tc.desc: Verify RebuildInRender with null animation path
 * @tc.type: FUNC
 */
HWTEST_F(RSPathAnimationTest, RebuildInRender001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSPathAnimationTest RebuildInRender001 start";
    auto property = std::make_shared<RSAnimatableProperty<Vector2f>>(Vector2f(0.f, 0.f));
    auto startProperty = std::make_shared<RSAnimatableProperty<Vector2f>>(Vector2f(0.f, 0.f));
    auto endProperty = std::make_shared<RSAnimatableProperty<Vector2f>>(Vector2f(1.f, 1.f));
    auto pathAnimation =
        std::make_shared<RSPathAnimation>(rsUIContext, property, ANIMATION_PATH, startProperty, endProperty);
    pathAnimation->SetRebuildParam({0.5f, false});
    pathAnimation->RebuildInRender();
    GTEST_LOG_(INFO) << "RSPathAnimationTest RebuildInRender001 end";
}
} // namespace Rosen
} // namespace OHOS
