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

#include "image/image.h"
#include "recording/cmd_list_helper.h"
#include "recording/draw_cmd.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace Drawing {
class CmdListHelperTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void CmdListHelperTest::SetUpTestCase() {}
void CmdListHelperTest::TearDownTestCase() {}
void CmdListHelperTest::SetUp() {}
void CmdListHelperTest::TearDown() {}

/**
 * @tc.name: AddImageToCmdList001
 * @tc.desc: Test the AddImageToCmdList function.
 * @tc.type: FUNC
 * @tc.require: I7SO7X
 */
HWTEST_F(CmdListHelperTest, AddImageToCmdList001, TestSize.Level1)
{
    CmdList cmdList;
    std::shared_ptr<Image> image = nullptr;
    ImageHandle handle = CmdListHelper::AddImageToCmdList(cmdList, image);
    EXPECT_EQ(handle.offset, 0);
    EXPECT_EQ(handle.size, 0);
}

/**
 * @tc.name: GetImageFromCmdList001
 * @tc.desc: Test the GetImageFromCmdList function.
 * @tc.type: FUNC
 * @tc.require: I7SO7X
 */
HWTEST_F(CmdListHelperTest, GetImageFromCmdList001, TestSize.Level1)
{
    CmdList cmdList;
    ImageHandle handle = {};
    auto imageData = std::make_shared<Data>();
    imageData->BuildUninitialized(10);
    handle.offset = cmdList.AddImageData(imageData->GetData(), imageData->GetSize());
    handle.size = imageData->GetSize();
    auto image = CmdListHelper::GetImageFromCmdList(cmdList, handle);
    EXPECT_EQ(image, nullptr);
}

/**
 * @tc.name: AddBitmapToCmdList001
 * @tc.desc: Test the AddBitmapToCmdList function.
 * @tc.type: FUNC
 * @tc.require: I7SO7X
 */
HWTEST_F(CmdListHelperTest, AddBitmapToCmdList001, TestSize.Level1)
{
    CmdList cmdList;
    Bitmap bitmap;
    ImageHandle handle = CmdListHelper::AddBitmapToCmdList(cmdList, bitmap);
    EXPECT_EQ(handle.offset, 0);
    EXPECT_EQ(handle.size, 0);

    BitmapFormat format = { ColorType::COLORTYPE_ALPHA_8, AlphaType::ALPHATYPE_PREMUL };
    bitmap.Build(100, 100, format);
    handle = CmdListHelper::AddBitmapToCmdList(cmdList, bitmap);
    EXPECT_EQ(handle.offset, 0);
    EXPECT_EQ(handle.size, 100 * 100);
}

/**
 * @tc.name: GetBitmapFromCmdList001
 * @tc.desc: Test the GetBitmapFromCmdList function.
 * @tc.type: FUNC
 * @tc.require: I7SO7X
 */
HWTEST_F(CmdListHelperTest, GetBitmapFromCmdList001, TestSize.Level1)
{
    CmdList cmdList;
    ImageHandle handle = {};
    auto bitmap = CmdListHelper::GetBitmapFromCmdList(cmdList, handle);
    EXPECT_EQ(bitmap, nullptr);

    handle.size = 100;
    bitmap = CmdListHelper::GetBitmapFromCmdList(cmdList, handle);
    EXPECT_EQ(bitmap, nullptr);
}

/**
 * @tc.name: GetPictureFromCmdList001
 * @tc.desc: Test the GetPictureFromCmdList function.
 * @tc.type: FUNC
 * @tc.require: I7SO7X
 */
HWTEST_F(CmdListHelperTest, GetPictureFromCmdList001, TestSize.Level1)
{
    CmdList cmdList;
    ImageHandle handle = {};
    auto imageData = std::make_shared<Data>();
    imageData->BuildUninitialized(10);
    handle.offset = cmdList.AddImageData(imageData->GetData(), imageData->GetSize());
    handle.size = imageData->GetSize();
    auto picture = CmdListHelper::GetPictureFromCmdList(cmdList, handle);
    EXPECT_EQ(picture, nullptr);
}

/**
 * @tc.name: AddCompressDataToCmdList001
 * @tc.desc: Test the AddCompressDataToCmdList function.
 * @tc.type: FUNC
 * @tc.require: I7SO7X
 */
HWTEST_F(CmdListHelperTest, AddCompressDataToCmdList001, TestSize.Level1)
{
    CmdList cmdList;
    Bitmap bitmap;
    std::shared_ptr<Data> data = nullptr;
    ImageHandle handle = CmdListHelper::AddCompressDataToCmdList(cmdList, data);
    EXPECT_EQ(handle.offset, 0);
    EXPECT_EQ(handle.size, 0);

    data = std::make_shared<Data>();
    handle = CmdListHelper::AddCompressDataToCmdList(cmdList, data);
    EXPECT_EQ(handle.offset, 0);
    EXPECT_EQ(handle.size, 0);

    data->BuildUninitialized(10);
    handle = CmdListHelper::AddCompressDataToCmdList(cmdList, data);
    EXPECT_EQ(handle.offset, 0);
    EXPECT_EQ(handle.size, data->GetSize());

    auto newData = CmdListHelper::GetCompressDataFromCmdList(cmdList, handle);
    EXPECT_NE(newData, nullptr);
}

/**
 * @tc.name: AddChildToCmdList001
 * @tc.desc: Test the AddChildToCmdList function.
 * @tc.type: FUNC
 * @tc.require: I7SO7X
 */
HWTEST_F(CmdListHelperTest, AddChildToCmdList001, TestSize.Level1)
{
    CmdList cmdList;
    std::shared_ptr<CmdList> childCmdList = nullptr;
    auto handle = CmdListHelper::AddChildToCmdList(cmdList, childCmdList);
    EXPECT_EQ(handle.offset, 0);
    EXPECT_EQ(handle.size, 0);

    childCmdList = std::make_shared<CmdList>();
    handle = CmdListHelper::AddChildToCmdList(cmdList, childCmdList);
    EXPECT_EQ(handle.offset, 0);
    EXPECT_EQ(handle.size, 0);

    auto imageData = std::make_shared<Data>();
    imageData->BuildUninitialized(10);
    childCmdList->AddOp<ClearOpItem>(Color::COLOR_BLACK);
    childCmdList->AddImageData(imageData->GetData(), imageData->GetSize());
    handle = CmdListHelper::AddChildToCmdList(cmdList, childCmdList);
    EXPECT_EQ(handle.size, childCmdList->GetData().second);
    EXPECT_EQ(handle.imageSize, imageData->GetSize());
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS