/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include "tv_metadata/rs_tv_metadata_util.h"

#include "feature_cfg/feature_param/performance_feature/video_metadata_param.h"
#include "platform/common/rs_log.h"

#undef LOG_TAG
#define LOG_TAG "RSTvMetadataUtil"

using namespace OHOS::HDI::Display::Graphic::Common::V1_0;

namespace OHOS::Rosen {
namespace {
constexpr uint8_t SCALER_MODE_GPU = 3;
}

void RSTvMetadataUtil::CombineMetadata(TvPQMetadata& dstMetadata, const TvPQMetadata& srcMetadata)
{
    srcMetadata.dpPixFmt != 0 ? dstMetadata.dpPixFmt = srcMetadata.dpPixFmt : 0;
    srcMetadata.uiFrameCnt != 0 ? dstMetadata.uiFrameCnt = srcMetadata.uiFrameCnt : 0;
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

void RSTvMetadataUtil::CopyFromLayersToSurface(const std::vector<RSLayerPtr>& layers,
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
                RSTvMetadataUtil::CombineMetadata(tvMetadataCombined, tvMetadata);
            }
        }
    }
    tvMetadataCombined.scaleMode = SCALER_MODE_GPU;
    auto buffer = surface->GetCurrentBuffer();
    if (MetadataHelper::SetVideoTVMetadata(buffer, tvMetadataCombined) != GSERROR_OK) {
        RS_LOGE("CopyFromLayersToSurface failed!");
    }
}

void RSTvMetadataUtil::CombineMetadataForAllLayers(const std::vector<RSLayerPtr>& layers)
{
    TvPQMetadata tvUniRenderMetadata = { 0 };
    TvPQMetadata tvSelfDrawMetadata = { 0 };
    sptr<SurfaceBuffer> tvUniRenderBuffer = nullptr;
    sptr<SurfaceBuffer> tvSelfDrawBuffer = nullptr;

    uint32_t zorderMin = 0;
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
        } else if (tvSelfDrawBuffer == nullptr || zorderMin > layer->GetZorder()) {
            zorderMin = layer->GetZorder();
            tvSelfDrawBuffer = buffer;
            tvSelfDrawMetadata = tvMetadata;
        }
    }
    if (tvSelfDrawBuffer == nullptr) {
        return;
    }
    // update ui info
    tvSelfDrawMetadata.uiFrameCnt = tvUniRenderMetadata.uiFrameCnt;
    tvSelfDrawMetadata.dpPixFmt = tvUniRenderMetadata.dpPixFmt;
    static_cast<void>(MetadataHelper::SetVideoTVMetadata(tvSelfDrawBuffer, tvSelfDrawMetadata));
}
} // namespace OHOS::Rosen
