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
#include "ui/"

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
    RSCustomModifierHelper::CreateDrawingContext(node);
    EXPECT_EQ(node, nullptr);

    bool isRenderServiceNode = false;
    std::shared_ptr<RSNode> node2 = RSRootNode::Create(isRenderServiceNode);
    RSCustomModifierHelper::CreateDrawingContext(node2);
    EXPECT_FALSE(node->IsInstanceOf<RSCanvasNode>());

    std::shared_ptr<RSNode> node3 = RSCanvasNode::Create();
    ModifierNG::RSDrawingContext context = RSCustomModifierHelper::CreateDrawingContext(node3);
    EXPECT_NE(context.canvas, nullptr);
}

/**
 * @tc.name: FinishDrawingTest
 * @tc.desc: Test the function FinishDrawing
 * @tc.type: FUNC
 */
HWTEST_F(RSCustomModifierHelperTest, FinishDrawingTest, TestSize.Level1)
{
    Drawing::Canvas canvas;
    float width = 100.0f;
    float height = 100.0f;
    ModifierNG::RSDrawingContext context = { &canvas, width, height };
    auto drawCmdList = RSCustomModifierHelper::FinishDrawing(context);
    EXPECT_EQ(drawCmdList, nullptr);

    ModifierNG::RSDrawingContext context2 = { nullptr, width, height };
    auto drawCmdList2 = RSCustomModifierHelper::FinishDrawing(context);
    EXPECT_EQ(drawCmdList2, nullptr);

    auto recordingCanvas = new ExtendRecordingCanvas(width, height);
    ModifierNG::RSDrawingContext context3 = { recordingCanvas, width, height };
    RSSystemProperties::SetDrawTextAsBitmap(true);
    auto drawCmdList3 = RSCustomModifierHelper::FinishDrawing(context3);
    EXPECT_NE(drawCmdList3, nullptr);
}
} // namespace OHOS::Rosen