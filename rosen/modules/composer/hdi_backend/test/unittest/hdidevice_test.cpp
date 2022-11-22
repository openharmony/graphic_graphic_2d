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
    ASSERT_EQ(HdiDeviceTest::hdiDevice_->RegHotPlugCallback(hotPlugCallback, nullptr), GRAPHIC_DISPLAY_NULL_PTR);
    uint32_t screenId = 0, screenModeId = 0, screenLightLevel = 0, num = 1;
    VBlankCallback vblCallback = nullptr;
    ASSERT_EQ(HdiDeviceTest::hdiDevice_->RegScreenVBlankCallback(screenId, vblCallback, nullptr),
              GRAPHIC_DISPLAY_NULL_PTR);
    bool enabled = false, needFlush = false;
    ASSERT_EQ(HdiDeviceTest::hdiDevice_->SetScreenVsyncEnabled(screenId, enabled), GRAPHIC_DISPLAY_NULL_PTR);
    GraphicDisplayCapability dcpInfo;
    ASSERT_EQ(HdiDeviceTest::hdiDevice_->GetScreenCapability(screenId, dcpInfo), GRAPHIC_DISPLAY_NULL_PTR);
    std::vector<GraphicDisplayModeInfo> dmodes;
    ASSERT_EQ(HdiDeviceTest::hdiDevice_->GetScreenSupportedModes(screenId, dmodes), GRAPHIC_DISPLAY_NULL_PTR);
    ASSERT_EQ(HdiDeviceTest::hdiDevice_->GetScreenMode(screenId, screenModeId), GRAPHIC_DISPLAY_NULL_PTR);
    ASSERT_EQ(HdiDeviceTest::hdiDevice_->SetScreenMode(screenId, screenModeId), GRAPHIC_DISPLAY_NULL_PTR);
    GraphicDispPowerStatus dstatus = GRAPHIC_POWER_STATUS_ON;
    ASSERT_EQ(HdiDeviceTest::hdiDevice_->GetScreenPowerStatus(screenId, dstatus), GRAPHIC_DISPLAY_NULL_PTR);
    ASSERT_EQ(HdiDeviceTest::hdiDevice_->SetScreenPowerStatus(screenId, dstatus), GRAPHIC_DISPLAY_NULL_PTR);
    ASSERT_EQ(HdiDeviceTest::hdiDevice_->GetScreenBacklight(screenId, screenLightLevel), GRAPHIC_DISPLAY_NULL_PTR);
    ASSERT_EQ(HdiDeviceTest::hdiDevice_->SetScreenBacklight(screenId, screenLightLevel), GRAPHIC_DISPLAY_NULL_PTR);
    ASSERT_EQ(HdiDeviceTest::hdiDevice_->PrepareScreenLayers(screenId, needFlush), GRAPHIC_DISPLAY_NULL_PTR);
    std::vector<uint32_t> layersId;
    std::vector<int32_t> types;
    ASSERT_EQ(HdiDeviceTest::hdiDevice_->GetScreenCompChange(screenId, layersId, types), GRAPHIC_DISPLAY_NULL_PTR);
    BufferHandle *buffer = nullptr;
    sptr<SyncFence> fence = nullptr;
    ASSERT_EQ(HdiDeviceTest::hdiDevice_->SetScreenClientBuffer(screenId, buffer, fence), GRAPHIC_DISPLAY_NULL_PTR);
    GraphicIRect damageRect = {0, 0, 0, 0};
    ASSERT_EQ(HdiDeviceTest::hdiDevice_->SetScreenClientDamage(screenId, num, damageRect), GRAPHIC_DISPLAY_NULL_PTR);
    std::vector<uint32_t> layers;
    std::vector<sptr<SyncFence>> fences;
    ASSERT_EQ(HdiDeviceTest::hdiDevice_->GetScreenReleaseFence(screenId, layers, fences), GRAPHIC_DISPLAY_NULL_PTR);
    std::vector<GraphicColorGamut> gamuts;
    ASSERT_EQ(HdiDeviceTest::hdiDevice_->GetScreenSupportedColorGamuts(screenId, gamuts), GRAPHIC_DISPLAY_NULL_PTR);
    GraphicColorGamut gamut = GRAPHIC_COLOR_GAMUT_INVALID;
    ASSERT_EQ(HdiDeviceTest::hdiDevice_->SetScreenColorGamut(screenId, gamut), GRAPHIC_DISPLAY_NULL_PTR);
    ASSERT_EQ(HdiDeviceTest::hdiDevice_->GetScreenColorGamut(screenId, gamut), GRAPHIC_DISPLAY_NULL_PTR);
    GraphicGamutMap gamutMap = GRAPHIC_GAMUT_MAP_CONSTANT;
    ASSERT_EQ(HdiDeviceTest::hdiDevice_->SetScreenGamutMap(screenId, gamutMap), GRAPHIC_DISPLAY_NULL_PTR);
    ASSERT_EQ(HdiDeviceTest::hdiDevice_->GetScreenGamutMap(screenId, gamutMap), GRAPHIC_DISPLAY_NULL_PTR);
    const float *matrix;
    ASSERT_EQ(HdiDeviceTest::hdiDevice_->SetScreenColorTransform(screenId, matrix), GRAPHIC_DISPLAY_NULL_PTR);
    GraphicHDRCapability info;
    ASSERT_EQ(HdiDeviceTest::hdiDevice_->GetHDRCapabilityInfos(screenId, info), GRAPHIC_DISPLAY_NULL_PTR);
    std::vector<GraphicHDRMetadataKey> keys;
    ASSERT_EQ(HdiDeviceTest::hdiDevice_->GetSupportedMetaDataKey(screenId, keys), GRAPHIC_DISPLAY_NULL_PTR);
    ASSERT_EQ(HdiDeviceTest::hdiDevice_->Commit(screenId, fence), GRAPHIC_DISPLAY_NULL_PTR);
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
    ASSERT_EQ(HdiDeviceTest::hdiDevice_->SetLayerAlpha(screenId, layerId, alpha), GRAPHIC_DISPLAY_NULL_PTR);
    GraphicIRect layerRect = {0, 0, 0, 0};
    ASSERT_EQ(HdiDeviceTest::hdiDevice_->SetLayerSize(screenId, layerId, layerRect), GRAPHIC_DISPLAY_NULL_PTR);
    GraphicTransformType type = GRAPHIC_ROTATE_NONE;
    ASSERT_EQ(HdiDeviceTest::hdiDevice_->SetTransformMode(screenId, layerId, type), GRAPHIC_DISPLAY_NULL_PTR);
    uint32_t num = 1;
    GraphicIRect visible = {0, 0, 0, 0};
    ASSERT_EQ(HdiDeviceTest::hdiDevice_->SetLayerVisibleRegion(screenId, layerId, num, visible),
              GRAPHIC_DISPLAY_NULL_PTR);
    GraphicIRect dirty = {0, 0, 0, 0};
    ASSERT_EQ(HdiDeviceTest::hdiDevice_->SetLayerDirtyRegion(screenId, layerId, dirty), GRAPHIC_DISPLAY_NULL_PTR);
    BufferHandle *handle = nullptr;
    sptr<SyncFence> acquireFence = nullptr;
    ASSERT_EQ(HdiDeviceTest::hdiDevice_->SetLayerBuffer(screenId, layerId, handle, acquireFence),
              GRAPHIC_DISPLAY_NULL_PTR);
    GraphicCompositionType cmpType = GRAPHIC_COMPOSITION_CLIENT;
    ASSERT_EQ(HdiDeviceTest::hdiDevice_->SetLayerCompositionType(screenId, layerId, cmpType), GRAPHIC_DISPLAY_NULL_PTR);
    GraphicBlendType blendType = GRAPHIC_BLEND_NONE;
    ASSERT_EQ(HdiDeviceTest::hdiDevice_->SetLayerBlendType(screenId, layerId, blendType), GRAPHIC_DISPLAY_NULL_PTR);
    GraphicIRect crop = {0, 0, 0, 0};
    ASSERT_EQ(HdiDeviceTest::hdiDevice_->SetLayerCrop(screenId, layerId, crop), GRAPHIC_DISPLAY_NULL_PTR);
    uint32_t zorder = 0;
    ASSERT_EQ(HdiDeviceTest::hdiDevice_->SetLayerZorder(screenId, layerId, zorder), GRAPHIC_DISPLAY_NULL_PTR);
    bool isPreMulti = false;
    ASSERT_EQ(HdiDeviceTest::hdiDevice_->SetLayerPreMulti(screenId, layerId, isPreMulti), GRAPHIC_DISPLAY_NULL_PTR);
    float *matrix;
    ASSERT_EQ(HdiDeviceTest::hdiDevice_->SetLayerColorTransform(screenId, layerId, matrix), GRAPHIC_DISPLAY_NULL_PTR);
    GraphicColorDataSpace colorSpace = GRAPHIC_COLOR_DATA_SPACE_UNKNOWN;
    ASSERT_EQ(HdiDeviceTest::hdiDevice_->SetLayerColorDataSpace(screenId, layerId, colorSpace),
              GRAPHIC_DISPLAY_NULL_PTR);
    ASSERT_EQ(HdiDeviceTest::hdiDevice_->GetLayerColorDataSpace(screenId, layerId, colorSpace),
              GRAPHIC_DISPLAY_NULL_PTR);
    std::vector<GraphicHDRMetaData> metaData;
    ASSERT_EQ(HdiDeviceTest::hdiDevice_->SetLayerMetaData(screenId, layerId, metaData), GRAPHIC_DISPLAY_NULL_PTR);
    GraphicHDRMetadataKey key = GRAPHIC_MATAKEY_RED_PRIMARY_X;
    std::vector<uint8_t> metaDatas;
    ASSERT_EQ(HdiDeviceTest::hdiDevice_->SetLayerMetaDataSet(screenId, layerId, key, metaDatas),
              GRAPHIC_DISPLAY_NULL_PTR);
    OHExtDataHandle *extDataHandle = nullptr;
    ASSERT_EQ(HdiDeviceTest::hdiDevice_->SetLayerTunnelHandle(screenId, layerId, extDataHandle),
              GRAPHIC_DISPLAY_NULL_PTR);
    GraphicPresentTimestampType presentTimesType = GRAPHIC_DISPLAY_PTS_UNSUPPORTED;
    ASSERT_EQ(HdiDeviceTest::hdiDevice_->GetSupportedPresentTimestampType(screenId, layerId, presentTimesType),
              GRAPHIC_DISPLAY_NULL_PTR);
    GraphicPresentTimestamp timestamp;
    ASSERT_EQ(HdiDeviceTest::hdiDevice_->GetPresentTimestamp(screenId, layerId, timestamp), GRAPHIC_DISPLAY_NULL_PTR);
    GraphicLayerInfo layerInfo;
    ASSERT_EQ(HdiDeviceTest::hdiDevice_->CreateLayer(screenId, layerInfo, layerId), GRAPHIC_DISPLAY_NULL_PTR);
    ASSERT_EQ(HdiDeviceTest::hdiDevice_->CloseLayer(screenId, layerId), GRAPHIC_DISPLAY_NULL_PTR);
}

} // namespace
} // namespace Rosen
} // namespace OHOS