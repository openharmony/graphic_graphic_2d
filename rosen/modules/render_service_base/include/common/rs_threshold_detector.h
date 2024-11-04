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
#ifndef RENDER_SERVICE_BASE_COMMON_RS_THRESHOLD_DETECTOR_H
#define RENDER_SERVICE_BASE_COMMON_RS_THRESHOLD_DETECTOR_H

#include "common/rs_common_def.h"
#include "common/rs_macros.h"

namespace OHOS {
namespace Rosen {
template<class T>
class RSB_EXPORT RSThresholdDetector final {
public:
    using DetectCallBack = std::function<void(const T value, bool isHigh)>;
    /*
     * @brief if value > thresholdHigh, callback once and high flag. Recover when value <= thresholdLow.
     * @param thresholdLow Low threshold.
     * @param thresholdHigh High threshold.
     */
    RSThresholdDetector(T thresholdLow, T thresholdHigh)
        :thresholdLow_(thresholdLow), thresholdHigh_(thresholdHigh), inThresHoldBand_(false) {}
    virtual ~RSThresholdDetector() = default;
    void Detect(const T value, const DetectCallBack& callback)
    {
        if (value > thresholdHigh_ && !inThresHoldBand_) {
            inThresHoldBand_ = true;
            callback(value, true);
        }
        if (value <= thresholdLow_ && inThresHoldBand_) {
            inThresHoldBand_ = false;
            callback(value, false);
        }
    }
private:
    T thresholdLow_;
    T thresholdHigh_;
    bool inThresHoldBand_; // used to save previous data status
};
} // namespace Rosen
} // namespace OHOS
#endif // RENDER_SERVICE_BASE_COMMON_RS_THRESHOLD_DETECTOR_H
