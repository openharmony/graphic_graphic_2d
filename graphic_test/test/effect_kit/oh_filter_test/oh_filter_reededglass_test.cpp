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

#include "oh_filter_test_params.h"
#include "oh_filter_test_utils.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
namespace {
const std::string FG_TEST_JPG_PATH = "/data/local/tmp/fg_test.jpg";
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
        auto pixelMapNative = CreateTestPixelMap(FG_TEST_JPG_PATH);
        auto ohFilter = CreateFilter(pixelMapNative);

        OH_Filter_ReededGlassDataParams params = {};
        params.refractionFactor = reededGlassParams[i][REFRACTION_FACTOR_INDEX];
        params.horizontalPatternNumber = static_cast<uint8_t>(reededGlassParams[i][HORIZONTAL_PATTERN_NUMBER_INDEX]);
        params.gridLightStrength = reededGlassParams[i][GRID_LIGHT_STRENGTH_INDEX];
        params.gridLightPositionStart = reededGlassParams[i][GRID_LIGHT_POSITION_START_INDEX];
        params.gridLightPositionEnd = reededGlassParams[i][GRID_LIGHT_POSITION_END_INDEX];
        params.gridShadowStrength = reededGlassParams[i][GRID_SHADOW_STRENGTH_INDEX];
        params.gridShadowPositionStart = reededGlassParams[i][GRID_SHADOW_POSITION_START_INDEX];
        params.gridShadowPositionEnd = reededGlassParams[i][GRID_SHADOW_POSITION_END_INDEX];
        params.portalLightSize = { reededGlassParams[i][PORTAL_LIGHT_SIZE_X_INDEX],
            reededGlassParams[i][PORTAL_LIGHT_SIZE_Y_INDEX] };
        params.portalLightTilt = { reededGlassParams[i][PORTAL_LIGHT_TILT_X_INDEX],
            reededGlassParams[i][PORTAL_LIGHT_TILT_Y_INDEX] };
        params.portalLightPosition = { reededGlassParams[i][PORTAL_LIGHT_POSITION_X_INDEX],
            reededGlassParams[i][PORTAL_LIGHT_POSITION_Y_INDEX] };
        params.portalLightDisperseAttenuation = reededGlassParams[i][PORTAL_LIGHT_DISPERSE_ATTENUATION_INDEX];
        params.portalLightDisperse = reededGlassParams[i][PORTAL_LIGHT_DISPERSE_INDEX];
        params.portalLightSmoothBorder = reededGlassParams[i][PORTAL_LIGHT_SMOOTH_BORDER_INDEX];
        params.portalLightShadowBorder = reededGlassParams[i][PORTAL_LIGHT_SHADOW_BORDER_INDEX];
        params.portalLightShadowPositionShift = reededGlassParams[i][PORTAL_LIGHT_SHADOW_POSITION_SHIFT_INDEX];
        params.portalLightStrength = reededGlassParams[i][PORTAL_LIGHT_STRENGTH_INDEX];

        OH_Filter_ReededGlass(ohFilter, &params);
        OH_Filter_GetEffectPixelMap(ohFilter, &pixelMapNative);

        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        DrawBackgroundNodeOHPixelMap(pixelMapNative, { x, y, sizeX, sizeY });
        OH_Filter_Release(ohFilter);
    }
}
} // namespace OHOS::Rosen