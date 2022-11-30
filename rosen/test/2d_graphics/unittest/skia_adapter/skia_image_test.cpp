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

#include <cstddef>
#include "gtest/gtest.h"
#include "skia_adapter/skia_image.h"
#include "image/image.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace Drawing {
class SkiaImageTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void SkiaImageTest::SetUpTestCase() {}
void SkiaImageTest::TearDownTestCase() {}
void SkiaImageTest::SetUp() {}
void SkiaImageTest::TearDown() {}

/**
 * @tc.name: BuildFromBitmap001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.author:
 */
HWTEST_F(SkiaImageTest, BuildFromBitmap001, TestSize.Level1)
{
    Bitmap bitmap;
    SkiaImage skiaImage;
    auto image = skiaImage.BuildFromBitmap(bitmap);
    EXPECT_EQ(image, nullptr);
}

/**
 * @tc.name: BuildFromPicture001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.author:
 */
HWTEST_F(SkiaImageTest, BuildFromPicture001, TestSize.Level1)
{
    Picture picture;
    SizeI dimensions;
    Matrix matrix;
    Brush brush;
    BitDepth bitDepth = BitDepth::KF16;
    std::shared_ptr<ColorSpace> colorSpace = ColorSpace::CreateSRGB();
    SkiaImage skiaImage;
    auto image = skiaImage.BuildFromPicture(picture, dimensions, matrix, brush, bitDepth, colorSpace);
    EXPECT_EQ(image, nullptr);
}

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS