/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#ifndef ROSEN_RENDER_SERVICE_BASE_RS_TRANSACTION_DATA_H
#define ROSEN_RENDER_SERVICE_BASE_RS_TRANSACTION_DATA_H

#include <memory>
#include <vector>

#include "command/rs_command.h"
#include "pipeline/rs_context.h"

#ifdef ROSEN_OHOS
#include <parcel.h>
#endif

namespace OHOS {
namespace Rosen {
#ifdef ROSEN_OHOS
class RSTransactionData : public Parcelable {
#else
class RSTransactionData {
#endif
public:
    RSTransactionData() = default;
    RSTransactionData(RSTransactionData&& other)
        : payload_(std::move(other.payload_)), timestamp_(std::move(other.timestamp_)),
          pid_(other.pid_), index_(other.index_)
    {}
    ~RSTransactionData() noexcept = default;

#ifdef ROSEN_OHOS
    static RSTransactionData* Unmarshalling(Parcel& parcel);
    bool Marshalling(Parcel& parcel) const override;
#endif

    unsigned long GetCommandCount() const
    {
        return payload_.size();
    }

    bool IsEmpty() const
    {
        return payload_.empty();
    }

    void Process(RSContext& context);

    void Clear();

    uint64_t GetTimestamp() const
    {
        return timestamp_;
    }

    void SetSendingPid(pid_t pid)
    {
        pid_ = pid;
    }

    pid_t GetSendingPid() const
    {
        return pid_;
    }

    void SetIndex(uint64_t index)
    {
        index_ = index;
    }

    uint64_t GetIndex() const
    {
        return index_;
    }

    std::vector<std::tuple<NodeId, FollowType, std::unique_ptr<RSCommand>>>& GetPayload()
    {
        return payload_;
    }

private:
    void AddCommand(std::unique_ptr<RSCommand>& command, NodeId nodeId, FollowType followType);
    void AddCommand(std::unique_ptr<RSCommand>&& command, NodeId nodeId, FollowType followType);

#ifdef ROSEN_OHOS
    bool UnmarshallingCommand(Parcel& parcel);
#endif
    std::vector<std::tuple<NodeId, FollowType, std::unique_ptr<RSCommand>>> payload_;
    uint64_t timestamp_ = 0;
    pid_t pid_ = 0;
    uint64_t index_ = 0;

    friend class RSTransactionProxy;
    friend class RSMessageProcessor;
};
using TransactionDataMap = std::unordered_map<pid_t, std::vector<std::unique_ptr<RSTransactionData>>>;
} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_RENDER_SERVICE_BASE_RS_TRANSACTION_DATA_H
