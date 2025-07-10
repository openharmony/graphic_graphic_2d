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

#include "hpae_base/rs_hpae_base_data.h"
#include "hpae_base/rs_hpae_log.h"

namespace OHOS::Rosen {
RSHpaeBaseData &RSHpaeBaseData::GetInstance()
{
    static RSHpaeBaseData hpaeBaseDataInstance;
    return hpaeBaseDataInstance;
}

RSHpaeBaseData::RSHpaeBaseData()
{
}

RSHpaeBaseData::~RSHpaeBaseData() noexcept
{
}

void RSHpaeBaseData::Reset()
{
}

void RSHpaeBaseData::SyncHpaeStatus(HpaeStatus status)
{
    hpaeStatus_ = status;
}

void RSHpaeBaseData::SetHpaeInputBuffer(HpaeBufferInfo& inputBuffer)
{
    inputBufferQueue_.clear();
    inputBufferQueue_.push_back(inputBuffer);
}

void RSHpaeBaseData::SetHpaeOutputBuffer(HpaeBufferInfo& outputBuffer)
{
    outputBufferQueue_.clear();
    outputBufferQueue_.push_back(outputBuffer);
}

HpaeBufferInfo RSHpaeBaseData::RequestHpaeInputBuffer()
{
    HpaeBufferInfo result;
    HPAE_TRACE_NAME_FMT("RequestHpaeInputBuffer: %zu", inputBufferQueue_.size());
    if (!inputBufferQueue_.empty()) {
        result = inputBufferQueue_.back();
        inputBufferQueue_.clear();
    }
    return result;
}

HpaeBufferInfo RSHpaeBaseData::RequestHpaeOutputBuffer()
{
    HpaeBufferInfo result;
    HPAE_TRACE_NAME_FMT("RequestHpaeOutputBuffer: %zu", outputBufferQueue_.size());
    if (!outputBufferQueue_.empty()) {
        result = outputBufferQueue_.back();
        outputBufferQueue_.clear();
    }
    return result;
}

bool RSHpaeBaseData::GetHasHpaeBlurNode()
{
    return hpaeStatus_.gotHpaeBlurNode;
}

NodeId RSHpaeBaseData::GetBlurNodeId()
{
    return hpaeStatus_.blurNodeId;
}

Vector4f RSHpaeBaseData::GetPixelStretch()
{
    if (hpaeStatus_.pixelStretch) {
        return *hpaeStatus_.pixelStretch;
    } else {
        return Vector4f(0.f, 0.f, 0.f, 0.f);
    }
}

Vector2f RSHpaeBaseData::GetGreyCoef()
{
    if (hpaeStatus_.greyCoef) {
        return *hpaeStatus_.greyCoef;
    } else {
        return Vector2f(0.f, 0.f);
    }
}

int RSHpaeBaseData::GetTileMode()
{
    return hpaeStatus_.tileMode;
}

float RSHpaeBaseData::GetBlurRadius()
{
    return hpaeStatus_.blurRadius;
}

float RSHpaeBaseData::GetBrightness()
{
    return hpaeStatus_.brightness;
}

float RSHpaeBaseData::GetSaturation()
{
    return hpaeStatus_.saturation;
}

void RSHpaeBaseData::SetIsFirstFrame(bool status)
{
    isFirstFrame_ = status;
    if (isFirstFrame_) {
        needReset_ = true;
    }
}

bool RSHpaeBaseData::GetIsFirstFrame()
{
    return isFirstFrame_;
}

bool RSHpaeBaseData::GetNeedReset()
{
    return needReset_;
}

void RSHpaeBaseData::SetResetDone()
{
    needReset_ = false;
}

void RSHpaeBaseData::SetBlurContentChanged(bool status)
{
    blurContentChanged_ = status;
}

bool RSHpaeBaseData::GetBlurContentChanged()
{
    return blurContentChanged_;
}

} // OHOS::Rosen