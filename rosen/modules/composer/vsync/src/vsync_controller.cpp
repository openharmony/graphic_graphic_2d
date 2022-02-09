/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "vsync_controller.h"

namespace OHOS {
namespace Rosen {
VSyncController::VSyncController(const sptr<VSyncGenerator> &geng, int64_t offset)
    : generator_(geng), callbackMutex_(), callback_(nullptr),
    offsetMutex_(), phaseOffset_(offset), enabled_(false)
{
}

VSyncController::~VSyncController()
{
}

void VSyncController::SetEnable(bool enbale)
{
    const sptr<VSyncGenerator> generator = generator_.promote();
    if (generator == nullptr) {
        return;
    }
    if (enbale) {
        generator_->AddListener(phaseOffset_, this);
    } else {
        generator_->RemoveListener(this);
    }
    enabled_ = enbale;
}

void VSyncController::SetCallback(Callback *cb)
{
    std::lock_guard<std::mutex> locker(callbackMutex_);
    callback_ = cb;
}

void VSyncController::SetPhaseOffset(int64_t offset)
{
    std::lock_guard<std::mutex> locker(offsetMutex_);
    phaseOffset_ = offset;
    generator_->ChangePhaseOffset(this, phaseOffset_);
}

void VSyncController::OnVSyncEvent(int64_t now)
{
    if (callback_ != nullptr) {
        callback_->OnVSyncEvent(now);
    }
}
}
}
