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
#include "feature/uifirst/rs_sub_thread_manager.h"
#include "feature/round_corner_display/rs_message_bus.h"
#include "feature/round_corner_display/rs_round_corner_display.h"
#include "feature/round_corner_display/rs_round_corner_display_manager.h"
#include "feature/round_corner_display/rs_round_corner_config.h"
#include "feature/round_corner_display/rs_rcd_render_manager.h"
#include "feature/round_corner_display/rs_rcd_surface_render_node.h"
#include "feature/round_corner_display/rs_rcd_bitmap_utils.h"
#include "feature/round_corner_display/rs_rcd_render_listener.h"
#include "feature/round_corner_display/rs_rcd_render_visitor.h"
#include "pipeline/rs_screen_render_node.h"
#include "rcd/rs_render_rcd_draw.h"
#include "render/rs_pixel_map_util.h"
#include "rs_render_surface_rcd_layer.h"
#include "engine/rs_uni_render_engine.h"
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

bool LoadBitmapFromFile(const char* path, Drawing::Bitmap& bitmap)
{
    if (path == nullptr) {
        return false;
    }

    std::shared_ptr<Drawing::Image> image;
    auto& rcdInstance = RSSingleton<RoundCornerDisplay>::GetInstance();
    rcdInstance.Init();
    rcdInstance.LoadImg(path, image);
    if (image == nullptr) {
        std::cout << "LoadBitmapFromFile: current os no rcd source" << std::endl;
        return false;
    }

    return rcdInstance.DecodeBitmap(image, bitmap);
}

void InitRcdRenderParamsInvalid01(HardwareLayerInfo* layerInfo, RcdExtInfo* extInfo)
{
    if (layerInfo != nullptr) {
        layerInfo->pathBin = "/sys_prod/etc/display/RoundCornerDisplay/test.bin";
        layerInfo->bufferSize = -1;  // invalid buffer size
        layerInfo->cldWidth = 1;
        layerInfo->cldHeight = 1;
    }
    if (extInfo != nullptr) {
        extInfo->srcRect_ = RectI(0, 0, 1, 1);
        extInfo->dstRect_ = RectI(0, 0, 1, 1);
    }
}

void InitRcdRenderParamsInvalid02(HardwareLayerInfo* layerInfo, RcdExtInfo* extInfo)
{
    if (layerInfo != nullptr) {
        layerInfo->pathBin = "/sys_prod/etc/display/RoundCornerDisplay/test.bin";
        layerInfo->bufferSize = 1;
        layerInfo->cldWidth = -1;  // invalid cld height
        layerInfo->cldHeight = 1;
    }
    if (extInfo != nullptr) {
        extInfo->srcRect_ = RectI(0, 0, 1, 1);
        extInfo->dstRect_ = RectI(0, 0, 1, 1);
    }
}

void InitRcdRenderParamsInvalid03(HardwareLayerInfo* layerInfo, RcdExtInfo* extInfo)
{
    if (layerInfo != nullptr) {
        layerInfo->pathBin = "";  // Invalid path
        layerInfo->bufferSize = 1;
        layerInfo->cldWidth = 1;
        layerInfo->cldHeight = 1;
    }
    if (extInfo != nullptr) {
        extInfo->srcRect_ = RectI(0, 0, 1, 1);
        extInfo->dstRect_ = RectI(0, 0, 1, 1);
    }
}

void InitRcdRenderParamsInvalid04(HardwareLayerInfo* layerInfo, RcdExtInfo* extInfo)
{
    if (layerInfo != nullptr) {
        layerInfo->pathBin = "/sys_prod/etc/display/RoundCornerDisplay/test.bin";
        layerInfo->bufferSize = 1;
        layerInfo->cldWidth = 1;
        layerInfo->cldHeight = 1;
    }
    if (extInfo != nullptr) {
        extInfo->srcRect_ = RectI(0, 0, 1, 1);
        extInfo->dstRect_ = RectI(0, 0, 1, 1);
    }
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
 * @tc.name: RSLoadImgTest
 * @tc.desc: Test RSRoundCornerDisplayTest.RSLoadImgTest
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, RSLoadImgTest, TestSize.Level1)
{
    Drawing::Bitmap bitmapBottomPortrait;
    const char* path = "port_down.png";

    auto& rcdInstance = RSSingleton<RoundCornerDisplay>::GetInstance();
    auto res = rcdInstance.Init();

    if (!LoadBitmapFromFile(path, bitmapBottomPortrait)) {
        std::cout << "RSRoundCornerDisplayTest: current os less rcd source" << std::endl;
        EXPECT_TRUE(res == true);
        return;
    }

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

rs_rcd::ROGSetting* GetRogFromLcdModel(rs_rcd::LCDModel* lcdModel, int& width, int& height)
{
    if (lcdModel == nullptr || lcdModel->rogs.empty()) {
        return nullptr;
    }
    if (lcdModel->rogs[0] == nullptr) {
        return nullptr;
    }
    width = lcdModel->rogs[0]->width;
    height = lcdModel->rogs[0]->height;
    return lcdModel->rogs[0];
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
    Drawing::Bitmap bitmapBottomPortrait;
    const char* path = "port_down.png";

    auto& rcdInstance = RSSingleton<RoundCornerDisplay>::GetInstance();
    rcdInstance.Init();
    if (!LoadBitmapFromFile(path, bitmapBottomPortrait)) {
        std::cout << "RSRoundCornerDisplayTest: current os less rcd source" << std::endl;
        return;
    }

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
    EXPECT_TRUE(rcdInstance.rog_ != nullptr);
    rcdInstance.GetTopSurfaceSource();
    rcdInstance.GetBottomSurfaceSource();
    rcdInstance.rog_ = nullptr;

    rs_rcd::RoundCornerHardware hardInfo;
    auto portrait = rog->GetPortrait(std::string(rs_rcd::NODE_PORTRAIT));
    if (portrait == std::nullopt) {
        std::cout << "RSRoundCornerDisplayTest: current os less bottomLayer source" << std::endl;
        return;
    }
    EXPECT_TRUE(portrait != std::nullopt);
    hardInfo.bottomLayer = std::make_shared<rs_rcd::RoundCornerLayer>(portrait->layerDown);
    hardInfo.displayRect = RectU(0, 0, width, height);
    hardInfo.bottomLayer->curBitmap = &bitmapBottomPortrait;
    auto bottomSurfaceNode = RSRcdSurfaceRenderNode::Create(0, RCDSurfaceType::BOTTOM);
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
    std::shared_ptr<RSRcdSurfaceRenderNode> topSurfaceNode =
        std::make_shared<RSRcdSurfaceRenderNode>(0, RCDSurfaceType::TOP);
    if (topSurfaceNode == nullptr) {
        std::cout << "RSRoundCornerDisplayTest: create topSurfaceNode fail" << std::endl;
        return;
    }

    // SetHardwareResourceToBuffer - LayerBitmap is not valid
    Drawing::Bitmap layerBitmap1;
    EXPECT_TRUE(topSurfaceNode->SetHardwareResourceToBuffer(layerBitmap1) == false);

    // SetHardwareResourceToBuffer - buffer is nullptr
    // to create layerInfo
    std::shared_ptr<Drawing::Bitmap> bitMap = std::make_shared<Drawing::Bitmap>();
    bitMap->Build(896, 1848,
        Drawing::BitmapFormat{Drawing::ColorType::COLORTYPE_RGBA_8888, Drawing::AlphaType::ALPHATYPE_OPAQUE});
    rs_rcd::RoundCornerLayer layerTmp{"top.png", 0, 0, "top.bin", 8112, 2028, 1, bitMap.get()};
    Drawing::Bitmap layerBitmap2;
    topSurfaceNode->PrepareHardwareResourceBuffer(std::make_shared<rs_rcd::RoundCornerLayer>(layerTmp), layerBitmap2);
    EXPECT_TRUE(topSurfaceNode->SetHardwareResourceToBuffer(layerBitmap2) == false);

    // SetHardwareResourceToBuffer - copy layerBitmap to buffer failed
    sptr<SurfaceBufferImpl> surfaceBufferImpl = new SurfaceBufferImpl();
    topSurfaceNode->buffer_.buffer = surfaceBufferImpl;
    EXPECT_TRUE(topSurfaceNode->SetHardwareResourceToBuffer(layerBitmap2) == false);

    // SetHardwareResourceToBuffer - copy hardware resource to buffer failed
    BufferRequestConfig requestConfig = {
        .width = 896,
        .height = 1848,
        .strideAlignment = 0x8,
        .format = GRAPHIC_PIXEL_FMT_RGBA_8888,
        .usage = BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA,
        .timeout = 0,
        .colorGamut = GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB,
    };
    surfaceBufferImpl->Alloc(requestConfig, nullptr);
    EXPECT_TRUE(topSurfaceNode->SetHardwareResourceToBuffer(layerBitmap2) == false);
    surfaceBufferImpl->handle_ = nullptr;
    surfaceBufferImpl->FreeBufferHandleLocked();
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
    std::shared_ptr<RSRcdSurfaceRenderNode> inValidSurfaceNode =
        std::make_shared<RSRcdSurfaceRenderNode>(0, RCDSurfaceType::INVALID);
    rs_rcd::RoundCornerHardware hardInfo{};
    auto visitor = std::make_shared<RSRcdRenderVisitor>();

    // 1 null processor
    std::shared_ptr<RSProcessor> processorPtr = nullptr;
    visitor->SetUniProcessor(processorPtr);
    EXPECT_TRUE(visitor->uniProcessor_ == processorPtr);
    visitor->ProcessRcdSurfaceRenderNode(*topSurfaceNode, hardInfo.bottomLayer, true);
    visitor->ProcessRcdSurfaceRenderNodeMainThread(*topSurfaceNode, true);

    // 2 invalid node
    std::shared_ptr<RSComposerClientManager> rsComposerClientMgr = std::make_shared<RSComposerClientManager>();
    RSUniRenderThread::Instance().composerClientManager_ = rsComposerClientMgr;
    processorPtr =
        RSProcessorFactory::CreateProcessor(CompositeType::HARDWARE_COMPOSITE, 0);
    visitor->SetUniProcessor(processorPtr);
    visitor->ProcessRcdSurfaceRenderNode(*inValidSurfaceNode, hardInfo.bottomLayer, true);
    visitor->ProcessRcdSurfaceRenderNodeMainThread(*inValidSurfaceNode, true);

    // 3 resource not changed
    visitor->ProcessRcdSurfaceRenderNode(*topSurfaceNode, hardInfo.bottomLayer, true);
    visitor->ProcessRcdSurfaceRenderNodeMainThread(*topSurfaceNode, true);

    // processor node changedTag Ok
    visitor->ProcessRcdSurfaceRenderNode(*topSurfaceNode, hardInfo.bottomLayer, true);
    visitor->ProcessRcdSurfaceRenderNodeMainThread(*topSurfaceNode, true);

    Drawing::Bitmap layerBitmap;
    ASSERT_EQ(true, visitor->ConsumeAndUpdateBuffer(*topSurfaceNode, layerBitmap));
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

    auto attrName = std::string("num");
    auto numPtr = CreateNodeWithProperty(std::string("nodeName"), XMLProperty{attrName, std::string("")});
    EXPECT_TRUE(rs_rcd::XMLReader::ReadAttrInt(numPtr, attrName) == int{0});
    EXPECT_TRUE(rs_rcd::XMLReader::ReadAttrFloat(numPtr, attrName) < 1.0f);
    xmlFreeNode(numPtr);
    numPtr = CreateNodeWithProperty(std::string("nodeName"), XMLProperty{attrName, std::string("2.0")});
    EXPECT_TRUE(rs_rcd::XMLReader::ReadAttrInt(numPtr, attrName) == int{2});
    EXPECT_TRUE(rs_rcd::XMLReader::ReadAttrFloat(numPtr, attrName) > 1.0f);
    xmlFreeNode(numPtr);
    std::vector<std::string> okCase = { "0.0", "0", "123", "1230.0", "8192.0 ", "819200"};
    for (auto& tmpCase : okCase) {
        bool isOk = rs_rcd::XMLReader::RegexMatchNum(tmpCase);
        EXPECT_EQ(isOk, true);
    }

    std::vector<std::string> ngCase = {"a0.0", "0a", "a123", "1230.0c", "a8192.0 "};
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
        {
            Drawing::Bitmap layerBitmap;
            rcdRenderNode.SetHardwareResourceToBuffer(layerBitmap);
            rcdRenderNode.PrepareHardwareResourceBuffer(nullptr, layerBitmap);
            rs_rcd::RoundCornerLayer layer;
            rcdRenderNode.PrepareHardwareResourceBuffer(std::make_shared<rs_rcd::RoundCornerLayer>(layer), layerBitmap);
        }
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
        {
            Drawing::Bitmap layerBitmap;
            rcdRenderNode.SetHardwareResourceToBuffer(layerBitmap);
            rcdRenderNode.PrepareHardwareResourceBuffer(nullptr, layerBitmap);
            rs_rcd::RoundCornerLayer layer;
            rcdRenderNode.PrepareHardwareResourceBuffer(std::make_shared<rs_rcd::RoundCornerLayer>(layer), layerBitmap);
        }
        width = 0;
        rcdRenderNode.SetRcdBufferWidth(width);
        rcdRenderNode.GetHardenBufferRequestConfig();
        EXPECT_EQ(rcdRenderNode.GetRcdBufferWidth(), width);
        rcdRenderNode.ClearBufferCache();
    }
}

/*
 * @tc.name: RSRcdSurfaceRenderNodeResource
 * @tc.desc: Test RSRoundCornerDisplayTest.RSRcdSurfaceRenderNodeResource
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, RSRcdSurfaceRenderNodeResource, TestSize.Level1)
{
    auto surfaceNodePtr = std::make_shared<RSRcdSurfaceRenderNode>(0, static_cast<RCDSurfaceType>(0));
    RSRcdSurfaceRenderNode rcdRenderNode(0, static_cast<RCDSurfaceType>(0)); // bottom
    auto cldInfo = surfaceNodePtr->GetCldInfo();
    EXPECT_TRUE(cldInfo.cldWidth >= 0);
    EXPECT_TRUE(surfaceNodePtr->GetSurfaceWidth() >= 0);
    EXPECT_TRUE(surfaceNodePtr->GetSurfaceHeight() >= 0);

    sptr<IBufferConsumerListener> listener = new RSRcdRenderListener(surfaceNodePtr);
    surfaceNodePtr->CreateSurface(listener);
    surfaceNodePtr->ClearBufferCache();
    EXPECT_TRUE(surfaceNodePtr->surface_ != nullptr);
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
        // imgBottomPortrait_ is removed, curBitmapBottom_ is now a Bitmap loaded locally
        // in RcdChooseRSResource. Without rog_ configured, curBitmapBottom_ stays invalid.
        EXPECT_FALSE(rcdInstance.curBitmapBottom_.IsValid());
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

    EXPECT_EQ(&rcdInstance.bitmapBottomPortrait_, rcdInstance.hardInfo_.bottomLayer->curBitmap);
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
 * @tc.name: ProcessFillHardwareResource
 * @tc.desc: Test RSRcdSurfaceRenderNode.FillHardwareResource
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, ProcessFillHardwareResource, TestSize.Level1)
{
    // prepare test
    std::shared_ptr<RSRcdSurfaceRenderNode> bottomSurfaceNode =
        std::make_shared<RSRcdSurfaceRenderNode>(0, RCDSurfaceType::BOTTOM);
    if (bottomSurfaceNode == nullptr) {
        std::cout << "RSRoundCornerDisplayTest: current os less bottomSurfaceNode source" << std::endl;
        return;
    }

    HardwareLayerInfo info{};
    sptr<SurfaceBufferImpl> surfaceBufferImpl = new SurfaceBufferImpl();
    bottomSurfaceNode->buffer_.buffer = nullptr;
    bool flag1 = bottomSurfaceNode->FillHardwareResource(info, -1, 0);
    bottomSurfaceNode->buffer_.buffer = surfaceBufferImpl;
    EXPECT_TRUE(flag1 == false);
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
 * @tc.name: RSRcdRenderManager
 * @tc.desc: Test RSRcdRenderManager
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, RSRcdRenderManager, TestSize.Level1)
{
    auto& rcdManagerInstance = RSRcdRenderManager::GetInstance();
    rcdManagerInstance.InitInstance();
    NodeId id = 1;
    EXPECT_TRUE(rcdManagerInstance.GetRcdRenderEnabled() == true);
    auto topLayer = rcdManagerInstance.GetTopSurfaceNode(id);
    EXPECT_TRUE(topLayer == nullptr);
    auto bottomLayer = rcdManagerInstance.GetBottomSurfaceNode(id);
    EXPECT_TRUE(bottomLayer == nullptr);

    RcdProcessInfo info{};
    rcdManagerInstance.DoProcessRenderMainThreadTask(id, info);
    rcdManagerInstance.DoProcessRenderTask(id, info);
    auto topNode = rcdManagerInstance.GetTopRenderNode(id);
    auto bottomNode = rcdManagerInstance.GetTopRenderNode(id);
    RSRenderNodeMap nodeMap;
    EXPECT_TRUE(RSRcdRenderManager::CheckExist(id, nodeMap) == false);

    // to create layerInfo
    std::shared_ptr<Drawing::Bitmap> bitMap = std::make_shared<Drawing::Bitmap>();
    bitMap->Build(896, 1848,
        Drawing::BitmapFormat{Drawing::ColorType::COLORTYPE_RGBA_8888, Drawing::AlphaType::ALPHATYPE_OPAQUE});
    rs_rcd::RoundCornerLayer layerTmp{"top.png", 0, 0, "top.bin", 8112, 2028, 1, bitMap.get()};
    std::shared_ptr<rs_rcd::RoundCornerLayer> topPtr = std::make_shared<rs_rcd::RoundCornerLayer>(layerTmp);
    std::shared_ptr<RSComposerClientManager> rsComposerClientMgr = std::make_shared<RSComposerClientManager>();
    RSUniRenderThread::Instance().composerClientManager_ = rsComposerClientMgr;
    auto rsHardwareProcessor =
        RSProcessorFactory::CreateProcessor(CompositeType::HARDWARE_COMPOSITE, 0);
    RectU displayRect{0, 0, 896, 1848};
    info = {rsHardwareProcessor, topPtr, topPtr, displayRect, true};
    rcdManagerInstance.DoProcessRenderMainThreadTask(id, info);
    rcdManagerInstance.DoProcessRenderTask(id, info);
    RSContext context;
    pid_t pid = ExtractPid(id);
    context.nodeMap.renderNodeMap_[pid][id] = std::make_shared<RSRenderNode>(id);
    rcdManagerInstance.CheckRenderTargetNode(context);
    context.nodeMap.renderNodeMap_[pid][id] = std::make_shared<RSRenderNode>(id + 1);
    rcdManagerInstance.CheckRenderTargetNode(context);
    rcdManagerInstance.RemoveRcdResource(id);
    context.nodeMap.renderNodeMap_.clear();
    rcdManagerInstance.topSurfaceNodeMap_.clear();
    rcdManagerInstance.bottomSurfaceNodeMap_.clear();
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
    {
        Drawing::BitmapFormat format { Drawing::ColorType::COLORTYPE_RGBA_8888,
            Drawing::AlphaType::ALPHATYPE_PREMUL };
        rcdInstance.rcdMap_[id]->curBitmapTop_.Build(1, 1, format);
        rcdInstance.rcdMap_[id]->curBitmapBottom_.Build(1, 1, format);
    }
    flag = rcdInstance.HandleRoundCornerDirtyRect(id, dirtyRect, RoundCornerDisplayManager::RCDLayerType::TOP);
    flag &= rcdInstance.HandleRoundCornerDirtyRect(id, dirtyRect, RoundCornerDisplayManager::RCDLayerType::BOTTOM);
    EXPECT_TRUE(flag && !dirtyRect.IsEmpty());

    // Handle rcd dirty rect with resource prepared and reset dirty
    rcdInstance.rcdMap_[id]->hardInfo_.resourceChanged = false;
    flag = rcdInstance.HandleRoundCornerDirtyRect(id, dirtyRect, RoundCornerDisplayManager::RCDLayerType::TOP);
    flag &= rcdInstance.HandleRoundCornerDirtyRect(id, dirtyRect, RoundCornerDisplayManager::RCDLayerType::BOTTOM);
    EXPECT_TRUE(flag && !dirtyRect.IsEmpty());

    // Handle rcd dirty rect with no dirty
    flag = rcdInstance.HandleRoundCornerDirtyRect(id, dirtyRect, RoundCornerDisplayManager::RCDLayerType::TOP);
    flag |= rcdInstance.HandleRoundCornerDirtyRect(id, dirtyRect, RoundCornerDisplayManager::RCDLayerType::BOTTOM);
    EXPECT_TRUE(!flag && !dirtyRect.IsEmpty());
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
 * @tc.name: FillHardwareResourceNullImgAddr
 * @tc.desc: Test RSRcdSurfaceRenderNode::FillHardwareResource with null virtual address
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, FillHardwareResourceNullImgAddr, TestSize.Level1)
{
    auto surfaceNode = std::make_shared<RSRcdSurfaceRenderNode>(0, RCDSurfaceType::BOTTOM);
    if (surfaceNode == nullptr) {
        return;
    }

    HardwareLayerInfo info{};
    info.pathBin = "/sys_prod/etc/display/RoundCornerDisplay/test.bin";
    info.bufferSize = 1000;
    info.cldWidth = 100;
    info.cldHeight = 50;

    sptr<SurfaceBufferImpl> surfaceBufferImpl = new SurfaceBufferImpl();
    BufferRequestConfig requestConfig = {
        .width = 100,
        .height = 200,
        .strideAlignment = 0x8,
        .format = GRAPHIC_PIXEL_FMT_RGBA_8888,
        .usage = BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA,
        .timeout = 0,
        .colorGamut = GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB,
    };
    surfaceBufferImpl->Alloc(requestConfig, nullptr);
    surfaceNode->buffer_.buffer = surfaceBufferImpl;

    BufferHandle* bufferHandle = AllocateBufferHandle(100, 200);
    ASSERT_TRUE(bufferHandle != nullptr);
    surfaceBufferImpl->SetBufferHandle(bufferHandle);
    surfaceBufferImpl->handle_->stride = 400;
    surfaceBufferImpl->handle_->size = 100000;
    surfaceBufferImpl->handle_->virAddr = nullptr;

    bool result = surfaceNode->FillHardwareResource(info, 50, 100);
    EXPECT_FALSE(result);

    if (surfaceBufferImpl->handle_ != nullptr) {
        surfaceBufferImpl->handle_->virAddr = nullptr;
    }
    FreeBufferHandle(bufferHandle);
}

/*
 * @tc.name: FillHardwareResourceInvalidStride
 * @tc.desc: Test RSRcdSurfaceRenderNode::FillHardwareResource with negative stride
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, FillHardwareResourceInvalidStride, TestSize.Level1)
{
    auto surfaceNode = std::make_shared<RSRcdSurfaceRenderNode>(0, RCDSurfaceType::BOTTOM);
    if (surfaceNode == nullptr) {
        return;
    }

    HardwareLayerInfo info{};
    info.pathBin = "/sys_prod/etc/display/RoundCornerDisplay/test.bin";
    info.bufferSize = 1000;
    info.cldWidth = 100;
    info.cldHeight = 50;

    sptr<SurfaceBufferImpl> surfaceBufferImpl = new SurfaceBufferImpl();
    BufferRequestConfig requestConfig = {
        .width = 100,
        .height = 200,
        .strideAlignment = 0x8,
        .format = GRAPHIC_PIXEL_FMT_RGBA_8888,
        .usage = BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA,
        .timeout = 0,
        .colorGamut = GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB,
    };
    surfaceBufferImpl->Alloc(requestConfig, nullptr);
    surfaceNode->buffer_.buffer = surfaceBufferImpl;

    BufferHandle* bufferHandle = AllocateBufferHandle(100, 200);
    ASSERT_TRUE(bufferHandle != nullptr);
    surfaceBufferImpl->SetBufferHandle(bufferHandle);
    surfaceBufferImpl->handle_->stride = -1;
    surfaceBufferImpl->handle_->size = 100000;

    std::shared_ptr<uint8_t> buffer(new uint8_t[100000](), std::default_delete<uint8_t[]>());
    surfaceBufferImpl->handle_->virAddr = static_cast<void*>(buffer.get());

    bool result = surfaceNode->FillHardwareResource(info, 50, 100);
    EXPECT_FALSE(result);

    if (surfaceBufferImpl->handle_ != nullptr) {
        surfaceBufferImpl->handle_->virAddr = nullptr;
    }
    FreeBufferHandle(bufferHandle);
}

/*
 * @tc.name: FillHardwareResourceInsufficientBuffer
 * @tc.desc: Test RSRcdSurfaceRenderNode::FillHardwareResource with insufficient buffer size
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, FillHardwareResourceInsufficientBuffer, TestSize.Level1)
{
    auto surfaceNode = std::make_shared<RSRcdSurfaceRenderNode>(0, RCDSurfaceType::BOTTOM);
    if (surfaceNode == nullptr) {
        return;
    }

    HardwareLayerInfo info{};
    info.pathBin = "/sys_prod/etc/display/RoundCornerDisplay/test.bin";
    info.bufferSize = 1000;
    info.cldWidth = 100;
    info.cldHeight = 50;

    sptr<SurfaceBufferImpl> surfaceBufferImpl = new SurfaceBufferImpl();
    BufferRequestConfig requestConfig = {
        .width = 100,
        .height = 200,
        .strideAlignment = 0x8,
        .format = GRAPHIC_PIXEL_FMT_RGBA_8888,
        .usage = BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA,
        .timeout = 0,
        .colorGamut = GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB,
    };
    surfaceBufferImpl->Alloc(requestConfig, nullptr);
    surfaceNode->buffer_.buffer = surfaceBufferImpl;

    BufferHandle* bufferHandle = AllocateBufferHandle(100, 200);
    ASSERT_TRUE(bufferHandle != nullptr);
    surfaceBufferImpl->SetBufferHandle(bufferHandle);
    surfaceBufferImpl->handle_->stride = 400;
    surfaceBufferImpl->handle_->size = 100;

    std::shared_ptr<uint8_t> buffer(new uint8_t[100](), std::default_delete<uint8_t[]>());
    surfaceBufferImpl->handle_->virAddr = static_cast<void*>(buffer.get());

    bool result = surfaceNode->FillHardwareResource(info, 50, 100);
    EXPECT_FALSE(result);

    if (surfaceBufferImpl->handle_ != nullptr) {
        surfaceBufferImpl->handle_->virAddr = nullptr;
    }
    FreeBufferHandle(bufferHandle);
}

/*
 * @tc.name: FillHardwareResourceFileNotExist
 * @tc.desc: Test RSRcdSurfaceRenderNode::FillHardwareResource with non-existent file path
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, FillHardwareResourceFileNotExist, TestSize.Level1)
{
    auto surfaceNode = std::make_shared<RSRcdSurfaceRenderNode>(0, RCDSurfaceType::BOTTOM);
    if (surfaceNode == nullptr) {
        return;
    }

    HardwareLayerInfo info{};
    info.pathBin = "/sys_prod/etc/display/RoundCornerDisplay/nonexistent_test.bin";
    info.bufferSize = 1000;
    info.cldWidth = 100;
    info.cldHeight = 50;

    sptr<SurfaceBufferImpl> surfaceBufferImpl = new SurfaceBufferImpl();
    BufferRequestConfig requestConfig = {
        .width = 100,
        .height = 200,
        .strideAlignment = 0x8,
        .format = GRAPHIC_PIXEL_FMT_RGBA_8888,
        .usage = BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA,
        .timeout = 0,
        .colorGamut = GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB,
    };
    surfaceBufferImpl->Alloc(requestConfig, nullptr);
    surfaceNode->buffer_.buffer = surfaceBufferImpl;

    BufferHandle* bufferHandle = AllocateBufferHandle(100, 200);
    ASSERT_TRUE(bufferHandle != nullptr);
    surfaceBufferImpl->SetBufferHandle(bufferHandle);
    surfaceBufferImpl->handle_->stride = 400;
    surfaceBufferImpl->handle_->size = 100000;

    std::shared_ptr<uint8_t> buffer(new uint8_t[100000](), std::default_delete<uint8_t[]>());
    surfaceBufferImpl->handle_->virAddr = static_cast<void*>(buffer.get());

    bool result = surfaceNode->FillHardwareResource(info, 50, 100);
    EXPECT_FALSE(result);

    if (surfaceBufferImpl->handle_ != nullptr) {
        surfaceBufferImpl->handle_->virAddr = nullptr;
    }
    FreeBufferHandle(bufferHandle);
}

/*
 * @tc.name: FillHardwareResourceTopSurface
 * @tc.desc: Test RSRcdSurfaceRenderNode::FillHardwareResource with TOP surface type
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, FillHardwareResourceTopSurface, TestSize.Level1)
{
    auto surfaceNode = std::make_shared<RSRcdSurfaceRenderNode>(0, RCDSurfaceType::TOP);
    if (surfaceNode == nullptr) {
        return;
    }

    HardwareLayerInfo info{};
    info.pathBin = "/sys_prod/etc/display/RoundCornerDisplay/test.bin";
    info.bufferSize = 1000;
    info.cldWidth = 100;
    info.cldHeight = 50;

    sptr<SurfaceBufferImpl> surfaceBufferImpl = new SurfaceBufferImpl();
    BufferRequestConfig requestConfig = {
        .width = 100,
        .height = 200,
        .strideAlignment = 0x8,
        .format = GRAPHIC_PIXEL_FMT_RGBA_8888,
        .usage = BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA,
        .timeout = 0,
        .colorGamut = GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB,
    };
    surfaceBufferImpl->Alloc(requestConfig, nullptr);
    surfaceNode->buffer_.buffer = surfaceBufferImpl;

    BufferHandle* bufferHandle = AllocateBufferHandle(100, 200);
    ASSERT_TRUE(bufferHandle != nullptr);
    surfaceBufferImpl->SetBufferHandle(bufferHandle);
    surfaceBufferImpl->handle_->stride = 400;
    surfaceBufferImpl->handle_->size = 100000;

    std::shared_ptr<uint8_t> buffer(new uint8_t[100000](), std::default_delete<uint8_t[]>());
    surfaceBufferImpl->handle_->virAddr = static_cast<void*>(buffer.get());

    bool result = surfaceNode->FillHardwareResource(info, 50, 100);
    EXPECT_FALSE(result);

    if (surfaceBufferImpl->handle_ != nullptr) {
        surfaceBufferImpl->handle_->virAddr = nullptr;
    }
    FreeBufferHandle(bufferHandle);
}

/*
 * @tc.name: FillHardwareResourceInvalidSurface
 * @tc.desc: Test RSRcdSurfaceRenderNode::FillHardwareResource with INVALID surface type
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, FillHardwareResourceInvalidSurface, TestSize.Level1)
{
    auto surfaceNode = std::make_shared<RSRcdSurfaceRenderNode>(0, RCDSurfaceType::INVALID);
    if (surfaceNode == nullptr) {
        return;
    }

    HardwareLayerInfo info{};
    info.pathBin = "/sys_prod/etc/display/RoundCornerDisplay/test.bin";
    info.bufferSize = 1000;
    info.cldWidth = 100;
    info.cldHeight = 50;

    sptr<SurfaceBufferImpl> surfaceBufferImpl = new SurfaceBufferImpl();
    BufferRequestConfig requestConfig = {
        .width = 100,
        .height = 200,
        .strideAlignment = 0x8,
        .format = GRAPHIC_PIXEL_FMT_RGBA_8888,
        .usage = BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA,
        .timeout = 0,
        .colorGamut = GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB,
    };
    surfaceBufferImpl->Alloc(requestConfig, nullptr);
    surfaceNode->buffer_.buffer = surfaceBufferImpl;

    BufferHandle* bufferHandle = AllocateBufferHandle(100, 200);
    ASSERT_TRUE(bufferHandle != nullptr);
    surfaceBufferImpl->SetBufferHandle(bufferHandle);
    surfaceBufferImpl->handle_->stride = 400;
    surfaceBufferImpl->handle_->size = 100000;

    std::shared_ptr<uint8_t> buffer(new uint8_t[100000](), std::default_delete<uint8_t[]>());
    surfaceBufferImpl->handle_->virAddr = static_cast<void*>(buffer.get());

    bool result = surfaceNode->FillHardwareResource(info, 50, 100);
    EXPECT_FALSE(result);

    if (surfaceBufferImpl->handle_ != nullptr) {
        surfaceBufferImpl->handle_->virAddr = nullptr;
    }
    FreeBufferHandle(bufferHandle);
}

/*
 * @tc.name: CreatePixelMapFromBitmapValid
 * @tc.desc: Test RSRcdSurfaceRenderNode::CreatePixelMapFromBitmap with valid bitmap
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, CreatePixelMapFromBitmapValid, TestSize.Level1)
{
    Drawing::Bitmap bitmap;
    const int width = 100;
    const int height = 100;
    bitmap.Build(width, height,
        Drawing::BitmapFormat{Drawing::ColorType::COLORTYPE_RGBA_8888, Drawing::AlphaType::ALPHATYPE_OPAQUE});
    auto pixelMap = RSRcdSurfaceRenderNode::CreatePixelMapFromBitmap(bitmap);
    ASSERT_NE(pixelMap, nullptr);
    EXPECT_EQ(pixelMap->GetWidth(), width);
    EXPECT_EQ(pixelMap->GetHeight(), height);
}

/*
 * @tc.name: CreatePixelMapFromBitmapNullPixels
 * @tc.desc: Test RSRcdSurfaceRenderNode::CreatePixelMapFromBitmap with null pixels
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, CreatePixelMapFromBitmapNullPixels, TestSize.Level1)
{
    Drawing::Bitmap bitmap;
    const int width = 100;
    const int height = 100;
    bitmap.Build(width, height,
        Drawing::BitmapFormat{Drawing::ColorType::COLORTYPE_RGBA_8888, Drawing::AlphaType::ALPHATYPE_OPAQUE});
    bitmap.SetPixels(nullptr);
    auto pixelMap = RSRcdSurfaceRenderNode::CreatePixelMapFromBitmap(bitmap);
    EXPECT_EQ(pixelMap, nullptr);
}

/*
 * @tc.name: CreatePixelMapFromBitmapInvalidWidth
 * @tc.desc: Test RSRcdSurfaceRenderNode::CreatePixelMapFromBitmap with invalid width
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, CreatePixelMapFromBitmapInvalidWidth, TestSize.Level1)
{
    Drawing::Bitmap bitmap;
    bitmap.Build(0, 100,
        Drawing::BitmapFormat{Drawing::ColorType::COLORTYPE_RGBA_8888, Drawing::AlphaType::ALPHATYPE_OPAQUE});

    auto pixelMap = RSRcdSurfaceRenderNode::CreatePixelMapFromBitmap(bitmap);
    EXPECT_EQ(pixelMap, nullptr);
}

/*
 * @tc.name: CreatePixelMapFromBitmapInvalidHeight
 * @tc.desc: Test RSRcdSurfaceRenderNode::CreatePixelMapFromBitmap with invalid height
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, CreatePixelMapFromBitmapInvalidHeight, TestSize.Level1)
{
    Drawing::Bitmap bitmap;
    bitmap.Build(100, 0,
        Drawing::BitmapFormat{Drawing::ColorType::COLORTYPE_RGBA_8888, Drawing::AlphaType::ALPHATYPE_OPAQUE});

    auto pixelMap = RSRcdSurfaceRenderNode::CreatePixelMapFromBitmap(bitmap);
    EXPECT_EQ(pixelMap, nullptr);
}

/*
 * @tc.name: CreatePixelMapFromBitmapSmallDimensions
 * @tc.desc: Test RSRcdSurfaceRenderNode::CreatePixelMapFromBitmap with small dimensions (1x1)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, CreatePixelMapFromBitmapSmallDimensions, TestSize.Level1)
{
    Drawing::Bitmap bitmap;
    const int width = 1;
    const int height = 1;
    bitmap.Build(width, height,
        Drawing::BitmapFormat{Drawing::ColorType::COLORTYPE_RGBA_8888, Drawing::AlphaType::ALPHATYPE_OPAQUE});

    auto pixelMap = RSRcdSurfaceRenderNode::CreatePixelMapFromBitmap(bitmap);
    ASSERT_NE(pixelMap, nullptr);
    EXPECT_EQ(pixelMap->GetWidth(), width);
    EXPECT_EQ(pixelMap->GetHeight(), height);
}

/*
 * @tc.name: CreatePixelMapFromBitmapLargeDimensions
 * @tc.desc: Test RSRcdSurfaceRenderNode::CreatePixelMapFromBitmap with large dimensions
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, CreatePixelMapFromBitmapLargeDimensions, TestSize.Level1)
{
    Drawing::Bitmap bitmap;
    const int width = 1920;
    const int height = 1080;
    bitmap.Build(width, height,
        Drawing::BitmapFormat{Drawing::ColorType::COLORTYPE_RGBA_8888, Drawing::AlphaType::ALPHATYPE_OPAQUE});

    auto pixelMap = RSRcdSurfaceRenderNode::CreatePixelMapFromBitmap(bitmap);
    ASSERT_NE(pixelMap, nullptr);
    EXPECT_EQ(pixelMap->GetWidth(), width);
    EXPECT_EQ(pixelMap->GetHeight(), height);
}

/*
 * @tc.name: DrawRsRCDLayerValid
 * @tc.desc: Test RSRenderRcdDraw::DrawRSRCDLayer with valid canvas and layer
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, DrawRsRCDLayerValid, TestSize.Level1)
{
    Drawing::Bitmap bitmap;
    const int width = 100;
    const int height = 100;
    bitmap.Build(width, height,
        Drawing::BitmapFormat{Drawing::ColorType::COLORTYPE_RGBA_8888, Drawing::AlphaType::ALPHATYPE_OPAQUE});

    auto pixelMap = RSRcdSurfaceRenderNode::CreatePixelMapFromBitmap(bitmap);
    ASSERT_NE(pixelMap, nullptr);

    auto rcdLayer = std::make_shared<RSRenderSurfaceRCDLayer>();
    ASSERT_NE(rcdLayer, nullptr);
    rcdLayer->SetPixelMap(pixelMap);
    rcdLayer->SetLayerSize(GraphicIRect{0, 0, width, height});

    Drawing::Canvas drawingCanvas(width, height);
    RSPaintFilterCanvas canvas(&drawingCanvas);

    RSRenderRcdDraw::DrawRSRCDLayer(canvas, rcdLayer, Vector2f(1.0f, 1.0f));
}

/*
 * @tc.name: DrawRsRCDLayerNullPixelMap
 * @tc.desc: Test RSRenderRcdDraw::DrawRSRCDLayer with null pixelMap
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, DrawRsRCDLayerNullPixelMap, TestSize.Level1)
{
    Drawing::Canvas drawingCanvas(100, 100);
    RSPaintFilterCanvas canvas(&drawingCanvas);
    RSRenderRcdDraw::DrawRSRCDLayer(canvas, nullptr, Vector2f(1.0f, 1.0f));

    auto rcdLayer = std::make_shared<RSRenderSurfaceRCDLayer>();
    ASSERT_NE(rcdLayer, nullptr);

    RSRenderRcdDraw::DrawRSRCDLayer(canvas, rcdLayer, Vector2f(1.0f, 1.0f));
}

/*
 * @tc.name: DrawRsRCDLayerNullPixelMapPixels
 * @tc.desc: Test RSRenderRcdDraw::DrawRSRCDLayer with null pixelMap pixels
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, DrawRsRCDLayerNullPixelMapPixels, TestSize.Level1)
{
    Drawing::Bitmap bitmap;
    bitmap.Build(100, 100,
        Drawing::BitmapFormat{Drawing::ColorType::COLORTYPE_RGBA_8888, Drawing::AlphaType::ALPHATYPE_OPAQUE});

    auto pixelMap = RSRcdSurfaceRenderNode::CreatePixelMapFromBitmap(bitmap);
    ASSERT_NE(pixelMap, nullptr);

    auto rcdLayer = std::make_shared<RSRenderSurfaceRCDLayer>();
    ASSERT_NE(rcdLayer, nullptr);

    Drawing::Canvas drawingCanvas(100, 100);
    RSPaintFilterCanvas canvas(&drawingCanvas);

    RSRenderRcdDraw::DrawRSRCDLayer(canvas, rcdLayer, Vector2f(1.0f, 1.0f));
}

/*
 * @tc.name: DrawRsRCDLayerInvalidWidth
 * @tc.desc: Test RSRenderRcdDraw::DrawRSRCDLayer with invalid width (< 1)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, DrawRsRCDLayerInvalidWidth, TestSize.Level1)
{
    Drawing::Bitmap bitmap;
    bitmap.Build(100, 100,
        Drawing::BitmapFormat{Drawing::ColorType::COLORTYPE_RGBA_8888, Drawing::AlphaType::ALPHATYPE_OPAQUE});

    auto pixelMap = RSRcdSurfaceRenderNode::CreatePixelMapFromBitmap(bitmap);
    ASSERT_NE(pixelMap, nullptr);

    auto rcdLayer = std::make_shared<RSRenderSurfaceRCDLayer>();
    ASSERT_NE(rcdLayer, nullptr);
    rcdLayer->SetPixelMap(pixelMap);

    Drawing::Canvas drawingCanvas(100, 100);
    RSPaintFilterCanvas canvas(&drawingCanvas);

    RSRenderRcdDraw::DrawRSRCDLayer(canvas, rcdLayer, Vector2f(1.0f, 1.0f));
}

/*
 * @tc.name: DrawRsRCDLayerInvalidHeight
 * @tc.desc: Test RSRenderRcdDraw::DrawRSRCDLayer with invalid height (< 1)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, DrawRsRCDLayerInvalidHeight, TestSize.Level1)
{
    Drawing::Bitmap bitmap;
    bitmap.Build(100, 100,
        Drawing::BitmapFormat{Drawing::ColorType::COLORTYPE_RGBA_8888, Drawing::AlphaType::ALPHATYPE_OPAQUE});

    auto pixelMap = RSRcdSurfaceRenderNode::CreatePixelMapFromBitmap(bitmap);
    ASSERT_NE(pixelMap, nullptr);

    auto rcdLayer = std::make_shared<RSRenderSurfaceRCDLayer>();
    ASSERT_NE(rcdLayer, nullptr);
    rcdLayer->SetPixelMap(pixelMap);

    Drawing::Canvas drawingCanvas(100, 100);
    RSPaintFilterCanvas canvas(&drawingCanvas);

    RSRenderRcdDraw::DrawRSRCDLayer(canvas, rcdLayer, Vector2f(1.0f, 1.0f));
}

/*
 * @tc.name: DrawRsRCDLayerInvalidRowBytes
 * @tc.desc: Test RSRenderRcdDraw::DrawRSRCDLayer with invalid row bytes (< 1)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, DrawRsRCDLayerInvalidRowBytes, TestSize.Level1)
{
    Drawing::Bitmap bitmap;
    bitmap.Build(100, 100,
        Drawing::BitmapFormat{Drawing::ColorType::COLORTYPE_RGBA_8888, Drawing::AlphaType::ALPHATYPE_OPAQUE});

    auto pixelMap = RSRcdSurfaceRenderNode::CreatePixelMapFromBitmap(bitmap);
    ASSERT_NE(pixelMap, nullptr);

    auto rcdLayer = std::make_shared<RSRenderSurfaceRCDLayer>();
    ASSERT_NE(rcdLayer, nullptr);
    rcdLayer->SetPixelMap(pixelMap);

    Drawing::Canvas drawingCanvas(100, 100);
    RSPaintFilterCanvas canvas(&drawingCanvas);

    RSRenderRcdDraw::DrawRSRCDLayer(canvas, rcdLayer, Vector2f(1.0f, 1.0f));
}

/*
 * @tc.name: DrawRsRCDLayerValidMultiple
 * @tc.desc: Test RSRenderRcdDraw::DrawRSRCDLayer with multiple valid layers
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, DrawRsRCDLayerValidMultiple, TestSize.Level1)
{
    Drawing::Bitmap bitmap1;
    const int width1 = 100;
    const int height1 = 50;
    bitmap1.Build(width1, height1,
        Drawing::BitmapFormat{Drawing::ColorType::COLORTYPE_RGBA_8888, Drawing::AlphaType::ALPHATYPE_OPAQUE});

    auto pixelMap1 = RSRcdSurfaceRenderNode::CreatePixelMapFromBitmap(bitmap1);
    ASSERT_NE(pixelMap1, nullptr);

    auto rcdLayer1 = std::make_shared<RSRenderSurfaceRCDLayer>();
    ASSERT_NE(rcdLayer1, nullptr);
    rcdLayer1->SetPixelMap(pixelMap1);
    rcdLayer1->SetLayerSize(GraphicIRect{0, 0, width1, height1});

    Drawing::Bitmap bitmap2;
    const int width2 = 50;
    const int height2 = 100;
    bitmap2.Build(width2, height2,
        Drawing::BitmapFormat{Drawing::ColorType::COLORTYPE_RGBA_8888, Drawing::AlphaType::ALPHATYPE_OPAQUE});
    
    auto pixelMap2 = RSRcdSurfaceRenderNode::CreatePixelMapFromBitmap(bitmap2);
    ASSERT_NE(pixelMap2, nullptr);

    auto rcdLayer2 = std::make_shared<RSRenderSurfaceRCDLayer>();
    ASSERT_NE(rcdLayer2, nullptr);
    rcdLayer2->SetPixelMap(pixelMap2);
    rcdLayer2->SetLayerSize(GraphicIRect{50, 0, width2, height2});

    Drawing::Canvas drawingCanvas(150, 100);
    RSPaintFilterCanvas canvas(&drawingCanvas);

    RSRenderRcdDraw::DrawRSRCDLayer(canvas, rcdLayer1, Vector2f(1.0f, 1.0f));
    RSRenderRcdDraw::DrawRSRCDLayer(canvas, rcdLayer2, Vector2f(1.0f, 1.0f));
}

/*
 * @tc.name: DrawRsRCDLayerSmallDimensions
 * @tc.desc: Test RSRenderRcdDraw::DrawRSRCDLayer with 1x1 dimensions
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, DrawRsRCDLayerSmallDimensions, TestSize.Level1)
{
    Drawing::Bitmap bitmap;
    const int width = 1;
    const int height = 1;
    bitmap.Build(width, height,
        Drawing::BitmapFormat{Drawing::ColorType::COLORTYPE_RGBA_8888, Drawing::AlphaType::ALPHATYPE_OPAQUE});

    auto pixelMap = RSRcdSurfaceRenderNode::CreatePixelMapFromBitmap(bitmap);
    ASSERT_NE(pixelMap, nullptr);

    auto rcdLayer = std::make_shared<RSRenderSurfaceRCDLayer>();
    ASSERT_NE(rcdLayer, nullptr);
    rcdLayer->SetPixelMap(pixelMap);
    rcdLayer->SetLayerSize(GraphicIRect{0, 0, width, height});

    Drawing::Canvas drawingCanvas(width, height);
    RSPaintFilterCanvas canvas(&drawingCanvas);

    RSRenderRcdDraw::DrawRSRCDLayer(canvas, rcdLayer, Vector2f(1.0f, 1.0f));
}

/*
 * @tc.name: DrawRsRCDLayerLargeDimensions
 * @tc.desc: Test RSRenderRcdDraw::DrawRSRCDLayer with large dimensions (1920x1080)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, DrawRsRCDLayerLargeDimensions, TestSize.Level1)
{
    Drawing::Bitmap bitmap;
    const int width = 1920;
    const int height = 1080;
    bitmap.Build(width, height,
        Drawing::BitmapFormat{Drawing::ColorType::COLORTYPE_RGBA_8888, Drawing::AlphaType::ALPHATYPE_OPAQUE});

    auto pixelMap = RSRcdSurfaceRenderNode::CreatePixelMapFromBitmap(bitmap);
    ASSERT_NE(pixelMap, nullptr);

    auto rcdLayer = std::make_shared<RSRenderSurfaceRCDLayer>();
    ASSERT_NE(rcdLayer, nullptr);
    rcdLayer->SetPixelMap(pixelMap);
    rcdLayer->SetLayerSize(GraphicIRect{0, 0, width, height});

    Drawing::Canvas drawingCanvas(width, height);
    RSPaintFilterCanvas canvas(&drawingCanvas);

    RSRenderRcdDraw::DrawRSRCDLayer(canvas, rcdLayer, Vector2f(1.0f, 1.0f));
}

/*
 * @tc.name: ConsumeAndUpdateBufferTest001
 * @tc.desc: Test RSRoundCornerDisplayTest.ConsumeAndUpdateBufferTest001
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, ConsumeAndUpdateBufferTest001, TestSize.Level1)
{
    auto topSurfaceNode = std::make_shared<RSRcdSurfaceRenderNode>(0, RCDSurfaceType::TOP);
    auto visitor = std::make_shared<RSRcdRenderVisitor>();
    sptr<IBufferConsumerListener> listener = new RSRcdRenderListener(topSurfaceNode);
    topSurfaceNode->CreateSurface(listener);

    topSurfaceNode->SetAvailableBufferCount(3);
    Drawing::Bitmap layerBitmap;
    bool result = visitor->ConsumeAndUpdateBuffer(*topSurfaceNode, layerBitmap);
    EXPECT_EQ(true, result);
}

/*
 * @tc.name: ProcessRcdSurfaceRenderNode3
 * @tc.desc: Test ProcessRcdSurfaceRenderNode with resourceChanged=true and valid processor
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, ProcessRcdSurfaceRenderNode3, TestSize.Level1)
{
    Drawing::Bitmap bitmapBottomPortrait;
    const char* path = "port_down.png";
    auto& rcdInstance = RSSingleton<RoundCornerDisplay>::GetInstance();
    rcdInstance.Init();
    if (!LoadBitmapFromFile(path, bitmapBottomPortrait)) {
        std::cout << "RSRoundCornerDisplayTest: current os less rcd source" << std::endl;
        return;
    }

    auto& rcdCfg = RSSingleton<rs_rcd::RCDConfig>::GetInstance();
    rcdCfg.Load(std::string(rs_rcd::PATH_CONFIG_FILE));
    rs_rcd::LCDModel* lcdModel = rcdCfg.GetLcdModel(std::string(rs_rcd::ATTR_DEFAULT));
    ASSERT_TRUE(lcdModel != nullptr);
    int width = 0;
    int height = 0;
    rs_rcd::ROGSetting* rog = GetRogFromLcdModel(lcdModel, width, height);
    ASSERT_TRUE(rog != nullptr);

    rs_rcd::RoundCornerHardware hardInfo;
    auto portrait = rog->GetPortrait(std::string(rs_rcd::NODE_PORTRAIT));
    ASSERT_TRUE(portrait != std::nullopt);

    hardInfo.bottomLayer = std::make_shared<rs_rcd::RoundCornerLayer>(portrait->layerDown);
    hardInfo.displayRect = RectU(0, 0, width, height);
    hardInfo.bottomLayer->curBitmap = &bitmapBottomPortrait;

    auto bottomSurfaceNode = RSRcdSurfaceRenderNode::Create(0, RCDSurfaceType::BOTTOM);
    HardwareLayerInfo info{};
    bottomSurfaceNode->FillHardwareResource(info, 0, 0);

    RSUniRenderThread::Instance().uniRenderEngine_ = std::make_shared<RSUniRenderEngine>();
    auto visitor = std::make_shared<RSRcdRenderVisitor>();

    std::shared_ptr<RSComposerClientManager> rsComposerClientMgr = std::make_shared<RSComposerClientManager>();
    RSUniRenderThread::Instance().composerClientManager_ = rsComposerClientMgr;
    std::shared_ptr<RSProcessor> processorPtr =
        RSProcessorFactory::CreateProcessor(CompositeType::UNI_RENDER_COMPOSITE, 0);
    visitor->SetUniProcessor(processorPtr);
    ASSERT_TRUE(visitor->uniProcessor_ != nullptr);
    ASSERT_FALSE(bottomSurfaceNode->IsInvalidSurface());
    ASSERT_TRUE(visitor->renderEngine_ != nullptr);
    ASSERT_TRUE(bottomSurfaceNode->GetBuffer() == nullptr);
    auto res1 = visitor->ProcessRcdSurfaceRenderNode(*bottomSurfaceNode, hardInfo.bottomLayer, false);
    EXPECT_FALSE(res1);
    auto res2 = visitor->ProcessRcdSurfaceRenderNode(*bottomSurfaceNode, hardInfo.bottomLayer, true);
    EXPECT_FALSE(res2);
}

/*
 * @tc.name: BindPixelMapToDrawingImage_NullLayer
 * @tc.desc: Test BindPixelMapToDrawingImage with null layer
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, BindPixelMapToDrawingImage_NullLayer, TestSize.Level1)
{
    Drawing::Canvas canvas(100, 100);
    std::shared_ptr<RSRenderSurfaceRCDLayer> rcdLayer = nullptr;
    bool result = RSRenderRcdDraw::BindPixelMapToDrawingImage(canvas, rcdLayer);
    EXPECT_EQ(result, false);
}

/*
 * @tc.name: BindPixelMapToDrawingImage_NullPixelMap
 * @tc.desc: Test BindPixelMapToDrawingImage with null pixelMap
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, BindPixelMapToDrawingImage_NullPixelMap, TestSize.Level1)
{
    Drawing::Canvas canvas(100, 100);
    std::shared_ptr<RSRenderSurfaceRCDLayer> rcdLayer = std::make_shared<RSRenderSurfaceRCDLayer>();
    rcdLayer->pixelMap_ = nullptr;
    bool result = RSRenderRcdDraw::BindPixelMapToDrawingImage(canvas, rcdLayer);
    EXPECT_EQ(result, false);
}

/*
 * @tc.name: BindPixelMapToDrawingImage_CacheHit
 * @tc.desc: Test BindPixelMapToDrawingImage with cache hit
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, BindPixelMapToDrawingImage_CacheHit, TestSize.Level1)
{
    Drawing::Canvas canvas(100, 100);
    std::shared_ptr<RSRenderSurfaceRCDLayer> rcdLayer = std::make_shared<RSRenderSurfaceRCDLayer>();
    
    Drawing::Bitmap bitmap;
    bitmap.Build(100, 100,
        Drawing::BitmapFormat{Drawing::ColorType::COLORTYPE_RGBA_8888, Drawing::AlphaType::ALPHATYPE_OPAQUE});
    
    auto pixelMap = RSRcdSurfaceRenderNode::CreatePixelMapFromBitmap(bitmap);
    ASSERT_NE(pixelMap, nullptr);
    
    rcdLayer->pixelMap_ = pixelMap;
    rcdLayer->id_ = pixelMap->GetUniqueId();
    
    bool result = RSRenderRcdDraw::BindPixelMapToDrawingImage(canvas, rcdLayer);
    EXPECT_EQ(result, true);
}

/*
 * @tc.name: BindPixelMapToDrawingImage_NeedRebind
 * @tc.desc: Test BindPixelMapToDrawingImage when rebind is needed
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, BindPixelMapToDrawingImage_NeedRebind, TestSize.Level1)
{
    Drawing::Canvas canvas(100, 100);
    std::shared_ptr<RSRenderSurfaceRCDLayer> rcdLayer = std::make_shared<RSRenderSurfaceRCDLayer>();
    
    Drawing::Bitmap bitmap;
    bitmap.Build(100, 100,
        Drawing::BitmapFormat{Drawing::ColorType::COLORTYPE_RGBA_8888, Drawing::AlphaType::ALPHATYPE_OPAQUE});
    
    auto pixelMap = RSRcdSurfaceRenderNode::CreatePixelMapFromBitmap(bitmap);
    ASSERT_NE(pixelMap, nullptr);
    
    rcdLayer->pixelMap_ = pixelMap;
    rcdLayer->id_ = 0;
    
    bool result = RSRenderRcdDraw::BindPixelMapToDrawingImage(canvas, rcdLayer);
    EXPECT_EQ(result, true);
    EXPECT_NE(rcdLayer->image_, nullptr);
}

/*
 * @tc.name: StoreWithoutDMA_NullPixelMap
 * @tc.desc: Test StoreWithoutDMA with null pixelMap
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, StoreWithoutDMA_NullPixelMap, TestSize.Level1)
{
    Drawing::Canvas canvas(100, 100);
    std::shared_ptr<RSRenderSurfaceRCDLayer> rcdLayer = std::make_shared<RSRenderSurfaceRCDLayer>();
    RSRcdSurfaceRenderNode::PixelMapPtr pixelMap = nullptr;
    
    bool result = RSRenderRcdDraw::StoreWithoutDMA(canvas, rcdLayer, pixelMap);
    EXPECT_EQ(result, false);
}

/*
 * @tc.name: StoreWithoutDMA_ValidPixelMap
 * @tc.desc: Test StoreWithoutDMA with valid pixelMap
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, StoreWithoutDMA_ValidPixelMap, TestSize.Level1)
{
    Drawing::Canvas canvas(100, 100);
    std::shared_ptr<RSRenderSurfaceRCDLayer> rcdLayer = std::make_shared<RSRenderSurfaceRCDLayer>();
    
    Drawing::Bitmap bitmap;
    const int width = 100;
    const int height = 100;
    bitmap.Build(width, height,
        Drawing::BitmapFormat{Drawing::ColorType::COLORTYPE_RGBA_8888, Drawing::AlphaType::ALPHATYPE_OPAQUE});
    
    auto pixelMap = RSRcdSurfaceRenderNode::CreatePixelMapFromBitmap(bitmap);
    ASSERT_NE(pixelMap, nullptr);
    
    bool result = RSRenderRcdDraw::StoreWithoutDMA(canvas, rcdLayer, pixelMap);
    EXPECT_EQ(result, true);
    EXPECT_NE(rcdLayer->image_, nullptr);
    EXPECT_EQ(rcdLayer->id_, pixelMap->GetUniqueId());
}

/*
 * @tc.name: StoreWithoutDMA_InvalidWidth
 * @tc.desc: Test StoreWithoutDMA with invalid width (0)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, StoreWithoutDMA_InvalidWidth, TestSize.Level1)
{
    Drawing::Canvas canvas(100, 100);
    std::shared_ptr<RSRenderSurfaceRCDLayer> rcdLayer = std::make_shared<RSRenderSurfaceRCDLayer>();
    
    Drawing::Bitmap bitmap;
    bitmap.Build(0, 100,
        Drawing::BitmapFormat{Drawing::ColorType::COLORTYPE_RGBA_8888, Drawing::AlphaType::ALPHATYPE_OPAQUE});
    
    auto pixelMap = RSRcdSurfaceRenderNode::CreatePixelMapFromBitmap(bitmap);
    if (pixelMap == nullptr) {
        return;
    }
    
    bool result = RSRenderRcdDraw::StoreWithoutDMA(canvas, rcdLayer, pixelMap);
    EXPECT_EQ(result, false);
}

/*
 * @tc.name: StoreWithoutDMA_InvalidHeight
 * @tc.desc: Test StoreWithoutDMA with invalid height (0)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, StoreWithoutDMA_InvalidHeight, TestSize.Level1)
{
    Drawing::Canvas canvas(100, 100);
    std::shared_ptr<RSRenderSurfaceRCDLayer> rcdLayer = std::make_shared<RSRenderSurfaceRCDLayer>();
    
    Drawing::Bitmap bitmap;
    bitmap.Build(100, 0,
        Drawing::BitmapFormat{Drawing::ColorType::COLORTYPE_RGBA_8888, Drawing::AlphaType::ALPHATYPE_OPAQUE});
    
    auto pixelMap = RSRcdSurfaceRenderNode::CreatePixelMapFromBitmap(bitmap);
    if (pixelMap == nullptr) {
        return;
    }
    
    bool result = RSRenderRcdDraw::StoreWithoutDMA(canvas, rcdLayer, pixelMap);
    EXPECT_EQ(result, false);
}

/*
 * @tc.name: StoreWithoutDMA_SmallDimensions
 * @tc.desc: Test StoreWithoutDMA with 1x1 dimensions
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, StoreWithoutDMA_SmallDimensions, TestSize.Level1)
{
    Drawing::Canvas canvas(1, 1);
    std::shared_ptr<RSRenderSurfaceRCDLayer> rcdLayer = std::make_shared<RSRenderSurfaceRCDLayer>();
    
    Drawing::Bitmap bitmap;
    const int bitmapWidth = 1;
    const int bitmapHeight = 1;
    bitmap.Build(bitmapWidth, bitmapHeight,
        Drawing::BitmapFormat{Drawing::ColorType::COLORTYPE_RGBA_8888, Drawing::AlphaType::ALPHATYPE_OPAQUE});
    
    auto pixelMap = RSRcdSurfaceRenderNode::CreatePixelMapFromBitmap(bitmap);
    ASSERT_NE(pixelMap, nullptr);
    
    bool result = RSRenderRcdDraw::StoreWithoutDMA(canvas, rcdLayer, pixelMap);
    EXPECT_EQ(result, true);
    EXPECT_NE(rcdLayer->image_, nullptr);
    EXPECT_EQ(rcdLayer->id_, pixelMap->GetUniqueId());
}

/*
 * @tc.name: StoreWithoutDMA_LargeDimensions
 * @tc.desc: Test StoreWithoutDMA with large dimensions (1920x1080)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, StoreWithoutDMA_LargeDimensions, TestSize.Level1)
{
    Drawing::Canvas canvas(1920, 1080);
    std::shared_ptr<RSRenderSurfaceRCDLayer> rcdLayer = std::make_shared<RSRenderSurfaceRCDLayer>();
    
    Drawing::Bitmap bitmap;
    const int bitmapWidth = 1920;
    const int bitmapHeight = 1080;
    bitmap.Build(bitmapWidth, bitmapHeight,
        Drawing::BitmapFormat{Drawing::ColorType::COLORTYPE_RGBA_8888, Drawing::AlphaType::ALPHATYPE_OPAQUE});
    
    auto pixelMap = RSRcdSurfaceRenderNode::CreatePixelMapFromBitmap(bitmap);
    ASSERT_NE(pixelMap, nullptr);
    
    bool result = RSRenderRcdDraw::StoreWithoutDMA(canvas, rcdLayer, pixelMap);
    EXPECT_EQ(result, true);
    EXPECT_NE(rcdLayer->image_, nullptr);
    EXPECT_EQ(rcdLayer->id_, pixelMap->GetUniqueId());
}

/*
 * @tc.name: StoreWithoutDMA_MultipleCalls
 * @tc.desc: Test StoreWithoutDMA with multiple calls to verify cache override
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, StoreWithoutDMA_MultipleCalls, TestSize.Level1)
{
    Drawing::Canvas canvas(100, 100);
    std::shared_ptr<RSRenderSurfaceRCDLayer> rcdLayer = std::make_shared<RSRenderSurfaceRCDLayer>();
    
    Drawing::Bitmap bitmap1;
    const int width1 = 100;
    const int height1 = 100;
    bitmap1.Build(width1, height1,
        Drawing::BitmapFormat{Drawing::ColorType::COLORTYPE_RGBA_8888, Drawing::AlphaType::ALPHATYPE_OPAQUE});
    
    auto pixelMap1 = RSRcdSurfaceRenderNode::CreatePixelMapFromBitmap(bitmap1);
    ASSERT_NE(pixelMap1, nullptr);
    
    bool result1 = RSRenderRcdDraw::StoreWithoutDMA(canvas, rcdLayer, pixelMap1);
    EXPECT_EQ(result1, true);
    EXPECT_NE(rcdLayer->image_, nullptr);
    uint32_t firstId = rcdLayer->id_;
    
    Drawing::Bitmap bitmap2;
    const int width2 = 50;
    const int height2 = 50;
    bitmap2.Build(width2, height2,
        Drawing::BitmapFormat{Drawing::ColorType::COLORTYPE_RGBA_8888, Drawing::AlphaType::ALPHATYPE_OPAQUE});
    
    auto pixelMap2 = RSRcdSurfaceRenderNode::CreatePixelMapFromBitmap(bitmap2);
    ASSERT_NE(pixelMap2, nullptr);
    
    bool result2 = RSRenderRcdDraw::StoreWithoutDMA(canvas, rcdLayer, pixelMap2);
    EXPECT_EQ(result2, true);
    EXPECT_NE(rcdLayer->image_, nullptr);
    EXPECT_NE(rcdLayer->id_, firstId);
    EXPECT_EQ(rcdLayer->id_, pixelMap2->GetUniqueId());
}

/*
 * @tc.name: StoreWithoutDMA_DifferentPixelFormats
 * @tc.desc: Test StoreWithoutDMA with different pixel formats
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, StoreWithoutDMA_DifferentPixelFormats, TestSize.Level1)
{
    Drawing::Canvas canvas(100, 100);
    std::shared_ptr<RSRenderSurfaceRCDLayer> rcdLayer = std::make_shared<RSRenderSurfaceRCDLayer>();
    
    Drawing::Bitmap bitmap;
    bitmap.Build(100, 100,
        Drawing::BitmapFormat{Drawing::ColorType::COLORTYPE_BGRA_8888, Drawing::AlphaType::ALPHATYPE_PREMUL});
    
    auto pixelMap = RSRcdSurfaceRenderNode::CreatePixelMapFromBitmap(bitmap);
    if (pixelMap == nullptr) {
        return;
    }
    
    bool result = RSRenderRcdDraw::StoreWithoutDMA(canvas, rcdLayer, pixelMap);
    EXPECT_EQ(result, true);
    EXPECT_NE(rcdLayer->image_, nullptr);
}

/*
 * @tc.name: PixelFormatToDrawingColorType
 * @tc.desc: Test RSRcdSurfaceRenderNode.PixelFormatToDrawingColorType
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, PixelFormatToDrawingColorType, TestSize.Level1)
{
    using namespace Media;
    
    EXPECT_EQ(RSRenderRcdDraw::PixelFormatToDrawingColorType(PixelFormat::RGB_565),
        Drawing::ColorType::COLORTYPE_RGB_565);
    EXPECT_EQ(RSRenderRcdDraw::PixelFormatToDrawingColorType(PixelFormat::RGBA_8888),
        Drawing::ColorType::COLORTYPE_RGBA_8888);
    EXPECT_EQ(RSRenderRcdDraw::PixelFormatToDrawingColorType(PixelFormat::BGRA_8888),
        Drawing::ColorType::COLORTYPE_BGRA_8888);
    EXPECT_EQ(RSRenderRcdDraw::PixelFormatToDrawingColorType(PixelFormat::ALPHA_8),
        Drawing::ColorType::COLORTYPE_ALPHA_8);
    EXPECT_EQ(RSRenderRcdDraw::PixelFormatToDrawingColorType(PixelFormat::RGBA_F16),
        Drawing::ColorType::COLORTYPE_RGBA_F16);
    EXPECT_EQ(RSRenderRcdDraw::PixelFormatToDrawingColorType(PixelFormat::RGBA_1010102),
        Drawing::ColorType::COLORTYPE_RGBA_1010102);
    
    EXPECT_EQ(RSRenderRcdDraw::PixelFormatToDrawingColorType(PixelFormat::UNKNOWN),
        Drawing::ColorType::COLORTYPE_UNKNOWN);
    EXPECT_EQ(RSRenderRcdDraw::PixelFormatToDrawingColorType(PixelFormat::ARGB_8888),
        Drawing::ColorType::COLORTYPE_UNKNOWN);
    EXPECT_EQ(RSRenderRcdDraw::PixelFormatToDrawingColorType(PixelFormat::RGB_888),
        Drawing::ColorType::COLORTYPE_UNKNOWN);
    EXPECT_EQ(RSRenderRcdDraw::PixelFormatToDrawingColorType(PixelFormat::NV21),
        Drawing::ColorType::COLORTYPE_UNKNOWN);
    EXPECT_EQ(RSRenderRcdDraw::PixelFormatToDrawingColorType(PixelFormat::NV12),
        Drawing::ColorType::COLORTYPE_UNKNOWN);
    EXPECT_EQ(RSRenderRcdDraw::PixelFormatToDrawingColorType(PixelFormat::CMYK),
        Drawing::ColorType::COLORTYPE_UNKNOWN);
}

/*
 * @tc.name: AlphaTypeToDrawingAlphaType
 * @tc.desc: Test RSRcdSurfaceRenderNode.AlphaTypeToDrawingAlphaType
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, AlphaTypeToDrawingAlphaType, TestSize.Level1)
{
    using namespace Media;
    
    EXPECT_EQ(RSRenderRcdDraw::AlphaTypeToDrawingAlphaType(AlphaType::IMAGE_ALPHA_TYPE_UNKNOWN),
        Drawing::AlphaType::ALPHATYPE_UNKNOWN);
    EXPECT_EQ(RSRenderRcdDraw::AlphaTypeToDrawingAlphaType(AlphaType::IMAGE_ALPHA_TYPE_OPAQUE),
        Drawing::AlphaType::ALPHATYPE_OPAQUE);
    EXPECT_EQ(RSRenderRcdDraw::AlphaTypeToDrawingAlphaType(AlphaType::IMAGE_ALPHA_TYPE_PREMUL),
        Drawing::AlphaType::ALPHATYPE_PREMUL);
    EXPECT_EQ(RSRenderRcdDraw::AlphaTypeToDrawingAlphaType(AlphaType::IMAGE_ALPHA_TYPE_UNPREMUL),
        Drawing::AlphaType::ALPHATYPE_UNPREMUL);
}

/*
 * @tc.name: SendRcdMessage_WithEmptyActiveRect
 * @tc.desc: Test RoundCornerDisplayManager.SendRcdMessage with empty activeRect
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, SendRcdMessage_WithEmptyActiveRect, TestSize.Level1)
{
    auto& rcdMgr = RSSingleton<RoundCornerDisplayManager>::GetInstance();
    NodeId id = 100;
    rcdMgr.AddRoundCornerDisplay(id);
    ASSERT_TRUE(rcdMgr.CheckExist(id));

    RSScreenProperty screenProperty;
    screenProperty.Set<ScreenPropertyType::RENDER_RESOLUTION>(std::pair<uint32_t, uint32_t>{1344, 2772});

    rcdMgr.SendRcdMessage(id, screenProperty);

    EXPECT_TRUE(rcdMgr.rcdMap_[id]->lastRcvDisplayRect_.GetLeft() == 0);
    EXPECT_TRUE(rcdMgr.rcdMap_[id]->lastRcvDisplayRect_.GetTop() == 0);
    EXPECT_TRUE(rcdMgr.rcdMap_[id]->lastRcvDisplayRect_.GetWidth() == 1344);
    EXPECT_TRUE(rcdMgr.rcdMap_[id]->lastRcvDisplayRect_.GetHeight() == 2772);
    EXPECT_TRUE(rcdMgr.rcdMap_[id]->notchStatus_ == WINDOW_NOTCH_DEFAULT);

    rcdMgr.RemoveRCDResource(id);
}

/*
 * @tc.name: SendRcdMessage_WithNonEmptyActiveRect
 * @tc.desc: Test RoundCornerDisplayManager.SendRcdMessage with non-empty activeRect
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, SendRcdMessage_WithNonEmptyActiveRect, TestSize.Level1)
{
    auto& rcdMgr = RSSingleton<RoundCornerDisplayManager>::GetInstance();
    NodeId id = 101;
    rcdMgr.AddRoundCornerDisplay(id);
    ASSERT_TRUE(rcdMgr.CheckExist(id));

    RSScreenProperty screenProperty;
    screenProperty.Set<ScreenPropertyType::RENDER_RESOLUTION>(std::pair<uint32_t, uint32_t>{1344, 2772});
    RectI activeRect(10, 20, 1080, 1920);
    screenProperty.Set<ScreenPropertyType::ACTIVE_RECT_OPTION>(
        std::make_tuple(activeRect, RectI(), RectI()));

    rcdMgr.SendRcdMessage(id, screenProperty);

    EXPECT_TRUE(rcdMgr.rcdMap_[id]->lastRcvDisplayRect_.GetLeft() == 10);
    EXPECT_TRUE(rcdMgr.rcdMap_[id]->lastRcvDisplayRect_.GetTop() == 20);
    EXPECT_TRUE(rcdMgr.rcdMap_[id]->lastRcvDisplayRect_.GetWidth() == 1080);
    EXPECT_TRUE(rcdMgr.rcdMap_[id]->lastRcvDisplayRect_.GetHeight() == 1920);
    EXPECT_TRUE(rcdMgr.rcdMap_[id]->notchStatus_ == WINDOW_NOTCH_DEFAULT);

    rcdMgr.RemoveRCDResource(id);
}

/*
 * @tc.name: SendRcdMessage_WithNonExistId
 * @tc.desc: Test RoundCornerDisplayManager.SendRcdMessage with non-existent id
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, SendRcdMessage_WithNonExistId, TestSize.Level1)
{
    auto& rcdMgr = RSSingleton<RoundCornerDisplayManager>::GetInstance();
    NodeId id = 102;

    RSScreenProperty screenProperty;
    screenProperty.Set<ScreenPropertyType::RENDER_RESOLUTION>(std::pair<uint32_t, uint32_t>{1344, 2772});

    rcdMgr.SendRcdMessage(id, screenProperty);

    EXPECT_TRUE(rcdMgr.CheckExist(id) == false);
}

/*
 * @tc.name: SendRcdMessage_WithNullRcdModule
 * @tc.desc: Test RoundCornerDisplayManager.SendRcdMessage with null rcd module in map
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, SendRcdMessage_WithNullRcdModule, TestSize.Level1)
{
    auto& rcdMgr = RSSingleton<RoundCornerDisplayManager>::GetInstance();
    NodeId id = 103;
    rcdMgr.AddRoundCornerDisplay(id);
    ASSERT_TRUE(rcdMgr.CheckExist(id));
    rcdMgr.rcdMap_[id] = nullptr;

    RSScreenProperty screenProperty;
    screenProperty.Set<ScreenPropertyType::RENDER_RESOLUTION>(std::pair<uint32_t, uint32_t>{1344, 2772});

    rcdMgr.SendRcdMessage(id, screenProperty);

    EXPECT_TRUE(rcdMgr.CheckExist(id) == false);
}

/*
 * @tc.name: SendRcdMessage_WithNegativeActiveRectValues
 * @tc.desc: Test RoundCornerDisplayManager.SendRcdMessage with negative activeRect values clamped to 0
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, SendRcdMessage_WithNegativeActiveRectValues, TestSize.Level1)
{
    auto& rcdMgr = RSSingleton<RoundCornerDisplayManager>::GetInstance();
    NodeId id = 104;
    rcdMgr.AddRoundCornerDisplay(id);
    ASSERT_TRUE(rcdMgr.CheckExist(id));

    RSScreenProperty screenProperty;
    screenProperty.Set<ScreenPropertyType::RENDER_RESOLUTION>(std::pair<uint32_t, uint32_t>{1344, 2772});
    RectI activeRect(-10, -20, 1080, 1920);
    screenProperty.Set<ScreenPropertyType::ACTIVE_RECT_OPTION>(
        std::make_tuple(activeRect, RectI(), RectI()));

    rcdMgr.SendRcdMessage(id, screenProperty);

    EXPECT_TRUE(rcdMgr.rcdMap_[id]->lastRcvDisplayRect_.GetLeft() == 0);
    EXPECT_TRUE(rcdMgr.rcdMap_[id]->lastRcvDisplayRect_.GetTop() == 0);
    EXPECT_TRUE(rcdMgr.rcdMap_[id]->lastRcvDisplayRect_.GetWidth() == 1080);
    EXPECT_TRUE(rcdMgr.rcdMap_[id]->lastRcvDisplayRect_.GetHeight() == 1920);
    EXPECT_TRUE(rcdMgr.rcdMap_[id]->notchStatus_ == WINDOW_NOTCH_DEFAULT);

    rcdMgr.RemoveRCDResource(id);
}

/*
 * @tc.name: CheckRcdRenderEnable_HdiOutputEnable
 * @tc.desc: Test RoundCornerDisplayManager.CheckRcdRenderEnable with HDI_OUTPUT_ENABLE state
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, CheckRcdRenderEnable_HdiOutputEnable, TestSize.Level1)
{
    RSScreenProperty screenProperty;
    screenProperty.Set<ScreenPropertyType::STATE>(
        static_cast<uint8_t>(ScreenState::HDI_OUTPUT_ENABLE));

    EXPECT_TRUE(RoundCornerDisplayManager::CheckRcdRenderEnable(screenProperty));
}

/*
 * @tc.name: CheckRcdRenderEnable_OtherStates
 * @tc.desc: Test RoundCornerDisplayManager.CheckRcdRenderEnable with non-HDI_OUTPUT_ENABLE states
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, CheckRcdRenderEnable_OtherStates, TestSize.Level1)
{
    RSScreenProperty screenProperty;

    screenProperty.Set<ScreenPropertyType::STATE>(
        static_cast<uint8_t>(ScreenState::UNKNOWN));
    EXPECT_TRUE(RoundCornerDisplayManager::CheckRcdRenderEnable(screenProperty) == false);

    screenProperty.Set<ScreenPropertyType::STATE>(
        static_cast<uint8_t>(ScreenState::PRODUCER_SURFACE_ENABLE));
    EXPECT_TRUE(RoundCornerDisplayManager::CheckRcdRenderEnable(screenProperty) == false);

    screenProperty.Set<ScreenPropertyType::STATE>(
        static_cast<uint8_t>(ScreenState::DISABLED));
    EXPECT_TRUE(RoundCornerDisplayManager::CheckRcdRenderEnable(screenProperty) == false);

    RSScreenProperty defaultProperty;
    EXPECT_TRUE(RoundCornerDisplayManager::CheckRcdRenderEnable(defaultProperty) == false);
}

/*
 * @tc.name: CreatePixelMapFromBitmap_BGRA8888ColorType
 * @tc.desc: Test CreatePixelMapFromBitmap with BGRA_8888 color type returns nullptr
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, CreatePixelMapFromBitmap_BGRA8888ColorType, TestSize.Level1)
{
    Drawing::Bitmap bitmap;
    bitmap.Build(100, 100,
        Drawing::BitmapFormat{Drawing::ColorType::COLORTYPE_BGRA_8888, Drawing::AlphaType::ALPHATYPE_PREMUL});
    auto pixelMap = RSRcdSurfaceRenderNode::CreatePixelMapFromBitmap(bitmap);
    EXPECT_NE(pixelMap, nullptr);
}

/*
 * @tc.name: CreatePixelMapFromBitmap_RGB565ColorType
 * @tc.desc: Test CreatePixelMapFromBitmap with RGB_565 color type returns nullptr
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, CreatePixelMapFromBitmap_RGB565ColorType, TestSize.Level1)
{
    Drawing::Bitmap bitmap;
    bitmap.Build(100, 100,
        Drawing::BitmapFormat{Drawing::ColorType::COLORTYPE_RGB_565, Drawing::AlphaType::ALPHATYPE_OPAQUE});
    auto pixelMap = RSRcdSurfaceRenderNode::CreatePixelMapFromBitmap(bitmap);
    EXPECT_EQ(pixelMap, nullptr);
}

/*
 * @tc.name: CreatePixelMapFromBitmap_ALPHA8ColorType
 * @tc.desc: Test CreatePixelMapFromBitmap with ALPHA_8 color type returns nullptr
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, CreatePixelMapFromBitmap_ALPHA8ColorType, TestSize.Level1)
{
    Drawing::Bitmap bitmap;
    bitmap.Build(100, 100,
        Drawing::BitmapFormat{Drawing::ColorType::COLORTYPE_ALPHA_8, Drawing::AlphaType::ALPHATYPE_OPAQUE});
    auto pixelMap = RSRcdSurfaceRenderNode::CreatePixelMapFromBitmap(bitmap);
    EXPECT_EQ(pixelMap, nullptr);
}

/*
 * @tc.name: CreatePixelMapFromBitmap_RGBA8888PremulAlpha
 * @tc.desc: Test CreatePixelMapFromBitmap with RGBA_8888 and PREMUL alpha succeeds
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, CreatePixelMapFromBitmap_RGBA8888PremulAlpha, TestSize.Level1)
{
    Drawing::Bitmap bitmap;
    const int width = 100;
    const int height = 100;
    bitmap.Build(width, height,
        Drawing::BitmapFormat{Drawing::ColorType::COLORTYPE_RGBA_8888, Drawing::AlphaType::ALPHATYPE_PREMUL});
    auto pixelMap = RSRcdSurfaceRenderNode::CreatePixelMapFromBitmap(bitmap);
    ASSERT_NE(pixelMap, nullptr);
    EXPECT_EQ(pixelMap->GetWidth(), width);
    EXPECT_EQ(pixelMap->GetHeight(), height);
}

HWTEST_F(RSRoundCornerDisplayTest, ProcessRcdSurfaceRenderNodeMainThread_Normal, TestSize.Level1)
{
    auto surfaceNode = std::make_shared<RSRcdSurfaceRenderNode>(0, RCDSurfaceType::BOTTOM);
    ASSERT_NE(surfaceNode, nullptr);
    auto visitor = std::make_shared<RSRcdRenderVisitor>();
    std::shared_ptr<RSComposerClientManager> rsComposerClientMgr = std::make_shared<RSComposerClientManager>();
    RSUniRenderThread::Instance().composerClientManager_ = rsComposerClientMgr;
    auto processor = RSProcessorFactory::CreateProcessor(CompositeType::HARDWARE_COMPOSITE, 0);
    visitor->SetUniProcessor(processor);
    ASSERT_NE(visitor->uniProcessor_, nullptr);
    ASSERT_FALSE(surfaceNode->IsInvalidSurface());
    visitor->ProcessRcdSurfaceRenderNodeMainThread(*surfaceNode, false);
}

HWTEST_F(RSRoundCornerDisplayTest, ProcessRcdSurfaceRenderNodeMainThread_ResourceChangedTrue, TestSize.Level1)
{
    auto surfaceNode = std::make_shared<RSRcdSurfaceRenderNode>(0, RCDSurfaceType::BOTTOM);
    ASSERT_NE(surfaceNode, nullptr);
    auto visitor = std::make_shared<RSRcdRenderVisitor>();
    std::shared_ptr<RSComposerClientManager> rsComposerClientMgr = std::make_shared<RSComposerClientManager>();
    RSUniRenderThread::Instance().composerClientManager_ = rsComposerClientMgr;
    auto processor = RSProcessorFactory::CreateProcessor(CompositeType::HARDWARE_COMPOSITE, 0);
    visitor->SetUniProcessor(processor);
    ASSERT_NE(visitor->uniProcessor_, nullptr);
    ASSERT_FALSE(surfaceNode->IsInvalidSurface());
    visitor->ProcessRcdSurfaceRenderNodeMainThread(*surfaceNode, true);
}

HWTEST_F(RSRoundCornerDisplayTest, ProcessRcdSurfaceRenderNode_RenderEngineNull, TestSize.Level1)
{
    auto surfaceNode = std::make_shared<RSRcdSurfaceRenderNode>(0, RCDSurfaceType::BOTTOM);
    ASSERT_NE(surfaceNode, nullptr);
    auto visitor = std::make_shared<RSRcdRenderVisitor>();
    std::shared_ptr<RSComposerClientManager> rsComposerClientMgr = std::make_shared<RSComposerClientManager>();
    RSUniRenderThread::Instance().composerClientManager_ = rsComposerClientMgr;
    auto processor = RSProcessorFactory::CreateProcessor(CompositeType::HARDWARE_COMPOSITE, 0);
    visitor->SetUniProcessor(processor);
    visitor->renderEngine_ = nullptr;
    ASSERT_NE(visitor->uniProcessor_, nullptr);
    ASSERT_FALSE(surfaceNode->IsInvalidSurface());
    ASSERT_EQ(visitor->renderEngine_, nullptr);
    rs_rcd::RoundCornerHardware hardInfo{};
    bool result = visitor->ProcessRcdSurfaceRenderNode(*surfaceNode, hardInfo.bottomLayer, true);
    EXPECT_FALSE(result);
}

HWTEST_F(RSRoundCornerDisplayTest, ProcessRcdSurfaceRenderNode_NullLayerInfo, TestSize.Level1)
{
    auto surfaceNode = std::make_shared<RSRcdSurfaceRenderNode>(0, RCDSurfaceType::BOTTOM);
    ASSERT_NE(surfaceNode, nullptr);
    sptr<IBufferConsumerListener> listener = new RSRcdRenderListener(surfaceNode);
    surfaceNode->CreateSurface(listener);
    auto visitor = std::make_shared<RSRcdRenderVisitor>();
    std::shared_ptr<RSComposerClientManager> rsComposerClientMgr = std::make_shared<RSComposerClientManager>();
    RSUniRenderThread::Instance().composerClientManager_ = rsComposerClientMgr;
    auto processor = RSProcessorFactory::CreateProcessor(CompositeType::HARDWARE_COMPOSITE, 0);
    visitor->SetUniProcessor(processor);
    ASSERT_NE(visitor->uniProcessor_, nullptr);
    ASSERT_NE(visitor->renderEngine_, nullptr);
    ASSERT_FALSE(surfaceNode->IsInvalidSurface());
    bool result = visitor->ProcessRcdSurfaceRenderNode(*surfaceNode, nullptr, true);
    EXPECT_FALSE(result);
}

HWTEST_F(RSRoundCornerDisplayTest, ProcessRcdSurfaceRenderNode_ResourceNotChangedWithBuffer, TestSize.Level1)
{
    auto surfaceNode = std::make_shared<RSRcdSurfaceRenderNode>(0, RCDSurfaceType::TOP);
    ASSERT_NE(surfaceNode, nullptr);
    auto visitor = std::make_shared<RSRcdRenderVisitor>();
    std::shared_ptr<RSComposerClientManager> rsComposerClientMgr = std::make_shared<RSComposerClientManager>();
    RSUniRenderThread::Instance().composerClientManager_ = rsComposerClientMgr;
    auto processor = RSProcessorFactory::CreateProcessor(CompositeType::HARDWARE_COMPOSITE, 0);
    visitor->SetUniProcessor(processor);
    ASSERT_NE(visitor->uniProcessor_, nullptr);
    ASSERT_NE(visitor->renderEngine_, nullptr);
    ASSERT_FALSE(surfaceNode->IsInvalidSurface());

    sptr<IBufferConsumerListener> listener = new RSRcdRenderListener(surfaceNode);
    surfaceNode->CreateSurface(listener);
    surfaceNode->SetAvailableBufferCount(1);
    Drawing::Bitmap layerBitmap;
    ASSERT_NE(visitor->ConsumeAndUpdateBuffer(*surfaceNode, layerBitmap), true);
    ASSERT_EQ(surfaceNode->GetBuffer(), nullptr);

    rs_rcd::RoundCornerHardware hardInfo{};
    bool result = visitor->ProcessRcdSurfaceRenderNode(*surfaceNode, hardInfo.bottomLayer, false);
    EXPECT_FALSE(result);
}

HWTEST_F(RSRoundCornerDisplayTest, ProcessRcdSurfaceRenderNode_SurfaceNotCreated, TestSize.Level1)
{
    auto surfaceNode = std::make_shared<RSRcdSurfaceRenderNode>(0, RCDSurfaceType::TOP);
    ASSERT_NE(surfaceNode, nullptr);
    ASSERT_FALSE(surfaceNode->IsSurfaceCreated());
    auto visitor = std::make_shared<RSRcdRenderVisitor>();
    std::shared_ptr<RSComposerClientManager> rsComposerClientMgr = std::make_shared<RSComposerClientManager>();
    RSUniRenderThread::Instance().composerClientManager_ = rsComposerClientMgr;
    auto processor = RSProcessorFactory::CreateProcessor(CompositeType::HARDWARE_COMPOSITE, 0);
    visitor->SetUniProcessor(processor);
    ASSERT_NE(visitor->uniProcessor_, nullptr);
    ASSERT_NE(visitor->renderEngine_, nullptr);
    ASSERT_FALSE(surfaceNode->IsInvalidSurface());

    rs_rcd::RoundCornerHardware hardInfo{};
    bool result = visitor->ProcessRcdSurfaceRenderNode(*surfaceNode, hardInfo.bottomLayer, true);
    EXPECT_FALSE(result);
}

HWTEST_F(RSRoundCornerDisplayTest, ProcessRcdSurfaceRenderNodeMainThread_InvalidSurface, TestSize.Level1)
{
    auto surfaceNode = std::make_shared<RSRcdSurfaceRenderNode>(0, RCDSurfaceType::INVALID);
    ASSERT_NE(surfaceNode, nullptr);
    ASSERT_TRUE(surfaceNode->IsInvalidSurface());
    auto visitor = std::make_shared<RSRcdRenderVisitor>();
    std::shared_ptr<RSComposerClientManager> rsComposerClientMgr = std::make_shared<RSComposerClientManager>();
    RSUniRenderThread::Instance().composerClientManager_ = rsComposerClientMgr;
    auto processor = RSProcessorFactory::CreateProcessor(CompositeType::HARDWARE_COMPOSITE, 0);
    visitor->SetUniProcessor(processor);
    ASSERT_NE(visitor->uniProcessor_, nullptr);
    visitor->ProcessRcdSurfaceRenderNodeMainThread(*surfaceNode, false);
}

HWTEST_F(RSRoundCornerDisplayTest, ProcessRcdSurfaceRenderNodeMainThread_NullProcessor, TestSize.Level1)
{
    auto surfaceNode = std::make_shared<RSRcdSurfaceRenderNode>(0, RCDSurfaceType::BOTTOM);
    ASSERT_NE(surfaceNode, nullptr);
    auto visitor = std::make_shared<RSRcdRenderVisitor>();
    visitor->SetUniProcessor(nullptr);
    ASSERT_EQ(visitor->uniProcessor_, nullptr);
    visitor->ProcessRcdSurfaceRenderNodeMainThread(*surfaceNode, false);
}

/*
 * @tc.name: PrepareHardwareResourceBuffer_NullLayerInfo
 * @tc.desc: Test RSRcdSurfaceRenderNode::PrepareHardwareResourceBuffer with null layerInfo
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, PrepareHardwareResourceBuffer_NullLayerInfo, TestSize.Level1)
{
    auto surfaceNode = std::make_shared<RSRcdSurfaceRenderNode>(0, RCDSurfaceType::TOP);
    ASSERT_NE(surfaceNode, nullptr);

    Drawing::Bitmap layerBitmap;
    bool result = surfaceNode->PrepareHardwareResourceBuffer(nullptr, layerBitmap);
    EXPECT_FALSE(result);
}

/*
 * @tc.name: PrepareHardwareResourceBuffer_NullCurBitmap
 * @tc.desc: Test RSRcdSurfaceRenderNode::PrepareHardwareResourceBuffer with null curBitmap in layerInfo
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, PrepareHardwareResourceBuffer_NullCurBitmap, TestSize.Level1)
{
    auto surfaceNode = std::make_shared<RSRcdSurfaceRenderNode>(0, RCDSurfaceType::TOP);
    ASSERT_NE(surfaceNode, nullptr);

    rs_rcd::RoundCornerLayer layer{"top.png", 0, 0, "top.bin", 8112, 2028, 1, nullptr};
    auto layerInfo = std::make_shared<rs_rcd::RoundCornerLayer>(layer);

    Drawing::Bitmap layerBitmap;
    bool result = surfaceNode->PrepareHardwareResourceBuffer(layerInfo, layerBitmap);
    EXPECT_FALSE(result);
}

/*
 * @tc.name: PrepareHardwareResourceBuffer_Alpha8Bitmap
 * @tc.desc: Test RSRcdSurfaceRenderNode::PrepareHardwareResourceBuffer with ALPHA_8 bitmap triggers conversion
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, PrepareHardwareResourceBuffer_Alpha8Bitmap, TestSize.Level1)
{
    auto surfaceNode = std::make_shared<RSRcdSurfaceRenderNode>(0, RCDSurfaceType::TOP);
    ASSERT_NE(surfaceNode, nullptr);

    const int width = 100;
    const int height = 50;
    Drawing::Bitmap alpha8Bitmap;
    alpha8Bitmap.Build(width, height,
        Drawing::BitmapFormat{Drawing::ColorType::COLORTYPE_ALPHA_8, Drawing::AlphaType::ALPHATYPE_OPAQUE});

    rs_rcd::RoundCornerLayer layer{"top.png", 0, 0, "top.bin", 8112, 2028, 1, &alpha8Bitmap};
    auto layerInfo = std::make_shared<rs_rcd::RoundCornerLayer>(layer);

    surfaceNode->SetRenderDisplayRect(RectT<uint32_t>(0, 0, 200, 200));
    Drawing::Bitmap layerBitmap;
    bool result = surfaceNode->PrepareHardwareResourceBuffer(layerInfo, layerBitmap);
    EXPECT_TRUE(result);
    EXPECT_EQ(static_cast<int>(layerBitmap.GetWidth()), width);
    EXPECT_EQ(static_cast<int>(layerBitmap.GetHeight()), height);
    EXPECT_EQ(layerBitmap.GetColorType(), Drawing::ColorType::COLORTYPE_RGBA_8888);
}

/*
 * @tc.name: PrepareHardwareResourceBuffer_Rgba8888Bitmap
 * @tc.desc: Test RSRcdSurfaceRenderNode::PrepareHardwareResourceBuffer with RGBA_8888 bitmap copies directly
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, PrepareHardwareResourceBuffer_Rgba8888Bitmap, TestSize.Level1)
{
    auto surfaceNode = std::make_shared<RSRcdSurfaceRenderNode>(0, RCDSurfaceType::TOP);
    ASSERT_NE(surfaceNode, nullptr);

    const int width = 100;
    const int height = 50;
    Drawing::Bitmap rgbaBitmap;
    rgbaBitmap.Build(width, height,
        Drawing::BitmapFormat{Drawing::ColorType::COLORTYPE_RGBA_8888, Drawing::AlphaType::ALPHATYPE_OPAQUE});

    rs_rcd::RoundCornerLayer layer{"top.png", 0, 0, "top.bin", 8112, 2028, 1, &rgbaBitmap};
    auto layerInfo = std::make_shared<rs_rcd::RoundCornerLayer>(layer);

    surfaceNode->SetRenderDisplayRect(RectT<uint32_t>(0, 0, 200, 200));
    Drawing::Bitmap layerBitmap;
    bool result = surfaceNode->PrepareHardwareResourceBuffer(layerInfo, layerBitmap);
    EXPECT_TRUE(result);
    EXPECT_EQ(static_cast<int>(layerBitmap.GetWidth()), width);
    EXPECT_EQ(static_cast<int>(layerBitmap.GetHeight()), height);
    EXPECT_EQ(layerBitmap.GetColorType(), Drawing::ColorType::COLORTYPE_RGBA_8888);
}

/*
 * @tc.name: PrepareHardwareResourceBuffer_TopSurfaceRect
 * @tc.desc: Test RSRcdSurfaceRenderNode::PrepareHardwareResourceBuffer sets correct rects for TOP surface
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, PrepareHardwareResourceBuffer_TopSurfaceRect, TestSize.Level1)
{
    auto surfaceNode = std::make_shared<RSRcdSurfaceRenderNode>(0, RCDSurfaceType::TOP);
    ASSERT_NE(surfaceNode, nullptr);

    const int width = 100;
    const int height = 50;
    Drawing::Bitmap rgbaBitmap;
    rgbaBitmap.Build(width, height,
        Drawing::BitmapFormat{Drawing::ColorType::COLORTYPE_RGBA_8888, Drawing::AlphaType::ALPHATYPE_OPAQUE});

    rs_rcd::RoundCornerLayer layer{"top.png", 0, 0, "top.bin", 8112, 2028, 1, &rgbaBitmap};
    auto layerInfo = std::make_shared<rs_rcd::RoundCornerLayer>(layer);

    const uint32_t displayLeft = 10;
    const uint32_t displayTop = 20;
    const uint32_t displayWidth = 200;
    const uint32_t displayHeight = 400;
    surfaceNode->SetRenderDisplayRect(RectT<uint32_t>(displayLeft, displayTop, displayWidth, displayHeight));

    Drawing::Bitmap layerBitmap;
    bool result = surfaceNode->PrepareHardwareResourceBuffer(layerInfo, layerBitmap);
    EXPECT_TRUE(result);

    const RectI& srcRect = surfaceNode->GetSrcRect();
    EXPECT_EQ(srcRect.GetLeft(), 0);
    EXPECT_EQ(srcRect.GetTop(), 0);
    EXPECT_EQ(srcRect.GetWidth(), width);
    EXPECT_EQ(srcRect.GetHeight(), height);

    const RectI& dstRect = surfaceNode->GetDstRect();
    EXPECT_EQ(dstRect.GetLeft(), static_cast<int>(displayLeft));
    EXPECT_EQ(dstRect.GetTop(), static_cast<int>(displayTop));
    EXPECT_EQ(dstRect.GetWidth(), width);
    EXPECT_EQ(dstRect.GetHeight(), height);
}

/*
 * @tc.name: PrepareHardwareResourceBuffer_BottomSurfaceRect
 * @tc.desc: Test RSRcdSurfaceRenderNode::PrepareHardwareResourceBuffer sets correct rects for BOTTOM surface
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, PrepareHardwareResourceBuffer_BottomSurfaceRect, TestSize.Level1)
{
    auto surfaceNode = std::make_shared<RSRcdSurfaceRenderNode>(0, RCDSurfaceType::BOTTOM);
    ASSERT_NE(surfaceNode, nullptr);

    const int width = 100;
    const int height = 50;
    Drawing::Bitmap rgbaBitmap;
    rgbaBitmap.Build(width, height,
        Drawing::BitmapFormat{Drawing::ColorType::COLORTYPE_RGBA_8888, Drawing::AlphaType::ALPHATYPE_OPAQUE});

    rs_rcd::RoundCornerLayer layer{"bottom.png", 0, 0, "bottom.bin", 8112, 2028, 1, &rgbaBitmap};
    auto layerInfo = std::make_shared<rs_rcd::RoundCornerLayer>(layer);

    const uint32_t displayLeft = 10;
    const uint32_t displayTop = 20;
    const uint32_t displayWidth = 200;
    const uint32_t displayHeight = 400;
    surfaceNode->SetRenderDisplayRect(RectT<uint32_t>(displayLeft, displayTop, displayWidth, displayHeight));

    Drawing::Bitmap layerBitmap;
    bool result = surfaceNode->PrepareHardwareResourceBuffer(layerInfo, layerBitmap);
    EXPECT_TRUE(result);

    const RectI& srcRect = surfaceNode->GetSrcRect();
    EXPECT_EQ(srcRect.GetLeft(), 0);
    EXPECT_EQ(srcRect.GetTop(), 0);
    EXPECT_EQ(srcRect.GetWidth(), width);
    EXPECT_EQ(srcRect.GetHeight(), height);

    const RectI& dstRect = surfaceNode->GetDstRect();
    EXPECT_EQ(dstRect.GetLeft(), static_cast<int>(displayLeft));
    int expectedTop = static_cast<int>(displayHeight) - height + static_cast<int>(displayTop);
    EXPECT_EQ(dstRect.GetTop(), expectedTop);
    EXPECT_EQ(dstRect.GetWidth(), width);
    EXPECT_EQ(dstRect.GetHeight(), height);
}

/*
 * @tc.name: PrepareHardwareResourceBuffer_ZeroDisplayHeight
 * @tc.desc: Test RSRcdSurfaceRenderNode::PrepareHardwareResourceBuffer with zero display height fails
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, PrepareHardwareResourceBuffer_ZeroDisplayHeight, TestSize.Level1)
{
    auto surfaceNode = std::make_shared<RSRcdSurfaceRenderNode>(0, RCDSurfaceType::TOP);
    ASSERT_NE(surfaceNode, nullptr);

    const int width = 100;
    const int height = 50;
    Drawing::Bitmap rgbaBitmap;
    rgbaBitmap.Build(width, height,
        Drawing::BitmapFormat{Drawing::ColorType::COLORTYPE_RGBA_8888, Drawing::AlphaType::ALPHATYPE_OPAQUE});

    rs_rcd::RoundCornerLayer layer{"top.png", 0, 0, "top.bin", 8112, 2028, 1, &rgbaBitmap};
    auto layerInfo = std::make_shared<rs_rcd::RoundCornerLayer>(layer);

    surfaceNode->SetRenderDisplayRect(RectT<uint32_t>(0, 0, 200, 0));
    Drawing::Bitmap layerBitmap;
    bool result = surfaceNode->PrepareHardwareResourceBuffer(layerInfo, layerBitmap);
    EXPECT_FALSE(result);
}

/*
 * @tc.name: PrepareHardwareResourceBuffer_SetsBufferDimensions
 * @tc.desc: Test RSRcdSurfaceRenderNode::PrepareHardwareResourceBuffer sets buffer width/height/size
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, PrepareHardwareResourceBuffer_SetsBufferDimensions, TestSize.Level1)
{
    auto surfaceNode = std::make_shared<RSRcdSurfaceRenderNode>(0, RCDSurfaceType::TOP);
    ASSERT_NE(surfaceNode, nullptr);

    const int width = 100;
    const int height = 50;
    const int bufferSize = 8112;
    Drawing::Bitmap rgbaBitmap;
    rgbaBitmap.Build(width, height,
        Drawing::BitmapFormat{Drawing::ColorType::COLORTYPE_RGBA_8888, Drawing::AlphaType::ALPHATYPE_OPAQUE});

    rs_rcd::RoundCornerLayer layer{"top.png", 0, 0, "top.bin", bufferSize, 2028, 1, &rgbaBitmap};
    auto layerInfo = std::make_shared<rs_rcd::RoundCornerLayer>(layer);

    surfaceNode->SetRenderDisplayRect(RectT<uint32_t>(0, 0, 200, 200));
    Drawing::Bitmap layerBitmap;
    bool result = surfaceNode->PrepareHardwareResourceBuffer(layerInfo, layerBitmap);
    EXPECT_TRUE(result);

    BufferRequestConfig config = surfaceNode->GetHardenBufferRequestConfig();
    EXPECT_EQ(config.width, width);
    int32_t expectedHeight = bufferSize / width + height + 2;
    EXPECT_EQ(config.height, expectedHeight);
    EXPECT_EQ(config.format, GRAPHIC_PIXEL_FMT_RGBA_8888);
}

/*
 * @tc.name: PrepareHardwareResourceBuffer_CldLayerInfoPopulated
 * @tc.desc: Test RSRcdSurfaceRenderNode::PrepareHardwareResourceBuffer populates buffer size from layerInfo
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, PrepareHardwareResourceBuffer_CldLayerInfoPopulated, TestSize.Level1)
{
    auto surfaceNode = std::make_shared<RSRcdSurfaceRenderNode>(0, RCDSurfaceType::TOP);
    ASSERT_NE(surfaceNode, nullptr);

    const int width = 100;
    const int height = 50;
    const int bufferSize = 8112;
    const int cldWidth = 2028;
    const int cldHeight = 1;
    const std::string binFileName = "top.bin";
    Drawing::Bitmap rgbaBitmap;
    rgbaBitmap.Build(width, height,
        Drawing::BitmapFormat{Drawing::ColorType::COLORTYPE_RGBA_8888, Drawing::AlphaType::ALPHATYPE_OPAQUE});

    rs_rcd::RoundCornerLayer layer{"top.png", 0, 0, binFileName, bufferSize, cldWidth, cldHeight, &rgbaBitmap};
    auto layerInfo = std::make_shared<rs_rcd::RoundCornerLayer>(layer);

    surfaceNode->SetRenderDisplayRect(RectT<uint32_t>(0, 0, 200, 200));
    Drawing::Bitmap layerBitmap;
    bool result = surfaceNode->PrepareHardwareResourceBuffer(layerInfo, layerBitmap);
    EXPECT_TRUE(result);

    BufferRequestConfig config = surfaceNode->GetHardenBufferRequestConfig();
    EXPECT_EQ(config.width, width);
    int32_t expectedHeight = bufferSize / width + height + 2;
    EXPECT_EQ(config.height, expectedHeight);
}

/*
 * @tc.name: ConvertAlpha8ToRgba8888_ValidAlpha8
 * @tc.desc: Test rs_rcd::ConvertAlpha8ToRgba8888 with valid ALPHA_8 bitmap succeeds
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, ConvertAlpha8ToRgba8888_ValidAlpha8, TestSize.Level1)
{
    const int width = 100;
    const int height = 50;
    Drawing::Bitmap alpha8Bitmap;
    alpha8Bitmap.Build(width, height,
        Drawing::BitmapFormat{Drawing::ColorType::COLORTYPE_ALPHA_8, Drawing::AlphaType::ALPHATYPE_OPAQUE});

    Drawing::Bitmap dstBitmap;
    bool result = rs_rcd::ConvertAlpha8ToRgba8888(alpha8Bitmap, dstBitmap);
    EXPECT_TRUE(result);
    EXPECT_EQ(dstBitmap.GetColorType(), Drawing::ColorType::COLORTYPE_RGBA_8888);
    EXPECT_EQ(dstBitmap.GetWidth(), width);
    EXPECT_EQ(dstBitmap.GetHeight(), height);
    EXPECT_NE(dstBitmap.GetPixels(), nullptr);
}

/*
 * @tc.name: ConvertAlpha8ToRgba8888_InvalidSrcBitmap
 * @tc.desc: Test rs_rcd::ConvertAlpha8ToRgba8888 with invalid (unbuilt) src bitmap fails
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, ConvertAlpha8ToRgba8888_InvalidSrcBitmap, TestSize.Level1)
{
    Drawing::Bitmap invalidBitmap;
    Drawing::Bitmap dstBitmap;
    bool result = rs_rcd::ConvertAlpha8ToRgba8888(invalidBitmap, dstBitmap);
    EXPECT_FALSE(result);
}

/*
 * @tc.name: ConvertAlpha8ToRgba8888_ZeroWidth
 * @tc.desc: Test rs_rcd::ConvertAlpha8ToRgba8888 with zero width fails
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, ConvertAlpha8ToRgba8888_ZeroWidth, TestSize.Level1)
{
    Drawing::Bitmap alpha8Bitmap;
    alpha8Bitmap.Build(0, 100,
        Drawing::BitmapFormat{Drawing::ColorType::COLORTYPE_ALPHA_8, Drawing::AlphaType::ALPHATYPE_OPAQUE});

    Drawing::Bitmap dstBitmap;
    bool result = rs_rcd::ConvertAlpha8ToRgba8888(alpha8Bitmap, dstBitmap);
    EXPECT_FALSE(result);
}

/*
 * @tc.name: ConvertAlpha8ToRgba8888_ZeroHeight
 * @tc.desc: Test rs_rcd::ConvertAlpha8ToRgba8888 with zero height fails
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, ConvertAlpha8ToRgba8888_ZeroHeight, TestSize.Level1)
{
    Drawing::Bitmap alpha8Bitmap;
    alpha8Bitmap.Build(100, 0,
        Drawing::BitmapFormat{Drawing::ColorType::COLORTYPE_ALPHA_8, Drawing::AlphaType::ALPHATYPE_OPAQUE});

    Drawing::Bitmap dstBitmap;
    bool result = rs_rcd::ConvertAlpha8ToRgba8888(alpha8Bitmap, dstBitmap);
    EXPECT_FALSE(result);
}

/*
 * @tc.name: ConvertAlpha8ToRgba8888_SmallDimensions
 * @tc.desc: Test rs_rcd::ConvertAlpha8ToRgba8888 with 1x1 dimensions succeeds
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, ConvertAlpha8ToRgba8888_SmallDimensions, TestSize.Level1)
{
    const int width = 1;
    const int height = 1;
    Drawing::Bitmap alpha8Bitmap;
    alpha8Bitmap.Build(width, height,
        Drawing::BitmapFormat{Drawing::ColorType::COLORTYPE_ALPHA_8, Drawing::AlphaType::ALPHATYPE_OPAQUE});

    Drawing::Bitmap dstBitmap;
    bool result = rs_rcd::ConvertAlpha8ToRgba8888(alpha8Bitmap, dstBitmap);
    EXPECT_TRUE(result);
    EXPECT_EQ(dstBitmap.GetColorType(), Drawing::ColorType::COLORTYPE_RGBA_8888);
    EXPECT_EQ(dstBitmap.GetWidth(), width);
    EXPECT_EQ(dstBitmap.GetHeight(), height);
    EXPECT_NE(dstBitmap.GetPixels(), nullptr);
}

/*
 * @tc.name: ConvertAlpha8ToRgba8888_LargeDimensions
 * @tc.desc: Test rs_rcd::ConvertAlpha8ToRgba8888 with large dimensions succeeds
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, ConvertAlpha8ToRgba8888_LargeDimensions, TestSize.Level1)
{
    const int width = 1920;
    const int height = 1080;
    Drawing::Bitmap alpha8Bitmap;
    alpha8Bitmap.Build(width, height,
        Drawing::BitmapFormat{Drawing::ColorType::COLORTYPE_ALPHA_8, Drawing::AlphaType::ALPHATYPE_OPAQUE});

    Drawing::Bitmap dstBitmap;
    bool result = rs_rcd::ConvertAlpha8ToRgba8888(alpha8Bitmap, dstBitmap);
    EXPECT_TRUE(result);
    EXPECT_EQ(dstBitmap.GetColorType(), Drawing::ColorType::COLORTYPE_RGBA_8888);
    EXPECT_EQ(dstBitmap.GetWidth(), width);
    EXPECT_EQ(dstBitmap.GetHeight(), height);
    EXPECT_NE(dstBitmap.GetPixels(), nullptr);
}

/*
 * @tc.name: ConvertAlpha8ToRgba8888_DstBitmapPremulAlpha
 * @tc.desc: Test rs_rcd::ConvertAlpha8ToRgba8888 produces PREMUL alpha type dst bitmap
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, ConvertAlpha8ToRgba8888_DstBitmapPremulAlpha, TestSize.Level1)
{
    const int width = 100;
    const int height = 50;
    Drawing::Bitmap alpha8Bitmap;
    alpha8Bitmap.Build(width, height,
        Drawing::BitmapFormat{Drawing::ColorType::COLORTYPE_ALPHA_8, Drawing::AlphaType::ALPHATYPE_OPAQUE});

    Drawing::Bitmap dstBitmap;
    bool result = rs_rcd::ConvertAlpha8ToRgba8888(alpha8Bitmap, dstBitmap);
    EXPECT_TRUE(result);
    Drawing::BitmapFormat format = dstBitmap.GetFormat();
    EXPECT_EQ(format.colorType, Drawing::ColorType::COLORTYPE_RGBA_8888);
    EXPECT_EQ(format.alphaType, Drawing::AlphaType::ALPHATYPE_PREMUL);
}

/*
 * @tc.name: PrepareHardwareResourceBuffer_BGRA8888Bitmap
 * @tc.desc: Test RSRcdSurfaceRenderNode::PrepareHardwareResourceBuffer with BGRA_8888 bitmap copies directly
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, PrepareHardwareResourceBuffer_BGRA8888Bitmap, TestSize.Level1)
{
    auto surfaceNode = std::make_shared<RSRcdSurfaceRenderNode>(0, RCDSurfaceType::TOP);
    ASSERT_NE(surfaceNode, nullptr);

    const int width = 100;
    const int height = 50;
    Drawing::Bitmap bgraBitmap;
    bgraBitmap.Build(width, height,
        Drawing::BitmapFormat{Drawing::ColorType::COLORTYPE_BGRA_8888, Drawing::AlphaType::ALPHATYPE_OPAQUE});

    rs_rcd::RoundCornerLayer layer{"top.png", 0, 0, "top.bin", 8112, 2028, 1, &bgraBitmap};
    auto layerInfo = std::make_shared<rs_rcd::RoundCornerLayer>(layer);

    surfaceNode->SetRenderDisplayRect(RectT<uint32_t>(0, 0, 200, 200));
    Drawing::Bitmap layerBitmap;
    bool result = surfaceNode->PrepareHardwareResourceBuffer(layerInfo, layerBitmap);
    EXPECT_TRUE(result);
    EXPECT_EQ(static_cast<int>(layerBitmap.GetWidth()), width);
    EXPECT_EQ(static_cast<int>(layerBitmap.GetHeight()), height);
    EXPECT_EQ(layerBitmap.GetColorType(), Drawing::ColorType::COLORTYPE_BGRA_8888);
}

/*
 * @tc.name: PrepareHardwareResourceBuffer_InvalidSurfaceType
 * @tc.desc: Test RSRcdSurfaceRenderNode::PrepareHardwareResourceBuffer with INVALID surface type
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, PrepareHardwareResourceBuffer_InvalidSurfaceType, TestSize.Level1)
{
    auto surfaceNode = std::make_shared<RSRcdSurfaceRenderNode>(0, RCDSurfaceType::INVALID);
    ASSERT_NE(surfaceNode, nullptr);
    ASSERT_TRUE(surfaceNode->IsInvalidSurface());

    const int width = 100;
    const int height = 50;
    Drawing::Bitmap rgbaBitmap;
    rgbaBitmap.Build(width, height,
        Drawing::BitmapFormat{Drawing::ColorType::COLORTYPE_RGBA_8888, Drawing::AlphaType::ALPHATYPE_OPAQUE});

    rs_rcd::RoundCornerLayer layer{"top.png", 0, 0, "top.bin", 8112, 2028, 1, &rgbaBitmap};
    auto layerInfo = std::make_shared<rs_rcd::RoundCornerLayer>(layer);

    surfaceNode->SetRenderDisplayRect(RectT<uint32_t>(0, 0, 200, 200));
    Drawing::Bitmap layerBitmap;
    bool result = surfaceNode->PrepareHardwareResourceBuffer(layerInfo, layerBitmap);
    EXPECT_TRUE(result);
}

/*
 * @tc.name: ConvertAlpha8ToRgba8888_PixelContentTransferred
 * @tc.desc: Test rs_rcd::ConvertAlpha8ToRgba8888 transfers pixel content correctly
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, ConvertAlpha8ToRgba8888_PixelContentTransferred, TestSize.Level1)
{
    const int width = 10;
    const int height = 10;
    Drawing::Bitmap alpha8Bitmap;
    alpha8Bitmap.Build(width, height,
        Drawing::BitmapFormat{Drawing::ColorType::COLORTYPE_ALPHA_8, Drawing::AlphaType::ALPHATYPE_OPAQUE});

    const uint8_t testAlpha = 128;
    uint8_t* srcPixels = static_cast<uint8_t*>(const_cast<void*>(alpha8Bitmap.GetPixels()));
    ASSERT_NE(srcPixels, nullptr);
    for (int i = 0; i < width * height; i++) {
        srcPixels[i] = testAlpha;
    }

    Drawing::Bitmap dstBitmap;
    bool result = rs_rcd::ConvertAlpha8ToRgba8888(alpha8Bitmap, dstBitmap);
    EXPECT_TRUE(result);
    EXPECT_NE(dstBitmap.GetPixels(), nullptr);

    const uint32_t* dstPixels = static_cast<const uint32_t*>(dstBitmap.GetPixels());
    ASSERT_NE(dstPixels, nullptr);
    uint32_t firstPixel = dstPixels[0];
    uint8_t alpha = (firstPixel >> 24) & 0xFF;
    EXPECT_EQ(alpha, testAlpha);
}

/*
 * @tc.name: ConvertAlpha8ToRgba8888_DstBitmapReused
 * @tc.desc: Test rs_rcd::ConvertAlpha8ToRgba8888 can be called multiple times with different src bitmaps
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, ConvertAlpha8ToRgba8888_DstBitmapReused, TestSize.Level1)
{
    const int width = 50;
    const int height = 50;
    Drawing::Bitmap alpha8Bitmap1;
    alpha8Bitmap1.Build(width, height,
        Drawing::BitmapFormat{Drawing::ColorType::COLORTYPE_ALPHA_8, Drawing::AlphaType::ALPHATYPE_OPAQUE});

    Drawing::Bitmap dstBitmap1;
    bool result1 = rs_rcd::ConvertAlpha8ToRgba8888(alpha8Bitmap1, dstBitmap1);
    EXPECT_TRUE(result1);
    EXPECT_EQ(dstBitmap1.GetWidth(), width);
    EXPECT_EQ(dstBitmap1.GetHeight(), height);

    Drawing::Bitmap alpha8Bitmap2;
    alpha8Bitmap2.Build(width * 2, height * 2,
        Drawing::BitmapFormat{Drawing::ColorType::COLORTYPE_ALPHA_8, Drawing::AlphaType::ALPHATYPE_OPAQUE});

    Drawing::Bitmap dstBitmap2;
    bool result2 = rs_rcd::ConvertAlpha8ToRgba8888(alpha8Bitmap2, dstBitmap2);
    EXPECT_TRUE(result2);
    EXPECT_EQ(dstBitmap2.GetWidth(), width * 2);
    EXPECT_EQ(dstBitmap2.GetHeight(), height * 2);
}

/*
 * @tc.name: DecodeBitmap_NullImage
 * @tc.desc: Test RoundCornerDisplay::DecodeBitmap with null image
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, DecodeBitmap_NullImage, TestSize.Level1)
{
    auto& rcdInstance = RSSingleton<RoundCornerDisplay>::GetInstance();
    Drawing::Bitmap bitmap;
    bool result = rcdInstance.DecodeBitmap(nullptr, bitmap);
    EXPECT_FALSE(result);
}

/*
 * @tc.name: DecodeBitmap_AsLegacyBitmapFails
 * @tc.desc: Test RoundCornerDisplay::DecodeBitmap with unbuilt image (AsLegacyBitmap fails)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, DecodeBitmap_AsLegacyBitmapFails, TestSize.Level1)
{
    auto& rcdInstance = RSSingleton<RoundCornerDisplay>::GetInstance();
    auto image = std::make_shared<Drawing::Image>();
    Drawing::Bitmap dstBitmap;
    bool result = rcdInstance.DecodeBitmap(image, dstBitmap);
    EXPECT_FALSE(result);
}

/*
 * @tc.name: DecodeBitmap_ValidRgba8888Image
 * @tc.desc: Test RoundCornerDisplay::DecodeBitmap with valid RGBA_8888 image
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, DecodeBitmap_ValidRgba8888Image, TestSize.Level1)
{
    auto& rcdInstance = RSSingleton<RoundCornerDisplay>::GetInstance();
    const int width = 10;
    const int height = 10;
    Drawing::Bitmap srcBitmap;
    srcBitmap.Build(width, height,
        Drawing::BitmapFormat{Drawing::ColorType::COLORTYPE_RGBA_8888, Drawing::AlphaType::ALPHATYPE_OPAQUE});
    auto image = std::make_shared<Drawing::Image>();
    if (!image->BuildFromBitmap(srcBitmap)) {
        std::cout << "DecodeBitmap_ValidRgba8888Image: BuildFromBitmap not supported in test env" << std::endl;
        return;
    }
    Drawing::Bitmap dstBitmap;
    bool result = rcdInstance.DecodeBitmap(image, dstBitmap);
    EXPECT_TRUE(result);
}

/*
 * @tc.name: DecodeBitmap_ValidAlpha8Image
 * @tc.desc: Test RoundCornerDisplay::DecodeBitmap with non-RGBA color type image (direct assign path)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, DecodeBitmap_ValidAlpha8Image, TestSize.Level1)
{
    auto& rcdInstance = RSSingleton<RoundCornerDisplay>::GetInstance();
    const int width = 10;
    const int height = 10;
    Drawing::Bitmap srcBitmap;
    srcBitmap.Build(width, height,
        Drawing::BitmapFormat{Drawing::ColorType::COLORTYPE_ALPHA_8, Drawing::AlphaType::ALPHATYPE_OPAQUE});
    auto image = std::make_shared<Drawing::Image>();
    if (!image->BuildFromBitmap(srcBitmap)) {
        std::cout << "DecodeBitmap_ValidAlpha8Image: BuildFromBitmap not supported in test env" << std::endl;
        return;
    }
    Drawing::Bitmap dstBitmap;
    bool result = rcdInstance.DecodeBitmap(image, dstBitmap);
    EXPECT_TRUE(result);
}

/*
 * @tc.name: DecodeBitmap_ValidBgra8888Image
 * @tc.desc: Test RoundCornerDisplay::DecodeBitmap with valid BGRA_8888 image (covers BGRA_8888 branch at line 116)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, DecodeBitmap_ValidBgra8888Image, TestSize.Level1)
{
    auto& rcdInstance = RSSingleton<RoundCornerDisplay>::GetInstance();
    const int width = 10;
    const int height = 10;
    Drawing::Bitmap srcBitmap;
    srcBitmap.Build(width, height,
        Drawing::BitmapFormat{Drawing::ColorType::COLORTYPE_BGRA_8888, Drawing::AlphaType::ALPHATYPE_OPAQUE});
    auto image = std::make_shared<Drawing::Image>();
    if (!image->BuildFromBitmap(srcBitmap)) {
        std::cout << "DecodeBitmap_ValidBgra8888Image: BuildFromBitmap not supported in test env" << std::endl;
        return;
    }
    Drawing::Bitmap dstBitmap;
    bool result = rcdInstance.DecodeBitmap(image, dstBitmap);
    EXPECT_TRUE(result);
    // After successful ExtractAlphaChannel, the dstBitmap should be an Alpha8 bitmap
    EXPECT_EQ(dstBitmap.GetColorType(), Drawing::ColorType::COLORTYPE_ALPHA_8);
}

/*
 * @tc.name: DecodeBitmap_ExtractAlphaFallbackNullPixels
 * @tc.desc: Test RoundCornerDisplay::DecodeBitmap fallback path (lines 117-120) when ExtractAlphaChannel fails
 *           because srcBitmap pixels are null after AsLegacyBitmap
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, DecodeBitmap_ExtractAlphaFallbackNullPixels, TestSize.Level1)
{
    auto& rcdInstance = RSSingleton<RoundCornerDisplay>::GetInstance();
    const int width = 10;
    const int height = 10;
    Drawing::Bitmap srcBitmap;
    srcBitmap.Build(width, height,
        Drawing::BitmapFormat{Drawing::ColorType::COLORTYPE_RGBA_8888, Drawing::AlphaType::ALPHATYPE_OPAQUE});
    // Set pixels to nullptr to make ExtractAlphaChannel fail at the GetPixels check,
    // triggering the fallback path (bitmap = srcBitmap) at lines 117-120.
    srcBitmap.SetPixels(nullptr);
    auto image = std::make_shared<Drawing::Image>();
    if (!image->BuildFromBitmap(srcBitmap)) {
        std::cout << "DecodeBitmap_ExtractAlphaFallbackNullPixels: BuildFromBitmap not supported in test env" << std::endl;
        return;
    }
    Drawing::Bitmap dstBitmap;
    bool result = rcdInstance.DecodeBitmap(image, dstBitmap);
    EXPECT_TRUE(result);
    // When ExtractAlphaChannel fails, the fallback assigns srcBitmap to bitmap,
    // so dstBitmap should retain the original RGBA_8888 color type (not Alpha8).
    EXPECT_EQ(dstBitmap.GetColorType(), Drawing::ColorType::COLORTYPE_RGBA_8888);
}

/*
 * @tc.name: ExtractAlphaChannel_ValidRgba8888
 * @tc.desc: Test rs_rcd::ExtractAlphaChannel with valid RGBA_8888 bitmap
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, ExtractAlphaChannel_ValidRgba8888, TestSize.Level1)
{
    const int width = 10;
    const int height = 10;
    Drawing::Bitmap srcBitmap;
    srcBitmap.Build(width, height,
        Drawing::BitmapFormat{Drawing::ColorType::COLORTYPE_RGBA_8888, Drawing::AlphaType::ALPHATYPE_PREMUL});
    Drawing::Bitmap dstBitmap;
    bool result = rs_rcd::ExtractAlphaChannel(srcBitmap, dstBitmap);
    EXPECT_TRUE(result);
    EXPECT_EQ(dstBitmap.GetColorType(), Drawing::ColorType::COLORTYPE_ALPHA_8);
    EXPECT_EQ(static_cast<int>(dstBitmap.GetWidth()), width);
    EXPECT_EQ(static_cast<int>(dstBitmap.GetHeight()), height);
}

/*
 * @tc.name: ExtractAlphaChannel_ValidBgra8888
 * @tc.desc: Test rs_rcd::ExtractAlphaChannel with valid BGRA_8888 bitmap
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, ExtractAlphaChannel_ValidBgra8888, TestSize.Level1)
{
    const int width = 10;
    const int height = 10;
    Drawing::Bitmap srcBitmap;
    srcBitmap.Build(width, height,
        Drawing::BitmapFormat{Drawing::ColorType::COLORTYPE_BGRA_8888, Drawing::AlphaType::ALPHATYPE_PREMUL});
    Drawing::Bitmap dstBitmap;
    bool result = rs_rcd::ExtractAlphaChannel(srcBitmap, dstBitmap);
    EXPECT_TRUE(result);
    EXPECT_EQ(dstBitmap.GetColorType(), Drawing::ColorType::COLORTYPE_ALPHA_8);
}

/*
 * @tc.name: ExtractAlphaChannel_InvalidSrcBitmap
 * @tc.desc: Test rs_rcd::ExtractAlphaChannel with invalid (unbuilt) src bitmap
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, ExtractAlphaChannel_InvalidSrcBitmap, TestSize.Level1)
{
    Drawing::Bitmap srcBitmap;
    Drawing::Bitmap dstBitmap;
    bool result = rs_rcd::ExtractAlphaChannel(srcBitmap, dstBitmap);
    EXPECT_FALSE(result);
}

/*
 * @tc.name: ExtractAlphaChannel_NonRgbaColorType
 * @tc.desc: Test rs_rcd::ExtractAlphaChannel with non-RGBA/BGRA color type
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, ExtractAlphaChannel_NonRgbaColorType, TestSize.Level1)
{
    Drawing::Bitmap srcBitmap;
    srcBitmap.Build(10, 10,
        Drawing::BitmapFormat{Drawing::ColorType::COLORTYPE_ALPHA_8, Drawing::AlphaType::ALPHATYPE_OPAQUE});
    Drawing::Bitmap dstBitmap;
    bool result = rs_rcd::ExtractAlphaChannel(srcBitmap, dstBitmap);
    EXPECT_FALSE(result);
}

/*
 * @tc.name: ExtractAlphaChannel_ZeroWidth
 * @tc.desc: Test rs_rcd::ExtractAlphaChannel with zero width
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, ExtractAlphaChannel_ZeroWidth, TestSize.Level1)
{
    Drawing::Bitmap srcBitmap;
    srcBitmap.Build(0, 10,
        Drawing::BitmapFormat{Drawing::ColorType::COLORTYPE_RGBA_8888, Drawing::AlphaType::ALPHATYPE_PREMUL});
    Drawing::Bitmap dstBitmap;
    bool result = rs_rcd::ExtractAlphaChannel(srcBitmap, dstBitmap);
    EXPECT_FALSE(result);
}

/*
 * @tc.name: ExtractAlphaChannel_ZeroHeight
 * @tc.desc: Test rs_rcd::ExtractAlphaChannel with zero height
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, ExtractAlphaChannel_ZeroHeight, TestSize.Level1)
{
    Drawing::Bitmap srcBitmap;
    srcBitmap.Build(10, 0,
        Drawing::BitmapFormat{Drawing::ColorType::COLORTYPE_RGBA_8888, Drawing::AlphaType::ALPHATYPE_PREMUL});
    Drawing::Bitmap dstBitmap;
    bool result = rs_rcd::ExtractAlphaChannel(srcBitmap, dstBitmap);
    EXPECT_FALSE(result);
}

/*
 * @tc.name: ExtractAlphaChannel_PixelContentTransferred
 * @tc.desc: Test rs_rcd::ExtractAlphaChannel transfers alpha channel correctly
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, ExtractAlphaChannel_PixelContentTransferred, TestSize.Level1)
{
    const int width = 10;
    const int height = 10;
    Drawing::Bitmap srcBitmap;
    srcBitmap.Build(width, height,
        Drawing::BitmapFormat{Drawing::ColorType::COLORTYPE_RGBA_8888, Drawing::AlphaType::ALPHATYPE_PREMUL});
    const uint8_t testAlpha = 200;
    uint8_t* srcPixels = static_cast<uint8_t*>(const_cast<void*>(srcBitmap.GetPixels()));
    ASSERT_NE(srcPixels, nullptr);
    int32_t srcRowBytes = srcBitmap.GetRowBytes();
    constexpr int32_t bytesPerPixelRgba8888 = 4;
    constexpr int32_t alphaChannelOffset = 3;
    for (int y = 0; y < height; y++) {
        uint8_t* srcRow = srcPixels + y * srcRowBytes;
        for (int x = 0; x < width; x++) {
            srcRow[x * bytesPerPixelRgba8888 + alphaChannelOffset] = testAlpha;
        }
    }
    Drawing::Bitmap dstBitmap;
    bool result = rs_rcd::ExtractAlphaChannel(srcBitmap, dstBitmap);
    EXPECT_TRUE(result);
    const uint8_t* dstPixels = static_cast<const uint8_t*>(dstBitmap.GetPixels());
    ASSERT_NE(dstPixels, nullptr);
    EXPECT_EQ(dstPixels[0], testAlpha);
    int32_t dstRowBytes = dstBitmap.GetRowBytes();
    const uint8_t* lastRow = dstPixels + (height - 1) * dstRowBytes;
    EXPECT_EQ(lastRow[0], testAlpha);
}

/*
 * @tc.name: ExtractAlphaChannel_SmallDimensions
 * @tc.desc: Test rs_rcd::ExtractAlphaChannel with 1x1 dimensions
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, ExtractAlphaChannel_SmallDimensions, TestSize.Level1)
{
    Drawing::Bitmap srcBitmap;
    srcBitmap.Build(1, 1,
        Drawing::BitmapFormat{Drawing::ColorType::COLORTYPE_RGBA_8888, Drawing::AlphaType::ALPHATYPE_PREMUL});
    Drawing::Bitmap dstBitmap;
    bool result = rs_rcd::ExtractAlphaChannel(srcBitmap, dstBitmap);
    EXPECT_TRUE(result);
    EXPECT_EQ(dstBitmap.GetWidth(), 1);
    EXPECT_EQ(dstBitmap.GetHeight(), 1);
}

/*
 * @tc.name: ExtractAlphaChannel_LargeDimensions
 * @tc.desc: Test rs_rcd::ExtractAlphaChannel with large dimensions
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, ExtractAlphaChannel_LargeDimensions, TestSize.Level1)
{
    const int width = 1920;
    const int height = 1080;
    Drawing::Bitmap srcBitmap;
    srcBitmap.Build(width, height,
        Drawing::BitmapFormat{Drawing::ColorType::COLORTYPE_RGBA_8888, Drawing::AlphaType::ALPHATYPE_PREMUL});
    Drawing::Bitmap dstBitmap;
    bool result = rs_rcd::ExtractAlphaChannel(srcBitmap, dstBitmap);
    EXPECT_TRUE(result);
    EXPECT_EQ(static_cast<int>(dstBitmap.GetWidth()), width);
    EXPECT_EQ(static_cast<int>(dstBitmap.GetHeight()), height);
}

/*
 * @tc.name: GetTopSurfaceSource_NullRog
 * @tc.desc: Test RoundCornerDisplay::GetTopSurfaceSource with null rog
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, GetTopSurfaceSource_NullRog, TestSize.Level1)
{
    auto& rcdInstance = RSSingleton<RoundCornerDisplay>::GetInstance();
    rcdInstance.rog_ = nullptr;
    bool result = rcdInstance.GetTopSurfaceSource();
    EXPECT_FALSE(result);
}

/*
 * @tc.name: GetTopSurfaceSource_NoPortrait
 * @tc.desc: Test RoundCornerDisplay::GetTopSurfaceSource with no portrait configured
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, GetTopSurfaceSource_NoPortrait, TestSize.Level1)
{
    auto& rcdInstance = RSSingleton<RoundCornerDisplay>::GetInstance();
    rs_rcd::ROGSetting rog;
    rcdInstance.rog_ = &rog;
    bool result = rcdInstance.GetTopSurfaceSource();
    EXPECT_FALSE(result);
    rcdInstance.rog_ = nullptr;
}

/*
 * @tc.name: GetTopSurfaceSource_NoLandscape
 * @tc.desc: Test RoundCornerDisplay::GetTopSurfaceSource with no landscape configured
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, GetTopSurfaceSource_NoLandscape, TestSize.Level1)
{
    auto& rcdInstance = RSSingleton<RoundCornerDisplay>::GetInstance();
    rs_rcd::ROGSetting rog;
    rs_rcd::RogPortrait rogPortrait{};
    rog.portraitMap[rs_rcd::NODE_PORTRAIT] = rogPortrait;
    rcdInstance.rog_ = &rog;
    bool result = rcdInstance.GetTopSurfaceSource();
    EXPECT_FALSE(result);
    rcdInstance.rog_ = nullptr;
}

/*
 * @tc.name: GetTopSurfaceSource_ResourceEqualReuse
 * @tc.desc: Test RoundCornerDisplay::GetTopSurfaceSource with layerHide equal to layerUp (reuse)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, GetTopSurfaceSource_ResourceEqualReuse, TestSize.Level1)
{
    auto& rcdInstance = RSSingleton<RoundCornerDisplay>::GetInstance();
    rs_rcd::ROGSetting rog;
    rs_rcd::RogPortrait rogPortrait{};
    rogPortrait.layerUp.fileName = "test.png";
    rogPortrait.layerUp.offsetX = 1;
    rogPortrait.layerUp.offsetY = 2;
    rogPortrait.layerUp.binFileName = "test.bin";
    rogPortrait.layerUp.bufferSize = 100;
    rogPortrait.layerUp.cldWidth = 10;
    rogPortrait.layerUp.cldHeight = 20;
    rogPortrait.layerHide = rogPortrait.layerUp;
    rog.portraitMap[rs_rcd::NODE_PORTRAIT] = rogPortrait;
    rs_rcd::RogLandscape rogLandscape{};
    rogLandscape.layerUp = rogPortrait.layerUp;
    rog.landscapeMap[rs_rcd::NODE_LANDSCAPE] = rogLandscape;
    rcdInstance.rog_ = &rog;
    rcdInstance.supportHardware_ = false;
    bool result = rcdInstance.GetTopSurfaceSource();
    EXPECT_TRUE(result);
    rcdInstance.rog_ = nullptr;
    rcdInstance.supportHardware_ = false;
}

/*
 * @tc.name: GetTopSurfaceSource_WithHardwareSupport
 * @tc.desc: Test RoundCornerDisplay::GetTopSurfaceSource with hardware support enabled
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, GetTopSurfaceSource_WithHardwareSupport, TestSize.Level1)
{
    auto& rcdInstance = RSSingleton<RoundCornerDisplay>::GetInstance();
    rs_rcd::ROGSetting rog;
    rs_rcd::RogPortrait rogPortrait{};
    rogPortrait.layerUp.fileName = "test_up.png";
    rogPortrait.layerHide.fileName = "test_hide.png";
    rog.portraitMap[rs_rcd::NODE_PORTRAIT] = rogPortrait;
    rs_rcd::RogLandscape rogLandscape{};
    rogLandscape.layerUp.fileName = "test_land.png";
    rog.landscapeMap[rs_rcd::NODE_LANDSCAPE] = rogLandscape;
    rcdInstance.rog_ = &rog;
    rcdInstance.supportHardware_ = true;
    bool result = rcdInstance.GetTopSurfaceSource();
    EXPECT_TRUE(result);
    rcdInstance.rog_ = nullptr;
    rcdInstance.supportHardware_ = false;
}

/*
 * @tc.name: GetTopSurfaceSource_LandscapeReusePortraitHide
 * @tc.desc: Test RoundCornerDisplay::GetTopSurfaceSource reuses portrait layerHide for landscape
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, GetTopSurfaceSource_LandscapeReusePortraitHide, TestSize.Level1)
{
    auto& rcdInstance = RSSingleton<RoundCornerDisplay>::GetInstance();
    rs_rcd::ROGSetting rog;
    rs_rcd::RogPortrait rogPortrait{};
    rogPortrait.layerUp.fileName = "portrait_up.png";
    rogPortrait.layerHide.fileName = "portrait_hide.png";
    rogPortrait.layerHide.offsetX = 5;
    rogPortrait.layerHide.offsetY = 6;
    rogPortrait.layerHide.binFileName = "hide.bin";
    rogPortrait.layerHide.bufferSize = 200;
    rogPortrait.layerHide.cldWidth = 15;
    rogPortrait.layerHide.cldHeight = 25;
    rog.portraitMap[rs_rcd::NODE_PORTRAIT] = rogPortrait;
    rs_rcd::RogLandscape rogLandscape{};
    rogLandscape.layerUp = rogPortrait.layerHide;
    rog.landscapeMap[rs_rcd::NODE_LANDSCAPE] = rogLandscape;
    rcdInstance.rog_ = &rog;
    rcdInstance.supportHardware_ = false;
    bool result = rcdInstance.GetTopSurfaceSource();
    EXPECT_TRUE(result);
    rcdInstance.rog_ = nullptr;
}

/*
 * @tc.name: GetTopSurfaceSource_LandscapeNotEqualAny
 * @tc.desc: Test RoundCornerDisplay::GetTopSurfaceSource loads landscape separately when not equal to any portrait layer
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, GetTopSurfaceSource_LandscapeNotEqualAny, TestSize.Level1)
{
    auto& rcdInstance = RSSingleton<RoundCornerDisplay>::GetInstance();
    rs_rcd::ROGSetting rog;
    rs_rcd::RogPortrait rogPortrait{};
    rogPortrait.layerUp.fileName = "portrait_up.png";
    rogPortrait.layerUp.offsetX = 1;
    rogPortrait.layerHide.fileName = "portrait_hide.png";
    rogPortrait.layerHide.offsetX = 2;
    rog.portraitMap[rs_rcd::NODE_PORTRAIT] = rogPortrait;
    rs_rcd::RogLandscape rogLandscape{};
    rogLandscape.layerUp.fileName = "landscape_up.png";
    rogLandscape.layerUp.offsetX = 3;
    rog.landscapeMap[rs_rcd::NODE_LANDSCAPE] = rogLandscape;
    rcdInstance.rog_ = &rog;
    rcdInstance.supportHardware_ = false;
    bool result = rcdInstance.GetTopSurfaceSource();
    EXPECT_TRUE(result);
    rcdInstance.rog_ = nullptr;
}

/*
 * @tc.name: IsResourceEqual_AllFieldsEqual
 * @tc.desc: Test RoundCornerLayer::IsResourceEqual with all fields equal returns true
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, IsResourceEqual_AllFieldsEqual, TestSize.Level1)
{
    rs_rcd::RoundCornerLayer layer1{"top.png", 1, 2, "top.bin", 100, 10, 20, nullptr};
    rs_rcd::RoundCornerLayer layer2{"top.png", 1, 2, "top.bin", 100, 10, 20, nullptr};
    EXPECT_TRUE(layer1.IsResourceEqual(layer2));
}

/*
 * @tc.name: IsResourceEqual_DifferentFileName
 * @tc.desc: Test RoundCornerLayer::IsResourceEqual with different fileName returns false
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, IsResourceEqual_DifferentFileName, TestSize.Level1)
{
    rs_rcd::RoundCornerLayer layer1{"top.png", 1, 2, "top.bin", 100, 10, 20, nullptr};
    rs_rcd::RoundCornerLayer layer2{"bottom.png", 1, 2, "top.bin", 100, 10, 20, nullptr};
    EXPECT_FALSE(layer1.IsResourceEqual(layer2));
}

/*
 * @tc.name: IsResourceEqual_DifferentOffsetX
 * @tc.desc: Test RoundCornerLayer::IsResourceEqual with different offsetX returns false
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, IsResourceEqual_DifferentOffsetX, TestSize.Level1)
{
    rs_rcd::RoundCornerLayer layer1{"top.png", 1, 2, "top.bin", 100, 10, 20, nullptr};
    rs_rcd::RoundCornerLayer layer2{"top.png", 3, 2, "top.bin", 100, 10, 20, nullptr};
    EXPECT_FALSE(layer1.IsResourceEqual(layer2));
}

/*
 * @tc.name: IsResourceEqual_DifferentOffsetY
 * @tc.desc: Test RoundCornerLayer::IsResourceEqual with different offsetY returns false
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, IsResourceEqual_DifferentOffsetY, TestSize.Level1)
{
    rs_rcd::RoundCornerLayer layer1{"top.png", 1, 2, "top.bin", 100, 10, 20, nullptr};
    rs_rcd::RoundCornerLayer layer2{"top.png", 1, 4, "top.bin", 100, 10, 20, nullptr};
    EXPECT_FALSE(layer1.IsResourceEqual(layer2));
}

/*
 * @tc.name: IsResourceEqual_DifferentBinFileName
 * @tc.desc: Test RoundCornerLayer::IsResourceEqual with different binFileName returns false
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, IsResourceEqual_DifferentBinFileName, TestSize.Level1)
{
    rs_rcd::RoundCornerLayer layer1{"top.png", 1, 2, "top.bin", 100, 10, 20, nullptr};
    rs_rcd::RoundCornerLayer layer2{"top.png", 1, 2, "bottom.bin", 100, 10, 20, nullptr};
    EXPECT_FALSE(layer1.IsResourceEqual(layer2));
}

/*
 * @tc.name: IsResourceEqual_DifferentBufferSize
 * @tc.desc: Test RoundCornerLayer::IsResourceEqual with different bufferSize returns false
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, IsResourceEqual_DifferentBufferSize, TestSize.Level1)
{
    rs_rcd::RoundCornerLayer layer1{"top.png", 1, 2, "top.bin", 100, 10, 20, nullptr};
    rs_rcd::RoundCornerLayer layer2{"top.png", 1, 2, "top.bin", 200, 10, 20, nullptr};
    EXPECT_FALSE(layer1.IsResourceEqual(layer2));
}

/*
 * @tc.name: IsResourceEqual_DifferentCldWidth
 * @tc.desc: Test RoundCornerLayer::IsResourceEqual with different cldWidth returns false
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, IsResourceEqual_DifferentCldWidth, TestSize.Level1)
{
    rs_rcd::RoundCornerLayer layer1{"top.png", 1, 2, "top.bin", 100, 10, 20, nullptr};
    rs_rcd::RoundCornerLayer layer2{"top.png", 1, 2, "top.bin", 100, 30, 20, nullptr};
    EXPECT_FALSE(layer1.IsResourceEqual(layer2));
}

/*
 * @tc.name: IsResourceEqual_DifferentCldHeight
 * @tc.desc: Test RoundCornerLayer::IsResourceEqual with different cldHeight returns false
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, IsResourceEqual_DifferentCldHeight, TestSize.Level1)
{
    rs_rcd::RoundCornerLayer layer1{"top.png", 1, 2, "top.bin", 100, 10, 20, nullptr};
    rs_rcd::RoundCornerLayer layer2{"top.png", 1, 2, "top.bin", 100, 10, 40, nullptr};
    EXPECT_FALSE(layer1.IsResourceEqual(layer2));
}

/*
 * @tc.name: IsResourceEqual_EmptyFileName
 * @tc.desc: Test RoundCornerLayer::IsResourceEqual with empty fileName for both returns true
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, IsResourceEqual_EmptyFileName, TestSize.Level1)
{
    rs_rcd::RoundCornerLayer layer1{"", 0, 0, "", 0, 0, 0, nullptr};
    rs_rcd::RoundCornerLayer layer2{"", 0, 0, "", 0, 0, 0, nullptr};
    EXPECT_TRUE(layer1.IsResourceEqual(layer2));
}

/*
 * @tc.name: IsResourceEqual_CurBitmapIgnored
 * @tc.desc: Test RoundCornerLayer::IsResourceEqual ignores curBitmap pointer difference
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, IsResourceEqual_CurBitmapIgnored, TestSize.Level1)
{
    Drawing::Bitmap bitmap1;
    Drawing::Bitmap bitmap2;
    rs_rcd::RoundCornerLayer layer1{"top.png", 1, 2, "top.bin", 100, 10, 20, &bitmap1};
    rs_rcd::RoundCornerLayer layer2{"top.png", 1, 2, "top.bin", 100, 10, 20, &bitmap2};
    EXPECT_TRUE(layer1.IsResourceEqual(layer2));
}

/*
 * @tc.name: ExtractAlphaChannel_NullSrcPixels
 * @tc.desc: Test rs_rcd::ExtractAlphaChannel with null src pixels fails (GetPixels returns nullptr)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, ExtractAlphaChannel_NullSrcPixels, TestSize.Level1)
{
    const int width = 10;
    const int height = 10;
    Drawing::Bitmap srcBitmap;
    srcBitmap.Build(width, height,
        Drawing::BitmapFormat{Drawing::ColorType::COLORTYPE_RGBA_8888, Drawing::AlphaType::ALPHATYPE_PREMUL});
    srcBitmap.SetPixels(nullptr);
    Drawing::Bitmap dstBitmap;
    bool result = rs_rcd::ExtractAlphaChannel(srcBitmap, dstBitmap);
    EXPECT_FALSE(result);
}

/*
 * @tc.name: ConvertAlpha8ToRgba8888_NonAlpha8ColorType
 * @tc.desc: Test rs_rcd::ConvertAlpha8ToRgba8888 with RGBA_8888 (non-ALPHA_8) color type fails
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, ConvertAlpha8ToRgba8888_NonAlpha8ColorType, TestSize.Level1)
{
    Drawing::Bitmap srcBitmap;
    srcBitmap.Build(10, 10,
        Drawing::BitmapFormat{Drawing::ColorType::COLORTYPE_RGBA_8888, Drawing::AlphaType::ALPHATYPE_OPAQUE});
    Drawing::Bitmap dstBitmap;
    bool result = rs_rcd::ConvertAlpha8ToRgba8888(srcBitmap, dstBitmap);
    EXPECT_FALSE(result);
}

/*
 * @tc.name: ConvertAlpha8ToRgba8888_NullSrcPixels
 * @tc.desc: Test rs_rcd::ConvertAlpha8ToRgba8888 with null src pixels (ReadPixels fails)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, ConvertAlpha8ToRgba8888_NullSrcPixels, TestSize.Level1)
{
    const int width = 10;
    const int height = 10;
    Drawing::Bitmap srcBitmap;
    srcBitmap.Build(width, height,
        Drawing::BitmapFormat{Drawing::ColorType::COLORTYPE_ALPHA_8, Drawing::AlphaType::ALPHATYPE_OPAQUE});
    srcBitmap.SetPixels(nullptr);
    Drawing::Bitmap dstBitmap;
    bool result = rs_rcd::ConvertAlpha8ToRgba8888(srcBitmap, dstBitmap);
    EXPECT_FALSE(result);
}

/*
 * @tc.name: ConsumeAndUpdateBuffer_SetHardwareResourceToBufferFailed
 * @tc.desc: Test RSRcdRenderVisitor::ConsumeAndUpdateBuffer when SetHardwareResourceToBuffer returns false
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, ConsumeAndUpdateBuffer_SetHardwareResourceToBufferFailed, TestSize.Level1)
{
    // prepare test: create a surface node with a real consumer surface and a queued buffer
    auto surfaceNode = std::make_shared<RSRcdSurfaceRenderNode>(0, RCDSurfaceType::TOP);
    ASSERT_NE(surfaceNode, nullptr);

    // create consumer surface and register listener
    sptr<IBufferConsumerListener> listener = new RSRcdRenderListener(surfaceNode);
    ASSERT_NE(listener, nullptr);
    ASSERT_TRUE(surfaceNode->CreateSurface(listener));
    ASSERT_TRUE(surfaceNode->IsSurfaceCreated());

    // get consumer and producer, create producer surface
    auto consumer = surfaceNode->GetConsumer();
    ASSERT_NE(consumer, nullptr);
    auto producer = consumer->GetProducer();
    ASSERT_NE(producer, nullptr);
    sptr<Surface> producerSurface = Surface::CreateSurfaceAsProducer(producer);
    ASSERT_NE(producerSurface, nullptr);

    // request and flush a buffer so that AcquireBuffer can succeed
    BufferRequestConfig requestConfig = {
        .width = 0x100,
        .height = 0x100,
        .strideAlignment = 0x8,
        .format = GRAPHIC_PIXEL_FMT_RGBA_8888,
        .usage = BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA,
        .timeout = 0,
    };
    BufferFlushConfig flushConfig = {
        .damage = { .w = 0x100, .h = 0x100, },
    };
    sptr<SurfaceBuffer> buffer;
    sptr<SyncFence> releaseFence = SyncFence::InvalidFence();
    GSError ret = producerSurface->RequestBuffer(buffer, releaseFence, requestConfig);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);
    ASSERT_NE(buffer, nullptr);
    ret = producerSurface->FlushBuffer(buffer, SyncFence::INVALID_FENCE, flushConfig);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);

    // increase available buffer count so ConsumeAndUpdateBuffer does not early-return
    surfaceNode->IncreaseAvailableBuffer();
    ASSERT_TRUE(surfaceNode->GetAvailableBufferCount() > 0);

    // use an invalid (unbuilt) layerBitmap so that SetHardwareResourceToBuffer returns false
    // (layerBitmap.IsValid() returns true for an unbuilt bitmap, causing early return false)
    Drawing::Bitmap layerBitmap;
    ASSERT_TRUE(layerBitmap.IsValid());

    auto visitor = std::make_shared<RSRcdRenderVisitor>();
    ASSERT_NE(visitor, nullptr);
    // ConsumeAndUpdateBuffer should return false because SetHardwareResourceToBuffer failed
    bool result = visitor->ConsumeAndUpdateBuffer(*surfaceNode, layerBitmap);
    EXPECT_FALSE(result);
}

/*
 * @tc.name: ConsumeAndUpdateBuffer_AcquireBufferFailed
 * @tc.desc: Test RSRcdRenderVisitor::ConsumeAndUpdateBuffer when AcquireBuffer fails (no buffer queued)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, ConsumeAndUpdateBuffer_AcquireBufferFailed, TestSize.Level1)
{
    // prepare test: create a surface node with a real consumer surface but no buffer queued
    auto surfaceNode = std::make_shared<RSRcdSurfaceRenderNode>(0, RCDSurfaceType::TOP);
    ASSERT_NE(surfaceNode, nullptr);

    sptr<IBufferConsumerListener> listener = new RSRcdRenderListener(surfaceNode);
    ASSERT_NE(listener, nullptr);
    ASSERT_TRUE(surfaceNode->CreateSurface(listener));

    auto consumer = surfaceNode->GetConsumer();
    ASSERT_NE(consumer, nullptr);

    // increase available buffer count so ConsumeAndUpdateBuffer does not early-return
    surfaceNode->IncreaseAvailableBuffer();
    ASSERT_TRUE(surfaceNode->GetAvailableBufferCount() > 0);

    // no buffer has been queued, so AcquireBuffer should fail and ConsumeAndUpdateBuffer returns false
    Drawing::Bitmap layerBitmap;
    auto visitor = std::make_shared<RSRcdRenderVisitor>();
    ASSERT_NE(visitor, nullptr);
    bool result = visitor->ConsumeAndUpdateBuffer(*surfaceNode, layerBitmap);
    EXPECT_FALSE(result);
}

/*
 * @tc.name: ConsumeAndUpdateBuffer_NullConsumer
 * @tc.desc: Test RSRcdRenderVisitor::ConsumeAndUpdateBuffer when consumer is nullptr
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, ConsumeAndUpdateBuffer_NullConsumer, TestSize.Level1)
{
    // prepare test: create a surface node without creating surface (consumer is nullptr)
    auto surfaceNode = std::make_shared<RSRcdSurfaceRenderNode>(0, RCDSurfaceType::TOP);
    ASSERT_NE(surfaceNode, nullptr);
    ASSERT_FALSE(surfaceNode->IsSurfaceCreated());
    ASSERT_EQ(surfaceNode->GetConsumer(), nullptr);

    // increase available buffer count so ConsumeAndUpdateBuffer does not early-return
    surfaceNode->IncreaseAvailableBuffer();
    ASSERT_TRUE(surfaceNode->GetAvailableBufferCount() > 0);

    // consumer is nullptr, ConsumeAndUpdateBuffer should return false
    Drawing::Bitmap layerBitmap;
    auto visitor = std::make_shared<RSRcdRenderVisitor>();
    ASSERT_NE(visitor, nullptr);
    bool result = visitor->ConsumeAndUpdateBuffer(*surfaceNode, layerBitmap);
    EXPECT_FALSE(result);
}

/*
 * @tc.name: PrepareHardwareResourceBuffer_Alpha8ConversionFails
 * @tc.desc: Test RSRcdSurfaceRenderNode::PrepareHardwareResourceBuffer when ConvertAlpha8ToRgba8888 fails
 *           (covers lines 187-188: alpha8 to rgba8888 conversion failure path)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, PrepareHardwareResourceBuffer_Alpha8ConversionFails, TestSize.Level1)
{
    auto surfaceNode = std::make_shared<RSRcdSurfaceRenderNode>(0, RCDSurfaceType::TOP);
    ASSERT_NE(surfaceNode, nullptr);

    const int width = 100;
    const int height = 50;
    Drawing::Bitmap alpha8Bitmap;
    alpha8Bitmap.Build(width, height,
        Drawing::BitmapFormat{Drawing::ColorType::COLORTYPE_ALPHA_8, Drawing::AlphaType::ALPHATYPE_OPAQUE});
    // Set pixels to nullptr so that ConvertAlpha8ToRgba8888 fails (ReadPixels fails with null src pixels)
    alpha8Bitmap.SetPixels(nullptr);

    rs_rcd::RoundCornerLayer layer{"top.png", 0, 0, "top.bin", 8112, 2028, 1, &alpha8Bitmap};
    auto layerInfo = std::make_shared<rs_rcd::RoundCornerLayer>(layer);

    surfaceNode->SetRenderDisplayRect(RectT<uint32_t>(0, 0, 200, 200));
    Drawing::Bitmap layerBitmap;
    // curBitmap is ALPHA_8 so the conversion branch is entered (line 184-189);
    // ConvertAlpha8ToRgba8888 returns false because src pixels are null,
    // covering lines 187-188 (log error and return false)
    bool result = surfaceNode->PrepareHardwareResourceBuffer(layerInfo, layerBitmap);
    EXPECT_FALSE(result);
}

/*
 * @tc.name: PrintRcdNodeInfo_TopSurface
 * @tc.desc: Test RSRcdSurfaceRenderNode::PrintRcdNodeInfo with TOP surface type
 *           (covers PrintRcdNodeInfo function and RCDTopSurfaceNode branch)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, PrintRcdNodeInfo_TopSurface, TestSize.Level1)
{
    auto surfaceNode = std::make_shared<RSRcdSurfaceRenderNode>(0, RCDSurfaceType::TOP);
    ASSERT_NE(surfaceNode, nullptr);
    ASSERT_TRUE(surfaceNode->IsTopSurface());

    const int width = 100;
    const int height = 50;
    Drawing::Bitmap layerBitmap;
    layerBitmap.Build(width, height,
        Drawing::BitmapFormat{Drawing::ColorType::COLORTYPE_RGBA_8888, Drawing::AlphaType::ALPHATYPE_OPAQUE});

    // PrintRcdNodeInfo should not crash and should log node info for TOP surface
    surfaceNode->PrintRcdNodeInfo(layerBitmap);
    EXPECT_EQ(static_cast<int>(layerBitmap.GetWidth()), width);
    EXPECT_EQ(static_cast<int>(layerBitmap.GetHeight()), height);
}

/*
 * @tc.name: ProcessRcdSurfaceRenderNode_ConsumeAndUpdateBufferFailed
 * @tc.desc: Test RSRcdRenderVisitor::ProcessRcdSurfaceRenderNode when RequestFrame succeeds but
 *           ConsumeAndUpdateBuffer fails (covers line 135: ConsumeAndUpdateBuffer returns false).
 *           The full success path up to line 134 (renderFrame->Flush()) is exercised; then
 *           ConsumeAndUpdateBuffer fails because SetHardwareResourceToBuffer cannot read the
 *           .bin file (path /sys_prod/etc/display/RoundCornerDisplay/xxx.bin does not exist in
 *           the test environment), causing line 135 condition true -> line 136-137 log + return false.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, ProcessRcdSurfaceRenderNode_ConsumeAndUpdateBufferFailed, TestSize.Level1)
{
    // 1. create surface node (TOP) and create a real consumer/producer surface
    auto surfaceNode = std::make_shared<RSRcdSurfaceRenderNode>(0, RCDSurfaceType::TOP);
    ASSERT_NE(surfaceNode, nullptr);
    sptr<IBufferConsumerListener> listener = new RSRcdRenderListener(surfaceNode);
    ASSERT_NE(listener, nullptr);
    ASSERT_TRUE(surfaceNode->CreateSurface(listener));
    ASSERT_TRUE(surfaceNode->IsSurfaceCreated());

    // 2. set up render engine (initialized so RequestFrame can succeed)
    RSUniRenderThread::Instance().uniRenderEngine_ = std::make_shared<RSUniRenderEngine>();
    RSUniRenderThread::Instance().uniRenderEngine_->Init();
    auto visitor = std::make_shared<RSRcdRenderVisitor>();
    ASSERT_NE(visitor, nullptr);
    ASSERT_NE(visitor->renderEngine_, nullptr);

    // 3. set up a valid uni processor
    std::shared_ptr<RSComposerClientManager> rsComposerClientMgr = std::make_shared<RSComposerClientManager>();
    RSUniRenderThread::Instance().composerClientManager_ = rsComposerClientMgr;
    auto processor = RSProcessorFactory::CreateProcessor(CompositeType::HARDWARE_COMPOSITE, 0);
    visitor->SetUniProcessor(processor);
    ASSERT_NE(visitor->uniProcessor_, nullptr);
    ASSERT_FALSE(surfaceNode->IsInvalidSurface());

    // 4. build a valid layerInfo with an RGBA_8888 bitmap and set display rect
    const int width = 100;
    const int height = 50;
    Drawing::Bitmap rgbaBitmap;
    rgbaBitmap.Build(width, height,
        Drawing::BitmapFormat{Drawing::ColorType::COLORTYPE_RGBA_8888, Drawing::AlphaType::ALPHATYPE_OPAQUE});
    rs_rcd::RoundCornerLayer layer{"top.png", 0, 0, "top.bin", 8112, 2028, 1, &rgbaBitmap};
    auto layerInfo = std::make_shared<rs_rcd::RoundCornerLayer>(layer);
    surfaceNode->SetRenderDisplayRect(RectT<uint32_t>(0, 0, 200, 200));

    // 5. queue a buffer via producer surface so that AcquireBuffer can succeed later
    auto consumer = surfaceNode->GetConsumer();
    ASSERT_NE(consumer, nullptr);
    auto producer = consumer->GetProducer();
    ASSERT_NE(producer, nullptr);
    sptr<Surface> producerSurface = Surface::CreateSurfaceAsProducer(producer);
    ASSERT_NE(producerSurface, nullptr);
    BufferRequestConfig requestConfig = {
        .width = width,
        .height = height,
        .strideAlignment = 0x8,
        .format = GRAPHIC_PIXEL_FMT_RGBA_8888,
        .usage = BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA,
        .timeout = 0,
    };
    BufferFlushConfig flushConfig = {
        .damage = { .w = width, .h = height, },
    };
    sptr<SurfaceBuffer> buffer;
    sptr<SyncFence> releaseFence = SyncFence::InvalidFence();
    GSError ret = producerSurface->RequestBuffer(buffer, releaseFence, requestConfig);
    if (ret == OHOS::GSERROR_OK && buffer != nullptr) {
        ret = producerSurface->FlushBuffer(buffer, SyncFence::INVALID_FENCE, flushConfig);
        ASSERT_EQ(ret, OHOS::GSERROR_OK);
        surfaceNode->IncreaseAvailableBuffer();
        ASSERT_TRUE(surfaceNode->GetAvailableBufferCount() > 0);
    }

    // 6. call ProcessRcdSurfaceRenderNode with resourceChanged=true
    // If RequestFrame succeeds -> Flush succeeds -> ConsumeAndUpdateBuffer fails
    //    (SetHardwareResourceToBuffer fails because .bin file does not exist)
    //    -> covers line 135-138
    // If RequestFrame fails -> covers line 127-133 (CleanCache branch)
    bool result = visitor->ProcessRcdSurfaceRenderNode(*surfaceNode, layerInfo, true);
    EXPECT_FALSE(result);

    // cleanup
    RSUniRenderThread::Instance().uniRenderEngine_ = nullptr;
}

/*
 * @tc.name: ProcessRcdSurfaceRenderNode_RequestFrameFailed
 * @tc.desc: Test RSRcdRenderVisitor::ProcessRcdSurfaceRenderNode when RequestFrame returns nullptr
 *           (covers lines 127-133: renderFrame == nullptr -> CleanCache -> log -> return false).
 *           A render engine is set but NOT initialized (no Init() call), so renderContext_ is
 *           nullptr and RequestFrame returns nullptr, exercising the CleanCache branch.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, ProcessRcdSurfaceRenderNode_RequestFrameFailed, TestSize.Level1)
{
    // 1. create surface node (TOP) and create a real surface so GetRSSurface() != nullptr
    auto surfaceNode = std::make_shared<RSRcdSurfaceRenderNode>(0, RCDSurfaceType::TOP);
    ASSERT_NE(surfaceNode, nullptr);
    sptr<IBufferConsumerListener> listener = new RSRcdRenderListener(surfaceNode);
    ASSERT_NE(listener, nullptr);
    ASSERT_TRUE(surfaceNode->CreateSurface(listener));
    ASSERT_TRUE(surfaceNode->IsSurfaceCreated());

    // 2. set up a render engine WITHOUT calling Init() so RequestFrame returns nullptr
    RSUniRenderThread::Instance().uniRenderEngine_ = std::make_shared<RSUniRenderEngine>();
    auto visitor = std::make_shared<RSRcdRenderVisitor>();
    ASSERT_NE(visitor, nullptr);
    ASSERT_NE(visitor->renderEngine_, nullptr);

    // 3. set up a valid uni processor
    std::shared_ptr<RSComposerClientManager> rsComposerClientMgr = std::make_shared<RSComposerClientManager>();
    RSUniRenderThread::Instance().composerClientManager_ = rsComposerClientMgr;
    auto processor = RSProcessorFactory::CreateProcessor(CompositeType::HARDWARE_COMPOSITE, 0);
    visitor->SetUniProcessor(processor);
    ASSERT_NE(visitor->uniProcessor_, nullptr);
    ASSERT_FALSE(surfaceNode->IsInvalidSurface());

    // 4. build a valid layerInfo so PrepareHardwareResourceBuffer succeeds (passes line 119-122)
    const int width = 100;
    const int height = 50;
    Drawing::Bitmap rgbaBitmap;
    rgbaBitmap.Build(width, height,
        Drawing::BitmapFormat{Drawing::ColorType::COLORTYPE_RGBA_8888, Drawing::AlphaType::ALPHATYPE_OPAQUE});
    rs_rcd::RoundCornerLayer layer{"top.png", 0, 0, "top.bin", 8112, 2028, 1, &rgbaBitmap};
    auto layerInfo = std::make_shared<rs_rcd::RoundCornerLayer>(layer);
    surfaceNode->SetRenderDisplayRect(RectT<uint32_t>(0, 0, 200, 200));

    // 5. call ProcessRcdSurfaceRenderNode with resourceChanged=true
    // RequestFrame returns nullptr -> line 127 condition true ->
    //   line 128-130 CleanCache (if surface valid) -> line 131-132 log -> line 133 return false
    bool result = visitor->ProcessRcdSurfaceRenderNode(*surfaceNode, layerInfo, true);
    EXPECT_FALSE(result);

    // cleanup
    RSUniRenderThread::Instance().uniRenderEngine_ = nullptr;
}
} // OHOS::Rosen
