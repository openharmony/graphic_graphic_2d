/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "hdi_output.h"
#include "hdilayer_context_systest.h"
#include "mock_hdi_device_systest.h"
#include <gtest/gtest.h>

using namespace testing;
using namespace testing::ext;
using namespace OHOS::Rosen::MockSys;

namespace OHOS {
namespace Rosen {
class HdiOutputSysTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    static inline std::shared_ptr<HdiOutput> hdiOutput_;
    static inline MockSys::HdiDeviceMock* mockDevice_ = nullptr;
    static inline std::vector<LayerInfoPtr> layerInfos_;
    static inline std::shared_ptr<HdiLayerContext> hdiLayerTemp_;
};

void HdiOutputSysTest::SetUpTestCase()
{
    uint32_t screenId = 0;
    hdiOutput_ = HdiOutput::CreateHdiOutput(screenId);

    int32_t width = 50;
    int32_t height = 50;
    GraphicIRect srcRect = {0, 0, width, height};
    GraphicIRect dstRect = {0, 0, width, height};
    uint32_t zOrder = 0;
    std::shared_ptr<HdiLayerContext> hdiLayerTemp_ = std::make_unique<HdiLayerContext>(dstRect, srcRect, zOrder);
    hdiLayerTemp_->DrawBufferColor();
    hdiLayerTemp_->FillHdiLayer();
    hdiLayerTemp_->GetHdiLayer()->SetCompositionType(GraphicCompositionType::GRAPHIC_COMPOSITION_CLIENT);
    layerInfos_.emplace_back(hdiLayerTemp_->GetHdiLayer());

    zOrder = 1;
    hdiLayerTemp_ = std::make_unique<HdiLayerContext>(dstRect, srcRect, zOrder);
    hdiLayerTemp_->DrawBufferColor();
    hdiLayerTemp_->FillHdiLayer();
    hdiLayerTemp_->GetHdiLayer()->SetCompositionType(GraphicCompositionType::GRAPHIC_COMPOSITION_CLIENT);
    layerInfos_.emplace_back(hdiLayerTemp_->GetHdiLayer());

    mockDevice_ = MockSys::HdiDeviceMock::GetInstance();
    hdiOutput_->SetHdiOutputDevice(mockDevice_);
}

void HdiOutputSysTest::TearDownTestCase() {}

namespace {
/*
* Function: PreProcessLayersComp001
* Type: Function
* Rank: Important(1)
* EnvConditions: N/A
* CaseDescription: 1. call PreProcessLayersComp()
*                  2. check ret
*/
HWTEST_F(HdiOutputSysTest, PreProcessLayersComp001, Function | MediumTest| Level1)
{
    // layerIdMap is null in hdiouput
    ASSERT_EQ(HdiOutputSysTest::hdiOutput_->PreProcessLayersComp(), GRAPHIC_DISPLAY_PARAM_ERR);

    // layerIdMap is not nullptr in hdiouput
    HdiOutputSysTest::hdiOutput_->SetLayerInfo(layerInfos_);
    EXPECT_CALL(*mockDevice_, PrepareScreenLayers(_, _)).WillRepeatedly(testing::Return(1));
    ASSERT_EQ(HdiOutputSysTest::hdiOutput_->PreProcessLayersComp(), GRAPHIC_DISPLAY_FAILURE);

    EXPECT_CALL(*mockDevice_, PrepareScreenLayers(_, _)).WillRepeatedly(testing::Return(0));
    EXPECT_CALL(*mockDevice_, GetScreenCompChange(_, _, _)).WillRepeatedly(testing::Return(1));
    ASSERT_EQ(HdiOutputSysTest::hdiOutput_->PreProcessLayersComp(), 1);

    HdiOutputSysTest::hdiOutput_->SetLayerCompCapacity(1);
    ASSERT_EQ(HdiOutputSysTest::hdiOutput_->PreProcessLayersComp(), GRAPHIC_DISPLAY_SUCCESS);
}

/*
* Function: UpdateLayerCompType001
* Type: Function
* Rank: Important(1)
* EnvConditions: N/A
* CaseDescription: 1. call UpdateLayerCompType()
*                  2. check ret
*/
HWTEST_F(HdiOutputSysTest, UpdateLayerCompType001, Function | MediumTest| Level1)
{
    EXPECT_CALL(*mockDevice_, GetScreenCompChange(_, _, _)).WillRepeatedly(testing::Return(0));
    ASSERT_EQ(HdiOutputSysTest::hdiOutput_->UpdateLayerCompType(), 0);
}

/*
* Function: UpdateInfosAfterCommit001
* Type: Function
* Rank: Important(1)
* EnvConditions: N/A
* CaseDescription: 1. call UpdateInfosAfterCommit()
*                  2. check ret
*/
HWTEST_F(HdiOutputSysTest, UpdateInfosAfterCommit001, Function | MediumTest| Level1)
{
    sptr<SyncFence> fbFence = SyncFence::INVALID_FENCE;
    EXPECT_CALL(*mockDevice_, SetScreenVsyncEnabled(_, _)).WillRepeatedly(testing::Return(0));
    ASSERT_EQ(HdiOutputSysTest::hdiOutput_->UpdateInfosAfterCommit(fbFence), 0);
}

/*
* Function: GetLayersReleaseFence001
* Type: Function
* Rank: Important(1)
* EnvConditions: N/A
* CaseDescription: 1. call GetLayersReleaseFence
*                  2. check ret
*/
HWTEST_F(HdiOutputSysTest, GetLayersReleaseFence001, Function | MediumTest| Level3)
{
    std::map<LayerInfoPtr, sptr<SyncFence>> res = HdiOutputSysTest::hdiOutput_->GetLayersReleaseFence();
    ASSERT_EQ(res.size(), 0);
}

/*
* Function: FlushScreen001
* Type: Function
* Rank: Important(1)
* EnvConditions: N/A
* CaseDescription: 1. call FlushScreen()
*                  2. check ret
*/
HWTEST_F(HdiOutputSysTest, FlushScreen001, Function | MediumTest| Level1)
{
    std::vector<LayerPtr> compClientLayers = {};
    // frame buffer is nullptr
    ASSERT_EQ(HdiOutputSysTest::hdiOutput_->FlushScreen(compClientLayers), -1);
}

/*
* Function: ReleaseFramebuffer001
* Type: Function
* Rank: Important(1)
* EnvConditions: N/A
* CaseDescription: 1. call ReleaseFramebuffer()
*                  2. check ret
*/
HWTEST_F(HdiOutputSysTest, ReleaseFramebuffer001, Function | MediumTest| Level1)
{
    sptr<SyncFence> releaseFence;
    // currentr frame buffer_ is nullptr
    ASSERT_EQ(HdiOutputSysTest::hdiOutput_->ReleaseFramebuffer(releaseFence), GRAPHIC_DISPLAY_NULL_PTR);
}

/*
* Function: SetHdiOutputDevice001
* Type: Function
* Rank: Important(1)
* EnvConditions: N/A
* CaseDescription: 1. call SetHdiOutputDevice
*                  2. check ret
*/
HWTEST_F(HdiOutputSysTest, SetHdiOutputDevice001, Function | MediumTest| Level3)
{
    // mockDevice_ is nullptr
    ASSERT_EQ(HdiOutputSysTest::hdiOutput_->SetHdiOutputDevice(nullptr), ROSEN_ERROR_INVALID_ARGUMENTS);
    // the device_ in hdiBackend_ is not nullptr alredy
    ASSERT_EQ(HdiOutputSysTest::hdiOutput_->SetHdiOutputDevice(mockDevice_), ROSEN_ERROR_OK);
    // Init the hdiouput
    ASSERT_EQ(HdiOutputSysTest::hdiOutput_->Init(), ROSEN_ERROR_OK);
}

/*
* Function: FlushScreen002
* Type: Function
* Rank: Important(1)
* EnvConditions: N/A
* CaseDescription: 1. call FlushScreen()
*                  2. check ret
*/
HWTEST_F(HdiOutputSysTest, FlushScreen002, Function | MediumTest| Level1)
{
    std::vector<LayerPtr> compClientLayers;
    HdiOutputSysTest::hdiOutput_->GetComposeClientLayers(compClientLayers);
    // frame buffer is nullptr
    ASSERT_EQ(HdiOutputSysTest::hdiOutput_->FlushScreen(compClientLayers), -1);

    sptr<Surface> frameSurface = HdiOutputSysTest::hdiOutput_->GetFrameBufferSurface();
    sptr<SurfaceBuffer> buffer;
    int32_t releaseFence = -1;
    BufferRequestConfig config = {
        .width = 50,
        .height = 50,
        .strideAlignment = 0x8,
        .format = GRAPHIC_PIXEL_FMT_RGBA_8888,
        .usage = (BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA),
    };
    ASSERT_EQ(frameSurface->RequestBuffer(buffer, releaseFence, config), GSERROR_OK);
    BufferFlushConfig flushConfig = {
        .damage = {
        .w = 50,
        .h = 50,
        },
    };
    ASSERT_EQ(frameSurface->FlushBuffer(buffer, -1, flushConfig), GSERROR_OK); // frame buffer is not nullptr

    EXPECT_CALL(*mockDevice_, SetScreenClientBuffer(_, _, _, _)).WillRepeatedly(testing::Return(0));
    EXPECT_CALL(*mockDevice_, SetScreenClientDamage(_, _)).WillRepeatedly(testing::Return(0));
    ASSERT_EQ(HdiOutputSysTest::hdiOutput_->FlushScreen(compClientLayers), 0);
}

/*
* Function: ReleaseFramebuffer002
* Type: Function
* Rank: Important(1)
* EnvConditions: N/A
* CaseDescription: 1. call ReleaseFramebuffer()
*                  2. check ret
*/
HWTEST_F(HdiOutputSysTest, ReleaseFramebuffer002, Function | MediumTest| Level1)
{
    // last frame buffer is nullptr
    sptr<SyncFence> releaseFence = SyncFence::INVALID_FENCE;
    ASSERT_EQ(HdiOutputSysTest::hdiOutput_->ReleaseFramebuffer(releaseFence), 0);

    // last frame buffer is not nullptr
    ASSERT_EQ(HdiOutputSysTest::hdiOutput_->ReleaseFramebuffer(releaseFence), GRAPHIC_DISPLAY_NULL_PTR);
}

/*
* Function: TestHdiOutput001
* Type: Function
* Rank: Important(1)
* EnvConditions: N/A
* CaseDescription: 1. call HdiOutputFunc
*                  2. check ret
*/
HWTEST_F(HdiOutputSysTest, TestHdiOutput001, Function | MediumTest| Level3)
{
    ASSERT_EQ(HdiOutputSysTest::hdiOutput_->GetScreenId(), 0u);

    std::string dumpStr = "";
    HdiOutputSysTest::hdiOutput_->Dump(dumpStr);
    std::size_t found = dumpStr.find("FrameBufferSurface");
    ASSERT_NE(found, -1);

    std::string dumpFpsStr = "";
    HdiOutputSysTest::hdiOutput_->DumpFps(dumpFpsStr, "composer");
    HdiOutputSysTest::hdiOutput_->DumpFps(dumpFpsStr, "noname");
    found = dumpFpsStr.find("screen");
    ASSERT_NE(found, -1);

    std::string clearFpsStr = "";
    HdiOutputSysTest::hdiOutput_->ClearFpsDump(clearFpsStr, "composer");
    found = clearFpsStr.find("screen");
    ASSERT_NE(found, -1);

    ASSERT_NE(HdiOutputSysTest::hdiOutput_->GetFrameBufferSurface(), nullptr);
    ASSERT_EQ(HdiOutputSysTest::hdiOutput_->GetFramebuffer(), nullptr);
}

/*
* Function: GetLayersReleaseFence002
* Type: Function
* Rank: Important(1)
* EnvConditions: N/A
* CaseDescription: 1. call GetLayersReleaseFence
*                  2. check ret
*/
HWTEST_F(HdiOutputSysTest, GetLayersReleaseFence002, Function | MediumTest| Level3)
{
    EXPECT_CALL(*mockDevice_, GetScreenReleaseFence(_, _, _)).WillRepeatedly(testing::Return(1));
    std::map<LayerInfoPtr, sptr<SyncFence>> res = HdiOutputSysTest::hdiOutput_->GetLayersReleaseFence();
    ASSERT_EQ(res.size(), 0);

    EXPECT_CALL(*mockDevice_, GetScreenReleaseFence(_, _, _)).WillRepeatedly(testing::Return(0));
    res = HdiOutputSysTest::hdiOutput_->GetLayersReleaseFence();
    ASSERT_EQ(res.size(), 0);
}
} // namespace
} // namespace Rosen
} // namespace OHOS