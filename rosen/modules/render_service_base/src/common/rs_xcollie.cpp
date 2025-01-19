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

#include "common/rs_xcollie.h"
#include "platform/common/rs_log.h"
#ifdef ROSEN_OHOS
#include "xcollie/xcollie.h"
#endif
namespace OHOS {
namespace Rosen {
RSXCollie::RSXCollie(const std::string& tag, uint32_t timeoutSeconds,
    std::function<void(void*)> func, void* arg, uint32_t flag)
{
    tag_ = tag;
#ifdef ROSEN_OHOS
    id_ = HiviewDFX::XCollie::GetInstance().SetTimer(tag_, timeoutSeconds, func, arg, flag);
#else
    id_ = -1;
#endif
    RS_LOGD("start RSXCollie, tag: %{public}s, timeout: %{public}u, flag: %{public}u, id: %{public}d",
        tag.c_str(), timeoutSeconds, flag, id_);
}

void RSXCollie::CancelRSXCollie()
{
    if (!isCanceled_) {
        isCanceled_ = true;
#ifdef ROSEN_OHOS
        HiviewDFX::XCollie::GetInstance().CancelTimer(id_);
#endif
        RS_LOGD("cancel RSXCollie, tag: %{public}s, id: %{public}d", tag_.c_str(), id_);
    }
}

RSXCollie::~RSXCollie()
{
    CancelRSXCollie();
}
} // namespace Rosen
} // namespace OHOS