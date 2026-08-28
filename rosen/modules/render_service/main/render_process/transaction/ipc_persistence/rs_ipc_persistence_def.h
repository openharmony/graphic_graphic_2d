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

#ifndef RENDER_SERVICE_MAIN_RENDER_PROCESS_TRANSACTION_IPC_PERSISTENCE_RS_IPC_PERSISTENCE_DEF_H
#define RENDER_SERVICE_MAIN_RENDER_PROCESS_TRANSACTION_IPC_PERSISTENCE_RS_IPC_PERSISTENCE_DEF_H

#include <cstdint>
#include <memory>
#include <mutex>
#include <sys/types.h>
#include <unordered_map>

#include <parcel.h>
#include "render_process/transaction/zidl/rs_iservice_to_render_connection_ipc_interface_code.h"

namespace OHOS {
namespace Rosen {

enum class FanoutPolicy : uint32_t {
    ANY_SUCCESS = 0, // success if any render process accepted the transfer
    FAIL_FAST = 1,   // return the first failure immediately without sending to the remaining processes
};

namespace Detail {
constexpr uint32_t MAX_PERSIST_MAP_SIZE = 100;
constexpr int32_t REPLY_RESULT_PENDING = -1;
} // namespace Detail

class RSRenderPipelineAgent;
class RSIpcTransferBase;

using IpcPersistenceMap =
    std::unordered_map<RSIServiceToRenderConnectionInterfaceCode, std::shared_ptr<RSIpcTransferBase>>;

class RSIpcTransferBase {
public:
    virtual ~RSIpcTransferBase() = default;

    virtual RSIServiceToRenderConnectionInterfaceCode GetTypeId() const = 0;
    virtual RSIServiceToRenderConnectionInterfaceCode GetPersistLockTypeId() const { return GetTypeId(); }
    virtual bool IsPersistent() const = 0; // true: persist and replay on render process restart
    virtual bool IsSync() const = 0; // true: sync IPC, reply marshalled back to proxy
    virtual FanoutPolicy GetFanoutPolicy() const = 0; // fanout aggregation rule across render processes
    virtual int32_t GetReplyResult() const { return Detail::REPLY_RESULT_PENDING; }

    virtual void Persist(IpcPersistenceMap& map, std::mutex& mutex) = 0;
    virtual void ClearPid(pid_t pid) = 0;

    virtual bool Apply(const sptr<RSRenderPipelineAgent>& agent) = 0;

    virtual bool ProxyMarshalling(Parcel& parcel) const = 0;
    virtual bool StubMarshalling(Parcel& parcel) const = 0;
    virtual bool ProxyUnmarshalling(Parcel& parcel) = 0;

    virtual std::shared_ptr<RSIpcTransferBase> CopyTransfer() const = 0;

    RSIpcTransferBase(const RSIpcTransferBase&) = delete;
    RSIpcTransferBase& operator=(const RSIpcTransferBase&) = delete;
    RSIpcTransferBase(RSIpcTransferBase&&) = delete;
    RSIpcTransferBase& operator=(RSIpcTransferBase&&) = delete;

protected:
    RSIpcTransferBase() = default;

    mutable std::mutex mutex_;
};

template<typename Derived>
class RSIpcPersistenceDataBase {
public:
    virtual ~RSIpcPersistenceDataBase() = default;

    virtual bool Marshalling(Parcel& parcel) const = 0;

    [[nodiscard]] static std::shared_ptr<Derived> Unmarshalling(Parcel& parcel, int32_t& errCode)
    {
        return Derived::Unmarshalling(parcel, errCode);
    }

    RSIpcPersistenceDataBase(const RSIpcPersistenceDataBase&) = delete;
    RSIpcPersistenceDataBase& operator=(const RSIpcPersistenceDataBase&) = delete;
    RSIpcPersistenceDataBase(RSIpcPersistenceDataBase&&) = delete;
    RSIpcPersistenceDataBase& operator=(RSIpcPersistenceDataBase&&) = delete;

protected:
    RSIpcPersistenceDataBase() = default;
};

template<typename Derived>
class RSIpcPersistenceReplyBase {
public:
    virtual ~RSIpcPersistenceReplyBase() = default;

    virtual bool Marshalling(Parcel& parcel) const = 0;
    virtual bool Unmarshalling(Parcel& parcel) = 0;

    RSIpcPersistenceReplyBase(const RSIpcPersistenceReplyBase&) = delete;
    RSIpcPersistenceReplyBase& operator=(const RSIpcPersistenceReplyBase&) = delete;
    RSIpcPersistenceReplyBase(RSIpcPersistenceReplyBase&&) = delete;
    RSIpcPersistenceReplyBase& operator=(RSIpcPersistenceReplyBase&&) = delete;

protected:
    RSIpcPersistenceReplyBase() = default;
};

class RSIpcEmptyReply final : public RSIpcPersistenceReplyBase<RSIpcEmptyReply> {
public:
    bool Marshalling(Parcel& parcel) const override
    {
        (void)parcel;
        return true;
    }
    bool Unmarshalling(Parcel& parcel) override
    {
        (void)parcel;
        return true;
    }
};

template<RSIServiceToRenderConnectionInterfaceCode TypeId, typename TransferClass>
class RSIpcPersistentTransferRegister;

template<typename TransferClass>
struct TransferRegistrationChecker {
    static constexpr bool Check = TransferClass::registered_;
    static const RSIpcPersistentTransferRegister<TransferClass::TypeId, TransferClass> registrar;
};

template<typename Derived, typename InputData, typename ReplyData>
class RSIpcTransferCRTP : public RSIpcTransferBase {
public:
    RSIpcTransferCRTP()
    {
        static_assert(TransferRegistrationChecker<Derived>::Check,
            "Transfer must declare static bool registered_ (required by RSIpcPersistentTransferRegister)");
        [[maybe_unused]] auto* reg = &TransferRegistrationChecker<Derived>::registrar;
    }

    bool StubMarshalling(Parcel& parcel) const override
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (!replyData_) {
            return false;
        }
        return replyData_->Marshalling(parcel);
    }

    bool ProxyUnmarshalling(Parcel& parcel) override
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (!replyData_) {
            replyData_ = std::make_shared<ReplyData>();
        }
        return replyData_->Unmarshalling(parcel);
    }

    [[nodiscard]] static std::shared_ptr<Derived> StubUnmarshalling(
        Parcel& parcel, uint32_t maxEntries, int32_t& errCode)
    {
        (void)maxEntries;
        auto input = RSIpcPersistenceDataBase<InputData>::Unmarshalling(parcel, errCode);
        if (!input) {
            return nullptr;
        }
        return std::make_shared<Derived>(input);
    }

protected:
    void SetReplyData(std::shared_ptr<ReplyData> data)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        replyData_ = std::move(data);
    }

    std::shared_ptr<ReplyData> replyData_;
};

} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_MAIN_RENDER_PROCESS_TRANSACTION_IPC_PERSISTENCE_RS_IPC_PERSISTENCE_DEF_H
