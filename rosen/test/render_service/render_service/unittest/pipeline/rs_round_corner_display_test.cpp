/*
 * Copyright (c) 2023-2024 Huawei Device Co., Ltd.
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

#include <filesystem>
#include "gtest/gtest.h"
#include "common/rs_singleton.h"
#include "params/rs_rcd_render_params.h"
#include "feature/round_corner_display/rs_message_bus.h"
#include "feature/round_corner_display/rs_round_corner_display.h"
#include "feature/round_corner_display/rs_round_corner_display_manager.h"
#include "feature/round_corner_display/rs_round_corner_config.h"
#include "feature/round_corner_display/rs_rcd_surface_render_node.h"
#include "feature/round_corner_display/rs_rcd_render_listener.h"
#include "feature/round_corner_display/rs_rcd_render_visitor.h"
#include "feature/round_corner_display/rs_rcd_surface_render_node_drawable.h"
#include "feature/uifirst/rs_sub_thread_manager.h"
#include "pipeline/rs_display_render_node.h"
#include "surface_buffer_impl.h"
#include "rs_test_util.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSRoundCornerDisplayTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();

    void SetUp() override;
    void TearDown() override;
};

void RSRoundCornerDisplayTest::SetUpTestCase() {}
void RSRoundCornerDisplayTest::TearDownTestCase() {}
void RSRoundCornerDisplayTest::SetUp() {}
void RSRoundCornerDisplayTest::TearDown() {}

const std::vector<rs_rcd::RoundCornerLayer> RCD_LAYER_INFO_OK_CASE = {
    {
        "test1", // fileName
        1, // offsetX
        1, // offsetY
        "test1.bin", // binFileName
        10000, // bufferSize
        2, // cldWidth
        2, // cldHeight
        nullptr
    },
    {
        "TEST2", // fileName
        100, // offsetX
        200, // offsetY
        "TEST2.bin", // binFileName
        10, // bufferSize
        15, // cldWidth
        13, // cldHeight
        nullptr
    },
    {
        "test3", // fileName
        1024, // offsetX
        2048, // offsetY
        "test3.bin", // binFileName
        8000000, // bufferSize
        77, // cldWidth
        23, // cldHeight
        nullptr
    },
    {
        "TEST4", // fileName
        3069, // offsetX
        8096, // offsetY
        "TEST4.bin", // binFileName
        111111, // bufferSize
        1111111, // cldWidth
        1111111, // cldHeight
        nullptr
    },
    {
        "trdytfuygygi", // fileName
        -3069, // offsetX
        -8096, // offsetY
        "hglkjhgliuhgiug.bin", // binFileName
        -111111, // bufferSize
        -1111111, // cldWidth
        -1111111, // cldHeight
        nullptr
    }
};

const std::vector<std::array<std::string, 7>> RCD_LAYER_INFO_NG_CASE = {
    {
        "test1", // fileName
        "1s2", // offsetX
        "1", // offsetY
        "test1.bin", // binFileName
        "1s0000", // bufferSize
        "s2", // cldWidth
        "2d" // cldHeight
    },
    {
        "TEST2", // fileName
        "100sf", // offsetX
        "200gh", // offsetY
        "TEST2.bin", // binFileName
        "10gh", // bufferSize
        "15df", // cldWidth
        "13jk" // cldHeight
    },
    {
        "test3", // fileName
        "10hj24", // offsetX
        "2048", // offsetY
        "test3.bin", // binFileName
        "8000000", // bufferSize
        "7d7", // cldWidth
        "2g3" // cldHeight
    },
    {
        "TEST4", // fileName
        "30f69", // offsetX
        "80s96", // offsetY
        "TEST4.bin", // binFileName
        "1111r11,", // bufferSize
        "111e1111", // cldWidth
        "1111g111" // cldHeight
    },
    {
        "trdytfuygygi", // fileName
        "-a3069", // offsetX
        "-809s6", // offsetY
        "hglkjhgliuhgiug.bin", // binFileName
        "-111111,", // bufferSize
        "-1111d111", // cldWidth
        "-11h11111" // cldHeight
    }
};

struct XMLProperty {
    std::string name;
    std::string value;
};

xmlNodePtr CreateNodeWithProperty(const std::string& nodeName, const XMLProperty& property)
{
    auto xmlptr = xmlNewNode(NULL, BAD_CAST(nodeName.c_str()));
    xmlNewProp(xmlptr, BAD_CAST(property.name.c_str()), BAD_CAST(property.value.c_str()));
    return xmlptr;
}

xmlNodePtr CreateNodeWithProperties(const std::string& nodeName, const std::vector<XMLProperty>& properties)
{
    auto xmlptr = xmlNewNode(NULL, BAD_CAST(nodeName.c_str()));
    for (auto& property : properties) {
        xmlNewProp(xmlptr, BAD_CAST(property.name.c_str()), BAD_CAST(property.value.c_str()));
    }
    return xmlptr;
}

xmlNodePtr CreateRCDLayer(const std::string& nodeName, const rs_rcd::RoundCornerLayer& layer)
{
    std::vector<XMLProperty> properties = {
        {rs_rcd::ATTR_FILENAME, layer.fileName},
        {rs_rcd::ATTR_OFFSET_X, std::to_string(layer.offsetX)},
        {rs_rcd::ATTR_OFFSET_Y, std::to_string(layer.offsetY)},
        {rs_rcd::ATTR_BINFILENAME, layer.binFileName},
        {rs_rcd::ATTR_BUFFERSIZE, std::to_string(layer.bufferSize)},
        {rs_rcd::ATTR_CLDWIDTH, std::to_string(layer.cldWidth)},
        {rs_rcd::ATTR_CLDHEIGHT, std::to_string(layer.cldHeight)}
    };
    return CreateNodeWithProperties(nodeName, properties);
}

xmlNodePtr CreateRCDLayer(const std::string& nodeName, const std::array<std::string, 7>& names)
{
    std::vector<XMLProperty> properties = {
        {rs_rcd::ATTR_FILENAME, names[0]},
        {rs_rcd::ATTR_OFFSET_X, names[1]},
        {rs_rcd::ATTR_OFFSET_Y, names[2]},
        {rs_rcd::ATTR_BINFILENAME, names[3]},
        {rs_rcd::ATTR_BUFFERSIZE, names[4]},
        {rs_rcd::ATTR_CLDWIDTH, names[5]},
        {rs_rcd::ATTR_CLDHEIGHT, names[6]}
    };
    return CreateNodeWithProperties(nodeName, properties);
}

xmlNodePtr CreateRogPortrait(const std::string& nodeName, const rs_rcd::RogPortrait& rogPort)
{
    auto layerUpPtr = CreateRCDLayer(std::string(rs_rcd::NODE_LAYERUP), rogPort.layerUp);
    auto layerDownPtr = CreateRCDLayer(std::string(rs_rcd::NODE_LAYERDOWN), rogPort.layerDown);
    auto layerHidePtr = CreateRCDLayer(std::string(rs_rcd::NODE_LAYERHIDE), rogPort.layerHide);
    auto xmlptr = xmlNewNode(NULL, BAD_CAST(nodeName.c_str()));
    xmlAddChild(xmlptr, layerUpPtr);
    xmlAddChild(xmlptr, layerDownPtr);
    xmlAddChild(xmlptr, layerHidePtr);
    return xmlptr;
}

xmlNodePtr CreateRogPortrait(const std::string& nodeName, xmlNodePtr& layerUpPtr,
    xmlNodePtr& layerDownPtr, xmlNodePtr& layerHidePtr)
{
    auto xmlptr = xmlNewNode(NULL, BAD_CAST(nodeName.c_str()));
    xmlAddChild(xmlptr, layerUpPtr);
    xmlAddChild(xmlptr, layerDownPtr);
    xmlAddChild(xmlptr, layerHidePtr);
    return xmlptr;
}

xmlNodePtr CreateRogLandscape(const std::string& nodeName, const rs_rcd::RogLandscape& rogland)
{
    auto layerUpPtr = CreateRCDLayer(std::string(rs_rcd::NODE_LAYERUP), rogland.layerUp);
    auto xmlptr = xmlNewNode(NULL, BAD_CAST(nodeName.c_str()));
    xmlAddChild(xmlptr, layerUpPtr);
    return xmlptr;
}

xmlNodePtr CreateRogLandscape(const std::string& nodeName, xmlNodePtr& layerUpPtr)
{
    auto xmlptr = xmlNewNode(NULL, BAD_CAST(nodeName.c_str()));
    xmlAddChild(xmlptr, layerUpPtr);
    return xmlptr;
}

xmlNodePtr CreateSurfaceConfig(const std::string& nodeName, xmlNodePtr& topSurfacePtr, xmlNodePtr& bootomSurfacePtr)
{
    auto xmlptr = xmlNewNode(NULL, BAD_CAST(nodeName.c_str()));
    xmlAddChild(xmlptr, topSurfacePtr);
    xmlAddChild(xmlptr, bootomSurfacePtr);
    return xmlptr;
}

xmlNodePtr CreateSurfaceConfig(const std::string& nodeName, const rs_rcd::SurfaceConfig& surfaceCfg)
{
    auto topSupport = surfaceCfg.topSurface.support ? "true" : "false";
    auto bottomSuport = surfaceCfg.bottomSurface.support ? "true" : "false";
    std::vector<XMLProperty> topProperty = {
        {rs_rcd::ATTR_SUPPORT, std::string(topSupport)},
        {rs_rcd::ATTR_DISPLAYMODE, std::to_string(surfaceCfg.topSurface.mode)}
    };
    std::vector<XMLProperty> bottomProperty = {
        {rs_rcd::ATTR_SUPPORT, std::string(bottomSuport)},
        {rs_rcd::ATTR_DISPLAYMODE, std::to_string(surfaceCfg.bottomSurface.mode)}
    };
    auto topSurfacePtr = CreateNodeWithProperties(std::string(rs_rcd::NODE_TOPSURFACE), topProperty);
    auto bootomSurfacePtr = CreateNodeWithProperties(std::string(rs_rcd::NODE_BOTTOMSURFACE), bottomProperty);
    auto xmlptr = xmlNewNode(NULL, BAD_CAST(nodeName.c_str()));
    xmlAddChild(xmlptr, topSurfacePtr);
    xmlAddChild(xmlptr, bootomSurfacePtr);
    return xmlptr;
}

xmlNodePtr CreateSideRegionConfig(const std::string& nodeName, xmlNodePtr& sideSurfacePtr)
{
    auto xmlptr = xmlNewNode(NULL, BAD_CAST(nodeName.c_str()));
    xmlAddChild(xmlptr, sideSurfacePtr);
    return xmlptr;
}

xmlNodePtr CreateSideRegionConfig(const std::string& nodeName, const rs_rcd::SideRegionConfig& sideCfg)
{
    auto sideSupport = sideCfg.sideRegion.support ? "true" : "false";
    std::vector<XMLProperty> sideProperty = {
        {rs_rcd::ATTR_SUPPORT, std::string(sideSupport)},
        {rs_rcd::ATTR_DISPLAYMODE, std::to_string(sideCfg.sideRegion.mode)}
    };

    auto sideSurfacePtr = CreateNodeWithProperties(std::string(rs_rcd::NODE_SIDEREGION), sideProperty);
    auto xmlptr = xmlNewNode(NULL, BAD_CAST(nodeName.c_str()));
    xmlAddChild(xmlptr, sideSurfacePtr);
    return xmlptr;
}

std::shared_ptr<Drawing::Bitmap> LoadBitmapFromFile(const char* path)
{
    if (path == nullptr) {
        return nullptr;
    }

    std::shared_ptr<Drawing::Image> image;
    std::shared_ptr<Drawing::Bitmap> bitmap;
    auto& rcdInstance = RSSingleton<RoundCornerDisplay>::GetInstance();
    rcdInstance.Init();
    rcdInstance.LoadImg(path, image);
    if (image == nullptr) {
        std::cout << "LoadBitmapFromFile: current os no rcd source" << std::endl;
        return nullptr;
    }

    rcdInstance.DecodeBitmap(image, bitmap);
    return bitmap;
}

void InitRcdRenderParams(RSRenderParams* params)
{
    if (params == nullptr) {
        return;
    }
    auto rcdParams = static_cast<RSRcdRenderParams*>(params);
    rcdParams->SetPathBin("/sys_prod/etc/display/RoundCornerDisplay/test.bin");
    rcdParams->SetBufferSize(1);
    rcdParams->SetCldWidth(1);
    rcdParams->SetCldHeight(1);
    rcdParams->SetSrcRect(RectI(0, 0, 1, 1));
    rcdParams->SetDstRect(RectI(0, 0, 1, 1));
    rcdParams->SetRcdBitmap(std::make_shared<Drawing::Bitmap>());
    rcdParams->SetRcdEnabled(true);
    rcdParams->SetResourceChanged(false);
}

void InitRcdRenderParamsUnsync(RSRenderParams* params)
{
    if (params == nullptr) {
        return;
    }
    auto rcdParams = static_cast<RSRcdRenderParams*>(params);
    rcdParams->SetPathBin("invalid");
    rcdParams->SetBufferSize(-1);
    rcdParams->SetCldWidth(-1);
    rcdParams->SetCldHeight(-1);
    rcdParams->SetSrcRect(RectI(-1, -1, -1, -1));
    rcdParams->SetDstRect(RectI(-1, -1, -1, -1));
    rcdParams->SetRcdBitmap(nullptr);
    rcdParams->SetRcdEnabled(false);
    rcdParams->SetResourceChanged(true);
}

void InitRcdRenderParamsInvalid(RSRenderParams* params)
{
    if (params == nullptr) {
        return;
    }
    auto rcdParams = static_cast<RSRcdRenderParams*>(params);
    rcdParams->SetPathBin("/sys_prod/etc/display/RoundCornerDisplay/test.bin");
    rcdParams->SetBufferSize(0);
    rcdParams->SetCldWidth(-1);
    rcdParams->SetCldHeight(-1);
    rcdParams->SetSrcRect(RectI(0, 0, -1, -1));
    rcdParams->SetDstRect(RectI(0, 0, -1, -1));
    rcdParams->SetRcdBitmap(nullptr);
    rcdParams->SetRcdEnabled(true);
    rcdParams->SetResourceChanged(false);
}

void InitRcdCldLayerInfo(HardwareLayerInfo* layerInfo)
{
    if (layerInfo == nullptr) {
        return;
    }
    layerInfo->pathBin = "/sys_prod/etc/display/RoundCornerDisplay/test.bin";
    layerInfo->bufferSize = 1;
    layerInfo->cldWidth = 1;
    layerInfo->cldHeight = 1;
}

void InitRcdExtInfo(RcdExtInfo* extInfo, RCDSurfaceType type = RCDSurfaceType::BOTTOM)
{
    if (extInfo == nullptr) {
        return;
    }
    extInfo->surfaceCreated = false;
    extInfo->srcRect_ = RectI(0, 0, 1, 1);
    extInfo->dstRect_ = RectI(0, 0, 1, 1);
    extInfo->surfaceBounds = RectF(0, 0, 1, 1);
    extInfo->frameBounds = RectF(0, 0, 1, 1);
    extInfo->frameViewPort = RectF(0, 0, 1, 1);
    extInfo->surfaceType = type;
}

void InitRcdSourceInfo(RcdSourceInfo* sourceInfo)
{
    if (sourceInfo == nullptr) {
        return;
    }
    sourceInfo->bufferWidth = 1;
    sourceInfo->bufferHeight = 1;
    sourceInfo->bufferSize = 1;
}

/*
 * @tc.name: RCDLoadConfigFileTest
 * @tc.desc: Test RSRoundCornerDisplayTest.RCDLoadConfigFileTest
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, RCDLoadConfigFileTest, TestSize.Level1)
{
    auto& rcdInstance = RSSingleton<RoundCornerDisplay>::GetInstance();
    auto res = rcdInstance.LoadConfigFile();
    std::filesystem::path pathCheck(rs_rcd::PATH_CONFIG_FILE);
    if (std::filesystem::exists(pathCheck)) {
        EXPECT_TRUE(res == true);
    } else {
        EXPECT_TRUE(res == false);
    }
}

/*
 * @tc.name: RCDInitTest
 * @tc.desc: Test RSRoundCornerDisplayTest.RCDInitTest
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, RCDInitTest, TestSize.Level1)
{
    auto& rcdInstance = RSSingleton<RoundCornerDisplay>::GetInstance();
    auto res = rcdInstance.Init();
    EXPECT_TRUE(res == true);
}

/*
 * @tc.name: UpdateParameterTest
 * @tc.desc: Test RSRoundCornerDisplayTest.UpdateParameterTest
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, UpdateParameterTest, TestSize.Level1)
{
    auto& rcdInstance = RSSingleton<RoundCornerDisplay>::GetInstance();
    auto res = rcdInstance.Init();
    EXPECT_TRUE(res == true);

    ScreenRotation curOrientation = ScreenRotation::ROTATION_0;
    rcdInstance.UpdateOrientationStatus(curOrientation);

    uint32_t width = 1344;
    uint32_t height = 2772;
    rcdInstance.UpdateDisplayParameter(0, 0, width, height);

    std::map<std::string, bool> updateFlag = {
        {"display", true},
        {"notch", true},
        {"orientation", true}
    };
    rcdInstance.UpdateParameter(updateFlag);

    int notchStatus = WINDOW_NOTCH_DEFAULT;
    rcdInstance.UpdateNotchStatus(notchStatus);
    EXPECT_TRUE(rcdInstance.notchStatus_ == notchStatus);
}

/*
 * @tc.name: RSDrawRoundCornerTest
 * @tc.desc: Test RSRoundCornerDisplayTest.RSDrawRoundCornerTest
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, RSDrawRoundCornerTest, TestSize.Level1)
{
    auto& rcdInstance = RSSingleton<RoundCornerDisplay>::GetInstance();
    auto res = rcdInstance.Init();
    EXPECT_TRUE(res == true);

    ScreenRotation curOrientation = ScreenRotation::ROTATION_0;
    rcdInstance.UpdateOrientationStatus(curOrientation);

    int notchStatus = WINDOW_NOTCH_DEFAULT;
    rcdInstance.UpdateNotchStatus(notchStatus);
    EXPECT_TRUE(rcdInstance.notchStatus_ == notchStatus);

    uint32_t width = 1344;
    uint32_t height = 2772;
    rcdInstance.UpdateDisplayParameter(0, 0, width, height);

    std::unique_ptr<Drawing::Canvas> drawingCanvas = std::make_unique<Drawing::Canvas>(width, height);
    std::shared_ptr<RSPaintFilterCanvas> canvas = std::make_shared<RSPaintFilterCanvas>(drawingCanvas.get());
    ASSERT_NE(canvas, nullptr);
    rcdInstance.DrawTopRoundCorner(canvas.get());
    rcdInstance.DrawBottomRoundCorner(canvas.get());
}

/*
 * @tc.name: RSLoadImgTest
 * @tc.desc: Test RSRoundCornerDisplayTest.RSLoadImgTest
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, RSLoadImgTest, TestSize.Level1)
{
    std::shared_ptr<Drawing::Image> imgBottomPortrait;
    std::shared_ptr<Drawing::Bitmap> bitmapBottomPortrait;
    const char* path = "port_down.png";

    auto& rcdInstance = RSSingleton<RoundCornerDisplay>::GetInstance();
    auto res = rcdInstance.Init();

    rcdInstance.LoadImg(path, imgBottomPortrait);
    if (imgBottomPortrait == nullptr) {
        std::cout << "RSRoundCornerDisplayTest: current os less rcd source" << std::endl;
        EXPECT_TRUE(res == true);
        return;
    }
    rcdInstance.DecodeBitmap(imgBottomPortrait, bitmapBottomPortrait);

    std::shared_ptr<Drawing::Image> imgNoneImageLoaded = nullptr;
    res = rcdInstance.DecodeBitmap(imgNoneImageLoaded, bitmapBottomPortrait);
    EXPECT_TRUE(res == false);
}

/*
 * @tc.name: RSLoadImgTest001
 * @tc.desc: Test RSRoundCornerDisplayTest.RSLoadImgTest001
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, RSLoadImgTest001, TestSize.Level1)
{
    std::shared_ptr<Drawing::Image> imgBottomPortrait;
    std::shared_ptr<Drawing::Bitmap> bitmapBottomPortrait;
    auto& rcdInstance = RSSingleton<RoundCornerDisplay>::GetInstance();
    rcdInstance.Init();

    // test waring path
    const char* path1 = "test_waring_path.png";
    bool flag1 = rcdInstance.LoadImg(path1, imgBottomPortrait);
    EXPECT_TRUE(flag1 == false);

    // test image is nullpr
    bool flag2 = rcdInstance.DecodeBitmap(nullptr, bitmapBottomPortrait);
    EXPECT_TRUE(flag2 == false);

    // test correct path, but the file type is incorrect.
    const char* path2 = "config.xml";
    bool flag3 = rcdInstance.LoadImg(path2, imgBottomPortrait);
    EXPECT_TRUE(flag3 == false);
}

/*
 * @tc.name: RSGetSurfaceSourceTest
 * @tc.desc: Test RSRoundCornerDisplayTest.RSGetSurfaceSourceTest
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, RSGetSurfaceSourceTest, TestSize.Level1)
{
    auto& rcdInstance = RSSingleton<RoundCornerDisplay>::GetInstance();
    rcdInstance.InitOnce();
    auto res = rcdInstance.Init();
    rcdInstance.InitOnce();
    EXPECT_TRUE(res == true);

    uint32_t width = 1344;
    uint32_t height = 2772;
    rcdInstance.UpdateDisplayParameter(0, 0, width, height);

    rcdInstance.GetTopSurfaceSource();
    rcdInstance.GetBottomSurfaceSource();
}

/*
 * @tc.name: RSChooseResourceTest
 * @tc.desc: Test RSRoundCornerDisplayTest.RSChooseResourceTest
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, RSChooseResourceTest, TestSize.Level1)
{
    auto& rcdInstance = RSSingleton<RoundCornerDisplay>::GetInstance();
    rcdInstance.Init();

    ScreenRotation curOrientation = ScreenRotation::ROTATION_90;
    rcdInstance.UpdateOrientationStatus(curOrientation);

    int notchStatus = WINDOW_NOTCH_HIDDEN;
    rcdInstance.UpdateNotchStatus(notchStatus);
    EXPECT_TRUE(rcdInstance.notchStatus_ == notchStatus);

    uint32_t width = 1344;
    uint32_t height = 2772;
    rcdInstance.UpdateDisplayParameter(0, 0, width, height);

    rcdInstance.RcdChooseTopResourceType();

    rcdInstance.RcdChooseRSResource();
    rcdInstance.RcdChooseHardwareResource();
}

/*
 * @tc.name: IsNotchNeedUpdate
 * @tc.desc: Test RSRoundCornerDisplayTest.IsNotchNeedUpdate
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, IsNotchNeedUpdate, TestSize.Level1)
{
    auto& rcdInstance = RSSingleton<RoundCornerDisplay>::GetInstance();
    rcdInstance.Init();
    rcdInstance.IsNotchNeedUpdate(true);
    bool ischange = rcdInstance.IsNotchNeedUpdate(false);
    EXPECT_EQ(true, ischange);
}

/*
 * @tc.name: RunHardwareTask
 * @tc.desc: Test RSRoundCornerDisplayTest.RunHardwareTask
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, RunHardwareTask, TestSize.Level1)
{
    auto& rcdInstance = RSSingleton<RoundCornerDisplay>::GetInstance();
    bool res = rcdInstance.Init();
    rcdInstance.RunHardwareTask(
        []() {
            std::cout << "do RSRoundCornerDisplayTest.RunHardwareTask1" << std::endl;
        }
    );
    rcdInstance.RunHardwareTask(
        []() {
            std::cout << "do RSRoundCornerDisplayTest.RunHardwareTask2" << std::endl;
        }
    );
    EXPECT_EQ(true, res);
}

/*
 * @tc.name: DrawRoundCorner
 * @tc.desc: Test RSRoundCornerDisplayTest.DrawRoundCorner
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, DrawRoundCorner, TestSize.Level1)
{
    auto& rcdInstance = RSSingleton<RoundCornerDisplay>::GetInstance();
    bool res = rcdInstance.Init();
    rcdInstance.DrawTopRoundCorner(nullptr);
    rcdInstance.DrawBottomRoundCorner(nullptr);

    rcdInstance.DrawTopRoundCorner(nullptr);
    rcdInstance.DrawBottomRoundCorner(nullptr);
    EXPECT_EQ(true, res);
}

rs_rcd::ROGSetting* GetRogFromLcdModel(rs_rcd::LCDModel* lcdModel, int& width, int& height)
{
    if (lcdModel == nullptr) {
        return nullptr;
    }
    int widthMate40 = 1344;
    int heightMate40 = 2772;
    int widthMate60 = 1260;
    int heightMate60 = 2720;
    width = widthMate40;
    height = heightMate40;
    rs_rcd::ROGSetting* rog = lcdModel->GetRog(width, height);
    if (rog == nullptr) {
        rog = lcdModel->GetRog(widthMate60, heightMate60);
        width = widthMate60;
        height = heightMate60;
    }
    return rog;
}

template<typename T1, typename T2, typename T3>
struct TestMsgBus {
    T1 mA;
    T2 mB;
    T3 mC;
    TestMsgBus(T1 a, T2 b, T3 c) : mA{a}, mB{b}, mC{c} {};

    void TestFunc1(T1 a)
    {
        std::cout << "TestMsg Bus Func1:" << sizeof(T1) << "," << &a << std::endl;
        EXPECT_TRUE(mA == a);
    }

    void TestFunc2(T1 a, T2 b)
    {
        std::cout << "TestMsg Bus Func2:" <<
            sizeof(T1) << "," << &a << std::endl <<
            sizeof(T2) << "," << &b << std::endl;
        EXPECT_TRUE(mA == a);
        EXPECT_TRUE(mB == b);
    }

    void TestFunc3(T1 a, T2 b, T3 c)
    {
        std::cout << "TestMsg Bus Func3:" <<
            sizeof(T1) << "," << &a << std::endl <<
            sizeof(T2) << "," << &b << std::endl <<
            sizeof(T3) << "," << &c << std::endl;
        EXPECT_TRUE(mA == a);
        EXPECT_TRUE(mB == b);
        EXPECT_TRUE(mC == c);
    }
};

template<typename T1, typename T2, typename T3>
void TestMsgBusFunc()
{
    std::string topic = "TEST_TOPIC";
    auto& msgBus = RSSingleton<RsMessageBus>::GetInstance();
    using TestMsgBusClass = TestMsgBus<T1, T2, T3>;
    TestMsgBusClass* objPtr = nullptr;
    int num1 = 1;
    int num2 = 2;
    int num3 = 3;
    TestMsgBusClass obj(static_cast<T1>(num1), static_cast<T2>(num2), static_cast<T3>(num3));
    msgBus.RegisterTopic<T1>(topic, objPtr, &TestMsgBusClass::TestFunc1);
    msgBus.RegisterTopic<T1>(topic, &obj, &TestMsgBusClass::TestFunc1);
    EXPECT_TRUE(msgBus.m_map.size() == 1);
    msgBus.SendMsg(topic, static_cast<T1>(num1));
    msgBus.RemoveTopic<T1>(topic);
    EXPECT_TRUE(msgBus.m_map.size() == 0);
    msgBus.RegisterTopic<T1, T2>(topic, &obj, &TestMsgBusClass::TestFunc2);
    EXPECT_TRUE(msgBus.m_map.size() == 1);
    msgBus.SendMsg(topic, static_cast<T1>(num1), static_cast<T2>(num2));
    msgBus.RemoveTopic<T1, T2>(topic);
    EXPECT_TRUE(msgBus.m_map.size() == 0);
    msgBus.RegisterTopic<T1, T2, T3>(topic, &obj, &TestMsgBusClass::TestFunc3);
    EXPECT_TRUE(msgBus.m_map.size() == 1);
    msgBus.SendMsg(topic, static_cast<T1>(num1), static_cast<T2>(num2), static_cast<T3>(num3));
    msgBus.RemoveTopic<T1, T2, T3>(topic);
    EXPECT_TRUE(msgBus.m_map.size() == 0);
}

template<typename T1, typename T2>
void TestMsgBusFunc3()
{
    TestMsgBusFunc<T1, T2, uint8_t>();
    TestMsgBusFunc<T1, T2, uint16_t>();
    TestMsgBusFunc<T1, T2, uint32_t>();
    TestMsgBusFunc<T1, T2, uint64_t>();
    TestMsgBusFunc<T1, T2, int>();
    TestMsgBusFunc<T1, T2, float>();
    TestMsgBusFunc<T1, T2, double>();
}

template<typename T1>
void TestMsgBusFunc2()
{
    TestMsgBusFunc3<T1, uint8_t>();
    TestMsgBusFunc3<T1, uint16_t>();
    TestMsgBusFunc3<T1, uint32_t>();
    TestMsgBusFunc3<T1, uint64_t>();
    TestMsgBusFunc3<T1, int>();
    TestMsgBusFunc3<T1, float>();
    TestMsgBusFunc3<T1, double>();
}

/*
 * @tc.name: MessageBus
 * @tc.desc: Test RSRoundCornerDisplayTest.MessageBus
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, MessageBus, TestSize.Level1)
{
    auto& msgBus = RSSingleton<RsMessageBus>::GetInstance();
    msgBus.RemoveTopic("NG_TOPIC");
    TestMsgBusFunc2<uint8_t>();
    TestMsgBusFunc2<uint16_t>();
    TestMsgBusFunc2<uint32_t>();
    TestMsgBusFunc2<uint64_t>();
    TestMsgBusFunc2<int>();
    TestMsgBusFunc2<float>();
    TestMsgBusFunc2<double>();
    EXPECT_TRUE(msgBus.m_map.size() == 0);
}

/*
 * @tc.name: RCDConfig
 * @tc.desc: Test RSRoundCornerDisplayTest.RCDConfig
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, RCDConfig, TestSize.Level1)
{
    auto& rcdCfg = RSSingleton<rs_rcd::RCDConfig>::GetInstance();
    rcdCfg.Load(std::string("NG_PATH_CONFIG_FILE"));
    rcdCfg.Load(std::string(rs_rcd::PATH_CONFIG_FILE));
    auto invalidLcd = rcdCfg.GetLcdModel(std::string(""));
    EXPECT_EQ(invalidLcd, nullptr);
    rs_rcd::LCDModel* nullLcdModel = nullptr;
    rcdCfg.lcdModels.push_back(nullLcdModel);
    invalidLcd = rcdCfg.GetLcdModel(std::string("invalideName"));
    EXPECT_EQ(invalidLcd, nullptr);
    rs_rcd::RCDConfig::PrintParseRog(nullptr);
    rs_rcd::ROGSetting rog;
    rs_rcd::RogPortrait rp;
    rs_rcd::RogLandscape rl;
    rog.portraitMap["a"] = rp;
    rog.landscapeMap["b"] = rl;
    rs_rcd::RCDConfig::PrintParseRog(&rog);
}

/*
 * @tc.name: LCDModel
 * @tc.desc: Test RSRoundCornerDisplayTest.LCDModel
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, LCDModel, TestSize.Level1)
{
    auto& rcdCfg = RSSingleton<rs_rcd::RCDConfig>::GetInstance();
    rcdCfg.Load(std::string(rs_rcd::PATH_CONFIG_FILE));
    auto defaultLcd = rcdCfg.GetLcdModel(std::string(rs_rcd::ATTR_DEFAULT));
    if (defaultLcd == nullptr) {
        std::cout << "OS less lcdModel resource" << std::endl;
        return;
    }
    rs_rcd::ROGSetting* nullRog = nullptr;
    defaultLcd->rogs.push_back(nullRog);
    defaultLcd->GetRog(0, 0);
    defaultLcd->GetSideRegionConfig();
    defaultLcd->GetSurfaceConfig();

    xmlNodePtr xmlptr = nullptr;
    auto res = defaultLcd->ReadXmlNode(xmlptr);
    EXPECT_EQ(res, false);
}

/*
 * @tc.name: HardwareComposerConfig
 * @tc.desc: Test RSRoundCornerDisplayTest.HardwareComposerConfig
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, HardwareComposerConfig, TestSize.Level1)
{
    rs_rcd::HardwareComposerConfig cfg;
    xmlNodePtr xmlptr = nullptr;
    bool res = cfg.ReadXmlNode(xmlptr);
    EXPECT_EQ(res, false);
    xmlptr = xmlNewNode(NULL, BAD_CAST(rs_rcd::NODE_HARDWARECOMPOSERCONFIG));
    auto child = xmlNewNode(NULL, BAD_CAST(rs_rcd::NODE_HARDWARECOMPOSER));
    xmlNewProp(child, BAD_CAST(rs_rcd::ATTR_SUPPORT), BAD_CAST("true"));
    xmlAddChild(xmlptr, child);
    res = cfg.ReadXmlNode(xmlptr);
    EXPECT_EQ(res, true);

    if (xmlptr != nullptr) {
        xmlFreeNode(xmlptr);
        xmlptr = nullptr;
    }
    auto ngPtr = CreateNodeWithProperty(std::string("node"), XMLProperty{std::string("a1"), std::string("")});
    rs_rcd::SupportConfig sc = {true, 1};
    EXPECT_EQ(sc.ReadXmlNode(ngPtr, std::string("aa"), std::string("mm")), false);
    if (ngPtr != nullptr) {
        xmlFreeNode(ngPtr);
        ngPtr = nullptr;
    }
}

/*
 * @tc.name: HardwareComposer
 * @tc.desc: Test RSRoundCornerDisplayTest.HardwareComposer
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, HardwareComposer, TestSize.Level1)
{
    rs_rcd::HardwareComposer cfg;
    xmlNodePtr xmlptr = nullptr;
    bool res = cfg.ReadXmlNode(xmlptr, "ngAttr");
    EXPECT_EQ(res, false);
}

/*
 * @tc.name: SideRegionConfig
 * @tc.desc: Test RSRoundCornerDisplayTest.SideRegionConfig
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, SideRegionConfig, TestSize.Level1)
{
    rs_rcd::SideRegionConfig cfg;
    xmlNodePtr xmlptr = nullptr;
    bool res = cfg.ReadXmlNode(xmlptr);
    EXPECT_EQ(res, false);

    cfg.sideRegion.support = true;
    xmlptr = CreateSideRegionConfig("TestName", cfg);
    res = cfg.ReadXmlNode(xmlptr);
    EXPECT_EQ(res, true);
    xmlFreeNode(xmlptr);

    xmlNodePtr sideSurfacePtr = nullptr;
    xmlptr = CreateSideRegionConfig("TestName", sideSurfacePtr);
    res = cfg.ReadXmlNode(xmlptr);
    EXPECT_EQ(res, false);
    xmlFreeNode(xmlptr);
}

/*
 * @tc.name: SurfaceConfig
 * @tc.desc: Test RSRoundCornerDisplayTest.SurfaceConfig
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, SurfaceConfig, TestSize.Level1)
{
    rs_rcd::SurfaceConfig cfg;
    xmlNodePtr xmlptr = nullptr;
    bool res = cfg.ReadXmlNode(xmlptr);
    EXPECT_EQ(res, false);

    cfg.topSurface.support = true;
    xmlptr = CreateSurfaceConfig("TestName", cfg);
    res = cfg.ReadXmlNode(xmlptr);
    EXPECT_EQ(res, true);
    xmlFreeNode(xmlptr);

    xmlNodePtr topSurfacePtr = nullptr;
    xmlNodePtr bootomSurfacePtr = nullptr;
    xmlptr = CreateSurfaceConfig("TestName", topSurfacePtr, bootomSurfacePtr);
    res = cfg.ReadXmlNode(xmlptr);
    EXPECT_EQ(res, false);
    xmlFreeNode(xmlptr);
    std::vector<XMLProperty> topProperty = {
        {rs_rcd::ATTR_SUPPORT, std::string("true")},
        {rs_rcd::ATTR_DISPLAYMODE, std::to_string(1)}
    };
    topSurfacePtr = CreateNodeWithProperties(std::string(rs_rcd::NODE_TOPSURFACE), topProperty);
    xmlptr = CreateSurfaceConfig("TestName", topSurfacePtr, bootomSurfacePtr);
    res = cfg.ReadXmlNode(xmlptr);
    EXPECT_EQ(res, false);
    xmlFreeNode(xmlptr);
}

/*
 * @tc.name: ROGSetting
 * @tc.desc: Test RSRoundCornerDisplayTest.ROGSetting
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, ROGSetting, TestSize.Level1)
{
    rs_rcd::ROGSetting cfg;
    xmlNodePtr xmlptr = nullptr;
    bool res = cfg.ReadXmlNode(xmlptr);
    EXPECT_EQ(res, false);
}

/*
 * @tc.name: RogLandscape
 * @tc.desc: Test RSRoundCornerDisplayTest.RogLandscape
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, RogLandscape, TestSize.Level1)
{
    rs_rcd::RogLandscape cfg;
    xmlNodePtr xmlptr = nullptr;
    bool res = cfg.ReadXmlNode(xmlptr);
    EXPECT_EQ(res, false);

    cfg.layerUp.offsetX = 100;
    xmlptr = CreateRogLandscape("TestName", cfg);
    res = cfg.ReadXmlNode(xmlptr);
    EXPECT_EQ(res, true);
    xmlFreeNode(xmlptr);

    xmlNodePtr layerHidePtr = nullptr;
    xmlptr = CreateRogLandscape("TestName", layerHidePtr);
    res = cfg.ReadXmlNode(xmlptr);
    EXPECT_EQ(res, false);
    xmlFreeNode(xmlptr);
}

/*
 * @tc.name: RogPortrait
 * @tc.desc: Test RSRoundCornerDisplayTest.RogPortrait
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, RogPortrait, TestSize.Level1)
{
    rs_rcd::RogPortrait cfg;
    xmlNodePtr xmlptr = nullptr;
    bool res = cfg.ReadXmlNode(xmlptr);
    EXPECT_EQ(res, false);
    cfg.layerUp.offsetX = 100;
    xmlptr = CreateRogPortrait("TestName", cfg);
    res = cfg.ReadXmlNode(xmlptr);
    EXPECT_EQ(res, true);
    xmlFreeNode(xmlptr);

    auto layerUpPtr = CreateRCDLayer(std::string(rs_rcd::NODE_LAYERUP), cfg.layerUp);
    auto layerDownPtr = CreateRCDLayer(std::string(rs_rcd::NODE_LAYERDOWN), cfg.layerDown);
    xmlNodePtr layerHidePtr = nullptr;
    xmlptr = CreateRogPortrait("TestName", layerUpPtr, layerDownPtr, layerHidePtr);
    res = cfg.ReadXmlNode(xmlptr);
    EXPECT_EQ(res, false);
    xmlFreeNode(xmlptr);

    layerUpPtr = CreateRCDLayer(std::string(rs_rcd::NODE_LAYERUP), cfg.layerUp);
    layerDownPtr = nullptr;
    xmlptr = CreateRogPortrait("TestName", layerUpPtr, layerDownPtr, layerHidePtr);
    res = cfg.ReadXmlNode(xmlptr);
    EXPECT_EQ(res, false);
    xmlFreeNode(xmlptr);

    layerUpPtr = nullptr;
    xmlptr = CreateRogPortrait("TestName", layerUpPtr, layerDownPtr, layerHidePtr);
    res = cfg.ReadXmlNode(xmlptr);
    EXPECT_EQ(res, false);
    xmlFreeNode(xmlptr);
}

/*
 * @tc.name: RoundCornerLayer
 * @tc.desc: Test RSRoundCornerDisplayTest.RoundCornerLayer
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, RoundCornerLayer, TestSize.Level1)
{
    rs_rcd::RoundCornerLayer cfg;
    xmlNodePtr xmlptr = nullptr;
    cfg.ReadXmlNode(xmlptr, {"a", "b"});

    std::vector<std::string> properties = {rs_rcd::ATTR_FILENAME, rs_rcd::ATTR_OFFSET_X,
        rs_rcd::ATTR_OFFSET_Y, rs_rcd::ATTR_BINFILENAME, rs_rcd::ATTR_BUFFERSIZE,
        rs_rcd::ATTR_CLDWIDTH, rs_rcd::ATTR_CLDHEIGHT};

    rs_rcd::RoundCornerLayer cfgData = {
        "test", // fileName
        1, // offsetX
        1, // offsetY
        "test.bin", // binFileName
        10000, // bufferSize
        2, // cldWidth
        2, // cldHeight
        nullptr};
    auto nodePtr = CreateRCDLayer(std::string("layer"), cfgData);
    cfg.ReadXmlNode(nodePtr, properties);

    EXPECT_EQ(cfg.fileName.compare(cfgData.fileName), int{0});
    EXPECT_EQ(cfg.binFileName.compare(cfgData.binFileName), int{0});
    EXPECT_EQ(cfg.bufferSize, cfgData.bufferSize);
    EXPECT_EQ(cfg.offsetX, cfgData.offsetX);
    EXPECT_EQ(cfg.offsetY, cfgData.offsetY);
    EXPECT_EQ(cfg.cldWidth, cfgData.cldWidth);
    EXPECT_EQ(cfg.cldHeight, cfgData.cldHeight);
    xmlFreeNode(nodePtr);
    nodePtr = nullptr;

    for (auto okCase : RCD_LAYER_INFO_OK_CASE) {
        auto nodeOK_Ptr = CreateRCDLayer(std::string("layer"), okCase);
        cfg.ReadXmlNode(nodeOK_Ptr, properties);

        EXPECT_EQ(cfg.fileName.compare(okCase.fileName), int{0});
        EXPECT_EQ(cfg.binFileName.compare(okCase.binFileName), int{0});
        EXPECT_EQ(cfg.bufferSize, okCase.bufferSize);
        EXPECT_EQ(cfg.offsetX, okCase.offsetX);
        EXPECT_EQ(cfg.offsetY, okCase.offsetY);
        EXPECT_EQ(cfg.cldWidth, okCase.cldWidth);
        EXPECT_EQ(cfg.cldHeight, okCase.cldHeight);
        xmlFreeNode(nodeOK_Ptr);
        nodeOK_Ptr = nullptr;
    }

    for (auto ngCase : RCD_LAYER_INFO_NG_CASE) {

        auto nodeNG_Ptr = CreateRCDLayer(std::string("layer"), ngCase);
        auto res = cfg.ReadXmlNode(nodeNG_Ptr, properties);
        EXPECT_EQ(res, true);
        xmlFreeNode(nodeNG_Ptr);
        nodeNG_Ptr = nullptr;
    }
}

/*
 * @tc.name: XMLReader
 * @tc.desc: Test RSRoundCornerDisplayTest.XMLReader
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, XMLReader, TestSize.Level1)
{
    xmlNodePtr nodePtr = nullptr;
    rs_rcd::XMLReader::ReadAttrStr(nodePtr, std::string("a"));
    rs_rcd::XMLReader::ReadAttrInt(nodePtr, std::string("a"));
    rs_rcd::XMLReader::ReadAttrFloat(nodePtr, std::string("a"));
    rs_rcd::XMLReader::ReadAttrBool(nodePtr, std::string("a"));

    auto attrName = std::string("num");
    auto numPtr = CreateNodeWithProperty(std::string("nodeName"), XMLProperty{attrName, std::string("")});
    EXPECT_TRUE(rs_rcd::XMLReader::ReadAttrInt(numPtr, attrName) == int{0});
    EXPECT_TRUE(rs_rcd::XMLReader::ReadAttrFloat(numPtr, attrName) < 1.0f);
    xmlFreeNode(numPtr);
    numPtr = CreateNodeWithProperty(std::string("nodeName"), XMLProperty{attrName, std::string("2.0")});
    EXPECT_TRUE(rs_rcd::XMLReader::ReadAttrInt(numPtr, attrName) == int{2});
    EXPECT_TRUE(rs_rcd::XMLReader::ReadAttrFloat(numPtr, attrName) > 1.0f);
    xmlFreeNode(numPtr);
    std::vector<std::string> okCase = {
        "0.0",
        "0",
        "123",
        "1230.0",
        "8192.0 ",
        "100.0",
        "101010",
        "321",
        "121212",
        "321321 ",
        "987654",
        "56789"};
    for (auto& tmpCase : okCase) {
        bool isOk = rs_rcd::XMLReader::RegexMatchNum(tmpCase);
        EXPECT_EQ(isOk, true);
    }

    std::vector<std::string> ngCase = {
        "a0.0",
        "0a",
        "a123",
        "1230.0c",
        "op0.0",
        "0afdsd",
        "yhgfjhgfj",
        "12g30.0c",
        "4t7192.0 "};
    for (auto& tmpCase : ngCase) {
        bool isOk = rs_rcd::XMLReader::RegexMatchNum(tmpCase);
        EXPECT_EQ(isOk, false);
    }
}

/*
 * @tc.name: RcdExtInfo
 * @tc.desc: Test RSRoundCornerDisplayTest.RcdExtInfo
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, RcdExtInfo, TestSize.Level1)
{
    RcdExtInfo info;
    info.Clear();
    bool res = info.GetFrameOffsetX() > -1;
    EXPECT_EQ(res, true);
    res = info.GetFrameOffsetY() > -1;
    EXPECT_EQ(res, true);
}

/*
 * @tc.name: RSRcdSurfaceRenderNode
 * @tc.desc: Test RSRoundCornerDisplayTest.RSRcdSurfaceRenderNode
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, RSRcdSurfaceRenderNode, TestSize.Level1)
{
    for (int i = 0; i < 2; i++)
    {
        auto rcdRenderNode = RSRcdSurfaceRenderNode::Create(0, static_cast<RCDSurfaceType>(i));
        rcdRenderNode->ResetCurrFrameState();
        rcdRenderNode->Reset();

        rcdRenderNode->GetSrcRect();
        rcdRenderNode->GetDstRect();
        rcdRenderNode->IsBottomSurface();
        rcdRenderNode->IsTopSurface();
        rcdRenderNode->IsInvalidSurface();
        rcdRenderNode->SetRcdBufferSize(0);
        rcdRenderNode->SetRcdBufferHeight(0);
        rcdRenderNode->SetRcdBufferWidth(0);
        rcdRenderNode->PrepareHardwareResourceBuffer(nullptr);
        rs_rcd::RoundCornerLayer layer;
        rcdRenderNode->PrepareHardwareResourceBuffer(std::make_shared<rs_rcd::RoundCornerLayer>(layer));
        uint32_t size = 10;
        rcdRenderNode->SetRcdBufferSize(size);
        auto bufferSize = rcdRenderNode->GetRcdBufferSize();
        EXPECT_EQ(bufferSize, size);

        uint32_t height = 20;
        rcdRenderNode->SetRcdBufferHeight(height);
        auto bufferHeight = rcdRenderNode->GetRcdBufferHeight();
        EXPECT_EQ(bufferHeight, height);

        uint32_t width = 100;
        rcdRenderNode->SetRcdBufferWidth(width);
        auto bufferWidth = rcdRenderNode->GetRcdBufferWidth();
        EXPECT_EQ(bufferWidth, width);
        rcdRenderNode->PrepareHardwareResourceBuffer(nullptr);
        rcdRenderNode->PrepareHardwareResourceBuffer(std::make_shared<rs_rcd::RoundCornerLayer>(layer));
        width = 0;
        rcdRenderNode->SetRcdBufferWidth(width);
        EXPECT_EQ(rcdRenderNode->GetRcdBufferWidth(), width);
    }
}

/*
 * @tc.name: RSRoundCornerDisplayResource
 * @tc.desc: Test RSRoundCornerDisplayTest.RSRoundCornerDisplayResource
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, RSRoundCornerDisplayResource, TestSize.Level1)
{
    auto& rcdInstance = RSSingleton<RoundCornerDisplay>::GetInstance();
    rcdInstance.lcdModel_ = nullptr;
    EXPECT_TRUE(rcdInstance.LoadImgsbyResolution(0, 0) == false);

    rcdInstance.hardInfo_.topLayer = std::make_shared<rs_rcd::RoundCornerLayer>();
    rcdInstance.hardInfo_.bottomLayer = std::make_shared<rs_rcd::RoundCornerLayer>();
    rcdInstance.displayRect_ = RectU(0, 0, 640, 480);
    EXPECT_TRUE(rcdInstance.SetHardwareLayerSize());
    rcdInstance.UpdateDisplayParameter(0, 0, rcdInstance.displayRect_.GetWidth(), rcdInstance.displayRect_.GetHeight());
    rcdInstance.updateFlag_["display"] = false;
    rcdInstance.UpdateDisplayParameter(0, 0, 0, 0);
    EXPECT_TRUE(rcdInstance.updateFlag_["display"] == false);

    rcdInstance.rog_ = new rs_rcd::ROGSetting();
    rcdInstance.lcdModel_ = new rs_rcd::LCDModel();
    rcdInstance.lcdModel_->rogs.push_back(rcdInstance.rog_);
    EXPECT_TRUE(rcdInstance.GetTopSurfaceSource() == false);
    EXPECT_TRUE(rcdInstance.GetBottomSurfaceSource() == false);
    const int supportTopTag = 3;
    const int supportBottomTag = 6;
    for (int i = 0; i < 8; i++) {
        rcdInstance.supportTopSurface_ = (i & 0x1) > 0;
        rcdInstance.supportHardware_ = (i & 0x02) > 0;
        rcdInstance.supportBottomSurface_ = (i & 0x04) > 0;
        EXPECT_TRUE(rcdInstance.LoadImgsbyResolution(0, 0) !=
            ((i & supportTopTag) == supportTopTag || (i & supportBottomTag) == supportBottomTag));
    }
    rs_rcd::RogPortrait rogPortrait{};
    rcdInstance.rog_->portraitMap[rs_rcd::NODE_PORTRAIT] = rogPortrait;
    EXPECT_TRUE(rcdInstance.GetTopSurfaceSource() == false);
    rs_rcd::RogLandscape rogLand{};
    rcdInstance.rog_->landscapeMap[rs_rcd::NODE_LANDSCAPE] = rogLand;
    rcdInstance.supportHardware_ = false;
    EXPECT_TRUE(rcdInstance.GetTopSurfaceSource() == true);

    rcdInstance.supportTopSurface_ = true;
    rcdInstance.supportHardware_ = true;
    rcdInstance.supportBottomSurface_ = false;
    EXPECT_TRUE(rcdInstance.LoadImgsbyResolution(0, 0) == true);

    rcdInstance.supportBottomSurface_ = true;
    EXPECT_TRUE(rcdInstance.LoadImgsbyResolution(0, 0) == true);
    rcdInstance.UpdateDisplayParameter(0, 0, 0, 0);
    EXPECT_TRUE(rcdInstance.updateFlag_["display"] == false);
    delete rcdInstance.rog_;
    rcdInstance.rog_ = nullptr;
    rcdInstance.lcdModel_->rogs.clear();
    delete rcdInstance.lcdModel_;
    rcdInstance.lcdModel_ = nullptr;
}

/*
 * @tc.name: RSRoundCornerDisplayUpdateHardWare
 * @tc.desc: Test RSRoundCornerDisplayTest.RSRoundCornerDisplayUpdateHardWare
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, RSRoundCornerDisplayUpdateHardWare, TestSize.Level1)
{
    auto& rcdInstance = RSSingleton<RoundCornerDisplay>::GetInstance();
    rcdInstance.hardInfo_.resourcePreparing = true;
    rcdInstance.UpdateHardwareResourcePrepared(true);
    EXPECT_TRUE(rcdInstance.hardInfo_.resourcePreparing == false);
    EXPECT_TRUE(rcdInstance.hardInfo_.resourceChanged == false);
    rcdInstance.UpdateHardwareResourcePrepared(true);
    EXPECT_TRUE(rcdInstance.hardInfo_.resourceChanged == false);
}

/*
 * @tc.name: RSRoundCornerDisplayChooseRS
 * @tc.desc: Test RSRoundCornerDisplayTest.RSRoundCornerDisplayChooseRS
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, RSRoundCornerDisplayChooseRS, TestSize.Level1)
{
    auto& rcdInstance = RSSingleton<RoundCornerDisplay>::GetInstance();
    for (int i = 0; i < 4; i++) {
        ShowTopResourceType type = static_cast<ShowTopResourceType>(i);
        rcdInstance.showResourceType_ = type;
        rcdInstance.RcdChooseRSResource();
        EXPECT_TRUE(rcdInstance.curBottom_ == rcdInstance.imgBottomPortrait_);
    }
}

/*
 * @tc.name: RSRoundCornerDisplayDrawOne
 * @tc.desc: Test RSRoundCornerDisplayTest.RSRoundCornerDisplayDrawOne
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, RSRoundCornerDisplayDrawOne, TestSize.Level1)
{
    auto& rcdInstance = RSSingleton<RoundCornerDisplay>::GetInstance();
    rcdInstance.curTop_ = std::make_shared<Drawing::Image>();
    rcdInstance.curBottom_ = std::make_shared<Drawing::Image>();
    auto baseCanvas = std::make_shared<Drawing::Canvas>();
    auto canvas = std::make_shared<RSPaintFilterCanvas>(baseCanvas.get(), 1.0f);
    rcdInstance.DrawTopRoundCorner(canvas.get());
    rcdInstance.DrawBottomRoundCorner(canvas.get());
    rcdInstance.DrawOneRoundCorner(canvas.get(), 3);
    EXPECT_TRUE(canvas.get() != nullptr);
}
/*
 * @tc.name: RcdChooseHardwareResourceTest
 * @tc.desc: Test RSRoundCornerDisplay.RcdChooseHardwareResource
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, RcdChooseHardwareResourceTest, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSSymbolAnimationTest RcdChooseHardwareResourceTest start";
    auto& rcdInstance = RSSingleton<RoundCornerDisplay>::GetInstance();
    rs_rcd::ROGSetting rog;
    rog.height = 2772;
    rog.width = 1344;
    rcdInstance.rog_ = &rog;

    rcdInstance.showResourceType_ = TOP_PORTRAIT;
    rcdInstance.RcdChooseHardwareResource();

    rcdInstance.showResourceType_ = TOP_HIDDEN;
    rcdInstance.RcdChooseHardwareResource();

    rcdInstance.showResourceType_ = TOP_LADS_ORIT;
    rcdInstance.RcdChooseHardwareResource();

    rcdInstance.showResourceType_ = 4;
    rcdInstance.RcdChooseHardwareResource();
    EXPECT_EQ(nullptr, rcdInstance.hardInfo_.bottomLayer->curBitmap);

    rs_rcd::RogPortrait rogPortrait{};
    rcdInstance.rog_->portraitMap[rs_rcd::NODE_PORTRAIT] = rogPortrait;
    rs_rcd::RogLandscape rogLand{};
    rcdInstance.rog_->landscapeMap[rs_rcd::NODE_LANDSCAPE] = rogLand;

    rcdInstance.showResourceType_ = TOP_PORTRAIT;
    rcdInstance.RcdChooseHardwareResource();

    rcdInstance.showResourceType_ = TOP_HIDDEN;
    rcdInstance.RcdChooseHardwareResource();

    rcdInstance.showResourceType_ = TOP_LADS_ORIT;
    rcdInstance.RcdChooseHardwareResource();

    rcdInstance.showResourceType_ = 4;
    rcdInstance.RcdChooseHardwareResource();

    EXPECT_EQ(rcdInstance.bitmapBottomPortrait_, rcdInstance.hardInfo_.bottomLayer->curBitmap);
    rcdInstance.rog_ = nullptr;
    GTEST_LOG_(INFO) << "RSSymbolAnimationTest RcdChooseHardwareResourceTest end";
}

/*
 * @tc.name: UpdateNotchStatusTest
 * @tc.desc: Test RSRoundCornerDisplay.UpdateNotchStatus
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, UpdateNotchStatusTest, TestSize.Level1)
{
    auto& rcdInstance = RSSingleton<RoundCornerDisplay>::GetInstance();
    rcdInstance.Init();
    rcdInstance.UpdateNotchStatus(WINDOW_NOTCH_DEFAULT);
    // test status is < 0
    int notchStatus = -1;
    rcdInstance.UpdateNotchStatus(notchStatus);
    EXPECT_TRUE(rcdInstance.notchStatus_ == WINDOW_NOTCH_DEFAULT);

    // test status is > 1
    int notchStatusTwo = 2;
    rcdInstance.UpdateNotchStatus(notchStatusTwo);
    EXPECT_TRUE(rcdInstance.notchStatus_ == WINDOW_NOTCH_DEFAULT);
}

/*
 * @tc.name: RcdChooseTopResourceTypeTest
 * @tc.desc: Test RSRoundCornerDisplay.RcdChooseTopResourceType
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, RcdChooseTopResourceTypeTest, TestSize.Level1)
{
    auto& rcdInstance = RSSingleton<RoundCornerDisplay>::GetInstance();
    rcdInstance.Init();
    // test curOrientation is INVALID_SCREEN_ROTATION
    ScreenRotation curOrientation = ScreenRotation::INVALID_SCREEN_ROTATION;
    rcdInstance.UpdateOrientationStatus(curOrientation);
    rcdInstance.RcdChooseTopResourceType();
    EXPECT_TRUE(rcdInstance.curOrientation_ == ScreenRotation::INVALID_SCREEN_ROTATION);
    EXPECT_TRUE(rcdInstance.showResourceType_ == TOP_PORTRAIT);

    // test ScreenRotation::ROTATION_180, notchStatus is WINDOW_NOTCH_HIDDEN
    curOrientation = ScreenRotation::ROTATION_180;
    int notchStatus = WINDOW_NOTCH_HIDDEN;
    rcdInstance.UpdateNotchStatus(notchStatus);
    rcdInstance.UpdateOrientationStatus(curOrientation);
    rcdInstance.RcdChooseTopResourceType();
    EXPECT_TRUE(rcdInstance.showResourceType_ == TOP_HIDDEN);

    // test ScreenRotation::ROTATION_180, notchStatus is WINDOW_NOTCH_DEFAULT
    notchStatus = WINDOW_NOTCH_DEFAULT;
    rcdInstance.UpdateNotchStatus(notchStatus);
    rcdInstance.UpdateOrientationStatus(curOrientation);
    rcdInstance.RcdChooseTopResourceType();
    EXPECT_TRUE(rcdInstance.curOrientation_ == ScreenRotation::ROTATION_180);
    EXPECT_TRUE(rcdInstance.showResourceType_ == TOP_PORTRAIT);

    // test ScreenRotation::ROTATION_270, notchStatus is WINDOW_NOTCH_DEFAULT
    curOrientation = ScreenRotation::ROTATION_270;
    rcdInstance.UpdateOrientationStatus(curOrientation);
    rcdInstance.RcdChooseTopResourceType();
    EXPECT_TRUE(rcdInstance.curOrientation_ == ScreenRotation::ROTATION_270);
    EXPECT_TRUE(rcdInstance.showResourceType_ == TOP_PORTRAIT);
}

/*
 * @tc.name: RoundCornerDisplayManager
 * @tc.desc: Test RoundCornerDisplayManager.AddRoundCornerDisplay
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, RoundCornerDisplayManager_AddRcd, TestSize.Level1)
{
    auto& rcdInstance = RSSingleton<RoundCornerDisplayManager>::GetInstance();
    // 1.add and remove rcd module via nodeId
    auto res = rcdInstance.CheckExist(1);
    EXPECT_TRUE(res == false);
    rcdInstance.AddRoundCornerDisplay(1);
    rcdInstance.AddRoundCornerDisplay(1);
    res = rcdInstance.CheckExist(1);
    EXPECT_TRUE(res);
    rcdInstance.RemoveRoundCornerDisplay(0);
    res = rcdInstance.CheckExist(1);
    EXPECT_TRUE(res);
    rcdInstance.RemoveRoundCornerDisplay(1);
    res = rcdInstance.CheckExist(1);
    EXPECT_TRUE(res == false);
    rcdInstance.RemoveRCDResource(1);
    res = rcdInstance.CheckExist(1);
    EXPECT_TRUE(res == false);
}

/*
 * @tc.name: RoundCornerDisplayManager
 * @tc.desc: Test RoundCornerDisplayManager.AddLayer
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, RoundCornerDisplayManager_AddLayer, TestSize.Level1)
{
    auto& rcdInstance = RSSingleton<RoundCornerDisplayManager>::GetInstance();
    // 1.add layerInfo via renderTarget nodeId
    NodeId id = 1;
    auto top = RoundCornerDisplayManager::RCDLayerType::TOP;

    auto topName = std::string("TopLayer");

    rcdInstance.AddLayer(topName, id, top);
    auto LayerInfo = rcdInstance.GetLayerPair(topName);
    EXPECT_TRUE(LayerInfo.first == id && LayerInfo.second == top);
    rcdInstance.RemoveRCDLayerInfo(id);
    rcdInstance.RemoveRCDResource(id);
    LayerInfo = rcdInstance.GetLayerPair(topName);
    EXPECT_TRUE(LayerInfo.second == RoundCornerDisplayManager::RCDLayerType::INVALID);
}

/*
 * @tc.name: RoundCornerDisplayManager
 * @tc.desc: Test RoundCornerDisplayManager null rcd Case
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, RoundCornerDisplayManagerNULLRcd, TestSize.Level1)
{
    auto& rcdInstance = RSSingleton<RoundCornerDisplayManager>::GetInstance();
    // add layer and NodeId
    NodeId id = 1;
    int status = 1;
    uint32_t w = 1080, h = 1920;
    auto rot = ScreenRotation::ROTATION_90;
    rcdInstance.AddRoundCornerDisplay(id);
    auto res = rcdInstance.CheckExist(id);
    EXPECT_TRUE(res == true);
    rcdInstance.rcdMap_[id] = nullptr;
    rcdInstance.UpdateDisplayParameter(id, 0, 0, w, h);
    res = rcdInstance.CheckExist(id);
    EXPECT_TRUE(res == false);
    rcdInstance.rcdMap_[id] = nullptr;
    rcdInstance.UpdateNotchStatus(id, status);
    res = rcdInstance.CheckExist(id);
    EXPECT_TRUE(res == false);
    rcdInstance.rcdMap_[id] = nullptr;
    rcdInstance.UpdateOrientationStatus(id, rot);
    res = rcdInstance.CheckExist(id);
    EXPECT_TRUE(res == false);
}

/*
 * @tc.name: RoundCornerDisplayManager
 * @tc.desc: Test RoundCornerDisplayManagerUpdate
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, RoundCornerDisplayManagerUpdate, TestSize.Level1)
{
    auto& rcdInstance = RSSingleton<RoundCornerDisplayManager>::GetInstance();
    // add layer and NodeId
    NodeId id = 1;
    int status = 1;
    uint32_t w = 1080, h = 1920;
    auto rot = ScreenRotation::ROTATION_90;
    std::function<void()> task = []() {std::cout << "hardwareComposer RoundCornerDisplayManager Task" << std::endl;};
    // normal flow
    rcdInstance.AddRoundCornerDisplay(id);
    rcdInstance.UpdateDisplayParameter(id, 0, 0, w, h);
    rcdInstance.UpdateNotchStatus(id, status);
    rcdInstance.UpdateOrientationStatus(id, rot);
    auto hardInfo = rcdInstance.GetHardwareInfo(id);
    rcdInstance.RunHardwareTask(id, task);
    auto res = rcdInstance.CheckExist(id);
    EXPECT_TRUE(res == true);
    // no id flow
    rcdInstance.RemoveRoundCornerDisplay(id);
    rcdInstance.UpdateDisplayParameter(id, 0, 0, w, h);
    rcdInstance.UpdateNotchStatus(id, status);
    rcdInstance.UpdateOrientationStatus(id, rot);
    hardInfo = rcdInstance.GetHardwareInfo(id);
    rcdInstance.RunHardwareTask(id, task);
    res = rcdInstance.CheckExist(id);
    EXPECT_TRUE(res == false);
}

/*
 * @tc.name: RoundCornerDisplayManager
 * @tc.desc: Test RoundCornerDisplayManagerDraw
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, RoundCornerDisplayManagerDraw, TestSize.Level1)
{
    auto& rcdInstance = RSSingleton<RoundCornerDisplayManager>::GetInstance();
    // add layer and NodeId
    NodeId id = 1;
    auto top = RoundCornerDisplayManager::RCDLayerType::TOP;
    auto bottom = RoundCornerDisplayManager::RCDLayerType::BOTTOM;
    auto topName = std::string("TopLayer");
    auto bottomName = std::string("BottomLayer");
    rcdInstance.AddRoundCornerDisplay(id);
    rcdInstance.AddLayer(topName, id, top);
    rcdInstance.AddLayer(bottomName, id, bottom);
    EXPECT_TRUE(rcdInstance.CheckLayerIsRCD(std::string("InvalidName")) == false);
    EXPECT_TRUE(rcdInstance.CheckLayerIsRCD(topName) == true);
    std::vector<std::string> renderLayersName = {topName, bottomName, std::string("InvalidName")};
    std::vector<std::pair<NodeId, RoundCornerDisplayManager::RCDLayerType>> renderTargetNodeInfoList;
    for (auto& layerName : renderLayersName) {
        auto nodeInfo = rcdInstance.GetLayerPair(layerName);
        renderTargetNodeInfoList.push_back(nodeInfo);
    }
    auto baseCanvas = std::make_shared<Drawing::Canvas>();
    auto canvas = std::make_shared<RSPaintFilterCanvas>(baseCanvas.get(), 1.0f);
    std::function<void()> task = []() {std::cout << "hardwareComposer RoundCornerDisplayManager Task" << std::endl;};
    rcdInstance.DrawRoundCorner(renderTargetNodeInfoList, canvas.get());
    auto lcdModel = RSSingleton<rs_rcd::RCDConfig>::GetInstance().GetLcdModel(rs_rcd::ATTR_DEFAULT);;
    if (lcdModel) {
        EXPECT_TRUE(rcdInstance.GetRcdEnable() ==
           (lcdModel->surfaceConfig.topSurface.support || lcdModel->surfaceConfig.bottomSurface.support));
    }
    for (auto& info : renderTargetNodeInfoList) {
        auto nodeId = info.first;
        auto type = info.second;
        if (type == top) {
            rcdInstance.rcdMap_[nodeId] = nullptr;
        }
        rcdInstance.DrawTopRoundCorner(nodeId, canvas.get());
        rcdInstance.DrawTopRoundCorner(nodeId, canvas.get());
        if (type == bottom) {
            rcdInstance.rcdMap_[nodeId] = nullptr;
        }
        rcdInstance.DrawBottomRoundCorner(nodeId, canvas.get());
        rcdInstance.DrawBottomRoundCorner(nodeId, canvas.get());
        EXPECT_TRUE(rcdInstance.CheckExist(nodeId) == false);
    }
}

/*
 * @tc.name: RSRoundCornerDirtyRegion
 * @tc.desc: Test RSRoundCornerDirtyRegion
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, RSRoundCornerDirtyRegion, TestSize.Level1)
{
    auto &rcdInstance = RSSingleton<RoundCornerDisplayManager>::GetInstance();
    NodeId id = 1;
    RectI dirtyRect;
    bool flag = rcdInstance.HandleRoundCornerDirtyRect(id, dirtyRect, RoundCornerDisplayManager::RCDLayerType::TOP);
    EXPECT_TRUE(!flag && dirtyRect.IsEmpty());

    // Add round corner display node
    rcdInstance.AddRoundCornerDisplay(id);
    rcdInstance.rcdMap_[id]->rcdDirtyType_ = RoundCornerDirtyType::RCD_DIRTY_ALL;
    rcdInstance.rcdMap_[id]->hardInfo_.resourceChanged = true;
    
    // Handle rcd dirty rect without image resource
    flag = rcdInstance.HandleRoundCornerDirtyRect(id, dirtyRect, RoundCornerDisplayManager::RCDLayerType::TOP);
    flag &= rcdInstance.HandleRoundCornerDirtyRect(id, dirtyRect, RoundCornerDisplayManager::RCDLayerType::BOTTOM);
    EXPECT_TRUE(flag && dirtyRect.IsEmpty());

    // Handle rcd dirty rect with image resource
    rcdInstance.rcdMap_[id]->curTop_ = std::make_shared<Drawing::Image>();
    rcdInstance.rcdMap_[id]->curBottom_ = std::make_shared<Drawing::Image>();
    flag = rcdInstance.HandleRoundCornerDirtyRect(id, dirtyRect, RoundCornerDisplayManager::RCDLayerType::TOP);
    flag &= rcdInstance.HandleRoundCornerDirtyRect(id, dirtyRect, RoundCornerDisplayManager::RCDLayerType::BOTTOM);
    EXPECT_TRUE(flag && dirtyRect.IsEmpty());

    // Handle rcd dirty rect with resource prepared and reset dirty
    rcdInstance.rcdMap_[id]->hardInfo_.resourceChanged = false;
    flag = rcdInstance.HandleRoundCornerDirtyRect(id, dirtyRect, RoundCornerDisplayManager::RCDLayerType::TOP);
    flag &= rcdInstance.HandleRoundCornerDirtyRect(id, dirtyRect, RoundCornerDisplayManager::RCDLayerType::BOTTOM);
    EXPECT_TRUE(flag && dirtyRect.IsEmpty());

    // Handle rcd dirty rect with no dirty
    flag = rcdInstance.HandleRoundCornerDirtyRect(id, dirtyRect, RoundCornerDisplayManager::RCDLayerType::TOP);
    flag |= rcdInstance.HandleRoundCornerDirtyRect(id, dirtyRect, RoundCornerDisplayManager::RCDLayerType::BOTTOM);
    EXPECT_TRUE(!flag && dirtyRect.IsEmpty());
}

/*
 * @tc.name: RoundCornerDisplayPrintRCD
 * @tc.desc: Test RoundCornerDisplayPrintRCD printrcd
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, RoundCornerDisplayPrintRCD, TestSize.Level1)
{
    auto& rcdInstance = RSSingleton<RoundCornerDisplay>::GetInstance();
    rcdInstance.lcdModel_ = nullptr;
    rcdInstance.rog_ = nullptr;
    rcdInstance.hardInfo_.topLayer = nullptr;
    rcdInstance.hardInfo_.topLayer = nullptr;
    rcdInstance.PrintRCDInfo();
    EXPECT_TRUE(rcdInstance.lcdModel_ == nullptr);
    rcdInstance.lcdModel_ = new rs_rcd::LCDModel();
    rcdInstance.PrintRCDInfo();
    EXPECT_TRUE(rcdInstance.lcdModel_ != nullptr);
    rcdInstance.rog_ = new rs_rcd::ROGSetting();
    rcdInstance.PrintRCDInfo();
    EXPECT_TRUE(rcdInstance.rog_ != nullptr);
    rcdInstance.hardInfo_.topLayer = std::make_shared<rs_rcd::RoundCornerLayer>();
    rcdInstance.PrintRCDInfo();
    EXPECT_TRUE(rcdInstance.hardInfo_.topLayer != nullptr);
    rcdInstance.hardInfo_.bottomLayer = std::make_shared<rs_rcd::RoundCornerLayer>();
    rcdInstance.PrintRCDInfo();
    EXPECT_TRUE(rcdInstance.hardInfo_.bottomLayer != nullptr);
    delete rcdInstance.lcdModel_;
    rcdInstance.lcdModel_ = nullptr;
    delete rcdInstance.rog_;
    rcdInstance.rog_ = nullptr;
}

/*
 * @tc.name: RcdVisitorPrepareResourceBuffer
 * @tc.desc: Test RSRcdRenderVisitor PrepareResourceBuffer func
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, RcdVisitorPrepareResourceBuffer, TestSize.Level1)
{
    auto rcdNode = RSRcdSurfaceRenderNode::Create(0, static_cast<RCDSurfaceType>(RCDSurfaceType::BOTTOM));
    if (rcdNode == nullptr) {
        return;
    }
    auto layerInfo = std::make_shared<rs_rcd::RoundCornerLayer>(rs_rcd::RoundCornerLayer{
        "test",      // fileName
        1,           // offsetX
        1,           // offsetY
        "test.bin",  // binFileName
        10000,       // bufferSize
        2,           // cldWidth
        2,           // cldHeight
        nullptr      // null ptr
    });

    auto rcdVisitor = std::make_shared<RSRcdRenderVisitor>();
    auto rcdDrawable = std::static_pointer_cast<DrawableV2::RSRcdSurfaceRenderNodeDrawable>(rcdNode->renderDrawable_);
    rcdNode->renderDrawable_ = nullptr;
    EXPECT_TRUE(!rcdVisitor->PrepareResourceBuffer(*rcdNode, layerInfo));

    rcdNode->renderDrawable_ = rcdDrawable;
    EXPECT_TRUE(!rcdVisitor->PrepareResourceBuffer(*rcdNode, layerInfo));

    InitRcdRenderParams(rcdNode->renderDrawable_->renderParams_.get());
    std::shared_ptr<rs_rcd::RoundCornerLayer> layerInfoNull = nullptr;
    EXPECT_TRUE(!rcdVisitor->PrepareResourceBuffer(*rcdNode, layerInfoNull));
    EXPECT_TRUE(!rcdVisitor->PrepareResourceBuffer(*rcdNode, layerInfo));

    const char* path = "port_down.png";
    auto layerInfoValid = std::make_shared<rs_rcd::RoundCornerLayer>(rs_rcd::RoundCornerLayer{
        "test",                     // fileName
        1,                          // offsetX
        1,                          // offsetY
        "test.bin",                 // binFileName
        10000,                      // bufferSize
        2,                          // cldWidth
        2,                          // cldHeight
        LoadBitmapFromFile(path)    // valid image
    });
    if (layerInfo->curBitmap != nullptr) {
        EXPECT_TRUE(!rcdVisitor->PrepareResourceBuffer(*rcdNode, layerInfoValid));
    }
}

/*
 * @tc.name: RcdVisitorPrepareResourceBufferInvalid
 * @tc.desc: Test RSRcdRenderVisitor PrepareResourceBuffer func
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, RcdVisitorPrepareResourceBufferInvalid, TestSize.Level1)
{
    auto rcdNode = RSRcdSurfaceRenderNode::Create(0, static_cast<RCDSurfaceType>(RCDSurfaceType::TOP));
    if (rcdNode == nullptr) {
        return;
    }
    auto layerInfo = std::make_shared<rs_rcd::RoundCornerLayer>(rs_rcd::RoundCornerLayer{
        "test",                              // fileName
        -1,                                  // offsetX
        -1,                                  // offsetY
        "",                                  // binFileName
        0,                                   // bufferSize
        0,                                   // cldWidth
        0,                                   // cldHeight
        std::make_shared<Drawing::Bitmap>(), // null image
    });

    auto rcdVisitor = std::make_shared<RSRcdRenderVisitor>();
    auto rcdDrawable = std::static_pointer_cast<DrawableV2::RSRcdSurfaceRenderNodeDrawable>(rcdNode->renderDrawable_);
    EXPECT_TRUE(!rcdVisitor->PrepareResourceBuffer(*rcdNode, layerInfo));

    InitRcdRenderParamsInvalid(rcdNode->renderDrawable_->renderParams_.get());
    std::shared_ptr<rs_rcd::RoundCornerLayer> layerInfoNull = nullptr;
    EXPECT_TRUE(!rcdVisitor->PrepareResourceBuffer(*rcdNode, layerInfoNull));
    EXPECT_TRUE(!rcdVisitor->PrepareResourceBuffer(*rcdNode, layerInfo));
}

/*
 * @tc.name: RcdVisitorProcessRcdSurfaceRenderNode
 * @tc.desc: Test RSRcdRenderVisitor ProcessRcdSurfaceRenderNode func
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, RcdVisitorProcessRcdSurfaceRenderNode, TestSize.Level1)
{
    const char* path = "port_down.png";
    auto bitmap = LoadBitmapFromFile(path);
    if (bitmap == nullptr) {
        std::cout << "RcdVisitorProcessRcdSurfaceRenderNode: current os no rcd source" << std::endl;
        return;
    }

    auto rcdVisitor = std::make_shared<RSRcdRenderVisitor>();
    auto rcdRenderNode = RSRcdSurfaceRenderNode::Create(0, static_cast<RCDSurfaceType>(RCDSurfaceType::INVALID));
    rcdVisitor->ProcessRcdSurfaceRenderNode(*rcdRenderNode, false);

    auto processor = std::make_shared<RSUniRenderProcessor>();
    rcdVisitor->SetUniProcessor(processor);
    EXPECT_TRUE(rcdVisitor->uniProcessor_ != nullptr);
    rcdVisitor->ProcessRcdSurfaceRenderNode(*rcdRenderNode, false);

    rcdRenderNode->rcdExtInfo_.surfaceType = RCDSurfaceType::BOTTOM;
    rcdVisitor->ProcessRcdSurfaceRenderNode(*rcdRenderNode, true);

    auto surfaceHandler = rcdRenderNode->surfaceHandler_;
    rcdRenderNode->surfaceHandler_ = nullptr;
    rcdVisitor->ProcessRcdSurfaceRenderNode(*rcdRenderNode, false);

    rcdRenderNode->surfaceHandler_ = surfaceHandler;
    rcdRenderNode->surfaceHandler_->buffer_.buffer = nullptr;
    rcdVisitor->ProcessRcdSurfaceRenderNode(*rcdRenderNode, false);

    auto surfaceBuffer = new SurfaceBufferImpl();
    rcdRenderNode->surfaceHandler_->buffer_.buffer = surfaceBuffer;
    rcdVisitor->ProcessRcdSurfaceRenderNode(*rcdRenderNode, false);
}

/*
 * @tc.name: PrepareHardwareResourceBuffer01
 * @tc.desc: Test RSRoundCornerDisplayTest.PrepareHardwareResourceBuffer01
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, PrepareHardwareResourceBuffer01, TestSize.Level1)
{
    auto rcdRenderNode = RSRcdSurfaceRenderNode::Create(0, static_cast<RCDSurfaceType>(RCDSurfaceType::BOTTOM));

    bool flag = rcdRenderNode->PrepareHardwareResourceBuffer(nullptr);
    EXPECT_TRUE(!flag);

    const char* path = "port_down.png";
    auto layerInfo = std::make_shared<rs_rcd::RoundCornerLayer>(rs_rcd::RoundCornerLayer{
        "test",                     // fileName
        1,                          // offsetX
        1,                          // offsetY
        "test.bin",                 // binFileName
        10000,                      // bufferSize
        2,                          // cldWidth
        2,                          // cldHeight
        LoadBitmapFromFile(path)    // valid image
    });
    flag = rcdRenderNode->PrepareHardwareResourceBuffer(layerInfo);
    EXPECT_TRUE(!flag);

    if (layerInfo->curBitmap == nullptr) {
        std::cout << "PrepareHardwareResourceBufferTest: current os no rcd source" << std::endl;
        return;
    }

    rcdRenderNode->SetRenderDisplayRect(RectU(0, 0, 10, 10));
    rcdRenderNode->PrepareHardwareResourceBuffer(layerInfo);
    uint32_t size = 10;
    rcdRenderNode->SetRcdBufferSize(size);
    auto bufferSize = rcdRenderNode->GetRcdBufferSize();
    EXPECT_EQ(bufferSize, size);

    uint32_t height = 20;
    rcdRenderNode->SetRcdBufferHeight(height);
    auto bufferHeight = rcdRenderNode->GetRcdBufferHeight();
    EXPECT_EQ(bufferHeight, height);

    uint32_t width = 100;
    rcdRenderNode->SetRcdBufferWidth(width);
    auto bufferWidth = rcdRenderNode->GetRcdBufferWidth();
    EXPECT_EQ(bufferWidth, width);
    rcdRenderNode->PrepareHardwareResourceBuffer(nullptr);
    rcdRenderNode->PrepareHardwareResourceBuffer(layerInfo);
    width = 0;
    rcdRenderNode->SetRcdBufferWidth(width);
    EXPECT_EQ(rcdRenderNode->GetRcdBufferWidth(), width);
}

/*
 * @tc.name: PrepareHardwareResourceBuffer02
 * @tc.desc: Test RSRoundCornerDisplayTest.PrepareHardwareResourceBuffer02
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, PrepareHardwareResourceBuffer02, TestSize.Level1)
{
    auto rcdRenderNode = RSRcdSurfaceRenderNode::Create(0, static_cast<RCDSurfaceType>(RCDSurfaceType::BOTTOM));

    bool flag = rcdRenderNode->PrepareHardwareResourceBuffer(nullptr);
    EXPECT_TRUE(!flag);

    auto layerInfo = std::make_shared<rs_rcd::RoundCornerLayer>(rs_rcd::RoundCornerLayer{
        "test",                              // fileName
        1,                                   // offsetX
        1,                                   // offsetY
        "test.bin",                          // binFileName
        10000,                               // bufferSize
        2,                                   // cldWidth
        2,                                   // cldHeight
        std::make_shared<Drawing::Bitmap>()  // valid image
    });

    flag = rcdRenderNode->PrepareHardwareResourceBuffer(layerInfo);
    EXPECT_TRUE(!flag);

    Drawing::BitmapFormat format = { Drawing::ColorType::COLORTYPE_RGBA_8888, Drawing::AlphaType::ALPHATYPE_PREMUL };
    layerInfo->curBitmap->Build(10, 0, format);
    flag = rcdRenderNode->PrepareHardwareResourceBuffer(layerInfo);
    EXPECT_TRUE(!flag);

    layerInfo->curBitmap->Build(0, 10, format);
    flag = rcdRenderNode->PrepareHardwareResourceBuffer(layerInfo);
    EXPECT_TRUE(!flag);

    layerInfo->curBitmap->Build(10, 10, format);
    flag = rcdRenderNode->PrepareHardwareResourceBuffer(layerInfo);
    EXPECT_TRUE(!flag);

    rcdRenderNode->SetRenderDisplayRect(RectU(0, 0, 10, 10));
    flag = rcdRenderNode->PrepareHardwareResourceBuffer(layerInfo);

    rcdRenderNode->SetRcdBufferHeight(10);
    rcdRenderNode->SetRcdBufferWidth(10);
    flag = rcdRenderNode->PrepareHardwareResourceBuffer(layerInfo);

    rcdRenderNode->rcdExtInfo_.surfaceType = RCDSurfaceType::TOP;
    flag = rcdRenderNode->PrepareHardwareResourceBuffer(layerInfo);
}

/*
 * @tc.name: RcdSurfaceRenderNodeOnSyncTest
 * @tc.desc: Test RSRoundCornerDisplayTest.RcdSurfaceRenderNodeOnSyncTest
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, RcdSurfaceRenderNodeOnSyncTest, TestSize.Level1)
{
    auto rcdRenderNode = RSRcdSurfaceRenderNode::Create(0, static_cast<RCDSurfaceType>(RCDSurfaceType::BOTTOM));

    InitRcdRenderParams(rcdRenderNode->stagingRenderParams_.get());
    InitRcdRenderParamsUnsync(rcdRenderNode->renderDrawable_->renderParams_.get());
    auto nodeParams = static_cast<RSRcdRenderParams*>(rcdRenderNode->stagingRenderParams_.get());
    auto drawableParams = static_cast<RSRcdRenderParams *>(rcdRenderNode->renderDrawable_->renderParams_.get());

    auto tempParams = std::move(rcdRenderNode->stagingRenderParams_);
    rcdRenderNode->OnSync();
    EXPECT_NE(nodeParams->GetPathBin(), drawableParams->GetPathBin());
    rcdRenderNode->stagingRenderParams_ = std::move(tempParams);

    auto tempDrawable = rcdRenderNode->renderDrawable_;
    rcdRenderNode->renderDrawable_ = nullptr;
    rcdRenderNode->OnSync();
    EXPECT_NE(nodeParams->GetPathBin(), drawableParams->GetPathBin());
    rcdRenderNode->renderDrawable_ = tempDrawable;

    EXPECT_NE(nodeParams->pathBin_, drawableParams->pathBin_);
    EXPECT_NE(nodeParams->bufferSize_, drawableParams->bufferSize_);
    EXPECT_NE(nodeParams->cldWidth_, drawableParams->cldWidth_);
    EXPECT_NE(nodeParams->cldHeight_, drawableParams->cldHeight_);
    EXPECT_NE(nodeParams->srcRect_.GetWidth(), drawableParams->srcRect_.GetWidth());
    EXPECT_NE(nodeParams->dstRect_.GetWidth(), drawableParams->dstRect_.GetWidth());
    EXPECT_NE(nodeParams->rcdBitmap_, drawableParams->rcdBitmap_);
    EXPECT_NE(nodeParams->rcdEnabled_, drawableParams->rcdEnabled_);
    EXPECT_NE(nodeParams->resourceChanged_, drawableParams->resourceChanged_);

    nodeParams->resourceChanged_ = false;
    nodeParams->rcdEnabled_ = !RSSingleton<RoundCornerDisplayManager>::GetInstance().GetRcdEnable();
    rcdRenderNode->OnSync();
    EXPECT_EQ(nodeParams->rcdEnabled_, RSSingleton<RoundCornerDisplayManager>::GetInstance().GetRcdEnable());

    nodeParams->resourceChanged_ = true;
    rcdRenderNode->OnSync();

    EXPECT_EQ(nodeParams->pathBin_, drawableParams->pathBin_);
    EXPECT_EQ(nodeParams->bufferSize_, drawableParams->bufferSize_);
    EXPECT_EQ(nodeParams->cldWidth_, drawableParams->cldWidth_);
    EXPECT_EQ(nodeParams->cldHeight_, drawableParams->cldHeight_);
    EXPECT_EQ(nodeParams->srcRect_.GetWidth(), drawableParams->srcRect_.GetWidth());
    EXPECT_EQ(nodeParams->dstRect_.GetWidth(), drawableParams->dstRect_.GetWidth());
    EXPECT_EQ(nodeParams->rcdBitmap_, drawableParams->rcdBitmap_);
    EXPECT_EQ(nodeParams->rcdEnabled_, drawableParams->rcdEnabled_);
    EXPECT_EQ(nodeParams->resourceChanged_, drawableParams->resourceChanged_);
}

/*
 * @tc.name: PrepareHardwareResourceTest
 * @tc.desc: Test RSRoundCornerDisplayTest.PrepareHardwareResourceTest
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, PrepareHardwareResourceTest, TestSize.Level1)
{
    auto rcdRenderNode = RSRcdSurfaceRenderNode::Create(0, static_cast<RCDSurfaceType>(RCDSurfaceType::BOTTOM));
    auto layerInfo = std::make_shared<rs_rcd::RoundCornerLayer>(rs_rcd::RoundCornerLayer{
        "test",                              // fileName
        1,                                   // offsetX
        1,                                   // offsetY
        "test.bin",                          // binFileName
        10000,                               // bufferSize
        2,                                   // cldWidth
        2,                                   // cldHeight
        std::make_shared<Drawing::Bitmap>()  // valid image
    });
    rcdRenderNode->PrepareHardwareResource(layerInfo);
    auto drawable =
        std::static_pointer_cast<DrawableV2::RSRcdSurfaceRenderNodeDrawable>(rcdRenderNode->renderDrawable_);
    EXPECT_TRUE(drawable->surface_ != nullptr);
}

/*
 * @tc.name: DoProcessRenderMainThreadTaskTest
 * @tc.desc: Test RSRoundCornerDisplayTest.DoProcessRenderMainThreadTaskTest
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, DoProcessRenderMainThreadTaskTest, TestSize.Level1)
{
    auto rcdRenderNode = RSRcdSurfaceRenderNode::Create(0, static_cast<RCDSurfaceType>(RCDSurfaceType::INVALID));

    const char* path = "port_down.png";
    auto bitmap = LoadBitmapFromFile(path);
    if (bitmap == nullptr) {
        std::cout << "DoProcessRenderMainThreadTaskTest: current os no rcd source" << std::endl;
        return;
    }

    rcdRenderNode->DoProcessRenderMainThreadTask(true, nullptr);

    auto processor = std::make_shared<RSUniRenderProcessor>();
    rcdRenderNode->DoProcessRenderMainThreadTask(true, processor);

    rcdRenderNode->rcdExtInfo_.surfaceType = RCDSurfaceType::BOTTOM;
    rcdRenderNode->DoProcessRenderMainThreadTask(true, processor);

    auto surfaceHandler = rcdRenderNode->surfaceHandler_;
    rcdRenderNode->surfaceHandler_ = nullptr;
    rcdRenderNode->DoProcessRenderMainThreadTask(false, processor);

    rcdRenderNode->surfaceHandler_ = surfaceHandler;
    rcdRenderNode->surfaceHandler_->buffer_.buffer = nullptr;
    rcdRenderNode->DoProcessRenderMainThreadTask(false, processor);

    auto surfaceBuffer = new SurfaceBufferImpl();
    rcdRenderNode->surfaceHandler_->buffer_.buffer = surfaceBuffer;
    rcdRenderNode->DoProcessRenderMainThreadTask(false, processor);

    EXPECT_TRUE(processor->layers_.size() == 0);
}

/*
 * @tc.name: RegisterRcdMsgTest
 * @tc.desc: Test RSRoundCornerDisplayTest.RegisterRcdMsgTest
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, RegisterRcdMsgTest, TestSize.Level1)
{
    auto& rcdInstance = RSSingleton<RoundCornerDisplayManager>::GetInstance();
    auto& msgBus = RSSingleton<RsMessageBus>::GetInstance();
    EXPECT_TRUE(!rcdInstance.isRcdMessageRegisted_);

    rcdInstance.RegisterRcdMsg();
    EXPECT_TRUE(rcdInstance.isRcdMessageRegisted_ == rcdInstance.GetRcdEnable());

    NodeId id = 0;
    msgBus.SendMsg<NodeId, uint32_t, uint32_t, uint32_t, uint32_t>(TOPIC_RCD_DISPLAY_SIZE, id, 0, 0, 0, 0);
    msgBus.SendMsg<NodeId, ScreenRotation>(TOPIC_RCD_DISPLAY_ROTATION, id, ScreenRotation::ROTATION_0);
    msgBus.SendMsg<NodeId, int>(TOPIC_RCD_DISPLAY_NOTCH, id, 0);
    msgBus.SendMsg<NodeId, bool>(TOPIC_RCD_DISPLAY_HWRESOURCE, id, 0);
}

/*
 * @tc.name: RcdCldLayerInfoTest
 * @tc.desc: Test RSRoundCornerDisplayTest.RcdCldLayerInfoTest
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, RcdCldLayerInfoTest, TestSize.Level1)
{
    auto rcdNode = RSRcdSurfaceRenderNode::Create(0, static_cast<RCDSurfaceType>(RCDSurfaceType::BOTTOM));

    InitRcdCldLayerInfo(&rcdNode->cldLayerInfo_);
    rcdNode->UpdateRcdRenderParams(true, nullptr);

    auto rcdParams = static_cast<RSRcdRenderParams*>(rcdNode->stagingRenderParams_.get());
    EXPECT_TRUE(rcdParams->GetPathBin() == "/sys_prod/etc/display/RoundCornerDisplay/test.bin");
    EXPECT_TRUE(rcdParams->GetBufferSize() == 1);
    EXPECT_TRUE(rcdParams->GetCldWidth() == 1);
    EXPECT_TRUE(rcdParams->GetCldHeight() == 1);
}

/*
 * @tc.name: RcdExtInfoTest
 * @tc.desc: Test RSRoundCornerDisplayTest.RcdExtInfoTest
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, RcdExtInfoTest, TestSize.Level1)
{
    auto rcdNode = RSRcdSurfaceRenderNode::Create(0, static_cast<RCDSurfaceType>(RCDSurfaceType::BOTTOM));

    InitRcdExtInfo(&rcdNode->rcdExtInfo_);
    rcdNode->UpdateRcdRenderParams(true, nullptr);

    auto rcdParams = static_cast<RSRcdRenderParams*>(rcdNode->stagingRenderParams_.get());
    EXPECT_FALSE(rcdParams->GetSrcRect().IsEmpty());
    EXPECT_FALSE(rcdParams->GetDstRect().IsEmpty());

    EXPECT_FALSE(rcdNode->GetSrcRect().IsEmpty());
    EXPECT_FALSE(rcdNode->GetDstRect().IsEmpty());
    EXPECT_TRUE(rcdNode->IsBottomSurface());

    rcdNode->ResetCurrFrameState();
    EXPECT_TRUE(rcdNode->GetSrcRect().IsEmpty());
    EXPECT_TRUE(rcdNode->GetDstRect().IsEmpty());
    EXPECT_TRUE(rcdNode->rcdExtInfo_.surfaceBounds.IsEmpty());
    EXPECT_TRUE(rcdNode->rcdExtInfo_.frameBounds.IsEmpty());
    EXPECT_TRUE(rcdNode->rcdExtInfo_.frameViewPort.IsEmpty());
}

/*
 * @tc.name: RcdSourceInfoTest
 * @tc.desc: Test RSRoundCornerDisplayTest.RcdSourceInfoTest
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, RcdSourceInfoTest, TestSize.Level1)
{
    auto rcdNode = RSRcdSurfaceRenderNode::Create(0, static_cast<RCDSurfaceType>(RCDSurfaceType::BOTTOM));

    InitRcdSourceInfo(&rcdNode->rcdSourceInfo);
    EXPECT_EQ(rcdNode->GetRcdBufferWidth(), 1);
    EXPECT_EQ(rcdNode->GetRcdBufferHeight(), 1);
    EXPECT_EQ(rcdNode->GetRcdBufferSize(), 1);

    rcdNode->SetRcdBufferWidth(100);
    rcdNode->SetRcdBufferHeight(100);
    rcdNode->SetRcdBufferSize(10000);
    EXPECT_EQ(rcdNode->GetRcdBufferWidth(), 100);
    EXPECT_EQ(rcdNode->GetRcdBufferHeight(), 100);
    EXPECT_EQ(rcdNode->GetRcdBufferSize(), 10000);
}
} // OHOS::Rosen
