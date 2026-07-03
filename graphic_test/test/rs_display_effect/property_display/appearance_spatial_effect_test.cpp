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
#include "rs_graphic_test_img.h"
#include "ui/rs_depth_node.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {

struct DepthSpaceConfig {
    int screenWidth = 944;
    int screenHeight = 2048;
    std::string depthImagePath;
    std::string backgroundImagePath;
    DepthSpaceType spaceType = DepthSpaceType::INSTANCE;
    DepthCameraPara cameraPara = {{0, 0, 0}, {0, 0, 0, 1}, 90.0f, 0.1f, 100.0f};
    DepthLightPara lightPara = {{0, 0, 1}, {44, 55, 66}, 7.0f};
};

struct SpatialParaConfig {
    Vector3f topLeft = {0.23733f, 0.20563f, 1.74357f};
    Vector3f topRight = {0.59956f, 0.27635f, 1.89782f};
    Vector3f bottomLeft = {0.23733f, -0.79426f, 1.74357f};
    Vector3f bottomRight = {0.59956f, -0.72365f, 1.89782f};
    float depthRatio = 1.0f;
};

struct ImageTestConfig {
    std::string depthImagePath;
    std::string backgroundImagePath;
    std::vector<SpatialParaConfig> spatialParas;
};

class SpatialEffectTestUtil {
public:
    static std::shared_ptr<RSDepthNode> CreateDepthNode(
        std::shared_ptr<RSGraphicRootNode> parent,
        const DepthSpaceConfig& config)
    {
        auto depthNode = RSDepthNode::Create(false, false, nullptr);
        depthNode->SetBounds({0, 0, static_cast<float>(config.screenWidth), static_cast<float>(config.screenHeight)});
        depthNode->SetFrame({0, 0, static_cast<float>(config.screenWidth), static_cast<float>(config.screenHeight)});
        depthNode->SetDepthSpaceType(config.spaceType);
        depthNode->SetDepthCameraPara(config.cameraPara);
        depthNode->SetDepthLightPara(config.lightPara);

        std::shared_ptr<Media::PixelMap> depthmap =
            DecodePixelMap(config.depthImagePath, Media::AllocatorType::SHARE_MEM_ALLOC);
        if (depthmap != nullptr) {
            auto depthimage = std::make_shared<Rosen::RSImage>();
            depthimage->SetPixelMap(depthmap);
            depthimage->SetImageFit((int)ImageFit::COVER);
            depthNode->SetDepthImage(depthimage);
        }

        parent->AddChild(depthNode);
        return depthNode;
    }

    static std::shared_ptr<RSCanvasNode> CreateBackgroundNode(
        std::shared_ptr<RSDepthNode> depthNode,
        const DepthSpaceConfig& config)
    {
        auto backgroundNode = RSCanvasNode::Create();
        backgroundNode->SetBounds({0, 0, static_cast<float>(config.screenWidth),
            static_cast<float>(config.screenHeight)});
        backgroundNode->SetFrame({0, 0, static_cast<float>(config.screenWidth),
            static_cast<float>(config.screenHeight)});
        backgroundNode->SetIsDepthBackground(true);

        std::shared_ptr<Media::PixelMap> bgpixelmap =
            DecodePixelMap(config.backgroundImagePath, Media::AllocatorType::SHARE_MEM_ALLOC);
        if (bgpixelmap != nullptr) {
            auto bgimage = std::make_shared<Rosen::RSImage>();
            bgimage->SetPixelMap(bgpixelmap);
            bgimage->SetImageFit((int)ImageFit::COVER);
            backgroundNode->SetBgImage(bgimage);
            backgroundNode->SetBgImageSize(config.screenWidth, config.screenHeight);
        }

        depthNode->AddChild(backgroundNode);
        return backgroundNode;
    }

    static std::shared_ptr<RSCanvasNode> CreateCardNode(
        std::shared_ptr<RSDepthNode> parent,
        float x, float y, float width, float height,
        uint32_t bgColor,
        const SpatialParaConfig& spatialPara)
    {
        auto cardNode = RSCanvasNode::Create();
        cardNode->SetBounds({x, y, width, height});
        cardNode->SetFrame({x, y, width, height});
        cardNode->SetBackgroundColor(bgColor);

        auto spaParaPtr = std::make_shared<SpatialEffectVariantPara>();
        SpatialEffectPara::CornerPositions corners;
        corners[SpatialEffectPara::LEFT_TOP_INDEX] = spatialPara.topLeft;
        corners[SpatialEffectPara::RIGHT_TOP_INDEX] = spatialPara.topRight;
        corners[SpatialEffectPara::LEFT_BOTTOM_INDEX] = spatialPara.bottomLeft;
        corners[SpatialEffectPara::RIGHT_TOP_INDEX] = spatialPara.bottomRight;
        spaParaPtr->position = corners;
        spaParaPtr->occlusionWeight = spatialPara.depthRatio;
        cardNode->SetSpatialEffectPara(spaParaPtr);

        parent->AddChild(cardNode);
        return cardNode;
    }
};

class AppearanceTest : public RSGraphicTest {
private:
    const int screenWidth = 944;
    const int screenHeight = 2048;

public:
    void BeforeEach() override
    {
        SetScreenSize(screenWidth, screenHeight);
    }

    static const std::vector<ImageTestConfig>& GetImageConfigs()
    {
        return imageConfigs_;
    }

private:
    static inline std::vector<ImageTestConfig> imageConfigs_ = {
        {"/data/local/tmp/depth1.png", "/data/local/tmp/bg1.png",
            {
                {
                    {-0.8554170749565639f, 1.1158402879629954f, -1.9411387470168184f},
                    {-0.01631486081205835f, 1.459251440037936f, -2.295061567845789f},
                    {-0.855417074956564f, -0.08019535077717789f, -1.9411387470168184f},
                    {-0.016314860812058463f, 0.2632158012977626f, -2.295061567845789f}, 1.0f
                },
                {
                    {0.4032362462601943f, 1.630957016075406f, -2.4720229782602745f},
                    {1.2423384604046999f, 1.9743681681503467f, -2.825945799089245f},
                    {0.4032362462601942f, 0.4349213773352328f, -2.4720229782602745f},
                    {1.2423384604046999f, 0.7783325294101733f, -2.825945799089245f}, 1.0f
                }
            }
        },
        {"/data/local/tmp/depth1.png", "/data/local/tmp/bg1.png",
            {
                {
                    {-0.38547484433986123f, -0.2038568888335805f, -0.8747313775404545f},
                    {-0.041059382906627806f, -0.18955884902662175f, -1.0200015122067532f},
                    {-0.3854748443398613f, -0.70886599980776776f, -0.8747313775404545f},
                    {-0.041059382906627764f, -0.694567960000719f, -1.0200015122067532f}, 1.0f
                },
                {
                    {0.09174111698830771f, 1.3647696649250691f, -2.340638228026013f},
                    {1.2423384604046994f, 1.7811511955980304f, -2.825945799089244f},
                    {0.09174111698830771f, 0.24409104509014778f, -2.340638228026013f},
                    {1.2423384604046994f, 0.660472575763109f, -2.825945799089244f}, 1.0f
                }
            }
        },
        {"/data/local/tmp/depth2.png", "/data/local/tmp/bg2.png",
            {
                {
                    {-0.8718175968404871f, 1.1779534635606934f, -1.9783553159072593f},
                    {-0.2579631923194405f, 0.9495393046382081f, -1.790098089064597f},
                    {-0.8718175968404871f, 0.17813580704673412f, -1.9783553159072593f},
                    {-0.2579631923194405f, -0.05027835187575122f, -1.790098089064597f}, 1.0f
                },
                {
                    {0.04896400994108285f, 0.8353322251769654f, -1.6959694756432655f},
                    {0.6628184144621295f, 0.60691806625448f, -1.5077122488006032f},
                    {0.04896400994108285f, -0.1644854313369939f, -1.6959694756432655f},
                    {0.6628184144621295f, -0.39289959025947924f, -1.5077122488006032f}, 1.0f
                }
            }
        },
        {"/data/local/tmp/depth2.png", "/data/local/tmp/bg2.png",
            {
                {
                    {-0.5864783719090199f, 0.9253494064442069f, -1.3308547670243143f},
                    {-0.04691148626323928f, 0.7107161260491976f, -1.165380079802576f},
                    {-0.5864783719090199f, 0.22697840835901967f, -1.3308547670243143f},
                    {-0.04691148626323928f, 0.01234512796401034f, -1.165380079802576f}, 1.0f
                },
                {
                    {0.0638016702731449f, 0.5874267355617259f, -1.6278047766986163f},
                    {0.6381748935797165f, 0.3998204152547621f, -1.451655661540367f},
                    {0.06380167027314493f, -0.4121243025751794f, -1.6278047766986163f},
                    {0.6381748935797165f, -0.5997306228821432f, -1.451655661540367f}, 1.0f
                }
            }
        },
        {"/data/local/tmp/depth3.png", "/data/local/tmp/bg3.png",
            {
                {
                    {-0.6131832459815691f, 0.6795131644170754f, -1.3914542889581762f},
                    {-0.05846027399213072f, 0.7244443862686137f, -1.5387773730746388f},
                    {-0.6131832459815691f, -0.1813017770570505f, -1.3914542889581762f},
                    {-0.05846027399213072f, -0.13637055520551222f, -1.538777373074639f}, 1.0f
                },
                {
                    {0.21890121200258839f, 0.7469099971943828f, -1.6124389151328704f},
                    {0.7736241839920267f, 0.7918412190459211f, -1.759761999249333f},
                    {0.21890121200258839f, -0.11390494427974307f, -1.6124389151328704f},
                    {0.7736241839920267f, -0.06897372242820479f, -1.7597619992493332f}, 1.0f
                }
            }
        },
        {"/data/local/tmp/depth3.png", "/data/local/tmp/bg3.png",
            {
                {
                    {-0.3738826134923138f, 0.05392537694600679f, -0.8484259306171735f},
                    {-0.03774627242850597f, 0.040937000445495704f, -0.9376968729607802f},
                    {-0.3738826134923137f, -0.4916196864910953f, -0.8484259306171735f},
                    {-0.037746272428505985f, -0.5046080629916063f, -0.9376968729607804f}, 1.0f
                },
                {
                    {0.061561600646635975f, 0.7832404878038607f, -1.5706527300114692f},
                    {0.7736241839920268f, 0.842520569299398f, -1.759761999249333f},
                    {0.061561600646635975f, -0.11147641198174622f, -1.5706527300114692f},
                    {0.7736241839920268f, -0.05219633048620904f, -1.7597619992493332f}, 1.0f
                }
            }
        },
        {"/data/local/tmp/depth4.png", "/data/local/tmp/bg4.png",
            {
                {
                    {-1.1140602412436535f, 1.1247723589479193f, -2.528059778206752f},
                    {-0.1726559694111145f, 1.209657611405191f, -2.64458643443324f},
                    {-1.1140602412436535f, -0.43919682587490183f, -2.528059778206752f},
                    {-0.1726559694111145f, -0.35431157341763f, -2.64458643443324f}, 1.0f
                },
                {
                    {0.29804616650515486f, 1.2521002376338268f, -2.7028497625464833f},
                    {1.2394504383376939f, 1.3369854900910987f, -2.819376418772971f},
                    {0.29804616650515486f, -0.31186894718899405f, -2.7028497625464833f},
                    {1.2394504383376939f, -0.22698369473172225f, -2.819376418772971f}, 1.0f
                }
            }
        },
        {"/data/local/tmp/depth4.png", "/data/local/tmp/bg4.png",
            {
                {
                    {-0.734762049452281f, 0.1766254926567983f, -1.667344650680176f},
                    {-0.07042783467827862f, 0.20659571951213027f, -1.7495756825340811f},
                    {-0.734762049452281f, -0.8954912477699675f, -1.6673446506801761f},
                    {-0.07042783467827868f, -0.8655210209146356f, -1.7495756825340811f}, 1.0f
                },
                {
                    {0.0956936537065108f, 0.8943737819561937f, -2.441481326998546f},
                    {1.1295805176994327f, 0.972475575864333f, -2.5694554426705167f},
                    {0.0956936537065108f, -0.587093497064269f, -2.441481326998546f},
                    {1.1295805176994327f, -0.5089917031561298f, -2.5694554426705167f}, 1.0f
                }
            }
        }
    };
};

static constexpr float CARD_X = 300;
static constexpr float CARD_Y = 400;
static constexpr float CARD_WIDTH = 500;
static constexpr float CARD_HEIGHT = 500;
static constexpr uint32_t CARD_BG_COLOR = 0xffffffff;

GRAPHIC_TEST(AppearanceTest, CONTENT_DISPLAY_TEST, Appearance_Spatial_Effect_Test_1)
{
    auto& imgConfig = AppearanceTest::GetImageConfigs()[0];
    DepthSpaceConfig spaceConfig;
    spaceConfig.depthImagePath = imgConfig.depthImagePath;
    spaceConfig.backgroundImagePath = imgConfig.backgroundImagePath;

    auto depthNode = SpatialEffectTestUtil::CreateDepthNode(GetRootNode(), spaceConfig);
    RegisterNode(depthNode);

    auto bgNode = SpatialEffectTestUtil::CreateBackgroundNode(depthNode, spaceConfig);
    RegisterNode(bgNode);

    for (const auto& para : imgConfig.spatialParas) {
        auto cardNode = SpatialEffectTestUtil::CreateCardNode(
            depthNode, CARD_X, CARD_Y, CARD_WIDTH, CARD_HEIGHT, CARD_BG_COLOR, para);
        RegisterNode(cardNode);
    }
}

GRAPHIC_TEST(AppearanceTest, CONTENT_DISPLAY_TEST, Appearance_Spatial_Effect_Test_2)
{
    auto& imgConfig = AppearanceTest::GetImageConfigs()[1];
    DepthSpaceConfig spaceConfig;
    spaceConfig.depthImagePath = imgConfig.depthImagePath;
    spaceConfig.backgroundImagePath = imgConfig.backgroundImagePath;

    auto depthNode = SpatialEffectTestUtil::CreateDepthNode(GetRootNode(), spaceConfig);
    RegisterNode(depthNode);

    auto bgNode = SpatialEffectTestUtil::CreateBackgroundNode(depthNode, spaceConfig);
    RegisterNode(bgNode);

    for (const auto& para : imgConfig.spatialParas) {
        auto cardNode = SpatialEffectTestUtil::CreateCardNode(
            depthNode, CARD_X, CARD_Y, CARD_WIDTH, CARD_HEIGHT, CARD_BG_COLOR, para);
        RegisterNode(cardNode);
    }
}

GRAPHIC_TEST(AppearanceTest, CONTENT_DISPLAY_TEST, Appearance_Spatial_Effect_Test_3)
{
    auto& imgConfig = AppearanceTest::GetImageConfigs()[2];
    DepthSpaceConfig spaceConfig;
    spaceConfig.depthImagePath = imgConfig.depthImagePath;
    spaceConfig.backgroundImagePath = imgConfig.backgroundImagePath;

    auto depthNode = SpatialEffectTestUtil::CreateDepthNode(GetRootNode(), spaceConfig);
    RegisterNode(depthNode);

    auto bgNode = SpatialEffectTestUtil::CreateBackgroundNode(depthNode, spaceConfig);
    RegisterNode(bgNode);

    for (const auto& para : imgConfig.spatialParas) {
        auto cardNode = SpatialEffectTestUtil::CreateCardNode(
            depthNode, CARD_X, CARD_Y, CARD_WIDTH, CARD_HEIGHT, CARD_BG_COLOR, para);
        RegisterNode(cardNode);
    }
}

GRAPHIC_TEST(AppearanceTest, CONTENT_DISPLAY_TEST, Appearance_Spatial_Effect_Test_4)
{
    auto& imgConfig = AppearanceTest::GetImageConfigs()[3];
    DepthSpaceConfig spaceConfig;
    spaceConfig.depthImagePath = imgConfig.depthImagePath;
    spaceConfig.backgroundImagePath = imgConfig.backgroundImagePath;

    auto depthNode = SpatialEffectTestUtil::CreateDepthNode(GetRootNode(), spaceConfig);
    RegisterNode(depthNode);

    auto bgNode = SpatialEffectTestUtil::CreateBackgroundNode(depthNode, spaceConfig);
    RegisterNode(bgNode);

    for (const auto& para : imgConfig.spatialParas) {
        auto cardNode = SpatialEffectTestUtil::CreateCardNode(
            depthNode, CARD_X, CARD_Y, CARD_WIDTH, CARD_HEIGHT, CARD_BG_COLOR, para);
        RegisterNode(cardNode);
    }
}

GRAPHIC_TEST(AppearanceTest, CONTENT_DISPLAY_TEST, Appearance_Spatial_Effect_Test_5)
{
    auto& imgConfig = AppearanceTest::GetImageConfigs()[4];
    DepthSpaceConfig spaceConfig;
    spaceConfig.depthImagePath = imgConfig.depthImagePath;
    spaceConfig.backgroundImagePath = imgConfig.backgroundImagePath;

    auto depthNode = SpatialEffectTestUtil::CreateDepthNode(GetRootNode(), spaceConfig);
    RegisterNode(depthNode);

    auto bgNode = SpatialEffectTestUtil::CreateBackgroundNode(depthNode, spaceConfig);
    RegisterNode(bgNode);

    for (const auto& para : imgConfig.spatialParas) {
        auto cardNode = SpatialEffectTestUtil::CreateCardNode(
            depthNode, CARD_X, CARD_Y, CARD_WIDTH, CARD_HEIGHT, CARD_BG_COLOR, para);
        RegisterNode(cardNode);
    }
}

GRAPHIC_TEST(AppearanceTest, CONTENT_DISPLAY_TEST, Appearance_Spatial_Effect_Test_6)
{
    auto& imgConfig = AppearanceTest::GetImageConfigs()[5];
    DepthSpaceConfig spaceConfig;
    spaceConfig.depthImagePath = imgConfig.depthImagePath;
    spaceConfig.backgroundImagePath = imgConfig.backgroundImagePath;

    auto depthNode = SpatialEffectTestUtil::CreateDepthNode(GetRootNode(), spaceConfig);
    RegisterNode(depthNode);

    auto bgNode = SpatialEffectTestUtil::CreateBackgroundNode(depthNode, spaceConfig);
    RegisterNode(bgNode);

    for (const auto& para : imgConfig.spatialParas) {
        auto cardNode = SpatialEffectTestUtil::CreateCardNode(
            depthNode, CARD_X, CARD_Y, CARD_WIDTH, CARD_HEIGHT, CARD_BG_COLOR, para);
        RegisterNode(cardNode);
    }
}

GRAPHIC_TEST(AppearanceTest, CONTENT_DISPLAY_TEST, Appearance_Spatial_Effect_Test_7)
{
    auto& imgConfig = AppearanceTest::GetImageConfigs()[6];
    DepthSpaceConfig spaceConfig;
    spaceConfig.depthImagePath = imgConfig.depthImagePath;
    spaceConfig.backgroundImagePath = imgConfig.backgroundImagePath;

    auto depthNode = SpatialEffectTestUtil::CreateDepthNode(GetRootNode(), spaceConfig);
    RegisterNode(depthNode);

    auto bgNode = SpatialEffectTestUtil::CreateBackgroundNode(depthNode, spaceConfig);
    RegisterNode(bgNode);

    for (const auto& para : imgConfig.spatialParas) {
        auto cardNode = SpatialEffectTestUtil::CreateCardNode(
            depthNode, CARD_X, CARD_Y, CARD_WIDTH, CARD_HEIGHT, CARD_BG_COLOR, para);
        RegisterNode(cardNode);
    }
}

GRAPHIC_TEST(AppearanceTest, CONTENT_DISPLAY_TEST, Appearance_Spatial_Effect_Test_8)
{
    auto& imgConfig = AppearanceTest::GetImageConfigs()[8];
    DepthSpaceConfig spaceConfig;
    spaceConfig.depthImagePath = imgConfig.depthImagePath;
    spaceConfig.backgroundImagePath = imgConfig.backgroundImagePath;

    auto depthNode = SpatialEffectTestUtil::CreateDepthNode(GetRootNode(), spaceConfig);
    RegisterNode(depthNode);

    auto bgNode = SpatialEffectTestUtil::CreateBackgroundNode(depthNode, spaceConfig);
    RegisterNode(bgNode);

    for (const auto& para : imgConfig.spatialParas) {
        auto cardNode = SpatialEffectTestUtil::CreateCardNode(
            depthNode, CARD_X, CARD_Y, CARD_WIDTH, CARD_HEIGHT, CARD_BG_COLOR, para);
        RegisterNode(cardNode);
    }
}

} // namespace OHOS::Rosen