/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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
#include "transaction/rs_interfaces.h"
#include "pixel_map.h"
#include <filesystem>
#include "rs_graphic_test_utils.h"
using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
namespace {
    constexpr int SCREEN_WIDTH = 1200;
    constexpr int SCREEN_HEIGHT = 960;
    constexpr int SLEEP_TIME_FOR_PROXY = 50000; // 50ms
    constexpr uint32_t BIG_PIXELMAP_WIDTH = 6 * 1024;
    constexpr uint32_t BIG_PIXELMAP_HEIGHT = 1024;
    constexpr uint32_t BIG_COLOR_LENTH = BIG_PIXELMAP_WIDTH * BIG_PIXELMAP_HEIGHT;
    constexpr uint32_t PIXELMAP_DEFUALT_COLOR = 0xCCFFCC;
    constexpr uint32_t MAX_PIXELMAP_SIZE = 1000;
}

class WatermarktTest : public RSGraphicTest {
public:
    // called before each tests
    void BeforeEach() override {}
    // called after each tests
    void AfterEach() override
    {
        RSInterfaces::GetInstance().ClearSurfaceWatermark(getpid(), watermarkName_);
        RSInterfaces::GetInstance().ShowWatermark(DisplayWaterMarkPixelMap_, false);
    }

    RSSurfaceNode::SharedPtr CreateTestSurfaceNode()
    {
        struct RSSurfaceNodeConfig rsSurfaceNodeConfig1;
        rsSurfaceNodeConfig1.SurfaceNodeName = "watermarkAppWindow";
        rsSurfaceNodeConfig1.isSync = false;
        RSSurfaceNodeType rsSurfaceNodeType = RSSurfaceNodeType::APP_WINDOW_NODE;
        auto surfaceNode = RSSurfaceNode::Create(rsSurfaceNodeConfig1, rsSurfaceNodeType);
        surfaceNode->SetBounds({ 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT });
        surfaceNode->SetFrame({ 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT });
        surfaceNode->SetBackgroundColor(0x00CC00FF);
        return surfaceNode;
    }

    std::shared_ptr<Media::PixelMap> DecodePixelMap(
        const std::string& pathName, const Media::AllocatorType& allocatorType = Media::AllocatorType::SHARE_MEM_ALLOC)
    {
        uint32_t errCode = 0;
        std::unique_ptr<Media::ImageSource> imageSource =
            Media::ImageSource::CreateImageSource(pathName, Media::SourceOptions(), errCode);
        if (imageSource == nullptr || errCode != 0) {
            return nullptr;
        }

        Media::DecodeOptions decodeOpt;
        decodeOpt.allocatorType = allocatorType;
        std::shared_ptr<Media::PixelMap> pixelmap = imageSource->CreatePixelMap(decodeOpt, errCode);
        if (pixelmap == nullptr || errCode != 0) {
            return nullptr;
        }
        return pixelmap;
    }
    std::string watermarkName_;
    std::shared_ptr<Media::PixelMap> displayWaterMarkpixelMap_;
}

/*
 * @tc.name: SA_Surface_Watermark_Test01
 * @tc.desc: test pixelMap is Less than 6M, saSurfaceWaterMaxSize = SA_WATER_MARK_MIDDLE_SIZE, show watermark
 * @tc.type FUN
*/
GRAPHIC_N_TEST(WatermarktTest, CONTENT_DISPLAY_TEST, SA_Surface_Watermark_Test01)
{
    auto surfaceNode = CreateTestSurfaceNode();
    GetRootNode()->AddChild(surfaceNode);
    RegisterNode(surfaceNode);
    watermarkName_ = "SA_Surface_Watermark_Test01";
    surfaceNode->SetWatermarkEnabled(watermarkName_, true);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    auto pixelMap = DecodePixelMap("/data/local/tmp/Images/450x311.jpg");
    RSInterfaces::GetInstance().SetWatermark(watermarkName_, pixelMap,
        SaSurfaceWatermarkMaxSize::SA_WATER_MARK_MIDDLE_SIZE);
    usleep(SLEEP_TIME_FOR_PROXY);
};

/*
 * @tc.name: SA_Surface_Watermark_Test02
 * @tc.desc: test pixelMap is Less than 6M, Name Length exceeds 128 not show watermark
 * @tc.type FUN
*/
GRAPHIC_N_TEST(WatermarktTest, CONTENT_DISPLAY_TEST, SA_Surface_Watermark_Test02)
{
    auto surfaceNode = CreateTestSurfaceNode();
    GetRootNode()->AddChild(surfaceNode);
    RegisterNode(surfaceNode);
    watermarkName_ = "SA_Surface_Watermark_Test02";
    surfaceNode->SetWatermarkEnabled(watermarkName_, true);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    std::string name5(129, 't');
    auto pixelMap = DecodePixelMap("/data/local/tmp/Images/450x311.jpg");
    RSInterfaces::GetInstance().SetWatermark(name5, pixelMap,
        SaSurfaceWatermarkMaxSize::SA_WATER_MARK_MIDDLE_SIZE);
    usleep(SLEEP_TIME_FOR_PROXY);
};

/*
 * @tc.name: SA_Surface_Watermark_Test03
 * @tc.desc: test pixelMap is Less than 6M, Name is empty
 * @tc.type FUN
*/
GRAPHIC_N_TEST(WatermarktTest, CONTENT_DISPLAY_TEST, SA_Surface_Watermark_Test03)
{
    auto surfaceNode = CreateTestSurfaceNode();
    GetRootNode()->AddChild(surfaceNode);
    RegisterNode(surfaceNode);
    watermarkName_ = "SA_Surface_Watermark_Test03";
    surfaceNode->SetWatermarkEnabled(watermarkName_, true);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    std::string name;
    auto pixelMap = DecodePixelMap("/data/local/tmp/Images/450x311.jpg");
    RSInterfaces::GetInstance().SetWatermark(name, pixelMap,
        SaSurfaceWatermarkMaxSize::SA_WATER_MARK_MIDDLE_SIZE);
    usleep(SLEEP_TIME_FOR_PROXY);
};

/*
 * @tc.name: SA_Surface_Watermark_Test04
 * @tc.desc: test pixelMap is Astc, saSurfaceWaterMaxSize = SA_WATER_MARK_MIDDLE_SIZE, not show watermark
 * @tc.type FUN
*/
GRAPHIC_N_TEST(WatermarktTest, CONTENT_DISPLAY_TEST, SA_Surface_Watermark_Test04)
{
    auto surfaceNode = CreateTestSurfaceNode();
    GetRootNode()->AddChild(surfaceNode);
    RegisterNode(surfaceNode);
    watermarkName_ = "SA_Surface_Watermark_Test04";
    surfaceNode->SetWatermarkEnabled(watermarkName_, true);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    std::string name;
    auto pixelMap = DecodePixelMap("/data/local/tmp/Images/450x311.jpg");
    pixelMap->SetAstc(true);
    RSInterfaces::GetInstance().SetWatermark(name, pixelMap,
        SaSurfaceWatermarkMaxSize::SA_WATER_MARK_MIDDLE_SIZE);
    usleep(SLEEP_TIME_FOR_PROXY);
};

/*
 * @tc.name: SA_Surface_Watermark_Test05
 * @tc.desc: test pixelMap is nullptr not show watermark
 * @tc.type FUN
*/
GRAPHIC_N_TEST(WatermarktTest, CONTENT_DISPLAY_TEST, SA_Surface_Watermark_Test05)
{
    auto surfaceNode = CreateTestSurfaceNode();
    GetRootNode()->AddChild(surfaceNode);
    RegisterNode(surfaceNode);
    watermarkName_ = "SA_Surface_Watermark_Test05";
    surfaceNode->SetWatermarkEnabled(watermarkName_, true);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    std::string name;
    auto pixelMap = DecodePixelMap("/data/local/tmp/Images/450x311.jpg");
    RSInterfaces::GetInstance().SetWatermark(name, nullptr,
        SaSurfaceWatermarkMaxSize::SA_WATER_MARK_MIDDLE_SIZE);
    usleep(SLEEP_TIME_FOR_PROXY);
};

/*
 * @tc.name: SA_Surface_Watermark_Test06
 * @tc.desc: test pixelMap is more than 6M, saSurfaceWaterMaxSize = SA_WATER_MARK_MIDDLE_SIZE, not show watermark
 * @tc.type FUN
*/
GRAPHIC_N_TEST(WatermarktTest, CONTENT_DISPLAY_TEST, SA_Surface_Watermark_Test06)
{
    auto surfaceNode = CreateTestSurfaceNode();
    GetRootNode()->AddChild(surfaceNode);
    RegisterNode(surfaceNode);
    watermarkName_ = "SA_Surface_Watermark_Test06";
    surfaceNode->SetWatermarkEnabled(watermarkName_, true);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    std::string name;

    uint32_t *color = new uint32_t[BIG_COLOR_LENTH];
    // pixelMap is 25141248
    Media::InitializationOptions opts;
    opts.size.width = BIG_PIXELMAP_WIDTH;
    opts.size.height = BIG_PIXELMAP_HEIGHT;
    opts.pixelFormat = Media::PixelFormat::RGBA_8888;
    opts.alphaType = Media::AlphaType::IMAGE_ALPHA_TYPE_OPAQUE;
    std::fill(color, color + BIG_COLOR_LENTH, PIXELMAP_DEFUALT_COLOR);
    std::shared_ptr<Media::PixelMap> pixelMap = Media::PixelMap::Create(color, BIG_COLOR_LENTH, opts);

    RSInterfaces::GetInstance().SetWatermark(name, pixelMap,
        SaSurfaceWatermarkMaxSize::SA_WATER_MARK_MIDDLE_SIZE);
    usleep(SLEEP_TIME_FOR_PROXY);
    delete []color;
};

/*
 * @tc.name: SA_Surface_Watermark_Test07
 * @tc.desc: test pixelMap is Less than 512M, SaSurfaceWatermarkMaxSize = SA_WATER_MARK_DEFAULT_SIZE, show watermark
 * @tc.type FUN
*/
GRAPHIC_N_TEST(WatermarktTest, CONTENT_DISPLAY_TEST, SA_Surface_Watermark_Test07)
{
    auto surfaceNode = CreateTestSurfaceNode();
    GetRootNode()->AddChild(surfaceNode);
    RegisterNode(surfaceNode);
    watermarkName_ = "SA_Surface_Watermark_Test07";
    surfaceNode->SetWatermarkEnabled(watermarkName_, true);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    auto pixelMap = DecodePixelMap("/data/local/tmp/Images/AllBlack.jpg");
    RSInterfaces::GetInstance().SetWatermark(watermarkName_, pixelMap);
    usleep(SLEEP_TIME_FOR_PROXY);
};

/*
 * @tc.name: SA_Surface_Watermark_Test08
 * @tc.desc: test pixelMap is Less than 512M, pixelMap is astc, not show watermark
 * @tc.type FUN
*/
GRAPHIC_N_TEST(WatermarktTest, CONTENT_DISPLAY_TEST, SA_Surface_Watermark_Test08)
{
    auto surfaceNode = CreateTestSurfaceNode();
    GetRootNode()->AddChild(surfaceNode);
    RegisterNode(surfaceNode);
    watermarkName_ = "SA_Surface_Watermark_Test07";
    surfaceNode->SetWatermarkEnabled(watermarkName_, true);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    auto pixelMap = DecodePixelMap("/data/local/tmp/Images/AllBlack.jpg");
    pixelMap->SetAstc(true);
    RSInterfaces::GetInstance().SetWatermark(watermarkName_, pixelMap);
    usleep(SLEEP_TIME_FOR_PROXY);
};

/*
 * @tc.name: SA_Surface_Watermark_Test09
 * @tc.desc: test pixelMap is Less than 512M, Name length exceeds 128 not show watermark
 * @tc.type FUN
*/
GRAPHIC_N_TEST(WatermarktTest, CONTENT_DISPLAY_TEST, SA_Surface_Watermark_Test09)
{
    auto surfaceNode = CreateTestSurfaceNode();
    GetRootNode()->AddChild(surfaceNode);
    RegisterNode(surfaceNode);
    watermarkName_ = "SA_Surface_Watermark_Test08";
    surfaceNode->SetWatermarkEnabled(watermarkName_, true);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    std::string name5(129, 't');
    auto pixelMap = DecodePixelMap("/data/local/tmp/Images/AllBlack.jpg");
    RSInterfaces::GetInstance().SetWatermark(name5, pixelMap);
    usleep(SLEEP_TIME_FOR_PROXY);
};

/*
 * @tc.name: SA_Surface_Watermark_Test10
 * @tc.desc: test pixelMap is max 1000
 * @tc.type FUN
*/
GRAPHIC_N_TEST(WatermarktTest, CONTENT_DISPLAY_TEST, SA_Surface_Watermark_Test10)
{
    auto surfaceNode = CreateTestSurfaceNode();
    GetRootNode()->AddChild(surfaceNode);
    RegisterNode(surfaceNode);
    watermarkName_ = "SA_Surface_Watermark_Test10";
    surfaceNode->SetWatermarkEnabled(watermarkName_, true);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    auto pixelMap1 = DecodePixelMap("/data/local/tmp/Images/AllWhite.jpg");
    auto pixelMap2 = DecodePixelMap("/data/local/tmp/Images/AllBlack.jpg");
    std::vector<std::string> nameList;
    for (uint32_t i = 0; i < MAX_PIXELMAP_SIZE; i++) {
        std::string name("watermark" + std::to_string(i));
        nameList.push_back(name);
        RSInterfaces::GetInstance().SetWatermark(name, pixelMap1);
    }
    RSInterfaces::GetInstance().SetWatermark(watermarkName_, pixelMap2);
    usleep(SLEEP_TIME_FOR_PROXY);

    for (auto name : nameList) {
        RSInterfaces::GetInstance().ClearSurfaceWatermark(getpid(), name);
    }
};

/*
 * @tc.name: SA_Surface_Watermark_Test03
 * @tc.desc: test pixelMap is Less than 512, Name is empty, not show
 * @tc.type FUN
*/
GRAPHIC_N_TEST(WatermarktTest, CONTENT_DISPLAY_TEST, SA_Surface_Watermark_Test11)
{
    auto surfaceNode = CreateTestSurfaceNode();
    GetRootNode()->AddChild(surfaceNode);
    RegisterNode(surfaceNode);
    watermarkName_ = "SA_Surface_Watermark_Test11";
    surfaceNode->SetWatermarkEnabled(watermarkName_, true);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    std::string name5;
    auto pixelMap = DecodePixelMap("/data/local/tmp/Images/AllBlack.jpg");
    RSInterfaces::GetInstance().SetWatermark(name5, pixelMap);
    usleep(SLEEP_TIME_FOR_PROXY);
};

/*
 * @tc.name: SA_Surface_Watermark_Test12
 * @tc.desc: test pixelMap is Less than 512, surfaceNode set false && name is empty, not watermark
 * @tc.type FUN
*/
GRAPHIC_N_TEST(WatermarktTest, CONTENT_DISPLAY_TEST, SA_Surface_Watermark_Test12)
{
    auto surfaceNode = CreateTestSurfaceNode();
    GetRootNode()->AddChild(surfaceNode);
    RegisterNode(surfaceNode);
    watermarkName_ = "SA_Surface_Watermark_Test12";
    surfaceNode->SetWatermarkEnabled(watermarkName_, false);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    std::string name5;
    auto pixelMap = DecodePixelMap("/data/local/tmp/Images/AllBlack.jpg");
    RSInterfaces::GetInstance().SetWatermark(name5, pixelMap);
    usleep(SLEEP_TIME_FOR_PROXY);
};

/*
 * @tc.name: SA_Surface_Watermark_Test13
 * @tc.desc: test pixelMap is Less than 512, surfaceNode set false
 * @tc.type FUN
*/
GRAPHIC_N_TEST(WatermarktTest, CONTENT_DISPLAY_TEST, SA_Surface_Watermark_Test13)
{
    auto surfaceNode = CreateTestSurfaceNode();
    GetRootNode()->AddChild(surfaceNode);
    RegisterNode(surfaceNode);
    watermarkName_ = "SA_Surface_Watermark_Test13";
    surfaceNode->SetWatermarkEnabled(watermarkName_, false);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    auto pixelMap = DecodePixelMap("/data/local/tmp/Images/AllBlack.jpg");
    RSInterfaces::GetInstance().SetWatermark(watermarkName_, pixelMap);
    usleep(SLEEP_TIME_FOR_PROXY);
};

/*
 * @tc.name: SA_Surface_Watermark_Test14
 * @tc.desc: test pixelMap is Less than 512, surfaceNode set false, Name length exceeds 128
 *  surfaceNode set false not watermark
 * @tc.type FUNC
*/
GRAPHIC_N_TEST(WatermarktTest, CONTENT_DISPLAY_TEST, SA_Surface_Watermark_Test14)
{
    auto surfaceNode = CreateTestSurfaceNode();
    GetRootNode()->AddChild(surfaceNode);
    RegisterNode(surfaceNode);
    watermarkName_ = "SA_Surface_Watermark_Test14";
    surfaceNode->SetWatermarkEnabled(watermarkName_, false);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    std::string name5(129, 't');
    auto pixelMap = DecodePixelMap("/data/local/tmp/Images/AllBlack.jpg");
    RSInterfaces::GetInstance().SetWatermark(name5, pixelMap);
    usleep(SLEEP_TIME_FOR_PROXY);
};

/*
 * @tc.name: SA_Surface_Watermark_Test15
 * @tc.desc: test pixelMap is Less than 512, surfaceNode set false, SetAstc is true
 * surfaceNode set false, not watermark
 * @tc.type FUNC
*/
GRAPHIC_N_TEST(WatermarktTest, CONTENT_DISPLAY_TEST, SA_Surface_Watermark_Test15)
{
    auto surfaceNode = CreateTestSurfaceNode();
    GetRootNode()->AddChild(surfaceNode);
    RegisterNode(surfaceNode);
    watermarkName_ = "SA_Surface_Watermark_Test15";
    surfaceNode->SetWatermarkEnabled(watermarkName_, false);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    auto pixelMap = DecodePixelMap("/data/local/tmp/Images/AllBlack.jpg");
    pixelMap->SetAstc(true);
    RSInterfaces::GetInstance().SetWatermark(name5, pixelMap);
    usleep(SLEEP_TIME_FOR_PROXY);
};


/*
 * @tc.name: Display_Watermark_Test1
 * @tc.desc: pixelmap == nullptr isshow = false
 * @tc.type FUNC
*/
GRAPHIC_N_TEST(WatermarktTest, CONTENT_DISPLAY_TEST, Display_Watermark_Test1)
{
    auto surfaceNode = CreateTestSurfaceNode();
    GetRootNode()->AddChild(surfaceNode);
    RegisterNode(surfaceNode);
    watermarkName_ = "Display_Watermark_Test1";
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    RSInterfaces::GetInstance().ShowWatermark(nullptr, false);
    usleep(SLEEP_TIME_FOR_PROXY);
};

/*
 * @tc.name: Display_Watermark_Test2
 * @tc.desc: pixelmap == nullptr isshow = true, not show
 * @tc.type FUNC
*/
GRAPHIC_N_TEST(WatermarktTest, CONTENT_DISPLAY_TEST, Display_Watermark_Test2)
{
    auto surfaceNode = CreateTestSurfaceNode();
    GetRootNode()->AddChild(surfaceNode);
    RegisterNode(surfaceNode);
    watermarkName_ = "Display_Watermark_Test2";
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    RSInterfaces::GetInstance().ShowWatermark(nullptr, true);
    usleep(SLEEP_TIME_FOR_PROXY);
};


/*
 * @tc.name: Display_Watermark_Test3
 * @tc.desc: pixelmap is vaild isshow = false, not show
 * @tc.type FUNC
*/
GRAPHIC_N_TEST(WatermarktTest, CONTENT_DISPLAY_TEST, Display_Watermark_Test3)
{
    auto surfaceNode = CreateTestSurfaceNode();
    GetRootNode()->AddChild(surfaceNode);
    RegisterNode(surfaceNode);
    watermarkName_ = "Display_Watermark_Test3";
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);
    auto pixelMap = DecodePixelMap("/data/local/tmp/Images/450x311.jpg");

    RSInterfaces::GetInstance().ShowWatermark(pixelMap, false);
    usleep(SLEEP_TIME_FOR_PROXY);
};

/*
 * @tc.name: App_Surface_Watermark_Test1
 * @tc.desc: pixelMap is valid, SYSTEM_WATER_MARK, show watermark
 * @tc.type FUN
*/
GRAPHIC_N_TEST(WatermarktTest, CONTENT_DISPLAY_TEST, App_Surface_Watermark_Test1)
{
    auto surfaceNode = CreateTestSurfaceNode();
    GetRootNode()->AddChild(surfaceNode);
    RegisterNode(surfaceNode);
    watermarkName_ = "App_Surface_Watermark_Test1";
    surfaceNode->SetWatermarkEnabled(watermarkName_, true);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    auto pixelMap = DecodePixelMap("/data/local/tmp/Images/450x311.jpg");
    RSInterfaces::GetInstance().SetSurfaceWatermark(getpid(), watermarkName_, pixelMap, {surfaceNode->GetId()},
        SurfaceWatermarkType::SYSTEM_WATER_MARK);
    usleep(SLEEP_TIME_FOR_PROXY);
};

/*
 * @tc.name: App_Surface_Watermark_Test2
 * @tc.desc: pixelMap is valid, CUSTOM_WATER_MARK, show watermark
 * @tc.type FUN
*/
GRAPHIC_N_TEST(WatermarktTest, CONTENT_DISPLAY_TEST, App_Surface_Watermark_Test2)
{
    auto surfaceNode = CreateTestSurfaceNode();
    GetRootNode()->AddChild(surfaceNode);
    RegisterNode(surfaceNode);
    watermarkName_ = "App_Surface_Watermark_Test2";
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    auto pixelMap = DecodePixelMap("/data/local/tmp/Images/450x311.jpg");
    RSInterfaces::GetInstance().SetSurfaceWatermark(getpid(), watermarkName_, pixelMap, {surfaceNode->GetId()},
        SurfaceWatermarkType::CUSTOM_WATER_MARK);
    usleep(SLEEP_TIME_FOR_PROXY);
};

/*
 * @tc.name: App_Surface_Watermark_Test3
 * @tc.desc: test pixelMap is astc,  SYSTEM_WATER_MARK, not show watermark
 * @tc.type FUN
*/
GRAPHIC_N_TEST(WatermarktTest, CONTENT_DISPLAY_TEST, App_Surface_Watermark_Test3)
{
    auto surfaceNode = CreateTestSurfaceNode();
    GetRootNode()->AddChild(surfaceNode);
    RegisterNode(surfaceNode);
    watermarkName_ = "App_Surface_Watermark_Test3";
    surfaceNode->SetWatermarkEnabled(watermarkName_, true);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    auto pixelMap = DecodePixelMap("/data/local/tmp/Images/450x311.jpg");
    pixelMap->SetAstc(true);
    RSInterfaces::GetInstance().SetSurfaceWatermark(getpid(), watermarkName_, pixelMap, {surfaceNode->GetId()},
        SurfaceWatermarkType::SYSTEM_WATER_MARK);
    usleep(SLEEP_TIME_FOR_PROXY);
};

/*
 * @tc.name: App_Surface_Watermark_Test4
 * @tc.desc: test pixelMap is astc,  CUSTOM_WATER_MARK, not show watermark
 * @tc.type FUN
*/
GRAPHIC_N_TEST(WatermarktTest, CONTENT_DISPLAY_TEST, App_Surface_Watermark_Test4)
{
    auto surfaceNode = CreateTestSurfaceNode();
    GetRootNode()->AddChild(surfaceNode);
    RegisterNode(surfaceNode);
    watermarkName_ = "App_Surface_Watermark_Test4";
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    auto pixelMap = DecodePixelMap("/data/local/tmp/Images/450x311.jpg");
    pixelMap->SetAstc(true);
    RSInterfaces::GetInstance().SetSurfaceWatermark(getpid(), watermarkName_, pixelMap, {surfaceNode->GetId()},
        SurfaceWatermarkType::CUSTOM_WATER_MARK);
    usleep(SLEEP_TIME_FOR_PROXY);
};


/*
 * @tc.name: App_Surface_Watermark_Test5
 * @tc.desc: name length = 129, SYSTEM_WATER_MARK, show watermark
 * @tc.type FUN
*/
GRAPHIC_N_TEST(WatermarktTest, CONTENT_DISPLAY_TEST, App_Surface_Watermark_Test5)
{
    auto surfaceNode = CreateTestSurfaceNode();
    GetRootNode()->AddChild(surfaceNode);
    RegisterNode(surfaceNode);
    watermarkName_ = "App_Surface_Watermark_Test5";
    surfaceNode->SetWatermarkEnabled(watermarkName_, true);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    auto pixelMap = DecodePixelMap("/data/local/tmp/Images/450x311.jpg");
    std::string name5(129, 't');
    RSInterfaces::GetInstance().SetSurfaceWatermark(getpid(), name5, pixelMap, {surfaceNode->GetId()},
        SurfaceWatermarkType::SYSTEM_WATER_MARK);
    usleep(SLEEP_TIME_FOR_PROXY);
};

/*
 * @tc.name: App_Surface_Watermark_Test6
 * @tc.desc: name length = 129, CUSTOM_WATER_MARK, show watermark
 * @tc.type FUN
*/
GRAPHIC_N_TEST(WatermarktTest, CONTENT_DISPLAY_TEST, App_Surface_Watermark_Test6)
{
    auto surfaceNode = CreateTestSurfaceNode();
    GetRootNode()->AddChild(surfaceNode);
    RegisterNode(surfaceNode);
    watermarkName_ = "App_Surface_Watermark_Test6";
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    auto pixelMap = DecodePixelMap("/data/local/tmp/Images/450x311.jpg");
    std::string name5(129, 't');
    RSInterfaces::GetInstance().SetSurfaceWatermark(getpid(), name5, pixelMap, {surfaceNode->GetId()},
        SurfaceWatermarkType::CUSTOM_WATER_MARK);
    usleep(SLEEP_TIME_FOR_PROXY);
};


/*
 * @tc.name: App_Surface_Watermark_Test7
 * @tc.desc: name is empty, SYSTEM_WATER_MARK, show watermark
 * @tc.type FUN
*/
GRAPHIC_N_TEST(WatermarktTest, CONTENT_DISPLAY_TEST, App_Surface_Watermark_Test7)
{
    auto surfaceNode = CreateTestSurfaceNode();
    GetRootNode()->AddChild(surfaceNode);
    RegisterNode(surfaceNode);
    watermarkName_ = "App_Surface_Watermark_Test7";
    surfaceNode->SetWatermarkEnabled(watermarkName_, true);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    auto pixelMap = DecodePixelMap("/data/local/tmp/Images/450x311.jpg");
    std::string name5;
    RSInterfaces::GetInstance().SetSurfaceWatermark(getpid(), name5, pixelMap, {surfaceNode->GetId()},
        SurfaceWatermarkType::SYSTEM_WATER_MARK);
    usleep(SLEEP_TIME_FOR_PROXY);
};

/*
 * @tc.name: App_Surface_Watermark_Test8
 * @tc.desc: name is empty, CUSTOM_WATER_MARK, show watermark
 * @tc.type FUN
*/
GRAPHIC_N_TEST(WatermarktTest, CONTENT_DISPLAY_TEST, App_Surface_Watermark_Test8)
{
    auto surfaceNode = CreateTestSurfaceNode();
    GetRootNode()->AddChild(surfaceNode);
    RegisterNode(surfaceNode);
    watermarkName_ = "App_Surface_Watermark_Test8";
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    auto pixelMap = DecodePixelMap("/data/local/tmp/Images/450x311.jpg");
    std::string name5;
    RSInterfaces::GetInstance().SetSurfaceWatermark(getpid(), name5, pixelMap, {surfaceNode->GetId()},
        SurfaceWatermarkType::CUSTOM_WATER_MARK);
    usleep(SLEEP_TIME_FOR_PROXY);
};

/*
 * @tc.name: App_Surface_Watermark_Test9
 * @tc.desc: pixelMap is nullptr, SYSTEM_WATER_MARK, show watermark
 * @tc.type FUN
*/
GRAPHIC_N_TEST(WatermarktTest, CONTENT_DISPLAY_TEST, App_Surface_Watermark_Test9)
{
    auto surfaceNode = CreateTestSurfaceNode();
    GetRootNode()->AddChild(surfaceNode);
    RegisterNode(surfaceNode);
    watermarkName_ = "App_Surface_Watermark_Test9";
    surfaceNode->SetWatermarkEnabled(watermarkName_, true);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    RSInterfaces::GetInstance().SetSurfaceWatermark(getpid(), watermarkName_, nullptr, {surfaceNode->GetId()},
        SurfaceWatermarkType::SYSTEM_WATER_MARK);
    usleep(SLEEP_TIME_FOR_PROXY);
};

/*
 * @tc.name: App_Surface_Watermark_Test10
 * @tc.desc: pixelMap is nullptr, CUSTOM_WATER_MARK, show watermark
 * @tc.type FUN
*/
GRAPHIC_N_TEST(WatermarktTest, CONTENT_DISPLAY_TEST, App_Surface_Watermark_Test10)
{
    auto surfaceNode = CreateTestSurfaceNode();
    GetRootNode()->AddChild(surfaceNode);
    RegisterNode(surfaceNode);
    watermarkName_ = "App_Surface_Watermark_Test10";
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    RSInterfaces::GetInstance().SetSurfaceWatermark(getpid(), watermarkName_, nullptr, {surfaceNode->GetId()},
        SurfaceWatermarkType::CUSTOM_WATER_MARK);
    usleep(SLEEP_TIME_FOR_PROXY);
};


/*
 * @tc.name: App_Surface_Watermark_Test11
 * @tc.desc: nodeList is empty, SYSTEM_WATER_MARK, show watermark
 * @tc.type FUN
*/
GRAPHIC_N_TEST(WatermarktTest, CONTENT_DISPLAY_TEST, App_Surface_Watermark_Test11)
{
    auto surfaceNode = CreateTestSurfaceNode();
    GetRootNode()->AddChild(surfaceNode);
    RegisterNode(surfaceNode);
    watermarkName_ = "App_Surface_Watermark_Test11";
    surfaceNode->SetWatermarkEnabled(watermarkName_, true);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    auto pixelMap = DecodePixelMap("/data/local/tmp/Images/450x311.jpg");
    RSInterfaces::GetInstance().SetSurfaceWatermark(getpid(), watermarkName_, pixelMap, {},
        SurfaceWatermarkType::SYSTEM_WATER_MARK);
    usleep(SLEEP_TIME_FOR_PROXY);
};

/*
 * @tc.name: App_Surface_Watermark_Test12
 * @tc.desc: nodeList is empty, CUSTOM_WATER_MARK, not show watermark
 * @tc.type FUN
*/
GRAPHIC_N_TEST(WatermarktTest, CONTENT_DISPLAY_TEST, App_Surface_Watermark_Test12)
{
    auto surfaceNode = CreateTestSurfaceNode();
    GetRootNode()->AddChild(surfaceNode);
    RegisterNode(surfaceNode);
    watermarkName_ = "App_Surface_Watermark_Test12";
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    auto pixelMap = DecodePixelMap("/data/local/tmp/Images/450x311.jpg");
    RSInterfaces::GetInstance().SetSurfaceWatermark(getpid(), watermarkName_, pixelMap, {},
        SurfaceWatermarkType::CUSTOM_WATER_MARK);
    usleep(SLEEP_TIME_FOR_PROXY);
};

/*
 * @tc.name: App_Surface_Watermark_Test13
 * @tc.desc: nodeList is empty, INVALID_WATER_MARK, not show watermark
 * @tc.type FUN
*/
GRAPHIC_N_TEST(WatermarktTest, CONTENT_DISPLAY_TEST, App_Surface_Watermark_Test13)
{
    auto surfaceNode = CreateTestSurfaceNode();
    GetRootNode()->AddChild(surfaceNode);
    RegisterNode(surfaceNode);
    watermarkName_ = "App_Surface_Watermark_Test13";
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    auto pixelMap = DecodePixelMap("/data/local/tmp/Images/450x311.jpg");
    RSInterfaces::GetInstance().SetSurfaceWatermark(getpid(), watermarkName_, pixelMap, {},
        SurfaceWatermarkType::INVALID_WATER_MARK);
    usleep(SLEEP_TIME_FOR_PROXY);
};

/*
 * @tc.name: App_Surface_Watermark_Test14
 * @tc.desc: CUSTOM_WATER_MARK, Pid != pid, but nativeTool is systemCalling show watermark
 * @tc.type FUN
*/
GRAPHIC_N_TEST(WatermarktTest, CONTENT_DISPLAY_TEST, App_Surface_Watermark_Test14)
{
    auto surfaceNode = CreateTestSurfaceNode();
    GetRootNode()->AddChild(surfaceNode);
    RegisterNode(surfaceNode);
    watermarkName_ = "App_Surface_Watermark_Test14";
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    auto pixelMap = DecodePixelMap("/data/local/tmp/Images/450x311.jpg");
    RSInterfaces::GetInstance().SetSurfaceWatermark(222, watermarkName_, pixelMap, {surfaceNode->GetId()},
        SurfaceWatermarkType::CUSTOM_WATER_MARK);
    usleep(SLEEP_TIME_FOR_PROXY);
};

/*
 * @tc.name: App_Surface_Watermark_Test15
 * @tc.desc: SYSTEM_WATER_MARK, Pid != pid, show watermark
 * @tc.type FUN
*/
GRAPHIC_N_TEST(WatermarktTest, CONTENT_DISPLAY_TEST, App_Surface_Watermark_Test15)
{
    auto surfaceNode = CreateTestSurfaceNode();
    GetRootNode()->AddChild(surfaceNode);
    RegisterNode(surfaceNode);
    watermarkName_ = "App_Surface_Watermark_Test15";
    surfaceNode->SetWatermarkEnabled(watermarkName_, true);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    auto pixelMap = DecodePixelMap("/data/local/tmp/Images/450x311.jpg");
    RSInterfaces::GetInstance().SetSurfaceWatermark(222, watermarkName_, pixelMap, {surfaceNode->GetId()},
        SurfaceWatermarkType::SYSTEM_WATER_MARK);
    usleep(SLEEP_TIME_FOR_PROXY);
};

/*
 * @tc.name: App_Surface_Watermark_Test16
 * @tc.desc: pixelmap is valid, SYSTEM_WATER_MARK, clear watermark, not show watermark
 * @tc.type FUN
*/
GRAPHIC_N_TEST(WatermarktTest, CONTENT_DISPLAY_TEST, App_Surface_Watermark_Test16)
{
    auto surfaceNode = CreateTestSurfaceNode();
    GetRootNode()->AddChild(surfaceNode);
    RegisterNode(surfaceNode);
    watermarkName_ = "App_Surface_Watermark_Test16";
    surfaceNode->SetWatermarkEnabled(watermarkName_, true);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    auto pixelMap = DecodePixelMap("/data/local/tmp/Images/450x311.jpg");
    RSInterfaces::GetInstance().SetSurfaceWatermark(getpid(), watermarkName_, pixelMap, {surfaceNode->GetId()},
        SurfaceWatermarkType::SYSTEM_WATER_MARK);
    usleep(SLEEP_TIME_FOR_PROXY);
    RSInterfaces::GetInstance().ClearSurfaceWatermark(getpid(), watermarkName_);
};

/*
 * @tc.name: App_Surface_Watermark_Test17
 * @tc.desc: pixelmap is valid, CUSTOM_WATER_MARK, clear watermark, not show watermark
 * @tc.type FUN
*/
GRAPHIC_N_TEST(WatermarktTest, CONTENT_DISPLAY_TEST, App_Surface_Watermark_Test17)
{
    auto surfaceNode = CreateTestSurfaceNode();
    GetRootNode()->AddChild(surfaceNode);
    RegisterNode(surfaceNode);
    watermarkName_ = "App_Surface_Watermark_Test17";
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    auto pixelMap = DecodePixelMap("/data/local/tmp/Images/450x311.jpg");
    RSInterfaces::GetInstance().SetSurfaceWatermark(getpid(), watermarkName_, pixelMap, {surfaceNode->GetId()},
        SurfaceWatermarkType::CUSTOM_WATER_MARK);
    usleep(SLEEP_TIME_FOR_PROXY);
    RSInterfaces::GetInstance().ClearSurfaceWatermark(getpid(), watermarkName_);
};

/*
 * @tc.name: App_Surface_Watermark_Test18
 * @tc.desc: pixelmap is valid, CUSTOM_WATER_MARK, clear watermark, not show watermark
 * @tc.type FUN
*/
GRAPHIC_N_TEST(WatermarktTest, CONTENT_DISPLAY_TEST, App_Surface_Watermark_Test18)
{
    auto surfaceNode = CreateTestSurfaceNode();
    GetRootNode()->AddChild(surfaceNode);
    RegisterNode(surfaceNode);
    watermarkName_ = "App_Surface_Watermark_Test18";
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    auto pixelMap = DecodePixelMap("/data/local/tmp/Images/450x311.jpg");
    RSInterfaces::GetInstance().SetSurfaceWatermark(getpid(), watermarkName_, pixelMap, {surfaceNode->GetId()},
        SurfaceWatermarkType::CUSTOM_WATER_MARK);
    usleep(SLEEP_TIME_FOR_PROXY);
    RSInterfaces::GetInstance().ClearSurfaceWatermark(222333, watermarkName_);
};

/*
 * @tc.name: App_Surface_Watermark_Test19
 * @tc.desc: pixelmap is valid, SYSTEM_WATER_MARK, clear watermark surfaceNode, SYSTEM_WATER_MARK not process node
 * show watermark
 * @tc.type FUN
*/
GRAPHIC_N_TEST(WatermarktTest, CONTENT_DISPLAY_TEST, App_Surface_Watermark_Test19)
{
    auto surfaceNode = CreateTestSurfaceNode();
    GetRootNode()->AddChild(surfaceNode);
    RegisterNode(surfaceNode);
    watermarkName_ = "App_Surface_Watermark_Test19";
    surfaceNode->SetWatermarkEnabled(watermarkName_, true);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    auto pixelMap = DecodePixelMap("/data/local/tmp/Images/450x311.jpg");
    RSInterfaces::GetInstance().SetSurfaceWatermark(getpid(), watermarkName_, pixelMap, {surfaceNode->GetId()},
        SurfaceWatermarkType::SYSTEM_WATER_MARK);
    usleep(SLEEP_TIME_FOR_PROXY);
    RSInterfaces::GetInstance().ClearSurfaceWatermarkForNodes(getpid(), watermarkName_, surfaceNode->GetId());
};

/*
 * @tc.name: App_Surface_Watermark_Test20
 * @tc.desc: pixelmap is valid, CUSTOM_WATER_MARK, clear surface watermark, not show watermark
 * @tc.type FUN
*/
GRAPHIC_N_TEST(WatermarktTest, CONTENT_DISPLAY_TEST, App_Surface_Watermark_Test20)
{
    auto surfaceNode = CreateTestSurfaceNode();
    GetRootNode()->AddChild(surfaceNode);
    RegisterNode(surfaceNode);
    watermarkName_ = "App_Surface_Watermark_Test20";
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    auto pixelMap = DecodePixelMap("/data/local/tmp/Images/450x311.jpg");
    RSInterfaces::GetInstance().SetSurfaceWatermark(getpid(), watermarkName_, pixelMap, {surfaceNode->GetId()},
        SurfaceWatermarkType::CUSTOM_WATER_MARK);
    usleep(SLEEP_TIME_FOR_PROXY);
    RSInterfaces::GetInstance().ClearSurfaceWatermarkForNodes(getpid(), watermarkName_, surfaceNode->GetId());
};

/*
 * @tc.name: App_Surface_Watermark_Test21
 * @tc.desc: pixelmap is valid, CUSTOM_WATER_MARK, clear surface watermark, pid != surfaceNodePid show watermark,
 * but native is systemCalling
 * @tc.type FUN
*/
GRAPHIC_N_TEST(WatermarktTest, CONTENT_DISPLAY_TEST, App_Surface_Watermark_Test21)
{
    auto surfaceNode = CreateTestSurfaceNode();
    GetRootNode()->AddChild(surfaceNode);
    RegisterNode(surfaceNode);
    watermarkName_ = "App_Surface_Watermark_Test21";
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    auto pixelMap = DecodePixelMap("/data/local/tmp/Images/450x311.jpg");
    RSInterfaces::GetInstance().SetSurfaceWatermark(getpid(), watermarkName_, pixelMap, {surfaceNode->GetId()},
        SurfaceWatermarkType::CUSTOM_WATER_MARK);
    usleep(SLEEP_TIME_FOR_PROXY);
    RSInterfaces::GetInstance().ClearSurfaceWatermarkForNodes(4445522, watermarkName_, surfaceNode->GetId());
};

/*
 * @tc.name: App_Surface_Watermark_Test22
 * @tc.desc: pixelmap is valid, CUSTOM_WATER_MARK, but not set surfaceNode, not show watermark
 * @tc.type FUN
*/
GRAPHIC_N_TEST(WatermarktTest, CONTENT_DISPLAY_TEST, App_Surface_Watermark_Test22)
{
    auto surfaceNode = CreateTestSurfaceNode();
    GetRootNode()->AddChild(surfaceNode);
    RegisterNode(surfaceNode);
    watermarkName_ = "App_Surface_Watermark_Test22";
    usleep(SLEEP_TIME_FOR_PROXY);

    auto pixelMap = DecodePixelMap("/data/local/tmp/Images/450x311.jpg");
    RSInterfaces::GetInstance().SetSurfaceWatermark(getpid(), watermarkName_, pixelMap, {surfaceNode->GetId()},
        SurfaceWatermarkType::CUSTOM_WATER_MARK);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);
};

/*
 * @tc.name: App_Surface_Watermark_Test23
 * @tc.desc: pixelmap is valid, SYSTEM_WATER_MARK, surfaceNode not setWatermark, not show watermark
 * @tc.type FUN
*/
GRAPHIC_N_TEST(WatermarktTest, CONTENT_DISPLAY_TEST, App_Surface_Watermark_Test23)
{
    auto surfaceNode = CreateTestSurfaceNode();
    GetRootNode()->AddChild(surfaceNode);
    RegisterNode(surfaceNode);
    watermarkName_ = "App_Surface_Watermark_Test23";
    surfaceNode->SetWatermarkEnabled(watermarkName_, true);
    usleep(SLEEP_TIME_FOR_PROXY);

    auto pixelMap = DecodePixelMap("/data/local/tmp/Images/450x311.jpg");
    RSInterfaces::GetInstance().SetSurfaceWatermark(getpid(), watermarkName_, pixelMap, {surfaceNode->GetId()},
        SurfaceWatermarkType::SYSTEM_WATER_MARK);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);
};

/*
 * @tc.name: App_Surface_Watermark_Test24
 * @tc.desc: pixelmap is valid, CUSTOM_WATER_MARK, set canvas nodeId, not show watermark
 * @tc.type FUN
*/
GRAPHIC_N_TEST(WatermarktTest, CONTENT_DISPLAY_TEST, App_Surface_Watermark_Test24)
{
    auto surfaceNode = CreateTestSurfaceNode();
    GetRootNode()->AddChild(surfaceNode);
    RegisterNode(surfaceNode);
    watermarkName_ = "App_Surface_Watermark_Test24";
    auto canvasNode = RSCanvasNode::Create();
    canvasNode->SetBounds({0, 0, SCREEN_WIDTH, SCREEN_HEIGHT});
    canvasNode->SetFrame({0, 0, SCREEN_WIDTH, SCREEN_HEIGHT});
    surfaceNode->AddChild(canvasNode, -1);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    auto pixelMap = DecodePixelMap("/data/local/tmp/Images/450x311.jpg");
    RSInterfaces::GetInstance().SetSurfaceWatermark(getpid(), watermarkName_, pixelMap, {surfaceNode->GetId()},
        SurfaceWatermarkType::CUSTOM_WATER_MARK);
    usleep(SLEEP_TIME_FOR_PROXY);
};

/*
 * @tc.name: App_Surface_Watermark_Test25
 * @tc.desc: pixelmap is valid, SYSTEM_WATER_MARK, set canvas nodeId, show watermark
 * @tc.type FUN
*/
GRAPHIC_N_TEST(WatermarktTest, CONTENT_DISPLAY_TEST, App_Surface_Watermark_Test25)
{
    auto surfaceNode = CreateTestSurfaceNode();
    GetRootNode()->AddChild(surfaceNode);
    RegisterNode(surfaceNode);
    watermarkName_ = "App_Surface_Watermark_Test25";
    auto canvasNode = RSCanvasNode::Create();
    canvasNode->SetBounds({0, 0, SCREEN_WIDTH, SCREEN_HEIGHT});
    canvasNode->SetFrame({0, 0, SCREEN_WIDTH, SCREEN_HEIGHT});
    surfaceNode->AddChild(canvasNode, -1);
    surfaceNode->SetWatermarkEnabled(watermarkName_, true);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    auto pixelMap = DecodePixelMap("/data/local/tmp/Images/450x311.jpg");
    RSInterfaces::GetInstance().SetSurfaceWatermark(getpid(), watermarkName_, pixelMap, {},
        SurfaceWatermarkType::SYSTEM_WATER_MARK);
    usleep(SLEEP_TIME_FOR_PROXY);
};

/*
 * @tc.name: App_Surface_Watermark_Test26
 * @tc.desc: pixelMap is valid, SYSTEM_WATER_MARK, but exceeds the screen size, show watermark
 * @tc.type FUN
*/
GRAPHIC_N_TEST(WatermarktTest, CONTENT_DISPLAY_TEST, App_Surface_Watermark_Test26)
{
    auto surfaceNode = CreateTestSurfaceNode();
    GetRootNode()->AddChild(surfaceNode);
    RegisterNode(surfaceNode);
    watermarkName_ = "App_Surface_Watermark_Test26";
    surfaceNode->SetWatermarkEnabled(watermarkName_, true);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    auto pixelMap = DecodePixelMap("/data/local/tmp/Images/3200X2000.jpg");
    RSInterfaces::GetInstance().SetSurfaceWatermark(getpid(), watermarkName_, pixelMap, {surfaceNode->GetId()},
        SurfaceWatermarkType::SYSTEM_WATER_MARK);
    usleep(SLEEP_TIME_FOR_PROXY);
};

/*
 * @tc.name: App_Surface_Watermark_Test27
 * @tc.desc: pixelMap is valid, CUSTOM_WATER_MARK, exceeds the screen size, not show watermark
 * @tc.type FUN
*/
GRAPHIC_N_TEST(WatermarktTest, CONTENT_DISPLAY_TEST, App_Surface_Watermark_Test27)
{
    auto surfaceNode = CreateTestSurfaceNode();
    GetRootNode()->AddChild(surfaceNode);
    RegisterNode(surfaceNode);
    watermarkName_ = "App_Surface_Watermark_Test27";
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    auto pixelMap = DecodePixelMap("/data/local/tmp/Images/3200X2000.jpg");
    RSInterfaces::GetInstance().SetSurfaceWatermark(getpid(), watermarkName_, pixelMap, {surfaceNode->GetId()},
        SurfaceWatermarkType::CUSTOM_WATER_MARK);
    usleep(SLEEP_TIME_FOR_PROXY);
};

/*
 * @tc.name: App_Surface_Watermark_Test28
 * @tc.desc: pixelMap is valid, CUSTOM_WATER_MARK, has one surfaceNode invalid, show watermark
 * @tc.type FUN
*/
GRAPHIC_N_TEST(WatermarktTest, CONTENT_DISPLAY_TEST, App_Surface_Watermark_Test28)
{
    auto surfaceNode = CreateTestSurfaceNode();
    GetRootNode()->AddChild(surfaceNode);
    RegisterNode(surfaceNode);
    watermarkName_ = "App_Surface_Watermark_Test28";
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    auto surfaceNode2 = CreateTestSurfaceNode();
    surfaceNode2->SetWatermarkEnabled(watermarkName_, true);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    auto pixelMap = DecodePixelMap("/data/local/tmp/Images/3200X2000.jpg");
    RSInterfaces::GetInstance().SetSurfaceWatermark(getpid(), watermarkName_, pixelMap,
        {surfaceNode->GetId(), surfaceNode2->GetId()},
        SurfaceWatermarkType::CUSTOM_WATER_MARK);
    usleep(SLEEP_TIME_FOR_PROXY);
};

/*
 * @tc.name: App_Surface_Watermark_Test29
 * @tc.desc: pixelMap is valid, SYSTEM_WATER_MARK, has one surfaceNode invalid, show watermark
 * @tc.type FUN
*/
GRAPHIC_N_TEST(WatermarktTest, CONTENT_DISPLAY_TEST, App_Surface_Watermark_Test29)
{
    auto surfaceNode = CreateTestSurfaceNode();
    GetRootNode()->AddChild(surfaceNode);
    RegisterNode(surfaceNode);
    watermarkName_ = "App_Surface_Watermark_Test29";
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    auto surfaceNode2 = CreateTestSurfaceNode();
    surfaceNode2->SetWatermarkEnabled(watermarkName_, true);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    auto pixelMap = DecodePixelMap("/data/local/tmp/Images/3200X2000.jpg");
    RSInterfaces::GetInstance().SetSurfaceWatermark(getpid(), watermarkName_, pixelMap,
        {surfaceNode->GetId(), surfaceNode2->GetId()},
        SurfaceWatermarkType::SYSTEM_WATER_MARK);
    usleep(SLEEP_TIME_FOR_PROXY);
};

/*
 * @tc.name: App_Surface_Watermark_Test30
 * @tc.desc: test pixelMap is max 1000
 * @tc.type FUN
*/
GRAPHIC_N_TEST(WatermarktTest, CONTENT_DISPLAY_TEST, App_Surface_Watermark_Test30)
{
    auto surfaceNode = CreateTestSurfaceNode();
    GetRootNode()->AddChild(surfaceNode);
    RegisterNode(surfaceNode);
    watermarkName_ = "App_Surface_Watermark_Test30";
    surfaceNode->SetWatermarkEnabled(watermarkName_, true);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    auto pixelMap1 = DecodePixelMap("/data/local/tmp/Images/AllWhite.jpg");
    auto pixelMap2 = DecodePixelMap("/data/local/tmp/Images/AllBlack.jpg");
    std::vector<std::string> nameList;
    for (uint32_t i = 0; i < MAX_PIXELMAP_SIZE; i++) {
        std::string name("watermark" + std::to_string(i));
        nameList.push_back(name);
        RSInterfaces::GetInstance().SetSurfaceWatermark(getpid(), name, pixelMap1,
            {surfaceNode->GetId()}
            SurfaceWatermarkType::SYSTEM_WATER_MARK);
    }
    RSInterfaces::GetInstance().SetSurfaceWatermark(getpid(), name, pixelMap2,
        {surfaceNode->GetId()},
        SurfaceWatermarkType::SYSTEM_WATER_MARK);
    usleep(SLEEP_TIME_FOR_PROXY);
    for (auto name : nameList) {
        RSInterfaces::GetInstance().ClearSurfaceWatermark(getpid(), name);
    }
};

/*
 * @tc.name: App_Surface_Watermark_Test31
 * @tc.desc: test pixelMap is max 1000
 * @tc.type FUN
*/
GRAPHIC_N_TEST(WatermarktTest, CONTENT_DISPLAY_TEST, App_Surface_Watermark_Test31)
{
    auto surfaceNode = CreateTestSurfaceNode();
    GetRootNode()->AddChild(surfaceNode);
    RegisterNode(surfaceNode);
    watermarkName_ = "App_Surface_Watermark_Test31";
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    auto pixelMap1 = DecodePixelMap("/data/local/tmp/Images/AllWhite.jpg");
    auto pixelMap2 = DecodePixelMap("/data/local/tmp/Images/AllBlack.jpg");
    std::vector<std::string> nameList;
    for (uint32_t i = 0; i < MAX_PIXELMAP_SIZE; i++) {
        std::string name("watermark" + std::to_string(i));
        nameList.push_back(name);
        RSInterfaces::GetInstance().SetSurfaceWatermark(getpid(), name, pixelMap1,
            {surfaceNode->GetId()}
            SurfaceWatermarkType::CUSTOM_WATER_MARK);
    }
    RSInterfaces::GetInstance().SetSurfaceWatermark(getpid(), name, pixelMap2,
        {surfaceNode->GetId()},
        SurfaceWatermarkType::CUSTOM_WATER_MARK);
    usleep(SLEEP_TIME_FOR_PROXY);
    for (auto name : nameList) {
        RSInterfaces::GetInstance().ClearSurfaceWatermark(getpid(), name);
    }
};

/*
 * @tc.name: App_Surface_Watermark_Test32
 * @tc.desc: test pixelMap is valid, SYSTEM_WATER_MARK, update small watermark, show watermark
 * @tc.type FUN
*/
GRAPHIC_N_TEST(WatermarktTest, CONTENT_DISPLAY_TEST, App_Surface_Watermark_Test32)
{
    auto surfaceNode = CreateTestSurfaceNode();
    GetRootNode()->AddChild(surfaceNode);
    RegisterNode(surfaceNode);
    watermarkName_ = "App_Surface_Watermark_Test32";
    surfaceNode->SetWatermarkEnabled(watermarkName_, true);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    auto pixelMap1 = DecodePixelMap("/data/local/tmp/Images/AllWhite.jpg");
    auto pixelMap2 = DecodePixelMap("/data/local/tmp/Images/AllBlack.jpg");
    RSInterfaces::GetInstance().SetSurfaceWatermark(getpid(), watermarkName_, pixelMap1,
        {surfaceNode->GetId()},
        SurfaceWatermarkType::SYSTEM_WATER_MARK);
    usleep(SLEEP_TIME_FOR_PROXY);
    RSInterfaces::GetInstance().SetSurfaceWatermark(getpid(), watermarkName_, pixelMap2,
        {surfaceNode->GetId()},
        SurfaceWatermarkType::SYSTEM_WATER_MARK);
    usleep(SLEEP_TIME_FOR_PROXY);
};

/*
 * @tc.name: App_Surface_Watermark_Test35
 * @tc.desc: test pixelMap is valid, CUSTOM_WATER_MARK, update small watermark, show watermark
 * @tc.type FUN
*/
GRAPHIC_N_TEST(WatermarktTest, CONTENT_DISPLAY_TEST, App_Surface_Watermark_Test35)
{
    auto surfaceNode = CreateTestSurfaceNode();
    GetRootNode()->AddChild(surfaceNode);
    RegisterNode(surfaceNode);
    watermarkName_ = "App_Surface_Watermark_Test35";
    surfaceNode->SetWatermarkEnabled(watermarkName_, true);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    auto pixelMap1 = DecodePixelMap("/data/local/tmp/Images/AllWhite.jpg");
    auto pixelMap2 = DecodePixelMap("/data/local/tmp/Images/AllBlack.jpg");
    RSInterfaces::GetInstance().SetSurfaceWatermark(getpid(), watermarkName_, pixelMap1,
        {surfaceNode->GetId()},
        SurfaceWatermarkType::CUSTOM_WATER_MARK);
    usleep(SLEEP_TIME_FOR_PROXY);
    RSInterfaces::GetInstance().SetSurfaceWatermark(getpid(), watermarkName_, pixelMap2,
        {surfaceNode->GetId()},
        SurfaceWatermarkType::CUSTOM_WATER_MARK);
    usleep(SLEEP_TIME_FOR_PROXY);
};

/*
 * @tc.name: App_Surface_Watermark_Test36
 * @tc.desc: test pixelMap is valid, SYSTEM_WATER_MARK, update big watermark, show watermark
 * @tc.type FUN
*/
GRAPHIC_N_TEST(WatermarktTest, CONTENT_DISPLAY_TEST, App_Surface_Watermark_Test36)
{
    auto surfaceNode = CreateTestSurfaceNode();
    GetRootNode()->AddChild(surfaceNode);
    RegisterNode(surfaceNode);
    watermarkName_ = "App_Surface_Watermark_Test36";
    surfaceNode->SetWatermarkEnabled(watermarkName_, true);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    auto pixelMap1 = DecodePixelMap("/data/local/tmp/Images/AllWhite.jpg");
    auto pixelMap2 = DecodePixelMap("/data/local/tmp/Images/3200x2000.jpg");
    RSInterfaces::GetInstance().SetSurfaceWatermark(getpid(), watermarkName_, pixelMap1,
        {surfaceNode->GetId()},
        SurfaceWatermarkType::SYSTEM_WATER_MARK);
    usleep(SLEEP_TIME_FOR_PROXY);
    RSInterfaces::GetInstance().SetSurfaceWatermark(getpid(), watermarkName_, pixelMap2,
        {surfaceNode->GetId()},
        SurfaceWatermarkType::SYSTEM_WATER_MARK);
    usleep(SLEEP_TIME_FOR_PROXY);
};

/*
 * @tc.name: App_Surface_Watermark_Test36
 * @tc.desc: test pixelMap is valid, CUSTOM_WATER_MARK, update big watermark, not show watermark
 * @tc.type FUN
*/
GRAPHIC_N_TEST(WatermarktTest, CONTENT_DISPLAY_TEST, App_Surface_Watermark_Test36)
{
    auto surfaceNode = CreateTestSurfaceNode();
    GetRootNode()->AddChild(surfaceNode);
    RegisterNode(surfaceNode);
    watermarkName_ = "App_Surface_Watermark_Test36";
    surfaceNode->SetWatermarkEnabled(watermarkName_, true);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    auto pixelMap1 = DecodePixelMap("/data/local/tmp/Images/AllWhite.jpg");
    auto pixelMap2 = DecodePixelMap("/data/local/tmp/Images/3200x2000.jpg");
    RSInterfaces::GetInstance().SetSurfaceWatermark(getpid(), watermarkName_, pixelMap1,
        {surfaceNode->GetId()},
        SurfaceWatermarkType::CUSTOM_WATER_MARK);
    usleep(SLEEP_TIME_FOR_PROXY);
    RSInterfaces::GetInstance().SetSurfaceWatermark(getpid(), watermarkName_, pixelMap2,
        {surfaceNode->GetId()},
        SurfaceWatermarkType::CUSTOM_WATER_MARK);
    usleep(SLEEP_TIME_FOR_PROXY);
};

/*
 * @tc.name: App_Surface_Watermark_Test36
 * @tc.desc: test pixelMap is valid, CUSTOM_WATER_MARK, pixelMap is asctc, nodeList is empty, not show watermark
 * @tc.type FUN
*/
GRAPHIC_N_TEST(WatermarktTest, CONTENT_DISPLAY_TEST, App_Surface_Watermark_Test36)
{
    auto surfaceNode = CreateTestSurfaceNode();
    GetRootNode()->AddChild(surfaceNode);
    RegisterNode(surfaceNode);
    watermarkName_ = "App_Surface_Watermark_Test36";
    surfaceNode->SetWatermarkEnabled(watermarkName_, true);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);
    auto pixelMap = DecodePixelMap("/data/local/tmp/Images/AllWhite.jpg");

    RSInterfaces::GetInstance().SetSurfaceWatermark(getpid(), watermarkName_, pixelMap1, {},
        SurfaceWatermarkType::CUSTOM_WATER_MARK);
    usleep(SLEEP_TIME_FOR_PROXY);
};

}