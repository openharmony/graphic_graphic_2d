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

#include "recording/draw_cmd.h"
#include "recording/draw_cmd_list.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace Drawing {
class DrawCmdTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void DrawCmdTest::SetUpTestCase() {}
void DrawCmdTest::TearDownTestCase() {}
void DrawCmdTest::SetUp() {}
void DrawCmdTest::TearDown() {}

/**
 * @tc.name: DrawCmdList001
 * @tc.desc: Test the creation of CmdList.
 * @tc.type: FUNC
 * @tc.require: I7SO7X
 */
HWTEST_F(DrawCmdTest, DrawCmdList001, TestSize.Level1)
{
    auto drawCmdList = DrawCmdList::CreateFromData({ nullptr, 0 }, false);
    drawCmdList->SetWidth(10);
    drawCmdList->SetHeight(20);
    drawCmdList->AddOp<ClearOpItem::ConstructorHandle>(Color::COLOR_BLACK);
    auto cmdData = drawCmdList->GetData();

    auto newDrawCmdList = DrawCmdList::CreateFromData(cmdData, true);
    EXPECT_TRUE(newDrawCmdList != nullptr);
    newDrawCmdList->SetWidth(drawCmdList->GetWidth());
    newDrawCmdList->SetHeight(drawCmdList->GetHeight());
    EXPECT_EQ(newDrawCmdList->GetWidth(), drawCmdList->GetWidth());
    EXPECT_EQ(newDrawCmdList->GetHeight(), drawCmdList->GetHeight());

    CmdListData cmdListData = { nullptr, 0 };
    newDrawCmdList = DrawCmdList::CreateFromData(cmdListData, false);
    EXPECT_TRUE(newDrawCmdList != nullptr);
    EXPECT_EQ(newDrawCmdList->GetWidth(), 0.f);
    EXPECT_EQ(newDrawCmdList->GetHeight(), 0.f);

    auto imageData = drawCmdList->GetAllImageData();
    std::vector<std::shared_ptr<Media::PixelMap>> pixelMapVec;
    drawCmdList->GetAllPixelMap(pixelMapVec);
    auto cmdList = DrawCmdList::CreateFromData(cmdData, false);
    cmdList->SetUpImageData(imageData.first, imageData.second);
    cmdList->SetupPixelMap(pixelMapVec);
    auto pixelMap = cmdList->GetPixelMap(0);
    EXPECT_TRUE(pixelMap == nullptr);
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS