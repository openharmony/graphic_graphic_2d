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

#include "command/rs_node_command.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSNodeCommandTypeTest : public testing::Test {
public:
    static void SetUpTestCase() {};
    static void TearDownTestCase() {};
    void SetUp() override {};
    void TearDown() override {};
};

/*
 * @tc.name: FixNodeCommandTest001
 * @tc.desc: Fix enum class to prevent undetectable changes
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSNodeCommandTypeTest, FixNodeCommandTest001, Level1 | Standard)
{
    auto value = 0;
    EXPECT_EQ(static_cast<uint16_t>(ADD_MODIFIER), value++);
    EXPECT_EQ(static_cast<uint16_t>(REMOVE_MODIFIER), value++);

    value = 0x0100;
    EXPECT_EQ(static_cast<uint16_t>(UPDATE_MODIFIER_BOOL), value++);
    EXPECT_EQ(static_cast<uint16_t>(UPDATE_MODIFIER_FLOAT), value++);
    EXPECT_EQ(static_cast<uint16_t>(UPDATE_MODIFIER_INT), value++);
    EXPECT_EQ(static_cast<uint16_t>(UPDATE_MODIFIER_COLOR), value++);
    EXPECT_EQ(static_cast<uint16_t>(UPDATE_MODIFIER_GRAVITY), value++);
    EXPECT_EQ(static_cast<uint16_t>(UPDATE_MODIFIER_MATRIX3F), value++);
    EXPECT_EQ(static_cast<uint16_t>(UPDATE_MODIFIER_QUATERNION), value++);
    EXPECT_EQ(static_cast<uint16_t>(UPDATE_MODIFIER_FILTER_PTR), value++);
    EXPECT_EQ(static_cast<uint16_t>(UPDATE_MODIFIER_IMAGE_PTR), value++);
    EXPECT_EQ(static_cast<uint16_t>(UPDATE_MODIFIER_MASK_PTR), value++);
    EXPECT_EQ(static_cast<uint16_t>(UPDATE_MODIFIER_PATH_PTR), value++);
    EXPECT_EQ(static_cast<uint16_t>(UPDATE_MODIFIER_DYNAMIC_BRIGHTNESS), value++);
    EXPECT_EQ(static_cast<uint16_t>(UPDATE_MODIFIER_GRADIENT_BLUR_PTR), value++);
    EXPECT_EQ(static_cast<uint16_t>(UPDATE_MODIFIER_EMITTER_UPDATER_PTR), value++);
    EXPECT_EQ(static_cast<uint16_t>(UPDATE_MODIFIER_NOISE_FIELD_PTR), value++);
    EXPECT_EQ(static_cast<uint16_t>(UPDATE_MODIFIER_SHADER_PTR), value++);
    EXPECT_EQ(static_cast<uint16_t>(UPDATE_MODIFIER_VECTOR2F), value++);
    EXPECT_EQ(static_cast<uint16_t>(UPDATE_MODIFIER_VECTOR3F), value++);
    EXPECT_EQ(static_cast<uint16_t>(UPDATE_MODIFIER_VECTOR4_BORDER_STYLE), value++);
    EXPECT_EQ(static_cast<uint16_t>(UPDATE_MODIFIER_VECTOR4_COLOR), value++);
    EXPECT_EQ(static_cast<uint16_t>(UPDATE_MODIFIER_VECTOR4F), value++);
    EXPECT_EQ(static_cast<uint16_t>(UPDATE_MODIFIER_RRECT), value++);
    EXPECT_EQ(static_cast<uint16_t>(UPDATE_MODIFIER_DRAW_CMD_LIST), value++);
    EXPECT_EQ(static_cast<uint16_t>(UPDATE_MODIFIER_DRAWING_MATRIX), value++);
}

/*
 * @tc.name: FixNodeCommandTest002
 * @tc.desc: Fix enum class to prevent undetectable changes
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSNodeCommandTypeTest, FixNodeCommandTest002, Level1 | Standard)
{
    auto value = 0x0200;
    EXPECT_EQ(static_cast<uint16_t>(SET_FREEZE), value++);
    EXPECT_EQ(static_cast<uint16_t>(SET_DRAW_REGION), value++);
    EXPECT_EQ(static_cast<uint16_t>(SET_OUT_OF_PARENT), value++);
    EXPECT_EQ(static_cast<uint16_t>(SET_TAKE_SURFACE_CAPTURE_FOR_UI_FLAG), value++);
    EXPECT_EQ(static_cast<uint16_t>(SET_UIFIRST_SWITCH), value++);

    value = 0x0300;
    EXPECT_EQ(static_cast<uint16_t>(REGISTER_GEOMETRY_TRANSITION), value++);
    EXPECT_EQ(static_cast<uint16_t>(UNREGISTER_GEOMETRY_TRANSITION), value++);

    value = 0x0400;
    EXPECT_EQ(static_cast<uint16_t>(MARK_NODE_GROUP), value++);
    EXPECT_EQ(static_cast<uint16_t>(MARK_NODE_SINGLE_FRAME_COMPOSER), value++);
    EXPECT_EQ(static_cast<uint16_t>(MARK_SUGGEST_OPINC_NODE), value++);

    value = 0x0500;
    EXPECT_EQ(static_cast<uint16_t>(MARK_UIFIRST_NODE), value++);
    EXPECT_EQ(static_cast<uint16_t>(MARK_UIFIRST_NODE_FORCE), value++);

    value = 0x0600;
    EXPECT_EQ(static_cast<uint16_t>(SET_NODE_NAME), value++);
    EXPECT_EQ(static_cast<uint16_t>(UPDATE_MODIFIER_MOTION_BLUR_PTR), value++);
    EXPECT_EQ(static_cast<uint16_t>(UPDATE_MODIFIER_MAGNIFIER_PTR), value++);
    EXPECT_EQ(static_cast<uint16_t>(UPDATE_MODIFIER_WATER_RIPPLE), value++);
    EXPECT_EQ(static_cast<uint16_t>(UPDATE_MODIFIER_FLY_OUT), value++);
    EXPECT_EQ(static_cast<uint16_t>(REMOVE_ALL_MODIFIERS), value++);

    value = 0x0700;
    EXPECT_EQ(static_cast<uint16_t>(DUMP_CLIENT_NODE_TREE), value++);
    EXPECT_EQ(static_cast<uint16_t>(COMMIT_DUMP_CLIENT_NODE_TREE), value++);
}
} // namespace OHOS::Rosen