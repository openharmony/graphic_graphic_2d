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

#include "rs_graphic_test.h"
#include "pixel_map.h"
#include "image_source.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class MESABlurShaderFilterTest : public RSGraphicTest {
public:
    // called before each tests
    void BeforeEach() override {}

    // called after each tests
    void AfterEach() override {}

    std::shared_ptr<Media::PixelMap> DecodePixelMap(const std::string &pathName,
        const Media::AllocatorType &allocatorType)
    {
        uint32_t errCode = 0;
        std::unique_ptr<Media::ImageSource> imageSource =
            Media::ImageSource::CreateImageSource(pathName, Media::SourceOptions(), errCode);
        if (imageSource == nullptr || errCode != 0) {
            std::cout << "imageSource : " << (imageSource != nullptr) << ", err:" << errCode << std::endl;
            return nullptr;
        }
        Media::DecodeOptions decodeOpt;
        decodeOpt.allocatorType = allocatorType;
        std::shared_ptr<Media::PixelMap> pixelmap = imageSource->CreatePixelMap(decodeOpt, errCode);
        if (pixelmap == nullptr || errCode != 0) {
            std::cout << "pixelmap == nullptr, err:" << errCode << std::endl;
            return nullptr;
        }
        return pixelmap;
    }

    std::shared_ptr<Rosen::RSCanvasNode> SetUpNodeBgImage(const std::string &pathName, const Rosen::Vector4f bounds)
    {
        std::shared_ptr<Media::PixelMap> pixelmap = DecodePixelMap(pathName, Media::AllocatorType::SHARE_MEM_ALLOC);
        auto image = std::make_shared<Rosen::RSImage>();
        image->SetPixelMap(pixelmap);
        image->SetImageFit((int)ImageFit::FILL);
        auto node = Rosen::RSCanvasNode::Create();
        node->SetBounds(bounds);
        node->SetBgImageWidth(bounds[2]);  // 2: index of width
        node->SetBgImageHeight(bounds[3]);  // 3: index of height
        node->SetBgImage(image);
        return node;
    }
};

/*
 * @tc.name: BACKGROUND_BLUR_TEST_1
 * @tc.desc: BACKGROUND_BLUR_TEST with various radius params
 * @tc.type: FUNC
 */
GRAPHIC_TEST(MESABlurShaderFilterTest, CONTENT_DISPLAY_TEST, BACKGROUND_BLUR_TEST_1)
{
    auto screenSize = GetScreenSize();
    int columnCount = 2;  // 2: column size
    int rowCount = 3;  // 3: row size
    auto sizeX = screenSize[0] / columnCount;
    auto sizeY = screenSize[1] / rowCount;

    // -1, 6, 20, 50, 80, 300: radius params
    std::vector<float> radiusList = { -1, 6, 20, 50, 80, 300 };

    for (int i = 0; i < radiusList.size(); i++) {
        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        // 10: border
        auto testNode = SetUpNodeBgImage("/data/local/rs_graphic_tests/backGroundImage.jpg",
            { x, y, sizeX - 10, sizeY - 10 });
        testNode->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
        testNode->SetBackgroundBlurRadius(radiusList[i]);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/*
 * @tc.name: BACKGROUND_BLUR_TEST_2
 * @tc.desc: BACKGROUND_BLUR_TEST with grey adjustment
 * @tc.type: FUNC
 */
GRAPHIC_TEST(MESABlurShaderFilterTest, CONTENT_DISPLAY_TEST, BACKGROUND_BLUR_TEST_2)
{
    auto screenSize = GetScreenSize();
    int columnCount = 2;  // 2: column size
    int rowCount = 3;  // 3: row size
    auto sizeX = screenSize[0] / columnCount;
    auto sizeY = screenSize[1] / rowCount;

    // 2, 6, 20, 50, 80, 300: radius params
    std::vector<float> radiusList = { 2, 6, 20, 50, 80, 300 };
    // 20.f: grey params
    Vector2f greyCoef(20.f, 20.f);

    for (int i = 0; i < radiusList.size(); i++) {
        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        // 10: border
        auto testNode = SetUpNodeBgImage("/data/local/rs_graphic_tests/backGroundImage.jpg",
            { x, y, sizeX - 10, sizeY - 10 });
        testNode->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
        testNode->SetBackgroundBlurRadius(radiusList[i]);
        testNode->SetGreyCoef(greyCoef);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/*
 * @tc.name: BACKGROUND_BLUR_TEST_3
 * @tc.desc: BACKGROUND_BLUR_TEST with pixel stretch (CLAMP)
 * @tc.type: FUNC
 */
GRAPHIC_TEST(MESABlurShaderFilterTest, CONTENT_DISPLAY_TEST, BACKGROUND_BLUR_TEST_3)
{
    auto screenSize = GetScreenSize();
    int columnCount = 2;  // 2: column size
    int rowCount = 3;  // 3: row size
    auto sizeX = screenSize[0] / columnCount;
    auto sizeY = screenSize[1] / rowCount;

    // 2, 6, 20, 50, 80, 300: radius params
    std::vector<float> radiusList = { 2, 6, 20, 50, 80, 300 };
    // 20.f: grey params
    Vector2f greyCoef(20.f, 20.f);
    // 50.f: pixel stretch params
    Vector4f pixelStretchCoef(50.f, 50.f, 50.f, 50.f);

    for (int i = 0; i < radiusList.size(); i++) {
        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        // 10: border
        auto testNode = SetUpNodeBgImage("/data/local/rs_graphic_tests/backGroundImage.jpg",
            { x, y, sizeX - 10, sizeY - 10 });
        testNode->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
        testNode->SetBackgroundBlurRadius(radiusList[i]);
        testNode->SetGreyCoef(greyCoef);
        testNode->SetPixelStretch(pixelStretchCoef, Drawing::TileMode::CLAMP);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/*
 * @tc.name: BACKGROUND_BLUR_TEST_4
 * @tc.desc: BACKGROUND_BLUR_TEST with pixel stretch (MIRROR)
 * @tc.type: FUNC
 */
GRAPHIC_TEST(MESABlurShaderFilterTest, CONTENT_DISPLAY_TEST, BACKGROUND_BLUR_TEST_4)
{
    auto screenSize = GetScreenSize();
    int columnCount = 2;  // 2: column size
    int rowCount = 3;  // 3: row size
    auto sizeX = screenSize[0] / columnCount;
    auto sizeY = screenSize[1] / rowCount;

    // 2, 6, 20, 50, 80, 300: radius params
    std::vector<float> radiusList = { 2, 6, 20, 50, 80, 300 };
    // 20.f: grey params
    Vector2f greyCoef(20.f, 20.f);
    // 50.f: pixel stretch params
    Vector4f pixelStretchCoef(50.f, 50.f, 50.f, 50.f);

    for (int i = 0; i < radiusList.size(); i++) {
        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        // 10: border
        auto testNode = SetUpNodeBgImage("/data/local/rs_graphic_tests/backGroundImage.jpg",
            { x, y, sizeX - 10, sizeY - 10 });
        testNode->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
        testNode->SetBackgroundBlurRadius(radiusList[i]);
        testNode->SetGreyCoef(greyCoef);
        testNode->SetPixelStretch(pixelStretchCoef, Drawing::TileMode::MIRROR);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/*
 * @tc.name: BACKGROUND_BLUR_TEST_5
 * @tc.desc: BACKGROUND_BLUR_TEST with grey adjustment
 * @tc.type: FUNC
 */
GRAPHIC_TEST(MESABlurShaderFilterTest, CONTENT_DISPLAY_TEST, BACKGROUND_BLUR_TEST_5)
{
    auto screenSize = GetScreenSize();
    int columnCount = 2;  // 2: column size
    int rowCount = 3;  // 3: row size
    auto sizeX = screenSize[0] / columnCount;
    auto sizeY = screenSize[1] / rowCount;

    // 2, 6, 20, 50, 80, 300: radius params
    std::vector<float> radiusList = { 2, 6, 20, 50, 80, 300 };
    // 20.f: grey params
    Vector2f greyCoef(20.f, 20.f);

    for (int i = 0; i < radiusList.size(); i++) {
        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        // 10: border
        auto testNode = SetUpNodeBgImage("/data/local/rs_graphic_tests/backGroundImage.jpg",
            { x, y, sizeX - 10, sizeY - 10 });
        testNode->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
        testNode->SetBackgroundBlurRadiusX(radiusList[i]);
        testNode->SetBackgroundBlurRadiusY(radiusList[i]);
        testNode->SetGreyCoef(greyCoef);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/*
 * @tc.name: FOREROUND_BLUR_TEST_1
 * @tc.desc: FOREGROUND_BLUR_TEST with grey adjustment
 * @tc.type: FUNC
 */
GRAPHIC_TEST(MESABlurShaderFilterTest, CONTENT_DISPLAY_TEST, FOREGROUND_BLUR_TEST_1)
{
    auto screenSize = GetScreenSize();
    int columnCount = 2;  // 2: column size
    int rowCount = 3;  // 3: row size
    auto sizeX = screenSize[0] / columnCount;
    auto sizeY = screenSize[1] / rowCount;

    // 0, 6, 20, 50, 80, 300: radius params
    std::vector<float> radiusList = { 0, 6, 20, 50, 80, 300 };
    // 20.f: grey params
    Vector2f greyCoef(20.f, 20.f);

    for (int i = 0; i < radiusList.size(); i++) {
        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        // 10: border
        auto testNode = SetUpNodeBgImage("/data/local/rs_graphic_tests/backGroundImage.jpg",
            { x, y, sizeX - 10, sizeY - 10 });
        testNode->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
        testNode->SetForegroundBlurRadius(radiusList[i]);
        testNode->SetGreyCoef(greyCoef);
        testNode->SetForegroundBlurMaskColor(RSColor::FromArgbInt(0x7dff0000));
        testNode->SetForegroundBlurColorMode(i);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/*
 * @tc.name: FOREROUND_BLUR_TEST_2
 * @tc.desc: FOREGROUND_BLUR_TEST with grey adjustment
 * @tc.type: FUNC
 */
GRAPHIC_TEST(MESABlurShaderFilterTest, CONTENT_DISPLAY_TEST, FOREGROUND_BLUR_TEST_2)
{
    auto screenSize = GetScreenSize();
    int columnCount = 2;  // 2: column size
    int rowCount = 3;  // 3: row size
    auto sizeX = screenSize[0] / columnCount;
    auto sizeY = screenSize[1] / rowCount;

    // 0, 6, 20, 50, 80, 300: radius params
    std::vector<float> radiusList = { 0, 6, 20, 50, 80, 300 };
    // 20.f: grey params
    Vector2f greyCoef(20.f, 20.f);

    for (int i = 0; i < radiusList.size(); i++) {
        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        // 10: border
        auto testNode = SetUpNodeBgImage("/data/local/rs_graphic_tests/backGroundImage.jpg",
            { x, y, sizeX - 10, sizeY - 10 });
        testNode->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
        testNode->SetForegroundBlurRadiusX(radiusList[i]);
        testNode->SetForegroundBlurRadiusY(radiusList[i]);
        testNode->SetGreyCoef(greyCoef);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}
}
