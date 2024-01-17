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
    ASSERT_NE(imgBottomPortrait, nullptr);
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

/*
 * @tc.name: MessageBus
 * @tc.desc: Test RSRoundCornerDisplayTest.MessageBus
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, MessageBus, TestSize.Level1)
{
    auto& rcdInstance = RSSingleton<RoundCornerDisplay>::GetInstance();
    auto& msgBus = RSSingleton<RsMessageBus>::GetInstance();
    msgBus.RegisterTopic<uint32_t, uint32_t>(
        TOPIC_RCD_DISPLAY_SIZE, &rcdInstance,
        &RoundCornerDisplay::UpdateDisplayParameter);
    msgBus.RegisterTopic<ScreenRotation>(
        TOPIC_RCD_DISPLAY_ROTATION, &rcdInstance,
        &RoundCornerDisplay::UpdateOrientationStatus);
    msgBus.RegisterTopic<int>(
        TOPIC_RCD_DISPLAY_NOTCH, &rcdInstance,
        &RoundCornerDisplay::UpdateNotchStatus);

    msgBus.SendMsg(TOPIC_RCD_DISPLAY_SIZE, 0, 0);

    msgBus.SendMsg(TOPIC_RCD_DISPLAY_ROTATION, static_cast<ScreenRotation>(0));

    msgBus.SendMsg(TOPIC_RCD_DISPLAY_NOTCH, static_cast<int>(0));

    msgBus.RemoveTopic(TOPIC_RCD_DISPLAY_SIZE);

    msgBus.RemoveTopic(TOPIC_RCD_DISPLAY_ROTATION);

    msgBus.RemoveTopic(TOPIC_RCD_DISPLAY_NOTCH);

    msgBus.RemoveTopic("NG_TOPIC");
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
    ASSERT_NE(defaultLcd, nullptr);
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
    ASSERT_NE(defaultLcd, nullptr);
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
    ASSERT_NE(okResult, false);
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
