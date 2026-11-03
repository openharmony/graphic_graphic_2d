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

#ifndef RENDER_SERVICE_MAIN_RENDER_PROCESS_TRANSACTION_ZIDL_TRANSFERS_RS_SET_SHOW_REFRESH_RATE_ENABLED_TRANSFER_H
#define RENDER_SERVICE_MAIN_RENDER_PROCESS_TRANSACTION_ZIDL_TRANSFERS_RS_SET_SHOW_REFRESH_RATE_ENABLED_TRANSFER_H

#include <cstdint>
#include <memory>

#include <parcel.h>

#include "rs_ipc_persistence_def.h"
#include "rs_ipc_persistence_manager.h"

namespace OHOS {
namespace Rosen {

class SetShowRefreshRateEnabledInput : public RSIpcPersistenceDataBase<SetShowRefreshRateEnabledInput> {
public:
    SetShowRefreshRateEnabledInput(bool enabled, int32_t type) : enabled_(enabled), type_(type) {}

    bool GetEnabled() const { return enabled_; }
    int32_t GetType() const { return type_; }

    bool Marshalling(Parcel& parcel) const override;
    [[nodiscard]] static std::shared_ptr<SetShowRefreshRateEnabledInput> Unmarshalling(
        Parcel& parcel, int32_t& errCode);

    SetShowRefreshRateEnabledInput(const SetShowRefreshRateEnabledInput&) = delete;
    SetShowRefreshRateEnabledInput& operator=(const SetShowRefreshRateEnabledInput&) = delete;
    SetShowRefreshRateEnabledInput(SetShowRefreshRateEnabledInput&&) = delete;
    SetShowRefreshRateEnabledInput& operator=(SetShowRefreshRateEnabledInput&&) = delete;

private:
    bool enabled_;
    int32_t type_;
};

class SetShowRefreshRateEnabledTransfer final
    : public RSIpcTransferCRTP<SetShowRefreshRateEnabledTransfer,
                                SetShowRefreshRateEnabledInput,
                                RSIpcEmptyReply> {
public:
    explicit SetShowRefreshRateEnabledTransfer(
        const std::shared_ptr<SetShowRefreshRateEnabledInput>& input)
        : inputData_(input) {}

    static constexpr RSIServiceToRenderConnectionInterfaceCode TypeId =
        RSIServiceToRenderConnectionInterfaceCode::SET_SHOW_REFRESH_RATE_ENABLED;
    RSIServiceToRenderConnectionInterfaceCode GetTypeId() const override { return TypeId; }
    bool IsPersistent() const override { return true; }
    bool IsSync() const override { return true; }
    FanoutPolicy GetFanoutPolicy() const override { return FanoutPolicy::ANY_SUCCESS; }

    bool Apply(const sptr<RSRenderPipelineAgent>& agent) override;
    bool ProxyMarshalling(Parcel& parcel) const override;
    std::shared_ptr<RSIpcTransferBase> CopyTransfer() const override
    {
        return std::make_shared<SetShowRefreshRateEnabledTransfer>(inputData_);
    }
    void ClearPid(pid_t) override {}

    SetShowRefreshRateEnabledTransfer(const SetShowRefreshRateEnabledTransfer&) = delete;
    SetShowRefreshRateEnabledTransfer& operator=(const SetShowRefreshRateEnabledTransfer&) = delete;
    SetShowRefreshRateEnabledTransfer(SetShowRefreshRateEnabledTransfer&&) = delete;
    SetShowRefreshRateEnabledTransfer& operator=(SetShowRefreshRateEnabledTransfer&&) = delete;

protected:
    void Persist(IpcPersistenceMap& map, std::mutex& mutex) override;

private:
    static constexpr bool registered_ = true;
    friend struct TransferRegistrationChecker<SetShowRefreshRateEnabledTransfer>;
    std::shared_ptr<SetShowRefreshRateEnabledInput> inputData_;
};

} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_MAIN_RENDER_PROCESS_TRANSACTION_ZIDL_TRANSFERS_RS_SET_SHOW_REFRESH_RATE_ENABLED_TRANSFER_H
