/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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
class RSMaskTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSMaskTest::SetUpTestCase() {}
void RSMaskTest::TearDownTestCase() {}
void RSMaskTest::SetUp() {}
void RSMaskTest::TearDown() {}

/**
 * @tc.name: LifeCycle001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSMaskTest, LifeCycle001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create RSMask by Gradient
     */
    Drawing::Brush brush;
    std::shared_ptr<RSMask> mask = RSMask::CreateGradientMask(brush);
    ASSERT_TRUE(mask != nullptr);

    ASSERT_TRUE(mask->IsGradientMask());
}

/**
 * @tc.name: LifeCycle002
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSMaskTest, LifeCycle002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create RSMask by Path
     */
    Drawing::Brush brush;
    Drawing::Path path;
    std::shared_ptr<RSMask> mask = RSMask::CreatePathMask(path, brush);
    ASSERT_TRUE(mask != nullptr);

    ASSERT_TRUE(mask->IsPathMask());
}

/**
 * @tc.name: CreatePathMaskTest002
 * @tc.desc: Verify function CreatePathMask
 * @tc.type:FUNC
 */
HWTEST_F(RSMaskTest, CreatePathMaskTest002, TestSize.Level1)
{
    Drawing::Brush brush;
    Drawing::Pen maskPen;
    Drawing::Path path;
    std::shared_ptr<RSMask> mask = RSMask::CreatePathMask(path, maskPen, brush);
    EXPECT_TRUE(mask != nullptr);
}

/**
 * @tc.name: LifeCycle003
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSMaskTest, LifeCycle003, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create RSMask by SVG
     */
    double x = 0.0f;
    double y = 0.0f;
    double scaleX = 1.0f;
    double scaleY = 1.0f;
    sk_sp<SkSVGDOM> svgDom = nullptr;
    std::shared_ptr<RSMask> mask = RSMask::CreateSVGMask(x, y, scaleX, scaleY, svgDom);
    ASSERT_TRUE(mask != nullptr);

    ASSERT_TRUE(mask->IsSvgMask());
}

/**
 * @tc.name: LifeCycle004
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSMaskTest, LifeCycle004, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create RSMask by Gradient
     */
    Drawing::Brush brush;
    std::shared_ptr<RSMask> mask = RSMask::CreateGradientMask(brush);
    ASSERT_TRUE(mask != nullptr);

    /**
     * @tc.steps: step2. check set value
     */
    ASSERT_TRUE(mask->GetMaskBrush() == brush);
}

/**
 * @tc.name: LifeCycle005
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSMaskTest, LifeCycle005, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create RSMask by Path
     */
    Drawing::Brush brush;
    Drawing::Path path;
    std::shared_ptr<RSMask> mask = RSMask::CreatePathMask(path, brush);
    ASSERT_TRUE(mask != nullptr);

    /**
     * @tc.steps: step2. check set value
     */
}

/**
 * @tc.name: LifeCycle006
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSMaskTest, LifeCycle006, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create RSMask by SVG
     */
    double x = 0.0f;
    double y = 0.0f;
    double scaleX = 1.0f;
    double scaleY = 1.0f;
    sk_sp<SkSVGDOM> svgDom = nullptr;
    std::shared_ptr<RSMask> mask = RSMask::CreateSVGMask(x, y, scaleX, scaleY, svgDom);
    ASSERT_TRUE(mask != nullptr);

    /**
     * @tc.steps: step2. check set value
     */
    ASSERT_EQ(mask->GetSvgX(), x);
    ASSERT_EQ(mask->GetSvgY(), y);
    ASSERT_EQ(mask->GetScaleX(), scaleX);
    ASSERT_EQ(mask->GetScaleY(), scaleY);
}

/**
 * @tc.name: LifeCycle007
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSMaskTest, LifeCycle007, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create RSMask by SVG
     */
    double x = 0.0f;
    double y = 0.0f;
    double scaleX = 1.0f;
    double scaleY = 1.0f;
    sk_sp<SkSVGDOM> svgDom = nullptr;
    std::shared_ptr<RSMask> mask = RSMask::CreateSVGMask(x, y, scaleX, scaleY, svgDom);
    ASSERT_TRUE(mask != nullptr);

    /**
     * @tc.steps: step2. check set value
     */
    mask->SetSvgX(1.0f);
    ASSERT_EQ(mask->GetSvgX(), 1.0f);

    mask->SetSvgY(1.0f);
    ASSERT_EQ(mask->GetSvgY(), 1.0f);

    mask->SetScaleX(1.0f);
    ASSERT_EQ(mask->GetScaleX(), 1.0f);

    mask->SetScaleY(1.0f);
    ASSERT_EQ(mask->GetScaleY(), 1.0f);
}

/**
 * @tc.name: LifeCycle008
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSMaskTest, LifeCycle008, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create RSMask by Path
     */
    Drawing::Brush brush;
    Drawing::Path path;
    std::shared_ptr<RSMask> mask = RSMask::CreatePathMask(path, brush);
    ASSERT_TRUE(mask != nullptr);

    /**
     * @tc.steps: step2. check set value
     */
    Drawing::Path path2;
    mask->SetMaskPath(path2);
}

/**
 * @tc.name: LifeCycle009
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSMaskTest, LifeCycle009, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create RSMask by Gradient
     */
    Drawing::Brush brush;
    std::shared_ptr<RSMask> mask = RSMask::CreateGradientMask(brush);
    ASSERT_TRUE(mask != nullptr);

    /**
     * @tc.steps: step2. check set value
     */
    Drawing::Brush brush2;
    mask->SetMaskBrush(brush2);
    ASSERT_TRUE(mask->GetMaskBrush() == brush2);
}

/**
 * @tc.name: GetMaskPathTest001
 * @tc.desc: Verify function GetMaskPath
 * @tc.type:FUNC
 */
HWTEST_F(RSMaskTest, GetMaskPathTest001, TestSize.Level1)
{
    auto mask = std::make_shared<RSMask>();
    EXPECT_TRUE(mask->GetMaskPath() != nullptr);
}

/**
 * @tc.name: GetMaskPenTest001
 * @tc.desc: Verify function GetMaskPen
 * @tc.type:FUNC
 */
HWTEST_F(RSMaskTest, GetMaskPenTest001, TestSize.Level1)
{
    auto mask = std::make_shared<RSMask>();
    EXPECT_EQ(mask->GetMaskPen().GetPathEffect(), nullptr);
}

/**
 * @tc.name: GetSvgDomTest001
 * @tc.desc: Verify function GetSvgDom
 * @tc.type:FUNC
 */
HWTEST_F(RSMaskTest, GetSvgDomTest001, TestSize.Level1)
{
    auto mask = std::make_shared<RSMask>();
    EXPECT_TRUE(mask->GetSvgDom() == nullptr);
}

/**
 * @tc.name: GetSvgPictureTest001
 * @tc.desc: Verify function GetSvgPicture
 * @tc.type:FUNC
 */
HWTEST_F(RSMaskTest, GetSvgPictureTest001, TestSize.Level1)
{
    auto mask = std::make_shared<RSMask>();
    EXPECT_TRUE(mask->GetSvgPicture() == nullptr);
}

/**
 * @tc.name: MarshallingTest001
 * @tc.desc: Verify function Marshalling
 * @tc.type:FUNC
 */
HWTEST_F(RSMaskTest, MarshallingTest001, TestSize.Level1)
{
    Parcel parcel;
    auto mask = std::make_shared<RSMask>();
    EXPECT_TRUE(mask->Marshalling(parcel));
}

/**
 * @tc.name: MarshallingPathAndBrushTest001
 * @tc.desc: Verify function MarshallingPathAndBrush
 * @tc.type:FUNC
 */
HWTEST_F(RSMaskTest, MarshallingPathAndBrushTest001, TestSize.Level1)
{
    Parcel parcel;
    auto mask = std::make_shared<RSMask>();
    EXPECT_TRUE(mask->MarshallingPathAndBrush(parcel));
}

/**
 * @tc.name: MarshlingandUnMarshling001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSMaskTest, MarshlingandUnMarshling001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create RSMask by Gradient
     */
    Drawing::Brush brush;
    std::shared_ptr<RSMask> mask = RSMask::CreateGradientMask(brush);
    ASSERT_TRUE(mask != nullptr);
    Parcel parcel;
    char* buffer = static_cast<char*>(malloc(parcel.GetMaxCapacity()));
    auto bufferSize = parcel.GetMaxCapacity();
    memset_s(buffer, parcel.GetMaxCapacity(), 0, parcel.GetMaxCapacity());
    ASSERT_TRUE(parcel.WriteUnpadBuffer(buffer, parcel.GetMaxCapacity()));
    bool ret = false;
    parcel.SkipBytes(bufferSize);
    while (!ret) {
        size_t position = parcel.GetWritePosition();
        ret = mask->Marshalling(parcel) && (RSMask::Unmarshalling(parcel) != nullptr);
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
 * @tc.name: CreatePixelMapMask
 * @tc.desc:
 * @tc.type:FUNC`
 */
HWTEST_F(RSMaskTest, CreatePixelMapMask001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create RSMask by PixelMap
     */
    std::shared_ptr<Media::PixelMap> pixelmap = nullptr;
    std::shared_ptr<RSMask> mask = RSMask::CreatePixelMapMask(pixelmap);
    ASSERT_TRUE(mask != nullptr);
    mask->SetPixelMap(pixelmap);
    ASSERT_TRUE(mask->GetImage() == nullptr);
    ASSERT_TRUE(mask->IsPixelMapMask());
}

/**
 * @tc.name: CreatePixelMapMask
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSMaskTest, CreatePixelMapMask002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create RSMask by PixelMap
     */
    Drawing::Brush brush;
    std::shared_ptr<RSMask> mask = RSMask::CreateGradientMask(brush);
    ASSERT_TRUE(mask != nullptr);
    ASSERT_TRUE(mask->IsPixelMapMask() == false);
}
} // namespace OHOS::Rosen