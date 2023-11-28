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

#include "gtest/gtest.h"

#include "render/rs_pixel_map_util.h"


using namespace testing;
using namespace testing::ext;
namespace OHOS {
namespace Rosen {

class RSPixelMapUtilTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSPixelMapUtilTest::SetUpTestCase() {}
void RSPixelMapUtilTest::TearDownTestCase() {}
void RSPixelMapUtilTest::SetUp() {}
void RSPixelMapUtilTest::TearDown() {}

static std::shared_ptr<Media::PixelMap> CreatePixelMap(int width, int height)
{
    Media::InitializationOptions opts;
    opts.size.width = width;
    opts.size.height = height;
    auto pixelmap = Media::PixelMap::Create(opts);
    auto address = const_cast<uint32_t*>(pixelmap->GetPixel32(0, 0));
    if (address == nullptr) {
        return nullptr;
    }
    SkImageInfo info =
        SkImageInfo::Make(pixelmap->GetWidth(), pixelmap->GetHeight(), kRGBA_8888_SkColorType, kPremul_SkAlphaType);
    auto surface = SkSurface::MakeRasterDirect(info, address, pixelmap->GetRowBytes());
    auto canvas = surface->getCanvas();
    canvas->clear(SK_ColorYELLOW);
    SkPaint paint;
    paint.setColor(SK_ColorRED);
    canvas->drawRect(SkRect::MakeXYWH(width / 4, height / 4, width / 2, height / 2), paint);
    return pixelmap;
}

/**
 * @tc.name: ExtractSkImage
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(RSPixelMapUtilTest, ExtractSkImage, TestSize.Level1)
{
    std::shared_ptr<Media::PixelMap> pixelmap;
    int width = 200;
    int height = 300;
    pixelmap = CreatePixelMap(width, height);

    EXPECT_NE(nullptr, RSPixelMapUtil::ExtractSkImage(pixelmap));
}
} // namespace Rosen
} // namespace OHOS