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
}

void HdiDeviceTest::TearDownTestCase()
{
    hdiDevice_ = nullptr;
}

namespace {
/*
* Function: DeviceFuncs001
* Type: Function
* Rank: Important(3)
* EnvConditions: N/A
* CaseDescription: 1. call DeviceFuncs
*                  2. check ret
*/
HWTEST_F(HdiDeviceTest, DeviceFuncs001, Function | MediumTest| Level3)
{
    ASSERT_EQ(HdiDeviceTest::hdiDevice_->RegHotPlugCallback(nullptr, nullptr), GRAPHIC_DISPLAY_SUCCESS);
    uint32_t screenId = 1, screenModeId = 0, screenLightLevel = 0;
    ASSERT_EQ(HdiDeviceTest::hdiDevice_->RegScreenVBlankCallback(screenId, nullptr, nullptr),
              GRAPHIC_DISPLAY_SUCCESS);
    bool enabled = false, needFlush = false;
    ASSERT_EQ(HdiDeviceTest::hdiDevice_->SetScreenVsyncEnabled(screenId, enabled), GRAPHIC_DISPLAY_SUCCESS);
    GraphicDisplayCapability dcpInfo;
    ASSERT_EQ(HdiDeviceTest::hdiDevice_->GetScreenCapability(screenId, dcpInfo), GRAPHIC_DISPLAY_SUCCESS);
    std::vector<GraphicDisplayModeInfo> dmodes;
    ASSERT_EQ(HdiDeviceTest::hdiDevice_->GetScreenSupportedModes(screenId, dmodes), GRAPHIC_DISPLAY_SUCCESS);
    ASSERT_EQ(HdiDeviceTest::hdiDevice_->GetScreenMode(screenId, screenModeId), GRAPHIC_DISPLAY_SUCCESS);
    ASSERT_EQ(HdiDeviceTest::hdiDevice_->SetScreenMode(screenId, screenModeId), GRAPHIC_DISPLAY_SUCCESS);
    uint32_t width = 1080;
    uint32_t height = 1920;
    ASSERT_EQ(HdiDeviceTest::hdiDevice_->SetScreenOverlayResolution(screenId, width, height), GRAPHIC_DISPLAY_SUCCESS);
    GraphicDispPowerStatus dstatus = GRAPHIC_POWER_STATUS_ON;
    ASSERT_EQ(HdiDeviceTest::hdiDevice_->GetScreenPowerStatus(screenId, dstatus), GRAPHIC_DISPLAY_SUCCESS);
    ASSERT_EQ(HdiDeviceTest::hdiDevice_->SetScreenPowerStatus(screenId, dstatus), GRAPHIC_DISPLAY_SUCCESS);
    ASSERT_EQ(HdiDeviceTest::hdiDevice_->GetScreenBacklight(screenId, screenLightLevel), GRAPHIC_DISPLAY_SUCCESS);
    ASSERT_EQ(HdiDeviceTest::hdiDevice_->SetScreenBacklight(screenId, screenLightLevel), GRAPHIC_DISPLAY_SUCCESS);
    ASSERT_EQ(HdiDeviceTest::hdiDevice_->PrepareScreenLayers(screenId, needFlush), GRAPHIC_DISPLAY_SUCCESS);
    std::vector<uint32_t> layersId;
    std::vector<int32_t> types;
    ASSERT_EQ(HdiDeviceTest::hdiDevice_->GetScreenCompChange(screenId, layersId, types), GRAPHIC_DISPLAY_SUCCESS);
    BufferHandle *buffer = nullptr;
    sptr<SyncFence> fence = nullptr;
    uint32_t cacheIndex = 0;
    ASSERT_EQ(HdiDeviceTest::hdiDevice_->SetScreenClientBuffer(screenId, buffer, cacheIndex, fence),
              GRAPHIC_DISPLAY_NULL_PTR);
    buffer = new BufferHandle();
    ASSERT_EQ(HdiDeviceTest::hdiDevice_->SetScreenClientBuffer(screenId, buffer, cacheIndex, fence),
              GRAPHIC_DISPLAY_NULL_PTR);
    fence = new SyncFence(-1);
    ASSERT_EQ(HdiDeviceTest::hdiDevice_->SetScreenClientBuffer(screenId, buffer, cacheIndex, fence),
              GRAPHIC_DISPLAY_SUCCESS);
    std::vector<GraphicIRect> damageRects = { {0, 0, 0, 0} };
    ASSERT_EQ(HdiDeviceTest::hdiDevice_->SetScreenClientDamage(screenId, damageRects), GRAPHIC_DISPLAY_SUCCESS);
    std::vector<uint32_t> layers;
    std::vector<sptr<SyncFence>> fences;
    ASSERT_EQ(HdiDeviceTest::hdiDevice_->GetScreenReleaseFence(screenId, layers, fences), GRAPHIC_DISPLAY_SUCCESS);
    std::vector<GraphicColorGamut> gamuts;
    ASSERT_EQ(HdiDeviceTest::hdiDevice_->GetScreenSupportedColorGamuts(screenId, gamuts), GRAPHIC_DISPLAY_NOT_SUPPORT);
    GraphicColorGamut gamut = GRAPHIC_COLOR_GAMUT_INVALID;
    ASSERT_EQ(HdiDeviceTest::hdiDevice_->SetScreenColorGamut(screenId, gamut), GRAPHIC_DISPLAY_NOT_SUPPORT);
    ASSERT_EQ(HdiDeviceTest::hdiDevice_->GetScreenColorGamut(screenId, gamut), GRAPHIC_DISPLAY_NOT_SUPPORT);
    GraphicGamutMap gamutMap = GRAPHIC_GAMUT_MAP_CONSTANT;
    ASSERT_EQ(HdiDeviceTest::hdiDevice_->SetScreenGamutMap(screenId, gamutMap), GRAPHIC_DISPLAY_NOT_SUPPORT);
    ASSERT_EQ(HdiDeviceTest::hdiDevice_->GetScreenGamutMap(screenId, gamutMap), GRAPHIC_DISPLAY_NOT_SUPPORT);
    std::vector<float> matrix = { 0.0 };
    ASSERT_EQ(HdiDeviceTest::hdiDevice_->SetScreenColorTransform(screenId, matrix), GRAPHIC_DISPLAY_NOT_SUPPORT);
}

/*
* Function: DeviceFuncs002
* Type: Function
* Rank: Important(3)
* EnvConditions: N/A
* CaseDescription: 1. call DeviceFuncs
*                  2. check ret
*/
HWTEST_F(HdiDeviceTest, DeviceFuncs002, Function | MediumTest| Level3)
{
    uint32_t screenId = 1;
    GraphicHDRCapability info;
    ASSERT_EQ(HdiDeviceTest::hdiDevice_->GetHDRCapabilityInfos(screenId, info), GRAPHIC_DISPLAY_NOT_SUPPORT);
    std::vector<GraphicHDRMetadataKey> keys;
    ASSERT_EQ(HdiDeviceTest::hdiDevice_->GetSupportedMetaDataKey(screenId, keys), GRAPHIC_DISPLAY_NOT_SUPPORT);
}

/*
* Function: LayerFuncs001
* Type: Function
* Rank: Important(3)
* EnvConditions: N/A
* CaseDescription: 1. call LayerFuncs
*                  2. check ret
*/
HWTEST_F(HdiDeviceTest, LayerFuncs001, Function | MediumTest| Level3)
{
    uint32_t screenId = 1, layerId = 0, zorder = 0;
    GraphicLayerAlpha alpha;
    ASSERT_EQ(HdiDeviceTest::hdiDevice_->SetLayerAlpha(screenId, layerId, alpha), GRAPHIC_DISPLAY_SUCCESS);
    GraphicIRect layerRect = {0, 0, 0, 0};
    ASSERT_EQ(HdiDeviceTest::hdiDevice_->SetLayerSize(screenId, layerId, layerRect), GRAPHIC_DISPLAY_SUCCESS);
    GraphicTransformType type = GRAPHIC_ROTATE_NONE;
    ASSERT_EQ(HdiDeviceTest::hdiDevice_->SetTransformMode(screenId, layerId, type), GRAPHIC_DISPLAY_SUCCESS);
    std::vector<GraphicIRect> visibles = { {0, 0, 0, 0} };
    ASSERT_EQ(HdiDeviceTest::hdiDevice_->SetLayerVisibleRegion(screenId, layerId, visibles), GRAPHIC_DISPLAY_SUCCESS);
    std::vector<GraphicIRect> dirtyRegions = { {0, 0, 0, 0} };
    ASSERT_EQ(HdiDeviceTest::hdiDevice_->SetLayerDirtyRegion(screenId, layerId, dirtyRegions),
              GRAPHIC_DISPLAY_SUCCESS);
    BufferHandle *handle = nullptr;
    sptr<SyncFence> acquireFence = nullptr;
    std::vector<uint32_t> deletingList = {};
    uint32_t cacheIndex = 0;
    GraphicLayerBuffer layerBuffer = {handle, cacheIndex, acquireFence, deletingList};
    ASSERT_EQ(HdiDeviceTest::hdiDevice_->SetLayerBuffer(screenId, layerId, layerBuffer), GRAPHIC_DISPLAY_NULL_PTR);
    GraphicCompositionType cmpType = GRAPHIC_COMPOSITION_CLIENT;
    ASSERT_EQ(HdiDeviceTest::hdiDevice_->SetLayerCompositionType(screenId, layerId, cmpType), GRAPHIC_DISPLAY_SUCCESS);
    GraphicBlendType blendType = GRAPHIC_BLEND_NONE;
    ASSERT_EQ(HdiDeviceTest::hdiDevice_->SetLayerBlendType(screenId, layerId, blendType), GRAPHIC_DISPLAY_SUCCESS);
    GraphicIRect crop = {0, 0, 0, 0};
    ASSERT_EQ(HdiDeviceTest::hdiDevice_->SetLayerCrop(screenId, layerId, crop), GRAPHIC_DISPLAY_SUCCESS);
    ASSERT_EQ(HdiDeviceTest::hdiDevice_->SetLayerZorder(screenId, layerId, zorder), GRAPHIC_DISPLAY_SUCCESS);
    bool isPreMulti = false;
    ASSERT_EQ(HdiDeviceTest::hdiDevice_->SetLayerPreMulti(screenId, layerId, isPreMulti), GRAPHIC_DISPLAY_SUCCESS);
    std::vector<float> matrix = { 0.0 };
    ASSERT_EQ(HdiDeviceTest::hdiDevice_->SetLayerColorTransform(screenId, layerId, matrix),
              GRAPHIC_DISPLAY_NOT_SUPPORT);
    GraphicColorDataSpace colorSpace = GRAPHIC_COLOR_DATA_SPACE_UNKNOWN;
    ASSERT_EQ(HdiDeviceTest::hdiDevice_->SetLayerColorDataSpace(screenId, layerId, colorSpace),
              GRAPHIC_DISPLAY_NOT_SUPPORT);
    ASSERT_EQ(HdiDeviceTest::hdiDevice_->GetLayerColorDataSpace(screenId, layerId, colorSpace),
              GRAPHIC_DISPLAY_NOT_SUPPORT);
    std::vector<GraphicHDRMetaData> metaData;
    ASSERT_EQ(HdiDeviceTest::hdiDevice_->SetLayerMetaData(screenId, layerId, metaData), GRAPHIC_DISPLAY_NOT_SUPPORT);
    GraphicHDRMetadataKey key = GRAPHIC_MATAKEY_RED_PRIMARY_X;
    std::vector<uint8_t> metaDatas;
    ASSERT_EQ(HdiDeviceTest::hdiDevice_->SetLayerMetaDataSet(screenId, layerId, key, metaDatas),
              GRAPHIC_DISPLAY_NOT_SUPPORT);
    GraphicExtDataHandle *extDataHandle = nullptr;
    ASSERT_EQ(HdiDeviceTest::hdiDevice_->SetLayerTunnelHandle(screenId, layerId, extDataHandle),
              GRAPHIC_DISPLAY_NOT_SUPPORT);
    GraphicPresentTimestampType presentTimesType = GRAPHIC_DISPLAY_PTS_UNSUPPORTED;
    ASSERT_EQ(HdiDeviceTest::hdiDevice_->GetSupportedPresentTimestampType(screenId, layerId, presentTimesType),
              GRAPHIC_DISPLAY_NOT_SUPPORT);
}

/*
* Function: LayerFuncs002
* Type: Function
* Rank: Important(3)
* EnvConditions: N/A
* CaseDescription: 1. call LayerFuncs
*                  2. check ret
*/
HWTEST_F(HdiDeviceTest, LayerFuncs002, Function | MediumTest| Level3)
{
    uint32_t screenId = 1, layerId = 0;
    GraphicPresentTimestamp timestamp;
    ASSERT_EQ(HdiDeviceTest::hdiDevice_->GetPresentTimestamp(screenId, layerId, timestamp),
              GRAPHIC_DISPLAY_NOT_SUPPORT);
    uint32_t layerMask = 0;
    ASSERT_EQ(HdiDeviceTest::hdiDevice_->SetLayerMaskInfo(screenId, layerId, layerMask), GRAPHIC_DISPLAY_SUCCESS);
    std::vector<std::string> keys;
    ASSERT_EQ(HdiDeviceTest::hdiDevice_->GetSupportedLayerPerFrameParameterKey(keys), GRAPHIC_DISPLAY_SUCCESS);
    const std::vector<int8_t> valueBlob{static_cast<int8_t>(1)};
    const std::string validKey = "ArsrDoEnhance";
    ASSERT_EQ(HdiDeviceTest::hdiDevice_->SetLayerPerFrameParameter(screenId, layerId, validKey, valueBlob),
              GRAPHIC_DISPLAY_SUCCESS);
    GraphicLayerInfo layerInfo;
    uint32_t cacheCount = 1;
    ASSERT_EQ(HdiDeviceTest::hdiDevice_->CreateLayer(screenId, layerInfo, cacheCount, layerId),
              GRAPHIC_DISPLAY_SUCCESS);
    ASSERT_EQ(HdiDeviceTest::hdiDevice_->CloseLayer(screenId, layerId), GRAPHIC_DISPLAY_SUCCESS);
}
} // namespace
} // namespace Rosen
} // namespace OHOS