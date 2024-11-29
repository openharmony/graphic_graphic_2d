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


#ifndef RS_HDR_COLLECTION_H
#define RS_HDR_COLLECTION_H

#include <mutex>

#include "singleton.h"

#include "common/rs_common_def.h"

namespace OHOS {
namespace Rosen {

class RSB_EXPORT RsHdrCollection : public DelayedSingleton<RsHdrCollection> {
    DECLARE_DELAYED_SINGLETON(RsHdrCollection);

public:
    void HandleHdrState(bool isHdrOn);
    void ResetHdrOnDuration();
    int64_t GetHdrOnDuration();

private:
    int64_t GetSysTimeMs();

private:
    bool isHdrOn_ {false};
    int64_t hdrOnTimestamp_ {0};
    int64_t hdrOnDuration_ {0};
    std::mutex hdrOnDurationMtx_;
};
} // namespace Rosen
} // namespace OHOS
#endif
