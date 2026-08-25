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

#include "rs_rcd_image_loader.h"

#include "common/rs_optional_trace.h"
#include "common/rs_singleton.h"
#include "feature/round_corner_display/rs_rcd_bitmap_utils.h"
#include "rs_trace.h"

namespace OHOS {
namespace Rosen {

std::shared_ptr<RCDImageLoader> RCDImageLoader::Create(rs_rcd::ImageLoadType type,
    bool supportHardware, bool supportTopSurface, bool supportBottomSurface)
{
    switch (type) {
        case rs_rcd::ImageLoadType::ROG_FULLY_LOAD:
            return std::make_shared<RCDImageRogFullyLoader>(supportHardware, supportTopSurface, supportBottomSurface);
        case rs_rcd::ImageLoadType::ROG_PARTIALLY_LOAD:
            return std::make_shared<RCDImageRogPartiallyLoader>(supportHardware, supportTopSurface,
                supportBottomSurface);
        case rs_rcd::ImageLoadType::END:
        default:
            return std::make_shared<RCDImageRogFullyLoader>(supportHardware, supportTopSurface, supportBottomSurface);
    }
}

bool RCDImageRogFullyLoader::GetTopSurfaceSource(rs_rcd::ROGSetting* rog)
{
    RS_OPTIONAL_TRACE_NAME("RoundCornerDisplay::GetTopSurfaceSource");
    if (rog == nullptr || !supportHardware_) {
        RS_LOGE("[%{public}s] Not support or No rog found in config file \n", __func__);
        return false;
    }
    rs_rcd::RCDConfig::PrintParseRog(rog);

    auto portrait = rog->GetPortrait(std::string(rs_rcd::NODE_PORTRAIT));
    if (portrait == std::nullopt) {
        RS_LOGE("[%{public}s] PORTRAIT layerUp do not configured \n", __func__);
        return false;
    }
    std::shared_ptr<Drawing::Image> imgTopPortrait = nullptr;
    std::shared_ptr<Drawing::Image> imgTopHidden = nullptr;
    RCDBitmapUtils::LoadImg(portrait->layerUp.fileName.c_str(), imgTopPortrait);
    // Reuse the decoded image when the resource config is identical to avoid
    // repeated LoadImg and DecodeBitmap.
    RCDBitmapUtils::LoadOrReuseImage(portrait->layerHide, { { portrait->layerUp, imgTopPortrait } }, imgTopHidden);

    auto landscape = rog->GetLandscape(std::string(rs_rcd::NODE_LANDSCAPE));
    if (landscape == std::nullopt) {
        RS_LOGE("[%{public}s] LANDSCAPE layerUp do not configured \n", __func__);
        return false;
    }
    std::shared_ptr<Drawing::Image> imgTopLadsOrit = nullptr;
    RCDBitmapUtils::LoadOrReuseImage(landscape->layerUp,
        { { portrait->layerUp, imgTopPortrait }, { portrait->layerHide, imgTopHidden } }, imgTopLadsOrit);

    bitmapTopPortrait_ = std::make_shared<Drawing::Bitmap>();
    RCDBitmapUtils::DecodeAlphaBitmap(imgTopPortrait, *bitmapTopPortrait_);
    // Reuse the bitmap when the source image is shared to avoid repeated decoding.
    RCDBitmapUtils::DecodeOrReuseBitmap(imgTopLadsOrit, { { imgTopPortrait, bitmapTopPortrait_ } }, bitmapTopLadsOrit_);
    RCDBitmapUtils::DecodeOrReuseBitmap(imgTopHidden,
        { { imgTopPortrait, bitmapTopPortrait_ }, { imgTopLadsOrit, bitmapTopLadsOrit_ } }, bitmapTopHidden_);
    return true;
}

bool RCDImageRogFullyLoader::GetBottomSurfaceSource(rs_rcd::ROGSetting* rog)
{
    RS_OPTIONAL_TRACE_NAME("RoundCornerDisplay::GetBottomSurfaceSource");
    if (rog == nullptr || !supportHardware_) {
        RS_LOGE("[%{public}s] No rog found in config file \n", __func__);
        return false;
    }
    auto portrait = rog->GetPortrait(std::string(rs_rcd::NODE_PORTRAIT));
    if (portrait == std::nullopt) {
        RS_LOGE("[%{public}s] PORTRAIT layerDown do not configured \n", __func__);
        return false;
    }
    std::shared_ptr<Drawing::Image> imgBottomPortrait = nullptr;
    RCDBitmapUtils::LoadImg(portrait->layerDown.fileName.c_str(), imgBottomPortrait);
    bitmapBottomPortrait_ = std::make_shared<Drawing::Bitmap>();
    RCDBitmapUtils::DecodeAlphaBitmap(imgBottomPortrait, *bitmapBottomPortrait_);
    return true;
}

void RCDImageRogFullyLoader::RcdChooseHardwareResource(int type, rs_rcd::ROGSetting* rog,
    rs_rcd::RoundCornerHardware& hardInfo)
{
    RS_OPTIONAL_TRACE_NAME("RCDImageRogFullyLoader::RcdChooseHardwareResource");
    if (rog == nullptr) {
        RS_LOGE_IF(DEBUG_PIPELINE, "[%{public}s] No rog info \n", __func__);
        return;
    }
    if (rog != rog_) {
        GetTopSurfaceSource(rog);
        GetBottomSurfaceSource(rog);
        rog_ = rog;
    }
    auto portrait = rog->GetPortrait(std::string(rs_rcd::NODE_PORTRAIT));
    auto landscape = rog->GetLandscape(std::string(rs_rcd::NODE_LANDSCAPE));
    switch (type) {
        case TOP_PORTRAIT:
            if (portrait == std::nullopt) {
                break;
            }
            hardInfo.topLayer = std::make_shared<rs_rcd::RoundCornerLayer>(portrait->layerUp);
            hardInfo.topLayer->curBitmap = bitmapTopPortrait_.get();
            break;
        case TOP_HIDDEN:
            if (portrait == std::nullopt) {
                break;
            }
            hardInfo.topLayer = std::make_shared<rs_rcd::RoundCornerLayer>(portrait->layerHide);
            hardInfo.topLayer->curBitmap = bitmapTopHidden_.get();
            break;
        case TOP_LADS_ORIT:
            if (landscape == std::nullopt) {
                break;
            }
            hardInfo.topLayer = std::make_shared<rs_rcd::RoundCornerLayer>(landscape->layerUp);
            hardInfo.topLayer->curBitmap = bitmapTopLadsOrit_.get();
            break;
        default:
            RS_LOGE("[%{public}s] No showResourceType found with type %{public}d \n", __func__, type);
            break;
    }
    if (portrait == std::nullopt) {
        return;
    }
    hardInfo.bottomLayer = std::make_shared<rs_rcd::RoundCornerLayer>(portrait->layerDown);
    hardInfo.bottomLayer->curBitmap = bitmapBottomPortrait_.get();
}

void RCDImageRogPartiallyLoader::RcdChooseHardwareResource(int type, rs_rcd::ROGSetting* rog,
    rs_rcd::RoundCornerHardware& hardInfo)
{
    RS_OPTIONAL_TRACE_NAME("RCDImageRogPartiallyLoader::RcdChooseHardwareResource");
    if (rog == nullptr) {
        RS_LOGE_IF(DEBUG_PIPELINE, "[%{public}s] No rog info \n", __func__);
        return;
    }
    rs_rcd::RCDConfig::PrintParseRog(rog);
    auto portrait = rog->GetPortrait(std::string(rs_rcd::NODE_PORTRAIT));
    auto landscape = rog->GetLandscape(std::string(rs_rcd::NODE_LANDSCAPE));
    std::shared_ptr<rs_rcd::RoundCornerLayer> topLayer = nullptr;
    switch (type) {
        case TOP_PORTRAIT:
            if (portrait == std::nullopt) {
                break;
            }
            topLayer = std::make_shared<rs_rcd::RoundCornerLayer>(portrait->layerUp);
            break;
        case TOP_HIDDEN:
            if (portrait == std::nullopt) {
                break;
            }
            topLayer = std::make_shared<rs_rcd::RoundCornerLayer>(portrait->layerHide);
            break;
        case TOP_LADS_ORIT:
            if (landscape == std::nullopt) {
                break;
            }
            topLayer = std::make_shared<rs_rcd::RoundCornerLayer>(landscape->layerUp);
            break;
        default:
            RS_LOGE("[%{public}s] No showResourceType found with type %{public}d \n", __func__, type);
            break;
    }
    if (portrait == std::nullopt) {
        return;
    }
    std::shared_ptr<rs_rcd::RoundCornerLayer> bottomLayer =
        std::make_shared<rs_rcd::RoundCornerLayer>(portrait->layerDown);
    UpdateTopLayer(hardInfo, topLayer);
    UpdateBottomLayer(hardInfo, bottomLayer);
}

void RCDImageRogPartiallyLoader::UpdateTopLayer(rs_rcd::RoundCornerHardware& hardInfo,
    std::shared_ptr<rs_rcd::RoundCornerLayer> layer)
{
    if (!supportTopSurface_ || !supportHardware_ || layer == nullptr) {
        return;
    }
    if (hardInfo.topLayer != nullptr && hardInfo.topLayer->curBitmap &&
        layer->IsResourceEqual(*hardInfo.topLayer)) {
        return;
    }
    RS_OPTIONAL_TRACE_NAME("RCDImageRogPartiallyLoader::UpdateTopLayer");
    bitmapTop_ = RCDBitmapUtils::LoadBitmap(layer->fileName.c_str());
    if (bitmapTop_ == nullptr) {
        RS_LOGE("RCDImageRogPartiallyLoader::UpdateTopLayer bitmap is null \n");
        return;
    }
    hardInfo.topLayer = layer;
    hardInfo.topLayer->curBitmap = bitmapTop_.get();
}

void RCDImageRogPartiallyLoader::UpdateBottomLayer(rs_rcd::RoundCornerHardware& hardInfo,
    std::shared_ptr<rs_rcd::RoundCornerLayer> layer)
{
    if (!supportBottomSurface_ || !supportHardware_ || layer == nullptr) {
        return;
    }
    if (hardInfo.bottomLayer != nullptr && hardInfo.bottomLayer->curBitmap &&
        layer->IsResourceEqual(*hardInfo.bottomLayer)) {
        return;
    }
    RS_OPTIONAL_TRACE_NAME("RCDImageRogPartiallyLoader::UpdateBottomLayer");
    bitmapBottom_ = RCDBitmapUtils::LoadBitmap(layer->fileName.c_str());
    if (bitmapBottom_ == nullptr) {
        RS_LOGE("RCDImageRogPartiallyLoader::UpdateBottomLayer bitmap is null \n");
        return;
    }
    hardInfo.bottomLayer = layer;
    hardInfo.bottomLayer->curBitmap = bitmapBottom_.get();
}
} // namespace Rosen
} // namespace OHOS