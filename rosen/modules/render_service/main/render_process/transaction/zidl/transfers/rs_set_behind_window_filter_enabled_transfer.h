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

#ifndef RENDER_SERVICE_MAIN_RENDER_PROCESS_TRANSACTION_ZIDL_TRANSFERS_RS_SET_BEHIND_WINDOW_FILTER_ENABLED_TRANSFER_H
#define RENDER_SERVICE_MAIN_RENDER_PROCESS_TRANSACTION_ZIDL_TRANSFERS_RS_SET_BEHIND_WINDOW_FILTER_ENABLED_TRANSFER_H

#include <cstdint>
#include <memory>

#include <parcel.h>

#include "rs_ipc_persistence_def.h"
#include "rs_ipc_persistence_manager.h"

namespace OHOS {
namespace Rosen {

class SetBehindWindowFilterEnabledInput : public RSIpcPersistenceDataBase<SetBehindWindowFilterEnabledInput> {
public:
    explicit SetBehindWindowFilterEnabledInput(bool enabled) : enabled_(enabled) {}

    bool GetEnabled() const { return enabled_; }

    bool Marshalling(Parcel& parcel) const override;
    [[nodiscard]] static std::shared_ptr<SetBehindWindowFilterEnabledInput> Unmarshalling(
        Parcel& parcel, int32_t& errCode);

    SetBehindWindowFilterEnabledInput(const SetBehindWindowFilterEnabledInput&) = delete;
    SetBehindWindowFilterEnabledInput& operator=(const SetBehindWindowFilterEnabledInput&) = delete;
    SetBehindWindowFilterEnabledInput(SetBehindWindowFilterEnabledInput&&) = delete;
    SetBehindWindowFilterEnabledInput& operator=(SetBehindWindowFilterEnabledInput&&) = delete;

private:
    bool enabled_;
};

class SetBehindWindowFilterEnabledTransfer final
    : public RSIpcTransferCRTP<SetBehindWindowFilterEnabledTransfer,
                                SetBehindWindowFilterEnabledInput,
                                RSIpcEmptyReply> {
public:
    explicit SetBehindWindowFilterEnabledTransfer(
        const std::shared_ptr<SetBehindWindowFilterEnabledInput>& input)
        : inputData_(input) {}

    static constexpr RSIServiceToRenderConnectionInterfaceCode TypeId =
        RSIServiceToRenderConnectionInterfaceCode::SET_BEHIND_WINDOW_FILTER_ENABLED;
    RSIServiceToRenderConnectionInterfaceCode GetTypeId() const override { return TypeId; }
    bool IsPersistent() const override { return true; }
    bool IsSync() const override { return true; }
    FanoutPolicy GetFanoutPolicy() const override { return FanoutPolicy::ANY_SUCCESS; }

    bool Apply(const sptr<RSRenderPipelineAgent>& agent) override;
    bool ProxyMarshalling(Parcel& parcel) const override;
    std::shared_ptr<RSIpcTransferBase> CopyTransfer() const override
    {
        return std::make_shared<SetBehindWindowFilterEnabledTransfer>(inputData_);
    }
    void ClearPid(pid_t) override {}

    SetBehindWindowFilterEnabledTransfer(const SetBehindWindowFilterEnabledTransfer&) = delete;
    SetBehindWindowFilterEnabledTransfer& operator=(const SetBehindWindowFilterEnabledTransfer&) = delete;
    SetBehindWindowFilterEnabledTransfer(SetBehindWindowFilterEnabledTransfer&&) = delete;
    SetBehindWindowFilterEnabledTransfer& operator=(SetBehindWindowFilterEnabledTransfer&&) = delete;

protected:
    void Persist(IpcPersistenceMap& map, std::mutex& mutex) override;

private:
    static constexpr bool registered_ = true;
    friend struct TransferRegistrationChecker<SetBehindWindowFilterEnabledTransfer>;
    std::shared_ptr<SetBehindWindowFilterEnabledInput> inputData_;
};

} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_MAIN_RENDER_PROCESS_TRANSACTION_ZIDL_TRANSFERS_RS_SET_BEHIND_WINDOW_FILTER_ENABLED_TRANSFER_H
