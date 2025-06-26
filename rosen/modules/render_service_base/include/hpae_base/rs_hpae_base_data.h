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

#ifndef RENDER_SERVICE_BASE_INCLUDE_HPAE_BASE_DATA_H
#define RENDER_SERVICE_BASE_INCLUDE_HPAE_BASE_DATA_H

#include <deque>
#include <mutex>
#include <optional>

#include "hpae_base/rs_hpae_base_types.h"

#include "common/rs_common_def.h"
#include "common/rs_vector2.h"
#include "common/rs_vector4.h"

namespace OHOS::Rosen {

constexpr NodeId INVALID_BLUR_NODE_ID = INVALID_NODEID; // 0

using HpaeStatus = struct HpaeStatus_ {
    bool gotHpaeBlurNode = false;
    NodeId blurNodeId = INVALID_BLUR_NODE_ID;
    bool hpaeBlurEnabled = false;

    std::optional<Vector4f> pixelStretch;
    int tileMode = 0;
    std::optional<Vector2f> greyCoef;
    float blurRadius = 0.0;
    float brightness = 0.0;
    float saturation = 0.0;
};

class RSB_EXPORT RSHpaeBaseData {
public:
    static RSHpaeBaseData &GetInstance();
    RSHpaeBaseData(const RSHpaeBaseData &) = delete;
    void operator=(const RSHpaeBaseData &) = delete;

    void Reset();
    void SetHpaeInputBuffer(HpaeBufferInfo& inputBuffer);
    void SetHpaeOutputBuffer(HpaeBufferInfo& outputBuffer);
    HpaeBufferInfo RequestHpaeInputBuffer();
    HpaeBufferInfo RequestHpaeOutputBuffer();
    void SyncHpaeStatus(HpaeStatus status);

    bool GetHasHpaeBlurNode();
    NodeId GetBlurNodeId();
    Vector4f GetPixelStretch();
    Vector2f GetGreyCoef();
    int GetTileMode();
    float GetBlurRadius();
    float GetBrightness();
    float GetSaturation();

    void SetIsFirstFrame(bool status);
    bool GetIsFirstFrame();
    bool GetNeedReset();
    void SetResetDone();

    void SetBlurContentChanged(bool status);
    bool GetBlurContentChanged();

    void NotifyBufferUsed(bool status) { bufferUsed_ = status; }
    bool GetBufferUsed() { return bufferUsed_; }

private:
    RSHpaeBaseData();
    ~RSHpaeBaseData() noexcept;

    std::deque<HpaeBufferInfo> inputBufferQueue_;
    std::deque<HpaeBufferInfo> outputBufferQueue_;
    bool isFirstFrame_ = false;
    bool needReset_ = false;
    bool blurContentChanged_ = false;

    HpaeStatus hpaeStatus_;
    bool bufferUsed_ = true;
};

} // OHOS::Rosen

#endif // RENDER_SERVICE_BASE_INCLUDE_HPAE_BASE_DATA_H