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

#include "hdi_device.h"
#include <gtest/gtest.h>

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class HdiDeviceTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();

    static inline HdiDevice* hdiDevice_ = nullptr;
};

void HdiDeviceTest::SetUpTestCase()
{
    hdiDevice_ = HdiDevice::GetInstance();
    hdiDevice_->ResetHdiFuncs();
}

void HdiDeviceTest::TearDownTestCase()
{
    hdiDevice_ = nullptr;
}

namespace {
/*
* Function: all DeviceFuncs
* Type: Function
* Rank: Important(3)
* EnvConditions: N/A
* CaseDescription: 1. call all DeviceFuncs
*                  2. check ret
*/
HWTEST_F(HdiDeviceTest, DeviceFuncs001, Function | MediumTest| Level3)
{
    HotPlugCallback hotPlugCallback = nullptr;
    ASSERT_EQ(HdiDeviceTest::hdiDevice_->RegHotPlugCallback(hotPlugCallback, nullptr), DISPLAY_NULL_PTR);
    uint32_t screenId = 0, screenModeId = 0, screenLightLevel = 0, num = 1;
    VBlankCallback vblCallback = nullptr;
    ASSERT_EQ(HdiDeviceTest::hdiDevice_->RegScreenVBlankCallback(screenId, vblCallback, nullptr), DISPLAY_NULL_PTR);
    bool enabled = false, needFlush = false;
    ASSERT_EQ(HdiDeviceTest::hdiDevice_->SetScreenVsyncEnabled(screenId, enabled), DISPLAY_NULL_PTR);
    DisplayCapability dcpInfo;
    ASSERT_EQ(HdiDeviceTest::hdiDevice_->GetScreenCapability(screenId, dcpInfo), DISPLAY_NULL_PTR);
    std::vector<DisplayModeInfo> dmodes;
    ASSERT_EQ(HdiDeviceTest::hdiDevice_->GetScreenSupportedModes(screenId, dmodes), DISPLAY_NULL_PTR);
    ASSERT_EQ(HdiDeviceTest::hdiDevice_->GetScreenMode(screenId, screenModeId), DISPLAY_NULL_PTR);
    ASSERT_EQ(HdiDeviceTest::hdiDevice_->SetScreenMode(screenId, screenModeId), DISPLAY_NULL_PTR);
    DispPowerStatus dstatus = POWER_STATUS_ON;
    ASSERT_EQ(HdiDeviceTest::hdiDevice_->GetScreenPowerStatus(screenId, dstatus), DISPLAY_NULL_PTR);
    ASSERT_EQ(HdiDeviceTest::hdiDevice_->SetScreenPowerStatus(screenId, dstatus), DISPLAY_NULL_PTR);
    ASSERT_EQ(HdiDeviceTest::hdiDevice_->GetScreenBacklight(screenId, screenLightLevel), DISPLAY_NULL_PTR);
    ASSERT_EQ(HdiDeviceTest::hdiDevice_->SetScreenBacklight(screenId, screenLightLevel), DISPLAY_NULL_PTR);
    ASSERT_EQ(HdiDeviceTest::hdiDevice_->PrepareScreenLayers(screenId, needFlush), DISPLAY_NULL_PTR);
    std::vector<uint32_t> layersId;
    std::vector<int32_t> types;
    ASSERT_EQ(HdiDeviceTest::hdiDevice_->GetScreenCompChange(screenId, layersId, types), DISPLAY_NULL_PTR);
    BufferHandle *buffer = nullptr;
    sptr<SyncFence> fence = nullptr;
    ASSERT_EQ(HdiDeviceTest::hdiDevice_->SetScreenClientBuffer(screenId, buffer, fence), DISPLAY_NULL_PTR);
    IRect damageRect = {0, 0, 0, 0};
    ASSERT_EQ(HdiDeviceTest::hdiDevice_->SetScreenClientDamage(screenId, num, damageRect), DISPLAY_NULL_PTR);
    std::vector<uint32_t> layers;
    std::vector<sptr<SyncFence>> fences;
    ASSERT_EQ(HdiDeviceTest::hdiDevice_->GetScreenReleaseFence(screenId, layers, fences), DISPLAY_NULL_PTR);
    std::vector<GraphicColorGamut> gamuts;
    ASSERT_EQ(HdiDeviceTest::hdiDevice_->GetScreenSupportedColorGamuts(screenId, gamuts), DISPLAY_NULL_PTR);
    GraphicColorGamut gamut = GRAPHIC_COLOR_GAMUT_INVALID;
    ASSERT_EQ(HdiDeviceTest::hdiDevice_->SetScreenColorGamut(screenId, gamut), DISPLAY_NULL_PTR);
    ASSERT_EQ(HdiDeviceTest::hdiDevice_->GetScreenColorGamut(screenId, gamut), DISPLAY_NULL_PTR);
    GamutMap gamutMap = GAMUT_MAP_CONSTANT;
    ASSERT_EQ(HdiDeviceTest::hdiDevice_->SetScreenGamutMap(screenId, gamutMap), DISPLAY_NULL_PTR);
    ASSERT_EQ(HdiDeviceTest::hdiDevice_->GetScreenGamutMap(screenId, gamutMap), DISPLAY_NULL_PTR);
    const float *matrix;
    ASSERT_EQ(HdiDeviceTest::hdiDevice_->SetScreenColorTransform(screenId, matrix), DISPLAY_NULL_PTR);
    HDRCapability info;
    ASSERT_EQ(HdiDeviceTest::hdiDevice_->GetHDRCapabilityInfos(screenId, info), DISPLAY_NULL_PTR);
    std::vector<GraphicHDRMetadataKey> keys;
    ASSERT_EQ(HdiDeviceTest::hdiDevice_->GetSupportedMetaDataKey(screenId, keys), DISPLAY_NULL_PTR);
    ASSERT_EQ(HdiDeviceTest::hdiDevice_->Commit(screenId, fence), DISPLAY_NULL_PTR);
}

/*
* Function: all LayerFuncs
* Type: Function
* Rank: Important(3)
* EnvConditions: N/A
* CaseDescription: 1. call all LayerFuncs
*                  2. check ret
*/
HWTEST_F(HdiDeviceTest, LayerFuncs001, Function | MediumTest| Level3)
{
    uint32_t screenId = 0, layerId = 0;
    GraphicLayerAlpha alpha;
    ASSERT_EQ(HdiDeviceTest::hdiDevice_->SetLayerAlpha(screenId, layerId, alpha), DISPLAY_NULL_PTR);
    IRect layerRect = {0, 0, 0, 0};
    ASSERT_EQ(HdiDeviceTest::hdiDevice_->SetLayerSize(screenId, layerId, layerRect), DISPLAY_NULL_PTR);
    GraphicTransformType type = GRAPHIC_ROTATE_NONE;
    ASSERT_EQ(HdiDeviceTest::hdiDevice_->SetTransformMode(screenId, layerId, type), DISPLAY_NULL_PTR);
    uint32_t num = 1;
    IRect visible = {0, 0, 0, 0};
    ASSERT_EQ(HdiDeviceTest::hdiDevice_->SetLayerVisibleRegion(screenId, layerId, num, visible), DISPLAY_NULL_PTR);
    IRect dirty = {0, 0, 0, 0};
    ASSERT_EQ(HdiDeviceTest::hdiDevice_->SetLayerDirtyRegion(screenId, layerId, dirty), DISPLAY_NULL_PTR);
    BufferHandle *handle = nullptr;
    sptr<SyncFence> acquireFence = nullptr;
    ASSERT_EQ(HdiDeviceTest::hdiDevice_->SetLayerBuffer(screenId, layerId, handle, acquireFence), DISPLAY_NULL_PTR);
    GraphicCompositionType cmpType = GRAPHIC_COMPOSITION_CLIENT;
    ASSERT_EQ(HdiDeviceTest::hdiDevice_->SetLayerCompositionType(screenId, layerId, cmpType), DISPLAY_NULL_PTR);
    GraphicBlendType blendType = GRAPHIC_BLEND_NONE;
    ASSERT_EQ(HdiDeviceTest::hdiDevice_->SetLayerBlendType(screenId, layerId, blendType), DISPLAY_NULL_PTR);
    IRect crop = {0, 0, 0, 0};
    ASSERT_EQ(HdiDeviceTest::hdiDevice_->SetLayerCrop(screenId, layerId, crop), DISPLAY_NULL_PTR);
    uint32_t zorder = 0;
    ASSERT_EQ(HdiDeviceTest::hdiDevice_->SetLayerZorder(screenId, layerId, zorder), DISPLAY_NULL_PTR);
    bool isPreMulti = false;
    ASSERT_EQ(HdiDeviceTest::hdiDevice_->SetLayerPreMulti(screenId, layerId, isPreMulti), DISPLAY_NULL_PTR);
    float *matrix;
    ASSERT_EQ(HdiDeviceTest::hdiDevice_->SetLayerColorTransform(screenId, layerId, matrix), DISPLAY_NULL_PTR);
    GraphicColorDataSpace colorSpace = GRAPHIC_COLOR_DATA_SPACE_UNKNOWN;
    ASSERT_EQ(HdiDeviceTest::hdiDevice_->SetLayerColorDataSpace(screenId, layerId, colorSpace), DISPLAY_NULL_PTR);
    ASSERT_EQ(HdiDeviceTest::hdiDevice_->GetLayerColorDataSpace(screenId, layerId, colorSpace), DISPLAY_NULL_PTR);
    std::vector<GraphicHDRMetaData> metaData;
    ASSERT_EQ(HdiDeviceTest::hdiDevice_->SetLayerMetaData(screenId, layerId, metaData), DISPLAY_NULL_PTR);
    GraphicHDRMetadataKey key = GRAPHIC_MATAKEY_RED_PRIMARY_X;
    std::vector<uint8_t> metaDatas;
    ASSERT_EQ(HdiDeviceTest::hdiDevice_->SetLayerMetaDataSet(screenId, layerId, key, metaDatas), DISPLAY_NULL_PTR);
    GraphicExtDataHandle *extDataHandle = nullptr;
    ASSERT_EQ(HdiDeviceTest::hdiDevice_->SetLayerTunnelHandle(screenId, layerId, extDataHandle), DISPLAY_NULL_PTR);
    GraphicPresentTimestampType presentTimesType = GRAPHIC_DISPLAY_PTS_UNSUPPORTED;
    ASSERT_EQ(HdiDeviceTest::hdiDevice_->GetSupportedPresentTimestampType(screenId, layerId, presentTimesType),
              DISPLAY_NULL_PTR);
    GraphicPresentTimestamp timestamp;
    ASSERT_EQ(HdiDeviceTest::hdiDevice_->GetPresentTimestamp(screenId, layerId, timestamp), DISPLAY_NULL_PTR);
    GraphicLayerInfo layerInfo;
    ASSERT_EQ(HdiDeviceTest::hdiDevice_->CreateLayer(screenId, layerInfo, layerId), DISPLAY_NULL_PTR);
    ASSERT_EQ(HdiDeviceTest::hdiDevice_->CloseLayer(screenId, layerId), DISPLAY_NULL_PTR);
}

} // namespace
} // namespace Rosen
} // namespace OHOS