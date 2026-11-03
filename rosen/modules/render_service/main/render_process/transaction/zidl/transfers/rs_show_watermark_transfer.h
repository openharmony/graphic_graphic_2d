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

#ifndef RENDER_SERVICE_MAIN_RENDER_PROCESS_TRANSACTION_ZIDL_TRANSFERS_RS_SHOW_WATERMARK_TRANSFER_H
#define RENDER_SERVICE_MAIN_RENDER_PROCESS_TRANSACTION_ZIDL_TRANSFERS_RS_SHOW_WATERMARK_TRANSFER_H

#include <cstdint>
#include <memory>

#include <parcel.h>

#include "rs_ipc_persistence_def.h"
#include "rs_ipc_persistence_manager.h"

namespace OHOS {
namespace Media {
class PixelMap;
} // namespace Media

namespace Rosen {

class ShowWatermarkInput : public RSIpcPersistenceDataBase<ShowWatermarkInput> {
public:
    ShowWatermarkInput(std::shared_ptr<Media::PixelMap> watermark, bool isShow)
        : watermark_(watermark), isShow_(isShow) {}

    const std::shared_ptr<Media::PixelMap>& GetWatermark() const { return watermark_; }
    bool IsShow() const { return isShow_; }

    bool Marshalling(Parcel& parcel) const override;
    [[nodiscard]] static std::shared_ptr<ShowWatermarkInput> Unmarshalling(Parcel& parcel, int32_t& errCode);

    ShowWatermarkInput(const ShowWatermarkInput&) = delete;
    ShowWatermarkInput& operator=(const ShowWatermarkInput&) = delete;
    ShowWatermarkInput(ShowWatermarkInput&&) = delete;
    ShowWatermarkInput& operator=(ShowWatermarkInput&&) = delete;

private:
    std::shared_ptr<Media::PixelMap> watermark_;
    bool isShow_;
};

class ShowWatermarkTransfer final
    : public RSIpcTransferCRTP<ShowWatermarkTransfer, ShowWatermarkInput, RSIpcEmptyReply> {
public:
    explicit ShowWatermarkTransfer(const std::shared_ptr<ShowWatermarkInput>& input) : inputData_(input) {}

    static constexpr RSIServiceToRenderConnectionInterfaceCode TypeId =
        RSIServiceToRenderConnectionInterfaceCode::SHOW_WATERMARK;
    RSIServiceToRenderConnectionInterfaceCode GetTypeId() const override { return TypeId; }
    bool IsPersistent() const override { return true; }
    bool IsSync() const override { return false; }
    FanoutPolicy GetFanoutPolicy() const override { return FanoutPolicy::ANY_SUCCESS; }

    bool Apply(const sptr<RSRenderPipelineAgent>& agent) override;
    bool ProxyMarshalling(Parcel& parcel) const override;
    std::shared_ptr<RSIpcTransferBase> CopyTransfer() const override
    {
        return std::make_shared<ShowWatermarkTransfer>(inputData_);
    }
    void ClearPid(pid_t) override {}

    ShowWatermarkTransfer(const ShowWatermarkTransfer&) = delete;
    ShowWatermarkTransfer& operator=(const ShowWatermarkTransfer&) = delete;
    ShowWatermarkTransfer(ShowWatermarkTransfer&&) = delete;
    ShowWatermarkTransfer& operator=(ShowWatermarkTransfer&&) = delete;

protected:
    void Persist(IpcPersistenceMap& map, std::mutex& mutex) override;

private:
    static constexpr bool registered_ = true;
    friend struct TransferRegistrationChecker<ShowWatermarkTransfer>;
    std::shared_ptr<ShowWatermarkInput> inputData_;
};

} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_MAIN_RENDER_PROCESS_TRANSACTION_ZIDL_TRANSFERS_RS_SHOW_WATERMARK_TRANSFER_H
