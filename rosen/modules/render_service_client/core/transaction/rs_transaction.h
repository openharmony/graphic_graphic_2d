/*
 * Copyright (c) 2021-2023 Huawei Device Co., Ltd.
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

#ifndef RENDER_SERVICE_CLIENT_CORE_UI_RS_TRANSACTION_H
#define RENDER_SERVICE_CLIENT_CORE_UI_RS_TRANSACTION_H

#include <message_parcel.h>
#include <mutex>
#include <parcel.h>
#include <refbase.h>

#include "common/rs_common_def.h"
#include "common/rs_macros.h"

namespace OHOS {
namespace Rosen {

class RSC_EXPORT RSTransaction : public Parcelable {
public:
    RSTransaction() = default;
    RSTransaction(const RSTransaction&) = delete;
    RSTransaction(const RSTransaction&&) = delete;
    RSTransaction& operator=(const RSTransaction&) = delete;
    RSTransaction& operator=(const RSTransaction&&) = delete;
    ~RSTransaction() override = default;

    static RSTransaction* Unmarshalling(Parcel& parcel);
    bool Marshalling(Parcel& parcel) const override;

    static void FlushImplicitTransaction();
    void OpenSyncTransaction();
    void CloseSyncTransaction();

    void Begin();
    void Commit();

    void SetDuration(int32_t duration) { duration_ = duration; }
    int32_t GetDuration() const { return duration_; }

private:
    uint64_t GenerateSyncId();
    void ResetSyncTransactionInfo();
    bool UnmarshallingParam(Parcel& parcel);

    uint64_t syncId_ { 0 };
    std::mutex mutex_;
    mutable int32_t transactionCount_ { 0 };
    int32_t duration_ = 0;

    friend class RSSyncTransactionController;
};

} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_UI_RS_TRANSACTION_H
