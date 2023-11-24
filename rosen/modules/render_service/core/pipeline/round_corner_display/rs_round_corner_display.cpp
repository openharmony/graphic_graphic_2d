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
#include <SkImage.h>
#include <SkData.h>
#include <SkStream.h>
#include <SkMatrix.h>
#include <SkCanvas.h>
#include <mutex>
#include "common/rs_singleton.h"
#include "rs_sub_thread_rcd.h"

namespace OHOS {
namespace Rosen {
RoundCornerDisplay::RoundCornerDisplay()
{
    RS_LOGD("[%{public}s] Created \n", __func__);
    Init();
}

RoundCornerDisplay::~RoundCornerDisplay()
{
    RS_LOGD("[%{public}s] Destroy \n", __func__);
}

bool RoundCornerDisplay::Init()
{
    LoadConfigFile();
    SeletedLcdModel(rs_rcd::ATTR_DEFAULT);
    LoadImgsbyResolution(displayWidth_, displayHeight_);
    RS_LOGD("[%{public}s] RoundCornerDisplay init \n", __func__);
    return true;
}

bool RoundCornerDisplay::SeletedLcdModel(const char* lcdModelName)
{
    auto& rcdCfg = RSSingleton<rs_rcd::RCDConfig>::GetInstance();
    lcdModel_ = rcdCfg.GetLcdModel(lcdModelName);
    if (lcdModel_ == nullptr) {
        RS_LOGE("[%{public}s] No lcdModel found in config file with name %{public}s \n", __func__, lcdModelName);
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
    RS_LOGD("[%{public}s] LoadConfigFile \n", __func__);
    auto& rcdCfg = RSSingleton<rs_rcd::RCDConfig>::GetInstance();
    return rcdCfg.Load(rs_rcd::PATH_CONFIG_FILE);
}

bool RoundCornerDisplay::LoadImg(const char* path, sk_sp<SkImage>& img)
{
    std::string filePath = std::string(rs_rcd::PATH_CONFIG_DIR) + "/" + path;
    RS_LOGD("[%{public}s] Read Img(%{public}s) \n", __func__, filePath.c_str());
    sk_sp<SkData> skData = SkData::MakeFromFileName(filePath.c_str());
    if (skData == nullptr) {
        RS_LOGE("[%{public}s] Open picture file failed! \n", __func__);
        return false;
    }
    img = SkImage::MakeFromEncoded(skData);
    if (img == nullptr) {
        RS_LOGE("[%{public}s] Decode picture file failed! \n", __func__);
        return false;
    }
    return true;
}

bool RoundCornerDisplay::DecodeBitmap(sk_sp<SkImage> image, SkBitmap &bitmap)
{
    if (image == nullptr) {
        RS_LOGE("[%{public}s] No image found \n", __func__);
        return false;
    }
    if (!image->asLegacyBitmap(&bitmap, SkImage::kRO_LegacyBitmapMode)) {
        RS_LOGE("[%{public}s] Create bitmap from skImage failed \n", __func__);
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
    hardInfo_.topLayer->layerWidth = displayWidth_;
    hardInfo_.topLayer->layerHeight = displayHeight_;
    hardInfo_.bottomLayer->layerWidth = displayWidth_;
    hardInfo_.bottomLayer->layerHeight = displayHeight_;
    return true;
}

bool RoundCornerDisplay::GetTopSurfaceSource()
{
    if (rog_ == nullptr) {
        RS_LOGE("[%{public}s] No rog found in config file \n", __func__);
        return false;
    }
    rs_rcd::RCDConfig::PrintParseRog(rog_);

    if (rog_->portraitMap.count(rs_rcd::NODE_PORTRAIT) < 1) {
        RS_LOGE("[%{public}s] PORTRAIT layerUp do not configured \n", __func__);
        return false;
    }
    LoadImg(rog_->portraitMap[rs_rcd::NODE_PORTRAIT].layerUp.fileName.c_str(), imgTopPortrait_);

    if (rog_->landscapeMap.count(rs_rcd::NODE_LANDSCAPE) < 1) {
        RS_LOGE("[%{public}s] LANDSACPE layerUp do not configured \n", __func__);
        return false;
    }
    LoadImg(rog_->landscapeMap[rs_rcd::NODE_LANDSCAPE].layerUp.fileName.c_str(), imgTopLadsOrit_);

    if (rog_->portraitMap.count(rs_rcd::NODE_PORTRAIT) < 1) {
        RS_LOGE("[%{public}s] PORTRAIT layerHide do not configured \n", __func__);
        return false;
    }
    LoadImg(rog_->portraitMap[rs_rcd::NODE_PORTRAIT].layerHide.fileName.c_str(), imgTopHidden_);
    if (supportHardware_) {
        DecodeBitmap(imgTopPortrait_, bitmapTopPortrait_);
        DecodeBitmap(imgTopLadsOrit_, bitmapTopLadsOrit_);
        DecodeBitmap(imgTopHidden_, bitmapTopHidden_);
    }
    return true;
}

bool RoundCornerDisplay::GetBottomSurfaceSource()
{
    if (rog_ == nullptr) {
        RS_LOGE("[%{public}s] No rog found in config file \n", __func__);
        return false;
    }
    if (rog_->portraitMap.count(rs_rcd::NODE_PORTRAIT) < 1) {
        RS_LOGE("[%{public}s] PORTRAIT layerDown do not configured \n", __func__);
        return false;
    }
    LoadImg(rog_->portraitMap[rs_rcd::NODE_PORTRAIT].layerDown.fileName.c_str(), imgBottomPortrait_);
    if (supportHardware_) {
        DecodeBitmap(imgBottomPortrait_, bitmapBottomPortrait_);
    }
    return true;
}

bool RoundCornerDisplay::LoadImgsbyResolution(uint32_t width, uint32_t height)
{
    std::lock_guard<std::mutex> lock(resourceMut_);

    if (lcdModel_ == nullptr) {
        RS_LOGE("[%{public}s] No lcdModel selected in config file \n", __func__);
        return false;
    }
    rog_ = lcdModel_->GetRog(width, height);
    if (rog_ == nullptr) {
        RS_LOGE("[%{public}s] Can't find resolution (%{public}u x %{public}u) in config file \n",
            __func__, width, height);
        return false;
    }
    RS_LOGD("[%{public}s] Get rog resolution (%{public}u x %{public}u) in config file \n", __func__, width, height);
    if (supportTopSurface_) {
        if (!GetTopSurfaceSource()) {
            RS_LOGE("[%{public}s] Top surface support configured, but resources if missing! \n", __func__);
            return false;
        }
    }
    if (supportBottomSurface_) {
        if (!GetBottomSurfaceSource()) {
            RS_LOGE("[%{public}s] Bottom surface support configured, but resources if missing! \n", __func__);
            return false;
        }
    }
    return true;
}

void RoundCornerDisplay::UpdateDisplayParameter(uint32_t width, uint32_t height)
{
    std::lock_guard<std::mutex> lock(resourceMut_);
    if (width == displayWidth_ && height == displayHeight_) {
        RS_LOGD("[%{public}s] DisplayParameter do not change \n", __func__);
        return;
    }
    RS_LOGD("[%{public}s] displayWidth_ updated from %{public}u -> %{public}u,"
        "displayHeight_ updated from %{public}u -> %{public}u \n", __func__,
        displayWidth_, width, displayHeight_, height);

    displayWidth_ = width;
    displayHeight_ = height;

    RSSingleton<RSSubThreadRCD>::GetInstance().PostTask([this]() {
        LoadImgsbyResolution(displayWidth_, displayHeight_);
    });
    updateFlag_["display"] = true;
}

void RoundCornerDisplay::UpdateNotchStatus(int status)
{
    std::lock_guard<std::mutex> lock(resourceMut_);
    // Update surface when surface status changed
    if (status < 0 || status > 1) {
        RS_LOGE("[%{public}s] notchStatus won't be over 1 or below 0 \n", __func__);
        return;
    }
    notchSetting_ = status;
    if (notchStatus_ == status) {
        RS_LOGD("[%{public}s] NotchStatus do not change \n", __func__);
        return;
    }
    RS_LOGD("[%{public}s] notchStatus change from %{public}d to %{public}d \n", __func__, notchStatus_, status);
    notchStatus_ = status;
    updateFlag_["notch"] = true;
}

void RoundCornerDisplay::UpdateOrientationStatus(ScreenRotation orientation)
{
    std::lock_guard<std::mutex> lock(resourceMut_);
    if (orientation == curOrientation_) {
        RS_LOGD("[%{public}s] OrientationStatus do not change \n", __func__);
        return;
    }
    
    const int ladsToPortrait = 0;
    const int portraitToLads = 1;

    lastOrientation_ = curOrientation_;
    curOrientation_ = orientation;
    RS_LOGD("[%{public}s] curOrientation_ = %{public}d, lastOrientation_ = %{public}d \n",
        __func__, curOrientation_, lastOrientation_);
    updateFlag_["orientation"] = true;
    int transitionFlag = -1; // default
    switch (lastOrientation_) {
        case ScreenRotation::ROTATION_0:
            if (curOrientation_ == ScreenRotation::ROTATION_90 || curOrientation_ == ScreenRotation::ROTATION_270) {
                transitionFlag = portraitToLads;
            }
            break;
        case ScreenRotation::ROTATION_90:
            if (curOrientation_ == ScreenRotation::ROTATION_0 || curOrientation_ == ScreenRotation::ROTATION_180) {
                transitionFlag = ladsToPortrait;
            }
            break;
        case ScreenRotation::ROTATION_180:
            if (curOrientation_ == ScreenRotation::ROTATION_90 || curOrientation_ == ScreenRotation::ROTATION_270) {
                transitionFlag = portraitToLads;
            }
            break;
        case ScreenRotation::ROTATION_270:
            if (curOrientation_ == ScreenRotation::ROTATION_0 || curOrientation_ == ScreenRotation::ROTATION_180) {
                transitionFlag = ladsToPortrait;
            }
            break;
        default:
            break;
    }
    if (notchSetting_ == WINDOW_NOTCH_DEFAULT) {
        if (transitionFlag == ladsToPortrait) {
            notchStatus_ = WINDOW_NOTCH_DEFAULT;
        } else if (transitionFlag == portraitToLads) {
            notchStatus_ = WINDOW_NOTCH_HIDDEN;
        }
    }
}

void RoundCornerDisplay::UpdateParameter(std::map<std::string, bool>& updateFlag)
{
    hardInfo_.resourceChanged = false;
    for (auto item = updateFlag.begin(); item != updateFlag.end(); item++) {
        if (item->second == true) {
            resourceChanged = true;
            item->second = false;  // reset
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
        resourceChanged = false; // reset
    } else {
        RS_LOGD("[%{public}s] Status is not changed \n", __func__);
    }
}

// Choose the approriate resource type according to orientation and notch status
void RoundCornerDisplay::RcdChooseTopResourceType()
{
    RS_LOGD("[%{public}s] Choose surface \n", __func__);
    RS_LOGD("[%{public}s] curOrientation is %{public}d \n", __func__, curOrientation_);
    RS_LOGD("[%{public}s] notchStatus is %{public}d \n", __func__, notchStatus_);
    switch (curOrientation_) {
        case ScreenRotation::ROTATION_0:
        case ScreenRotation::ROTATION_180:
            if (notchStatus_ == WINDOW_NOTCH_HIDDEN) {
                RS_LOGD("prepare TOP_HIDDEN show resource \n");
                showResourceType_ = TOP_HIDDEN;
            } else {
                RS_LOGD("prepare TOP_PORTRAIT show resource \n");
                showResourceType_ = TOP_PORTRAIT;
            }
            break;
        case ScreenRotation::ROTATION_90:
        case ScreenRotation::ROTATION_270:
            if (notchStatus_ == WINDOW_NOTCH_HIDDEN) {
                RS_LOGD("prepare TOP_LADS_ORIT show resource \n");
                showResourceType_ = TOP_LADS_ORIT;
            } else {
                RS_LOGD("prepare TOP_PORTRAIT show resource \n");
                showResourceType_ = TOP_PORTRAIT;
            }
            break;
        default:
            RS_LOGD("Unknow orientation, use default type \n");
            showResourceType_ = TOP_PORTRAIT;
            break;
    }
}

void RoundCornerDisplay::RcdChooseRSResource()
{
    switch (showResourceType_) {
        case TOP_PORTRAIT:
            curTop_ = imgTopPortrait_;
            RS_LOGD("prepare imgTopPortrait_ resource \n");
            break;
        case TOP_HIDDEN:
            curTop_ = imgTopHidden_;
            RS_LOGD("prepare imgTopHidden_ resource \n");
            break;
        case TOP_LADS_ORIT:
            curTop_ = imgTopLadsOrit_;
            RS_LOGD("prepare imgTopLadsOrit_ resource \n");
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
        RS_LOGE("[%{public}s] No rog info \n", __func__);
        return;
    }
    switch (showResourceType_) {
        case TOP_PORTRAIT:
            if (rog_->portraitMap.count(rs_rcd::NODE_PORTRAIT) < 1) {
                RS_LOGE("[%{public}s] PORTRAIT layerHide do not configured \n", __func__);
                break;
            }
            hardInfo_.topLayer = &rog_->portraitMap[rs_rcd::NODE_PORTRAIT].layerUp;
            hardInfo_.topLayer->curBitmap = &bitmapTopPortrait_;
            break;
        case TOP_HIDDEN:
            if (rog_->portraitMap.count(rs_rcd::NODE_PORTRAIT) < 1) {
                RS_LOGE("[%{public}s] PORTRAIT layerHide do not configured \n", __func__);
                break;
            }
            hardInfo_.topLayer = &rog_->portraitMap[rs_rcd::NODE_PORTRAIT].layerHide;
            hardInfo_.topLayer->curBitmap = &bitmapTopHidden_;
            break;
        case TOP_LADS_ORIT:
            if (rog_->landscapeMap.count(rs_rcd::NODE_LANDSCAPE) < 1) {
                RS_LOGE("[%{public}s] PORTRAIT layerHide do not configured \n", __func__);
                break;
            }
            hardInfo_.topLayer = &rog_->landscapeMap[rs_rcd::NODE_LANDSCAPE].layerUp;
            hardInfo_.topLayer->curBitmap = &bitmapTopLadsOrit_;
            break;
        default:
            RS_LOGE("[%{public}s] No showResourceType found with type %{public}d \n", __func__, showResourceType_);
            break;
    }
    if (rog_->portraitMap.count(rs_rcd::NODE_PORTRAIT) < 1) {
        RS_LOGE("[%{public}s] PORTRAIT layerHide do not configured \n", __func__);
        return;
    }
    hardInfo_.bottomLayer = &rog_->portraitMap[rs_rcd::NODE_PORTRAIT].layerDown;
    hardInfo_.bottomLayer->curBitmap = &bitmapBottomPortrait_;
}

void RoundCornerDisplay::DrawRoundCorner(std::shared_ptr<RSPaintFilterCanvas> canvas)
{
    std::lock_guard<std::mutex> lock(resourceMut_);
    if (supportHardware_) {
        return;
    }
    if (canvas == nullptr) {
        RS_LOGE("[%{public}s] Canvas is null \n", __func__);
        return;
    }
    UpdateParameter(updateFlag_);
    if (supportTopSurface_) {
        RS_LOGD("[%{public}s] TopSurface supported \n", __func__);
        if (curTop_ != nullptr) {
            canvas->drawImage(curTop_, 0, 0);
            RS_LOGD("[%{public}s] Draw top \n", __func__);
        }
    }
    if (supportBottomSurface_) {
        RS_LOGD("[%{public}s] BottomSurface supported \n", __func__);
        if (curBottom_ != nullptr) {
            canvas->drawImage(curBottom_, 0, displayHeight_ - curBottom_->height());
            RS_LOGD("[%{public}s] Draw Bottom \n", __func__);
        }
    }
}
} // namespace Rosen
} // namespace OHOS
