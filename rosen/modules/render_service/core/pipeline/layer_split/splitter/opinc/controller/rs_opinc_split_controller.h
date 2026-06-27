/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#ifndef RS_OPINC_SPLIT_CONTROLLER_H
#define RS_OPINC_SPLIT_CONTROLLER_H

#include "pipeline/layer_split/common/rs_layer_split_types.h"

namespace OHOS::Rosen {

class RequestController {
public:
    RequestController();
    void IncStayOnCount();
    void Update(bool needLeave, PlanStatus planStatus, bool canDoDirectComposition, bool isHardwareEnabled);
    void Reset();
    bool CheckNeedRequest();
    bool IsLongTermOff() const;
private:
    uint32_t stayOnCount_ = 0;
    uint32_t noDirectCount_ = 0;
    uint32_t hardwareEnabledCount_ = 0;
    uint32_t noLeaveCount_ = 0;
    uint32_t requestThreshold_ = 1;
    uint32_t stayOffCount_ = 0;
    bool doDirectExpected_ = true;
    std::deque<uint32_t> lastStayOnCounts_;
};

}

#endif