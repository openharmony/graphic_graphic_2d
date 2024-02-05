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

#include "gtest/gtest.h"
#include "common/rs_singleton.h"
#include "pipeline/parallel_render/rs_sub_thread_manager.h"
#include "pipeline/round_corner_display/rs_sub_thread_rcd.h"
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

/*
 * @tc.name: RSSubThreadRCDTest
 * @tc.desc: Test RSRoundCornerDisplayTest.RSSubThreadRCDTest
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, RSSubThreadRCDTest, TestSize.Level1)
{
    auto context = new RenderContext();
    auto threadRcd = &(RSSingleton<RSSubThreadRCD>::GetInstance());
    threadRcd->Start(nullptr);
    threadRcd->Start(context);
    delete context;
    context = nullptr;
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
    rcdInstance.LoadConfigFile();
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
    rcdInstance.Init();
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
    rcdInstance.Init();

    ScreenRotation curOrientation = ScreenRotation::ROTATION_0;
    rcdInstance.UpdateOrientationStatus(curOrientation);

    int notchStatus = WINDOW_NOTCH_DEFAULT;
    rcdInstance.UpdateNotchStatus(notchStatus);

    uint32_t width = 1344;
    uint32_t height = 2772;
    rcdInstance.UpdateDisplayParameter(width, height);

    std::map<std::string, bool> updateFlag = {
        {"display", true},
        {"notch", true},
        {"orientation", true}
    };
    rcdInstance.UpdateParameter(updateFlag);
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
    rcdInstance.Init();

    ScreenRotation curOrientation = ScreenRotation::ROTATION_0;
    rcdInstance.UpdateOrientationStatus(curOrientation);

    int notchStatus = WINDOW_NOTCH_DEFAULT;
    rcdInstance.UpdateNotchStatus(notchStatus);

    uint32_t width = 1344;
    uint32_t height = 2772;
    rcdInstance.UpdateDisplayParameter(width, height);

#ifndef USE_ROSEN_DRAWING
    std::unique_ptr<SkCanvas> skCanvas = std::make_unique<SkCanvas>(width, height);
    std::shared_ptr<RSPaintFilterCanvas> canvas = std::make_shared<RSPaintFilterCanvas>(skCanvas.get());
#else
    std::unique_ptr<Drawing::Canvas> drawingCanvas = std::make_unique<Drawing::Canvas>(width, height);
    std::shared_ptr<RSPaintFilterCanvas> canvas = std::make_shared<RSPaintFilterCanvas>(drawingCanvas.get());
#endif
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
#ifndef USE_ROSEN_DRAWING
    sk_sp<SkImage> imgBottomPortrait;
    SkBitmap bitmapBottomPortrait;
#else
    std::shared_ptr<Drawing::Image> imgBottomPortrait;
    Drawing::Bitmap bitmapBottomPortrait;
#endif
    const char* path = "port_down.png";

    auto& rcdInstance = RSSingleton<RoundCornerDisplay>::GetInstance();
    rcdInstance.Init();

    rcdInstance.LoadImg(path, imgBottomPortrait);
    if (imgBottomPortrait == nullptr) {
        std::cout << "RSRoundCornerDisplayTest: current os less rcd source" << std::endl;
        return;
    }
    rcdInstance.DecodeBitmap(imgBottomPortrait, bitmapBottomPortrait);
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
    rcdInstance.Init();

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

    uint32_t width = 1344;
    uint32_t height = 2772;
    rcdInstance.UpdateDisplayParameter(width, height);

    rcdInstance.RcdChooseTopResourceType();

    rcdInstance.RcdChooseRSResource();
    rcdInstance.RcdChooseHardwareResource();
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
    rcdInstance.Init();
    rcdInstance.RunHardwareTask(
        []() {
            std::cout << "do RSRoundCornerDisplayTest.RunHardwareTask" << std::endl;
        }
    );
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
#ifndef USE_ROSEN_DRAWING
    sk_sp<SkImage> imgBottomPortrait;
    SkBitmap bitmapBottomPortrait;
#else
    std::shared_ptr<Drawing::Image> imgBottomPortrait;
    Drawing::Bitmap bitmapBottomPortrait;
#endif
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
    rcdCfg.Load(rs_rcd::PATH_CONFIG_FILE);
    rs_rcd::LCDModel* lcdModel = rcdCfg.GetLcdModel(rs_rcd::ATTR_DEFAULT);
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
    if (bottomSurfaceNode == nullptr) {
        std::cout << "RSRoundCornerDisplayTest: current os less bottomSurfaceNode source" << std::endl;
        return;
    }
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
#ifndef USE_ROSEN_DRAWING
    sk_sp<SkImage> imgBottomPortrait;
    SkBitmap bitmapBottomPortrait;
#else
    std::shared_ptr<Drawing::Image> imgBottomPortrait;
    Drawing::Bitmap bitmapBottomPortrait;
#endif
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
    rcdCfg.Load(rs_rcd::PATH_CONFIG_FILE);
    rs_rcd::LCDModel* lcdModel = rcdCfg.GetLcdModel(rs_rcd::ATTR_DEFAULT);
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
    rcdCfg.Load("NG_PATH_CONFIG_FILE");
    rcdCfg.Load(rs_rcd::PATH_CONFIG_FILE);
    auto invalidLcd = rcdCfg.GetLcdModel("invalideName");
    rs_rcd::RCDConfig::PrintParseLcdModel(invalidLcd);
    auto defaultLcd = rcdCfg.GetLcdModel(rs_rcd::ATTR_DEFAULT);
    rs_rcd::RCDConfig::PrintParseLcdModel(defaultLcd);

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
    rcdCfg.Load(rs_rcd::PATH_CONFIG_FILE);
    auto defaultLcd = rcdCfg.GetLcdModel(rs_rcd::ATTR_DEFAULT);
    if (defaultLcd == nullptr) {
        std::cout << "OS less lcdModel resource" << std::endl;
        return;
    }
    defaultLcd->GetRog(0, 0);
    defaultLcd->GetHardwareComposerConfig();
    defaultLcd->GetSideRegionConfig();
    defaultLcd->GetSurfaceConfig();

    xmlNodePtr xmlptr = nullptr;
    defaultLcd->ReadXmlNode(xmlptr);
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
    cfg.ReadXmlNode(xmlptr);
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
    cfg.ReadXmlNode(xmlptr, "ngAttr");
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
    cfg.ReadXmlNode(xmlptr);
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
    cfg.ReadXmlNode(xmlptr);
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
    cfg.ReadXmlNode(xmlptr);
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
    cfg.ReadXmlNode(xmlptr);
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
    cfg.ReadXmlNode(xmlptr);
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
}

/*
 * @tc.name: XMLReader
 * @tc.desc: Test RSRoundCornerDisplayTest.XMLReader
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, XMLReader, TestSize.Level1)
{
    rs_rcd::XMLReader reader;
    auto ngResult = reader.Init("nofiles");
    ASSERT_NE(ngResult, true);
    auto okResult = reader.Init(rs_rcd::PATH_CONFIG_FILE);
    if (okResult == false) {
        std::cout << "OS less roundcorner resource" << std::endl;
    }
    reader.ReadNode({"a", "b"});
    reader.Read({"a", "b"});

    xmlNodePtr nodePtr = nullptr;
    rs_rcd::XMLReader::ReadAttrStr(nodePtr, std::string("a"));
    rs_rcd::XMLReader::ReadAttrInt(nodePtr, std::string("a"));
    rs_rcd::XMLReader::ReadAttrFloat(nodePtr, std::string("a"));
    rs_rcd::XMLReader::ReadAttrBool(nodePtr, std::string("a"));
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
    info.GetFrameOffsetX();
    info.GetFrameOffsetY();
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
        rcdRenderNode.GetRSSurface();
        auto comsumer = rcdRenderNode.GetConsumerListener();
        rcdRenderNode.CreateSurface(comsumer);
        rcdRenderNode.SetRcdBufferSize(0);
        rcdRenderNode.SetRcdBufferHeight(0);
        rcdRenderNode.SetRcdBufferWidth(0);
        rcdRenderNode.SetHardwareResourceToBuffer();
        rcdRenderNode.PrepareHardwareResourceBuffer(nullptr);
        rs_rcd::RoundCornerLayer layer;
        rcdRenderNode.PrepareHardwareResourceBuffer(&layer);
        rcdRenderNode.SetRcdBufferSize(10);
        rcdRenderNode.SetRcdBufferHeight(20);
        rcdRenderNode.SetRcdBufferWidth(100);
        rcdRenderNode.SetHardwareResourceToBuffer();
        rcdRenderNode.PrepareHardwareResourceBuffer(nullptr);
        rcdRenderNode.PrepareHardwareResourceBuffer(&layer);
    }
}

}
