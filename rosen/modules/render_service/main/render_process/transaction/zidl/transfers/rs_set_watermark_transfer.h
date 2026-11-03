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

#ifndef RENDER_SERVICE_MAIN_RENDER_PROCESS_TRANSACTION_ZIDL_TRANSFERS_RS_SET_WATERMARK_TRANSFER_H
#define RENDER_SERVICE_MAIN_RENDER_PROCESS_TRANSACTION_ZIDL_TRANSFERS_RS_SET_WATERMARK_TRANSFER_H

#include <cstdint>
#include <map>
#include <memory>
#include <string>
#include <sys/types.h>

#include <parcel.h>

#include "rs_ipc_persistence_def.h"
#include "rs_ipc_persistence_manager.h"

namespace OHOS {
namespace Media {
class PixelMap;
} // namespace Media

namespace Rosen {

class SetWatermarkInput : public RSIpcPersistenceDataBase<SetWatermarkInput> {
public:
    SetWatermarkInput(pid_t pid, const std::string& name,
                      std::shared_ptr<Media::PixelMap> watermark,
                      uint32_t rowCount, uint32_t colCount)
        : pid_(pid), name_(name), watermark_(watermark), rowCount_(rowCount), colCount_(colCount) {}

    pid_t GetPid() const { return pid_; }
    const std::string& GetName() const { return name_; }
    const std::shared_ptr<Media::PixelMap>& GetWatermark() const { return watermark_; }
    uint32_t GetRowCount() const { return rowCount_; }
    uint32_t GetColCount() const { return colCount_; }

    bool Marshalling(Parcel& parcel) const override;
    [[nodiscard]] static std::shared_ptr<SetWatermarkInput> Unmarshalling(Parcel& parcel, int32_t& errCode);

    SetWatermarkInput(const SetWatermarkInput&) = delete;
    SetWatermarkInput& operator=(const SetWatermarkInput&) = delete;
    SetWatermarkInput(SetWatermarkInput&&) = delete;
    SetWatermarkInput& operator=(SetWatermarkInput&&) = delete;

private:
    pid_t pid_;
    std::string name_;
    std::shared_ptr<Media::PixelMap> watermark_;
    uint32_t rowCount_;
    uint32_t colCount_;
};

class SetWatermarkTransfer final
    : public RSIpcTransferCRTP<SetWatermarkTransfer, SetWatermarkInput, RSIpcEmptyReply> {
public:
    explicit SetWatermarkTransfer(std::map<pid_t, std::shared_ptr<SetWatermarkInput>> inputs)
        : inputs_(std::move(inputs)) {}

    static constexpr RSIServiceToRenderConnectionInterfaceCode TypeId =
        RSIServiceToRenderConnectionInterfaceCode::SET_WATERMARK;
    RSIServiceToRenderConnectionInterfaceCode GetTypeId() const override { return TypeId; }
    bool IsPersistent() const override { return true; }
    bool IsSync() const override { return false; }
    FanoutPolicy GetFanoutPolicy() const override { return FanoutPolicy::FAIL_FAST; }

    bool Apply(const sptr<RSRenderPipelineAgent>& agent) override;

    bool ProxyMarshalling(Parcel& parcel) const override;
    [[nodiscard]] static std::shared_ptr<SetWatermarkTransfer> StubUnmarshalling(
        Parcel& parcel, uint32_t maxEntries, int32_t& errCode);
    std::shared_ptr<RSIpcTransferBase> CopyTransfer() const override
    {
        return std::make_shared<SetWatermarkTransfer>(inputs_);
    }
    void ClearPid(pid_t pid) override;

    SetWatermarkTransfer(const SetWatermarkTransfer&) = delete;
    SetWatermarkTransfer& operator=(const SetWatermarkTransfer&) = delete;
    SetWatermarkTransfer(SetWatermarkTransfer&&) = delete;
    SetWatermarkTransfer& operator=(SetWatermarkTransfer&&) = delete;

protected:
    void Persist(IpcPersistenceMap& map, std::mutex& mutex) override;

private:
    static constexpr bool registered_ = true;
    friend struct TransferRegistrationChecker<SetWatermarkTransfer>;
    std::map<pid_t, std::shared_ptr<SetWatermarkInput>> inputs_;
};

} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_MAIN_RENDER_PROCESS_TRANSACTION_ZIDL_TRANSFERS_RS_SET_WATERMARK_TRANSFER_H
