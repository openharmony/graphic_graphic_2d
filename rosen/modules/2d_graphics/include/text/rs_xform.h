/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#ifndef RS_XFORM_H
#define RS_XFORM_H

#include <cmath>
#include <stdint.h>
#include <string>

#include "utils/scalar.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
struct RSXform {
    static RSXform Make(scalar cos, scalar sin, scalar tx, scalar ty)
    {
        RSXform xform = { cos, sin, tx, ty };
        return xform;
    }

    inline void Dump(std::string& out) const
    {
        out += "[cos:" + std::to_string(cos_);
        out += " sin:" + std::to_string(sin_);
        out += " tx:" + std::to_string(tx_);
        out += " ty:" + std::to_string(ty_);
        out += ']';
    }

    scalar cos_;
    scalar sin_;
    scalar tx_;
    scalar ty_;
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif