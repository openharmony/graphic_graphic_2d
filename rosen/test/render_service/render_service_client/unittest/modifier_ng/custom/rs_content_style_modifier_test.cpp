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

#include <cstdlib>
#include <cstring>
#include <memory>
#include <securec.h>

#include "gtest/gtest.h"
#include "gtest/hwext/gtest-tag.h"
#include "message_parcel.h"

#include "common/rs_vector4.h"
#include "modifier_ng/custom/rs_content_style_modifier.h"
#include "ui/rs_canvas_drawing_node.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS::Rosen::ModifierNG;

namespace OHOS::Rosen {
class RSContentStyleModifierNGTypeTest : public testing::Test {
public:
    static void SetUpTestCase() {};
    static void TearDownTestCase() {};
    void SetUp() override {};
    void TearDown() override {};
};

/**
 * @tc.name: RSContentStyleModifierTest
 * @tc.desc: Test Set/Get functions of RSContentStyleModifier
 * @tc.type: FUNC
 */
HWTEST_F(RSContentStyleModifierNGTypeTest, RSContentStyleModifierTest, TestSize.Level1)
{
    std::shared_ptr<ModifierNG::RSContentStyleModifier> modifier =
        std::make_shared<ModifierNG::RSContentStyleModifier>();
    EXPECT_EQ(modifier->GetType(), ModifierNG::RSModifierType::CONTENT_STYLE);
    EXPECT_EQ(modifier->GetInnerPropertyType(), ModifierNG::RSPropertyType::CONTENT_STYLE);
}

#ifdef RS_MODIFIERS_DRAW_ENABLE
/**
 * @tc.name: FlushContentModifierImmediatelyTest001
 * @tc.desc: Test FlushContentModifierImmediately with expired node
 * @tc.type: FUNC
 */
HWTEST_F(RSContentStyleModifierNGTypeTest, FlushContentModifierImmediatelyTest001, TestSize.Level1)
{
    auto modifier = std::make_shared<RSContentStyleModifier>();
    modifier->FlushContentModifierImmediately();
}
 
/**
 * @tc.name: RenderInClientTest001
 * @tc.desc: Test RenderInClient with nullptr node
 * @tc.type: FUNC
 */
HWTEST_F(RSContentStyleModifierNGTypeTest, RenderInClientTest001, TestSize.Level1)
{
    auto modifier = std::make_shared<RSContentStyleModifier>();
    auto drawCmdList = std::make_shared<Drawing::DrawCmdList>();
    bool ret = modifier->RenderInClient(drawCmdList, nullptr);
    EXPECT_FALSE(ret);
}
 
/**
 * @tc.name: RenderInClientTest002
 * @tc.desc: Test RenderInClient with valid node
 * @tc.type: FUNC
 */
HWTEST_F(RSContentStyleModifierNGTypeTest, RenderInClientTest002, TestSize.Level1)
{
    auto node = std::make_shared<RSCanvasDrawingNode>(0);
    auto modifier = std::make_shared<RSContentStyleModifier>();
    auto drawCmdList = std::make_shared<Drawing::DrawCmdList>();
    bool ret = modifier->RenderInClient(drawCmdList, node);
    EXPECT_FALSE(ret);
}
 
/**
 * @tc.name: FlushContentModifierImmediatelyTest002
 * @tc.desc: Test FlushContentModifierImmediately with non-canvas drawing node
 * @tc.type: FUNC
 */
HWTEST_F(RSContentStyleModifierNGTypeTest, FlushContentModifierImmediatelyTest002, TestSize.Level1)
{
    auto node = std::make_shared<RSNode>(0);
    auto modifier = std::make_shared<RSContentStyleModifier>();
    modifier->FlushContentModifierImmediately();
}
#endif
} // namespace OHOS::Rosen