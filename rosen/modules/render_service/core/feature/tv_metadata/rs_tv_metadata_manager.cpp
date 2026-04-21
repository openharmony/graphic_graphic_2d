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

#include "feature_cfg/feature_param/performance_feature/video_metadata_param.h"
#include "platform/common/rs_log.h"
#include "pipeline/render_thread/rs_uni_render_thread.h"
#include "pipeline/main_thread/rs_main_thread.h"

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
constexpr uint8_t VIDEO_PLAYING_NO_AIHDR_SCENE = 2;
constexpr uint8_t PRIORITY_FOR_FULLSCREEN = 1;
constexpr uint8_t PRIORITY_FOR_SMALLSCREEN = 1;
constexpr uint8_t PRIORITY_FOR_VIDEO_EFFECT = 2;
constexpr uint8_t RESERVED_INDEX_FOR_NODE_ID = 0;
constexpr uint8_t RESERVED_INDEX_FOR_CACHE = 1;
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

void RSTvMetadataManager::RecordAndCombineMetadata(const TvPQMetadata& metadata)
{
    std::lock_guard<std::mutex> lock(mutex_);
    RSTvMetadataUtil::CombineMetadata(metadata_, metadata);
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
    TvPQMetadata* metadata = &metadata_;
    if (CheckCacheValid()) {
        if (HasNoVideo(metadata_)) {
            // if the recorded metadata does not have video info, then use cache
            metadata = &cachedMetadata_;
        } else if (recordedSurfaceNodeId_ != cachedSurfaceNodeId_ &&
            GetPriority(metadata_) <= GetPriority(cachedMetadata_)) {
            // if cached nodeId is different from the recorded nodeId, then use the metadata with the highest priority
            metadata = &cachedMetadata_;
        }
    }
    static uint8_t uiFrameCnt = 0;
    uiFrameCnt++;
    metadata->uiFrameCnt = uiFrameCnt;
    metadata->dpPixFmt = metadata_.dpPixFmt; // use new
    if (MetadataHelper::SetVideoTVMetadata(buffer, *metadata) != GSERROR_OK) {
        RS_LOGE("SetVideoTVMetadata failed!");
    }
    if (metadata != &cachedMetadata_) {
        if (recordedSurfaceNodeId_ > 0) {
            // update cached metadata
            cachedSurfaceNodeId_ = recordedSurfaceNodeId_;
            cachedMetadata_ = metadata_;
            cachedMetadata_.reserved[RESERVED_INDEX_FOR_CACHE] = 1;
        }
    }
    // reset recorded data
    recordedSurfaceNodeId_ = 0;
    (void)memset_s(&metadata_, sizeof(metadata_), 0, sizeof(metadata_));
}

void RSTvMetadataManager::UpdateTvMetadata(const RSSurfaceRenderParams& params, const sptr<SurfaceBuffer>& buffer)
{
    if (buffer == nullptr) {
        return;
    }
    TvPQMetadata info{};
    std::string bundleName = params.GetBundleName();
    if (MetadataHelper::GetVideoTVMetadata(buffer, info) != GSERROR_OK &&
        !IsSdpInfoAppId(bundleName)) {
        return;
    }

    if (info.sceneTag < VIDEO_PLAYING_SCENE) {
        info.sceneTag = VIDEO_PLAYING_NO_AIHDR_SCENE;
    }
    CollectTvMetadata(params, buffer, info);
    info.scaleMode = 0;
    info.reserved[RESERVED_INDEX_FOR_NODE_ID] = static_cast<uint8_t>(params.GetId());
    sptr<SurfaceBuffer> mutableBuffer = buffer;
    static_cast<void>(MetadataHelper::SetVideoTVMetadata(mutableBuffer, info));
}

void RSTvMetadataManager::RecordTvMetadata(const RSSurfaceRenderParams& params, const sptr<SurfaceBuffer>& buffer)
{
    if (buffer == nullptr) {
        return;
    }
    TvPQMetadata info{};
    std::string bundleName = params.GetBundleName();
    if (MetadataHelper::GetVideoTVMetadata(buffer, info) != GSERROR_OK &&
        !IsSdpInfoAppId(bundleName)) {
        return;
    }

    if (info.sceneTag < VIDEO_PLAYING_SCENE) {
        info.sceneTag = VIDEO_PLAYING_NO_AIHDR_SCENE;
    }
    CollectTvMetadata(params, buffer, info);
    info.scaleMode = SCALER_MODE_GPU;
    info.reserved[RESERVED_INDEX_FOR_NODE_ID] = static_cast<uint8_t>(params.GetId());
    std::lock_guard<std::mutex> lock(mutex_);
    if (GetPriority(info) > GetPriority(metadata_)) {
        recordedSurfaceNodeId_ = params.GetId();
        info.dpPixFmt = metadata_.dpPixFmt;
        metadata_ = info;
    }
}

void RSTvMetadataManager::CollectTvMetadata(const RSSurfaceRenderParams& params,
    const sptr<SurfaceBuffer>& buffer, TvPQMetadata& metaData)
{
    if (HasVideo(metaData)) {
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
    if (layerInfo.dstRect.w * WIND_SIZE_DENOMINATOR > screenRect.GetWidth() * WIND_SIZE_NUMERATOR ||
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

bool RSTvMetadataManager::IsSdpInfoAppId(const std::string& bundleName)
{
    const auto& sdpAppMap = VideoMetadataParam::GetVideoMetadataAppMap();
    auto it = sdpAppMap.find(bundleName);
    return (it != sdpAppMap.end());
}

void RSTvMetadataManager::SetUniRenderThreadParam(std::unique_ptr<RSRenderThreadParams>& renderThreadParams)
{
    renderThreadParams->SetCachedNodeId(cachedSurfaceNodeId_);
    const auto& selfDrawingNodes = RSMainThread::Instance()->GetSelfDrawingNodes();
    for (auto& surfaceNode : selfDrawingNodes) {
        if (surfaceNode == nullptr) {
            continue;
        }
        if (surfaceNode->GetId() == cachedSurfaceNodeId_) {
            bool isOnTheTree = surfaceNode->IsOnTheTree();
            NodeId nodeId = surfaceNode->GetId();
            renderThreadParams->SetCachedNodeOnTheTree(isOnTheTree);
            renderThreadParams->SetCachedNodeId(nodeId);
        }
    }
}

uint8_t RSTvMetadataManager::GetPriority(TvPQMetadata& metadata)
{
    uint8_t priority = 0;
    if (HasNoVideo(metadata)) {
        return priority;
    }
    priority += PRIORITY_FOR_SMALLSCREEN;
    if (metadata.vidWinSize == WIND_SIZE_FULLSCREEN) {
        priority += PRIORITY_FOR_FULLSCREEN;
    }
    if (metadata.colorimetry > 0 || metadata.hdr > 0) {
        priority += PRIORITY_FOR_VIDEO_EFFECT;
    }
    return priority;
}

bool RSTvMetadataManager::CheckCacheValid()
{
    if (cachedSurfaceNodeId_ == 0) {
        return false;
    }
    auto& renderThreadParams = RSUniRenderThread::Instance().GetRSRenderThreadParams();
    if (renderThreadParams == nullptr) {
        cachedSurfaceNodeId_ = 0;
        return false;
    }
    if (!renderThreadParams->GetCachedNodeOnTheTree() &&
        renderThreadParams->GetCachedNodeId() == cachedSurfaceNodeId_) {
        cachedSurfaceNodeId_ = 0;
        return false;
    }
    return true;
}
} // namespace OHOS::Rosen
