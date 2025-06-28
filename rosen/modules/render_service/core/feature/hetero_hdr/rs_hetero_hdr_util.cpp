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

#include "rs_hetero_hdr_util.h"

namespace OHOS {
namespace Rosen {
RSHeteroHdrUtil &RSHeteroHdrUtil::GetInstance()
{
    static RSHeteroHdrUtil instance;
    return instance;
}

RSHeteroHdrUtil::RSHeteroHdrUtil()
{
    mdcHandle = dlopen(mdcLib, RTLD_NOW);
    if (mdcHandle == nullptr) {
        RS_LOGW("[%{public}s_%{public}d]:load library failed, reason: %{public}s", __func__, __LINE__, dlerror());
        return;
    }
    *reinterpret_cast<void **>(&getMdcDevice) = dlsym(mdcHandle, "GetMdcDevice");
    mdcDev = getMdcDevice();
    if (mdcDev == nullptr) {
        RS_LOGE("mdcDev is nullptr");
        return;
    }
}

RSHeteroHdrUtil::~RSHeteroHdrUtil()
{
    if (mdcHandle) {
        dlclose(mdcHandle);
        mdcDev = nullptr;
    }
}

int32_t RSHeteroHdrUtil::BuildHpaeHdrTask(hapeTaskInfoT& taskinfo)
{
    if (taskinfo.dstRect.right == 0 || taskinfo.dstRect.bottom == 0) {
        *taskinfo.taskPtr = nullptr;
        RS_LOGE("dstRect is invalid");
        return -1;
    }
    if (!(mdcExistedStatus_.load())) {
        return -1;
    }

    mdcContent.srcRect = taskinfo.srcRect;
    mdcContent.dstRect = taskinfo.dstRect;
    mdcContent.perfLev = DVFS_LEVEL_HIGH;
 
    mdcContent.transform = taskinfo.transform;
    mdcContent.srcHandle = taskinfo.srcHandle;
    mdcContent.dstHandle = taskinfo.dstHandle;
    mdcContent.acquireFenceFd = taskinfo.acquireFenceFd;
    mdcContent.releaseFenceFd = taskinfo.releaseFenceFd;
    mdcContent.displaySdrNit = taskinfo.displaySDRNits;
    mdcContent.displayHdrNit = taskinfo.displayHDRNits;
    mdcContent.isAsyncTask = true;
    mdcContent.taskId = taskinfo.taskId;
    mdcContent.taskPtr = taskinfo.taskPtr;

    mdcContent.effectResourceRequest = 1ULL << EFFECT_RESOURCE_HDR_BIT;

    int ret = mdcDev->copybit(mdcDev, 0, &mdcContent);
    if (ret != 0) {
        mdcStatus_.store(false);
        RS_LOGE("do mdc copybit fail");
        mdcDev->releaseChannel(mdcDev, 0);
        mdcContent.taskPtr = nullptr;
        return -1;
    }
    mdcStatus_.store(true);
    return 0;
}

int32_t RSHeteroHdrUtil::RequestHpaeChannel()
{
    if (mdcExistedStatus_.load()) {
        return 0;
    }
    if (mdcDev->requestChannelByCap == nullptr) {
        RS_LOGE("requestChannelByCap is null");
        return -1;
    }
    int channelStatus = mdcDev->requestChannelByCap(mdcDev, MDC_CAP_HEBCE | MDC_CAP_HDR | MDC_CAP_ROT | MDC_CAP_SCALE);
    if (channelStatus < 0) {
        RS_LOGE("request mediacomm channel fail");
        return channelStatus;
    }
    mdcExistedStatus_.store(true);
    return 0;
}

void RSHeteroHdrUtil::DestroyHpaeHdrTask(uint32_t taskId)
{
    if (mdcStatus_.load()) {
        mdcDev->destroyTask(taskId);
        mdcStatus_.store(true);
    }
}

void RSHeteroHdrUtil::ReleaseHpaeHdrChannel()
{
    if (mdcExistedStatus_.load()) {
        RS_TRACE_NAME("RSHeteroHdrUtil::ReleaseHpaeHdrChannel no need post task");
        RS_LOGD("RSHeteroHdrUtil::ReleaseHpaeHdrChannel no need post task");
        mdcDev->releaseChannel(mdcDev, 0);
        mdcExistedStatus_.store(false);
    }
}
} // namespace Rosen
} // namespace OHOS
