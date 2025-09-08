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
namespace {
constexpr int DVFS_LEVEL_MIDDLE = 3;
constexpr int DVFS_LEVEL_HIGH = 4;
constexpr float MAX_SCALE_ARSR = 0.1;
// for scene id capability bit0 ~ bit55
constexpr uint64_t MDC_CAP_HEBCE = 1;
constexpr uint64_t MDC_CAP_UVUP = 1 << 1;
constexpr uint64_t MDC_CAP_SCALE = 1 << 2;
constexpr uint64_t MDC_CAP_ROT = 1 << 3;
constexpr uint64_t MDC_CAP_HDR = 1 << 4;
constexpr uint64_t MDC_USER_RS_VIDEO_HDR = 1ULL << 35;
constexpr uint64_t HDR_VIDEO_USER_TYPE = MDC_CAP_HEBCE | MDC_CAP_HDR | MDC_CAP_ROT |
    MDC_CAP_SCALE | MDC_USER_RS_VIDEO_HDR;
constexpr uint64_t AIHDR_VIDEO_USER_TYPE = MDC_CAP_HEBCE | MDC_CAP_ROT | MDC_CAP_SCALE | MDC_USER_RS_VIDEO_HDR;
}

RSHeteroHDRHpae& RSHeteroHDRHpae::GetInstance()
{
    static RSHeteroHDRHpae instance;
    return instance;
}

RSHeteroHDRHpae::RSHeteroHDRHpae()
{
    mdcHandle_ = dlopen(mdcLib_, RTLD_NOW);
    if (mdcHandle_ == nullptr) {
        RS_LOGE("[hdrHetero]:RSHeteroHDRHpae dlopen MDClib is failed");
        return;
    }
    *reinterpret_cast<void**>(&getMDCDevice) = dlsym(mdcHandle_, "GetMdcDevice");
    if (getMDCDevice == nullptr) {
        dlclose(mdcHandle_);
        mdcHandle_ = nullptr;
    }
    mdcDev_ = getMDCDevice();
    if (mdcDev_ == nullptr) {
        dlclose(mdcHandle_);
        mdcHandle_ = nullptr;
        getMDCDevice = nullptr;
        RS_LOGE("[hdrHetero]:RSHeteroHDRHpae getMDCDevice is failed");
        return;
    }
}

RSHeteroHDRHpae::~RSHeteroHDRHpae()
{
    if (mdcHandle_) {
        dlclose(mdcHandle_);
        mdcHandle_ = nullptr;
        mdcDev_ = nullptr;
    }
}

bool RSHeteroHDRHpae::IsHpaeAvailable() const
{
    return (mdcDev_ && mdcHandle_);
}

void RSHeteroHDRHpae::SetEffectResourceRequest(HdrStatus curHandleHdrStatus)
{
    if (curHandleHdrStatus == AI_HDR_VIDEO_GAINMAP) {
        mdcContent_.effectResourceRequest = AIHDR_HIGHLIGHT_MODE;
        return;
    }
    /*
    * The precondition has already determined that the width and height of dstBuffer are not zero,
    * so there is no need to check them here.
    */
    float wRatio = float(mdcContent_.srcRect.right - mdcContent_.srcRect.left) /
                   float(mdcContent_.dstRect.right - mdcContent_.dstRect.left);
    float hRatio = float(mdcContent_.srcRect.bottom - mdcContent_.srcRect.top) /
                   float(mdcContent_.dstRect.bottom - mdcContent_.dstRect.top);

    bool scaleARSR = (wRatio < MAX_SCALE_ARSR && hRatio < MAX_SCALE_ARSR);
    if (scaleARSR) {
        mdcContent_.effectResourceRequest = EFFECT_HDR | EFFECT_ARSR;
    } else {
        mdcContent_.effectResourceRequest = EFFECT_HDR;
    }
}

int32_t RSHeteroHDRHpae::BuildHpaeHDRTask(HpaeTaskInfoT& taskInfo)
{
    if (taskInfo.dstRect.right == 0 || taskInfo.dstRect.bottom == 0) {
        *(taskInfo.taskPtr) = nullptr;
        RS_LOGE("[hdrHetero]:RSHeteroHDRHpae BuildHpaeHDRTask dstRect is invalid");
        return -1;
    }
    if (!(mdcExistedStatus_.load())) {
        return -1;
    }
    mdcContent_.srcRect = taskInfo.srcRect;
    mdcContent_.dstRect = taskInfo.dstRect;
    mdcContent_.perfLev = DVFS_LEVEL_HIGH;
    mdcContent_.transform = taskInfo.transform;
    mdcContent_.srcHandle = taskInfo.srcHandle;
    mdcContent_.dstHandle = taskInfo.dstHandle;
    mdcContent_.acquireFenceFd = taskInfo.acquireFenceFd;
    mdcContent_.releaseFenceFd = taskInfo.releaseFenceFd;
    mdcContent_.displaySdrNit = taskInfo.displaySdrNit;
    mdcContent_.displayHdrNit = taskInfo.displayHdrNit;
    mdcContent_.isAsyncTask = true;
    mdcContent_.taskId = taskInfo.taskId;
    mdcContent_.taskPtr = taskInfo.taskPtr;

    SetEffectResourceRequest(taskInfo.curHandleStatus);

    int ret = mdcDev_->copybit(mdcDev_, 0, &mdcContent_);

    RS_LOGD("[hdrHetero]:RSHeteroHDRHpae BuildHpaeHDRTask compose ret:%{public}d copybit taskId:%{public}d",
        ret, *mdcContent_.taskId);

    if (ret != 0) {
        mdcStatus_.store(false);
        mdcDev_->destroyTask(*mdcContent_.taskId);
        mdcContent_.taskPtr = nullptr;
        return -1;
    }
    mdcStatus_.store(true);
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
    if (mdcExistedStatus_.load()) {
        if (existedChannelStatus_.load() == curHandleHdrStatus) {
            return 0;
        } else {
            ReleaseHpaeHDRChannel();
        }
    }
    if (mdcDev_->requestChannelByCap == nullptr) {
        RS_LOGE("[hdrHetero]:RSHeteroHDRHpae RequestHpaeChannel requestChannelByCap is null");
        return -1;
    }
    int channelStatus = -1;
    uint64_t channelCap = GetChannelCaps(curHandleHdrStatus);
    if (channelCap != 0) {
        channelStatus = mdcDev_->requestChannelByCap(mdcDev_, channelCap);
    }
    if (channelStatus < 0) {
        RS_LOGE("[hdrHetero]:RSHeteroHDRHpae RequestHpaeChannel request MDC channel failed caps:%{public}" PRIu64,
            channelCap);
        return channelStatus;
    }
    mdcExistedStatus_.store(true);
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
        isConflict = mdcDev_->checkResourceConflict(mdcDev_, channelCap);
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
    if (mdcStatus_.load()) {
        mdcDev_->destroyTask(taskId);
        mdcStatus_.store(true);
    }
}

void RSHeteroHDRHpae::ReleaseHpaeHDRChannel()
{
    if (mdcExistedStatus_.load() && mdcDev_ != nullptr) {
        RS_TRACE_NAME("[hdrHetero]:RSHeteroHDRHpae ReleaseHpaeHDRChannel");
        mdcDev_->releaseChannel(mdcDev_, 0);
        mdcExistedStatus_.store(false);
        existedChannelStatus_.store(HdrStatus::NO_HDR);
        RS_LOGD("[hdrHetero]:RSHeteroHDRHpae ReleaseHpaeHDRChannel done");
    }
}
} // namespace Rosen
} // namespace OHOS