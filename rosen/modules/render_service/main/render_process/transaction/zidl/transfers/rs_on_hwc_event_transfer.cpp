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

#include "rs_on_hwc_event_transfer.h"

#include "core/rs_render_pipeline_agent.h"
#include "ipc_persistence/rs_ipc_persistence_manager.h"

#include "platform/common/rs_log.h"

#undef LOG_TAG
#define LOG_TAG "OnHwcEventTransfer"

namespace OHOS {
namespace Rosen {

namespace {
constexpr size_t HWC_EVENT_DATA_SIZE_MAX = 100;
}

bool OnHwcEventInput::Marshalling(Parcel& parcel) const
{
    if (!parcel.WriteUint32(deviceId_)) {
        RS_LOGE("%{public}s: WriteUint32 deviceId failed", __func__);
        return false;
    }
    if (!parcel.WriteUint32(eventId_)) {
        RS_LOGE("%{public}s: WriteUint32 eventId failed", __func__);
        return false;
    }
    if (!parcel.WriteInt32Vector(eventData_)) {
        RS_LOGE("%{public}s: WriteInt32Vector eventData failed", __func__);
        return false;
    }
    return true;
}

std::shared_ptr<OnHwcEventInput> OnHwcEventInput::Unmarshalling(Parcel& parcel, int32_t& errCode)
{
    uint32_t deviceId;
    if (!parcel.ReadUint32(deviceId)) {
        RS_LOGE("%{public}s: ReadUint32 deviceId failed", __func__);
        errCode = ERR_INVALID_DATA;
        return nullptr;
    }
    uint32_t eventId;
    if (!parcel.ReadUint32(eventId)) {
        RS_LOGE("%{public}s: ReadUint32 eventId failed", __func__);
        errCode = ERR_INVALID_DATA;
        return nullptr;
    }
    std::vector<int32_t> eventData;
    if (!parcel.ReadInt32Vector(&eventData)) {
        RS_LOGE("%{public}s: ReadInt32Vector eventData failed", __func__);
        errCode = ERR_INVALID_DATA;
        return nullptr;
    }
    if (eventData.size() > HWC_EVENT_DATA_SIZE_MAX) {
        RS_LOGE("%{public}s: eventData size[%{public}zu] exceeds max[%{public}zu]",
            __func__, eventData.size(), HWC_EVENT_DATA_SIZE_MAX);
        errCode = ERR_INVALID_DATA;
        return nullptr;
    }
    return std::make_shared<OnHwcEventInput>(deviceId, eventId, eventData);
}

void OnHwcEventTransfer::Persist(IpcPersistenceMap& map, std::mutex& mutex)
{
    auto typeId = GetTypeId();
    std::shared_ptr<OnHwcEventTransfer> target;
    {
        std::lock_guard<std::mutex> lock(mutex);
        auto it = map.find(typeId);
        if (it == map.end()) {
            if (map.size() >= Detail::MAX_PERSIST_MAP_SIZE) {
                RS_LOGE("%{public}s: persist map full, size=%{public}zu", __func__, map.size());
                return;
            }
            map[typeId] = std::make_shared<OnHwcEventTransfer>(inputData_);
            return;
        }
        target = std::static_pointer_cast<OnHwcEventTransfer>(it->second);
    }
    if (!target) {
        RS_LOGE("%{public}s: existing transfer is nullptr", __func__);
        return;
    }
    std::lock_guard<std::mutex> targetLock(target->mutex_);
    target->inputData_ = inputData_;
}

bool OnHwcEventTransfer::ProxyMarshalling(Parcel& parcel) const
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (!inputData_) {
        return false;
    }
    return inputData_->Marshalling(parcel);
}

bool OnHwcEventTransfer::Apply(const sptr<RSRenderPipelineAgent>& agent)
{
    if (!agent || !inputData_) {
        RS_LOGE("%{public}s: agent or input is nullptr", __func__);
        return false;
    }
    agent->NotifyHwcEventToRender(
        inputData_->GetDeviceId(), inputData_->GetEventId(), inputData_->GetEventData());
    return true;
}

// Explicit ODR-use: guarantee factory registration at static init time, independent of
// whether any OnHwcEventTransfer is constructed before CreateTransferByTypeId.
[[maybe_unused]] const auto* g_onHwcEventRegistrar =
    &TransferRegistrationChecker<OnHwcEventTransfer>::registrar;

} // namespace Rosen
} // namespace OHOS
