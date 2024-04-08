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
    Drawing::Bitmap bitmap;
    Drawing::BitmapFormat format { Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_PREMUL};
    bitmap.Build(pixelmap->GetWidth(), pixelmap->GetHeight(), format);
    auto surface = std::make_shared<Drawing::Surface>();
    surface->Bind(bitmap);
    auto canvas = surface->GetCanvas();
    canvas->Clear(Drawing::Color::COLOR_YELLOW);
    Drawing::Brush brush;
    brush.SetColor(Drawing::Color::COLOR_RED);
    int w = 50;
    int h = 50;
    int half = 25;
    canvas->AttachBrush(brush);
    canvas->DrawRect(Drawing::Rect(w, h, half, half));
    canvas->DetachBrush();
    return pixelmap;
}

/**
 * @tc.name: ExtractDrawingImage
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(RSPixelMapUtilTest, ExtractDrawingImage, TestSize.Level1)
{
    std::shared_ptr<Media::PixelMap> pixelmap;
    int width = 200;
    int height = 300;
    pixelmap = CreatePixelMap(width, height);

    EXPECT_NE(nullptr, RSPixelMapUtil::ExtractDrawingImage(pixelmap));
}

/**
 * @tc.name: DrawPixelMap
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(RSPixelMapUtilTest, DrawPixelMap, TestSize.Level1)
{
    std::shared_ptr<Media::PixelMap> pixelmap;
    int width = 200;
    int height = 300;
    pixelmap = CreatePixelMap(width, height);

    auto canvas = std::make_unique<Drawing::Canvas>();
    EXPECT_NE(nullptr, canvas);

    RSPixelMapUtil::DrawPixelMap(*canvas, *pixelmap, 0.0f, 0.0f);
}
} // namespace Rosen
} // namespace OHOS