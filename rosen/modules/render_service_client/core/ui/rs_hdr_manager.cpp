/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "ui/rs_hdr_manager.h"

#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {

RSHDRManager::RSHDRManager()
{}
RSHDRManager::~RSHDRManager()
{}

RSHDRManager& RSHDRManager::Instance()
{
    static RSHDRManager instance;
    return instance;
}

int RSHDRManager::IncreaseHDRNum()
{
    std::lock_guard<std::mutex> lockGuard(mutex_);

    hdrNum_++;
    if (hdrNum_ == 1 && setHDRPresent_ != nullptr && nodeId_ != INVALID_NODEID) {
        ROSEN_LOGD("SetHDRPresent true");
        setHDRPresent_(true, nodeId_);
    }

    return hdrNum_;
}

int RSHDRManager::ReduceHDRNum()
{
    std::lock_guard<std::mutex> lockGuard(mutex_);

    if (hdrNum_ == 0) {
        ROSEN_LOGE("ReduceHDRNum error, hdrNum_ is 0, cannot ReduceHDRNum");
        return hdrNum_;
    }

    hdrNum_--;
    if (hdrNum_ == 0 && setHDRPresent_ != nullptr && nodeId_ != INVALID_NODEID) {
        ROSEN_LOGD("SetHDRPresent false");
        setHDRPresent_(false, nodeId_);
    }

    return hdrNum_;
}

void RSHDRManager::ResetHDRNum()
{
    hdrNum_ = 0;
}

int RSHDRManager::getHDRNum()
{
    return hdrNum_;
}

void RSHDRManager::RegisterSetHDRPresent(HDRFunc func, NodeId id)
{
    std::lock_guard<std::mutex> lockGuard(mutex_);

    if (func == nullptr || setHDRPresent_ != nullptr || id == INVALID_NODEID) {
        ROSEN_LOGE("RegisterSetHDRPresent fail");
        return;
    }
    nodeId_ = id;
    setHDRPresent_ = func;
}

void RSHDRManager::UnRegisterSetHDRPresent(NodeId id)
{
    std::lock_guard<std::mutex> lockGuard(mutex_);

    if (nodeId_ != id) {
        return;
    }
    nodeId_ = INVALID_NODEID;
    setHDRPresent_ = nullptr;
    ResetHDRNum();
}

} // namespace Rosen
} // namespace OHOS
