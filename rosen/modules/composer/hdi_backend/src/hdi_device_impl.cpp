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
#include "hdi_log.h"
#include <cstddef>
#include <cstdlib>
#include <mutex>
#include <scoped_bytrace.h>
#include <valarray>
#include <securec.h>
#include "v1_1/include/idisplay_composer_interface.h"
#include "v1_2/include/idisplay_composer_interface.h"

#define CHECK_FUNC(composerSptr)                                     \
    do {                                                             \
        if ((composerSptr) == nullptr) {                             \
            HLOGD("[%{public}s]composerSptr is nullptr.", __func__); \
            return GRAPHIC_DISPLAY_NULL_PTR;                         \
        }                                                            \
    } while (0)

namespace OHOS {
namespace Rosen {
namespace {
using namespace OHOS::HDI::Display::Composer::V1_2;
using IDisplayComposerInterfaceSptr = sptr<OHOS::HDI::Display::Composer::V1_2::IDisplayComposerInterface>;
static IDisplayComposerInterfaceSptr g_composer;
}

class HwcDeathRecipient : public IRemoteObject::DeathRecipient {
public:
    explicit HwcDeathRecipient(OnHwcDeadCallback callback = nullptr, void *data = nullptr)
        : deathCbFunc_(callback), data_(data) {}
    void OnRemoteDied(const wptr<IRemoteObject> &object) override
    {
        if (deathCbFunc_ != nullptr) {
            HLOGI("%{public}s: notify the death event of composer to RS", __func__);
            deathCbFunc_(data_);
        }
    }
private:
    OnHwcDeadCallback deathCbFunc_;
    void *data_;
};

HdiDeviceImpl::HdiDeviceImpl()
{
}

HdiDeviceImpl::~HdiDeviceImpl()
{
    Destroy();
}

RosenError HdiDeviceImpl::Init()
{
    if (g_composer == nullptr) {
        g_composer = OHOS::HDI::Display::Composer::V1_2::IDisplayComposerInterface::Get();
        if (g_composer == nullptr) {
            HLOGE("IDisplayComposerInterface::Get return nullptr.");
            return ROSEN_ERROR_NOT_INIT;
        }
    }
    return ROSEN_ERROR_OK;
}

void HdiDeviceImpl::Destroy()
{
    g_composer = nullptr;
}

/* set & get device screen info begin */
int32_t HdiDeviceImpl::RegHotPlugCallback(HotPlugCallback callback, void *data)
{
    CHECK_FUNC(g_composer);
    return g_composer->RegHotPlugCallback(callback, data);
}

bool HdiDeviceImpl::RegHwcDeadCallback(OnHwcDeadCallback callback, void *data)
{
    CHECK_FUNC(g_composer);
    sptr<HwcDeathRecipient> recipient = new HwcDeathRecipient(callback, data);
    return g_composer->AddDeathRecipient(recipient);
}

int32_t HdiDeviceImpl::RegScreenVBlankCallback(uint32_t screenId, VBlankCallback callback, void *data)
{
    CHECK_FUNC(g_composer);
    return g_composer->RegDisplayVBlankCallback(screenId, callback, data);
}

int32_t HdiDeviceImpl::SetScreenVsyncEnabled(uint32_t screenId, bool enabled)
{
    CHECK_FUNC(g_composer);
    return g_composer->SetDisplayVsyncEnabled(screenId, enabled);
}

int32_t HdiDeviceImpl::GetScreenCapability(uint32_t screenId, GraphicDisplayCapability &info)
{
    CHECK_FUNC(g_composer);
    DisplayCapability hdiInfo;
    uint32_t propertyId = DisplayPropertyID::DISPLAY_PROPERTY_ID_SKIP_VALIDATE;
    uint64_t propertyValue;
    int32_t ret = g_composer->GetDisplayCapability(screenId, hdiInfo);
    if (ret == GRAPHIC_DISPLAY_SUCCESS) {
        info.name = hdiInfo.name;
        info.type = static_cast<GraphicInterfaceType>(hdiInfo.type);
        info.phyWidth = hdiInfo.phyWidth;
        info.phyHeight = hdiInfo.phyHeight;
        info.supportLayers = hdiInfo.supportLayers;
        info.virtualDispCount = hdiInfo.virtualDispCount;
        info.supportWriteBack = hdiInfo.supportWriteBack;
        info.propertyCount = hdiInfo.propertyCount;
        info.props.clear();
        info.props.reserve(hdiInfo.propertyCount);
        for (uint32_t i = 0; i < hdiInfo.propertyCount; i++) {
            GraphicPropertyObject graphicProperty = {
                .name = hdiInfo.props[i].name,
                .propId = hdiInfo.props[i].propId,
                .value = hdiInfo.props[i].value,
            };
            info.props.emplace_back(graphicProperty);
        }
    }

    ret = g_composer->GetDisplayProperty(screenId, propertyId, propertyValue);
    return ret;
}

int32_t HdiDeviceImpl::GetScreenSupportedModes(uint32_t screenId, std::vector<GraphicDisplayModeInfo> &modes)
{
    CHECK_FUNC(g_composer);
    std::vector<DisplayModeInfo> hdiModes;
    int32_t ret = g_composer->GetDisplaySupportedModes(screenId, hdiModes);
    if (ret != GRAPHIC_DISPLAY_SUCCESS) {
        HLOGE("Get screen supported modes failed, ret is %{public}d.", ret);
        return ret;
    }

    modes.clear();
    modes.reserve(hdiModes.size());
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
    CHECK_FUNC(g_composer);
    return g_composer->GetDisplayMode(screenId, modeId);
}

int32_t HdiDeviceImpl::SetScreenMode(uint32_t screenId, uint32_t modeId)
{
    CHECK_FUNC(g_composer);
    return g_composer->SetDisplayMode(screenId, modeId);
}

int32_t HdiDeviceImpl::SetScreenOverlayResolution(uint32_t screenId, uint32_t width, uint32_t height)
{
    CHECK_FUNC(g_composer);
    return g_composer->SetDisplayOverlayResolution(screenId, width, height);
}

int32_t HdiDeviceImpl::GetScreenPowerStatus(uint32_t screenId, GraphicDispPowerStatus &status)
{
    CHECK_FUNC(g_composer);
    HDI::Display::Composer::V1_0::DispPowerStatus hdiStatus;
    int32_t ret = g_composer->GetDisplayPowerStatus(screenId, hdiStatus);
    if (ret == GRAPHIC_DISPLAY_SUCCESS) {
        status = static_cast<GraphicDispPowerStatus>(hdiStatus);
    }
    return ret;
}

int32_t HdiDeviceImpl::SetScreenPowerStatus(uint32_t screenId, GraphicDispPowerStatus status)
{
    CHECK_FUNC(g_composer);
    return g_composer->SetDisplayPowerStatus(screenId,
        static_cast<HDI::Display::Composer::V1_0::DispPowerStatus>(status));
}

int32_t HdiDeviceImpl::GetScreenBacklight(uint32_t screenId, uint32_t &level)
{
    CHECK_FUNC(g_composer);
    return g_composer->GetDisplayBacklight(screenId, level);
}

int32_t HdiDeviceImpl::SetScreenBacklight(uint32_t screenId, uint32_t level)
{
    CHECK_FUNC(g_composer);
    return g_composer->SetDisplayBacklight(screenId, level);
}

int32_t HdiDeviceImpl::PrepareScreenLayers(uint32_t screenId, bool &needFlush)
{
    CHECK_FUNC(g_composer);
    return g_composer->PrepareDisplayLayers(screenId, needFlush);
}

int32_t HdiDeviceImpl::GetScreenCompChange(uint32_t screenId, std::vector<uint32_t> &layersId,
                                           std::vector<int32_t> &types)
{
    CHECK_FUNC(g_composer);
    int32_t ret = g_composer->GetDisplayCompChange(screenId, layersId, types);
    return ret;
}

int32_t HdiDeviceImpl::SetScreenClientBuffer(uint32_t screenId, const BufferHandle *buffer, uint32_t cacheIndex,
                                             const sptr<SyncFence> &fence)
{
    CHECK_FUNC(g_composer);
    if ((buffer == nullptr && cacheIndex == INVALID_BUFFER_CACHE_INDEX) || fence == nullptr) {
        return GRAPHIC_DISPLAY_PARAM_ERR;
    }

    int32_t fenceFd = fence->Get();
    return g_composer->SetDisplayClientBuffer(screenId, buffer, cacheIndex, fenceFd);
}

int32_t HdiDeviceImpl::SetScreenClientBufferCacheCount(uint32_t screen, uint32_t count)
{
    CHECK_FUNC(g_composer);
    if (count == 0 || count > SURFACE_MAX_QUEUE_SIZE) {
        return GRAPHIC_DISPLAY_PARAM_ERR;
    }
    return g_composer->SetClientBufferCacheCount(screen, count);
}

int32_t HdiDeviceImpl::SetScreenClientDamage(uint32_t screenId, const std::vector<GraphicIRect> &damageRect)
{
    CHECK_FUNC(g_composer);
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
    return g_composer->SetDisplayClientDamage(screenId, hdiDamageRect);
}

int32_t HdiDeviceImpl::GetScreenReleaseFence(uint32_t screenId, std::vector<uint32_t> &layers,
                                             std::vector<sptr<SyncFence>> &fences)
{
    CHECK_FUNC(g_composer);
    std::vector<int32_t> fenceFds;
    int32_t ret = g_composer->GetDisplayReleaseFence(screenId, layers, fenceFds);
    if (ret != GRAPHIC_DISPLAY_SUCCESS || fenceFds.size() == 0) {
        return ret;
    }

    size_t fencesNum = fenceFds.size();
    fences.resize(fencesNum);
    for (size_t i = 0; i < fencesNum; i++) {
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
    CHECK_FUNC(g_composer);
    std::vector<ColorGamut> hdiGamuts;
    int32_t ret = g_composer->GetDisplaySupportedColorGamuts(screenId, hdiGamuts);
    if (ret != GRAPHIC_DISPLAY_SUCCESS) {
        return ret;
    }

    gamuts.clear();
    gamuts.reserve(hdiGamuts.size());
    for (auto iter = hdiGamuts.begin(); iter != hdiGamuts.end(); iter++) {
        GraphicColorGamut tempGamut = static_cast<GraphicColorGamut>(*iter);
        gamuts.emplace_back(tempGamut);
    }
    return ret;
}

int32_t HdiDeviceImpl::SetScreenColorGamut(uint32_t screenId, GraphicColorGamut gamut)
{
    CHECK_FUNC(g_composer);
    return g_composer->SetDisplayColorGamut(screenId, static_cast<ColorGamut>(gamut));
}

int32_t HdiDeviceImpl::GetScreenColorGamut(uint32_t screenId, GraphicColorGamut &gamut)
{
    CHECK_FUNC(g_composer);
    ColorGamut hdiGamut;
    int32_t ret = g_composer->GetDisplayColorGamut(screenId, hdiGamut);
    if (ret == GRAPHIC_DISPLAY_SUCCESS) {
        gamut = static_cast<GraphicColorGamut>(hdiGamut);
    }
    return ret;
}

int32_t HdiDeviceImpl::SetScreenGamutMap(uint32_t screenId, GraphicGamutMap gamutMap)
{
    CHECK_FUNC(g_composer);
    return g_composer->SetDisplayGamutMap(screenId, static_cast<GamutMap>(gamutMap));
}

int32_t HdiDeviceImpl::GetScreenGamutMap(uint32_t screenId, GraphicGamutMap &gamutMap)
{
    CHECK_FUNC(g_composer);
    GamutMap hdiGamutMap;
    int32_t ret = g_composer->GetDisplayGamutMap(screenId, hdiGamutMap);
    if (ret == GRAPHIC_DISPLAY_SUCCESS) {
        gamutMap = static_cast<GraphicGamutMap>(hdiGamutMap);
    }
    return ret;
}

int32_t HdiDeviceImpl::SetScreenColorTransform(uint32_t screenId, const std::vector<float> &matrix)
{
    CHECK_FUNC(g_composer);
    return g_composer->SetDisplayColorTransform(screenId, matrix);
}

int32_t HdiDeviceImpl::GetHDRCapabilityInfos(uint32_t screenId, GraphicHDRCapability &info)
{
    CHECK_FUNC(g_composer);
    HDRCapability hdiInfo;
    int32_t ret = g_composer->GetHDRCapabilityInfos(screenId, hdiInfo);
    if (ret != GRAPHIC_DISPLAY_SUCCESS) {
        return ret;
    }
    uint32_t formatCount = hdiInfo.formatCount;
    info.formats.clear();
    info.formats.reserve(formatCount);
    for (uint32_t i = 0; i < formatCount; i++) {
        GraphicHDRFormat format = static_cast<GraphicHDRFormat>(hdiInfo.formats[i]);
        info.formats.emplace_back(format);
    }
    info.maxLum = hdiInfo.maxLum;
    info.maxAverageLum = hdiInfo.maxAverageLum;
    info.minLum = hdiInfo.minLum;
    return ret;
}

int32_t HdiDeviceImpl::GetSupportedMetaDataKey(uint32_t screenId, std::vector<GraphicHDRMetadataKey> &keys)
{
    CHECK_FUNC(g_composer);
    std::vector<HDRMetadataKey> hdiKeys;
    int32_t ret = g_composer->GetSupportedMetadataKey(screenId, hdiKeys);
    if (ret != GRAPHIC_DISPLAY_SUCCESS) {
        return ret;
    }
    keys.clear();
    keys.reserve(hdiKeys.size());
    for (auto iter = hdiKeys.begin(); iter != hdiKeys.end(); iter++) {
        GraphicHDRMetadataKey tempKey = static_cast<GraphicHDRMetadataKey>(*iter);
        keys.emplace_back(tempKey);
    }
    return ret;
}

int32_t HdiDeviceImpl::Commit(uint32_t screenId, sptr<SyncFence> &fence)
{
    ScopedBytrace bytrace(__func__);
    CHECK_FUNC(g_composer);
    int32_t fenceFd = -1;
    int32_t ret = g_composer->Commit(screenId, fenceFd);

    if (fenceFd >= 0) {
        fence = new SyncFence(fenceFd);
    } else {
        fence = new SyncFence(-1);
    }

    return ret;
}

int32_t HdiDeviceImpl::CommitAndGetReleaseFence(uint32_t screenId, sptr<SyncFence> &fence,
    int32_t &skipState, bool &needFlush, std::vector<uint32_t>& layers, std::vector<sptr<SyncFence>>& fences)
{
    ScopedBytrace bytrace(__func__);
    CHECK_FUNC(g_composer);
    int32_t fenceFd = -1;
    std::vector<int32_t>fenceFds;
    
    int32_t ret = g_composer->CommitAndGetReleaseFence(screenId, fenceFd, skipState, needFlush, layers, fenceFds);

    if (skipState == 0 || fenceFd >= 0) {
        fence = new SyncFence(fenceFd);
    } else {
        fence =new SyncFence(-1);
    }

    size_t fencesNum = fenceFds.size();
    fences.resize(fencesNum);
    for(size_t i = 0; i < fencesNum; i++) {
        if(fenceFds[i] >= 0) {
            fences[i] = new SyncFence(fenceFds[i]);
        } else {
            fences[i] = new SyncFence(-1);
        }

    }
    return ret;
}
/* set & get device screen info end */

/* set & get device layer info begin */
int32_t HdiDeviceImpl::SetLayerAlpha(uint32_t screenId, uint32_t layerId, const GraphicLayerAlpha &alpha)
{
    CHECK_FUNC(g_composer);
    LayerAlpha hdiLayerAlpha = {
        .enGlobalAlpha = alpha.enGlobalAlpha,
        .enPixelAlpha = alpha.enPixelAlpha,
        .alpha0 = alpha.alpha0,
        .alpha1 = alpha.alpha1,
        .gAlpha = alpha.gAlpha,
    };
    return g_composer->SetLayerAlpha(screenId, layerId, hdiLayerAlpha);
}

int32_t HdiDeviceImpl::SetLayerSize(uint32_t screenId, uint32_t layerId, const GraphicIRect &layerRect)
{
    CHECK_FUNC(g_composer);
    IRect hdiLayerRect = {
        .x = layerRect.x,
        .y = layerRect.y,
        .w = layerRect.w,
        .h = layerRect.h,
    };
    return g_composer->SetLayerRegion(screenId, layerId, hdiLayerRect);
}

int32_t HdiDeviceImpl::SetTransformMode(uint32_t screenId, uint32_t layerId, GraphicTransformType type)
{
    CHECK_FUNC(g_composer);
    TransformType hdiType = static_cast<TransformType>(type);
    return g_composer->SetLayerTransformMode(screenId, layerId, hdiType);
}

int32_t HdiDeviceImpl::SetLayerVisibleRegion(uint32_t screenId, uint32_t layerId,
                                             const std::vector<GraphicIRect> &visible)
{
    CHECK_FUNC(g_composer);
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
    return g_composer->SetLayerVisibleRegion(screenId, layerId, hdiVisibleRects);
}

int32_t HdiDeviceImpl::SetLayerDirtyRegion(uint32_t screenId, uint32_t layerId,
                                           const std::vector<GraphicIRect> &dirtyRegions)
{
    CHECK_FUNC(g_composer);
    std::vector<IRect> hdiDirtyRegions;
    for (auto iter = dirtyRegions.begin(); iter != dirtyRegions.end(); iter++) {
        IRect hdiDirtyRect = {
            .x = iter->x,
            .y = iter->y,
            .w = iter->w,
            .h = iter->h,
        };
        hdiDirtyRegions.emplace_back(hdiDirtyRect);
    }
    return g_composer->SetLayerDirtyRegion(screenId, layerId, hdiDirtyRegions);
}

int32_t HdiDeviceImpl::SetLayerBuffer(uint32_t screenId, uint32_t layerId, const GraphicLayerBuffer &layerBuffer)
{
    CHECK_FUNC(g_composer);
    if ((layerBuffer.handle == nullptr && layerBuffer.cacheIndex == INVALID_BUFFER_CACHE_INDEX) ||
        layerBuffer.acquireFence == nullptr) {
        return GRAPHIC_DISPLAY_PARAM_ERR;
    }
    int32_t fenceFd = (layerBuffer.acquireFence)->Get();
    return g_composer->SetLayerBuffer(screenId, layerId, layerBuffer.handle, layerBuffer.cacheIndex,
                                      fenceFd, layerBuffer.deletingList);
}

int32_t HdiDeviceImpl::SetLayerCompositionType(uint32_t screenId, uint32_t layerId, GraphicCompositionType type)
{
    CHECK_FUNC(g_composer);
    HDI::Display::Composer::V1_0::CompositionType hdiType
        = static_cast<HDI::Display::Composer::V1_0::CompositionType>(type);
    return g_composer->SetLayerCompositionType(screenId, layerId, hdiType);
}

int32_t HdiDeviceImpl::SetLayerBlendType(uint32_t screenId, uint32_t layerId, GraphicBlendType type)
{
    CHECK_FUNC(g_composer);
    BlendType hdiBlendType = static_cast<BlendType>(type);
    return g_composer->SetLayerBlendType(screenId, layerId, hdiBlendType);
}

int32_t HdiDeviceImpl::SetLayerCrop(uint32_t screenId, uint32_t layerId, const GraphicIRect &crop)
{
    CHECK_FUNC(g_composer);
    IRect hdiCropRect = {
        .x = crop.x,
        .y = crop.y,
        .w = crop.w,
        .h = crop.h,
    };
    return g_composer->SetLayerCrop(screenId, layerId, hdiCropRect);
}

int32_t HdiDeviceImpl::SetLayerZorder(uint32_t screenId, uint32_t layerId, uint32_t zorder)
{
    CHECK_FUNC(g_composer);
    return g_composer->SetLayerZorder(screenId, layerId, zorder);
}

int32_t HdiDeviceImpl::SetLayerPreMulti(uint32_t screenId, uint32_t layerId, bool isPreMulti)
{
    CHECK_FUNC(g_composer);
    return g_composer->SetLayerPreMulti(screenId, layerId, isPreMulti);
}

int32_t HdiDeviceImpl::SetLayerColor(uint32_t screenId, uint32_t layerId, GraphicLayerColor layerColor)
{
    CHECK_FUNC(g_composer);
    LayerColor color = {
        .r = layerColor.r,
        .g = layerColor.g,
        .b = layerColor.b,
        .a = layerColor.a
    };

    HLOGD("SetLayerColor screenId:%{public}u, layerId:%{public}u", screenId, layerId);

    return g_composer->SetLayerColor(screenId, layerId, color);
}

int32_t HdiDeviceImpl::SetLayerColorTransform(uint32_t screenId, uint32_t layerId, const std::vector<float> &matrix)
{
    CHECK_FUNC(g_composer);
    return g_composer->SetLayerColorTransform(screenId, layerId, matrix);
}

int32_t HdiDeviceImpl::SetLayerColorDataSpace(uint32_t screenId, uint32_t layerId, GraphicColorDataSpace colorSpace)
{
    CHECK_FUNC(g_composer);
    ColorDataSpace hdiColorDataSpace = static_cast<ColorDataSpace>(colorSpace);
    return g_composer->SetLayerColorDataSpace(screenId, layerId, hdiColorDataSpace);
}

int32_t HdiDeviceImpl::GetLayerColorDataSpace(uint32_t screenId, uint32_t layerId, GraphicColorDataSpace &colorSpace)
{
    CHECK_FUNC(g_composer);
    ColorDataSpace hdiColorDataSpace = COLOR_DATA_SPACE_UNKNOWN;
    int32_t ret = g_composer->GetLayerColorDataSpace(screenId, layerId, hdiColorDataSpace);
    if (ret == GRAPHIC_DISPLAY_SUCCESS) {
        colorSpace = static_cast<GraphicColorDataSpace>(hdiColorDataSpace);
    }
    return ret;
}

int32_t HdiDeviceImpl::SetLayerMetaData(uint32_t screenId, uint32_t layerId,
                                        const std::vector<GraphicHDRMetaData> &metaData)
{
    CHECK_FUNC(g_composer);
    std::vector<HDRMetaData> hdiMetaDatas;
    std::size_t metaDataSize = metaData.size();
    for (std::size_t i = 0; i < metaDataSize; i++) {
        HDRMetaData hdiMetaData = {
            .key = static_cast<HDRMetadataKey>(metaData[i].key),
            .value = metaData[i].value,
        };
        hdiMetaDatas.emplace_back(hdiMetaData);
    }
    return g_composer->SetLayerMetaData(screenId, layerId, hdiMetaDatas);
}

int32_t HdiDeviceImpl::SetLayerMetaDataSet(uint32_t screenId, uint32_t layerId, GraphicHDRMetadataKey key,
                                           const std::vector<uint8_t> &metaData)
{
    CHECK_FUNC(g_composer);
    HDRMetadataKey hdiKey = static_cast<HDRMetadataKey>(key);
    return g_composer->SetLayerMetaDataSet(screenId, layerId, hdiKey, metaData);
}

int32_t HdiDeviceImpl::GetSupportedLayerPerFrameParameterKey(std::vector<std::string>& keys)
{
    CHECK_FUNC(g_composer);
    return g_composer->GetSupportedLayerPerFrameParameterKey(keys);
}

int32_t HdiDeviceImpl::SetLayerPerFrameParameter(uint32_t devId, uint32_t layerId, const std::string& key,
                                                 const std::vector<int8_t>& value)
{
    CHECK_FUNC(g_composer);
    return g_composer->SetLayerPerFrameParameter(devId, layerId, key, value);
}

int32_t HdiDeviceImpl::SetLayerTunnelHandle(uint32_t screenId, uint32_t layerId, GraphicExtDataHandle *handle)
{
    CHECK_FUNC(g_composer);
    return g_composer->SetLayerTunnelHandle(screenId, layerId, (*(reinterpret_cast<ExtDataHandle *>(handle))));
}

int32_t HdiDeviceImpl::GetSupportedPresentTimestampType(uint32_t screenId, uint32_t layerId,
                                                        GraphicPresentTimestampType &type)
{
    CHECK_FUNC(g_composer);
    PresentTimestampType hdiType = PresentTimestampType::HARDWARE_DISPLAY_PTS_UNSUPPORTED;
    int32_t ret = g_composer->GetSupportedPresentTimestamp(screenId, layerId, hdiType);
    if (ret == GRAPHIC_DISPLAY_SUCCESS) {
        type = static_cast<GraphicPresentTimestampType>(hdiType);
    }
    return ret;
}

int32_t HdiDeviceImpl::GetPresentTimestamp(uint32_t screenId, uint32_t layerId, GraphicPresentTimestamp &timestamp)
{
    CHECK_FUNC(g_composer);
    PresentTimestamp hdiTimestamp = {HARDWARE_DISPLAY_PTS_UNSUPPORTED, 0};
    int32_t ret = g_composer->GetHwPresentTimestamp(screenId, layerId, hdiTimestamp);
    if (ret == GRAPHIC_DISPLAY_SUCCESS) {
        timestamp.time = hdiTimestamp.time;
        timestamp.type = static_cast<GraphicPresentTimestampType>(hdiTimestamp.type);
    }
    return ret;
}

int32_t HdiDeviceImpl::SetLayerMaskInfo(uint32_t screenId, uint32_t layerId, uint32_t maskInfo)
{
    CHECK_FUNC(g_composer);
    MaskInfo info = static_cast<MaskInfo>(maskInfo);
    return g_composer->SetLayerMaskInfo(screenId, layerId, info);
}

/* set & get device layer info end */
int32_t HdiDeviceImpl::CreateLayer(uint32_t screenId, const GraphicLayerInfo &layerInfo, uint32_t cacheCount,
                                   uint32_t &layerId)
{
    CHECK_FUNC(g_composer);
    LayerInfo hdiLayerInfo = {
        .width = layerInfo.width,
        .height = layerInfo.height,
        .type = static_cast<LayerType>(layerInfo.type),
        .pixFormat = static_cast<HDI::Display::Composer::V1_0::PixelFormat>(layerInfo.pixFormat),
    };
    return g_composer->CreateLayer(screenId, hdiLayerInfo, cacheCount, layerId);
}

int32_t HdiDeviceImpl::CloseLayer(uint32_t screenId, uint32_t layerId)
{
    CHECK_FUNC(g_composer);
    return g_composer->DestroyLayer(screenId, layerId);
}

} // namespace Rosen
} // namespace OHOS
