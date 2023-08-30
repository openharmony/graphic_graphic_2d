/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "modifier/rs_modifier_manager.h"
#include "animation/rs_render_animation.h"
#include "command/rs_animation_command.h"
#include "command/rs_message_processor.h"
#include "modifier/rs_property_modifier.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSModifierManagerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSModifierManagerTest::SetUpTestCase() {}
void RSModifierManagerTest::TearDownTestCase() {}
void RSModifierManagerTest::SetUp() {}
void RSModifierManagerTest::TearDown() {}

/**
 * @tc.name: CreateDrawingContextTest
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSModifierManagerTest, CreateDrawingContextTest, TestSize.Level1)
{
    RSModifierManager rsModifierManager;
    rsModifierManager.Draw();
}

/**
 * @tc.name: AddAnimationTest
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSModifierManagerTest, AddAnimationTest, TestSize.Level1)
{
    auto animation = std::shared_ptr<RSRenderAnimation>();
    RSModifierManager rsModifierManager;
    rsModifierManager.AddAnimation(animation);
}

/**
 * @tc.name: RemoveAnimationTest
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSModifierManagerTest, RemoveAnimationTest, TestSize.Level1)
{
    auto animation = std::shared_ptr<RSRenderAnimation>();
    AnimationId key = animation->GetAnimationId();
    RSModifierManager rsModifierManager;
    rsModifierManager.RemoveAnimation(key);
}
} // namespace OHOS::Rosen