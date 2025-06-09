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

#ifndef RS_HARDWARE_COMPOSE_DISABLED_REASON_COLLECTION_H
#define RS_HARDWARE_COMPOSE_DISABLED_REASON_COLLECTION_H

#include <mutex>
#include <vector>

#include "common/rs_common_def.h"

namespace OHOS {
namespace Rosen {
static const size_t HWC_DISABLED_REASON_INFO_MINIMUM_SIZE = 14 * sizeof(int32_t);

enum HwcDisabledReasons {
    DISABLED_BY_FLITER_RECT = 0,
    DISABLED_BY_TRANSPARENT_CLEAN_FLITER = 1,
    DISABLED_BY_TRANSPARENT_DIRTY_FLITER = 2,
    DISABLED_BY_ACCUMULATED_ALPHA = 3,
    DISABLED_BY_ROTATION = 4,
    DISABLED_BY_HWC_NODE_BELOW_SELF_IN_APP = 5,
    DISABLED_BY_BACKGROUND_ALPHA = 6,
    DISABLED_BY_INVALID_PARAM = 7,
    DISABLED_BY_PREVALIDATE = 8,
    DISABLED_BY_SRC_PIXEL = 9,
    DISABLED_BY_BUFFER_NONMATCH = 10,
    DISABLED_BY_RENDER_HDR_SURFACE = 11,
    DISABLED_BY_SOLID_BACKGROUND_ALPHA = 12,
    DISABLED_REASON_LENGTH = 13,
};

struct HwcDisabledReasonInfo {
    int32_t disabledReasonStatistics[HwcDisabledReasons::DISABLED_REASON_LENGTH] = {0};
    int32_t pidOfBelongsApp = 0;
    std::string nodeName = "";
};

using HwcDisabledReasonInfos = std::vector<HwcDisabledReasonInfo>;

class RSB_EXPORT HwcDisabledReasonCollection {
public:
    static HwcDisabledReasonCollection& GetInstance();

    void UpdateHwcDisabledReasonForDFX(NodeId id, int32_t disabledReason, const std::string& nodeName);
    HwcDisabledReasonInfos GetHwcDisabledReasonInfo();

private:
    HwcDisabledReasonCollection();
    ~HwcDisabledReasonCollection() noexcept;
    HwcDisabledReasonCollection(const HwcDisabledReasonCollection&) = delete;
    HwcDisabledReasonCollection(const HwcDisabledReasonCollection&&) = delete;
    HwcDisabledReasonCollection& operator=(const HwcDisabledReasonCollection&) = delete;
    HwcDisabledReasonCollection& operator=(const HwcDisabledReasonCollection&&) = delete;

    std::unordered_map<NodeId, HwcDisabledReasonInfo> hwcDisabledReasonInfoMap_;
    mutable std::mutex hwcDisabledReasonMtx_;
};
} // namespace Rosen
} // namespace OHOS

#endif // RS_HARDWARE_COMPOSE_DISABLED_REASON_COLLECTION_H