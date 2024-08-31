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
#include "pipeline/parallel_render/rs_sub_thread_manager.h"
#include "pipeline/round_corner_display/rs_message_bus.h"
#include "pipeline/round_corner_display/rs_round_corner_display.h"
#include "pipeline/round_corner_display/rs_round_corner_config.h"
#include "pipeline/round_corner_display/rs_rcd_surface_render_node.h"
#include "pipeline/round_corner_display/rs_rcd_render_visitor.h"
#include "pipeline/rs_display_render_node.h"
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
    rcdInstance.UpdateDisplayParameter(width, height);

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
    rcdInstance.UpdateDisplayParameter(width, height);

    std::unique_ptr<Drawing::Canvas> drawingCanvas = std::make_unique<Drawing::Canvas>(width, height);
    std::shared_ptr<RSPaintFilterCanvas> canvas = std::make_shared<RSPaintFilterCanvas>(drawingCanvas.get());
    ASSERT_NE(canvas, nullptr);
    rcdInstance.DrawRoundCorner(canvas.get());
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
    Drawing::Bitmap bitmapBottomPortrait;
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

    std::shared_ptr<Drawing::Image> imgNoneImageLoaded = std::make_shared<Drawing::Image>();
    res = rcdInstance.DecodeBitmap(imgNoneImageLoaded, bitmapBottomPortrait);
    EXPECT_TRUE(res == true);
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
    Drawing::Bitmap bitmapBottomPortrait;
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
    auto res = rcdInstance.Init();
    EXPECT_TRUE(res == true);

    uint32_t width = 1344;
    uint32_t height = 2772;
    rcdInstance.UpdateDisplayParameter(width, height);

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
    rcdInstance.UpdateDisplayParameter(width, height);

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

/*
 * @tc.name: ProcessRcdSurfaceRenderNode1
 * @tc.desc: Test ProcessRcdSurfaceRenderNode1
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, ProcessRcdSurfaceRenderNode1, TestSize.Level1)
{
    // prepare test
    std::shared_ptr<Drawing::Image> imgBottomPortrait;
    Drawing::Bitmap bitmapBottomPortrait;
    const char* path = "port_down.png";

    auto& rcdInstance = RSSingleton<RoundCornerDisplay>::GetInstance();
    rcdInstance.Init();
    rcdInstance.LoadImg(path, imgBottomPortrait);
    if (imgBottomPortrait == nullptr) {
        std::cout << "RSRoundCornerDisplayTest: current os less rcd source" << std::endl;
        return;
    }
    rcdInstance.DecodeBitmap(imgBottomPortrait, bitmapBottomPortrait);

    auto& rcdCfg = RSSingleton<rs_rcd::RCDConfig>::GetInstance();
    rcdCfg.Load(std::string(rs_rcd::PATH_CONFIG_FILE));
    rs_rcd::LCDModel* lcdModel = rcdCfg.GetLcdModel(std::string(rs_rcd::ATTR_DEFAULT));
    if (lcdModel == nullptr) {
        std::cout << "RSRoundCornerDisplayTest: current os less lcdModel source" << std::endl;
        return;
    }
    int width = 0;
    int height = 0;
    rs_rcd::ROGSetting* rog = GetRogFromLcdModel(lcdModel, width, height);
    if (rog == nullptr) {
        std::cout << "RSRoundCornerDisplayTest: current os less rog source" << std::endl;
        return;
    }
    rcdInstance.rog_ = rog;
    rcdInstance.GetTopSurfaceSource();
    rcdInstance.GetBottomSurfaceSource();
    rcdInstance.rog_ = nullptr;

    rs_rcd::RoundCornerHardware hardInfo;
    hardInfo.bottomLayer = &rog->portraitMap[rs_rcd::NODE_PORTRAIT].layerDown;
    if (hardInfo.bottomLayer == nullptr) {
        std::cout << "RSRoundCornerDisplayTest: current os less bottomLayer source" << std::endl;
        return;
    }
    hardInfo.bottomLayer->layerWidth = width;
    hardInfo.bottomLayer->layerHeight = height;
    hardInfo.bottomLayer->curBitmap = &bitmapBottomPortrait;

    std::shared_ptr<RSRcdSurfaceRenderNode> bottomSurfaceNode =
        std::make_shared<RSRcdSurfaceRenderNode>(0, RCDSurfaceType::BOTTOM);
    if (bottomSurfaceNode == nullptr) {
        std::cout << "RSRoundCornerDisplayTest: current os less bottomSurfaceNode source" << std::endl;
        return;
    }
    HardwareLayerInfo info{};
    bottomSurfaceNode->FillHardwareResource(info, 0, 0);
    auto visitor = std::make_shared<RSRcdRenderVisitor>();
    // test
    visitor->ProcessRcdSurfaceRenderNode(*bottomSurfaceNode, hardInfo.bottomLayer, true);
}

/*
 * @tc.name: ProcessRcdSurfaceRenderNode2
 * @tc.desc: Test ProcessRcdSurfaceRenderNode2
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, ProcessRcdSurfaceRenderNode2, TestSize.Level1)
{
    // prepare test
    std::shared_ptr<Drawing::Image> imgBottomPortrait;
    Drawing::Bitmap bitmapBottomPortrait;
    const char* path = "port_down.png";

    auto& rcdInstance = RSSingleton<RoundCornerDisplay>::GetInstance();
    rcdInstance.Init();
    rcdInstance.LoadImg(path, imgBottomPortrait);
    if (imgBottomPortrait == nullptr) {
        std::cout << "RSRoundCornerDisplayTest: current os less rcd source" << std::endl;
        return;
    }
    rcdInstance.DecodeBitmap(imgBottomPortrait, bitmapBottomPortrait);

    auto& rcdCfg = RSSingleton<rs_rcd::RCDConfig>::GetInstance();
    rcdCfg.Load(std::string(rs_rcd::PATH_CONFIG_FILE));
    rs_rcd::LCDModel* lcdModel = rcdCfg.GetLcdModel(std::string(rs_rcd::ATTR_DEFAULT));
    if (lcdModel == nullptr) {
        std::cout << "RSRoundCornerDisplayTest: current os less lcdModel source" << std::endl;
        return;
    }
    int width = 0;
    int height = 0;
    rs_rcd::ROGSetting* rog = GetRogFromLcdModel(lcdModel, width, height);
    if (rog == nullptr) {
        std::cout << "RSRoundCornerDisplayTest: current os less rog source" << std::endl;
        return;
    }

    rs_rcd::RoundCornerHardware hardInfo;
    hardInfo.bottomLayer = &rog->portraitMap[rs_rcd::NODE_PORTRAIT].layerDown;
    if (hardInfo.bottomLayer == nullptr) {
        std::cout << "RSRoundCornerDisplayTest: current os less bottomLayer source" << std::endl;
        return;
    }
    hardInfo.bottomLayer->layerWidth = width;
    hardInfo.bottomLayer->layerHeight = height;
    hardInfo.bottomLayer->curBitmap = &bitmapBottomPortrait;

    std::shared_ptr<RSRcdSurfaceRenderNode> bottomSurfaceNode =
        std::make_shared<RSRcdSurfaceRenderNode>(0, RCDSurfaceType::BOTTOM);
    auto visitor = std::make_shared<RSRcdRenderVisitor>();

    // test
    visitor->ProcessRcdSurfaceRenderNode(*bottomSurfaceNode, hardInfo.bottomLayer, false);
}


/*
 * @tc.name: ConsumeAndUpdateBufferTest
 * @tc.desc: Test RSRoundCornerDisplayTest.ConsumeAndUpdateBufferTest
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, ConsumeAndUpdateBufferTest, TestSize.Level1)
{
    std::shared_ptr<RSRcdSurfaceRenderNode> topSurfaceNode =
        std::make_shared<RSRcdSurfaceRenderNode>(0, RCDSurfaceType::TOP);
    ASSERT_NE(topSurfaceNode, nullptr);
    auto visitor = std::make_shared<RSRcdRenderVisitor>();
    ASSERT_EQ(true, visitor->ConsumeAndUpdateBuffer(*topSurfaceNode));
}

struct TestMsgBus {
    template<typename T>
    void TestFunc1(T a)
    {
        std::cout << "TestMsg Bus Func1:" << sizeof(T) << "," << &a << std::endl;
    }

    template<typename T1, typename T2>
    void TestFunc2(T1 a, T2 b)
    {
        std::cout << "TestMsg Bus Func2:" <<
            sizeof(T1) << "," << &a << std::endl <<
            sizeof(T2) << "," << &b << std::endl;
    }

    template<typename T1, typename T2, typename T3>
    void TestFunc3(T1 a, T2 b, T3 c)
    {
        std::cout << "TestMsg Bus Func3:" <<
            sizeof(T1) << "," << &a << std::endl <<
            sizeof(T2) << "," << &b << std::endl <<
            sizeof(T3) << "," << &c << std::endl;
    }
};

template<typename T1, typename T2, typename T3>
void TestMsgBusFunc()
{
    std::string topic = "TEST_TOPIC";
    auto& msgBus = RSSingleton<RsMessageBus>::GetInstance();
    TestMsgBus obj;
    int num1 = 1;
    int num2 = 2;
    int num3 = 3;
    msgBus.RegisterTopic<T1>(topic, &obj, &TestMsgBus::TestFunc1<T1>);
    msgBus.SendMsg(topic, static_cast<T1>(num1));
    msgBus.RemoveTopic<T1>(topic);
    msgBus.RegisterTopic<T1, T2>(topic, &obj, &TestMsgBus::TestFunc2<T1, T2>);
    msgBus.SendMsg(topic, static_cast<T1>(num1), static_cast<T2>(num2));
    msgBus.RemoveTopic<T1, T2>(topic);
    msgBus.RegisterTopic<T1, T2, T3>(topic, &obj, &TestMsgBus::TestFunc3<T1, T2, T3>);
    msgBus.SendMsg(topic, static_cast<T1>(num1), static_cast<T2>(num2), static_cast<T3>(num3));
    msgBus.RemoveTopic<T1, T2, T3>(topic);
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
    auto invalidLcd = rcdCfg.GetLcdModel(std::string("invalideName"));
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
    defaultLcd->GetRog(0, 0);
    defaultLcd->GetHardwareComposerConfig();
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

    std::vector<std::string> properties = {
        rs_rcd::ATTR_FILENAME,
        rs_rcd::ATTR_OFFSET_X,
        rs_rcd::ATTR_OFFSET_Y,
        rs_rcd::ATTR_BINFILENAME,
        rs_rcd::ATTR_BUFFERSIZE,
        rs_rcd::ATTR_CLDWIDTH,
        rs_rcd::ATTR_CLDHEIGHT
    };

    rs_rcd::RoundCornerLayer cfgData = {
        "test", // fileName
        1, // offsetX
        1, // offsetY
        "test.bin", // binFileName
        10000, // bufferSize
        2, // cldWidth
        2, // cldHeight
        0,
        0,
        nullptr
    };
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

    std::vector<std::string> okCase = {
        "0.0",
        "0",
        "123",
        "1230.0",
        "8192.0 ",
        "819200",
    };
    for (auto& tmpCase : okCase) {
        bool isOk = rs_rcd::XMLReader::RegexMatchNum(tmpCase);
        EXPECT_EQ(isOk, true);
    }

    std::vector<std::string> ngCase = {
        "a0.0",
        "0a",
        "a123",
        "1230.0c",
        "a8192.0 ",
    };
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
        RSRcdSurfaceRenderNode rcdRenderNode(0, static_cast<RCDSurfaceType>(i));
        rcdRenderNode.ClearBufferCache();
        rcdRenderNode.ResetCurrFrameState();
        rcdRenderNode.Reset();

        rcdRenderNode.GetSrcRect();
        rcdRenderNode.GetDstRect();
        rcdRenderNode.IsSurfaceCreated();
        rcdRenderNode.IsBottomSurface();
        rcdRenderNode.IsTopSurface();
        rcdRenderNode.IsInvalidSurface();
        rcdRenderNode.GetFrameOffsetX();
        rcdRenderNode.GetFrameOffsetY();
        rcdRenderNode.GetRSSurface();
        rcdRenderNode.GetHardenBufferRequestConfig();
        auto comsumer = rcdRenderNode.GetConsumerListener();
        rcdRenderNode.CreateSurface(comsumer);
        rcdRenderNode.SetRcdBufferSize(0);
        rcdRenderNode.SetRcdBufferHeight(0);
        rcdRenderNode.SetRcdBufferWidth(0);
        rcdRenderNode.SetHardwareResourceToBuffer();
        rcdRenderNode.PrepareHardwareResourceBuffer(nullptr);
        rs_rcd::RoundCornerLayer layer;
        rcdRenderNode.PrepareHardwareResourceBuffer(&layer);
        uint32_t size = 10;
        rcdRenderNode.SetRcdBufferSize(size);
        auto bufferSize = rcdRenderNode.GetRcdBufferSize();
        EXPECT_EQ(bufferSize, size);

        uint32_t height = 20;
        rcdRenderNode.SetRcdBufferHeight(height);
        auto bufferHeight = rcdRenderNode.GetRcdBufferHeight();
        EXPECT_EQ(bufferHeight, height);

        uint32_t width = 100;
        rcdRenderNode.SetRcdBufferWidth(width);
        auto bufferWidth = rcdRenderNode.GetRcdBufferWidth();
        EXPECT_EQ(bufferWidth, width);
        rcdRenderNode.GetHardenBufferRequestConfig();
        rcdRenderNode.SetHardwareResourceToBuffer();
        rcdRenderNode.PrepareHardwareResourceBuffer(nullptr);
        rcdRenderNode.PrepareHardwareResourceBuffer(&layer);
    }
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
    int type = 4;
    EXPECT_EQ(rcdInstance.showResourceType_, type);
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
    EXPECT_TRUE(rcdInstance.showResourceType_ == TOP_PORTRAIT);

    // test ScreenRotation::ROTATION_270, notchStatus is WINDOW_NOTCH_DEFAULT
    curOrientation = ScreenRotation::ROTATION_270;
    rcdInstance.UpdateOrientationStatus(curOrientation);
    rcdInstance.RcdChooseTopResourceType();
    EXPECT_TRUE(rcdInstance.showResourceType_ == TOP_PORTRAIT);
}
}
