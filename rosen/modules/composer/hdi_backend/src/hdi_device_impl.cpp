/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include "hdi_device_impl.h"
#include <cstddef>
#include <cstdlib>
#include <iterator>
#include <mutex>
#include <scoped_bytrace.h>
#include <valarray>
#include <securec.h>

#define CHECK_FUNC(composerSptr)                                     \
    do {                                                             \
        if (composerSptr == nullptr) {                               \
            HLOGD("[%{public}s]composerSptr is nullptr.", __func__); \
            return GRAPHIC_DISPLAY_NULL_PTR;                         \
        }                                                            \
    } while(0) 

namespace OHOS {
namespace Rosen {
using namespace OHOS::HDI::Display::Composer::V1_0;
HdiDeviceImpl::HdiDeviceImpl()
{
}

HdiDeviceImpl::~HdiDeviceImpl()
{
    Destroy();
}

RosenError HdiDeviceImpl::Init()
{
    if (composer_ == nullptr) {
        composer_.reset(IDisplayComposerInterface::Get());
        if (composer_ == nullptr) {
            return ROSEN_ERROR_NOT_INIT;
        }
    }
    return ROSEN_ERROR_OK;
}

void HdiDeviceImpl::Destroy()
{
    composer_ = nullptr;
}

/* set & get device screen info begin */
int32_t HdiDeviceImpl::RegHotPlugCallback(HotPlugCallback callback, void *data)
{
    CHECK_FUNC(composer_);
    return composer_->RegHotPlugCallback(callback, data);
}

int32_t HdiDeviceImpl::RegScreenVBlankCallback(uint32_t screenId, VBlankCallback callback, void *data)
{
    CHECK_FUNC(composer_);
    return composer_->RegDisplayVBlankCallback(screenId, callback, data);
}

int32_t HdiDeviceImpl::SetScreenVsyncEnabled(uint32_t screenId, bool enabled)
{
    CHECK_FUNC(composer_);
    return composer_->SetDisplayVsyncEnabled(screenId, enabled);
}

int32_t HdiDeviceImpl::GetScreenCapability(uint32_t screenId, GraphicDisplayCapability &info)
{
    CHECK_FUNC(composer_);
    DisplayCapability hdiInfo;
    int32_t ret = composer_->GetDisplayCapability(screenId, hdiInfo);
    if (ret == GRAPHIC_DISPLAY_SUCCESS) {
        info.name = hdiInfo.name;
        info.type = static_cast<GraphicInterfaceType>(hdiInfo.type);
        info.phyWidth = hdiInfo.phyWidth;
        info.phyHeight = hdiInfo.phyHeight;
        info.supportLayers = hdiInfo.supportLayers;
        info.virtualDispCount = hdiInfo.virtualDispCount;
        info.supportWriteBack = hdiInfo.supportWriteBack;
        info.propertyCount = hdiInfo.propertyCount;
        info.props = (GraphicPropertyObject*)malloc(info.propertyCount * sizeof(GraphicPropertyObject));
        for (uint32_t i = 0; i < info.propertyCount; i++) {
            info.props[i].name = hdiInfo.props[i].name;
            info.props[i].propId = hdiInfo.props[i].propId;
            info.props[i].value = hdiInfo.props[i].value;
        }
    }
    return ret;
}

int32_t HdiDeviceImpl::GetScreenSupportedModes(uint32_t screenId, std::vector<GraphicDisplayModeInfo> &modes)
{
    CHECK_FUNC(composer_);
    std::vector<DisplayModeInfo> hdiModes;
    int32_t ret = composer_->GetDisplaySupportedModes(screenId, hdiModes);
    if (ret != GRAPHIC_DISPLAY_SUCCESS) {
        return ret;
    }
    for (auto iter = hdiModes.begin(); iter != hdiModes.end(); iter++) {
        GraphicDisplayModeInfo tempMode = {
            .width = iter->width,
            .height = iter->height,
            .freshRate = iter->freshRate,
            .id = iter->id,
        };
        modes.emplace_back(tempMode);
    }
    return ret;
}

int32_t HdiDeviceImpl::GetScreenMode(uint32_t screenId, uint32_t &modeId)
{
    CHECK_FUNC(composer_);
    return composer_->GetDisplayMode(screenId, modeId);
}

int32_t HdiDeviceImpl::SetScreenMode(uint32_t screenId, uint32_t modeId)
{
    CHECK_FUNC(composer_);
    return composer_->SetDisplayMode(screenId, modeId);
}

int32_t HdiDeviceImpl::GetScreenPowerStatus(uint32_t screenId, GraphicDispPowerStatus &status)
{
    CHECK_FUNC(composer_);
    DispPowerStatus hdiStatus;
    int32_t ret = composer_->GetDisplayPowerStatus(screenId, hdiStatus);
    if (ret == GRAPHIC_DISPLAY_SUCCESS) {
        status = static_cast<GraphicDispPowerStatus>(hdiStatus);
    }
    return ret;
}

int32_t HdiDeviceImpl::SetScreenPowerStatus(uint32_t screenId, GraphicDispPowerStatus status)
{
    CHECK_FUNC(composer_);
    return composer_->SetDisplayPowerStatus(screenId, static_cast<DispPowerStatus>(status));
}

int32_t HdiDeviceImpl::GetScreenBacklight(uint32_t screenId, uint32_t &level)
{
    CHECK_FUNC(composer_);
    return composer_->GetDisplayBacklight(screenId, level);
}

int32_t HdiDeviceImpl::SetScreenBacklight(uint32_t screenId, uint32_t level)
{
    CHECK_FUNC(composer_);
    return composer_->SetDisplayBacklight(screenId, level);
}

int32_t HdiDeviceImpl::PrepareScreenLayers(uint32_t screenId, bool &needFlush)
{
    CHECK_FUNC(composer_);
    return composer_->PrepareDisplayLayers(screenId, needFlush);
}

int32_t HdiDeviceImpl::GetScreenCompChange(uint32_t screenId, std::vector<uint32_t> &layersId,
                                           std::vector<int32_t> &types)
{
    CHECK_FUNC(composer_);
    int32_t ret = composer_->GetDisplayCompChange(screenId, layersId, types);
    return ret;
}

int32_t HdiDeviceImpl::SetScreenClientBuffer(uint32_t screenId, const BufferHandle *buffer,
                                             const sptr<SyncFence> &fence)
{
    CHECK_FUNC(composer_);
    if (fence == nullptr) {
        return GRAPHIC_DISPLAY_NULL_PTR;
    }

    int32_t fenceFd = fence->Get();
    return composer_->SetDisplayClientBuffer(screenId, *buffer, fenceFd);
}

int32_t HdiDeviceImpl::SetScreenClientDamage(uint32_t screenId, std::vector<GraphicIRect>& damageRect)
{
    CHECK_FUNC(composer_);
    std::vector<IRect> hdiDamageRect;
    for (auto iter = damageRect.begin(); iter != damageRect.end(); iter++) {
        IRect tempDamageRect = {
            .x = iter->x,
            .y = iter->y,
            .w = iter->w,
            .h = iter->h,
        };
        hdiDamageRect.emplace_back(tempDamageRect);
    }
    return composer_->SetDisplayClientDamage(screenId, hdiDamageRect);
}

int32_t HdiDeviceImpl::GetScreenReleaseFence(uint32_t screenId, std::vector<uint32_t> &layers,
                                             std::vector<sptr<SyncFence>> &fences)
{
    CHECK_FUNC(composer_);
    std::vector<int32_t> fenceFds;
    int32_t ret = composer_->GetDisplayReleaseFence(screenId, layers, fenceFds);
    if (ret != GRAPHIC_DISPLAY_SUCCESS || fenceFds.size() <= 0) {
        return ret;
    }

    uint32_t fencesNum = static_cast<uint32_t>(fenceFds.size());
    fences.resize(fencesNum);
    for (uint32_t i = 0; i < fencesNum; i++) {
        if (fenceFds[i] >= 0) {
            fences[i] = new SyncFence(fenceFds[i]);
        } else {
            fences[i] = new SyncFence(-1);
        }
    }

    return ret;
}

int32_t HdiDeviceImpl::GetScreenSupportedColorGamuts(uint32_t screenId, std::vector<GraphicColorGamut> &gamuts)
{
    CHECK_FUNC(composer_);
    std::vector<ColorGamut> hdiGamuts;
    int32_t ret = composer_->GetDisplaySupportedColorGamuts(screenId, hdiGamuts);
    if (ret != GRAPHIC_DISPLAY_SUCCESS) {
        return ret;
    }
    
    for (auto iter = hdiGamuts.begin(); iter != hdiGamuts.end(); iter++) {
        GraphicColorGamut tempGamut = static_cast<GraphicColorGamut>(*iter);
        gamuts.emplace_back(tempGamut);
    }
    return ret;
}

int32_t HdiDeviceImpl::SetScreenColorGamut(uint32_t screenId, GraphicColorGamut gamut)
{
    CHECK_FUNC(composer_);
    return composer_->SetDisplayColorGamut(screenId, static_cast<ColorGamut>(gamut));
}

int32_t HdiDeviceImpl::GetScreenColorGamut(uint32_t screenId, GraphicColorGamut &gamut)
{
    CHECK_FUNC(composer_);
    ColorGamut hdiGamut;
    int32_t ret = composer_->GetDisplayColorGamut(screenId, hdiGamut);
    if (ret == GRAPHIC_DISPLAY_SUCCESS) {
        gamut = static_cast<GraphicColorGamut>(hdiGamut);
    }
    return ret;
}

int32_t HdiDeviceImpl::SetScreenGamutMap(uint32_t screenId, GraphicGamutMap gamutMap)
{
    CHECK_FUNC(composer_);
    return composer_->SetDisplayGamutMap(screenId, static_cast<GamutMap>(gamutMap));
}

int32_t HdiDeviceImpl::GetScreenGamutMap(uint32_t screenId, GraphicGamutMap &gamutMap)
{
    CHECK_FUNC(composer_);
    GamutMap hdiGamutMap;
    int32_t ret = composer_->GetDisplayGamutMap(screenId, hdiGamutMap);
    if (ret == GRAPHIC_DISPLAY_SUCCESS) {
        gamutMap = static_cast<GraphicGamutMap>(hdiGamutMap);
    }
    return ret;
}

int32_t HdiDeviceImpl::SetScreenColorTransform(uint32_t screenId, const float *matrix)
{
    CHECK_FUNC(composer_);
    std::vector<float> transformMatrix;
    for (uint32_t i = 0; i < COLOR_TRANSFORM_MATRIX_SIZE; i++) {
        transformMatrix.push_back(matrix[i]);
    }
    return composer_->SetDisplayColorTransform(screenId, transformMatrix);
}

int32_t HdiDeviceImpl::GetHDRCapabilityInfos(uint32_t screenId, GraphicHDRCapability &info)
{
    CHECK_FUNC(composer_);
    HDRCapability hdiInfo;
    int32_t ret = composer_->GetHDRCapabilityInfos(screenId, hdiInfo);
    if (ret != GRAPHIC_DISPLAY_SUCCESS) {
        return ret;
    }
    uint32_t formatCount = hdiInfo.formatCount;
    info.formats = (GraphicHDRFormat*)std::malloc(formatCount * sizeof(GRAPHIC_NOT_SUPPORT_HDR));
    if (memset_s(info.formats, sizeof(info.formats), 0, sizeof(info.formats))) {
        HLOGD("memset_s failed when get HDRCapability format infos.");
    }
    for (uint32_t i = 0; i < formatCount; i++) {
        info.formats[i] = static_cast<GraphicHDRFormat>(hdiInfo.formats[i]);
    }
    info.maxLum = hdiInfo.maxLum;
    info.maxAverageLum = hdiInfo.maxAverageLum;
    info.minLum = hdiInfo.minLum;
    return ret;
}

int32_t HdiDeviceImpl::GetSupportedMetaDataKey(uint32_t screenId, std::vector<GraphicHDRMetadataKey> &keys)
{
    CHECK_FUNC(composer_);
    std::vector<HDRMetadataKey> hdiKeys;
    int32_t ret = composer_->GetSupportedMetadataKey(screenId, hdiKeys);
    if (ret != GRAPHIC_DISPLAY_SUCCESS) {
        return ret;
    }

    for (auto iter = hdiKeys.begin(); iter != hdiKeys.end(); iter++) {
        GraphicHDRMetadataKey tempKey = static_cast<GraphicHDRMetadataKey>(*iter);
        keys.emplace_back(tempKey);
    }
    return ret;
}

int32_t HdiDeviceImpl::Commit(uint32_t screenId, sptr<SyncFence> &fence)
{
    ScopedBytrace bytrace(__func__);
    CHECK_FUNC(composer_);
    int32_t fenceFd = -1;
    int32_t ret = composer_->Commit(screenId, fenceFd);

    if (fenceFd >= 0) {
        fence = new SyncFence(fenceFd);
    } else {
        fence = new SyncFence(-1);
    }

    return ret;
}
/* set & get device screen info end */

/* set & get device layer info begin */
int32_t HdiDeviceImpl::SetLayerAlpha(uint32_t screenId, uint32_t layerId, GraphicLayerAlpha &alpha)
{
    CHECK_FUNC(composer_);
    LayerAlpha hdiLayerAlpha = {
        .enGlobalAlpha = alpha.enGlobalAlpha,
        .enPixelAlpha = alpha.enPixelAlpha,
        .alpha0 = alpha.alpha0,
        .alpha1 = alpha.alpha1,
        .gAlpha = alpha.gAlpha,
    };
    return composer_->SetLayerAlpha(screenId, layerId, hdiLayerAlpha);
}

int32_t HdiDeviceImpl::SetLayerSize(uint32_t screenId, uint32_t layerId, GraphicIRect &layerRect)
{
    CHECK_FUNC(composer_);
    IRect hdiLayerRect = {
        .x = layerRect.x,
        .y = layerRect.y,
        .w = layerRect.w,
        .h = layerRect.h,
    };
    return composer_->SetLayerPosition(screenId, layerId, hdiLayerRect);
}

int32_t HdiDeviceImpl::SetTransformMode(uint32_t screenId, uint32_t layerId, GraphicTransformType type)
{
    CHECK_FUNC(composer_);
    TransformType hdiType = static_cast<TransformType>(type);
    return composer_->SetTransformMode(screenId, layerId, hdiType);
}

int32_t HdiDeviceImpl::SetLayerVisibleRegion(uint32_t screenId, uint32_t layerId,
                                             std::vector<GraphicIRect> &visible)
{
    CHECK_FUNC(composer_);
    std::vector<IRect> hdiVisibleRects;
    for (auto iter = visible.begin(); iter != visible.end(); iter++) {
        IRect tempVisibleRect = {
            .x = iter->x,
            .y = iter->y,
            .w = iter->w,
            .h = iter->h,
        };
        hdiVisibleRects.emplace_back(tempVisibleRect);
    }
    return composer_->SetLayerVisibleRegion(screenId, layerId, hdiVisibleRects);
}

int32_t HdiDeviceImpl::SetLayerDirtyRegion(uint32_t screenId, uint32_t layerId, GraphicIRect &dirty)
{
    CHECK_FUNC(composer_);
    IRect hdiDirtyRect = {
        .x = dirty.x,
        .y = dirty.y,
        .w = dirty.w,
        .h = dirty.h,
    };
    return composer_->SetLayerDirtyRegion(screenId, layerId, hdiDirtyRect);
}

int32_t HdiDeviceImpl::SetLayerBuffer(uint32_t screenId, uint32_t layerId, const BufferHandle *handle,
                                      const sptr<SyncFence> &acquireFence)
{
    CHECK_FUNC(composer_);
    if (acquireFence == nullptr) {
        return GRAPHIC_DISPLAY_NULL_PTR;
    }
    int32_t fenceFd = acquireFence->Get();
    return composer_->SetLayerBuffer(screenId, layerId, *handle, fenceFd);
}

int32_t HdiDeviceImpl::SetLayerCompositionType(uint32_t screenId, uint32_t layerId, GraphicCompositionType type)
{
    CHECK_FUNC(composer_);
    CompositionType hdiType = static_cast<CompositionType>(type);
    return composer_->SetLayerCompositionType(screenId, layerId, hdiType);
}

int32_t HdiDeviceImpl::SetLayerBlendType(uint32_t screenId, uint32_t layerId, GraphicBlendType type)
{
    CHECK_FUNC(composer_);
    BlendType hdiBlendType = static_cast<BlendType>(type);
    return composer_->SetLayerBlendType(screenId, layerId, hdiBlendType);
}

int32_t HdiDeviceImpl::SetLayerCrop(uint32_t screenId, uint32_t layerId, GraphicIRect &crop)
{
    CHECK_FUNC(composer_);
    IRect hdiCropRect = {
        .x = crop.x,
        .y = crop.y,
        .w = crop.w,
        .h = crop.h,
    };
    return composer_->SetLayerCrop(screenId, layerId, hdiCropRect);
}

int32_t HdiDeviceImpl::SetLayerZorder(uint32_t screenId, uint32_t layerId, uint32_t zorder)
{
    CHECK_FUNC(composer_);
    return composer_->SetLayerZorder(screenId, layerId, zorder);
}

int32_t HdiDeviceImpl::SetLayerPreMulti(uint32_t screenId, uint32_t layerId, bool isPreMulti)
{
    CHECK_FUNC(composer_);
    return composer_->SetLayerPreMulti(screenId, layerId, isPreMulti);
}

int32_t HdiDeviceImpl::SetLayerColorTransform(uint32_t screenId, uint32_t layerId, const float *matrix)
{
    CHECK_FUNC(composer_);
    std::vector<float> transformMatrix;
    for (uint32_t i = 0; i < COLOR_TRANSFORM_MATRIX_SIZE; i++) {
        transformMatrix.push_back(matrix[i]);
    }
    return composer_->SetLayerColorTransform(screenId, layerId, transformMatrix);
}

int32_t HdiDeviceImpl::SetLayerColorDataSpace(uint32_t screenId, uint32_t layerId, GraphicColorDataSpace colorSpace)
{
    CHECK_FUNC(composer_);
    ColorDataSpace hdiColorDataSpace = static_cast<ColorDataSpace>(colorSpace);
    return composer_->SetLayerColorDataSpace(screenId, layerId, hdiColorDataSpace);
}

int32_t HdiDeviceImpl::GetLayerColorDataSpace(uint32_t screenId, uint32_t layerId, GraphicColorDataSpace &colorSpace)
{
    CHECK_FUNC(composer_);
    ColorDataSpace hdiColorDataSpace = COLOR_DATA_SPACE_UNKNOWN;
    int32_t ret = composer_->GetLayerColorDataSpace(screenId, layerId, hdiColorDataSpace);
    if (ret == GRAPHIC_DISPLAY_SUCCESS) {
        colorSpace = static_cast<GraphicColorDataSpace>(hdiColorDataSpace);
    }
    return ret;
}

int32_t HdiDeviceImpl::SetLayerMetaData(uint32_t screenId, uint32_t layerId,
                                        const std::vector<GraphicHDRMetaData> &metaData)
{
    CHECK_FUNC(composer_);
    std::vector<HDRMetaData> hdiMetaDatas;
    std::size_t metaDataSize = metaData.size();
    for (std::size_t i = 0; i < metaDataSize; i++) {
        HDRMetaData hdiMetaData = {
            .key = static_cast<HDRMetadataKey>(metaData[i].key),
            .value = metaData[i].value,
        };
        hdiMetaDatas.emplace_back(hdiMetaData);
    }
    return composer_->SetLayerMetaData(screenId, layerId, hdiMetaDatas);
}

int32_t HdiDeviceImpl::SetLayerMetaDataSet(uint32_t screenId, uint32_t layerId, GraphicHDRMetadataKey key,
                                           const std::vector<uint8_t> &metaData)
{
    CHECK_FUNC(composer_);
    HDRMetadataKey hdiKey = static_cast<HDRMetadataKey>(key);
    return composer_->SetLayerMetaDataSet(screenId, layerId, hdiKey, metaData);
}

int32_t HdiDeviceImpl::SetLayerTunnelHandle(uint32_t screenId, uint32_t layerId, OHExtDataHandle *handle)
{
    CHECK_FUNC(composer_);
    return composer_->SetLayerTunnelHandle(screenId, layerId, (*(reinterpret_cast<ExtDataHandle *>(handle))));
}

int32_t HdiDeviceImpl::GetSupportedPresentTimestampType(uint32_t screenId, uint32_t layerId,
                                                        GraphicPresentTimestampType &type)
{
    CHECK_FUNC(composer_);
    PresentTimestampType hdiType = PresentTimestampType::HARDWARE_DISPLAY_PTS_UNSUPPORTED;
    int32_t ret = composer_->GetSupportedPresentTimestamp(screenId, layerId, hdiType);
    if (ret == GRAPHIC_DISPLAY_SUCCESS) {
        type = static_cast<GraphicPresentTimestampType>(hdiType);
    }
    return ret;
}

int32_t HdiDeviceImpl::GetPresentTimestamp(uint32_t screenId, uint32_t layerId, GraphicPresentTimestamp &timestamp)
{
    CHECK_FUNC(composer_);
    PresentTimestamp hdiTimestamp = {HARDWARE_DISPLAY_PTS_UNSUPPORTED, 0};
    int32_t ret = composer_->GetHwPresentTimestamp(screenId, layerId, hdiTimestamp);
    if (ret == GRAPHIC_DISPLAY_SUCCESS) {
        timestamp.time = hdiTimestamp.time;
        timestamp.type = static_cast<GraphicPresentTimestampType>(hdiTimestamp.type);
    }
    return ret;
}

/* set & get device layer info end */
int32_t HdiDeviceImpl::CreateLayer(uint32_t screenId, const GraphicLayerInfo &layerInfo, uint32_t &layerId)
{
    CHECK_FUNC(composer_);
    LayerInfo hdiLayerInfo = {
        .width = layerInfo.width,
        .height = layerInfo.height,
        .type = static_cast<LayerType>(layerInfo.type),
        .pixFormat = static_cast<PixelFormat>(layerInfo.pixFormat),
    };
    return composer_->CreateLayer(screenId, hdiLayerInfo, layerId);
}

int32_t HdiDeviceImpl::CloseLayer(uint32_t screenId, uint32_t layerId)
{
    CHECK_FUNC(composer_);
    return composer_->DestroyLayer(screenId, layerId);
}

} // namespace Rosen
} // namespace OHOS