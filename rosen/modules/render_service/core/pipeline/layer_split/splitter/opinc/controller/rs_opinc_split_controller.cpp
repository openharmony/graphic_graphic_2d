
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

#include "rs_opinc_split_controller.h"
#include "pipeline/main_thread/rs_main_thread.h"
#include "pipeline/render_thread/rs_uni_render_processor.h"
#include "drawable/rs_render_node_drawable.h"
#include "drawable/rs_screen_render_node_drawable.h"
#include "params/rs_screen_render_params.h"
#include "screen_manager/rs_screen_manager.h"

namespace OHOS::Rosen {
namespace {
constexpr uint32_t MAX_STAY_ON_HISTORY_SIZE = 3;
constexpr uint32_t MIN_STAY_ON_HISTORY_FOR_EVAL = 3;
constexpr uint32_t SHORT_STAY_ON_THRESHOLD = 5;
constexpr uint32_t REQUEST_THRESHOLD_MIN = 1;
constexpr uint32_t REQUEST_THRESHOLD_MAX = 512;
constexpr uint32_t THRESHOLD_MULTIPLY_FACTOR = 2;
constexpr float NO_DIRECT_RATIO_THRESHOLD = 0.5f;
constexpr float HARDWARE_ENABLED_RATIO = 0.7f;
constexpr uint32_t LONG_TERM_OFF_THRESHOLD = 100;
} // namespace

RequestController::RequestController()
{
}

void RequestController::IncStayOnCount()
{
    ++stayOnCount_;
}

void RequestController::Update(
    bool needLeave, PlanStatus planStatus, bool canDoDirectComposition, bool isHardwareEnabled)
{
    noLeaveCount_ = needLeave ? 0 : noLeaveCount_ + 1;
    if (planStatus == PlanStatus::OFF) {
        ++stayOffCount_;
    } else if (planStatus == PlanStatus::PREPARE) {
        stayOffCount_ = 0;
    } else if (planStatus == PlanStatus::ON) {
        ++noDirectCount_;
        if (isHardwareEnabled) {
            ++hardwareEnabledCount_;
        }

        if (canDoDirectComposition) {
            doDirectExpected_ = false;
        }
    } else if (planStatus == PlanStatus::LEAVE) {
        lastStayOnCounts_.push_back(stayOnCount_);
        if (lastStayOnCounts_.size() > MAX_STAY_ON_HISTORY_SIZE) {
            lastStayOnCounts_.pop_front();
        }
        if (lastStayOnCounts_.size() == MIN_STAY_ON_HISTORY_FOR_EVAL) {
            uint32_t totalStayOnCount = 0;
            for (uint32_t count : lastStayOnCounts_) {
                totalStayOnCount += count;
            }
            if (totalStayOnCount / MIN_STAY_ON_HISTORY_FOR_EVAL <= SHORT_STAY_ON_THRESHOLD) {
                requestThreshold_ *= THRESHOLD_MULTIPLY_FACTOR;
            }
        }

        if (!doDirectExpected_) {
            requestThreshold_ *= THRESHOLD_MULTIPLY_FACTOR;
        }

        if (noDirectCount_ > stayOnCount_ * NO_DIRECT_RATIO_THRESHOLD &&
            hardwareEnabledCount_ < noDirectCount_ * HARDWARE_ENABLED_RATIO) {
            requestThreshold_ *= THRESHOLD_MULTIPLY_FACTOR;
        } else {
            requestThreshold_ /= THRESHOLD_MULTIPLY_FACTOR;
        }

        requestThreshold_ = std::clamp(requestThreshold_, REQUEST_THRESHOLD_MIN, REQUEST_THRESHOLD_MAX);

        LAYER_SPLIT_LOGD("RequestController::Update requestThreshold_=%{public}u", requestThreshold_);
        stayOnCount_ = 0;
        noDirectCount_ = 0;
        hardwareEnabledCount_ = 0;
        doDirectExpected_ = true;
    }
}

bool RequestController::IsLongTermOff() const
{
    return stayOffCount_ > LONG_TERM_OFF_THRESHOLD;
}

void RequestController::Reset()
{
    requestThreshold_ = 1;
    stayOffCount_ = 0;
    lastStayOnCounts_.clear();
}

bool RequestController::CheckNeedRequest()
{
    return noLeaveCount_ >= requestThreshold_;
}

}