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

#ifndef RENDER_SERVICE_MAIN_RENDER_PROCESS_TRANSACTION_ZIDL_TRANSFERS_RS_SELF_DRAWING_NODE_RECT_CHANGE_CALLBACK_TRANSFER_H
#define RENDER_SERVICE_MAIN_RENDER_PROCESS_TRANSACTION_ZIDL_TRANSFERS_RS_SELF_DRAWING_NODE_RECT_CHANGE_CALLBACK_TRANSFER_H

#include <cstdint>
#include <map>
#include <memory>
#include <sys/types.h>

#include <parcel.h>

#include "rs_ipc_persistence_def.h"
#include "rs_ipc_persistence_manager.h"
#include "ipc_callbacks/rs_iself_drawing_node_rect_change_callback.h"
#include "common/rs_self_draw_rect_change_callback_constraint.h"

namespace OHOS {
namespace Rosen {

class SelfDrawingNodeRectChangeCallbackInput
    : public RSIpcPersistenceDataBase<SelfDrawingNodeRectChangeCallbackInput> {
public:
    SelfDrawingNodeRectChangeCallbackInput(pid_t pid, const RectConstraint& constraint,
                                            sptr<RSISelfDrawingNodeRectChangeCallback> callback)
        : pid_(pid), constraint_(constraint), callback_(callback) {}

    pid_t GetPid() const { return pid_; }
    const RectConstraint& GetConstraint() const { return constraint_; }
    const sptr<RSISelfDrawingNodeRectChangeCallback>& GetCallback() const { return callback_; }

    bool Marshalling(Parcel& parcel) const override;
    [[nodiscard]] static std::shared_ptr<SelfDrawingNodeRectChangeCallbackInput> Unmarshalling(
        Parcel& parcel, int32_t& errCode);

    SelfDrawingNodeRectChangeCallbackInput(const SelfDrawingNodeRectChangeCallbackInput&) = delete;
    SelfDrawingNodeRectChangeCallbackInput& operator=(const SelfDrawingNodeRectChangeCallbackInput&) = delete;
    SelfDrawingNodeRectChangeCallbackInput(SelfDrawingNodeRectChangeCallbackInput&&) = delete;
    SelfDrawingNodeRectChangeCallbackInput& operator=(SelfDrawingNodeRectChangeCallbackInput&&) = delete;

private:
    pid_t pid_;
    RectConstraint constraint_;
    sptr<RSISelfDrawingNodeRectChangeCallback> callback_;
};

class SelfDrawingNodeRectChangeCallbackReply
    : public RSIpcPersistenceReplyBase<SelfDrawingNodeRectChangeCallbackReply> {
public:
    explicit SelfDrawingNodeRectChangeCallbackReply(int32_t result = Detail::REPLY_RESULT_PENDING)
        : result_(result) {}

    int32_t GetResult() const { return result_; }

    bool Marshalling(Parcel& parcel) const override;
    bool Unmarshalling(Parcel& parcel) override;

    SelfDrawingNodeRectChangeCallbackReply(const SelfDrawingNodeRectChangeCallbackReply&) = delete;
    SelfDrawingNodeRectChangeCallbackReply& operator=(const SelfDrawingNodeRectChangeCallbackReply&) = delete;
    SelfDrawingNodeRectChangeCallbackReply(SelfDrawingNodeRectChangeCallbackReply&&) = delete;
    SelfDrawingNodeRectChangeCallbackReply& operator=(SelfDrawingNodeRectChangeCallbackReply&&) = delete;

private:
    int32_t result_ = Detail::REPLY_RESULT_PENDING;
};

class SelfDrawingNodeRectChangeCallbackTransfer final
    : public RSIpcTransferCRTP<SelfDrawingNodeRectChangeCallbackTransfer,
                                SelfDrawingNodeRectChangeCallbackInput,
                                SelfDrawingNodeRectChangeCallbackReply> {
public:
    explicit SelfDrawingNodeRectChangeCallbackTransfer(
        std::map<pid_t, std::shared_ptr<SelfDrawingNodeRectChangeCallbackInput>> inputs)
        : inputs_(std::move(inputs)) {}

    static constexpr RSIServiceToRenderConnectionInterfaceCode TypeId =
        RSIServiceToRenderConnectionInterfaceCode::REGISTER_SELF_DRAWING_NODE_RECT_CHANGE_CALLBACK;
    RSIServiceToRenderConnectionInterfaceCode GetTypeId() const override { return TypeId; }
    bool IsPersistent() const override { return true; }
    bool IsSync() const override { return true; }
    FanoutPolicy GetFanoutPolicy() const override { return FanoutPolicy::FAIL_FAST; }
    int32_t GetReplyResult() const override
    {
        std::lock_guard<std::mutex> lock(mutex_);
        return replyData_ ? replyData_->GetResult() : Detail::REPLY_RESULT_PENDING;
    }

    bool Apply(const sptr<RSRenderPipelineAgent>& agent) override;

    bool ProxyMarshalling(Parcel& parcel) const override;
    [[nodiscard]] static std::shared_ptr<SelfDrawingNodeRectChangeCallbackTransfer> StubUnmarshalling(
        Parcel& parcel, uint32_t maxEntries, int32_t& errCode);
    std::shared_ptr<RSIpcTransferBase> CopyTransfer() const override
    {
        return std::make_shared<SelfDrawingNodeRectChangeCallbackTransfer>(inputs_);
    }
    void ClearPid(pid_t pid) override;

    SelfDrawingNodeRectChangeCallbackTransfer(const SelfDrawingNodeRectChangeCallbackTransfer&) = delete;
    SelfDrawingNodeRectChangeCallbackTransfer& operator=(const SelfDrawingNodeRectChangeCallbackTransfer&) = delete;
    SelfDrawingNodeRectChangeCallbackTransfer(SelfDrawingNodeRectChangeCallbackTransfer&&) = delete;
    SelfDrawingNodeRectChangeCallbackTransfer& operator=(SelfDrawingNodeRectChangeCallbackTransfer&&) = delete;

protected:
    void Persist(IpcPersistenceMap& map, std::mutex& mutex) override;

private:
    static constexpr bool registered_ = true;
    friend struct TransferRegistrationChecker<SelfDrawingNodeRectChangeCallbackTransfer>;
    std::map<pid_t, std::shared_ptr<SelfDrawingNodeRectChangeCallbackInput>> inputs_;
};

class UnRegisterSelfDrawingNodeRectChangeCallbackInput
    : public RSIpcPersistenceDataBase<UnRegisterSelfDrawingNodeRectChangeCallbackInput> {
public:
    explicit UnRegisterSelfDrawingNodeRectChangeCallbackInput(pid_t pid) : pid_(pid) {}

    pid_t GetPid() const { return pid_; }

    bool Marshalling(Parcel& parcel) const override;
    [[nodiscard]] static std::shared_ptr<UnRegisterSelfDrawingNodeRectChangeCallbackInput> Unmarshalling(
        Parcel& parcel, int32_t& errCode);

    UnRegisterSelfDrawingNodeRectChangeCallbackInput(const UnRegisterSelfDrawingNodeRectChangeCallbackInput&) = delete;
    UnRegisterSelfDrawingNodeRectChangeCallbackInput& operator=(
        const UnRegisterSelfDrawingNodeRectChangeCallbackInput&) = delete;
    UnRegisterSelfDrawingNodeRectChangeCallbackInput(UnRegisterSelfDrawingNodeRectChangeCallbackInput&&) = delete;
    UnRegisterSelfDrawingNodeRectChangeCallbackInput& operator=(
        UnRegisterSelfDrawingNodeRectChangeCallbackInput&&) = delete;

private:
    pid_t pid_;
};

class UnRegisterSelfDrawingNodeRectChangeCallbackReply
    : public RSIpcPersistenceReplyBase<UnRegisterSelfDrawingNodeRectChangeCallbackReply> {
public:
    explicit UnRegisterSelfDrawingNodeRectChangeCallbackReply(int32_t result = Detail::REPLY_RESULT_PENDING)
        : result_(result) {}

    int32_t GetResult() const { return result_; }

    bool Marshalling(Parcel& parcel) const override;
    bool Unmarshalling(Parcel& parcel) override;

    UnRegisterSelfDrawingNodeRectChangeCallbackReply(const UnRegisterSelfDrawingNodeRectChangeCallbackReply&) = delete;
    UnRegisterSelfDrawingNodeRectChangeCallbackReply& operator=(
        const UnRegisterSelfDrawingNodeRectChangeCallbackReply&) = delete;
    UnRegisterSelfDrawingNodeRectChangeCallbackReply(UnRegisterSelfDrawingNodeRectChangeCallbackReply&&) = delete;
    UnRegisterSelfDrawingNodeRectChangeCallbackReply& operator=(
        UnRegisterSelfDrawingNodeRectChangeCallbackReply&&) = delete;

private:
    int32_t result_ = Detail::REPLY_RESULT_PENDING;
};

class UnRegisterSelfDrawingNodeRectChangeCallbackTransfer final
    : public RSIpcTransferCRTP<UnRegisterSelfDrawingNodeRectChangeCallbackTransfer,
                                UnRegisterSelfDrawingNodeRectChangeCallbackInput,
                                UnRegisterSelfDrawingNodeRectChangeCallbackReply> {
public:
    explicit UnRegisterSelfDrawingNodeRectChangeCallbackTransfer(
        const std::shared_ptr<UnRegisterSelfDrawingNodeRectChangeCallbackInput>& input)
        : inputData_(input) {}

    static constexpr RSIServiceToRenderConnectionInterfaceCode TypeId =
        RSIServiceToRenderConnectionInterfaceCode::UNREGISTER_SELF_DRAWING_NODE_RECT_CHANGE_CALLBACK;
    RSIServiceToRenderConnectionInterfaceCode GetTypeId() const override { return TypeId; }
    RSIServiceToRenderConnectionInterfaceCode GetPersistLockTypeId() const override
    {
        return RSIServiceToRenderConnectionInterfaceCode::REGISTER_SELF_DRAWING_NODE_RECT_CHANGE_CALLBACK;
    }
    bool IsPersistent() const override { return true; }
    bool IsSync() const override { return true; }
    FanoutPolicy GetFanoutPolicy() const override { return FanoutPolicy::FAIL_FAST; }
    int32_t GetReplyResult() const override
    {
        std::lock_guard<std::mutex> lock(mutex_);
        return replyData_ ? replyData_->GetResult() : Detail::REPLY_RESULT_PENDING;
    }

    bool Apply(const sptr<RSRenderPipelineAgent>& agent) override;
    bool ProxyMarshalling(Parcel& parcel) const override;
    std::shared_ptr<RSIpcTransferBase> CopyTransfer() const override
    {
        return std::make_shared<UnRegisterSelfDrawingNodeRectChangeCallbackTransfer>(inputData_);
    }
    void ClearPid(pid_t) override {}

    UnRegisterSelfDrawingNodeRectChangeCallbackTransfer(
        const UnRegisterSelfDrawingNodeRectChangeCallbackTransfer&) = delete;
    UnRegisterSelfDrawingNodeRectChangeCallbackTransfer& operator=(
        const UnRegisterSelfDrawingNodeRectChangeCallbackTransfer&) = delete;
    UnRegisterSelfDrawingNodeRectChangeCallbackTransfer(
        UnRegisterSelfDrawingNodeRectChangeCallbackTransfer&&) = delete;
    UnRegisterSelfDrawingNodeRectChangeCallbackTransfer& operator=(
        UnRegisterSelfDrawingNodeRectChangeCallbackTransfer&&) = delete;

protected:
    void Persist(IpcPersistenceMap& map, std::mutex& mutex) override;

private:
    static constexpr bool registered_ = true;
    friend struct TransferRegistrationChecker<UnRegisterSelfDrawingNodeRectChangeCallbackTransfer>;
    std::shared_ptr<UnRegisterSelfDrawingNodeRectChangeCallbackInput> inputData_;
};

} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_MAIN_RENDER_PROCESS_TRANSACTION_ZIDL_TRANSFERS_RS_SELF_DRAWING_NODE_RECT_CHANGE_CALLBACK_TRANSFER_H
