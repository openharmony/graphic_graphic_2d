/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "rs_hetero_hdr_hpae.h"

namespace OHOS {
namespace Rosen {
inline constexpr int DVFS_LEVEL_MIDDLE = 3;
inline constexpr int DVFS_LEVEL_HIGH = 4;
inline constexpr float MAX_SCALE_ARSR = 2.0;
// for scene id capability bit0 ~ bit55
inline constexpr uint64_t MDC_CAP_HEBCE = 1;
inline constexpr uint64_t MDC_CAP_UVUP = 1 << 1;
inline constexpr uint64_t MDC_CAP_SCALE = 1 << 2;
inline constexpr uint64_t MDC_CAP_ROT = 1 << 3;
inline constexpr uint64_t MDC_CAP_HDR = 1 << 4;
inline constexpr uint64_t MDC_USER_RS_VIDEO_HDR = 1ULL << 35;
inline constexpr uint64_t HDR_VIDEO_USER_TYPE = MDC_CAP_HEBCE | MDC_CAP_HDR | MDC_CAP_ROT |
    MDC_CAP_SCALE | MDC_USER_RS_VIDEO_HDR;
inline constexpr uint64_t AIHDR_VIDEO_USER_TYPE = MDC_CAP_HEBCE | MDC_CAP_ROT | MDC_CAP_SCALE | MDC_USER_RS_VIDEO_HDR;

RSHeteroHDRHpae& RSHeteroHDRHpae::GetInstance()
{
    static RSHeteroHDRHpae instance;
    return instance;
}

RSHeteroHDRHpae::RSHeteroHDRHpae()
{
    MDCHandle_ = dlopen(MDCLib_, RTLD_NOW);
    if (MDCHandle_ == nullptr) {
        RS_LOGE("[hdrHetero]:RSHeteroHDRHpae dlopen MDClib is failed");
        return;
    }
    *reinterpret_cast<void**>(&getMDCDevice) = dlsym(MDCHandle_, "GetMdcDevice");
    if (getMDCDevice == nullptr) {
        dlclose(MDCHandle_);
        MDCHandle_ = nullptr;
    }
    MDCDev_ = getMDCDevice();
    if (MDCDev_ == nullptr) {
        dlclose(MDCHandle_);
        MDCHandle_ = nullptr;
        getMDCDevice = nullptr;
        RS_LOGE("[hdrHetero]:RSHeteroHDRHpae getMDCDevice is failed");
        return;
    }
}

RSHeteroHDRHpae::~RSHeteroHDRHpae()
{
    if (MDCHandle_) {
        dlclose(MDCHandle_);
        MDCHandle_ = nullptr;
        MDCDev_ = nullptr;
    }
}

bool RSHeteroHDRHpae::IsHpaeAvailable() const
{
    return (MDCDev_ && MDCHandle_);
}

void RSHeteroHDRHpae::SetEffectResourceRequest(HdrStatus curHandleHdrStatus)
{
    if (curHandleHdrStatus == AI_HDR_VIDEO_GAINMAP) {
        MDCContent_.effectResourceRequest = AIHDR_HIGHLIGHT_MODE;
        return;
    }
    /*
    * The precondition has already determined that the width and height of dstBuffer are not zero,
    * so there is no need to check them here.
    */
    float wRatio = float(MDCContent_.srcRect.right - MDCContent_.srcRect.left) /
                   float(MDCContent_.dstRect.right - MDCContent_.dstRect.left);
    float hRatio = float(MDCContent_.srcRect.bottom - MDCContent_.srcRect.top) /
                   float(MDCContent_.dstRect.bottom - MDCContent_.dstRect.top);

    bool scaleARSR = (wRatio < MAX_SCALE_ARSR && hRatio < MAX_SCALE_ARSR);
    if (scaleARSR) {
        MDCContent_.effectResourceRequest = EFFECT_HDR | EFFECT_ARSR;
    } else {
        MDCContent_.effectResourceRequest = EFFECT_HDR;
    }
}

int32_t RSHeteroHDRHpae::BuildHpaeHDRTask(HpaeTaskInfoT& taskInfo)
{
    if (taskInfo.dstRect.right == 0 || taskInfo.dstRect.bottom == 0) {
        *(taskInfo.taskPtr) = nullptr;
        RS_LOGE("[hdrHetero]:RSHeteroHDRHpae BuildHpaeHDRTask dstRect is invalid");
        return -1;
    }
    if (!(MDCExistedStatus_.load())) {
        return -1;
    }
    MDCContent_.srcRect = taskInfo.srcRect;
    MDCContent_.dstRect = taskInfo.dstRect;
    MDCContent_.perfLev = DVFS_LEVEL_HIGH;
    MDCContent_.transform = taskInfo.transform;
    MDCContent_.srcHandle = taskInfo.srcHandle;
    MDCContent_.dstHandle = taskInfo.dstHandle;
    MDCContent_.acquireFenceFd = taskInfo.acquireFenceFd;
    MDCContent_.releaseFenceFd = taskInfo.releaseFenceFd;
    MDCContent_.displaySdrNit = taskInfo.displaySdrNit;
    MDCContent_.displayHdrNit = taskInfo.displayHdrNit;
    MDCContent_.isAsyncTask = true;
    MDCContent_.taskId = taskInfo.taskId;
    MDCContent_.taskPtr = taskInfo.taskPtr;

    SetEffectResourceRequest(taskInfo.curHandleStatus);

    int ret = MDCDev_->copybit(MDCDev_, 0, &MDCContent_);

    RS_LOGD("[hdrHetero]:RSHeteroHDRHpae BuildHpaeHDRTask compose ret:%{public}d copybit taskId:%{public}d",
        ret, *MDCContent_.taskId);

    if (ret != 0) {
        MDCStatus_.store(false);
        MDCDev_->destroyTask(*MDCContent_.taskId);
        MDCContent_.taskPtr = nullptr;
        return -1;
    }
    MDCStatus_.store(true);
    return 0;
}

uint64_t RSHeteroHDRHpae::GetChannelCaps(HdrStatus curHandleHdrStatus)
{
    uint64_t channelCap = 0;
    if (curHandleHdrStatus == HDR_VIDEO) {
        channelCap = HDR_VIDEO_USER_TYPE;
    } else if (curHandleHdrStatus == HdrStatus::AI_HDR_VIDEO_GAINMAP) {
        channelCap = AIHDR_VIDEO_USER_TYPE;
    }
    return channelCap;
}

int32_t RSHeteroHDRHpae::RequestHpaeChannel(HdrStatus curHandleHdrStatus)
{
    if (MDCExistedStatus_.load()) {
        if (existedChannelStatus_.load() == curHandleHdrStatus) {
            return 0;
        } else {
            ReleaseHpaeHDRChannel();
        }
    }
    if (MDCDev_->requestChannelByCap == nullptr) {
        RS_LOGE("[hdrHetero]:RSHeteroHDRHpae RequestHpaeChannel requestChannelByCap is null");
        return -1;
    }
    int channelStatus = -1;
    uint64_t channelCap = GetChannelCaps(curHandleHdrStatus);
    if (channelCap != 0) {
        channelStatus = MDCDev_->requestChannelByCap(MDCDev_, channelCap);
    }
    if (channelStatus < 0) {
        RS_LOGE("[hdrHetero]:RSHeteroHDRHpae RequestHpaeChannel request MDC channel failed caps:%{public}" PRIu64,
            channelCap);
        return channelStatus;
    }
    MDCExistedStatus_.store(true);
    existedChannelStatus_.store(curHandleHdrStatus);
    return 0;
}

bool RSHeteroHDRHpae::CheckHpaeAccessible(HdrStatus curHandleHdrStatus)
{
    if (RequestHpaeChannel(curHandleHdrStatus) != 0) {
        return false;
    }
    bool isConflict = true;
    uint64_t channelCap = GetChannelCaps(curHandleHdrStatus);
    if (channelCap != 0) {
        isConflict = MDCDev_->checkResourceConflict(MDCDev_, channelCap);
    } else {
        RS_LOGW("[hdrHetero]:RSHeteroHDRHpae CheckHpaeAccessible channelCap is invalid");
        return false;
    }
    if (isConflict) {
        RS_LOGW("[hdrHetero]:RSHeteroHDRHpae CheckHpaeAccessible Hpae is conflict:%{public}" PRIu64, channelCap);
        ReleaseHpaeHDRChannel();
        return false;
    }
    return true;
}

void RSHeteroHDRHpae::DestroyHpaeHDRTask(uint32_t taskId)
{
    if (MDCStatus_.load()) {
        MDCDev_->destroyTask(taskId);
        MDCStatus_.store(true);
    }
}

void RSHeteroHDRHpae::ReleaseHpaeHDRChannel()
{
    if (MDCExistedStatus_.load() && MDCDev_ != nullptr) {
        RS_TRACE_NAME("[hdrHetero]:RSHeteroHDRHpae ReleaseHpaeHDRChannel");
        MDCDev_->releaseChannel(MDCDev_, 0);
        MDCExistedStatus_.store(false);
        existedChannelStatus_.store(HdrStatus::NO_HDR);
        RS_LOGD("[hdrHetero]:RSHeteroHDRHpae ReleaseHpaeHDRChannel done");
    }
}
} // namespace Rosen
} // namespace OHOS
