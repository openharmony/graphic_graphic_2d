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
#include "mock_hdi_device.h"
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
    static inline Mock::HdiDeviceMock* hdiDeviceMock_;
};

void HdiDeviceTest::SetUpTestCase()
{
    hdiDevice_ = HdiDevice::GetInstance();
    hdiDeviceMock_ = Mock::HdiDeviceMock::GetInstance();
}

void HdiDeviceTest::TearDownTestCase()
{
    hdiDevice_ = nullptr;
    hdiDeviceMock_ = nullptr;
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
    EXPECT_CALL(*hdiDeviceMock_, RegHotPlugCallback(_, _))
        .WillRepeatedly(testing::Return(GRAPHIC_DISPLAY_SUCCESS));
    EXPECT_EQ(hdiDeviceMock_->RegHotPlugCallback(nullptr, nullptr), GRAPHIC_DISPLAY_SUCCESS);
    uint32_t screenId = 0;
    uint32_t screenModeId = 0;
    uint32_t screenLightLevel = 0;
    EXPECT_CALL(*hdiDeviceMock_, RegScreenVBlankCallback(_, _, _))
        .WillRepeatedly(testing::Return(GRAPHIC_DISPLAY_SUCCESS));
    EXPECT_EQ(hdiDeviceMock_->RegScreenVBlankCallback(screenId, nullptr, nullptr),
              GRAPHIC_DISPLAY_SUCCESS);
    bool enabled = false, needFlush = false;
    EXPECT_CALL(*hdiDeviceMock_,
        SetScreenVsyncEnabled(_, _)).WillRepeatedly(testing::Return(GRAPHIC_DISPLAY_SUCCESS));
    EXPECT_EQ(hdiDeviceMock_->SetScreenVsyncEnabled(screenId, enabled), GRAPHIC_DISPLAY_SUCCESS);
    uint32_t pid = 1;
    uint64_t pvalue = 0;
    EXPECT_CALL(*hdiDeviceMock_,
        GetDisplayProperty(_, _, _)).WillRepeatedly(testing::Return(GRAPHIC_DISPLAY_SUCCESS));
    EXPECT_EQ(hdiDeviceMock_->GetDisplayProperty(screenId, pid, pvalue), GRAPHIC_DISPLAY_SUCCESS);
    EXPECT_CALL(*hdiDeviceMock_,
        SetDisplayProperty(_, _, _)).WillRepeatedly(testing::Return(GRAPHIC_DISPLAY_SUCCESS));
    EXPECT_EQ(hdiDeviceMock_->SetDisplayProperty(screenId, pid, pvalue), GRAPHIC_DISPLAY_SUCCESS);
    GraphicDisplayCapability dcpInfo;
    EXPECT_CALL(*hdiDeviceMock_,
        GetScreenCapability(_, _)).WillRepeatedly(testing::Return(GRAPHIC_DISPLAY_SUCCESS));
    EXPECT_EQ(hdiDeviceMock_->GetScreenCapability(screenId, dcpInfo), GRAPHIC_DISPLAY_SUCCESS);
    std::vector<GraphicDisplayModeInfo> dmodes;
    EXPECT_CALL(*hdiDeviceMock_, GetScreenSupportedModes(_, _))
        .WillRepeatedly(testing::Return(GRAPHIC_DISPLAY_SUCCESS));
    EXPECT_EQ(hdiDeviceMock_->GetScreenSupportedModes(screenId, dmodes), GRAPHIC_DISPLAY_SUCCESS);
    EXPECT_CALL(*hdiDeviceMock_, GetScreenMode(_, _))
        .WillRepeatedly(testing::Return(GRAPHIC_DISPLAY_SUCCESS));
    EXPECT_EQ(hdiDeviceMock_->GetScreenMode(screenId, screenModeId), GRAPHIC_DISPLAY_SUCCESS);
    EXPECT_CALL(*hdiDeviceMock_, SetScreenMode(_, _))
        .WillRepeatedly(testing::Return(GRAPHIC_DISPLAY_SUCCESS));
    EXPECT_EQ(hdiDeviceMock_->SetScreenMode(screenId, screenModeId), GRAPHIC_DISPLAY_SUCCESS);
    uint32_t width = 1080;
    uint32_t height = 1920;
    EXPECT_CALL(*hdiDeviceMock_,
        SetScreenOverlayResolution(_, _, _)).WillRepeatedly(testing::Return(GRAPHIC_DISPLAY_SUCCESS));
    EXPECT_EQ(hdiDeviceMock_->SetScreenOverlayResolution(screenId, width, height),
        GRAPHIC_DISPLAY_SUCCESS);
    GraphicDispPowerStatus dstatus = GRAPHIC_POWER_STATUS_ON;
    EXPECT_CALL(*hdiDeviceMock_,
        SetScreenPowerStatus(_, _)).WillRepeatedly(testing::Return(GRAPHIC_DISPLAY_SUCCESS));
    EXPECT_EQ(hdiDeviceMock_->SetScreenPowerStatus(screenId, dstatus), GRAPHIC_DISPLAY_SUCCESS);
    EXPECT_CALL(*hdiDeviceMock_,
        GetScreenPowerStatus(_, _)).WillRepeatedly(testing::Return(GRAPHIC_DISPLAY_SUCCESS));
    EXPECT_EQ(hdiDeviceMock_->GetScreenPowerStatus(screenId, dstatus), GRAPHIC_DISPLAY_SUCCESS);
    EXPECT_CALL(*hdiDeviceMock_,
        GetScreenBacklight(_, _)).WillRepeatedly(testing::Return(GRAPHIC_DISPLAY_SUCCESS));
    EXPECT_EQ(hdiDeviceMock_->GetScreenBacklight(screenId, screenLightLevel), GRAPHIC_DISPLAY_SUCCESS);
    EXPECT_CALL(*hdiDeviceMock_,
        SetScreenBacklight(_, _)).WillRepeatedly(testing::Return(GRAPHIC_DISPLAY_SUCCESS));
    EXPECT_EQ(hdiDeviceMock_->SetScreenBacklight(screenId, screenLightLevel), GRAPHIC_DISPLAY_SUCCESS);
    EXPECT_CALL(*hdiDeviceMock_,
        PrepareScreenLayers(_, _)).WillRepeatedly(testing::Return(GRAPHIC_DISPLAY_SUCCESS));
    EXPECT_EQ(hdiDeviceMock_->PrepareScreenLayers(screenId, needFlush), GRAPHIC_DISPLAY_SUCCESS);
    std::vector<uint32_t> layersId;
    std::vector<int32_t> types;
    screenId = UINT32_MAX;
    EXPECT_CALL(*hdiDeviceMock_, GetScreenCompChange(_, _, _))
        .WillRepeatedly(testing::Return(GRAPHIC_DISPLAY_SUCCESS));
    EXPECT_EQ(hdiDeviceMock_->GetScreenCompChange(screenId, layersId, types), GRAPHIC_DISPLAY_SUCCESS);
    BufferHandle *buffer = nullptr;
    sptr<SyncFence> fence = nullptr;
    uint32_t cacheIndex = 0;
    EXPECT_CALL(*hdiDeviceMock_, SetScreenClientBuffer(_, _, _, _))
        .WillRepeatedly(testing::Return(GRAPHIC_DISPLAY_PARAM_ERR));
    EXPECT_EQ(hdiDeviceMock_->SetScreenClientBuffer(screenId, buffer, cacheIndex, fence),
              GRAPHIC_DISPLAY_PARAM_ERR);
    buffer = new BufferHandle();
    EXPECT_EQ(hdiDeviceMock_->SetScreenClientBuffer(screenId, buffer, cacheIndex, fence),
              GRAPHIC_DISPLAY_PARAM_ERR);
    fence = new SyncFence(-1);
    EXPECT_CALL(*hdiDeviceMock_, SetScreenClientBuffer(_, _, _, _))
        .WillRepeatedly(testing::Return(GRAPHIC_DISPLAY_SUCCESS));
    EXPECT_EQ(hdiDeviceMock_->SetScreenClientBuffer(screenId, buffer, cacheIndex, fence),
              GRAPHIC_DISPLAY_SUCCESS);
    std::vector<GraphicIRect> damageRects = { {0, 0, 0, 0} };
    EXPECT_CALL(*hdiDeviceMock_, SetScreenClientDamage(_, _))
        .WillRepeatedly(testing::Return(GRAPHIC_DISPLAY_SUCCESS));
    ASSERT_EQ(hdiDeviceMock_->SetScreenClientDamage(screenId, damageRects), GRAPHIC_DISPLAY_SUCCESS);
    std::vector<GraphicColorGamut> gamuts;
    EXPECT_CALL(*hdiDeviceMock_,
        GetScreenSupportedColorGamuts(_, _)).WillRepeatedly(testing::Return(GRAPHIC_DISPLAY_SUCCESS));
    EXPECT_EQ(hdiDeviceMock_->GetScreenSupportedColorGamuts(screenId, gamuts),
              GRAPHIC_DISPLAY_SUCCESS);
    GraphicColorGamut gamut = GRAPHIC_COLOR_GAMUT_INVALID;
    EXPECT_CALL(*hdiDeviceMock_, SetScreenColorGamut(_, _))
        .WillRepeatedly(testing::Return(GRAPHIC_DISPLAY_NOT_SUPPORT));
    EXPECT_EQ(hdiDeviceMock_->SetScreenColorGamut(screenId, gamut), GRAPHIC_DISPLAY_NOT_SUPPORT);
    EXPECT_CALL(*hdiDeviceMock_, GetScreenColorGamut(_, _))
        .WillRepeatedly(testing::Return(GRAPHIC_DISPLAY_NOT_SUPPORT));
    EXPECT_EQ(hdiDeviceMock_->GetScreenColorGamut(screenId, gamut), GRAPHIC_DISPLAY_NOT_SUPPORT);
    GraphicGamutMap gamutMap = GRAPHIC_GAMUT_MAP_CONSTANT;
    EXPECT_CALL(*hdiDeviceMock_, SetScreenGamutMap(_, _))
        .WillRepeatedly(testing::Return(GRAPHIC_DISPLAY_NOT_SUPPORT));
    EXPECT_EQ(hdiDeviceMock_->SetScreenGamutMap(screenId, gamutMap), GRAPHIC_DISPLAY_NOT_SUPPORT);
    EXPECT_CALL(*hdiDeviceMock_, GetScreenGamutMap(_, _))
        .WillRepeatedly(testing::Return(GRAPHIC_DISPLAY_NOT_SUPPORT));
    EXPECT_EQ(hdiDeviceMock_->GetScreenGamutMap(screenId, gamutMap), GRAPHIC_DISPLAY_NOT_SUPPORT);
    std::vector<float> matrix = { 0.0 };
    EXPECT_CALL(*hdiDeviceMock_, SetScreenColorTransform(_, _))
        .WillRepeatedly(testing::Return(GRAPHIC_DISPLAY_NOT_SUPPORT));
    EXPECT_EQ(hdiDeviceMock_->SetScreenColorTransform(screenId, matrix), GRAPHIC_DISPLAY_NOT_SUPPORT);
    EXPECT_CALL(*hdiDeviceMock_,
        Commit(_, _)).WillRepeatedly(testing::Return(GRAPHIC_DISPLAY_SUCCESS));
    EXPECT_EQ(hdiDeviceMock_->Commit(screenId, fence), GRAPHIC_DISPLAY_SUCCESS);
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
    uint32_t screenId = UINT32_MAX;
    GraphicHDRCapability info;
    EXPECT_CALL(*hdiDeviceMock_,
        GetHDRCapabilityInfos(_, _)).WillRepeatedly(testing::Return(GRAPHIC_DISPLAY_SUCCESS));
    EXPECT_EQ(hdiDeviceMock_->GetHDRCapabilityInfos(screenId, info), GRAPHIC_DISPLAY_SUCCESS);
    std::vector<GraphicHDRMetadataKey> keys;
    EXPECT_CALL(*hdiDeviceMock_,
        GetSupportedMetaDataKey(_, _)).WillRepeatedly(testing::Return(GRAPHIC_DISPLAY_SUCCESS));
    EXPECT_EQ(hdiDeviceMock_->GetSupportedMetaDataKey(screenId, keys), GRAPHIC_DISPLAY_SUCCESS);
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
    uint32_t screenId = UINT32_MAX, layerId = 0, zorder = 0;
    GraphicLayerAlpha alpha;
    EXPECT_CALL(*hdiDeviceMock_, SetLayerAlpha(_, _, _))
        .WillRepeatedly(testing::Return(GRAPHIC_DISPLAY_SUCCESS));
    EXPECT_EQ(hdiDeviceMock_->SetLayerAlpha(screenId, layerId, alpha), GRAPHIC_DISPLAY_SUCCESS);
    GraphicIRect layerRect = {0, 0, 0, 0};
    EXPECT_CALL(*hdiDeviceMock_, SetLayerSize(_, _, _))
        .WillRepeatedly(testing::Return(GRAPHIC_DISPLAY_SUCCESS));
    EXPECT_EQ(hdiDeviceMock_->SetLayerSize(screenId, layerId, layerRect), GRAPHIC_DISPLAY_SUCCESS);
    GraphicTransformType type = GRAPHIC_ROTATE_NONE;
    EXPECT_CALL(*hdiDeviceMock_, SetTransformMode(_, _, _))
        .WillRepeatedly(testing::Return(GRAPHIC_DISPLAY_SUCCESS));
    EXPECT_EQ(hdiDeviceMock_->SetTransformMode(screenId, layerId, type), GRAPHIC_DISPLAY_SUCCESS);
    std::vector<GraphicIRect> visibles = { {0, 0, 0, 0} };
    EXPECT_CALL(*hdiDeviceMock_, SetLayerVisibleRegion(_, _, _))
        .WillRepeatedly(testing::Return(GRAPHIC_DISPLAY_SUCCESS));
    EXPECT_EQ(hdiDeviceMock_->SetLayerVisibleRegion(screenId, layerId, visibles),
              GRAPHIC_DISPLAY_SUCCESS);
    std::vector<GraphicIRect> dirtyRegions = { {0, 0, 0, 0} };
    EXPECT_CALL(*hdiDeviceMock_, SetLayerDirtyRegion(_, _, _))
        .WillRepeatedly(testing::Return(GRAPHIC_DISPLAY_SUCCESS));
    EXPECT_EQ(hdiDeviceMock_->SetLayerDirtyRegion(screenId, layerId, dirtyRegions),
              GRAPHIC_DISPLAY_SUCCESS);
    BufferHandle *handle = nullptr;
    sptr<SyncFence> acquireFence = nullptr;
    std::vector<uint32_t> deletingList = {};
    uint32_t cacheIndex = INVALID_BUFFER_CACHE_INDEX;
    GraphicLayerBuffer layerBuffer = {handle, cacheIndex, acquireFence, deletingList};
    EXPECT_CALL(*hdiDeviceMock_, SetLayerBuffer(_, _, _))
        .WillRepeatedly(testing::Return(GRAPHIC_DISPLAY_PARAM_ERR));
    EXPECT_EQ(hdiDeviceMock_->SetLayerBuffer(screenId, layerId, layerBuffer), GRAPHIC_DISPLAY_PARAM_ERR);
    handle = new BufferHandle();
    EXPECT_EQ(hdiDeviceMock_->SetLayerBuffer(screenId, layerId, layerBuffer), GRAPHIC_DISPLAY_PARAM_ERR);
    layerBuffer.acquireFence = new SyncFence(10);
    EXPECT_EQ(hdiDeviceMock_->SetLayerBuffer(screenId, layerId, layerBuffer), GRAPHIC_DISPLAY_PARAM_ERR);
    layerBuffer.cacheIndex = 0;
    EXPECT_CALL(*hdiDeviceMock_,
        SetLayerBuffer(_, _, _)).WillRepeatedly(testing::Return(GRAPHIC_DISPLAY_SUCCESS));
    EXPECT_EQ(hdiDeviceMock_->SetLayerBuffer(screenId, layerId, layerBuffer),
              GRAPHIC_DISPLAY_SUCCESS);
    GraphicCompositionType cmpType = GRAPHIC_COMPOSITION_CLIENT;
    EXPECT_CALL(*hdiDeviceMock_, SetLayerCompositionType(_, _, _))
        .WillRepeatedly(testing::Return(GRAPHIC_DISPLAY_SUCCESS));
    EXPECT_EQ(hdiDeviceMock_->SetLayerCompositionType(screenId, layerId, cmpType),
              GRAPHIC_DISPLAY_SUCCESS);
    GraphicBlendType blendType = GRAPHIC_BLEND_NONE;
    EXPECT_CALL(*hdiDeviceMock_, SetLayerBlendType(_, _, _))
        .WillRepeatedly(testing::Return(GRAPHIC_DISPLAY_SUCCESS));
    EXPECT_EQ(hdiDeviceMock_->SetLayerBlendType(screenId, layerId, blendType),
              GRAPHIC_DISPLAY_SUCCESS);
    GraphicIRect crop = {0, 0, 0, 0};
    EXPECT_CALL(*hdiDeviceMock_, SetLayerCrop(_, _, _))
        .WillRepeatedly(testing::Return(GRAPHIC_DISPLAY_SUCCESS));
    EXPECT_EQ(hdiDeviceMock_->SetLayerCrop(screenId, layerId, crop), GRAPHIC_DISPLAY_SUCCESS);
    EXPECT_CALL(*hdiDeviceMock_, SetLayerZorder(_, _, _))
        .WillRepeatedly(testing::Return(GRAPHIC_DISPLAY_SUCCESS));
    EXPECT_EQ(hdiDeviceMock_->SetLayerZorder(screenId, layerId, zorder), GRAPHIC_DISPLAY_SUCCESS);
    bool isPreMulti = false;
    EXPECT_CALL(*hdiDeviceMock_, SetLayerPreMulti(_, _, _))
        .WillRepeatedly(testing::Return(GRAPHIC_DISPLAY_SUCCESS));
    EXPECT_EQ(hdiDeviceMock_->SetLayerPreMulti(screenId, layerId, isPreMulti), GRAPHIC_DISPLAY_SUCCESS);
    std::vector<float> matrix = { 0.0 };
    EXPECT_CALL(*hdiDeviceMock_, SetLayerColorTransform(_, _, _))
        .WillRepeatedly(testing::Return(GRAPHIC_DISPLAY_NOT_SUPPORT));
    EXPECT_EQ(hdiDeviceMock_->SetLayerColorTransform(screenId, layerId, matrix),
              GRAPHIC_DISPLAY_NOT_SUPPORT);
    GraphicColorDataSpace colorSpace = GRAPHIC_COLOR_DATA_SPACE_UNKNOWN;
    EXPECT_CALL(*hdiDeviceMock_, SetLayerColorDataSpace(_, _, _))
        .WillRepeatedly(testing::Return(GRAPHIC_DISPLAY_NOT_SUPPORT));
    EXPECT_EQ(hdiDeviceMock_->SetLayerColorDataSpace(screenId, layerId, colorSpace),
              GRAPHIC_DISPLAY_NOT_SUPPORT);
    EXPECT_CALL(*hdiDeviceMock_, GetLayerColorDataSpace(_, _, _))
        .WillRepeatedly(testing::Return(GRAPHIC_DISPLAY_NOT_SUPPORT));
    EXPECT_EQ(hdiDeviceMock_->GetLayerColorDataSpace(screenId, layerId, colorSpace),
              GRAPHIC_DISPLAY_NOT_SUPPORT);
    std::vector<GraphicHDRMetaData> metaData;
    EXPECT_CALL(*hdiDeviceMock_, SetLayerMetaData(_, _, _))
        .WillRepeatedly(testing::Return(GRAPHIC_DISPLAY_NOT_SUPPORT));
    EXPECT_EQ(hdiDeviceMock_->SetLayerMetaData(screenId, layerId, metaData), GRAPHIC_DISPLAY_NOT_SUPPORT);
    GraphicHDRMetadataKey key = GRAPHIC_MATAKEY_RED_PRIMARY_X;
    std::vector<uint8_t> metaDatas;
    EXPECT_CALL(*hdiDeviceMock_, SetLayerMetaDataSet(_, _, _, _))
        .WillRepeatedly(testing::Return(GRAPHIC_DISPLAY_NOT_SUPPORT));
    EXPECT_EQ(hdiDeviceMock_->SetLayerMetaDataSet(screenId, layerId, key, metaDatas),
              GRAPHIC_DISPLAY_NOT_SUPPORT);
    GraphicExtDataHandle *extDataHandle = nullptr;
    EXPECT_CALL(*hdiDeviceMock_, SetLayerTunnelHandle(_, _, _))
        .WillRepeatedly(testing::Return(GRAPHIC_DISPLAY_NOT_SUPPORT));
    EXPECT_EQ(hdiDeviceMock_->SetLayerTunnelHandle(screenId, layerId, extDataHandle),
              GRAPHIC_DISPLAY_NOT_SUPPORT);
    GraphicPresentTimestampType presentTimesType = GRAPHIC_DISPLAY_PTS_UNSUPPORTED;
    EXPECT_CALL(*hdiDeviceMock_, GetSupportedPresentTimestampType(_, _, _))
        .WillRepeatedly(testing::Return(GRAPHIC_DISPLAY_NOT_SUPPORT));
    EXPECT_EQ(hdiDeviceMock_->GetSupportedPresentTimestampType(screenId, layerId, presentTimesType),
              GRAPHIC_DISPLAY_NOT_SUPPORT);
}

/*
* Function: SetTunnelLayerId
* Type: Function
* Rank: Important(3)
* EnvConditions: N/A
* CaseDescription: 1. call SetTunnelLayerId
*                  2. check ret
*/
HWTEST_F(HdiDeviceTest, SetTunnelLayerId, Function | MediumTest| Level3)
{
    uint32_t screenId = UINT32_MAX;
    uint32_t layerId = 0;
    uint64_t tunnelId = 0;
    EXPECT_CALL(*hdiDeviceMock_, SetTunnelLayerId(_, _, _))
        .WillRepeatedly(testing::Return(GRAPHIC_DISPLAY_FAILURE));
    EXPECT_NE(hdiDeviceMock_->SetTunnelLayerId(screenId, layerId, tunnelId), GRAPHIC_DISPLAY_SUCCESS);
}

/*
* Function: SetTunnelLayerProperty
* Type: Function
* Rank: Important(3)
* EnvConditions: N/A
* CaseDescription: 1. call SetTunnelLayerProperty
*                  2. check ret
*/
HWTEST_F(HdiDeviceTest, SetTunnelLayerProperty, Function | MediumTest| Level3)
{
    uint32_t screenId = UINT32_MAX;
    uint32_t layerId = 0;
    uint32_t property = 0;
    EXPECT_CALL(*hdiDeviceMock_, SetTunnelLayerProperty(_, _, _))
        .WillRepeatedly(testing::Return(GRAPHIC_DISPLAY_FAILURE));
    EXPECT_NE(hdiDeviceMock_->SetTunnelLayerProperty(screenId, layerId, property), GRAPHIC_DISPLAY_SUCCESS);
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
    uint32_t screenId = UINT32_MAX, layerId = 0;
    GraphicPresentTimestamp timestamp;
    EXPECT_CALL(*hdiDeviceMock_, GetPresentTimestamp(_, _, _))
        .WillRepeatedly(testing::Return(GRAPHIC_DISPLAY_NOT_SUPPORT));
    EXPECT_EQ(hdiDeviceMock_->GetPresentTimestamp(screenId, layerId, timestamp),
              GRAPHIC_DISPLAY_NOT_SUPPORT);
    uint32_t layerMask = 0;
    EXPECT_CALL(*hdiDeviceMock_, SetLayerMaskInfo(_, _, _))
        .WillRepeatedly(testing::Return(GRAPHIC_DISPLAY_SUCCESS));
    EXPECT_EQ(hdiDeviceMock_->SetLayerMaskInfo(screenId, layerId, layerMask), GRAPHIC_DISPLAY_SUCCESS);
    GraphicLayerInfo layerInfo;
    uint32_t cacheCount = 1;
    EXPECT_CALL(*hdiDeviceMock_, CreateLayer(_, _, _, _))
        .WillRepeatedly(testing::Return(GRAPHIC_DISPLAY_FAILURE));
    EXPECT_EQ(hdiDeviceMock_->CreateLayer(screenId, layerInfo, cacheCount, layerId),
              GRAPHIC_DISPLAY_FAILURE);
    static std::vector<std::string> emptyKeys;
    EXPECT_CALL(*hdiDeviceMock_, GetSupportedLayerPerFrameParameterKey())
        .WillRepeatedly(testing::ReturnRef(emptyKeys));
    EXPECT_EQ(hdiDeviceMock_->GetSupportedLayerPerFrameParameterKey().size(), 0u);
    const std::string validKey = "ArsrDoEnhance";
    if (std::find(valueRet.begin(), valueRet.end(), validKey) != valueRet.end()) {
        const std::vector<int8_t> valueBlob{static_cast<int8_t>(1)};
        EXPECT_CALL(*hdiDeviceMock_, SetLayerPerFrameParameter(_, _, _, _))
            .WillRepeatedly(testing::Return(GRAPHIC_DISPLAY_FAILURE));
        EXPECT_EQ(hdiDeviceMock_->SetLayerPerFrameParameter(screenId, layerId, validKey, valueBlob),
              GRAPHIC_DISPLAY_FAILURE);
    }
    EXPECT_CALL(*hdiDeviceMock_, CloseLayer(_, _))
        .WillRepeatedly(testing::Return(GRAPHIC_DISPLAY_FAILURE));
    EXPECT_EQ(hdiDeviceMock_->CloseLayer(screenId, layerId), GRAPHIC_DISPLAY_FAILURE);
}

/*
* Function: LayerFuncs003
* Type: Function
* Rank: Important(3)
* EnvConditions: N/A
* CaseDescription: 1. call LayerFuncs
*                  2. check ret
*/
HWTEST_F(HdiDeviceTest, LayerFuncs003, Function | MediumTest| Level3)
{
    uint32_t screenId = UINT32_MAX, layerId = 0, zorder = 1;
    GraphicLayerAlpha alpha;
    EXPECT_CALL(*hdiDeviceMock_, SetLayerAlpha(_, _, _))
        .WillRepeatedly(testing::Return(GRAPHIC_DISPLAY_SUCCESS));
    EXPECT_EQ(hdiDeviceMock_->SetLayerAlpha(screenId, layerId, alpha), GRAPHIC_DISPLAY_SUCCESS);
    GraphicIRect layerRect = {0, 0, 0, 0};
    EXPECT_CALL(*hdiDeviceMock_, SetLayerSize(_, _, _))
        .WillRepeatedly(testing::Return(GRAPHIC_DISPLAY_SUCCESS));
    EXPECT_EQ(hdiDeviceMock_->SetLayerSize(screenId, layerId, layerRect), GRAPHIC_DISPLAY_SUCCESS);
    GraphicTransformType type = GRAPHIC_ROTATE_90;
    EXPECT_CALL(*hdiDeviceMock_, SetTransformMode(_, _, _))
        .WillRepeatedly(testing::Return(GRAPHIC_DISPLAY_SUCCESS));
    EXPECT_EQ(hdiDeviceMock_->SetTransformMode(screenId, layerId, type), GRAPHIC_DISPLAY_SUCCESS);
    std::vector<GraphicIRect> visibles = { {0, 0, 1, 1} };
    EXPECT_CALL(*hdiDeviceMock_, SetLayerVisibleRegion(_, _, _))
        .WillRepeatedly(testing::Return(GRAPHIC_DISPLAY_SUCCESS));
    EXPECT_EQ(hdiDeviceMock_->SetLayerVisibleRegion(screenId, layerId, visibles),
              GRAPHIC_DISPLAY_SUCCESS);
    std::vector<GraphicIRect> dirtyRegions = { {0, 0, 1, 1} };
    EXPECT_CALL(*hdiDeviceMock_, SetLayerDirtyRegion(_, _, _))
        .WillRepeatedly(testing::Return(GRAPHIC_DISPLAY_SUCCESS));
    EXPECT_EQ(hdiDeviceMock_->SetLayerDirtyRegion(screenId, layerId, dirtyRegions),
              GRAPHIC_DISPLAY_SUCCESS);
    BufferHandle *handle = nullptr;
    sptr<SyncFence> acquireFence = nullptr;
    std::vector<uint32_t> deletingList = {};
    uint32_t cacheIndex = INVALID_BUFFER_CACHE_INDEX;
    GraphicLayerBuffer layerBuffer = {handle, cacheIndex, acquireFence, deletingList};
    EXPECT_CALL(*hdiDeviceMock_, SetLayerBuffer(_, _, _))
        .WillRepeatedly(testing::Return(GRAPHIC_DISPLAY_PARAM_ERR));
    EXPECT_EQ(hdiDeviceMock_->SetLayerBuffer(screenId, layerId, layerBuffer), GRAPHIC_DISPLAY_PARAM_ERR);
    handle = new BufferHandle();
    EXPECT_EQ(hdiDeviceMock_->SetLayerBuffer(screenId, layerId, layerBuffer), GRAPHIC_DISPLAY_PARAM_ERR);
    layerBuffer.acquireFence = new SyncFence(10);
    EXPECT_EQ(hdiDeviceMock_->SetLayerBuffer(screenId, layerId, layerBuffer), GRAPHIC_DISPLAY_PARAM_ERR);
    layerBuffer.cacheIndex = 0;
    EXPECT_CALL(*hdiDeviceMock_,
        SetLayerBuffer(_, _, _)).WillRepeatedly(testing::Return(GRAPHIC_DISPLAY_SUCCESS));
    EXPECT_EQ(hdiDeviceMock_->SetLayerBuffer(screenId, layerId, layerBuffer),
              GRAPHIC_DISPLAY_SUCCESS);
    GraphicCompositionType cmpType = GRAPHIC_COMPOSITION_CLIENT;
    EXPECT_CALL(*hdiDeviceMock_, SetLayerCompositionType(_, _, _))
        .WillRepeatedly(testing::Return(GRAPHIC_DISPLAY_SUCCESS));
    EXPECT_EQ(hdiDeviceMock_->SetLayerCompositionType(screenId, layerId, cmpType),
              GRAPHIC_DISPLAY_SUCCESS);
    GraphicBlendType blendType = GRAPHIC_BLEND_NONE;
    EXPECT_CALL(*hdiDeviceMock_, SetLayerBlendType(_, _, _))
        .WillRepeatedly(testing::Return(GRAPHIC_DISPLAY_SUCCESS));
    EXPECT_EQ(hdiDeviceMock_->SetLayerBlendType(screenId, layerId, blendType),
              GRAPHIC_DISPLAY_SUCCESS);
    GraphicIRect crop = {0, 0, 0, 0};
    EXPECT_CALL(*hdiDeviceMock_, SetLayerCrop(_, _, _))
        .WillRepeatedly(testing::Return(GRAPHIC_DISPLAY_SUCCESS));
    EXPECT_EQ(hdiDeviceMock_->SetLayerCrop(screenId, layerId, crop), GRAPHIC_DISPLAY_SUCCESS);
    EXPECT_CALL(*hdiDeviceMock_, SetLayerZorder(_, _, _))
        .WillRepeatedly(testing::Return(GRAPHIC_DISPLAY_SUCCESS));
    EXPECT_EQ(hdiDeviceMock_->SetLayerZorder(screenId, layerId, zorder), GRAPHIC_DISPLAY_SUCCESS);
    bool isPreMulti = false;
    EXPECT_CALL(*hdiDeviceMock_, SetLayerPreMulti(_, _, _))
        .WillRepeatedly(testing::Return(GRAPHIC_DISPLAY_SUCCESS));
    EXPECT_EQ(hdiDeviceMock_->SetLayerPreMulti(screenId, layerId, isPreMulti), GRAPHIC_DISPLAY_SUCCESS);
    std::vector<float> matrix = { 0.0 };
    EXPECT_CALL(*hdiDeviceMock_, SetLayerColorTransform(_, _, _))
        .WillRepeatedly(testing::Return(GRAPHIC_DISPLAY_NOT_SUPPORT));
    EXPECT_EQ(hdiDeviceMock_->SetLayerColorTransform(screenId, layerId, matrix),
              GRAPHIC_DISPLAY_NOT_SUPPORT);
    GraphicColorDataSpace colorSpace = GRAPHIC_COLOR_DATA_SPACE_UNKNOWN;
    EXPECT_CALL(*hdiDeviceMock_, SetLayerColorDataSpace(_, _, _))
        .WillRepeatedly(testing::Return(GRAPHIC_DISPLAY_NOT_SUPPORT));
    EXPECT_EQ(hdiDeviceMock_->SetLayerColorDataSpace(screenId, layerId, colorSpace),
              GRAPHIC_DISPLAY_NOT_SUPPORT);
    EXPECT_CALL(*hdiDeviceMock_, GetLayerColorDataSpace(_, _, _))
        .WillRepeatedly(testing::Return(GRAPHIC_DISPLAY_NOT_SUPPORT));
    EXPECT_EQ(hdiDeviceMock_->GetLayerColorDataSpace(screenId, layerId, colorSpace),
              GRAPHIC_DISPLAY_NOT_SUPPORT);
    std::vector<GraphicHDRMetaData> metaData;
    EXPECT_CALL(*hdiDeviceMock_, SetLayerMetaData(_, _, _))
        .WillRepeatedly(testing::Return(GRAPHIC_DISPLAY_NOT_SUPPORT));
    EXPECT_EQ(hdiDeviceMock_->SetLayerMetaData(screenId, layerId, metaData), GRAPHIC_DISPLAY_NOT_SUPPORT);
    GraphicHDRMetadataKey key = GRAPHIC_MATAKEY_RED_PRIMARY_X;
    std::vector<uint8_t> metaDatas;
    EXPECT_CALL(*hdiDeviceMock_, SetLayerMetaDataSet(_, _, _, _))
        .WillRepeatedly(testing::Return(GRAPHIC_DISPLAY_NOT_SUPPORT));
    EXPECT_EQ(hdiDeviceMock_->SetLayerMetaDataSet(screenId, layerId, key, metaDatas),
              GRAPHIC_DISPLAY_NOT_SUPPORT);
    GraphicExtDataHandle *extDataHandle = nullptr;
    EXPECT_CALL(*hdiDeviceMock_, SetLayerTunnelHandle(_, _, _))
        .WillRepeatedly(testing::Return(GRAPHIC_DISPLAY_NOT_SUPPORT));
    EXPECT_EQ(hdiDeviceMock_->SetLayerTunnelHandle(screenId, layerId, extDataHandle),
              GRAPHIC_DISPLAY_NOT_SUPPORT);
    GraphicPresentTimestampType presentTimesType = GRAPHIC_DISPLAY_PTS_UNSUPPORTED;
    EXPECT_CALL(*hdiDeviceMock_, GetSupportedPresentTimestampType(_, _, _))
        .WillRepeatedly(testing::Return(GRAPHIC_DISPLAY_NOT_SUPPORT));
    EXPECT_EQ(hdiDeviceMock_->GetSupportedPresentTimestampType(screenId, layerId, presentTimesType),
              GRAPHIC_DISPLAY_NOT_SUPPORT);
}

/*
* Function: LayerFuncs004
* Type: Function
* Rank: Important(3)
* EnvConditions: N/A
* CaseDescription: 1. call LayerFuncs
*                  2. check ret
*/
HWTEST_F(HdiDeviceTest, LayerFuncs004, Function | MediumTest| Level3)
{
    uint32_t screenId = UINT32_MAX, layerId = 0, zorder = 1;
    GraphicLayerAlpha alpha;
    EXPECT_CALL(*hdiDeviceMock_, SetLayerAlpha(_, _, _))
        .WillRepeatedly(testing::Return(GRAPHIC_DISPLAY_SUCCESS));
    EXPECT_EQ(hdiDeviceMock_->SetLayerAlpha(screenId, layerId, alpha), GRAPHIC_DISPLAY_SUCCESS);
    GraphicIRect layerRect = {0, 0, 0, 0};
    EXPECT_CALL(*hdiDeviceMock_, SetLayerSize(_, _, _))
        .WillRepeatedly(testing::Return(GRAPHIC_DISPLAY_SUCCESS));
    EXPECT_EQ(hdiDeviceMock_->SetLayerSize(screenId, layerId, layerRect), GRAPHIC_DISPLAY_SUCCESS);
    GraphicTransformType type = GRAPHIC_FLIP_H_ROT90;
    EXPECT_CALL(*hdiDeviceMock_, SetTransformMode(_, _, _))
        .WillRepeatedly(testing::Return(GRAPHIC_DISPLAY_SUCCESS));
    EXPECT_EQ(hdiDeviceMock_->SetTransformMode(screenId, layerId, type), GRAPHIC_DISPLAY_SUCCESS);
    std::vector<GraphicIRect> visibles = { {0, 0, 1, 1} };
    EXPECT_CALL(*hdiDeviceMock_, SetLayerVisibleRegion(_, _, _))
        .WillRepeatedly(testing::Return(GRAPHIC_DISPLAY_SUCCESS));
    EXPECT_EQ(hdiDeviceMock_->SetLayerVisibleRegion(screenId, layerId, visibles),
              GRAPHIC_DISPLAY_SUCCESS);
    std::vector<GraphicIRect> dirtyRegions = { {0, 0, 1, 1} };
    EXPECT_CALL(*hdiDeviceMock_, SetLayerDirtyRegion(_, _, _))
        .WillRepeatedly(testing::Return(GRAPHIC_DISPLAY_SUCCESS));
    EXPECT_EQ(hdiDeviceMock_->SetLayerDirtyRegion(screenId, layerId, dirtyRegions),
              GRAPHIC_DISPLAY_SUCCESS);
    BufferHandle *handle = nullptr;
    sptr<SyncFence> acquireFence = nullptr;
    std::vector<uint32_t> deletingList = {};
    uint32_t cacheIndex = INVALID_BUFFER_CACHE_INDEX;
    GraphicLayerBuffer layerBuffer = {handle, cacheIndex, acquireFence, deletingList};
    EXPECT_CALL(*hdiDeviceMock_, SetLayerBuffer(_, _, _))
        .WillRepeatedly(testing::Return(GRAPHIC_DISPLAY_PARAM_ERR));
    EXPECT_EQ(hdiDeviceMock_->SetLayerBuffer(screenId, layerId, layerBuffer), GRAPHIC_DISPLAY_PARAM_ERR);
    handle = new BufferHandle();
    EXPECT_EQ(hdiDeviceMock_->SetLayerBuffer(screenId, layerId, layerBuffer), GRAPHIC_DISPLAY_PARAM_ERR);
    layerBuffer.acquireFence = new SyncFence(10);
    EXPECT_EQ(hdiDeviceMock_->SetLayerBuffer(screenId, layerId, layerBuffer), GRAPHIC_DISPLAY_PARAM_ERR);
    layerBuffer.cacheIndex = 0;
    EXPECT_CALL(*hdiDeviceMock_,
        SetLayerBuffer(_, _, _)).WillRepeatedly(testing::Return(GRAPHIC_DISPLAY_SUCCESS));
    EXPECT_EQ(hdiDeviceMock_->SetLayerBuffer(screenId, layerId, layerBuffer),
              GRAPHIC_DISPLAY_SUCCESS);
    GraphicCompositionType cmpType = GRAPHIC_COMPOSITION_DEVICE;
    EXPECT_CALL(*hdiDeviceMock_, SetLayerCompositionType(_, _, _))
        .WillRepeatedly(testing::Return(GRAPHIC_DISPLAY_SUCCESS));
    EXPECT_EQ(hdiDeviceMock_->SetLayerCompositionType(screenId, layerId, cmpType),
              GRAPHIC_DISPLAY_SUCCESS);
    GraphicBlendType blendType = GRAPHIC_BLEND_NONE;
    EXPECT_CALL(*hdiDeviceMock_, SetLayerBlendType(_, _, _))
        .WillRepeatedly(testing::Return(GRAPHIC_DISPLAY_SUCCESS));
    EXPECT_EQ(hdiDeviceMock_->SetLayerBlendType(screenId, layerId, blendType),
              GRAPHIC_DISPLAY_SUCCESS);
    GraphicIRect crop = {0, 0, 1, 1};
    EXPECT_CALL(*hdiDeviceMock_, SetLayerCrop(_, _, _))
        .WillRepeatedly(testing::Return(GRAPHIC_DISPLAY_SUCCESS));
    EXPECT_EQ(hdiDeviceMock_->SetLayerCrop(screenId, layerId, crop), GRAPHIC_DISPLAY_SUCCESS);
    EXPECT_CALL(*hdiDeviceMock_, SetLayerZorder(_, _, _))
        .WillRepeatedly(testing::Return(GRAPHIC_DISPLAY_SUCCESS));
    EXPECT_EQ(hdiDeviceMock_->SetLayerZorder(screenId, layerId, zorder), GRAPHIC_DISPLAY_SUCCESS);
    bool isPreMulti = false;
    EXPECT_CALL(*hdiDeviceMock_, SetLayerPreMulti(_, _, _))
        .WillRepeatedly(testing::Return(GRAPHIC_DISPLAY_SUCCESS));
    EXPECT_EQ(hdiDeviceMock_->SetLayerPreMulti(screenId, layerId, isPreMulti), GRAPHIC_DISPLAY_SUCCESS);
    std::vector<float> matrix = { 0.0 };
    EXPECT_CALL(*hdiDeviceMock_, SetLayerColorTransform(_, _, _))
        .WillRepeatedly(testing::Return(GRAPHIC_DISPLAY_NOT_SUPPORT));
    EXPECT_EQ(hdiDeviceMock_->SetLayerColorTransform(screenId, layerId, matrix),
              GRAPHIC_DISPLAY_NOT_SUPPORT);
    GraphicColorDataSpace colorSpace = GRAPHIC_COLOR_DATA_SPACE_UNKNOWN;
    EXPECT_CALL(*hdiDeviceMock_, SetLayerColorDataSpace(_, _, _))
        .WillRepeatedly(testing::Return(GRAPHIC_DISPLAY_NOT_SUPPORT));
    EXPECT_EQ(hdiDeviceMock_->SetLayerColorDataSpace(screenId, layerId, colorSpace),
              GRAPHIC_DISPLAY_NOT_SUPPORT);
    EXPECT_CALL(*hdiDeviceMock_, GetLayerColorDataSpace(_, _, _))
        .WillRepeatedly(testing::Return(GRAPHIC_DISPLAY_NOT_SUPPORT));
    EXPECT_EQ(hdiDeviceMock_->GetLayerColorDataSpace(screenId, layerId, colorSpace),
              GRAPHIC_DISPLAY_NOT_SUPPORT);
    std::vector<GraphicHDRMetaData> metaData;
    EXPECT_CALL(*hdiDeviceMock_, SetLayerMetaData(_, _, _))
        .WillRepeatedly(testing::Return(GRAPHIC_DISPLAY_NOT_SUPPORT));
    EXPECT_EQ(hdiDeviceMock_->SetLayerMetaData(screenId, layerId, metaData), GRAPHIC_DISPLAY_NOT_SUPPORT);
    GraphicHDRMetadataKey key = GRAPHIC_MATAKEY_RED_PRIMARY_X;
    std::vector<uint8_t> metaDatas;
    EXPECT_CALL(*hdiDeviceMock_, SetLayerMetaDataSet(_, _, _, _))
        .WillRepeatedly(testing::Return(GRAPHIC_DISPLAY_NOT_SUPPORT));
    EXPECT_EQ(hdiDeviceMock_->SetLayerMetaDataSet(screenId, layerId, key, metaDatas),
              GRAPHIC_DISPLAY_NOT_SUPPORT);
    GraphicExtDataHandle *extDataHandle = nullptr;
    EXPECT_CALL(*hdiDeviceMock_, SetLayerTunnelHandle(_, _, _))
        .WillRepeatedly(testing::Return(GRAPHIC_DISPLAY_NOT_SUPPORT));
    EXPECT_EQ(hdiDeviceMock_->SetLayerTunnelHandle(screenId, layerId, extDataHandle),
              GRAPHIC_DISPLAY_NOT_SUPPORT);
    GraphicPresentTimestampType presentTimesType = GRAPHIC_DISPLAY_PTS_UNSUPPORTED;
    EXPECT_CALL(*hdiDeviceMock_, GetSupportedPresentTimestampType(_, _, _))
        .WillRepeatedly(testing::Return(GRAPHIC_DISPLAY_NOT_SUPPORT));
    EXPECT_EQ(hdiDeviceMock_->GetSupportedPresentTimestampType(screenId, layerId, presentTimesType),
              GRAPHIC_DISPLAY_NOT_SUPPORT);
}

/*
* Function: LayerFuncs005
* Type: Function
* Rank: Important(3)
* EnvConditions: N/A
* CaseDescription: 1. call LayerFuncs
*                  2. check ret
*/
HWTEST_F(HdiDeviceTest, LayerFuncs005, Function | MediumTest| Level3)
{
    uint32_t screenId = UINT32_MAX, layerId = 0, zorder = 1;
    GraphicLayerAlpha alpha;
    EXPECT_CALL(*hdiDeviceMock_, SetLayerAlpha(_, _, _))
        .WillRepeatedly(testing::Return(GRAPHIC_DISPLAY_SUCCESS));
    EXPECT_EQ(hdiDeviceMock_->SetLayerAlpha(screenId, layerId, alpha), GRAPHIC_DISPLAY_SUCCESS);
    GraphicIRect layerRect = {0, 0, 0, 0};
    EXPECT_CALL(*hdiDeviceMock_, SetLayerSize(_, _, _))
        .WillRepeatedly(testing::Return(GRAPHIC_DISPLAY_SUCCESS));
    EXPECT_EQ(hdiDeviceMock_->SetLayerSize(screenId, layerId, layerRect), GRAPHIC_DISPLAY_SUCCESS);
    GraphicTransformType type = GRAPHIC_FLIP_H_ROT90;
    EXPECT_CALL(*hdiDeviceMock_, SetTransformMode(_, _, _))
        .WillRepeatedly(testing::Return(GRAPHIC_DISPLAY_SUCCESS));
    EXPECT_EQ(hdiDeviceMock_->SetTransformMode(screenId, layerId, type), GRAPHIC_DISPLAY_SUCCESS);
    std::vector<GraphicIRect> visibles = { {0, 0, 1, 1} };
    EXPECT_CALL(*hdiDeviceMock_, SetLayerVisibleRegion(_, _, _))
        .WillRepeatedly(testing::Return(GRAPHIC_DISPLAY_SUCCESS));
    EXPECT_EQ(hdiDeviceMock_->SetLayerVisibleRegion(screenId, layerId, visibles),
              GRAPHIC_DISPLAY_SUCCESS);
    std::vector<GraphicIRect> dirtyRegions = { {0, 0, 1, 1} };
    EXPECT_CALL(*hdiDeviceMock_, SetLayerDirtyRegion(_, _, _))
        .WillRepeatedly(testing::Return(GRAPHIC_DISPLAY_SUCCESS));
    EXPECT_EQ(hdiDeviceMock_->SetLayerDirtyRegion(screenId, layerId, dirtyRegions),
              GRAPHIC_DISPLAY_SUCCESS);
    BufferHandle *handle = nullptr;
    sptr<SyncFence> acquireFence = nullptr;
    std::vector<uint32_t> deletingList = {};
    uint32_t cacheIndex = INVALID_BUFFER_CACHE_INDEX;
    GraphicLayerBuffer layerBuffer = {handle, cacheIndex, acquireFence, deletingList};
    EXPECT_CALL(*hdiDeviceMock_, SetLayerBuffer(_, _, _))
        .WillRepeatedly(testing::Return(GRAPHIC_DISPLAY_PARAM_ERR));
    EXPECT_EQ(hdiDeviceMock_->SetLayerBuffer(screenId, layerId, layerBuffer), GRAPHIC_DISPLAY_PARAM_ERR);
    handle = new BufferHandle();
    EXPECT_EQ(hdiDeviceMock_->SetLayerBuffer(screenId, layerId, layerBuffer), GRAPHIC_DISPLAY_PARAM_ERR);
    layerBuffer.acquireFence = new SyncFence(10);
    EXPECT_EQ(hdiDeviceMock_->SetLayerBuffer(screenId, layerId, layerBuffer), GRAPHIC_DISPLAY_PARAM_ERR);
    layerBuffer.cacheIndex = 0;
    EXPECT_CALL(*hdiDeviceMock_,
        SetLayerBuffer(_, _, _)).WillRepeatedly(testing::Return(GRAPHIC_DISPLAY_SUCCESS));
    EXPECT_EQ(hdiDeviceMock_->SetLayerBuffer(screenId, layerId, layerBuffer),
              GRAPHIC_DISPLAY_SUCCESS);
    GraphicCompositionType cmpType = GRAPHIC_COMPOSITION_DEVICE;
    EXPECT_CALL(*hdiDeviceMock_, SetLayerCompositionType(_, _, _))
        .WillRepeatedly(testing::Return(GRAPHIC_DISPLAY_SUCCESS));
    EXPECT_EQ(hdiDeviceMock_->SetLayerCompositionType(screenId, layerId, cmpType),
              GRAPHIC_DISPLAY_SUCCESS);
    GraphicBlendType blendType = GRAPHIC_BLEND_NONE;
    EXPECT_CALL(*hdiDeviceMock_, SetLayerBlendType(_, _, _))
        .WillRepeatedly(testing::Return(GRAPHIC_DISPLAY_SUCCESS));
    EXPECT_EQ(hdiDeviceMock_->SetLayerBlendType(screenId, layerId, blendType),
              GRAPHIC_DISPLAY_SUCCESS);
    GraphicIRect crop = {0, 0, 1, 1};
    EXPECT_CALL(*hdiDeviceMock_, SetLayerCrop(_, _, _))
        .WillRepeatedly(testing::Return(GRAPHIC_DISPLAY_SUCCESS));
    EXPECT_EQ(hdiDeviceMock_->SetLayerCrop(screenId, layerId, crop), GRAPHIC_DISPLAY_SUCCESS);
    EXPECT_CALL(*hdiDeviceMock_, SetLayerZorder(_, _, _))
        .WillRepeatedly(testing::Return(GRAPHIC_DISPLAY_SUCCESS));
    EXPECT_EQ(hdiDeviceMock_->SetLayerZorder(screenId, layerId, zorder), GRAPHIC_DISPLAY_SUCCESS);
    bool isPreMulti = false;
    EXPECT_CALL(*hdiDeviceMock_, SetLayerPreMulti(_, _, _))
        .WillRepeatedly(testing::Return(GRAPHIC_DISPLAY_SUCCESS));
    EXPECT_EQ(hdiDeviceMock_->SetLayerPreMulti(screenId, layerId, isPreMulti), GRAPHIC_DISPLAY_SUCCESS);
    std::vector<float> matrix =
        { 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f };
    EXPECT_CALL(*hdiDeviceMock_, SetLayerColorTransform(_, _, _))
        .WillRepeatedly(testing::Return(GRAPHIC_DISPLAY_NOT_SUPPORT));
    EXPECT_EQ(hdiDeviceMock_->SetLayerColorTransform(screenId, layerId, matrix),
              GRAPHIC_DISPLAY_NOT_SUPPORT);
    GraphicColorDataSpace colorSpace = GRAPHIC_COLOR_DATA_SPACE_UNKNOWN;
    EXPECT_CALL(*hdiDeviceMock_, SetLayerColorDataSpace(_, _, _))
        .WillRepeatedly(testing::Return(GRAPHIC_DISPLAY_NOT_SUPPORT));
    EXPECT_EQ(hdiDeviceMock_->SetLayerColorDataSpace(screenId, layerId, colorSpace),
              GRAPHIC_DISPLAY_NOT_SUPPORT);
    EXPECT_CALL(*hdiDeviceMock_, GetLayerColorDataSpace(_, _, _))
        .WillRepeatedly(testing::Return(GRAPHIC_DISPLAY_NOT_SUPPORT));
    EXPECT_EQ(hdiDeviceMock_->GetLayerColorDataSpace(screenId, layerId, colorSpace),
              GRAPHIC_DISPLAY_NOT_SUPPORT);
    std::vector<GraphicHDRMetaData> metaData;
    EXPECT_CALL(*hdiDeviceMock_, SetLayerMetaData(_, _, _))
        .WillRepeatedly(testing::Return(GRAPHIC_DISPLAY_NOT_SUPPORT));
    EXPECT_EQ(hdiDeviceMock_->SetLayerMetaData(screenId, layerId, metaData), GRAPHIC_DISPLAY_NOT_SUPPORT);
    GraphicHDRMetadataKey key = GRAPHIC_MATAKEY_RED_PRIMARY_X;
    std::vector<uint8_t> metaDatas;
    EXPECT_CALL(*hdiDeviceMock_, SetLayerMetaDataSet(_, _, _, _))
        .WillRepeatedly(testing::Return(GRAPHIC_DISPLAY_NOT_SUPPORT));
    EXPECT_EQ(hdiDeviceMock_->SetLayerMetaDataSet(screenId, layerId, key, metaDatas),
              GRAPHIC_DISPLAY_NOT_SUPPORT);
    GraphicExtDataHandle *extDataHandle = nullptr;
    EXPECT_CALL(*hdiDeviceMock_, SetLayerTunnelHandle(_, _, _))
        .WillRepeatedly(testing::Return(GRAPHIC_DISPLAY_NOT_SUPPORT));
    EXPECT_EQ(hdiDeviceMock_->SetLayerTunnelHandle(screenId, layerId, extDataHandle),
              GRAPHIC_DISPLAY_NOT_SUPPORT);
    GraphicPresentTimestampType presentTimesType = GRAPHIC_DISPLAY_PTS_UNSUPPORTED;
    EXPECT_CALL(*hdiDeviceMock_, GetSupportedPresentTimestampType(_, _, _))
        .WillRepeatedly(testing::Return(GRAPHIC_DISPLAY_NOT_SUPPORT));
    EXPECT_EQ(hdiDeviceMock_->GetSupportedPresentTimestampType(screenId, layerId, presentTimesType),
              GRAPHIC_DISPLAY_NOT_SUPPORT);
}

/*
* Function: LayerFuncs006
* Type: Function
* Rank: Important(3)
* EnvConditions: N/A
* CaseDescription: 1. call LayerFuncs
*                  2. check ret
*/
HWTEST_F(HdiDeviceTest, LayerFuncs006, Function | MediumTest| Level3)
{
    uint32_t screenId = UINT32_MAX, layerId = 0, zorder = 1;
    GraphicLayerAlpha alpha;
    EXPECT_CALL(*hdiDeviceMock_, SetLayerAlpha(_, _, _))
        .WillRepeatedly(testing::Return(GRAPHIC_DISPLAY_SUCCESS));
    EXPECT_EQ(hdiDeviceMock_->SetLayerAlpha(screenId, layerId, alpha), GRAPHIC_DISPLAY_SUCCESS);
    GraphicIRect layerRect = {0, 0, 0, 0};
    EXPECT_CALL(*hdiDeviceMock_, SetLayerSize(_, _, _))
        .WillRepeatedly(testing::Return(GRAPHIC_DISPLAY_SUCCESS));
    EXPECT_EQ(hdiDeviceMock_->SetLayerSize(screenId, layerId, layerRect), GRAPHIC_DISPLAY_SUCCESS);
    GraphicTransformType type = GRAPHIC_FLIP_H_ROT90;
    EXPECT_CALL(*hdiDeviceMock_, SetTransformMode(_, _, _))
        .WillRepeatedly(testing::Return(GRAPHIC_DISPLAY_SUCCESS));
    EXPECT_EQ(hdiDeviceMock_->SetTransformMode(screenId, layerId, type), GRAPHIC_DISPLAY_SUCCESS);
    std::vector<GraphicIRect> visibles = { {0, 0, 1, 1} };
    EXPECT_CALL(*hdiDeviceMock_, SetLayerVisibleRegion(_, _, _))
        .WillRepeatedly(testing::Return(GRAPHIC_DISPLAY_SUCCESS));
    EXPECT_EQ(hdiDeviceMock_->SetLayerVisibleRegion(screenId, layerId, visibles),
              GRAPHIC_DISPLAY_SUCCESS);
    std::vector<GraphicIRect> dirtyRegions = { {0, 0, 2, 2} };
    EXPECT_CALL(*hdiDeviceMock_, SetLayerDirtyRegion(_, _, _))
        .WillRepeatedly(testing::Return(GRAPHIC_DISPLAY_SUCCESS));
    EXPECT_EQ(hdiDeviceMock_->SetLayerDirtyRegion(screenId, layerId, dirtyRegions),
              GRAPHIC_DISPLAY_SUCCESS);
    BufferHandle *handle = nullptr;
    sptr<SyncFence> acquireFence = nullptr;
    std::vector<uint32_t> deletingList = {};
    uint32_t cacheIndex = INVALID_BUFFER_CACHE_INDEX;
    GraphicLayerBuffer layerBuffer = {handle, cacheIndex, acquireFence, deletingList};
    EXPECT_CALL(*hdiDeviceMock_, SetLayerBuffer(_, _, _))
        .WillRepeatedly(testing::Return(GRAPHIC_DISPLAY_PARAM_ERR));
    EXPECT_EQ(hdiDeviceMock_->SetLayerBuffer(screenId, layerId, layerBuffer), GRAPHIC_DISPLAY_PARAM_ERR);
    handle = new BufferHandle();
    EXPECT_EQ(hdiDeviceMock_->SetLayerBuffer(screenId, layerId, layerBuffer), GRAPHIC_DISPLAY_PARAM_ERR);
    layerBuffer.acquireFence = new SyncFence(10);
    EXPECT_EQ(hdiDeviceMock_->SetLayerBuffer(screenId, layerId, layerBuffer), GRAPHIC_DISPLAY_PARAM_ERR);
    layerBuffer.cacheIndex = 0;
    EXPECT_CALL(*hdiDeviceMock_,
        SetLayerBuffer(_, _, _)).WillRepeatedly(testing::Return(GRAPHIC_DISPLAY_SUCCESS));
    EXPECT_EQ(hdiDeviceMock_->SetLayerBuffer(screenId, layerId, layerBuffer),
              GRAPHIC_DISPLAY_SUCCESS);
    GraphicCompositionType cmpType = GRAPHIC_COMPOSITION_DEVICE;
    EXPECT_CALL(*hdiDeviceMock_, SetLayerCompositionType(_, _, _))
        .WillRepeatedly(testing::Return(GRAPHIC_DISPLAY_SUCCESS));
    EXPECT_EQ(hdiDeviceMock_->SetLayerCompositionType(screenId, layerId, cmpType),
              GRAPHIC_DISPLAY_SUCCESS);
    GraphicBlendType blendType = GRAPHIC_BLEND_NONE;
    EXPECT_CALL(*hdiDeviceMock_, SetLayerBlendType(_, _, _))
        .WillRepeatedly(testing::Return(GRAPHIC_DISPLAY_SUCCESS));
    EXPECT_EQ(hdiDeviceMock_->SetLayerBlendType(screenId, layerId, blendType),
              GRAPHIC_DISPLAY_SUCCESS);
    GraphicIRect crop = {0, 0, 1, 1};
    EXPECT_CALL(*hdiDeviceMock_, SetLayerCrop(_, _, _))
        .WillRepeatedly(testing::Return(GRAPHIC_DISPLAY_SUCCESS));
    EXPECT_EQ(hdiDeviceMock_->SetLayerCrop(screenId, layerId, crop), GRAPHIC_DISPLAY_SUCCESS);
    EXPECT_CALL(*hdiDeviceMock_, SetLayerZorder(_, _, _))
        .WillRepeatedly(testing::Return(GRAPHIC_DISPLAY_SUCCESS));
    EXPECT_EQ(hdiDeviceMock_->SetLayerZorder(screenId, layerId, zorder), GRAPHIC_DISPLAY_SUCCESS);
    bool isPreMulti = false;
    EXPECT_CALL(*hdiDeviceMock_, SetLayerPreMulti(_, _, _))
        .WillRepeatedly(testing::Return(GRAPHIC_DISPLAY_SUCCESS));
    EXPECT_EQ(hdiDeviceMock_->SetLayerPreMulti(screenId, layerId, isPreMulti), GRAPHIC_DISPLAY_SUCCESS);
    std::vector<float> matrix =
        { 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f };
    EXPECT_CALL(*hdiDeviceMock_, SetLayerColorTransform(_, _, _))
        .WillRepeatedly(testing::Return(GRAPHIC_DISPLAY_NOT_SUPPORT));
    EXPECT_EQ(hdiDeviceMock_->SetLayerColorTransform(screenId, layerId, matrix),
              GRAPHIC_DISPLAY_NOT_SUPPORT);
    GraphicColorDataSpace colorSpace = GRAPHIC_COLOR_DATA_SPACE_UNKNOWN;
    EXPECT_CALL(*hdiDeviceMock_, SetLayerColorDataSpace(_, _, _))
        .WillRepeatedly(testing::Return(GRAPHIC_DISPLAY_NOT_SUPPORT));
    EXPECT_EQ(hdiDeviceMock_->SetLayerColorDataSpace(screenId, layerId, colorSpace),
              GRAPHIC_DISPLAY_NOT_SUPPORT);
    EXPECT_CALL(*hdiDeviceMock_, GetLayerColorDataSpace(_, _, _))
        .WillRepeatedly(testing::Return(GRAPHIC_DISPLAY_NOT_SUPPORT));
    EXPECT_EQ(hdiDeviceMock_->GetLayerColorDataSpace(screenId, layerId, colorSpace),
              GRAPHIC_DISPLAY_NOT_SUPPORT);
    std::vector<GraphicHDRMetaData> metaData;
    EXPECT_CALL(*hdiDeviceMock_, SetLayerMetaData(_, _, _))
        .WillRepeatedly(testing::Return(GRAPHIC_DISPLAY_NOT_SUPPORT));
    EXPECT_EQ(hdiDeviceMock_->SetLayerMetaData(screenId, layerId, metaData), GRAPHIC_DISPLAY_NOT_SUPPORT);
    GraphicHDRMetadataKey key = GRAPHIC_MATAKEY_RED_PRIMARY_X;
    std::vector<uint8_t> metaDatas;
    EXPECT_CALL(*hdiDeviceMock_, SetLayerMetaDataSet(_, _, _, _))
        .WillRepeatedly(testing::Return(GRAPHIC_DISPLAY_NOT_SUPPORT));
    EXPECT_EQ(hdiDeviceMock_->SetLayerMetaDataSet(screenId, layerId, key, metaDatas),
              GRAPHIC_DISPLAY_NOT_SUPPORT);
    GraphicExtDataHandle *extDataHandle = nullptr;
    EXPECT_CALL(*hdiDeviceMock_, SetLayerTunnelHandle(_, _, _))
        .WillRepeatedly(testing::Return(GRAPHIC_DISPLAY_NOT_SUPPORT));
    EXPECT_EQ(hdiDeviceMock_->SetLayerTunnelHandle(screenId, layerId, extDataHandle),
              GRAPHIC_DISPLAY_NOT_SUPPORT);
    GraphicPresentTimestampType presentTimesType = GRAPHIC_DISPLAY_PTS_UNSUPPORTED;
    EXPECT_CALL(*hdiDeviceMock_, GetSupportedPresentTimestampType(_, _, _))
        .WillRepeatedly(testing::Return(GRAPHIC_DISPLAY_NOT_SUPPORT));
    EXPECT_EQ(hdiDeviceMock_->GetSupportedPresentTimestampType(screenId, layerId, presentTimesType),
              GRAPHIC_DISPLAY_NOT_SUPPORT);
}

/*
* Function: GetDisplayClientTargetProperty
* Type: Function
* Rank: Important(3)
* EnvConditions: N/A
* CaseDescription: 1. call GetDisplayClientTargetProperty
*                  2. check ret
*/
HWTEST_F(HdiDeviceTest, GetDisplayClientTargetProperty, Function | MediumTest| Level3)
{
    uint32_t screenId = 0;
    int32_t pixelFormat = 0;
    int32_t dataspace = 0;
    EXPECT_CALL(*hdiDeviceMock_, GetDisplayClientTargetProperty(_, _, _)).WillRepeatedly(
            testing::Return(GRAPHIC_DISPLAY_SUCCESS));
    EXPECT_EQ(hdiDeviceMock_->GetDisplayClientTargetProperty(screenId, pixelFormat, dataspace),
              GRAPHIC_DISPLAY_SUCCESS);
    EXPECT_CALL(*hdiDeviceMock_, GetDisplayClientTargetProperty(_, _, _)).WillRepeatedly(
            testing::Return(GRAPHIC_DISPLAY_NOT_SUPPORT));
    EXPECT_EQ(hdiDeviceMock_->GetDisplayClientTargetProperty(screenId, pixelFormat, dataspace),
              GRAPHIC_DISPLAY_NOT_SUPPORT);
}

/*
* Function: GetPanelPowerStatus001
* Type: Function
* Rank: Important(3)
* EnvConditions: N/A
* CaseDescription: 1. call GetPanelPowerStatus
*                  2. check ret
*/
HWTEST_F(HdiDeviceTest, GetPanelPowerStatus001, Function | MediumTest| Level3)
{
    uint32_t devId = 0;
    GraphicPanelPowerStatus status;
    EXPECT_CALL(*hdiDeviceMock_, GetPanelPowerStatus(_, _))
        .WillRepeatedly(testing::Return(GRAPHIC_DISPLAY_SUCCESS));
    EXPECT_EQ(hdiDeviceMock_->GetPanelPowerStatus(devId, status), GRAPHIC_DISPLAY_SUCCESS);
}

/*
* Function: SetScreenClientBuffer001
* Type: Function
* Rank: Important(3)
* EnvConditions: N/A
* CaseDescription: 1. call SetScreenClientBuffer with nullptr buffer and invalid cacheIndex
*                  2. check ret
*/
HWTEST_F(HdiDeviceTest, SetScreenClientBuffer001, Function | MediumTest| Level3)
{
    uint32_t screenId = 0;
    BufferHandle *buffer = nullptr;
    uint32_t cacheIndex = INVALID_BUFFER_CACHE_INDEX;
    sptr<SyncFence> fence = new SyncFence(-1);
    EXPECT_EQ(hdiDeviceMock_->SetScreenClientBuffer(screenId, buffer, cacheIndex, fence),
              GRAPHIC_DISPLAY_PARAM_ERR);
}

/*
* Function: SetScreenClientBuffer002
* Type: Function
* Rank: Important(3)
* EnvConditions: N/A
* CaseDescription: 1. call SetScreenClientBuffer with nullptr fence
*                  2. check ret
*/
HWTEST_F(HdiDeviceTest, SetScreenClientBuffer002, Function | MediumTest| Level3)
{
    uint32_t screenId = 0;
    BufferHandle *buffer = new BufferHandle();
    uint32_t cacheIndex = 0;
    sptr<SyncFence> fence = nullptr;
    EXPECT_EQ(hdiDeviceMock_->SetScreenClientBuffer(screenId, buffer, cacheIndex, fence),
              GRAPHIC_DISPLAY_PARAM_ERR);
}

/*
* Function: SetScreenClientBufferCacheCount001
* Type: Function
* Rank: Important(3)
* EnvConditions: N/A
* CaseDescription: 1. call SetScreenClientBufferCacheCount with count = 0
*                  2. check ret
*/
HWTEST_F(HdiDeviceTest, SetScreenClientBufferCacheCount001, Function | MediumTest| Level3)
{
    uint32_t screenId = 0;
    uint32_t count = 0;
    EXPECT_EQ(hdiDeviceMock_->SetScreenClientBufferCacheCount(screenId, count), GRAPHIC_DISPLAY_PARAM_ERR);
}

/*
* Function: SetScreenClientBufferCacheCount002
* Type: Function
* Rank: Important(3)
* EnvConditions: N/A
* CaseDescription: 1. call SetScreenClientBufferCacheCount with count > SURFACE_MAX_QUEUE_SIZE
*                  2. check ret
*/
HWTEST_F(HdiDeviceTest, SetScreenClientBufferCacheCount002, Function | MediumTest| Level3)
{
    uint32_t screenId = 0;
    uint32_t count = SURFACE_MAX_QUEUE_SIZE + 1;
    EXPECT_EQ(hdiDeviceMock_->SetScreenClientBufferCacheCount(screenId, count), GRAPHIC_DISPLAY_PARAM_ERR);
}

/*
* Function: SetLayerBuffer001
* Type: Function
* Rank: Important(3)
* EnvConditions: N/A
* CaseDescription: 1. call SetLayerBuffer with nullptr handle and invalid cacheIndex
*                  2. check ret
*/
HWTEST_F(HdiDeviceTest, SetLayerBuffer001, Function | MediumTest| Level3)
{
    uint32_t screenId = 0;
    uint32_t layerId = 0;
    GraphicLayerBuffer layerBuffer = {nullptr, INVALID_BUFFER_CACHE_INDEX, nullptr, {}};
    EXPECT_EQ(hdiDeviceMock_->SetLayerBuffer(screenId, layerId, layerBuffer), GRAPHIC_DISPLAY_PARAM_ERR);
}

/*
* Function: SetLayerBuffer002
* Type: Function
* Rank: Important(3)
* EnvConditions: N/A
* CaseDescription: 1. call SetLayerBuffer with nullptr acquireFence
*                  2. check ret
*/
HWTEST_F(HdiDeviceTest, SetLayerBuffer002, Function | MediumTest| Level3)
{
    uint32_t screenId = 0;
    uint32_t layerId = 0;
    BufferHandle *handle = new BufferHandle();
    GraphicLayerBuffer layerBuffer = {handle, 0, nullptr, {}};
    EXPECT_EQ(hdiDeviceMock_->SetLayerBuffer(screenId, layerId, layerBuffer), GRAPHIC_DISPLAY_PARAM_ERR);
}

/*
* Function: CommitAndGetReleaseFence001
* Type: Function
* Rank: Important(3)
* EnvConditions: N/A
* CaseDescription: 1. call CommitAndGetReleaseFence with skipState = 0
*                  2. check ret
*/
HWTEST_F(HdiDeviceTest, CommitAndGetReleaseFence001, Function | MediumTest| Level3)
{
    uint32_t screenId = 0;
    sptr<SyncFence> fence = nullptr;
    int32_t skipState = 0;
    bool needFlush = false;
    std::vector<uint32_t> layers;
    std::vector<sptr<SyncFence>> fences;
    bool isValidated = false;

    EXPECT_CALL(*hdiDeviceMock_, CommitAndGetReleaseFence(_, _, _, _, _, _, _))
        .WillRepeatedly(DoAll(
            testing::SetArgReferee<1>(10),
            testing::SetArgReferee<2>(skipState),
            testing::SetArgReferee<3>(needFlush),
            testing::SetArgReferee<4>(layers),
            testing::Return(GRAPHIC_DISPLAY_SUCCESS)
        ));
    EXPECT_EQ(
        hdiDeviceMock_->CommitAndGetReleaseFence(screenId, fence, skipState, needFlush, layers, fences, isValidated),
        GRAPHIC_DISPLAY_SUCCESS);
}

/*
* Function: CommitAndGetReleaseFence002
* Type: Function
* Rank: Important(3)
* EnvConditions: N/A
* CaseDescription: 1. call CommitAndGetReleaseFence with negative fenceFd
*                  2. check ret
*/
HWTEST_F(HdiDeviceTest, CommitAndGetReleaseFence002, Function | MediumTest| Level3)
{
    uint32_t screenId = 0;
    sptr<SyncFence> fence = nullptr;
    int32_t skipState = 1;
    bool needFlush = false;
    std::vector<uint32_t> layers;
    std::vector<sptr<SyncFence>> fences;
    std::vector<int32_t> fenceFds = {-1};
    bool isValidated = false;

    EXPECT_CALL(*hdiDeviceMock_, CommitAndGetReleaseFence(_, _, _, _, _, _, _))
        .WillRepeatedly(DoAll(
            testing::SetArgReferee<1>(-1),
            testing::SetArgReferee<2>(skipState),
            testing::SetArgReferee<3>(needFlush),
            testing::SetArgReferee<4>(layers),
            testing::SetArgReferee<5>(fenceFds),
            testing::Return(GRAPHIC_DISPLAY_SUCCESS)
        ));
    EXPECT_EQ(
        hdiDeviceMock_->CommitAndGetReleaseFence(screenId, fence, skipState, needFlush, layers, fences, isValidated),
        GRAPHIC_DISPLAY_SUCCESS);
}

/*
* Function: GetScreenSupportedModes001
* Type: Function
* Rank: Important(3)
* EnvConditions: N/A
* CaseDescription: 1. call GetScreenSupportedModes with failure return
*                  2. check ret
*/
HWTEST_F(HdiDeviceTest, GetScreenSupportedModes001, Function | MediumTest| Level3)
{
    uint32_t screenId = 0;
    std::vector<GraphicDisplayModeInfo> modes;
    EXPECT_CALL(*hdiDeviceMock_, GetScreenSupportedModes(_, _))
        .WillRepeatedly(testing::Return(GRAPHIC_DISPLAY_FAILURE));
    EXPECT_EQ(hdiDeviceMock_->GetScreenSupportedModes(screenId, modes),
              GRAPHIC_DISPLAY_FAILURE);
}

/*
* Function: GetScreenSupportedColorGamuts001
* Type: Function
* Rank: Important(3)
* EnvConditions: N/A
* CaseDescription: 1. call GetScreenSupportedColorGamuts with failure return
*                  2. check ret
*/
HWTEST_F(HdiDeviceTest, GetScreenSupportedColorGamuts001, Function | MediumTest| Level3)
{
    uint32_t screenId = 0;
    std::vector<GraphicColorGamut> gamuts;
    EXPECT_CALL(*hdiDeviceMock_, GetScreenSupportedColorGamuts(_, _))
        .WillRepeatedly(testing::Return(GRAPHIC_DISPLAY_FAILURE));
    EXPECT_EQ(hdiDeviceMock_->GetScreenSupportedColorGamuts(screenId, gamuts),
              GRAPHIC_DISPLAY_FAILURE);
}

/*
* Function: GetHDRCapabilityInfos001
* Type: Function
* Rank: Important(3)
* EnvConditions: N/A
* CaseDescription: 1. call GetHDRCapabilityInfos with failure return
*                  2. check ret
*/
HWTEST_F(HdiDeviceTest, GetHDRCapabilityInfos001, Function | MediumTest| Level3)
{
    uint32_t screenId = 0;
    GraphicHDRCapability info;
    EXPECT_CALL(*hdiDeviceMock_, GetHDRCapabilityInfos(_, _))
        .WillRepeatedly(testing::Return(GRAPHIC_DISPLAY_FAILURE));
    EXPECT_EQ(hdiDeviceMock_->GetHDRCapabilityInfos(screenId, info),
              GRAPHIC_DISPLAY_FAILURE);
}

/*
* Function: GetSupportedMetaDataKey001
* Type: Function
* Rank: Important(3)
* EnvConditions: N/A
* CaseDescription: 1. call GetSupportedMetaDataKey with failure return
*                  2. check ret
*/
HWTEST_F(HdiDeviceTest, GetSupportedMetaDataKey001, Function | MediumTest| Level3)
{
    uint32_t screenId = 0;
    std::vector<GraphicHDRMetadataKey> keys;
    EXPECT_CALL(*hdiDeviceMock_, GetSupportedMetaDataKey(_, _))
        .WillRepeatedly(testing::Return(GRAPHIC_DISPLAY_FAILURE));
    EXPECT_EQ(hdiDeviceMock_->GetSupportedMetaDataKey(screenId, keys),
              GRAPHIC_DISPLAY_FAILURE);
}

/*
* Function: SetScreenActiveRect001
* Type: Function
* Rank: Important(3)
* EnvConditions: N/A
* CaseDescription: 1. call SetScreenActiveRect with invalid rect (w <= 0)
*                  2. check ret
*/
HWTEST_F(HdiDeviceTest, SetScreenActiveRect001, Function | MediumTest| Level3)
{
    uint32_t screenId = 0;
    GraphicIRect activeRect = {0, 0, -1, 100};
    EXPECT_EQ(hdiDeviceMock_->SetScreenActiveRect(screenId, activeRect), GRAPHIC_DISPLAY_PARAM_ERR);
}

/*
* Function: SetScreenActiveRect002
* Type: Function
* Rank: Important(3)
* EnvConditions: N/A
* CaseDescription: 1. call SetScreenActiveRect with invalid rect (h <= 0)
*                  2. check ret
*/
HWTEST_F(HdiDeviceTest, SetScreenActiveRect002, Function | MediumTest| Level3)
{
    uint32_t screenId = 0;
    GraphicIRect activeRect = {0, 0, 100, -1};
    EXPECT_EQ(hdiDeviceMock_->SetScreenActiveRect(screenId, activeRect), GRAPHIC_DISPLAY_PARAM_ERR);
}

/*
* Function: GetScreenCapability001
* Type: Function
* Rank: Important(3)
* EnvConditions: N/A
* CaseDescription: 1. call GetScreenCapability with failure return
*                  2. check ret
*/
HWTEST_F(HdiDeviceTest, GetScreenCapability001, Function | MediumTest| Level3)
{
    uint32_t screenId = 0;
    GraphicDisplayCapability info;
    EXPECT_CALL(*hdiDeviceMock_, GetScreenCapability(_, _))
        .WillRepeatedly(testing::Return(GRAPHIC_DISPLAY_FAILURE));
    EXPECT_EQ(hdiDeviceMock_->GetScreenCapability(screenId, info), GRAPHIC_DISPLAY_FAILURE);
}

/*
* Function: SetScreenClientBuffer003
* Type: Function
* Rank: Important(3)
* EnvConditions: N/A
* CaseDescription: 1. call SetScreenClientBuffer with valid parameters
*                  2. check ret SUCCESS
*/
HWTEST_F(HdiDeviceTest, SetScreenClientBuffer003, Function | MediumTest| Level3)
{
    uint32_t screenId = 0;
    BufferHandle *buffer = new BufferHandle();
    uint32_t cacheIndex = 0;
    sptr<SyncFence> fence = new SyncFence(10);
    EXPECT_CALL(*hdiDeviceMock_, SetScreenClientBuffer(_, _, _, _))
        .WillRepeatedly(testing::Return(GRAPHIC_DISPLAY_SUCCESS));
    EXPECT_EQ(hdiDeviceMock_->SetScreenClientBuffer(screenId, buffer, cacheIndex, fence),
              GRAPHIC_DISPLAY_SUCCESS);
}

/*
* Function: SetScreenClientBufferCacheCount003
* Type: Function
* Rank: Important(3)
* EnvConditions: N/A
* CaseDescription: 1. call SetScreenClientBufferCacheCount with valid count
*                  2. check ret SUCCESS
*/
HWTEST_F(HdiDeviceTest, SetScreenClientBufferCacheCount003, Function | MediumTest| Level3)
{
    uint32_t screenId = 0;
    uint32_t count = 3;
    EXPECT_CALL(*hdiDeviceMock_, SetScreenClientBufferCacheCount(_, _))
        .WillRepeatedly(testing::Return(GRAPHIC_DISPLAY_SUCCESS));
    EXPECT_EQ(hdiDeviceMock_->SetScreenClientBufferCacheCount(screenId, count),
              GRAPHIC_DISPLAY_SUCCESS);
}

/*
* Function: SetLayerBuffer003
* Type: Function
* Rank: Important(3)
* EnvConditions: N/A
* CaseDescription: 1. call SetLayerBuffer with valid parameters
*                  2. check ret SUCCESS
*/
HWTEST_F(HdiDeviceTest, SetLayerBuffer003, Function | MediumTest| Level3)
{
    uint32_t screenId = 0;
    uint32_t layerId = 0;
    BufferHandle *handle = new BufferHandle();
    sptr<SyncFence> acquireFence = new SyncFence(10);
    std::vector<uint32_t> deletingList;
    GraphicLayerBuffer layerBuffer = {handle, 0, acquireFence, deletingList};
    EXPECT_CALL(*hdiDeviceMock_, SetLayerBuffer(_, _, _))
        .WillRepeatedly(testing::Return(GRAPHIC_DISPLAY_SUCCESS));
    EXPECT_EQ(hdiDeviceMock_->SetLayerBuffer(screenId, layerId, layerBuffer),
              GRAPHIC_DISPLAY_SUCCESS);
}

/*
* Function: SetScreenActiveRect003
* Type: Function
* Rank: Important(3)
* EnvConditions: N/A
* CaseDescription: 1. call SetScreenActiveRect with valid rect
*                  2. check ret SUCCESS
*/
HWTEST_F(HdiDeviceTest, SetScreenActiveRect003, Function | MediumTest| Level3)
{
    uint32_t screenId = 0;
    GraphicIRect activeRect = {0, 0, 100, 100};
    EXPECT_CALL(*hdiDeviceMock_, SetScreenActiveRect(_, _))
        .WillRepeatedly(testing::Return(GRAPHIC_DISPLAY_SUCCESS));
    EXPECT_EQ(hdiDeviceMock_->SetScreenActiveRect(screenId, activeRect), GRAPHIC_DISPLAY_SUCCESS);
}
} // namespace
} // namespace Rosen
} // namespace OHOS