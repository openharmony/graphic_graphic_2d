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
    EXPECT_EQ(static_cast<uint16_t>(RSAnimationCommandType::ANIMATION_CREATE_CURVE), 0x0100);
    EXPECT_EQ(static_cast<uint16_t>(RSAnimationCommandType::ANIMATION_CREATE_PARTICLE), 0x0101);
    EXPECT_EQ(static_cast<uint16_t>(RSAnimationCommandType::ANIMATION_CREATE_KEYFRAME), 0x0102);
    EXPECT_EQ(static_cast<uint16_t>(RSAnimationCommandType::ANIMATION_CREATE_PATH), 0x0103);
    EXPECT_EQ(static_cast<uint16_t>(RSAnimationCommandType::ANIMATION_CREATE_TRANSITION), 0x0104);
    EXPECT_EQ(static_cast<uint16_t>(RSAnimationCommandType::ANIMATION_CREATE_SPRING), 0x0105);
    EXPECT_EQ(static_cast<uint16_t>(RSAnimationCommandType::ANIMATION_CREATE_INTERPOLATING_SPRING), 0x0106);

    EXPECT_EQ(static_cast<uint16_t>(RSAnimationCommandType::ANIMATION_START), 0x0200);
    EXPECT_EQ(static_cast<uint16_t>(RSAnimationCommandType::ANIMATION_PAUSE), 0x0201);
    EXPECT_EQ(static_cast<uint16_t>(RSAnimationCommandType::ANIMATION_RESUME), 0x0202);
    EXPECT_EQ(static_cast<uint16_t>(RSAnimationCommandType::ANIMATION_FINISH), 0x0203);
    EXPECT_EQ(static_cast<uint16_t>(RSAnimationCommandType::ANIMATION_REVERSE), 0x0204);
    EXPECT_EQ(static_cast<uint16_t>(RSAnimationCommandType::ANIMATION_SET_FRACTION), 0x0205);
    EXPECT_EQ(static_cast<uint16_t>(RSAnimationCommandType::ANIMATION_CANCEL), 0x0206);

    EXPECT_EQ(static_cast<uint16_t>(RSAnimationCommandType::ANIMATION_CALLBACK), 0x0300);

    EXPECT_EQ(static_cast<uint16_t>(RSAnimationCommandType::INTERACTIVE_ANIMATOR_CREATE), 0x0400);
    EXPECT_EQ(static_cast<uint16_t>(RSAnimationCommandType::INTERACTIVE_ANIMATOR_DESTORY), 0x0401);
    EXPECT_EQ(static_cast<uint16_t>(RSAnimationCommandType::INTERACTIVE_ANIMATOR_PAUSE), 0x0402);
    EXPECT_EQ(static_cast<uint16_t>(RSAnimationCommandType::INTERACTIVE_ANIMATOR_CONTINUE), 0x0403);
    EXPECT_EQ(static_cast<uint16_t>(RSAnimationCommandType::INTERACTIVE_ANIMATOR_FINISH), 0x0404);
    EXPECT_EQ(static_cast<uint16_t>(RSAnimationCommandType::INTERACTIVE_ANIMATOR_REVERSE), 0x0405);
    EXPECT_EQ(static_cast<uint16_t>(RSAnimationCommandType::INTERACTIVE_ANIMATOR_SET_FRACTION), 0x0406);
}
} // namespace OHOS::Rosen
