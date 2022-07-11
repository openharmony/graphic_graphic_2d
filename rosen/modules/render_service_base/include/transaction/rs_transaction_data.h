/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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
    RSTransactionData(RSTransactionData&& other) : commands_(std::move(other.commands_)),
        followTypes_(std::move(other.followTypes_)), nodeIds_(std::move(other.nodeIds_)), timestamp_(std::move(other.timestamp_)) {}
    ~RSTransactionData() noexcept;

#ifdef ROSEN_OHOS
    static RSTransactionData* Unmarshalling(Parcel& parcel);
    bool Marshalling(Parcel& parcel) const override;
#endif

    int GetCommandCount() const
    {
        return commands_.size();
    }

    bool IsEmpty() const
    {
        return commands_.empty();
    }

    void Process(RSContext& context);

    void Clear();

    uint64_t GetTimestamp() const
    {
        return timestamp_;
    }

    std::vector<std::unique_ptr<RSCommand>>& GetCommands()
    {
        return commands_;
    }

    const std::vector<FollowType>& GetFollowTypes() const
    {
        return followTypes_;
    }

    const std::vector<NodeId>& GetNodeIds() const
    {
        return nodeIds_;
    }

private:
    void AddCommand(std::unique_ptr<RSCommand>& command, NodeId nodeId, FollowType followType);
    void AddCommand(std::unique_ptr<RSCommand>&& command, NodeId nodeId, FollowType followType);

#ifdef ROSEN_OHOS
    bool UnmarshallingCommand(Parcel& parcel);
#endif
    std::vector<std::unique_ptr<RSCommand>> commands_;
    std::vector<FollowType> followTypes_;
    std::vector<NodeId> nodeIds_;
    uint64_t timestamp_ = 0;

    friend class RSTransactionProxy;
    friend class RSMessageProcessor;
};

} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_RENDER_SERVICE_BASE_RS_TRANSACTION_DATA_H
