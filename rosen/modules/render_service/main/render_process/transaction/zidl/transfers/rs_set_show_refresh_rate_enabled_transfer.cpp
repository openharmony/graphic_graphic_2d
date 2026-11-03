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

#include "rs_set_show_refresh_rate_enabled_transfer.h"

#include "core/rs_render_pipeline_agent.h"
#include "ipc_persistence/rs_ipc_persistence_manager.h"

#include "platform/common/rs_log.h"

#undef LOG_TAG
#define LOG_TAG "SetShowRefreshRateEnabledTransfer"

namespace OHOS {
namespace Rosen {

bool SetShowRefreshRateEnabledInput::Marshalling(Parcel& parcel) const
{
    if (!parcel.WriteBool(enabled_)) {
        RS_LOGE("%{public}s: WriteBool enabled failed", __func__);
        return false;
    }
    if (!parcel.WriteInt32(type_)) {
        RS_LOGE("%{public}s: WriteInt32 type failed", __func__);
        return false;
    }
    return true;
}

std::shared_ptr<SetShowRefreshRateEnabledInput> SetShowRefreshRateEnabledInput::Unmarshalling(
    Parcel& parcel, int32_t& errCode)
{
    bool enabled;
    if (!parcel.ReadBool(enabled)) {
        RS_LOGE("%{public}s: ReadBool enabled failed", __func__);
        errCode = ERR_INVALID_DATA;
        return nullptr;
    }
    int32_t type;
    if (!parcel.ReadInt32(type)) {
        RS_LOGE("%{public}s: ReadInt32 type failed", __func__);
        errCode = ERR_INVALID_DATA;
        return nullptr;
    }
    return std::make_shared<SetShowRefreshRateEnabledInput>(enabled, type);
}

void SetShowRefreshRateEnabledTransfer::Persist(IpcPersistenceMap& map, std::mutex& mutex)
{
    auto typeId = GetTypeId();
    std::shared_ptr<SetShowRefreshRateEnabledTransfer> target;
    {
        std::lock_guard<std::mutex> lock(mutex);
        auto it = map.find(typeId);
        if (it == map.end()) {
            if (map.size() >= Detail::MAX_PERSIST_MAP_SIZE) {
                RS_LOGE("%{public}s: persist map full, size=%{public}zu", __func__, map.size());
                return;
            }
            map[typeId] = std::make_shared<SetShowRefreshRateEnabledTransfer>(inputData_);
            return;
        }
        target = std::static_pointer_cast<SetShowRefreshRateEnabledTransfer>(it->second);
    }
    if (!target) {
        RS_LOGE("%{public}s: existing transfer is nullptr", __func__);
        return;
    }
    std::lock_guard<std::mutex> targetLock(target->mutex_);
    target->inputData_ = inputData_;
}

bool SetShowRefreshRateEnabledTransfer::ProxyMarshalling(Parcel& parcel) const
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (!inputData_) {
        return false;
    }
    return inputData_->Marshalling(parcel);
}

bool SetShowRefreshRateEnabledTransfer::Apply(const sptr<RSRenderPipelineAgent>& agent)
{
    if (!agent || !inputData_) {
        RS_LOGE("%{public}s: agent or input is nullptr", __func__);
        return false;
    }
    agent->SetShowRefreshRateEnabled(inputData_->GetEnabled(), inputData_->GetType());
    SetReplyData(std::make_shared<RSIpcEmptyReply>());
    return true;
}

// Explicit ODR-use: guarantee factory registration at static init time, independent of
// whether any SetShowRefreshRateEnabledTransfer is constructed before CreateTransferByTypeId.
[[maybe_unused]] const auto* g_setShowRefreshRateRegistrar =
    &TransferRegistrationChecker<SetShowRefreshRateEnabledTransfer>::registrar;

} // namespace Rosen
} // namespace OHOS
