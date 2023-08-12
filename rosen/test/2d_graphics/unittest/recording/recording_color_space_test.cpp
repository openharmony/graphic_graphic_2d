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

#include "recording/recording_color_space.h"
#include "image/image.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace Drawing {
class RecordingColorSpaceTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RecordingColorSpaceTest::SetUpTestCase() {}
void RecordingColorSpaceTest::TearDownTestCase() {}
void RecordingColorSpaceTest::SetUp() {}
void RecordingColorSpaceTest::TearDown() {}

/**
 * @tc.name: CreateSRGB001
 * @tc.desc: Test the playback of the CreateSRGB function.
 * @tc.type: FUNC
 * @tc.require: I76XAX
 */
HWTEST_F(RecordingColorSpaceTest, CreateSRGB001, TestSize.Level1)
{
    auto recordingColorSpace = RecordingColorSpace::CreateSRGB();
    EXPECT_TRUE(recordingColorSpace != nullptr);
    auto colorSpaceCmdList = recordingColorSpace->GetCmdList();
    EXPECT_TRUE(colorSpaceCmdList != nullptr);
    auto colorSpace = colorSpaceCmdList->Playback();
    EXPECT_TRUE(colorSpace != nullptr);

    auto newCmdList = ColorSpaceCmdList::CreateFromData(colorSpaceCmdList->GetData(), true);
    EXPECT_TRUE(newCmdList != nullptr);
    colorSpace = newCmdList->Playback();
    EXPECT_TRUE(colorSpace != nullptr);

    newCmdList = ColorSpaceCmdList::CreateFromData(colorSpaceCmdList->GetData(), false);
    EXPECT_TRUE(newCmdList != nullptr);
    colorSpace = newCmdList->Playback();
    EXPECT_TRUE(colorSpace != nullptr);
}

/**
 * @tc.name: CreateSRGBLinear001
 * @tc.desc: Test the playback of the CreateSRGBLinear function.
 * @tc.type: FUNC
 * @tc.require: I76XAX
 */
HWTEST_F(RecordingColorSpaceTest, CreateSRGBLinear001, TestSize.Level1)
{
    auto recordingColorSpace = RecordingColorSpace::CreateSRGBLinear();
    EXPECT_TRUE(recordingColorSpace != nullptr);
    auto colorSpaceCmdList = recordingColorSpace->GetCmdList();
    EXPECT_TRUE(colorSpaceCmdList != nullptr);
    auto colorSpace = colorSpaceCmdList->Playback();
    EXPECT_TRUE(colorSpace != nullptr);
}

/**
 * @tc.name: CreateRefImage001
 * @tc.desc: Test the playback of the CreateRefImage function.
 * @tc.type: FUNC
 * @tc.require: I76XAX
 */
HWTEST_F(RecordingColorSpaceTest, CreateRefImage001, TestSize.Level1)
{
    Bitmap bmp;
    BitmapFormat format { COLORTYPE_RGBA_8888, ALPHATYPE_OPAQUE };
    bmp.Build(10, 10, format);
    Image image;
    image.BuildFromBitmap(bmp);
    auto recordingColorSpace = RecordingColorSpace::CreateRefImage(image);
    EXPECT_TRUE(recordingColorSpace != nullptr);
    auto colorSpaceCmdList = recordingColorSpace->GetCmdList();
    EXPECT_TRUE(colorSpaceCmdList != nullptr);
    auto colorSpace = colorSpaceCmdList->Playback();
    EXPECT_TRUE(colorSpace != nullptr);
}

/**
 * @tc.name: CreateRGB001
 * @tc.desc: Test the playback of the CreateRGB function.
 * @tc.type: FUNC
 * @tc.require: I76XAX
 */
HWTEST_F(RecordingColorSpaceTest, CreateRGB001, TestSize.Level1)
{
    auto recordingColorSpace = RecordingColorSpace::CreateRGB(CMSTransferFuncType::DOT2, CMSMatrixType::ADOBE_RGB);
    EXPECT_TRUE(recordingColorSpace != nullptr);
    auto colorSpaceCmdList = recordingColorSpace->GetCmdList();
    EXPECT_TRUE(colorSpaceCmdList != nullptr);
    auto colorSpace = colorSpaceCmdList->Playback();
    EXPECT_TRUE(colorSpace != nullptr);
}

/**
 * @tc.name: CreateRGB002
 * @tc.desc: Test the playback of the CreateRGB function.
 * @tc.type: FUNC
 * @tc.require: I76XAX
 */
HWTEST_F(RecordingColorSpaceTest, CreateRGB002, TestSize.Level1)
{
    auto recordingColorSpace = RecordingColorSpace::CreateRGB(CMSTransferFuncType::LINEAR, CMSMatrixType::DCIP3);
    EXPECT_TRUE(recordingColorSpace != nullptr);
    auto colorSpaceCmdList = recordingColorSpace->GetCmdList();
    EXPECT_TRUE(colorSpaceCmdList != nullptr);
    auto colorSpace = colorSpaceCmdList->Playback();
    EXPECT_TRUE(colorSpace != nullptr);
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
