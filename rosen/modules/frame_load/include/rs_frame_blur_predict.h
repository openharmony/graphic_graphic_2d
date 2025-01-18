/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#ifndef ROSEN_MODULE_RS_FRAME_BLUR_PREDICT_H
#define ROSEN_MODULE_RS_FRAME_BLUR_PREDICT_H

#include <string>

#include "pipeline/rs_render_node.h"
#include "variable_frame_rate/rs_variable_frame_rate.h"

namespace OHOS {
namespace Rosen {

class RsFrameBlurPredict {
public:
    static RsFrameBlurPredict& GetInstance();
    void AdjustCurrentFrameDrawLargeAreaBlurFrequencyPredictively();
    void TakeEffectBlurScene(const EventInfo& eventInfo);
    void UpdateCurrentFrameDrawLargeAreaBlurFrequencyPrecisely();
    void PredictDrawLargeAreaBlur(RSRenderNode& node);
 
private:
    RsFrameBlurPredict();
    ~RsFrameBlurPredict();
    void GetCurrentFrameDrawLargeAreaBlurPredictively();
    void GetCurrentFrameDrawLargeAreaBlurPrecisely();
    void ResetPredictDrawLargeAreaBlur();

    const std::string RS_BLUR_PREDICT_INVALID = "0";
    const std::string RS_BLUR_PREDICT_LONG = "1";
    const std::string RS_BLUR_PREDICT_SHORT = "2";
    std::atomic<bool> isValidBlurFrame_{false};
    std::atomic<bool> predictBegin_{false};
    std::pair<bool, bool> predictDrawLargeAreaBlur_ = {false, false};
};
} // namespace Rosen
} // namespace OHOS
#endif // ROSEN_MODULE_RS_FRAME_BLUR_PREDICT_H
