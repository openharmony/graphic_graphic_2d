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

#ifndef RENDER_SERVICE_BASE_TRANSACTION_RS_BUFFER_TRANSACTION_H
#define RENDER_SERVICE_BASE_TRANSACTION_RS_BUFFER_TRANSACTION_H

#include <shared_mutex>
#include <vector>

#include "parcel.h"
#include "surface_buffer.h"
#include "sync_fence.h"

#include "common/rs_macros.h"

namespace OHOS {
namespace Rosen {
struct RSTransactionConfig;

class RSB_EXPORT RSBufferTransaction : public RefBase {
public:
    RSBufferTransaction() = default;

    RSBufferTransaction(sptr<SurfaceBuffer>& buffer);

    ~RSBufferTransaction() = default;

    sptr<SurfaceBuffer> GetBuffer() const;

    sptr<SyncFence> GetFence() const;

    void SetFence(sptr<SyncFence> fence);

    const std::vector<OHOS::Rect>& GetDamages() const;

    void SetDamages(std::vector<OHOS::Rect>& damages);

    int64_t GetUiTimestamp() const;

    void SetUiTimestamp(int64_t uiTimestamp);

    int64_t GetDesiredPresentTimestamp() const;

    void SetDesiredPresentTimestamp(int64_t desiredPresentTimestamp);

    GSError WriteToMessageParcel(MessageParcel& parcel);

    static sptr<RSBufferTransaction> ReadFromMessageParcel(MessageParcel& parcel);

    static GSError ReadTransactionConfigFromMessageParcel(MessageParcel& parcel, RSTransactionConfig& config);

    static GSError WriteTransactionConfigToMessageParcel(MessageParcel& parcel, const RSTransactionConfig& config);

private:
    mutable std::shared_mutex mutex_;
    sptr<SurfaceBuffer> buffer_ = nullptr;
    sptr<SyncFence> fence_ = nullptr;
    std::vector<OHOS::Rect> damages_ = {};
    int64_t uiTimestamp_ = 0;
    int64_t desiredPresentTimestamp_ = 0;
};

struct RSTransactionConfig {
    uint64_t nodeId = 0;
    sptr<RSBufferTransaction> transaction = nullptr;
};
} // namespace Rosen
} // namespace OHOS
#endif // RENDER_SERVICE_BASE_TRANSACTION_RS_BUFFER_TRANSACTION_H