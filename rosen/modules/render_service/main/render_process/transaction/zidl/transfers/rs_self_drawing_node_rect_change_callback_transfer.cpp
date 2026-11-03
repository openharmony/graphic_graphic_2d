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

#include "rs_self_drawing_node_rect_change_callback_transfer.h"

#include <algorithm>
#include <cstdint>

#include "core/rs_render_pipeline_agent.h"
#include "ipc_persistence/rs_ipc_persistence_manager.h"
#include "message_parcel.h"

#include "common/rs_self_draw_rect_change_callback_constraint.h"
#include "platform/common/rs_log.h"

#undef LOG_TAG
#define LOG_TAG "SelfDrawingNodeRectChangeCallbackTransfer"

namespace OHOS {
namespace Rosen {

namespace {
constexpr uint32_t MAX_PID_SIZE_NUMBER = 100000;
constexpr size_t MAX_CALLBACK_INPUTS = 100;
}

bool SelfDrawingNodeRectChangeCallbackInput::Marshalling(Parcel& parcel) const
{
    if (!callback_) {
        RS_LOGE("%{public}s: callback is nullptr", __func__);
        return false;
    }
    if (!parcel.WriteInt32(pid_)) {
        RS_LOGE("%{public}s: WriteInt32 pid failed", __func__);
        return false;
    }
    const auto& pids = constraint_.pids;
    uint32_t pidsSize = static_cast<uint32_t>(pids.size());
    if (pidsSize > MAX_PID_SIZE_NUMBER) {
        RS_LOGE("%{public}s: pids size %{public}u exceeds max", __func__, pidsSize);
        return false;
    }
    if (!parcel.WriteUint32(pidsSize)) {
        RS_LOGE("%{public}s: WriteUint32 pids size failed", __func__);
        return false;
    }
    for (int32_t p : pids) {
        if (!parcel.WriteInt32(p)) {
            RS_LOGE("%{public}s: WriteInt32 pid in constraint failed", __func__);
            return false;
        }
    }
    if (!parcel.WriteInt32(constraint_.range.lowLimit.width) ||
        !parcel.WriteInt32(constraint_.range.lowLimit.height) ||
        !parcel.WriteInt32(constraint_.range.highLimit.width) ||
        !parcel.WriteInt32(constraint_.range.highLimit.height)) {
        RS_LOGE("%{public}s: Write rectRange failed", __func__);
        return false;
    }
    if (!static_cast<MessageParcel*>(&parcel)->WriteRemoteObject(callback_->AsObject())) {
        RS_LOGE("%{public}s: WriteRemoteObject callback failed", __func__);
        return false;
    }
    return true;
}

std::shared_ptr<SelfDrawingNodeRectChangeCallbackInput> SelfDrawingNodeRectChangeCallbackInput::Unmarshalling(
    Parcel& parcel, int32_t& errCode)
{
    pid_t pid;
    if (!parcel.ReadInt32(pid)) {
        RS_LOGE("%{public}s: ReadInt32 pid failed", __func__);
        errCode = ERR_INVALID_DATA;
        return nullptr;
    }
    uint32_t pidsSize;
    if (!parcel.ReadUint32(pidsSize) || pidsSize > MAX_PID_SIZE_NUMBER) {
        RS_LOGE("%{public}s: pids size exceeds max", __func__);
        errCode = ERR_INVALID_DATA;
        return nullptr;
    }
    RectConstraint constraint;
    for (uint32_t i = 0; i < pidsSize; ++i) {
        pid_t pidValue;
        if (!parcel.ReadInt32(pidValue)) {
            RS_LOGE("%{public}s: ReadInt32 pid[%{public}u] failed", __func__, i);
            errCode = ERR_INVALID_DATA;
            return nullptr;
        }
        constraint.pids.insert(pidValue);
    }
    if (!parcel.ReadInt32(constraint.range.lowLimit.width) ||
        !parcel.ReadInt32(constraint.range.lowLimit.height) ||
        !parcel.ReadInt32(constraint.range.highLimit.width) ||
        !parcel.ReadInt32(constraint.range.highLimit.height)) {
        RS_LOGE("%{public}s: Read rectRange failed", __func__);
        errCode = ERR_INVALID_DATA;
        return nullptr;
    }
    auto remoteObject = static_cast<MessageParcel*>(&parcel)->ReadRemoteObject();
    if (remoteObject == nullptr) {
        RS_LOGE("%{public}s: ReadRemoteObject failed", __func__);
        errCode = ERR_NULL_OBJECT;
        return nullptr;
    }
    auto callback = iface_cast<RSISelfDrawingNodeRectChangeCallback>(remoteObject);
    if (callback == nullptr) {
        RS_LOGE("%{public}s: iface_cast failed", __func__);
        errCode = ERR_NULL_OBJECT;
        return nullptr;
    }
    return std::make_shared<SelfDrawingNodeRectChangeCallbackInput>(pid, constraint, callback);
}

bool SelfDrawingNodeRectChangeCallbackReply::Marshalling(Parcel& parcel) const
{
    if (!parcel.WriteInt32(result_)) {
        RS_LOGE("%{public}s: WriteInt32 result failed", __func__);
        return false;
    }
    return true;
}

bool SelfDrawingNodeRectChangeCallbackReply::Unmarshalling(Parcel& parcel)
{
    if (!parcel.ReadInt32(result_)) {
        RS_LOGE("%{public}s: ReadInt32 result failed", __func__);
        return false;
    }
    return true;
}

void SelfDrawingNodeRectChangeCallbackTransfer::Persist(IpcPersistenceMap& map, std::mutex& mutex)
{
    auto typeId = GetTypeId();
    std::shared_ptr<SelfDrawingNodeRectChangeCallbackTransfer> target;
    {
        std::lock_guard<std::mutex> lock(mutex);
        auto it = map.find(typeId);
        if (it == map.end()) {
            if (map.size() >= Detail::MAX_PERSIST_MAP_SIZE) {
                RS_LOGE("%{public}s: persist map full, size=%{public}zu", __func__, map.size());
                return;
            }
            map[typeId] = std::make_shared<SelfDrawingNodeRectChangeCallbackTransfer>(inputs_);
            return;
        }
        target = std::static_pointer_cast<SelfDrawingNodeRectChangeCallbackTransfer>(it->second);
    }
    if (!target) {
        RS_LOGE("%{public}s: existing transfer is nullptr", __func__);
        return;
    }
    std::lock_guard<std::mutex> targetLock(target->mutex_);
    for (const auto& [pid, input] : inputs_) {
        if (target->inputs_.size() >= MAX_CALLBACK_INPUTS) {
            RS_LOGE("%{public}s: inputs_ map full, size=%{public}zu", __func__, target->inputs_.size());
            break;
        }
        target->inputs_[pid] = input;
    }
}

void SelfDrawingNodeRectChangeCallbackTransfer::ClearPid(pid_t pid)
{
    std::lock_guard<std::mutex> lock(mutex_);
    inputs_.erase(pid);
}

bool SelfDrawingNodeRectChangeCallbackTransfer::Apply(const sptr<RSRenderPipelineAgent>& agent)
{
    if (!agent) {
        RS_LOGE("%{public}s: agent is nullptr", __func__);
        return false;
    }
    int32_t result = ERR_OK;
    for (const auto& [pid, input] : inputs_) {
        if (!input || !input->GetCallback()) {
            RS_LOGE("%{public}s: input or callback is nullptr for pid %{public}d", __func__, pid);
            continue;
        }
        int32_t ret = agent->RegisterSelfDrawingNodeRectChangeCallback(
            pid, input->GetConstraint(), input->GetCallback());
        if (ret != ERR_OK && result == ERR_OK) {
            result = ret;
        }
    }
    SetReplyData(std::make_shared<SelfDrawingNodeRectChangeCallbackReply>(result));
    return true;
}

bool SelfDrawingNodeRectChangeCallbackTransfer::ProxyMarshalling(Parcel& parcel) const
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

std::shared_ptr<SelfDrawingNodeRectChangeCallbackTransfer>
SelfDrawingNodeRectChangeCallbackTransfer::StubUnmarshalling(Parcel& parcel, uint32_t maxEntries, int32_t& errCode)
{
    const uint32_t cap = std::min(maxEntries, static_cast<uint32_t>(MAX_CALLBACK_INPUTS));
    uint32_t size = 0;
    if (!parcel.ReadUint32(size) || size > cap) {
        RS_LOGE("%{public}s: count %{public}u exceeds max %{public}u", __func__, size, cap);
        errCode = ERR_INVALID_DATA;
        return nullptr;
    }
    std::map<pid_t, std::shared_ptr<SelfDrawingNodeRectChangeCallbackInput>> inputs;
    for (uint32_t i = 0; i < size; ++i) {
        auto input = SelfDrawingNodeRectChangeCallbackInput::Unmarshalling(parcel, errCode);
        if (!input) {
            return nullptr;
        }
        inputs[input->GetPid()] = input;
    }
    return std::make_shared<SelfDrawingNodeRectChangeCallbackTransfer>(std::move(inputs));
}

bool UnRegisterSelfDrawingNodeRectChangeCallbackInput::Marshalling(Parcel& parcel) const
{
    if (!parcel.WriteInt32(pid_)) {
        RS_LOGE("%{public}s: WriteInt32 pid failed", __func__);
        return false;
    }
    return true;
}

std::shared_ptr<UnRegisterSelfDrawingNodeRectChangeCallbackInput>
UnRegisterSelfDrawingNodeRectChangeCallbackInput::Unmarshalling(Parcel& parcel, int32_t& errCode)
{
    pid_t pid;
    if (!parcel.ReadInt32(pid)) {
        RS_LOGE("%{public}s: ReadInt32 pid failed", __func__);
        errCode = ERR_INVALID_DATA;
        return nullptr;
    }
    return std::make_shared<UnRegisterSelfDrawingNodeRectChangeCallbackInput>(pid);
}

bool UnRegisterSelfDrawingNodeRectChangeCallbackReply::Marshalling(Parcel& parcel) const
{
    if (!parcel.WriteInt32(result_)) {
        RS_LOGE("%{public}s: WriteInt32 result failed", __func__);
        return false;
    }
    return true;
}

bool UnRegisterSelfDrawingNodeRectChangeCallbackReply::Unmarshalling(Parcel& parcel)
{
    if (!parcel.ReadInt32(result_)) {
        RS_LOGE("%{public}s: ReadInt32 result failed", __func__);
        return false;
    }
    return true;
}

void UnRegisterSelfDrawingNodeRectChangeCallbackTransfer::Persist(IpcPersistenceMap& map, std::mutex& mutex)
{
    constexpr auto registerTypeId =
        RSIServiceToRenderConnectionInterfaceCode::REGISTER_SELF_DRAWING_NODE_RECT_CHANGE_CALLBACK;
    if (!inputData_) {
        RS_LOGE("%{public}s: inputData_ is nullptr", __func__);
        return;
    }
    std::shared_ptr<RSIpcTransferBase> target;
    {
        std::lock_guard<std::mutex> lock(mutex);
        auto it = map.find(registerTypeId);
        if (it == map.end()) {
            return;
        }
        target = it->second;
    }
    if (target) {
        target->ClearPid(inputData_->GetPid());
    }
}

bool UnRegisterSelfDrawingNodeRectChangeCallbackTransfer::ProxyMarshalling(Parcel& parcel) const
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (!inputData_) {
        return false;
    }
    return inputData_->Marshalling(parcel);
}

bool UnRegisterSelfDrawingNodeRectChangeCallbackTransfer::Apply(const sptr<RSRenderPipelineAgent>& agent)
{
    if (!agent || !inputData_) {
        RS_LOGE("%{public}s: agent or input is nullptr", __func__);
        return false;
    }
    auto result = agent->UnRegisterSelfDrawingNodeRectChangeCallback(inputData_->GetPid());
    SetReplyData(std::make_shared<UnRegisterSelfDrawingNodeRectChangeCallbackReply>(result));
    return true;
}

// Explicit ODR-use: guarantee factory registration at static init time, independent of
// whether any SelfDrawing Transfer is constructed before CreateTransferByTypeId.
[[maybe_unused]] const auto* g_selfDrawingRegisterRegistrar =
    &TransferRegistrationChecker<SelfDrawingNodeRectChangeCallbackTransfer>::registrar;
[[maybe_unused]] const auto* g_selfDrawingUnRegisterRegistrar =
    &TransferRegistrationChecker<UnRegisterSelfDrawingNodeRectChangeCallbackTransfer>::registrar;

} // namespace Rosen
} // namespace OHOS
