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

void RSTvMetadataManager::RecordAndCombineMetadata(const TvPQMetadata& metadata)
{
    std::lock_guard<std::mutex> lock(mutex_);
    metadata.sceneTag != 0 ? metadata_.sceneTag = metadata.sceneTag : 0;
    metadata.uiFrameCnt != 0 ? metadata_.uiFrameCnt = metadata.uiFrameCnt : 0;
    metadata.vidFrameCnt != 0 ? metadata_.vidFrameCnt = metadata.vidFrameCnt : 0;
    metadata.vidFps != 0 ? metadata_.vidFps = metadata.vidFps : 0;
    metadata.vidWinX != 0 ? metadata_.vidWinX = metadata.vidWinX : 0;
    metadata.vidWinY != 0 ? metadata_.vidWinY = metadata.vidWinY : 0;
    metadata.vidWinWidth != 0 ? metadata_.vidWinWidth = metadata.vidWinWidth : 0;
    metadata.vidWinHeight != 0 ? metadata_.vidWinHeight = metadata.vidWinHeight : 0;
    metadata.vidWinSize != 0 ? metadata_.vidWinSize = metadata.vidWinSize : 0;
    metadata.vidVdhWidth != 0 ? metadata_.vidVdhWidth = metadata.vidVdhWidth : 0;
    metadata.vidVdhHeight != 0 ? metadata_.vidVdhHeight = metadata.vidVdhHeight : 0;
    metadata.scaleMode != 0 ? metadata_.scaleMode = metadata.scaleMode : 0;
    metadata.dpPixFmt != 0 ? metadata_.dpPixFmt = metadata.dpPixFmt : 0;
    metadata.colorimetry != 0 ? metadata_.colorimetry = metadata.colorimetry : 0;
    metadata.hdr != 0 ? metadata_.hdr = metadata.hdr : 0;
    metadata.hdrLuminance != 0 ? metadata_.hdrLuminance = metadata.hdrLuminance : 0;
    metadata.hdrRatio != 0 ? metadata_.hdrRatio = metadata.hdrRatio : 0;
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

} // namespace OHOS::Rosen
