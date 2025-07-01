/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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
#ifndef UIEFFECT_MASK_PARA_H
#define UIEFFECT_MASK_PARA_H

#include <memory>
#include <functional>

#include <parcel.h>

#include "common/rs_macros.h"

namespace OHOS {
namespace Rosen {
class RSC_EXPORT MaskPara {
public:
    enum Type {
        NONE,
        RIPPLE_MASK,
        RADIAL_GRADIENT_MASK,
        PIXEL_MAP_MASK,
    };

    static constexpr size_t UNMARSHALLING_MAX_VECTOR_SIZE = 65535;
    using UnmarshallingFunc = std::function<bool (Parcel&, std::shared_ptr<MaskPara>&)>;

    MaskPara()  = default;
    virtual ~MaskPara() = default;

    virtual bool Marshalling(Parcel& parcel) const;

    static bool RegisterUnmarshallingCallback(uint16_t type, UnmarshallingFunc func);

    [[nodiscard]] static bool Unmarshalling(Parcel& parcel, std::shared_ptr<MaskPara>& val);

    virtual std::shared_ptr<MaskPara> Clone() const;

    Type GetMaskParaType()
    {
        return type_;
    }

protected:
    Type type_ = Type::NONE;
};
} // namespace Rosen
} // namespace OHOS
#endif // UIEFFECT_MASK_PARA_H
