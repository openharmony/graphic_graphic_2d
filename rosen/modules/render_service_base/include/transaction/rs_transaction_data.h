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

#include <list>
#include <memory>

#ifdef ROSEN_OHOS
#include <parcel.h>
#endif

namespace OHOS {
namespace Rosen {
class RSCommand;
class RSContext;

#ifdef ROSEN_OHOS
class RSTransactionData : public Parcelable {
#else
class RSTransactionData {
#endif
public:
    RSTransactionData() = default;
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

private:
    void AddCommand(std::unique_ptr<RSCommand>& command);
    void AddCommand(std::unique_ptr<RSCommand>&& command);

#ifdef ROSEN_OHOS
    bool UnmarshallingCommand(Parcel& parcel);
#endif

    std::list<std::unique_ptr<RSCommand>> commands_;

    friend class RSTransactionProxy;
};

} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_RENDER_SERVICE_BASE_RS_TRANSACTION_DATA_H
