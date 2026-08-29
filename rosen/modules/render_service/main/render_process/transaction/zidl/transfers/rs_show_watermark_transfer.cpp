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

#include "rs_show_watermark_transfer.h"

#include "core/rs_render_pipeline_agent.h"
#include "ipc_persistence/rs_ipc_persistence_manager.h"

#include "platform/common/rs_log.h"

#undef LOG_TAG
#define LOG_TAG "ShowWatermarkTransfer"

namespace OHOS {
namespace Rosen {

bool ShowWatermarkInput::Marshalling(Parcel& parcel) const
{
    if (!parcel.WriteParcelable(watermark_.get())) {
        RS_LOGE("%{public}s: WriteParcelable watermark failed", __func__);
        return false;
    }
    if (!parcel.WriteBool(isShow_)) {
        RS_LOGE("%{public}s: WriteBool isShow failed", __func__);
        return false;
    }
    return true;
}

std::shared_ptr<ShowWatermarkInput> ShowWatermarkInput::Unmarshalling(Parcel& parcel, int32_t& errCode)
{
    auto watermark = std::shared_ptr<Media::PixelMap>(parcel.ReadParcelable<Media::PixelMap>());
    bool isShow;
    if (!parcel.ReadBool(isShow)) {
        RS_LOGE("%{public}s: ReadBool isShow failed", __func__);
        errCode = ERR_INVALID_DATA;
        return nullptr;
    }
    if (!watermark) {
        RS_LOGE("%{public}s: watermark is nullptr", __func__);
        errCode = ERR_INVALID_DATA;
        return nullptr;
    }
    return std::make_shared<ShowWatermarkInput>(watermark, isShow);
}

void ShowWatermarkTransfer::Persist(IpcPersistenceMap& map, std::mutex& mutex)
{
    auto typeId = GetTypeId();
    std::shared_ptr<ShowWatermarkTransfer> target;
    {
        std::lock_guard<std::mutex> lock(mutex);
        auto it = map.find(typeId);
        if (it == map.end()) {
            if (map.size() >= Detail::MAX_PERSIST_MAP_SIZE) {
                RS_LOGE("%{public}s: persist map full, size=%{public}zu", __func__, map.size());
                return;
            }
            map[typeId] = std::make_shared<ShowWatermarkTransfer>(inputData_);
            return;
        }
        target = std::static_pointer_cast<ShowWatermarkTransfer>(it->second);
    }
    if (!target) {
        RS_LOGE("%{public}s: existing transfer is nullptr", __func__);
        return;
    }
    std::lock_guard<std::mutex> targetLock(target->mutex_);
    target->inputData_ = inputData_;
}

bool ShowWatermarkTransfer::ProxyMarshalling(Parcel& parcel) const
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (!inputData_) {
        return false;
    }
    return inputData_->Marshalling(parcel);
}

bool ShowWatermarkTransfer::Apply(const sptr<RSRenderPipelineAgent>& agent)
{
    if (!agent || !inputData_) {
        RS_LOGE("%{public}s: agent or input is nullptr", __func__);
        return false;
    }
    agent->ShowWatermark(inputData_->GetWatermark(), inputData_->IsShow());
    return true;
}

// Explicit ODR-use: guarantee factory registration at static init time, independent of
// whether any ShowWatermarkTransfer is constructed before CreateTransferByTypeId.
[[maybe_unused]] const auto* g_showWatermarkRegistrar =
    &TransferRegistrationChecker<ShowWatermarkTransfer>::registrar;

} // namespace Rosen
} // namespace OHOS
