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

#include "command/rs_canvas_drawing_node_command.h"

#include "gtest/gtest.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSCanvasDrawingNodeCommandTypeTest : public testing::Test {
public:
    static void SetUpTestCase() {};
    static void TearDownTestCase() {};
    void SetUp() override {};
    void TearDown() override {};
};

/*
 * @tc.name: FixRSCanvasDrawingNodeCommand
 * @tc.desc: Fix enum class to prevent undetectable changes
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSCanvasDrawingNodeCommandTypeTest, FixRSCanvasDrawingNodeCommand, Level1 | Standard)
{
    auto value = 0;
    EXPECT_EQ(static_cast<uint16_t>(CANVAS_DRAWING_NODE_CREATE), value++);
    EXPECT_EQ(static_cast<uint16_t>(CANVAS_DRAWING_NODE_RESET_SURFACE), value++);
}
} // namespace OHOS::Rosen