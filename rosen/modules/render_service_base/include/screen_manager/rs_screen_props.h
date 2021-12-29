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

#ifndef RS_SCREEN_PROPS
#define RS_SCREEN_PROPS

#include <cstdint>
#include <parcel.h>
#include <refbase.h>
#include <string>

#include "display_type.h"
#include "screen_manager/screen_types.h"

namespace OHOS {
namespace Rosen {
class RSScreenProps : public Parcelable {
public:
    RSScreenProps() = default;
    RSScreenProps(std::string propName, uint32_t propId, uint64_t value);
    ~RSScreenProps() = default;
    static RSScreenProps* Unmarshalling(Parcel &parcel);
    bool Marshalling(Parcel &parcel) const override;

    void SetPropertyName(const std::string& propName);
    void SetPropId(uint32_t propId);
    void SetValue(uint64_t value);

    const std::string& GetPropertyName() const;
    uint32_t GetPropId() const;
    uint64_t GetValue() const;

private:
    std::string propName_;
    uint32_t propId_;
    uint64_t value_;
};
} // namespace Rosen
} // namespace OHOS
#endif // RS_SCREEN_PROPS