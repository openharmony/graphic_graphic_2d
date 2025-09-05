/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "rs_tv_metadata_manager.h"
#include "platform/common/rs_log.h"

#undef LOG_TAG
#define LOG_TAG "RSTvMetadataManager"

using namespace OHOS::HDI::Display::Graphic::Common::V1_0;

namespace OHOS::Rosen {
namespace {
constexpr uint32_t WIND_SIZE_NUMERATOR = 2;
constexpr uint32_t WIND_SIZE_DENOMINATOR = 3;
constexpr uint8_t WIND_SIZE_FULLSCREEN = 2;
constexpr uint8_t WIND_SIZE_SMALLSCREEN = 1;
constexpr uint8_t SCALER_MODE_GPU = 3;
constexpr uint8_t VIDEO_PLAYING_SCENE = 1;
}

enum TvColorPrimaries {
    TVCOLOR_PRIMARIES_BT709 = 1,
    TVCOLOR_PRIMARIES_BT601_P,
    TVCOLOR_PRIMARIES_BT601_N,
    TVCOLOR_PRIMARIES_BT2020,
    TVCOLOR_PRIMARIES_P3_DCI,
    TVCOLOR_PRIMARIES_P3_D65,
    TVCOLOR_PRIMARIES_MONO,
    TVCOLOR_PRIMARIES_ADOBERGB
};

RSTvMetadataManager& RSTvMetadataManager::Instance()
{
    static RSTvMetadataManager instance;
    return instance;
}

void RSTvMetadataManager::CombineMetadata(TvPQMetadata& dstMetadata, const TvPQMetadata& srcMetadata)
{
    srcMetadata.dpPixFmt != 0 ? dstMetadata.dpPixFmt = srcMetadata.dpPixFmt : 0;
    srcMetadata.uiFrameCnt != 0 ? dstMetadata.uiFrameCnt = srcMetadata.uiFrameCnt : 0;
    if (dstMetadata.vidWinSize == WIND_SIZE_FULLSCREEN && srcMetadata.vidWinSize != WIND_SIZE_FULLSCREEN) {
        return;
    }
    srcMetadata.sceneTag != 0 ? dstMetadata.sceneTag = srcMetadata.sceneTag : 0;
    srcMetadata.vidFrameCnt != 0 ? dstMetadata.vidFrameCnt = srcMetadata.vidFrameCnt : 0;
    srcMetadata.vidFps != 0 ? dstMetadata.vidFps = srcMetadata.vidFps : 0;
    srcMetadata.vidWinX != 0 ? dstMetadata.vidWinX = srcMetadata.vidWinX : 0;
    srcMetadata.vidWinY != 0 ? dstMetadata.vidWinY = srcMetadata.vidWinY : 0;
    srcMetadata.vidWinWidth != 0 ? dstMetadata.vidWinWidth = srcMetadata.vidWinWidth : 0;
    srcMetadata.vidWinHeight != 0 ? dstMetadata.vidWinHeight = srcMetadata.vidWinHeight : 0;
    srcMetadata.vidWinSize != 0 ? dstMetadata.vidWinSize = srcMetadata.vidWinSize : 0;
    srcMetadata.vidVdhWidth != 0 ? dstMetadata.vidVdhWidth = srcMetadata.vidVdhWidth : 0;
    srcMetadata.vidVdhHeight != 0 ? dstMetadata.vidVdhHeight = srcMetadata.vidVdhHeight : 0;
    srcMetadata.scaleMode != 0 ? dstMetadata.scaleMode = srcMetadata.scaleMode : 0;
    srcMetadata.colorimetry != 0 ? dstMetadata.colorimetry = srcMetadata.colorimetry : 0;
    srcMetadata.hdr != 0 ? dstMetadata.hdr = srcMetadata.hdr : 0;
    srcMetadata.hdrLuminance != 0 ? dstMetadata.hdrLuminance = srcMetadata.hdrLuminance : 0;
    srcMetadata.hdrRatio != 0 ? dstMetadata.hdrRatio = srcMetadata.hdrRatio : 0;
}

void RSTvMetadataManager::RecordAndCombineMetadata(const TvPQMetadata& metadata)
{
    std::lock_guard<std::mutex> lock(mutex_);
    CombineMetadata(metadata_, metadata);
}

void RSTvMetadataManager::Reset()
{
    std::lock_guard<std::mutex> lock(mutex_);
    (void)memset_s(&metadata_, sizeof(metadata_), 0, sizeof(metadata_));
}

TvPQMetadata RSTvMetadataManager::GetMetadata() const
{
    std::lock_guard<std::mutex> lock(mutex_);
    return metadata_;
}

void RSTvMetadataManager::ResetDpPixelFormat()
{
    std::lock_guard<std::mutex> lock(mutex_);
    metadata_.dpPixFmt = 0;
}

void RSTvMetadataManager::CopyTvMetadataToSurface(std::shared_ptr<RSSurfaceOhos>& surface)
{
    if (surface == nullptr) {
        return;
    }
    auto buffer = surface->GetCurrentBuffer();
    std::lock_guard<std::mutex> lock(mutex_);
    static uint8_t uiFrameCnt = 0;
    uiFrameCnt++;
    metadata_.uiFrameCnt = uiFrameCnt;
    if (MetadataHelper::SetVideoTVMetadata(buffer, metadata_) != GSERROR_OK) {
        RS_LOGE("SetVideoTVMetadata failed!");
    }
    (void)memset_s(&metadata_, sizeof(metadata_), 0, sizeof(metadata_));
}

void RSTvMetadataManager::CopyFromLayersToSurface(const std::vector<LayerInfoPtr>& layers,
    std::shared_ptr<RSSurfaceOhos>& surface)
{
    if (surface == nullptr || surface->GetCurrentBuffer() == nullptr) {
        return;
    }
    TvPQMetadata tvMetadataCombined = { 0 };
    for (const auto& layer : layers) {
        if (layer && layer->GetBuffer()) {
            auto buffer = layer->GetBuffer();
            TvPQMetadata tvMetadata = { 0 };
            if (MetadataHelper::GetVideoTVMetadata(buffer, tvMetadata) == GSERROR_OK) {
                CombineMetadata(tvMetadataCombined, tvMetadata);
            }
        }
    }
    tvMetadataCombined.scaleMode = SCALER_MODE_GPU;
    auto buffer = surface->GetCurrentBuffer();
    if (MetadataHelper::SetVideoTVMetadata(buffer, tvMetadataCombined) != GSERROR_OK) {
        RS_LOGE("CopyFromLayersToSurface failed!");
    }
}

void RSTvMetadataManager::ClearVideoMetadata(TvPQMetadata& metadata)
{
    metadata.sceneTag = 0;
    metadata.vidFrameCnt = 0;
    metadata.vidFps = 0;
    metadata.vidWinX = 0;
    metadata.vidWinY = 0;
    metadata.vidWinWidth = 0;
    metadata.vidWinHeight = 0;
    metadata.vidWinSize = 0;
    metadata.vidVdhWidth = 0;
    metadata.vidVdhHeight = 0;
    metadata.scaleMode = 0;
    metadata.colorimetry = 0;
    metadata.hdr = 0;
    metadata.hdrLuminance = 0;
    metadata.hdrRatio = 0;
}

void RSTvMetadataManager::CombineMetadataForAllLayers(const std::vector<LayerInfoPtr>& layers)
{
    TvPQMetadata tvUniRenderMetadata = { 0 };
    TvPQMetadata tvSelfDrawMetadata = { 0 };
    static TvPQMetadata preUniRenderTvMetadata = { 0 };
    sptr<SurfaceBuffer> tvUniRenderBuffer = nullptr;
    sptr<SurfaceBuffer> tvSelfDrawBuffer = nullptr;
 
    for (const auto& layer : layers) {
        if (layer == nullptr) {
            continue;
        }
        auto buffer = layer->GetBuffer();
        if (buffer == nullptr) {
            continue;
        }
        TvPQMetadata tvMetadata = { 0 };
        if (MetadataHelper::GetVideoTVMetadata(buffer, tvMetadata) != GSERROR_OK) {
            continue;
        }
        if (layer->GetUniRenderFlag()) {
            tvUniRenderBuffer = buffer;
            tvUniRenderMetadata = tvMetadata;
            preUniRenderTvMetadata = tvMetadata;
        } else {
            if (tvSelfDrawBuffer) {
                static_cast<void>(MetadataHelper::EraseVideoTVInfoKey(tvSelfDrawBuffer));
            }
            tvSelfDrawBuffer = buffer;
            tvSelfDrawMetadata = tvMetadata;
        }
    }
    if (tvSelfDrawBuffer == nullptr) {
        return;
    }
    if (tvUniRenderBuffer == nullptr) {
        tvUniRenderMetadata =  preUniRenderTvMetadata;
    } else {
        static_cast<void>(MetadataHelper::EraseVideoTVInfoKey(tvUniRenderBuffer));
    }
    ClearVideoMetadata(tvUniRenderMetadata);
    CombineMetadata(tvSelfDrawMetadata, tvUniRenderMetadata);
    static_cast<void>(MetadataHelper::SetVideoTVMetadata(tvSelfDrawBuffer, tvSelfDrawMetadata));
}

void RSTvMetadataManager::UpdateTvMetadata(const RSSurfaceRenderParams& params, const sptr<SurfaceBuffer>& buffer)
{
    if (buffer == nullptr) {
        return;
    }
    TvPQMetadata info{};
    if (MetadataHelper::GetVideoTVMetadata(buffer, info) != GSERROR_OK) {
        return;
    }
    CollectTvMetadata(params, buffer, info);
    info.scaleMode = 0;
    static_cast<void>(MetadataHelper::SetVideoTVMetadata(const_cast<sptr<SurfaceBuffer>&>(buffer), info));
}

void RSTvMetadataManager::RecordTvMetadata(const RSSurfaceRenderParams& params, const sptr<SurfaceBuffer>& buffer)
{
    if (buffer == nullptr) {
        return;
    }
    TvPQMetadata info{};
    if (MetadataHelper::GetVideoTVMetadata(buffer, info) != GSERROR_OK) {
        return;
    }
    CollectTvMetadata(params, buffer, info);
    info.scaleMode = SCALER_MODE_GPU;
    RecordAndCombineMetadata(info);
}

void RSTvMetadataManager::CollectTvMetadata(const RSSurfaceRenderParams& params,
    const sptr<SurfaceBuffer>& buffer, TvPQMetadata& metaData)
{
    if (metaData.sceneTag >= VIDEO_PLAYING_SCENE) {
        CollectSurfaceSize(params, metaData);
        CollectColorPrimaries(buffer, metaData);
        CollectHdrType(buffer, metaData);
    }
}

void RSTvMetadataManager::CollectSurfaceSize(const RSSurfaceRenderParams& surfaceNodeParams, TvPQMetadata& metaData)
{
    auto screenRect = surfaceNodeParams.GetScreenRect();
    auto& layerInfo = surfaceNodeParams.GetLayerInfo();
    metaData.vidWinX = static_cast<uint16_t>(layerInfo.dstRect.x);
    metaData.vidWinY = static_cast<uint16_t>(layerInfo.dstRect.y);
    metaData.vidWinWidth = static_cast<uint16_t>(layerInfo.dstRect.w);
    metaData.vidWinHeight = static_cast<uint16_t>(layerInfo.dstRect.h);
    metaData.vidVdhWidth = static_cast<uint16_t>(layerInfo.srcRect.w);
    metaData.vidVdhHeight = static_cast<uint16_t>(layerInfo.srcRect.h);
    if (layerInfo.dstRect.w * WIND_SIZE_DENOMINATOR > screenRect.GetWidth() * WIND_SIZE_NUMERATOR &&
        layerInfo.dstRect.h * WIND_SIZE_DENOMINATOR > screenRect.GetHeight() * WIND_SIZE_NUMERATOR) {
        metaData.vidWinSize = WIND_SIZE_FULLSCREEN;
    } else {
        metaData.vidWinSize = WIND_SIZE_SMALLSCREEN;
    }
}

void RSTvMetadataManager::CollectColorPrimaries(const sptr<SurfaceBuffer>& buffer, TvPQMetadata& metaData)
{
    CM_ColorSpaceInfo colorSpaceInfo{};
    if (MetadataHelper::GetColorSpaceInfo(buffer, colorSpaceInfo) == GSERROR_OK) {
        static const std::unordered_map<CM_ColorPrimaries, TvColorPrimaries> colorPrimariesMap {
            {COLORPRIMARIES_BT709, TVCOLOR_PRIMARIES_BT709},
            {COLORPRIMARIES_BT601_P, TVCOLOR_PRIMARIES_BT601_P},
            {COLORPRIMARIES_BT601_N, TVCOLOR_PRIMARIES_BT601_N},
            {COLORPRIMARIES_BT2020, TVCOLOR_PRIMARIES_BT2020},
            {COLORPRIMARIES_P3_DCI, TVCOLOR_PRIMARIES_P3_DCI},
            {COLORPRIMARIES_P3_D65, TVCOLOR_PRIMARIES_P3_D65},
            {COLORPRIMARIES_MONO, TVCOLOR_PRIMARIES_MONO},
            {COLORPRIMARIES_ADOBERGB, TVCOLOR_PRIMARIES_ADOBERGB}
        };
        if (colorPrimariesMap.find(colorSpaceInfo.primaries) == colorPrimariesMap.end()) {
            metaData.colorimetry = 0;
        } else {
            metaData.colorimetry = colorPrimariesMap.at(colorSpaceInfo.primaries);
        }
    }
}

void RSTvMetadataManager::CollectHdrType(const sptr<SurfaceBuffer>& buffer, TvPQMetadata& metaData)
{
    CM_HDR_Metadata_Type hdrMetadataType{};
    if (MetadataHelper::GetHDRMetadataType(buffer, hdrMetadataType) == GSERROR_OK) {
        metaData.hdr = hdrMetadataType;
    }
}
} // namespace OHOS::Rosen
