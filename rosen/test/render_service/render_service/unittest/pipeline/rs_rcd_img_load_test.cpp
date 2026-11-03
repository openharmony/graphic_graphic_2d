/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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
#include "feature/round_corner_display/rs_rcd_bitmap_utils.h"
#include "feature/round_corner_display/rs_rcd_image_loader.h"
#include "feature/round_corner_display/rs_round_corner_config.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSRcdImageLoaderTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();

    void SetUp() override;
    void TearDown() override;
};

void RSRcdImageLoaderTest::SetUpTestCase() {}
void RSRcdImageLoaderTest::TearDownTestCase() {}
void RSRcdImageLoaderTest::SetUp() {}
void RSRcdImageLoaderTest::TearDown() {}

/*
 * @tc.name: RSRcdImageLoaderTest
 * @tc.desc: Test RSRcdImageLoaderTest.RCDImageLoaderCreateTest
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRcdImageLoaderTest, RCDImageLoaderCreateTest, TestSize.Level1)
{
    auto loaderFully = RCDImageLoader::Create(rs_rcd::ImageLoadType::ROG_FULLY_LOAD, true, true, true);
    EXPECT_TRUE(loaderFully != nullptr);

    auto loaderPartially = RCDImageLoader::Create(rs_rcd::ImageLoadType::ROG_PARTIALLY_LOAD, true, true, true);
    EXPECT_TRUE(loaderPartially != nullptr);

    auto loaderEnd = RCDImageLoader::Create(rs_rcd::ImageLoadType::END, true, true, true);
    EXPECT_TRUE(loaderEnd != nullptr);

    auto loaderDefault = RCDImageLoader::Create(static_cast<rs_rcd::ImageLoadType>(100), true, true, true);
    EXPECT_TRUE(loaderDefault != nullptr);
}

/*
 * @tc.name: RSRcdImageLoaderTest
 * @tc.desc: Test RSRcdImageLoaderTest.RCDImageLoaderLoadImgTest
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRcdImageLoaderTest, RCDImageLoaderLoadImgTest, TestSize.Level1)
{
    std::shared_ptr<Drawing::Image> img = nullptr;
    bool result = RCDBitmapUtils::LoadImg(nullptr, img);
    EXPECT_FALSE(result);
    EXPECT_TRUE(img == nullptr);

    result = RCDBitmapUtils::LoadImg("nonexistent.png", img);
    EXPECT_FALSE(result);

    result = RCDBitmapUtils::LoadImg("", img);
    EXPECT_FALSE(result);
}

/*
 * @tc.name: RSRcdImageLoaderTest
 * @tc.desc: Test RSRcdImageLoaderTest.RCDImageLoaderDecodeBitmapTest
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRcdImageLoaderTest, RCDImageLoaderDecodeBitmapTest, TestSize.Level1)
{
    Drawing::Bitmap bitmap;
    bool result = RCDBitmapUtils::DecodeBitmap(nullptr, bitmap);
    EXPECT_FALSE(result);

    std::shared_ptr<Drawing::Image> invalidImg = std::make_shared<Drawing::Image>();
    result = RCDBitmapUtils::DecodeBitmap(invalidImg, bitmap);
    EXPECT_FALSE(result);
}

/*
 * @tc.name: RSRcdImageLoaderTest
 * @tc.desc: Test RSRcdImageLoaderTest.RCDImageLoaderLoadBitmapTest
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRcdImageLoaderTest, RCDImageLoaderLoadBitmapTest, TestSize.Level1)
{
    auto bitmapPtr = RCDBitmapUtils::LoadBitmap(nullptr);
    EXPECT_TRUE(bitmapPtr == nullptr);

    bitmapPtr = RCDBitmapUtils::LoadBitmap("nonexistent.png");
    EXPECT_TRUE(bitmapPtr == nullptr);
}

} // OHOS::Rosen