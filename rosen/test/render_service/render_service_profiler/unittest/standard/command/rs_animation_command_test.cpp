/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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
#include "gtest/hwext/gtest-tag.h"

#include "command/rs_animation_command.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSAnimationCommandTest : public testing::Test {
public:
    static void SetUpTestCase() {};
    static void TearDownTestCase() {};
    void SetUp() override {};
    void TearDown() override {};
};

/*
 * @tc.name: FixRSAnimationCommand
 * @tc.desc: Fix enum class to prevent undetectable changes
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSAnimationCommandTest, FixRSAnimationCommand, Level1 | Standard)
{
    auto value = 0x0100;
    EXPECT_EQ(static_cast<uint16_t>(RSAnimationCommandType::ANIMATION_CREATE_CURVE), value++);
    EXPECT_EQ(static_cast<uint16_t>(RSAnimationCommandType::ANIMATION_CREATE_PARTICLE), value++);
    EXPECT_EQ(static_cast<uint16_t>(RSAnimationCommandType::ANIMATION_CREATE_KEYFRAME), value++);
    EXPECT_EQ(static_cast<uint16_t>(RSAnimationCommandType::ANIMATION_CREATE_PATH), value++);
    EXPECT_EQ(static_cast<uint16_t>(RSAnimationCommandType::ANIMATION_CREATE_TRANSITION), value++);
    EXPECT_EQ(static_cast<uint16_t>(RSAnimationCommandType::ANIMATION_CREATE_SPRING), value++);
    EXPECT_EQ(static_cast<uint16_t>(RSAnimationCommandType::ANIMATION_CREATE_INTERPOLATING_SPRING), value++);

    value = 0x0200;
    EXPECT_EQ(static_cast<uint16_t>(RSAnimationCommandType::ANIMATION_START), value++);
    EXPECT_EQ(static_cast<uint16_t>(RSAnimationCommandType::ANIMATION_PAUSE), value++);
    EXPECT_EQ(static_cast<uint16_t>(RSAnimationCommandType::ANIMATION_RESUME), value++);
    EXPECT_EQ(static_cast<uint16_t>(RSAnimationCommandType::ANIMATION_FINISH), value++);
    EXPECT_EQ(static_cast<uint16_t>(RSAnimationCommandType::ANIMATION_REVERSE), value++);
    EXPECT_EQ(static_cast<uint16_t>(RSAnimationCommandType::ANIMATION_SET_FRACTION), value++);
    EXPECT_EQ(static_cast<uint16_t>(RSAnimationCommandType::ANIMATION_CANCEL), value++);

    EXPECT_EQ(static_cast<uint16_t>(RSAnimationCommandType::ANIMATION_CALLBACK), 0x0300);

    value = 0x0400;
    EXPECT_EQ(static_cast<uint16_t>(RSAnimationCommandType::INTERACTIVE_ANIMATOR_CREATE), value++);
    EXPECT_EQ(static_cast<uint16_t>(RSAnimationCommandType::INTERACTIVE_ANIMATOR_DESTORY), value++);
    EXPECT_EQ(static_cast<uint16_t>(RSAnimationCommandType::INTERACTIVE_ANIMATOR_PAUSE), value++);
    EXPECT_EQ(static_cast<uint16_t>(RSAnimationCommandType::INTERACTIVE_ANIMATOR_CONTINUE), value++);
    EXPECT_EQ(static_cast<uint16_t>(RSAnimationCommandType::INTERACTIVE_ANIMATOR_FINISH), value++);
    EXPECT_EQ(static_cast<uint16_t>(RSAnimationCommandType::INTERACTIVE_ANIMATOR_REVERSE), value++);
    EXPECT_EQ(static_cast<uint16_t>(RSAnimationCommandType::INTERACTIVE_ANIMATOR_SET_FRACTION), value++);
}
} // namespace OHOS::Rosen
