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

namespace OHOS::Rosen {

RSTvMetadataManager& RSTvMetadataManager::Instance()
{
    static RSTvMetadataManager instance;
    return instance;
}

void RSTvMetadataManager::CombineMetadata(TvPQMetadata& dstMetadata, const TvPQMetadata& srcMetadata)
{
    srcMetadata.sceneTag != 0 ? dstMetadata.sceneTag = srcMetadata.sceneTag : 0;
    srcMetadata.uiFrameCnt != 0 ? dstMetadata.uiFrameCnt = srcMetadata.uiFrameCnt : 0;
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
    srcMetadata.dpPixFmt != 0 ? dstMetadata.dpPixFmt = srcMetadata.dpPixFmt : 0;
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
    if (MetadataHelper::SetVideoTVMetadata(buffer, metadata_) != GSERROR_OK) {
        RS_LOGE("RSTvMetadataManager SetVideoTVMetadata failed!");
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
    for (const auto &layer : layers) {
        if (layer && layer->GetBuffer()) {
            auto buffer = layer->GetBuffer();
            TvPQMetadata tvMetadata = { 0 };
            if (MetadataHelper::GetVideoTVMetadata(buffer, tvMetadata) == GSERROR_OK) {
                CombineMetadata(tvMetadataCombined, tvMetadata);
            }
        }
    }

    auto buffer = surface->GetCurrentBuffer();
    if (MetadataHelper::SetVideoTVMetadata(buffer, tvMetadataCombined) != GSERROR_OK) {
        RS_LOGE("RSTvMetadataManager CopyFromLayersToSurface failed!");
    }
}
} // namespace OHOS::Rosen
