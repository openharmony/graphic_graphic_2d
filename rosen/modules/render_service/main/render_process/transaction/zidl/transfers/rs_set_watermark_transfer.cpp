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

#include "rs_set_watermark_transfer.h"

#include <algorithm>

#include "core/rs_render_pipeline_agent.h"
#include "errors.h"
#include "ipc_persistence/rs_ipc_persistence_manager.h"

#include "platform/common/rs_log.h"
#include "platform/common/rs_system_properties.h"

#undef LOG_TAG
#define LOG_TAG "SetWatermarkTransfer"

namespace OHOS {
namespace Rosen {

namespace {
constexpr uint32_t MAX_WATERMARK_GRID_COUNT = 255;
constexpr size_t MAX_WATERMARK_ENTRIES = 100;
}

bool SetWatermarkInput::Marshalling(Parcel& parcel) const
{
    if (!parcel.WriteInt32(pid_)) {
        RS_LOGE("%{public}s: WriteInt32 pid failed", __func__);
        return false;
    }
    if (!parcel.WriteString(name_)) {
        RS_LOGE("%{public}s: WriteString name failed", __func__);
        return false;
    }
    if (!parcel.WriteParcelable(watermark_.get())) {
        RS_LOGE("%{public}s: WriteParcelable watermark failed", __func__);
        return false;
    }
    if (!parcel.WriteUint32(rowCount_)) {
        RS_LOGE("%{public}s: WriteUint32 rowCount failed", __func__);
        return false;
    }
    if (!parcel.WriteUint32(colCount_)) {
        RS_LOGE("%{public}s: WriteUint32 colCount failed", __func__);
        return false;
    }
    return true;
}

std::shared_ptr<SetWatermarkInput> SetWatermarkInput::Unmarshalling(Parcel& parcel, int32_t& errCode)
{
    pid_t pid;
    if (!parcel.ReadInt32(pid)) {
        RS_LOGE("%{public}s: ReadInt32 pid failed", __func__);
        errCode = ERR_INVALID_DATA;
        return nullptr;
    }
    std::string name;
    if (!parcel.ReadString(name)) {
        RS_LOGE("%{public}s: ReadString name failed", __func__);
        errCode = ERR_INVALID_DATA;
        return nullptr;
    }
    auto watermark = std::shared_ptr<Media::PixelMap>(parcel.ReadParcelable<Media::PixelMap>());
    if (!watermark) {
        RS_LOGE("%{public}s: ReadParcelable watermark failed", __func__);
        errCode = ERR_INVALID_DATA;
        return nullptr;
    }
    uint32_t rowCount;
    if (!parcel.ReadUint32(rowCount)) {
        RS_LOGE("%{public}s: ReadUint32 rowCount failed", __func__);
        errCode = ERR_INVALID_DATA;
        return nullptr;
    }
    uint32_t colCount;
    if (!parcel.ReadUint32(colCount)) {
        RS_LOGE("%{public}s: ReadUint32 colCount failed", __func__);
        errCode = ERR_INVALID_DATA;
        return nullptr;
    }
    if (rowCount > MAX_WATERMARK_GRID_COUNT || colCount > MAX_WATERMARK_GRID_COUNT) {
        RS_LOGE("%{public}s: rowCount[%{public}u] or colCount[%{public}u] out of range",
            __func__, rowCount, colCount);
        errCode = ERR_INVALID_DATA;
        return nullptr;
    }
    return std::make_shared<SetWatermarkInput>(pid, name, watermark, rowCount, colCount);
}

void SetWatermarkTransfer::Persist(IpcPersistenceMap& map, std::mutex& mutex)
{
    auto typeId = GetTypeId();
    std::shared_ptr<SetWatermarkTransfer> target;
    {
        std::lock_guard<std::mutex> lock(mutex);
        auto it = map.find(typeId);
        if (it == map.end()) {
            if (map.size() >= Detail::MAX_PERSIST_MAP_SIZE) {
                RS_LOGE("%{public}s: persist map full, size=%{public}zu", __func__, map.size());
                return;
            }
            map[typeId] = std::make_shared<SetWatermarkTransfer>(inputs_);
            return;
        }
        target = std::static_pointer_cast<SetWatermarkTransfer>(it->second);
    }
    if (!target) {
        RS_LOGE("%{public}s: existing transfer is nullptr", __func__);
        return;
    }
    std::lock_guard<std::mutex> targetLock(target->mutex_);
    for (const auto& [pid, input] : inputs_) {
        if (target->inputs_.size() >= MAX_WATERMARK_ENTRIES) {
            RS_LOGE("%{public}s: inputs_ map full, size=%{public}zu", __func__, target->inputs_.size());
            break;
        }
        target->inputs_[pid] = input;
    }
}

void SetWatermarkTransfer::ClearPid(pid_t pid)
{
    std::lock_guard<std::mutex> lock(mutex_);
    inputs_.erase(pid);
}

bool SetWatermarkTransfer::Apply(const sptr<RSRenderPipelineAgent>& agent)
{
    if (!RSSystemProperties::GetSurfaceNodeWatermarkEnabled()) {
        RS_LOGI("%{public}s: watermark disabled by system property", __func__);
        return true;
    }
    if (!agent) {
        RS_LOGE("%{public}s: agent is nullptr", __func__);
        return false;
    }
    bool allSuccess = true;
    for (const auto& [pid, input] : inputs_) {
        if (!input) {
            RS_LOGE("%{public}s: input is nullptr for pid %{public}d", __func__, pid);
            allSuccess = false;
            continue;
        }
        bool success = false;
        ErrCode ret = agent->SetWatermark(pid, input->GetName(), input->GetWatermark(),
            success, input->GetRowCount(), input->GetColCount());
        if (ret != ERR_OK || !success) {
            RS_LOGE("%{public}s: SetWatermark failed, pid=%{public}d ret=%{public}d success=%{public}d",
                __func__, pid, ret, success);
            allSuccess = false;
        }
    }
    return allSuccess;
}

bool SetWatermarkTransfer::ProxyMarshalling(Parcel& parcel) const
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (!parcel.WriteUint32(static_cast<uint32_t>(inputs_.size()))) {
        RS_LOGE("%{public}s: WriteUint32 count failed", __func__);
        return false;
    }
    for (const auto& [pid, input] : inputs_) {
        if (!input || !input->Marshalling(parcel)) {
            RS_LOGE("%{public}s: Input Marshalling failed for pid %{public}d", __func__, pid);
            return false;
        }
    }
    return true;
}

std::shared_ptr<SetWatermarkTransfer> SetWatermarkTransfer::StubUnmarshalling(
    Parcel& parcel, uint32_t maxEntries, int32_t& errCode)
{
    const uint32_t cap = std::min(maxEntries, static_cast<uint32_t>(MAX_WATERMARK_ENTRIES));
    uint32_t size = 0;
    if (!parcel.ReadUint32(size) || size > cap) {
        RS_LOGE("%{public}s: count %{public}u exceeds max %{public}u", __func__, size, cap);
        errCode = ERR_INVALID_DATA;
        return nullptr;
    }
    std::map<pid_t, std::shared_ptr<SetWatermarkInput>> inputs;
    for (uint32_t i = 0; i < size; ++i) {
        auto input = SetWatermarkInput::Unmarshalling(parcel, errCode);
        if (!input) {
            return nullptr;
        }
        inputs[input->GetPid()] = input;
    }
    return std::make_shared<SetWatermarkTransfer>(std::move(inputs));
}

// Explicit ODR-use: guarantee factory registration at static init time, independent of
// whether any SetWatermarkTransfer is constructed before CreateTransferByTypeId.
[[maybe_unused]] const auto* g_setWatermarkRegistrar =
    &TransferRegistrationChecker<SetWatermarkTransfer>::registrar;

} // namespace Rosen
} // namespace OHOS
