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

#include "command/rs_canvas_node_command.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSCanvasNodeCommandTypeTest : public testing::Test {
public:
    static void SetUpTestCase() {};
    static void TearDownTestCase() {};
    void SetUp() override {};
    void TearDown() override {};
};

/*
 * @tc.name: FixCanvasNodeCommand
 * @tc.desc: Fix enum class to prevent undetectable changes
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSCanvasNodeCommandTypeTest, FixCanvasNodeCommand, Level1 | Standard)
{
    auto value = 0;
    EXPECT_EQ(static_cast<uint16_t>(CANVAS_NODE_CREATE), value++);
    EXPECT_EQ(static_cast<uint16_t>(CANVAS_NODE_UPDATE_RECORDING), value++);
    EXPECT_EQ(static_cast<uint16_t>(CANVAS_NODE_CLEAR_RECORDING), value++);
    EXPECT_EQ(static_cast<uint16_t>(CANVAS_NODE_SET_HDR_PRESENT), value++);
    EXPECT_EQ(static_cast<uint16_t>(CANVAS_NODE_SET_LINKED_ROOTNODE), value++);
    EXPECT_EQ(static_cast<uint16_t>(CANVAS_NODE_SET_COLOR_GAMUT), value++);
}
} // namespace OHOS::Rosen