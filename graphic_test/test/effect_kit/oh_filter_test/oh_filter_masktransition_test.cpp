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

#include "oh_filter_test_utils.h"
#include "oh_filter_test_params.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
namespace {
const std::string BG_TEST_JPG_PATH = "/data/local/tmp/bg_test.jpg";
const std::string FG_TEST_JPG_PATH = "/data/local/tmp/fg_test.jpg";
}

class OHFilterMaskTransitionTest : public RSGraphicTest {
public:
    void BeforeEach() override
    {
        SetScreenSize(screenWidth, screenHeight);
    }

void DrawBackgroundNodeOHPixelMap(OH_PixelmapNative* pixelMapNative, const Rosen::Vector4f bounds)
{
    auto pixelmap = pixelMapNative->GetInnerPixelmap();
    auto image = std::make_shared<Rosen::RSImage>();
    image->SetPixelMap(pixelmap);
    image->SetImageFit((int)ImageFit::FILL);
    auto node = Rosen::RSCanvasNode::Create();
    node->SetBounds(bounds);
    node->SetFrame(bounds);
    node->SetBgImageSize(bounds[WIDTH_INDEX], bounds[HEIGHT_INDEX]);
    node->SetBgImage(image);
    GetRootNode()->AddChild(node);
    RegisterNode(node);
}

private:
    const int screenWidth = 1200;
    const int screenHeight = 2000;
};

GRAPHIC_TEST(OHFilterMaskTransitionTest, EFFECT_TEST, LinearGradientMaskTest)
{
    int columnCount = 2;
    int rowCount = static_cast<int>(linearGradientMaskParams.size());
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight * columnCount / rowCount;

    for (int i = 0; i < rowCount; i++) {
        auto bgPixelMapNative = CreateTestPixelMap(BG_TEST_JPG_PATH);
        auto fgPixelMapNative = CreateTestPixelMap(FG_TEST_JPG_PATH);
        auto ohFilter = CreateFilter(bgPixelMapNative);

        auto fractionStops = linearGradientMaskParams[i].fractionStops;

        OH_Filter_LinearGradientMask mask = {};
        mask.startPosition = linearGradientMaskParams[i].startPosition;
        mask.endPosition = linearGradientMaskParams[i].endPosition;
        mask.fractionStops = fractionStops.data();
        mask.fractionStopsLength = static_cast<uint32_t>(fractionStops.size());

        OH_Filter_MaskTransition(ohFilter, fgPixelMapNative, &mask,
            EffectMaskType::LINEAR_GRADIENT_MASK,
            linearGradientMaskParams[i].factor,
            linearGradientMaskParams[i].inverse);
        OH_Filter_GetEffectPixelMap(ohFilter, &bgPixelMapNative);

        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        DrawBackgroundNodeOHPixelMap(bgPixelMapNative, {x, y, sizeX, sizeY});
        OH_Filter_Release(ohFilter);
    }
}

GRAPHIC_TEST(OHFilterMaskTransitionTest, EFFECT_TEST, RadialGradientMaskTest)
{
    int columnCount = 2;
    int rowCount = static_cast<int>(radialGradientMaskParams.size());
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight * columnCount / rowCount;

    for (int i = 0; i < rowCount; i++) {
        auto bgPixelMapNative = CreateTestPixelMap(BG_TEST_JPG_PATH);
        auto fgPixelMapNative = CreateTestPixelMap(FG_TEST_JPG_PATH);
        auto ohFilter = CreateFilter(bgPixelMapNative);

        auto fractionStops = radialGradientMaskParams[i].fractionStops;

        OH_Filter_RadialGradientMask mask = {};
        mask.center = radialGradientMaskParams[i].center;
        mask.radiusX = radialGradientMaskParams[i].radiusX;
        mask.radiusY = radialGradientMaskParams[i].radiusY;
        mask.fractionStops = fractionStops.data();
        mask.fractionStopsLength = static_cast<uint32_t>(fractionStops.size());

        OH_Filter_MaskTransition(ohFilter, fgPixelMapNative, &mask,
            EffectMaskType::RADIAL_GRADIENT_MASK,
            radialGradientMaskParams[i].factor,
            radialGradientMaskParams[i].inverse);
        OH_Filter_GetEffectPixelMap(ohFilter, &bgPixelMapNative);

        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        DrawBackgroundNodeOHPixelMap(bgPixelMapNative, {x, y, sizeX, sizeY});
        OH_Filter_Release(ohFilter);
    }
}

}  // namespace OHOS::Rosen
