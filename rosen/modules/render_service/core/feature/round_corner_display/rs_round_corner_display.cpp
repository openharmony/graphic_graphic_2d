/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "rs_round_corner_display.h"

#include "common/rs_optional_trace.h"
#include "common/rs_singleton.h"
#include "platform/common/rs_system_properties.h"
#include "feature/round_corner_display/rs_message_bus.h"
#include "rs_trace.h"

namespace OHOS {
namespace Rosen {
RoundCornerDisplay::RoundCornerDisplay(NodeId id) : renderTargetId_{id}
{
    RS_LOGD_IF(DEBUG_PIPELINE, "[%{public}s] Created with render target %{public}" PRIu64 " \n", __func__,
        renderTargetId_);
}

RoundCornerDisplay::~RoundCornerDisplay()
{
    RS_LOGD_IF(DEBUG_PIPELINE, "[%{public}s] Destroy for render target %{public}" PRIu64 " \n", __func__,
        renderTargetId_);
}

bool RoundCornerDisplay::Init()
{
    std::unique_lock<std::shared_mutex> lock(resourceMut_);
    LoadConfigFile();
    SeletedLcdModel(rs_rcd::ATTR_DEFAULT);
    LoadImgsbyResolution(displayRect_.GetWidth(), displayRect_.GetHeight());
    RS_LOGI("[%{public}s] RoundCornerDisplay init \n", __func__);
    return true;
}

void RoundCornerDisplay::InitOnce()
{
    if (!isInit) {
        Init();
        isInit = true;
    }
}

bool RoundCornerDisplay::SeletedLcdModel(const char* lcdModelName)
{
    auto& rcdCfg = RSSingleton<rs_rcd::RCDConfig>::GetInstance();
    lcdModel_ = rcdCfg.GetLcdModel(std::string(lcdModelName));
    if (lcdModel_ == nullptr) {
        RS_LOGD_IF(DEBUG_PIPELINE, "[%{public}s] No lcdModel found in config file with name %{public}s \n", __func__,
            lcdModelName);
        return false;
    }
    supportTopSurface_ = lcdModel_->surfaceConfig.topSurface.support;
    supportBottomSurface_ = lcdModel_->surfaceConfig.bottomSurface.support;
    supportHardware_ = lcdModel_->hardwareConfig.hardwareComposer.support;
    RS_LOGI("[%{public}s] Selected model: %{public}s, supported: top->%{public}d, bottom->%{public}d,"
        "hardware->%{public}d \n", __func__, lcdModelName, static_cast<int>(supportTopSurface_),
        static_cast<int>(supportBottomSurface_), static_cast<int>(supportHardware_));
    return true;
}

bool RoundCornerDisplay::LoadConfigFile()
{
    RS_LOGD_IF(DEBUG_PIPELINE, "[%{public}s] LoadConfigFile \n", __func__);
    auto& rcdCfg = RSSingleton<rs_rcd::RCDConfig>::GetInstance();
    return rcdCfg.Load(std::string(rs_rcd::PATH_CONFIG_FILE));
}

bool RoundCornerDisplay::LoadImg(const char* path, std::shared_ptr<Drawing::Image>& img)
{
    std::string filePath = std::string(rs_rcd::PATH_CONFIG_DIR) + "/" + path;
    RS_LOGD_IF(DEBUG_PIPELINE, "[%{public}s] Read Img(%{public}s) \n", __func__, filePath.c_str());
    std::shared_ptr<Drawing::Data> drData = Drawing::Data::MakeFromFileName(filePath.c_str());
    if (drData == nullptr) {
        RS_LOGE("[%{public}s] Open picture file failed! \n", __func__);
        return false;
    }
    img = std::make_shared<Drawing::Image>();
    if (!img->MakeFromEncoded(drData)) {
        img = nullptr;
        RS_LOGE("[%{public}s] Decode picture file failed! \n", __func__);
        return false;
    }
    return true;
}

bool RoundCornerDisplay::DecodeBitmap(std::shared_ptr<Drawing::Image> image, Drawing::Bitmap &bitmap)
{
    if (image == nullptr) {
        RS_LOGE("[%{public}s] No image found \n", __func__);
        return false;
    }
    if (!image->AsLegacyBitmap(bitmap)) {
        RS_LOGE("[%{public}s] Create bitmap from drImage failed \n", __func__);
        return false;
    }
    return true;
}

bool RoundCornerDisplay::SetHardwareLayerSize()
{
    if (hardInfo_.topLayer == nullptr) {
        RS_LOGE("[%{public}s] No topLayer found in hardInfo \n", __func__);
        return false;
    }
    if (hardInfo_.bottomLayer == nullptr) {
        RS_LOGE("[%{public}s] No bottomLayer found in hardInfo \n", __func__);
        return false;
    }
    hardInfo_.displayRect = displayRect_;
    return true;
}

bool RoundCornerDisplay::GetTopSurfaceSource()
{
    RS_TRACE_NAME("RoundCornerDisplay::GetTopSurfaceSource");
    if (rog_ == nullptr) {
        RS_LOGE("[%{public}s] No rog found in config file \n", __func__);
        return false;
    }
    rs_rcd::RCDConfig::PrintParseRog(rog_);

    auto portrait = rog_->GetPortrait(std::string(rs_rcd::NODE_PORTRAIT));
    if (portrait == std::nullopt) {
        RS_LOGE("[%{public}s] PORTRAIT layerUp do not configured \n", __func__);
        return false;
    }
    LoadImg(portrait->layerUp.fileName.c_str(), imgTopPortrait_);
    LoadImg(portrait->layerHide.fileName.c_str(), imgTopHidden_);

    auto landscape = rog_->GetLandscape(std::string(rs_rcd::NODE_LANDSCAPE));
    if (landscape == std::nullopt) {
        RS_LOGE("[%{public}s] LANDSACPE layerUp do not configured \n", __func__);
        return false;
    }
    LoadImg(landscape->layerUp.fileName.c_str(), imgTopLadsOrit_);

    if (supportHardware_) {
        DecodeBitmap(imgTopPortrait_, bitmapTopPortrait_);
        DecodeBitmap(imgTopLadsOrit_, bitmapTopLadsOrit_);
        DecodeBitmap(imgTopHidden_, bitmapTopHidden_);
    }
    return true;
}

bool RoundCornerDisplay::GetBottomSurfaceSource()
{
    RS_TRACE_NAME("RoundCornerDisplay::GetBottomSurfaceSource");
    if (rog_ == nullptr) {
        RS_LOGE("[%{public}s] No rog found in config file \n", __func__);
        return false;
    }
    auto portrait = rog_->GetPortrait(std::string(rs_rcd::NODE_PORTRAIT));
    if (portrait == std::nullopt) {
        RS_LOGE("[%{public}s] PORTRAIT layerDown do not configured \n", __func__);
        return false;
    }
    LoadImg(portrait->layerDown.fileName.c_str(), imgBottomPortrait_);
    if (supportHardware_) {
        DecodeBitmap(imgBottomPortrait_, bitmapBottomPortrait_);
    }
    return true;
}

bool RoundCornerDisplay::HandleTopRcdDirty(RectI& dirtyRect)
{
    if ((static_cast<uint8_t>(rcdDirtyType_) & static_cast<uint8_t>(RoundCornerDirtyType::RCD_DIRTY_TOP)) !=
        static_cast<uint8_t>(RoundCornerDirtyType::RCD_DIRTY_TOP)) {
        return false;
    }
    if (curTop_ != nullptr) {
        dirtyRect = dirtyRect.JoinRect(RectI(displayRect_.GetLeft(), displayRect_.GetTop(),
            curTop_->GetWidth(), curTop_->GetHeight()));
    }
    std::shared_lock<std::shared_mutex> lock(resourceMut_);
    if (!hardInfo_.resourceChanged) {
        rcdDirtyType_ = static_cast<RoundCornerDirtyType>(
            (~static_cast<uint8_t>(RoundCornerDirtyType::RCD_DIRTY_TOP)) & static_cast<uint8_t>(rcdDirtyType_));
    }
    return true;
}

bool RoundCornerDisplay::HandleBottomRcdDirty(RectI& dirtyRect)
{
    if ((static_cast<uint8_t>(rcdDirtyType_) & static_cast<uint8_t>(RoundCornerDirtyType::RCD_DIRTY_BOTTOM)) !=
        static_cast<uint8_t>(RoundCornerDirtyType::RCD_DIRTY_BOTTOM)) {
        return false;
    }
    if (curBottom_ != nullptr) {
        dirtyRect = dirtyRect.JoinRect(
            RectI(displayRect_.GetLeft(), displayRect_.GetHeight() - curBottom_->GetHeight() + displayRect_.GetTop(),
            curBottom_->GetWidth(), curBottom_->GetHeight()));
    }
    std::shared_lock<std::shared_mutex> lock(resourceMut_);
    if (!hardInfo_.resourceChanged) {
        rcdDirtyType_ = static_cast<RoundCornerDirtyType>(
            (~static_cast<uint8_t>(RoundCornerDirtyType::RCD_DIRTY_BOTTOM)) & static_cast<uint8_t>(rcdDirtyType_));
    }
    return true;
}

bool RoundCornerDisplay::LoadImgsbyResolution(uint32_t width, uint32_t height)
{
    RS_TRACE_NAME("RoundCornerDisplay::LoadImgsbyResolution");

    if (lcdModel_ == nullptr) {
        RS_LOGD_IF(DEBUG_PIPELINE, "[%{public}s] No lcdModel selected in config file \n", __func__);
        return false;
    }
    auto rog = lcdModel_->GetRog(width, height);
    if (rog == nullptr) {
        RS_LOGE_IF(DEBUG_PIPELINE, "[%{public}s] Can't find resolution (%{public}u x %{public}u) in config file \n",
            __func__, width, height);
        return false;
    }
    rog_ = rog;
    RS_LOGD_IF(DEBUG_PIPELINE, "[%{public}s] Get rog resolution (%{public}u x %{public}u) in config file \n", __func__,
        width, height);
    if (supportTopSurface_ && supportHardware_) {
        if (!GetTopSurfaceSource()) {
            RS_LOGE("[%{public}s] Top surface support configured, but resources is missing! \n", __func__);
            return false;
        }
    }
    if (supportBottomSurface_ && supportHardware_) {
        if (!GetBottomSurfaceSource()) {
            RS_LOGE("[%{public}s] Bottom surface support configured, but resources is missing! \n", __func__);
            return false;
        }
    }
    return true;
}

void RoundCornerDisplay::UpdateDisplayParameter(uint32_t left, uint32_t top, uint32_t width, uint32_t height)
{
    std::unique_lock<std::shared_mutex> lock(resourceMut_);
    RectU displayRect(left, top, width, height);
    if (displayRect != displayRect_) {
        RS_LOGI("[%{public}s] rcd last rect %{public}u, %{public}u, %{public}u, %{public}u"
            "rcv rect %{public}u, %{public}u, %{public}u, %{public}u \n", __func__,
            lastRcvDisplayRect_.GetLeft(), lastRcvDisplayRect_.GetTop(),
            lastRcvDisplayRect_.GetWidth(), lastRcvDisplayRect_.GetHeight(),
            displayRect.GetLeft(), displayRect.GetTop(), displayRect.GetWidth(), displayRect.GetHeight());
        lastRcvDisplayRect_ = displayRect;
        PrintRCDInfo();
    } else {
        RS_LOGD_IF(DEBUG_PIPELINE, "[%{public}s] DisplayParameter do not change \n", __func__);
        return;
    }
    if (LoadImgsbyResolution(displayRect.GetWidth(), displayRect.GetHeight())) {
        rcdDirtyType_ = static_cast<RoundCornerDirtyType>(
            static_cast<uint8_t>(RoundCornerDirtyType::RCD_DIRTY_ALL) | static_cast<uint8_t>(rcdDirtyType_));
        updateFlag_["display"] = true;
        displayRect_ = displayRect;
    }
}

void RoundCornerDisplay::UpdateNotchStatus(int status)
{
    std::unique_lock<std::shared_mutex> lock(resourceMut_);
    // Update surface when surface status changed
    if (status < 0 || status > 1) {
        RS_LOGE("[%{public}s] notchStatus won't be over 1 or below 0 \n", __func__);
        return;
    }
    if (notchStatus_ == status) {
        RS_LOGD_IF(DEBUG_PIPELINE, "[%{public}s] NotchStatus do not change \n", __func__);
        return;
    }
    RS_LOGI("[%{public}s] rcd notchStatus change from %{public}d to %{public}d \n", __func__,
        notchStatus_, status);
    PrintRCDInfo();
    notchStatus_ = status;
    rcdDirtyType_ = static_cast<RoundCornerDirtyType>(
        static_cast<uint8_t>(RoundCornerDirtyType::RCD_DIRTY_TOP) | static_cast<uint8_t>(rcdDirtyType_));
    updateFlag_["notch"] = true;
}

void RoundCornerDisplay::UpdateOrientationStatus(ScreenRotation orientation)
{
    std::unique_lock<std::shared_mutex> lock(resourceMut_);
    if (orientation == curOrientation_) {
        RS_LOGD_IF(DEBUG_PIPELINE, "[%{public}s] OrientationStatus do not change \n", __func__);
        return;
    }
    RS_LOGI("[%{public}s] rcd Orientation change from %{public}d to %{public}d \n", __func__,
        curOrientation_, orientation);
    PrintRCDInfo();
    lastOrientation_ = curOrientation_;
    curOrientation_ = orientation;
    rcdDirtyType_ = static_cast<RoundCornerDirtyType>(
        static_cast<uint8_t>(RoundCornerDirtyType::RCD_DIRTY_TOP) | static_cast<uint8_t>(rcdDirtyType_));
    updateFlag_["orientation"] = true;
}

void RoundCornerDisplay::UpdateHardwareResourcePrepared(bool prepared)
{
    std::unique_lock<std::shared_mutex> lock(resourceMut_);
    if (hardInfo_.resourcePreparing) {
        hardInfo_.resourcePreparing = false;
        hardInfo_.resourceChanged = !prepared;
    }
}

void RoundCornerDisplay::UpdateParameter(std::map<std::string, bool>& updateFlag)
{
    std::unique_lock<std::shared_mutex> lock(resourceMut_);
    for (auto item = updateFlag.begin(); item != updateFlag.end(); item++) {
        if (item->second == true) {
            resourceChanged = true;
            item->second = false; // reset
        }
    }
    if (resourceChanged) {
        RcdChooseTopResourceType();
        RcdChooseRSResource();
        if (supportHardware_) {
            RcdChooseHardwareResource();
            SetHardwareLayerSize();
        }
        hardInfo_.resourceChanged = resourceChanged; // output
        hardInfo_.resourcePreparing = false; // output
        resourceChanged = false; // reset
    } else {
        RS_LOGD_IF(DEBUG_PIPELINE, "[%{public}s] Status is not changed \n", __func__);
    }
}

// Choose the approriate resource type according to orientation and notch status
void RoundCornerDisplay::RcdChooseTopResourceType()
{
    RS_LOGD_IF(DEBUG_PIPELINE, "[%{public}s] Choose surface \n", __func__);
    RS_LOGD_IF(DEBUG_PIPELINE, "[%{public}s] curOrientation is %{public}d \n", __func__, curOrientation_);
    RS_LOGD_IF(DEBUG_PIPELINE, "[%{public}s] notchStatus is %{public}d \n", __func__, notchStatus_);
    switch (curOrientation_) {
        case ScreenRotation::ROTATION_0:
        case ScreenRotation::ROTATION_180:
            if (notchStatus_ == WINDOW_NOTCH_HIDDEN) {
                RS_LOGD_IF(DEBUG_PIPELINE, "[%{public}s] prepare TOP_HIDDEN show resource \n", __func__);
                showResourceType_ = TOP_HIDDEN;
            } else {
                RS_LOGD_IF(DEBUG_PIPELINE, "[%{public}s] prepare TOP_PORTRAIT show resource \n", __func__);
                showResourceType_ = TOP_PORTRAIT;
            }
            break;
        case ScreenRotation::ROTATION_90:
        case ScreenRotation::ROTATION_270:
            if (notchStatus_ == WINDOW_NOTCH_HIDDEN) {
                RS_LOGD_IF(DEBUG_PIPELINE, "[%{public}s] prepare TOP_LADS_ORIT show resource \n", __func__);
                showResourceType_ = TOP_LADS_ORIT;
            } else {
                RS_LOGD_IF(DEBUG_PIPELINE, "[%{public}s] prepare TOP_PORTRAIT show resource \n", __func__);
                showResourceType_ = TOP_PORTRAIT;
            }
            break;
        default:
            RS_LOGD_IF(DEBUG_PIPELINE, "[%{public}s] Unknow orientation, use default type \n", __func__);
            showResourceType_ = TOP_PORTRAIT;
            break;
    }
}

void RoundCornerDisplay::RcdChooseRSResource()
{
    switch (showResourceType_) {
        case TOP_PORTRAIT:
            curTop_ = imgTopPortrait_;
            RS_LOGD_IF(DEBUG_PIPELINE, "prepare imgTopPortrait_ resource \n");
            break;
        case TOP_HIDDEN:
            curTop_ = imgTopHidden_;
            RS_LOGD_IF(DEBUG_PIPELINE, "prepare imgTopHidden_ resource \n");
            break;
        case TOP_LADS_ORIT:
            curTop_ = imgTopLadsOrit_;
            RS_LOGD_IF(DEBUG_PIPELINE, "prepare imgTopLadsOrit_ resource \n");
            break;
        default:
            RS_LOGE("[%{public}s] No showResourceType found with type %{public}d \n", __func__, showResourceType_);
            break;
    }
    curBottom_ = imgBottomPortrait_;
}

void RoundCornerDisplay::RcdChooseHardwareResource()
{
    if (rog_ == nullptr) {
        RS_LOGE_IF(DEBUG_PIPELINE, "[%{public}s] No rog info \n", __func__);
        return;
    }
    auto portrait = rog_->GetPortrait(std::string(rs_rcd::NODE_PORTRAIT));
    auto landscape = rog_->GetLandscape(std::string(rs_rcd::NODE_LANDSCAPE));
    switch (showResourceType_) {
        case TOP_PORTRAIT:
            if (portrait == std::nullopt) {
                break;
            }
            hardInfo_.topLayer = std::make_shared<rs_rcd::RoundCornerLayer>(portrait->layerUp);
            hardInfo_.topLayer->curBitmap = &bitmapTopPortrait_;
            break;
        case TOP_HIDDEN:
            if (portrait == std::nullopt) {
                break;
            }
            hardInfo_.topLayer = std::make_shared<rs_rcd::RoundCornerLayer>(portrait->layerHide);
            hardInfo_.topLayer->curBitmap = &bitmapTopHidden_;
            break;
        case TOP_LADS_ORIT:
            if (landscape == std::nullopt) {
                break;
            }
            hardInfo_.topLayer = std::make_shared<rs_rcd::RoundCornerLayer>(landscape->layerUp);
            hardInfo_.topLayer->curBitmap = &bitmapTopLadsOrit_;
            break;
        default:
            RS_LOGE("[%{public}s] No showResourceType found with type %{public}d \n", __func__, showResourceType_);
            break;
    }
    if (portrait == std::nullopt) {
        return;
    }
    hardInfo_.bottomLayer = std::make_shared<rs_rcd::RoundCornerLayer>(portrait->layerDown);
    hardInfo_.bottomLayer->curBitmap = &bitmapBottomPortrait_;
}

void RoundCornerDisplay::DrawOneRoundCorner(RSPaintFilterCanvas* canvas, int surfaceType)
{
    RS_TRACE_BEGIN("RCD::DrawOneRoundCorner : surfaceType" + std::to_string(surfaceType));
    if (canvas == nullptr) {
        RS_LOGE("[%{public}s] Canvas is null \n", __func__);
        RS_TRACE_END();
        return;
    }
    UpdateParameter(updateFlag_);
    if (surfaceType == TOP_SURFACE) {
        RS_LOGD_IF(DEBUG_PIPELINE, "[%{public}s] draw TopSurface start  \n", __func__);
        if (curTop_ != nullptr) {
            Drawing::Brush brush;
            canvas->AttachBrush(brush);
            canvas->DrawImage(*curTop_, displayRect_.GetLeft(), displayRect_.GetTop(), Drawing::SamplingOptions());
            canvas->DetachBrush();
            RS_LOGD_IF(DEBUG_PIPELINE, "[%{public}s] Draw top \n", __func__);
        }
    } else if (surfaceType == BOTTOM_SURFACE) {
        RS_LOGD_IF(DEBUG_PIPELINE, "[%{public}s] BottomSurface supported \n", __func__);
        if (curBottom_ != nullptr) {
            Drawing::Brush brush;
            canvas->AttachBrush(brush);
            canvas->DrawImage(*curBottom_, displayRect_.GetLeft(), displayRect_.GetHeight() - curBottom_->GetHeight() +
                displayRect_.GetTop(), Drawing::SamplingOptions());
            canvas->DetachBrush();
            RS_LOGD_IF(DEBUG_PIPELINE, "[%{public}s] Draw Bottom \n", __func__);
        }
    } else {
        RS_LOGD_IF(DEBUG_PIPELINE, "[%{public}s] Surface Type is not valid \n", __func__);
    }
    RS_TRACE_END();
}

void RoundCornerDisplay::DrawTopRoundCorner(RSPaintFilterCanvas* canvas)
{
    DrawOneRoundCorner(canvas, TOP_SURFACE);
}

void RoundCornerDisplay::DrawBottomRoundCorner(RSPaintFilterCanvas* canvas)
{
    DrawOneRoundCorner(canvas, BOTTOM_SURFACE);
}

void RoundCornerDisplay::PrintRCDInfo()
{
    RS_LOGI("[%{public}s] begin \n", __func__);
    if (lcdModel_ != nullptr) {
        RS_LOGI("[%{public}s] Selected model: %{public}s, supported: top->%{public}d, bottom->%{public}d,"
            "hardware->%{public}d rogListSize %{public}d\n", __func__, lcdModel_->name.c_str(),
            static_cast<int>(supportTopSurface_), static_cast<int>(supportBottomSurface_),
            static_cast<int>(supportHardware_),
            static_cast<int>(lcdModel_->rogs.size()));
    }
    if (rog_ != nullptr) {
        RS_LOGI("[%{public}s] rog info : \n", __func__);
        rs_rcd::RCDConfig::PrintParseRog(rog_);
    }
    RS_LOGI("[%{public}s] render target displayNode Id %{public}" PRIu64 " \n", __func__,
        renderTargetId_);
    RS_LOGI("[%{public}s] current rect, notch: %{public}u, %{public}u, %{public}u, %{public}u, %{public}d \n", __func__,
        displayRect_.GetLeft(), displayRect_.GetTop(), displayRect_.GetWidth(), displayRect_.GetHeight(), notchStatus_);

    RS_LOGI("[%{public}s] current hardware Info rcdchangeTag, rcdPreparingTag : %{public}d , %{public}d \n", __func__,
        hardInfo_.resourceChanged, hardInfo_.resourcePreparing);
    if (hardInfo_.topLayer != nullptr) {
        RS_LOGW("[%{public}s] current hardware Info toplayer w h : %{public}u , %{public}u\n", __func__,
            hardInfo_.topLayer->cldWidth, hardInfo_.topLayer->cldHeight);
    }
    if (hardInfo_.bottomLayer != nullptr) {
        RS_LOGW("[%{public}s] current hardware Info bottomlayer w h : %{public}u , %{public}u\n", __func__,
            hardInfo_.bottomLayer->cldWidth, hardInfo_.bottomLayer->cldHeight);
    }
    RS_LOGI("[%{public}s] end \n", __func__);
}
} // namespace Rosen
} // namespace OHOS
