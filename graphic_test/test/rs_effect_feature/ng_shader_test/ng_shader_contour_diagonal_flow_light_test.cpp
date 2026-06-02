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

#include "rs_graphic_test.h"
#include "rs_graphic_test_director.h"
#include "rs_graphic_test_img.h"
#include "ui_effect/property/include/rs_ui_shader_base.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {

namespace {
const std::string TEST_IMAGE_PATH = "/data/local/tmp/Images/backGroundImage.jpg";
const int SCREEN_WIDTH = 1200;
const int SCREEN_HEIGHT = 2000;

// Contour values
const std::vector<std::vector<Vector2f>> contourValues = {
    {
        Vector2f{0.436198, 0.122504}, Vector2f{0.418602, 0.126364}, Vector2f{0.401042, 0.128906},
        Vector2f{0.367740, 0.129354}, Vector2f{0.333333, 0.126953}, Vector2f{0.320173, 0.127114},
        Vector2f{0.308594, 0.132270}, Vector2f{0.282406, 0.140389}, Vector2f{0.260417, 0.154297},
        Vector2f{0.241507, 0.165446}, Vector2f{0.228443, 0.180664}, Vector2f{0.206792, 0.205941},
        Vector2f{0.195168, 0.234375}, Vector2f{0.179982, 0.277536}, Vector2f{0.178386, 0.322266},
        Vector2f{0.176220, 0.354231}, Vector2f{0.181569, 0.385742}, Vector2f{0.193819, 0.408522},
        Vector2f{0.210648, 0.429688}, Vector2f{0.224975, 0.450066}, Vector2f{0.248264, 0.466037},
        Vector2f{0.264219, 0.481627}, Vector2f{0.273438, 0.500000}, Vector2f{0.286533, 0.536936},
        Vector2f{0.301794, 0.573242}, Vector2f{0.305483, 0.578097}, Vector2f{0.301939, 0.583008},
        Vector2f{0.296319, 0.590440}, Vector2f{0.290220, 0.597656}, Vector2f{0.285010, 0.604772},
        Vector2f{0.276042, 0.609375}, Vector2f{0.259386, 0.617175}, Vector2f{0.241175, 0.622830},
        Vector2f{0.206852, 0.632376}, Vector2f{0.170573, 0.632812}, Vector2f{0.158118, 0.632785},
        Vector2f{0.147136, 0.637478}, Vector2f{0.122095, 0.648149}, Vector2f{0.099393, 0.661458},
        Vector2f{0.088528, 0.669341}, Vector2f{0.080151, 0.678711}, Vector2f{0.074248, 0.688205},
        Vector2f{0.070457, 0.698242}, Vector2f{0.062967, 0.734686}, Vector2f{0.066406, 0.771484},
        Vector2f{0.071382, 0.808030}, Vector2f{0.073496, 0.844727}, Vector2f{0.076234, 0.852934},
        Vector2f{0.068866, 0.859375}, Vector2f{0.063638, 0.869408},
        Vector2f{0.055122, 0.877713}, Vector2f{0.032138, 0.892197}, Vector2f{0.019097, 0.913086},
        Vector2f{0.011010, 0.921940}, Vector2f{0.008536, 0.932617}, Vector2f{0.007380, 0.937471},
        Vector2f{0.007957, 0.942383}, Vector2f{0.014872, 0.954917}, Vector2f{0.023438, 0.966797},
        Vector2f{0.022116, 0.976836}, Vector2f{0.025897, 0.986328}, Vector2f{0.029415, 0.991230},
        Vector2f{0.033276, 0.995985}, Vector2f{0.037547, 0.999953}, Vector2f{0.044271, 0.999023},
        Vector2f{0.438151, 0.999023}, Vector2f{0.832031, 0.999023}, Vector2f{0.838542, 0.999023},
        Vector2f{0.845052, 0.999023}, Vector2f{0.848307, 0.999023}, Vector2f{0.851563, 0.999023},
        Vector2f{0.854022, 0.998481}, Vector2f{0.856482, 0.997938}, Vector2f{0.856337, 0.996040},
        Vector2f{0.856192, 0.994141}, Vector2f{0.852706, 0.989490}, Vector2f{0.850695, 0.984375},
        Vector2f{0.832893, 0.939715}, Vector2f{0.809317, 0.896484}, Vector2f{0.799994, 0.877343},
        Vector2f{0.780093, 0.864692}, Vector2f{0.773769, 0.861201}, Vector2f{0.772280, 0.855469},
        Vector2f{0.771753, 0.850365}, Vector2f{0.774595, 0.845703}, Vector2f{0.798601, 0.766116},
        Vector2f{0.811057, 0.684570}, Vector2f{0.811274, 0.679689}, Vector2f{0.811202, 0.674805},
        Vector2f{0.811202, 0.672363}, Vector2f{0.811202, 0.669922}, Vector2f{0.811780, 0.667480},
        Vector2f{0.812359, 0.665039}, Vector2f{0.813733, 0.664225}, Vector2f{0.815108, 0.663411},
        Vector2f{0.815759, 0.665202}, Vector2f{0.816410, 0.666992}, Vector2f{0.816050, 0.671905},
        Vector2f{0.817133, 0.676758}, Vector2f{0.818146, 0.678114},
        Vector2f{0.819158, 0.679471}, Vector2f{0.820244, 0.678114}, Vector2f{0.821329, 0.676758},
        Vector2f{0.822341, 0.674316}, Vector2f{0.823354, 0.671875}, Vector2f{0.824652, 0.667035},
        Vector2f{0.824222, 0.662109}, Vector2f{0.824565, 0.649629}, Vector2f{0.819593, 0.637695},
        Vector2f{0.816373, 0.627546}, Vector2f{0.810189, 0.618164}, Vector2f{0.806704, 0.613836},
        Vector2f{0.803534, 0.609375}, Vector2f{0.801797, 0.607042}, Vector2f{0.800061, 0.604709},
        Vector2f{0.799121, 0.604438}, Vector2f{0.798181, 0.604167}, Vector2f{0.799338, 0.606283},
        Vector2f{0.800496, 0.608398}, Vector2f{0.803682, 0.613163}, Vector2f{0.806138, 0.618164},
        Vector2f{0.807512, 0.620605}, Vector2f{0.808887, 0.623047}, Vector2f{0.810117, 0.625488},
        Vector2f{0.811346, 0.627930}, Vector2f{0.810912, 0.629666}, Vector2f{0.810478, 0.631402},
        Vector2f{0.808163, 0.630588}, Vector2f{0.805849, 0.629774}, Vector2f{0.803606, 0.627387},
        Vector2f{0.801364, 0.625000}, Vector2f{0.797670, 0.620021}, Vector2f{0.793551, 0.615234},
        Vector2f{0.789764, 0.610527}, Vector2f{0.786896, 0.605469}, Vector2f{0.783653, 0.600816},
        Vector2f{0.781543, 0.595812}, Vector2f{0.786908, 0.593851}, Vector2f{0.788343, 0.591797},
        Vector2f{0.784427, 0.586662}, Vector2f{0.779518, 0.582031}, Vector2f{0.769707, 0.573498},
        Vector2f{0.758250, 0.566298}, Vector2f{0.733814, 0.551521}, Vector2f{0.705733, 0.541233},
        Vector2f{0.678167, 0.534987}, Vector2f{0.653650, 0.523872}, Vector2f{0.647171, 0.520723},
        Vector2f{0.642221, 0.516276}, Vector2f{0.636389, 0.512502},
        Vector2f{0.637157, 0.506836}, Vector2f{0.647536, 0.496735}, Vector2f{0.647139, 0.484375},
        Vector2f{0.653280, 0.465012}, Vector2f{0.651046, 0.445312}, Vector2f{0.651211, 0.440331},
        Vector2f{0.653071, 0.435547}, Vector2f{0.657028, 0.431839}, Vector2f{0.663777, 0.435113},
        Vector2f{0.664935, 0.432888}, Vector2f{0.666092, 0.430664}, Vector2f{0.667197, 0.423164},
        Vector2f{0.663488, 0.416016}, Vector2f{0.662186, 0.411152}, Vector2f{0.663198, 0.406250},
        Vector2f{0.667417, 0.400922}, Vector2f{0.674483, 0.406033}, Vector2f{0.676190, 0.399300},
        Vector2f{0.678534, 0.392687}, Vector2f{0.687172, 0.389038}, Vector2f{0.690108, 0.398329},
        Vector2f{0.691483, 0.399143}, Vector2f{0.692857, 0.399957}, Vector2f{0.693436, 0.398383},
        Vector2f{0.694014, 0.396810}, Vector2f{0.693146, 0.394911}, Vector2f{0.692278, 0.393012},
        Vector2f{0.690613, 0.388486}, Vector2f{0.690832, 0.383789}, Vector2f{0.675793, 0.366942},
        Vector2f{0.661462, 0.349609}, Vector2f{0.637506, 0.309303}, Vector2f{0.621243, 0.266602},
        Vector2f{0.595395, 0.221147}, Vector2f{0.562359, 0.178711}, Vector2f{0.543795, 0.158132},
        Vector2f{0.517221, 0.143121}, Vector2f{0.504438, 0.133908}, Vector2f{0.487562, 0.129774},
        Vector2f{0.469685, 0.124847}, Vector2f{0.450525, 0.123047}, Vector2f{0.443446, 0.121296}
    },
    {
        Vector2f{0.09999999999999998f, 0.9f}, Vector2f{0.21428571428571427f, 0.9f},
        Vector2f{0.32857142857142857f, 0.9f}, Vector2f{0.44285714285714284f, 0.9f},
        Vector2f{0.5571428571428572f, 0.9f}, Vector2f{0.6714285714285715f, 0.9f},
        Vector2f{0.7857142857142857f, 0.9f}, Vector2f{0.9f, 0.9f},
        Vector2f{0.9f, 0.7857142857142857f}, Vector2f{0.9f, 0.6714285714285715f},
        Vector2f{0.9f, 0.5571428571428572f}, Vector2f{0.9f, 0.44285714285714284f},
        Vector2f{0.9f, 0.3285714285714285f}, Vector2f{0.9f, 0.2142857142857143f},
        Vector2f{0.9f, 0.09999999999999998f}, Vector2f{0.7857142857142857f, 0.09999999999999998f},
        Vector2f{0.6714285714285715f, 0.09999999999999998f}, Vector2f{0.5571428571428572f, 0.09999999999999998f},
        Vector2f{0.44285714285714284f, 0.09999999999999998f}, Vector2f{0.3285714285714285f, 0.09999999999999998f},
        Vector2f{0.2142857142857143f, 0.09999999999999998f}, Vector2f{0.09999999999999998f, 0.09999999999999998f},
        Vector2f{0.09999999999999998f, 0.21428571428571427f}, Vector2f{0.09999999999999998f, 0.32857142857142857f},
        Vector2f{0.09999999999999998f, 0.44285714285714284f}, Vector2f{0.09999999999999998f, 0.5571428571428572f},
        Vector2f{0.09999999999999998f, 0.6714285714285715f}, Vector2f{0.09999999999999998f, 0.7857142857142857f},
        Vector2f{0.9f, 1.0f}, Vector2f{1.1f, 0.09999999999999998f}
    }
};

// Thickness values
const std::vector<float> thicknessValues = {0.1f, 0.2f, 0.3f, 0.4f};

// Halo radius values
const std::vector<float> haloRadiusValues = {0.0f, 10.0f, 25.0f, 50.0f};

// Light weight values
const std::vector<float> lightWeightValues = {0.0f, 0.5f, 0.8f, 1.0f};


void InitContourDiagonalFlowLight(std::shared_ptr<RSNGContourDiagonalFlowLight>& contourDiagonalFlowLight)
{
    if (!contourDiagonalFlowLight) {
        return;
    }
    
    std::vector<Vector2f> contour = contourValues[0];
    contourDiagonalFlowLight->Setter<ContourDiagonalFlowLightContourTag>(contour);
    // Line1Start
    contourDiagonalFlowLight->Setter<ContourDiagonalFlowLightLine1StartTag>(0.0f);
    // Line1Length
    contourDiagonalFlowLight->Setter<ContourDiagonalFlowLightLine1LengthTag>(0.35f);
    // Line1Color
    contourDiagonalFlowLight->Setter<ContourDiagonalFlowLightLine1ColorTag>(Vector4f{1.0f, 1.0f, 1.0f, 1.0f});
    // Line2Start
    contourDiagonalFlowLight->Setter<ContourDiagonalFlowLightLine2StartTag>(0.5f);
    // Line2Length
    contourDiagonalFlowLight->Setter<ContourDiagonalFlowLightLine2LengthTag>(0.35f);
    // Line2Color
    contourDiagonalFlowLight->Setter<ContourDiagonalFlowLightLine2ColorTag>(Vector4f{0.8f, 0.8f, 1.0f, 1.0f});
    // Thickness
    contourDiagonalFlowLight->Setter<ContourDiagonalFlowLightThicknessTag>(0.3f);
    // HaloRadius
    contourDiagonalFlowLight->Setter<ContourDiagonalFlowLightHaloRadiusTag>(25.0f);
    // LightWeight
    contourDiagonalFlowLight->Setter<ContourDiagonalFlowLightLightWeightTag>(1.0f);
    // HaloWeight
    contourDiagonalFlowLight->Setter<ContourDiagonalFlowLightHaloWeightTag>(2.0f);
}
}

class NGShaderContourDiagonalFlowLightTest : public RSGraphicTest {
public:
    void BeforeEach() override
    {
        SetScreenSize(SCREEN_WIDTH, SCREEN_HEIGHT);
    }

private:
    void SetUpTestNode(const size_t i, const size_t columnCount, const size_t rowCount,
        std::shared_ptr<RSNGContourDiagonalFlowLight>& contourLight)
    {
        if (columnCount == 0 || rowCount == 0) {
            return;  // Invalid test configuration
        }
        const size_t sizeX = SCREEN_WIDTH / columnCount;
        const size_t sizeY = SCREEN_HEIGHT / rowCount;
        const size_t x = (i % columnCount) * sizeX;
        const size_t y = (i / columnCount) * sizeY;

        auto testNode = SetUpNodeBgImage(TEST_IMAGE_PATH, {x, y, sizeX, sizeY});
        testNode->SetBackgroundNGShader(contourLight);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
};


GRAPHIC_TEST(NGShaderContourDiagonalFlowLightTest, EFFECT_TEST,
    Set_Contour_Diagonal_Flow_Light_Contour_thicknessValues_Test)
{
    const int columnCount = 1;
    const int rowCount = thicknessValues.size();
    auto sizeX = SCREEN_WIDTH / columnCount;
    auto sizeY = SCREEN_HEIGHT * columnCount / rowCount;

    for (size_t i = 0; i < thicknessValues.size(); ++i) {
        auto contourLight = std::make_shared<RSNGContourDiagonalFlowLight>();
        InitContourDiagonalFlowLight(contourLight);
        contourLight->Setter<ContourDiagonalFlowLightThicknessTag>(thicknessValues[i]);
        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;

        auto node = RSCanvasNode::Create(false, false, RSGraphicTestDirector::Instance().GetRSUIContext());
        if (!node) {
            return;
        }
        node->SetBounds({x, y, sizeX, sizeY});
        node->SetFrame({x, y, sizeX, sizeY});
        node->SetBackgroundColor(0xff000000);
        node->SetBackgroundNGShader(contourLight);
        GetRootNode()->AddChild(node);
        RegisterNode(node);
    }
}

GRAPHIC_TEST(NGShaderContourDiagonalFlowLightTest, EFFECT_TEST,
    Set_Contour_Diagonal_Flow_Light_ContourValues_Test)
{
    const size_t columnCount = 1;
    const size_t rowCount = contourValues.size();
    auto sizeX = SCREEN_WIDTH / columnCount;
    auto sizeY = SCREEN_HEIGHT * columnCount / rowCount;

    for (size_t i = 0; i < contourValues.size(); i++) {
        auto contourLight = std::make_shared<RSNGContourDiagonalFlowLight>();
        InitContourDiagonalFlowLight(contourLight);
        contourLight->Setter<ContourDiagonalFlowLightContourTag>(contourValues[i]);
        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;

        auto node = RSCanvasNode::Create(false, false, RSGraphicTestDirector::Instance().GetRSUIContext());
        if (!node) {
            return;
        }
        node->SetBounds({x, y, sizeX, sizeY});
        node->SetFrame({x, y, sizeX, sizeY});
        node->SetBackgroundColor(0xff000000);
        node->SetBackgroundNGShader(contourLight);
        GetRootNode()->AddChild(node);
        RegisterNode(node);
    }
}

GRAPHIC_TEST(NGShaderContourDiagonalFlowLightTest, EFFECT_TEST,
    Set_Contour_Diagonal_Flow_Light_Contour_haloRadiusValues_Test)
{
    const size_t columnCount = 2;
    const size_t rowCount = haloRadiusValues.size();
    auto sizeX = SCREEN_WIDTH / columnCount;
    auto sizeY = SCREEN_HEIGHT * columnCount / rowCount;

    for (size_t i = 0; i < haloRadiusValues.size(); ++i) {
        auto contourLight = std::make_shared<RSNGContourDiagonalFlowLight>();
        InitContourDiagonalFlowLight(contourLight);
        contourLight->Setter<ContourDiagonalFlowLightHaloRadiusTag>(haloRadiusValues[i]);
        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        
        auto node = RSCanvasNode::Create(false, false, RSGraphicTestDirector::Instance().GetRSUIContext());
        if (!node) {
            return;
        }
        node->SetBounds({x, y, sizeX, sizeY});
        node->SetFrame({x, y, sizeX, sizeY});
        node->SetBackgroundColor(0xff000000);
        node->SetBackgroundNGShader(contourLight);
        GetRootNode()->AddChild(node);
        RegisterNode(node);
    }
}

GRAPHIC_TEST(NGShaderContourDiagonalFlowLightTest, EFFECT_TEST,
    Set_Contour_Diagonal_Flow_Contour_LightWeight_Test)
{
    const size_t columnCount = 2;
    const size_t rowCount = lightWeightValues.size();
    auto sizeX = SCREEN_WIDTH / columnCount;
    auto sizeY = SCREEN_HEIGHT * columnCount / rowCount;

    for (size_t i = 0; i < lightWeightValues.size(); ++i) {
        auto contourLight = std::make_shared<RSNGContourDiagonalFlowLight>();
        contourLight->Setter<ContourDiagonalFlowLightLightWeightTag>(lightWeightValues[i]);
        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        
        auto node = RSCanvasNode::Create(false, false, RSGraphicTestDirector::Instance().GetRSUIContext());
        if (!node) {
            return;
        }
        node->SetBounds({x, y, sizeX, sizeY});
        node->SetFrame({x, y, sizeX, sizeY});
        node->SetBackgroundColor(0xff000000);
        node->SetBackgroundNGShader(contourLight);
        GetRootNode()->AddChild(node);
        RegisterNode(node);
    }
}

} // namespace OHOS::Rosen
