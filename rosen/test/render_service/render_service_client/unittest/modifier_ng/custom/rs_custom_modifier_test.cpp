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
#include "modifier_ng/custom/rs_custom_modifier.h"
#include "ui/rs_display_node.h"
#include "ui/rs_canvas_node.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS::Rosen::ModifierNG;

namespace OHOS::Rosen {
class RSCustomModifierHelperTest : public testing::Test {
public:
    static void SetUpTestCase() {};
    static void TearDownTestCase() {};
    void SetUp() override {};
    void TearDown() override {};
};

/**
 * @tc.name: CreateDrawingContextTest
 * @tc.desc: Test the function CreateDrawingContext
 * @tc.type: FUNC
 */
HWTEST_F(RSCustomModifierHelperTest, CreateDrawingContextTest, TestSize.Level1)
{
    std::shared_ptr<RSNode> node = nullptr;
    ModifierNG::RSDrawingContext context = RSCustomModifierHelper::CreateDrawingContext(node);
    EXPECT_EQ(context.canvas, nullptr);

    RSDisplayNodeConfig config;
    std::shared_ptr<RSDisplayNode> node2 = RSDisplayNode::Create(config);
    RSCustomModifierHelper::CreateDrawingContext(node2);
    EXPECT_FALSE(node2->IsInstanceOf<RSCanvasNode>());

    std::shared_ptr<RSNode> node3 = RSCanvasNode::Create();
    ModifierNG::RSDrawingContext context3 = RSCustomModifierHelper::CreateDrawingContext(node3);
    EXPECT_NE(context3.canvas, nullptr);
}

/**
 * @tc.name: FinishDrawingTest
 * @tc.desc: Test the function FinishDrawing
 * @tc.type: FUNC
 */
HWTEST_F(RSCustomModifierHelperTest, FinishDrawingTest, TestSize.Level1)
{
    float width = 100.0f;
    float height = 100.0f;
    ModifierNG::RSDrawingContext context1 = { nullptr, width, height };
    auto drawCmdList1 = RSCustomModifierHelper::FinishDrawing(context1);
    EXPECT_EQ(drawCmdList1, nullptr);

    auto canvas2 = new ExtendRecordingCanvas(width, height);
    ModifierNG::RSDrawingContext context2 = { canvas2, width, height };
    RSSystemProperties::SetDrawTextAsBitmap(true);
    auto drawCmdList2 = RSCustomModifierHelper::FinishDrawing(context2);
    EXPECT_NE(drawCmdList2, nullptr);

    auto canvas3 = new ExtendRecordingCanvas(width, height);
    ModifierNG::RSDrawingContext context3 = { canvas3, width, height };
    RSSystemProperties::SetDrawTextAsBitmap(false);
    auto drawCmdList3 = RSCustomModifierHelper::FinishDrawing(context3);
    EXPECT_NE(drawCmdList3, nullptr);

    auto canvas4 = new ExtendRecordingCanvas(width, height);
    ModifierNG::RSDrawingContext context4 = { canvas4, width, height };
    canvas4->cmdList_ = nullptr;
    auto drawCmdList4 = RSCustomModifierHelper::FinishDrawing(context4);
    EXPECT_EQ(drawCmdList4, nullptr);
}
} // namespace OHOS::Rosen