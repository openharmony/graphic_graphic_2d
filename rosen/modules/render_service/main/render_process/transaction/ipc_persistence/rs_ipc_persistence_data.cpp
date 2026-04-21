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

#include "rs_ipc_persistence_data.h"

#include <cstdint>

#include "platform/common/rs_log.h"
#include "rs_render_pipeline_agent.h"
#include "transaction/rs_marshalling_helper.h"

#undef LOG_TAG
#define LOG_TAG "RSIpcPersistenceData"

namespace OHOS {
namespace Rosen {
namespace {
constexpr uint32_t MAX_PID_SIZE_NUMBER = 100000;
} // namespace

// SetWaterMark
bool SetWatermarkPersistenceData::Marshalling(Parcel& parcel) const
{
    return RSMarshallingHelper::Marshalling(parcel, pid_) &&
           RSMarshallingHelper::Marshalling(parcel, name_) &&
           parcel.WriteParcelable(watermark_.get()) &&
           RSMarshallingHelper::Marshalling(parcel, success_);
}

SetWatermarkPersistenceData* SetWatermarkPersistenceData::Unmarshalling(Parcel& parcel)
{
    auto result = std::make_unique<SetWatermarkPersistenceData>();
    if (!RSMarshallingHelper::Unmarshalling(parcel, result->pid_)) {
        RS_LOGE("SetWatermarkPersistenceData::Unmarshalling: failed to read pid");
        return nullptr;
    }
    if (!RSMarshallingHelper::Unmarshalling(parcel, result->name_)) {
        RS_LOGE("SetWatermarkPersistenceData::Unmarshalling: failed to read name");
        return nullptr;
    }
    if (result->watermark_ = std::shared_ptr<Media::PixelMap>(parcel.ReadParcelable<Media::PixelMap>());
        !result->watermark_) {
        RS_LOGE("SetWatermarkPersistenceData::Unmarshalling: failed to read watermark");
        return nullptr;
    }
    if (!RSMarshallingHelper::Unmarshalling(parcel, result->success_)) {
        RS_LOGE("SetWatermarkPersistenceData::Unmarshalling: failed to read success");
        return nullptr;
    }
    return result.release();
}

void SetWatermarkPersistenceData::Apply(const sptr<RSRenderPipelineAgent>& renderPipelineAgent)
{
    if (renderPipelineAgent) {
        bool success = false;
        renderPipelineAgent->SetWatermark(pid_, name_, watermark_, success);
    }
}

// ShowWaterMark
bool ShowWatermarkPersistenceData::Marshalling(Parcel& parcel) const
{
    return parcel.WriteParcelable(watermarkImg_.get()) &&
           RSMarshallingHelper::Marshalling(parcel, isShow_);
}

ShowWatermarkPersistenceData* ShowWatermarkPersistenceData::Unmarshalling(Parcel& parcel)
{
    auto result = std::make_unique<ShowWatermarkPersistenceData>();
    if (result->watermarkImg_ = std::shared_ptr<Media::PixelMap>(parcel.ReadParcelable<Media::PixelMap>());
        !result->watermarkImg_) {
        RS_LOGE("ShowWatermarkPersistenceData::Unmarshalling: failed to read watermarkImg");
        return nullptr;
    }
    if (!RSMarshallingHelper::Unmarshalling(parcel, result->isShow_)) {
        RS_LOGE("ShowWatermarkPersistenceData::Unmarshalling: failed to read isShow");
        return nullptr;
    }
    return result.release();
}

void ShowWatermarkPersistenceData::Apply(const sptr<RSRenderPipelineAgent>& renderPipelineAgent)
{
    if (renderPipelineAgent) {
        renderPipelineAgent->ShowWatermark(watermarkImg_, isShow_);
    }
}

// OnHwcEvent
bool OnHwcEventPersistenceData::Marshalling(Parcel& parcel) const
{
    return RSMarshallingHelper::Marshalling(parcel, deviceId_) &&
           RSMarshallingHelper::Marshalling(parcel, eventId_) &&
           RSMarshallingHelper::Marshalling(parcel, eventData_);
}

OnHwcEventPersistenceData* OnHwcEventPersistenceData::Unmarshalling(Parcel& parcel)
{
    auto result = std::make_unique<OnHwcEventPersistenceData>();
    if (!RSMarshallingHelper::Unmarshalling(parcel, result->deviceId_)) {
        RS_LOGE("OnHwcEventPersistenceData::Unmarshalling: failed to read deviceId");
        return nullptr;
    }
    if (!RSMarshallingHelper::Unmarshalling(parcel, result->eventId_)) {
        RS_LOGE("OnHwcEventPersistenceData::Unmarshalling: failed to read eventId");
        return nullptr;
    }
    if (!RSMarshallingHelper::Unmarshalling(parcel, result->eventData_)) {
        RS_LOGE("OnHwcEventPersistenceData::Unmarshalling: failed to read eventData");
        return nullptr;
    }
    return result.release();
}

void OnHwcEventPersistenceData::Apply(const sptr<RSRenderPipelineAgent>& renderPipelineAgent)
{
    if (renderPipelineAgent) {
        renderPipelineAgent->NotifyHwcEventToRender(deviceId_, eventId_, eventData_);
    }
}

// SetBehindWindowFilterEnabled
bool SetBehindWindowFilterEnabledPersistenceData::Marshalling(Parcel& parcel) const
{
    return RSMarshallingHelper::Marshalling(parcel, enabled_);
}

SetBehindWindowFilterEnabledPersistenceData* SetBehindWindowFilterEnabledPersistenceData::Unmarshalling(Parcel& parcel)
{
    auto result = std::make_unique<SetBehindWindowFilterEnabledPersistenceData>();
    if (!RSMarshallingHelper::Unmarshalling(parcel, result->enabled_)) {
        RS_LOGE("SetBehindWindowFilterEnabledPersistenceData::Unmarshalling: failed to read enabled");
        return nullptr;
    }
    return result.release();
}

void SetBehindWindowFilterEnabledPersistenceData::Apply(const sptr<RSRenderPipelineAgent>& renderPipelineAgent)
{
    if (renderPipelineAgent) {
        renderPipelineAgent->SetBehindWindowFilterEnabled(enabled_);
    }
}

// SetShowRefreshRateEnabled
bool SetShowRefreshRateEnabledPersistenceData::Marshalling(Parcel& parcel) const
{
    return RSMarshallingHelper::Marshalling(parcel, enabled_) &&
           RSMarshallingHelper::Marshalling(parcel, type_);
}

SetShowRefreshRateEnabledPersistenceData* SetShowRefreshRateEnabledPersistenceData::Unmarshalling(Parcel& parcel)
{
    auto result = std::make_unique<SetShowRefreshRateEnabledPersistenceData>();
    if (!RSMarshallingHelper::Unmarshalling(parcel, result->enabled_)) {
        RS_LOGE("SetShowRefreshRateEnabledPersistenceData::Unmarshalling: failed to read enabled");
        return nullptr;
    }
    if (!RSMarshallingHelper::Unmarshalling(parcel, result->type_)) {
        RS_LOGE("SetShowRefreshRateEnabledPersistenceData::Unmarshalling: failed to read type");
        return nullptr;
    }
    return result.release();
}

void SetShowRefreshRateEnabledPersistenceData::Apply(const sptr<RSRenderPipelineAgent>& renderPipelineAgent)
{
    if (renderPipelineAgent) {
        renderPipelineAgent->SetShowRefreshRateEnabled(enabled_, type_);
    }
}

// SelfDrawingNodeRectChangeCallback
bool SelfDrawingNodeRectChangeCallbackPersistenceData::Marshalling(Parcel& parcel) const
{
    MessageParcel* message = static_cast<MessageParcel*>(&parcel);
    if (!message->WriteInt32(pid_)) {
        return false;
    }

    size_t pidsSize = constraint_.pids.size();
    if (pidsSize > UINT32_MAX) {
        RS_LOGE("%{public}s: pids.size() exceeds UINT32_MAX", __func__);
        return false;
    }
    uint32_t size = static_cast<uint32_t>(pidsSize);
    if (!message->WriteUint32(size)) {
        return false;
    }
    for (int32_t pid : constraint_.pids) {
        if (!message->WriteInt32(pid)) {
            return false;
        }
    }

    if (!message->WriteInt32(constraint_.range.lowLimit.width) ||
        !message->WriteInt32(constraint_.range.lowLimit.height) ||
        !message->WriteInt32(constraint_.range.highLimit.width) ||
        !message->WriteInt32(constraint_.range.highLimit.height)) {
        return false;
    }
    if (!message->WriteRemoteObject(callback_->AsObject())) {
        return false;
    }
    return true;
}

SelfDrawingNodeRectChangeCallbackPersistenceData* SelfDrawingNodeRectChangeCallbackPersistenceData::Unmarshalling(
    Parcel& parcel)
{
    auto result = std::make_unique<SelfDrawingNodeRectChangeCallbackPersistenceData>();
    MessageParcel* message = static_cast<MessageParcel*>(&parcel);
    if (!message->ReadInt32(result->pid_)) {
        RS_LOGE("SelfDrawingNodeRectChangeCallbackPersistenceData::Unmarshalling: failed to read pid");
        return nullptr;
    }
    uint32_t size;
    if (!message->ReadUint32(size)) {
        RS_LOGE("SelfDrawingNodeRectChangeCallbackPersistenceData::Unmarshalling: failed to read pids size");
        return nullptr;
    }
    if (size > MAX_PID_SIZE_NUMBER) {
        RS_LOGE("SelfDrawingNodeRectChangeCallbackPersistenceData::Unmarshalling: pids size exceeds MAX_PID_SIZE_NUMBER");
        return nullptr;
    }
    for (uint32_t i = 0; i < size; ++i) {
        pid_t pid;
        if (!message->ReadInt32(pid)) {
            RS_LOGE("SelfDrawingNodeRectChangeCallbackPersistenceData::Unmarshalling: failed to read pid in pids");
            return nullptr;
        }
        result->constraint_.pids.insert(pid);
    }
    if (!message->ReadInt32(result->constraint_.range.lowLimit.width) ||
        !message->ReadInt32(result->constraint_.range.lowLimit.height) ||
        !message->ReadInt32(result->constraint_.range.highLimit.width) ||
        !message->ReadInt32(result->constraint_.range.highLimit.height)) {
        RS_LOGE("SelfDrawingNodeRectChangeCallbackPersistenceData::Unmarshalling: failed to read range");
        return nullptr;
    }

    auto remoteObject = message->ReadRemoteObject();
    if (remoteObject == nullptr) {
        RS_LOGE("SelfDrawingNodeRectChangeCallbackPersistenceData::Unmarshalling: failed to read remoteObject");
        return nullptr;
    }
    result->callback_ = iface_cast<RSISelfDrawingNodeRectChangeCallback>(remoteObject);
    if (result->callback_ == nullptr) {
        RS_LOGE("SelfDrawingNodeRectChangeCallbackPersistenceData::Unmarshalling: failed to cast callback");
        return nullptr;
    }
    return result.release();
}

void SelfDrawingNodeRectChangeCallbackPersistenceData::Apply(const sptr<RSRenderPipelineAgent>& renderPipelineAgent)
{
    if (renderPipelineAgent) {
        renderPipelineAgent->RegisterSelfDrawingNodeRectChangeCallback(pid_, constraint_, callback_);
    }
}
} // namespace Rosen
} // namespace OHOS
