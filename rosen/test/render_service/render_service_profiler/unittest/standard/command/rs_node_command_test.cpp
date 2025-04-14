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
 * @tc.name: FixNodeCommandTest
 * @tc.desc: Fix enum class to prevent undetectable changes
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSNodeCommandTypeTest, FixNodeCommandTest, Level1 | Standard)
{
    EXPECT_EQ(static_cast<uint16_t>(ADD_MODIFIER), 0x0000);
    EXPECT_EQ(static_cast<uint16_t>(REMOVE_MODIFIER), 0x0001);

    EXPECT_EQ(static_cast<uint16_t>(UPDATE_MODIFIER_BOOL), 0x0100);
    EXPECT_EQ(static_cast<uint16_t>(UPDATE_MODIFIER_FLOAT), 0x0101);
    EXPECT_EQ(static_cast<uint16_t>(UPDATE_MODIFIER_INT), 0x0102);
    EXPECT_EQ(static_cast<uint16_t>(UPDATE_MODIFIER_COLOR), 0x0103);
    EXPECT_EQ(static_cast<uint16_t>(UPDATE_MODIFIER_GRAVITY), 0x0104);
    EXPECT_EQ(static_cast<uint16_t>(UPDATE_MODIFIER_MATRIX3F), 0x0105);
    EXPECT_EQ(static_cast<uint16_t>(UPDATE_MODIFIER_QUATERNION), 0x0106);
    EXPECT_EQ(static_cast<uint16_t>(UPDATE_MODIFIER_FILTER_PTR), 0x0107);
    EXPECT_EQ(static_cast<uint16_t>(UPDATE_MODIFIER_IMAGE_PTR), 0x0108);
    EXPECT_EQ(static_cast<uint16_t>(UPDATE_MODIFIER_MASK_PTR), 0x0109);
    EXPECT_EQ(static_cast<uint16_t>(UPDATE_MODIFIER_PATH_PTR), 0x010A);
    EXPECT_EQ(static_cast<uint16_t>(UPDATE_MODIFIER_DYNAMIC_BRIGHTNESS), 0x010B);
    EXPECT_EQ(static_cast<uint16_t>(UPDATE_MODIFIER_GRADIENT_BLUR_PTR), 0x010C);
    EXPECT_EQ(static_cast<uint16_t>(UPDATE_MODIFIER_EMITTER_UPDATER_PTR), 0x010D);
    EXPECT_EQ(static_cast<uint16_t>(UPDATE_MODIFIER_NOISE_FIELD_PTR), 0x010E);
    EXPECT_EQ(static_cast<uint16_t>(UPDATE_MODIFIER_SHADER_PTR), 0x010F);
    EXPECT_EQ(static_cast<uint16_t>(UPDATE_MODIFIER_VECTOR2F), 0x0110);
    EXPECT_EQ(static_cast<uint16_t>(UPDATE_MODIFIER_VECTOR3F), 0x0111);
    EXPECT_EQ(static_cast<uint16_t>(UPDATE_MODIFIER_VECTOR4_BORDER_STYLE), 0x0112);
    EXPECT_EQ(static_cast<uint16_t>(UPDATE_MODIFIER_VECTOR4_COLOR), 0x0113);
    EXPECT_EQ(static_cast<uint16_t>(UPDATE_MODIFIER_VECTOR4F), 0x0114);
    EXPECT_EQ(static_cast<uint16_t>(UPDATE_MODIFIER_RRECT), 0x0115);
    EXPECT_EQ(static_cast<uint16_t>(UPDATE_MODIFIER_DRAW_CMD_LIST), 0x0116);
    EXPECT_EQ(static_cast<uint16_t>(UPDATE_MODIFIER_DRAWING_MATRIX), 0x0117);

    EXPECT_EQ(static_cast<uint16_t>(SET_FREEZE), 0x0200);
    EXPECT_EQ(static_cast<uint16_t>(SET_DRAW_REGION), 0x0201);
    EXPECT_EQ(static_cast<uint16_t>(SET_OUT_OF_PARENT), 0x0202);
    EXPECT_EQ(static_cast<uint16_t>(SET_TAKE_SURFACE_CAPTURE_FOR_UI_FLAG), 0x0203);
    EXPECT_EQ(static_cast<uint16_t>(SET_UIFIRST_SWITCH), 0x0204);

    EXPECT_EQ(static_cast<uint16_t>(REGISTER_GEOMETRY_TRANSITION), 0x0300);
    EXPECT_EQ(static_cast<uint16_t>(UNREGISTER_GEOMETRY_TRANSITION), 0x0301);

    EXPECT_EQ(static_cast<uint16_t>(MARK_NODE_GROUP), 0x0400);
    EXPECT_EQ(static_cast<uint16_t>(MARK_NODE_SINGLE_FRAME_COMPOSER), 0x0401);
    EXPECT_EQ(static_cast<uint16_t>(MARK_SUGGEST_OPINC_NODE), 0x0402);

    EXPECT_EQ(static_cast<uint16_t>(MARK_UIFIRST_NODE), 0x0500);
    EXPECT_EQ(static_cast<uint16_t>(MARK_UIFIRST_NODE_FORCE), 0x0501);

    EXPECT_EQ(static_cast<uint16_t>(SET_NODE_NAME), 0x0600);
    EXPECT_EQ(static_cast<uint16_t>(UPDATE_MODIFIER_MOTION_BLUR_PTR), 0x0601);
    EXPECT_EQ(static_cast<uint16_t>(UPDATE_MODIFIER_MAGNIFIER_PTR), 0x0602);
    EXPECT_EQ(static_cast<uint16_t>(UPDATE_MODIFIER_WATER_RIPPLE), 0x0603);
    EXPECT_EQ(static_cast<uint16_t>(UPDATE_MODIFIER_FLY_OUT), 0x0604);
    EXPECT_EQ(static_cast<uint16_t>(REMOVE_ALL_MODIFIERS), 0x0605);

    EXPECT_EQ(static_cast<uint16_t>(DUMP_CLIENT_NODE_TREE), 0x0700);
    EXPECT_EQ(static_cast<uint16_t>(COMMIT_DUMP_CLIENT_NODE_TREE), 0x0701);
}
} // namespace OHOS::Rosen