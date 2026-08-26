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

#ifndef RENDER_SERVICE_MAIN_RENDER_PROCESS_TRANSACTION_ZIDL_TRANSFERS_RS_ON_HWC_EVENT_TRANSFER_H
#define RENDER_SERVICE_MAIN_RENDER_PROCESS_TRANSACTION_ZIDL_TRANSFERS_RS_ON_HWC_EVENT_TRANSFER_H

#include <cstdint>
#include <memory>
#include <vector>

#include <parcel.h>

#include "rs_ipc_persistence_def.h"
#include "rs_ipc_persistence_manager.h"

namespace OHOS {
namespace Rosen {

class OnHwcEventInput : public RSIpcPersistenceDataBase<OnHwcEventInput> {
public:
    OnHwcEventInput(uint32_t deviceId, uint32_t eventId, const std::vector<int32_t>& eventData)
        : deviceId_(deviceId), eventId_(eventId), eventData_(eventData) {}

    uint32_t GetDeviceId() const { return deviceId_; }
    uint32_t GetEventId() const { return eventId_; }
    const std::vector<int32_t>& GetEventData() const { return eventData_; }

    bool Marshalling(Parcel& parcel) const override;
    [[nodiscard]] static std::shared_ptr<OnHwcEventInput> Unmarshalling(Parcel& parcel, int32_t& errCode);

    OnHwcEventInput(const OnHwcEventInput&) = delete;
    OnHwcEventInput& operator=(const OnHwcEventInput&) = delete;
    OnHwcEventInput(OnHwcEventInput&&) = delete;
    OnHwcEventInput& operator=(OnHwcEventInput&&) = delete;

private:
    uint32_t deviceId_;
    uint32_t eventId_;
    std::vector<int32_t> eventData_;
};

class OnHwcEventTransfer final
    : public RSIpcTransferCRTP<OnHwcEventTransfer, OnHwcEventInput, RSIpcEmptyReply> {
public:
    explicit OnHwcEventTransfer(const std::shared_ptr<OnHwcEventInput>& input) : inputData_(input) {}

    static constexpr RSIServiceToRenderConnectionInterfaceCode TypeId =
        RSIServiceToRenderConnectionInterfaceCode::HANDLE_HWC_EVENT;
    RSIServiceToRenderConnectionInterfaceCode GetTypeId() const override { return TypeId; }
    bool IsPersistent() const override { return true; }
    bool IsSync() const override { return false; }
    FanoutPolicy GetFanoutPolicy() const override { return FanoutPolicy::ANY_SUCCESS; }

    bool Apply(const sptr<RSRenderPipelineAgent>& agent) override;
    bool ProxyMarshalling(Parcel& parcel) const override;
    std::shared_ptr<RSIpcTransferBase> CopyTransfer() const override
    {
        return std::make_shared<OnHwcEventTransfer>(inputData_);
    }
    void ClearPid(pid_t) override {}

    OnHwcEventTransfer(const OnHwcEventTransfer&) = delete;
    OnHwcEventTransfer& operator=(const OnHwcEventTransfer&) = delete;
    OnHwcEventTransfer(OnHwcEventTransfer&&) = delete;
    OnHwcEventTransfer& operator=(OnHwcEventTransfer&&) = delete;

protected:
    void Persist(IpcPersistenceMap& map, std::mutex& mutex) override;

private:
    static constexpr bool registered_ = true;
    friend struct TransferRegistrationChecker<OnHwcEventTransfer>;
    std::shared_ptr<OnHwcEventInput> inputData_;
};

} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_MAIN_RENDER_PROCESS_TRANSACTION_ZIDL_TRANSFERS_RS_ON_HWC_EVENT_TRANSFER_H
