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


#include "info_collection/rs_hdr_collection.h"

#include <chrono>

namespace OHOS {
namespace Rosen {

RsHdrCollection::RsHdrCollection() {}
RsHdrCollection::~RsHdrCollection() {}

void RsHdrCollection::HandleHdrState(bool isHdrOn)
{
    std::lock_guard<std::mutex> lock(hdrOnDurationMtx_);
    if (isHdrOn_ != isHdrOn) {
        isHdrOn_ = isHdrOn;
        if (isHdrOn_) {
            hdrOnTimestamp_ = GetSysTimeMs();
        } else {
            int64_t now = GetSysTimeMs();
            hdrOnDuration_ += now > hdrOnTimestamp_ ? now - hdrOnTimestamp_ : 0;
        }
    }
}

void RsHdrCollection::ResetHdrOnDuration()
{
    std::lock_guard<std::mutex> lock(hdrOnDurationMtx_);
    hdrOnDuration_ = 0;
}

int64_t RsHdrCollection::GetHdrOnDuration()
{
    std::lock_guard<std::mutex> lock(hdrOnDurationMtx_);
    return hdrOnDuration_;
}

int64_t RsHdrCollection::GetSysTimeMs()
{
    auto now = std::chrono::steady_clock::now().time_since_epoch();
    return static_cast<int64_t>(std::chrono::duration_cast<std::chrono::milliseconds>(now).count());
}
} // namespace Rosen
} // namespace OHOS
