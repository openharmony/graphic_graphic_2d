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
}

class OHFilterReededGlassTest : public RSGraphicTest {
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

GRAPHIC_TEST(OHFilterReededGlassTest, EFFECT_TEST, ReededGlassTest)
{
    int columnCount = 2;
    int rowCount = static_cast<int>(OHFilterTestDataGroupParamsType::COUNT);
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight * columnCount / rowCount;

    for (int i = 0; i < rowCount; i++) {
        auto pixelMapNative = CreateTestPixelMap(BG_TEST_JPG_PATH);
        auto ohFilter = CreateFilter(pixelMapNative);

        OH_Filter_ReededGlassDataParams params = {};
        params.refractionFactor = reededGlassParams[i][0];
        params.dispersionStrength = reededGlassParams[i][1];
        params.roughness = reededGlassParams[i][2];
        params.noiseFrequency = reededGlassParams[i][3];
        params.horizontalPatternNumber = static_cast<uint8_t>(reededGlassParams[i][4]);
        params.saturationFactor = reededGlassParams[i][5];
        params.gridLightStrength = reededGlassParams[i][6];
        params.gridLightPositionStart = reededGlassParams[i][7];
        params.gridLightPositionEnd = reededGlassParams[i][8];
        params.gridShadowStrength = reededGlassParams[i][9];
        params.gridShadowPositionStart = reededGlassParams[i][10];
        params.gridShadowPositionEnd = reededGlassParams[i][11];
        params.pointLightColor = {
            reededGlassParams[i][12], reededGlassParams[i][13],
            reededGlassParams[i][14], reededGlassParams[i][15]
        };
        params.pointLight1Position = {reededGlassParams[i][16], reededGlassParams[i][17]};
        params.pointLight1Strength = reededGlassParams[i][18];
        params.pointLight2Position = {reededGlassParams[i][19], reededGlassParams[i][20]};
        params.pointLight2Strength = reededGlassParams[i][21];
        params.portalLightSize = {reededGlassParams[i][22], reededGlassParams[i][23]};
        params.portalLightTilt = {reededGlassParams[i][24], reededGlassParams[i][25]};
        params.portalLightPosition = {reededGlassParams[i][26], reededGlassParams[i][27]};
        params.portalLightDisperseAttenuation = reededGlassParams[i][28];
        params.portalLightDisperse = reededGlassParams[i][29];
        params.portalLightSmoothBorder = reededGlassParams[i][30];
        params.portalLightShadowBorder = reededGlassParams[i][31];
        params.portalLightShadowPositionShift = reededGlassParams[i][32];
        params.portalLightStrength = reededGlassParams[i][33];

        OH_Filter_ReededGlass(ohFilter, &params);
        OH_Filter_GetEffectPixelMap(ohFilter, &pixelMapNative);

        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        DrawBackgroundNodeOHPixelMap(pixelMapNative, {x, y, sizeX, sizeY});
        OH_Filter_Release(ohFilter);
    }
}
}  // namespace OHOS::Rosen