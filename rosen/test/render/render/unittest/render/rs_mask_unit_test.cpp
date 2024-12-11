/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include <securec.h>

#include "gtest/gtest.h"
#include "include/render/rs_mask.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSMaskUnitTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSMaskUnitTest::SetUpTestCase() {}
void RSMaskUnitTest::TearDownTestCase() {}
void RSMaskUnitTest::SetUp() {}
void RSMaskUnitTest::TearDown() {}

/**
 * @tc.name: TestIsGradientMask01
 * @tc.desc: Verify function IsGradientMask
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSMaskUnitTest, TestIsGradientMask01, TestSize.Level1)
{
    Drawing::Brush brush;
    std::shared_ptr<RSMask> testMask = RSMask::CreateGradientMask(brush);
    ASSERT_TRUE(testMask != nullptr);

    ASSERT_TRUE(testMask->IsGradientMask());
}

/**
 * @tc.name: TestIsPathMask01
 * @tc.desc: Verify function IsPathMask
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSMaskUnitTest, TestIsPathMask01, TestSize.Level1)
{
    Drawing::Brush brush;
    Drawing::Path path;
    std::shared_ptr<RSMask> testMask = RSMask::CreatePathMask(path, brush);
    ASSERT_TRUE(testMask != nullptr);

    ASSERT_TRUE(testMask->IsPathMask());
}

/**
 * @tc.name: TestCreatePathMask01
 * @tc.desc: Verify function CreatePathMask
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMaskUnitTest, TestCreatePathMask01, TestSize.Level1)
{
    Drawing::Brush brush;
    Drawing::Pen maskPen;
    Drawing::Path path;
    std::shared_ptr<RSMask> testMask = RSMask::CreatePathMask(path, maskPen, brush);
    EXPECT_TRUE(testMask != nullptr);
}

/**
 * @tc.name: LifeCycle003
 * @tc.desc: Verify function IsSvgMask
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMaskUnitTest, TestIsSvgMask01, TestSize.Level1)
{
    double x = 0.0f;
    double y = 0.0f;
    double scaleX = 1.0f;
    double scaleY = 1.0f;
    sk_sp<SkSVGDOM> svgDom = nullptr;
    std::shared_ptr<RSMask> testMask = RSMask::CreateSVGMask(x, y, scaleX, scaleY, svgDom);
    ASSERT_TRUE(testMask != nullptr);

    ASSERT_TRUE(testMask->IsSvgMask());
    ASSERT_TRUE(testMask->GetSvgDom() == svgDom);
}

/**
 * @tc.name: TestGetMaskBrush01
 * @tc.desc: Verify function GetMaskBrush
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSMaskUnitTest, TestGetMaskBrush01, TestSize.Level1)
{
    Drawing::Brush brush;
    std::shared_ptr<RSMask> testMask = RSMask::CreateGradientMask(brush);
    ASSERT_TRUE(testMask != nullptr);

    ASSERT_TRUE(testMask->GetMaskBrush() == brush);
}

/**
 * @tc.name: TestCreatePathMask02
 * @tc.desc: Verify function CreatePathMask
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSMaskUnitTest, TestCreatePathMask02, TestSize.Level1)
{
    Drawing::Brush brush;
    Drawing::Path path;
    std::shared_ptr<RSMask> testMask = RSMask::CreatePathMask(path, brush);
    ASSERT_TRUE(testMask != nullptr);
}

/**
 * @tc.name: TestCreateSVGMask01
 * @tc.desc: Verify function CreateSVGMask
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMaskUnitTest, TestCreateSVGMask01, TestSize.Level1)
{
    double x = 0.0f;
    double y = 0.0f;
    double scaleX = 1.0f;
    double scaleY = 1.0f;
    sk_sp<SkSVGDOM> svgDom = nullptr;
    std::shared_ptr<RSMask> testMask = RSMask::CreateSVGMask(x, y, scaleX, scaleY, svgDom);
    ASSERT_TRUE(testMask != nullptr);

    ASSERT_EQ(testMask->GetSvgX(), x);
    ASSERT_EQ(testMask->GetSvgY(), y);
    ASSERT_EQ(testMask->GetScaleX(), scaleX);
    ASSERT_EQ(testMask->GetScaleY(), scaleY);
}

/**
 * @tc.name: TestCreateSVGMask02
 * @tc.desc: Verify function CreateSVGMask
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMaskUnitTest, TestCreateSVGMask02, TestSize.Level1)
{
    double x = 0.0f;
    double y = 0.0f;
    double scaleX = 1.0f;
    double scaleY = 1.0f;
    sk_sp<SkSVGDOM> svgDom = nullptr;
    std::shared_ptr<RSMask> testMask = RSMask::CreateSVGMask(x, y, scaleX, scaleY, svgDom);
    ASSERT_TRUE(testMask != nullptr);

    testMask->SetSvgX(1.0f);
    ASSERT_EQ(testMask->GetSvgX(), 1.0f);

    testMask->SetSvgY(1.0f);
    ASSERT_EQ(testMask->GetSvgY(), 1.0f);

    testMask->SetScaleX(1.0f);
    ASSERT_EQ(testMask->GetScaleX(), 1.0f);

    testMask->SetScaleY(1.0f);
    ASSERT_EQ(testMask->GetScaleY(), 1.0f);
}

/**
 * @tc.name: TestGetSvgPicture01
 * @tc.desc: Verify function GetSvgPicture
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMaskUnitTest, TestGetSvgPicture01, TestSize.Level1)
{
    Drawing::Brush brush;
    Drawing::Path path;
    std::shared_ptr<RSMask> testMask = RSMask::CreatePathMask(path, brush);
    ASSERT_TRUE(testMask != nullptr);

    Drawing::Path path2;
    testMask->SetMaskPath(path2);
    auto path3 = testMask->GetMaskPath();
    ASSERT_TRUE(path2.GetBounds() == path3->GetBounds());

    ASSERT_TRUE(!testMask->GetSvgPicture());
}

/**
 * @tc.name: TestGetMaskBrush02
 * @tc.desc: Verify function GetMaskBrush
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMaskUnitTest, TestGetMaskBrush02, TestSize.Level1)
{
    Drawing::Brush brush;
    std::shared_ptr<RSMask> testMask = RSMask::CreateGradientMask(brush);
    ASSERT_TRUE(testMask != nullptr);

    Drawing::Brush brush2;
    testMask->SetMaskBrush(brush2);
    ASSERT_TRUE(testMask->GetMaskBrush() == brush2);
}

/**
 * @tc.name: TestCreatePathMask03
 * @tc.desc: Verify function CreatePathMask
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMaskUnitTest, TestCreatePathMask03, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create RSMask by Path
     */
    Drawing::Brush brush;
    Drawing::Pen pen;
    Drawing::Path path;
    std::shared_ptr<RSMask> testMask = RSMask::CreatePathMask(path, pen, brush);
    ASSERT_TRUE(testMask != nullptr);
}

/**
 * @tc.name: TestGetMaskPath01
 * @tc.desc: Verify function GetMaskPath
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMaskUnitTest, TestGetMaskPath01, TestSize.Level1)
{
    auto testMask = std::make_shared<RSMask>();
    EXPECT_TRUE(testMask->GetMaskPath() != nullptr);
}

/**
 * @tc.name: TestSetMaskPen01
 * @tc.desc: Verify function SetMaskPen
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMaskUnitTest, TestSetMaskPen01, TestSize.Level1)
{
    auto testMask = std::make_shared<RSMask>();
    Drawing::Pen pen;
    testMask->SetMaskPen(pen);
    EXPECT_EQ(testMask->GetMaskPen(), pen);
}

/**
 * @tc.name: TestGetMaskPen01
 * @tc.desc: Verify function GetMaskPen
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMaskUnitTest, TestGetMaskPen01, TestSize.Level1)
{
    auto testMask = std::make_shared<RSMask>();
    EXPECT_EQ(testMask->GetMaskPen().GetPathEffect(), nullptr);
}

/**
 * @tc.name: TestGetSvgDom01
 * @tc.desc: Verify function GetSvgDom
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMaskUnitTest, TestGetSvgDom01, TestSize.Level1)
{
    auto testMask = std::make_shared<RSMask>();
    EXPECT_TRUE(testMask->GetSvgDom() == nullptr);
}

/**
 * @tc.name: TestGetSvgPicture01
 * @tc.desc: Verify function GetSvgPicture
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMaskUnitTest, TestGetSvgPicture01, TestSize.Level1)
{
    auto testMask = std::make_shared<RSMask>();
    EXPECT_TRUE(testMask->GetSvgPicture() == nullptr);
}

/**
 * @tc.name: TestMarshalling01
 * @tc.desc: Verify function Marshalling
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMaskUnitTest, TestMarshalling01, TestSize.Level1)
{
    Parcel parcel;
    auto testMask = std::make_shared<RSMask>();
    EXPECT_TRUE(testMask->Marshalling(parcel));
}

/**
 * @tc.name: TestMarshallingPathAndBrush01
 * @tc.desc: Verify function MarshallingPathAndBrush
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMaskUnitTest, TestMarshallingPathAndBrush01, TestSize.Level1)
{
    Parcel parcel;
    auto testMask = std::make_shared<RSMask>();
    EXPECT_TRUE(testMask->MarshallingPathAndBrush(parcel));
}

/**
 * @tc.name: TestMarshlingandUnMarshling01
 * @tc.desc: Verify function MarshlingandUnMarshling
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMaskUnitTest, TestMarshlingandUnMarshling01, TestSize.Level1)
{
    Drawing::Brush brush;
    std::shared_ptr<RSMask> testMask = RSMask::CreateGradientMask(brush);
    ASSERT_TRUE(testMask != nullptr);
    Parcel parcel;
    char* buffer = static_cast<char*>(malloc(parcel.GetMaxCapacity()));
    auto bufferSize = parcel.GetMaxCapacity();
    memset_s(buffer, parcel.GetMaxCapacity(), 0, parcel.GetMaxCapacity());
    ASSERT_TRUE(parcel.WriteUnpadBuffer(buffer, parcel.GetMaxCapacity()));
    bool ret = false;
    parcel.SkipBytes(bufferSize);
    while (!ret) {
        size_t position = parcel.GetWritePosition();
        ret = testMask->Marshalling(parcel) && (RSMask::Unmarshalling(parcel) != nullptr);
        parcel.SetMaxCapacity(parcel.GetMaxCapacity() + 1);
        if (!ret) {
            parcel.RewindWrite(position);
            parcel.RewindRead(bufferSize);
        }
    }
    free(buffer);
    ASSERT_TRUE(ret);
}

/**
 * @tc.name: TestCreatePixelMapMask01
 * @tc.desc: Verify function CreatePixelMapMask
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMaskUnitTest, TestCreatePixelMapMask01, TestSize.Level1)
{
    std::shared_ptr<Media::PixelMap> pixelmap = nullptr;
    std::shared_ptr<RSMask> testMask = RSMask::CreatePixelMapMask(pixelmap);
    ASSERT_TRUE(testMask != nullptr);
    testMask->SetPixelMap(pixelmap);
    ASSERT_TRUE(testMask->GetImage() == nullptr);
    EXPECT_EQ(testMask->GetPixelMap(), nullptr);
    ASSERT_TRUE(testMask->IsPixelMapMask());
}

/**
 * @tc.name: TestCreatePixelMapMask02
 * @tc.desc: Verify function CreatePixelMapMask
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMaskUnitTest, TestCreatePixelMapMask02, TestSize.Level1)
{
    Drawing::Brush brush;
    std::shared_ptr<RSMask> testMask = RSMask::CreateGradientMask(brush);
    ASSERT_TRUE(testMask != nullptr);
    ASSERT_TRUE(testMask->IsPixelMapMask() == false);
}

/**
 * @tc.name: TestGetPixelMap01
 * @tc.desc: Verify function GetPixelMap
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSMaskUnitTest, TestGetPixelMap01, TestSize.Level1)
{
    Drawing::Brush brush;
    std::shared_ptr<RSMask> testMask = RSMask::CreateGradientMask(brush);
    ASSERT_EQ(testMask->GetPixelMap(), nullptr);
}

/**
 * @tc.name: TestDump01
 * @tc.desc: Verify function Dump
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMaskUnitTest, TestDump01, TestSize.Level1)
{
    Drawing::Brush brush;
    std::shared_ptr<RSMask> testMask = RSMask::CreateGradientMask(brush);
    std::string out;
    EXPECT_TRUE(out.empty());
    testMask->Dump(out);
    EXPECT_FALSE(out.empty());
}
} // namespace OHOS::Rosen