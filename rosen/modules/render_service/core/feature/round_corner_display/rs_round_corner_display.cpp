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
#include "feature/round_corner_display/rs_rcd_bitmap_utils.h"
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
    auto mode = lcdModel_->imageLoadConfig.mode;
    imgLoader_ = RCDImageLoader::Create(mode, supportHardware_, supportTopSurface_, supportBottomSurface_);
    RS_LOGI("[%{public}s] Selected model: %{public}s, supported: top->%{public}d, bottom->%{public}d,"
        "hardware->%{public}d loadeType %{public}d \n", __func__, lcdModelName, static_cast<int>(supportTopSurface_),
        static_cast<int>(supportBottomSurface_), static_cast<int>(supportHardware_), static_cast<int>(mode));
    return true;
}

bool RoundCornerDisplay::LoadConfigFile()
{
    RS_LOGD_IF(DEBUG_PIPELINE, "[%{public}s] LoadConfigFile \n", __func__);
    auto& rcdCfg = RSSingleton<rs_rcd::RCDConfig>::GetInstance();
    return rcdCfg.Load(std::string(rs_rcd::PATH_CONFIG_FILE));
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

bool RoundCornerDisplay::HandleTopRcdDirty(RectI& dirtyRect)
{
    std::shared_lock<std::shared_mutex> lock(resourceMut_);
    if ((static_cast<uint8_t>(rcdDirtyType_) & static_cast<uint8_t>(RoundCornerDirtyType::RCD_DIRTY_TOP)) !=
        static_cast<uint8_t>(RoundCornerDirtyType::RCD_DIRTY_TOP)) {
        return false;
    }
    if (hardInfo_.topLayer != nullptr && hardInfo_.topLayer->curBitmap != nullptr) {
        dirtyRect = dirtyRect.JoinRect(RectI(displayRect_.GetLeft(), displayRect_.GetTop(),
            hardInfo_.topLayer->curBitmap->GetWidth(), hardInfo_.topLayer->curBitmap->GetHeight()));
    }
    if (!hardInfo_.resourceChanged) {
        rcdDirtyType_ = static_cast<RoundCornerDirtyType>(
            (~static_cast<uint8_t>(RoundCornerDirtyType::RCD_DIRTY_TOP)) & static_cast<uint8_t>(rcdDirtyType_));
    }
    return true;
}

bool RoundCornerDisplay::HandleBottomRcdDirty(RectI& dirtyRect)
{
    std::shared_lock<std::shared_mutex> lock(resourceMut_);
    if ((static_cast<uint8_t>(rcdDirtyType_) & static_cast<uint8_t>(RoundCornerDirtyType::RCD_DIRTY_BOTTOM)) !=
        static_cast<uint8_t>(RoundCornerDirtyType::RCD_DIRTY_BOTTOM)) {
        return false;
    }
    if (hardInfo_.bottomLayer != nullptr && hardInfo_.bottomLayer->curBitmap != nullptr) {
        dirtyRect = dirtyRect.JoinRect(
            RectI(displayRect_.GetLeft(),
            displayRect_.GetHeight() - hardInfo_.bottomLayer->curBitmap->GetHeight() + displayRect_.GetTop(),
            hardInfo_.bottomLayer->curBitmap->GetWidth(),
            hardInfo_.bottomLayer->curBitmap->GetHeight()));
    }
    if (!hardInfo_.resourceChanged) {
        rcdDirtyType_ = static_cast<RoundCornerDirtyType>(
            (~static_cast<uint8_t>(RoundCornerDirtyType::RCD_DIRTY_BOTTOM)) & static_cast<uint8_t>(rcdDirtyType_));
    }
    return true;
}

bool RoundCornerDisplay::LoadImgsbyResolution(uint32_t width, uint32_t height)
{
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
    return true;
}

void RoundCornerDisplay::UpdateDisplayParameter(uint32_t left, uint32_t top, uint32_t width, uint32_t height)
{
    std::unique_lock<std::shared_mutex> lock(resourceMut_);
    RectU displayRect(left, top, width, height);
    if (displayRect != lastRcvDisplayRect_) {
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
    for (auto item = updateFlag.begin(); item != updateFlag.end(); item++) {
        if (item->second == true) {
            resourceChanged = true;
            item->second = false; // reset
        }
    }
    if (resourceChanged) {
        RcdChooseTopResourceType();
        if (supportHardware_ && imgLoader_) {
            imgLoader_->RcdChooseHardwareResource(showResourceType_, rog_, hardInfo_);
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

void RoundCornerDisplay::PrintRCDInfo()
{
    RS_LOGD("[%{public}s] begin \n", __func__);
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
    RS_LOGI("[%{public}s] render target id: %{public}" PRIu64
            ", display rect: (%{public}u, %{public}u, %{public}u, %{public}u), notch: %{public}d, resource tag: "
            "%{public}d , %{public}d\n",
        __func__,
        renderTargetId_,
        displayRect_.GetLeft(),
        displayRect_.GetTop(),
        displayRect_.GetWidth(),
        displayRect_.GetHeight(),
        notchStatus_,
        hardInfo_.resourceChanged,
        hardInfo_.resourcePreparing);
    if (hardInfo_.topLayer != nullptr) {
        RS_LOGW("[%{public}s] current hardware Info toplayer w h : %{public}u , %{public}u\n", __func__,
            hardInfo_.topLayer->cldWidth, hardInfo_.topLayer->cldHeight);
    }
    if (hardInfo_.bottomLayer != nullptr) {
        RS_LOGW("[%{public}s] current hardware Info bottomlayer w h : %{public}u , %{public}u\n", __func__,
            hardInfo_.bottomLayer->cldWidth, hardInfo_.bottomLayer->cldHeight);
    }
    RS_LOGD("[%{public}s] end \n", __func__);
}
} // namespace Rosen
} // namespace OHOS
