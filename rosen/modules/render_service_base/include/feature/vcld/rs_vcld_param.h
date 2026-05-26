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
#ifndef RENDER_SERVICE_BASE_FEATURE_RS_VCLD_PARAM_H
#define RENDER_SERVICE_BASE_FEATURE_RS_VCLD_PARAM_H

#include "utils/scalar.h"

namespace OHOS {
namespace Rosen {

struct RSVcldParam {
    bool enable = false;
    float radius = 0.0f;                           /* vcld radius, range [1.5~1/2*(min(img_width, img_height))] */
    float posOffset[4] = {0.0f, 0.0f, 0.0f, 0.0f};  /* video dest frame rect offset, has two value, 0 or 0.5 */

    bool operator==(const RSVcldParam& vcldParam) const
    {
        constexpr int LEFT = 0;
        constexpr int TOP = 1;
        constexpr int RIGHT = 2;
        constexpr int BOTTOM = 3;
        
        return (enable == vcldParam.enable) &&
            Drawing::IsScalarAlmostEqual(radius, vcldParam.radius) &&
            Drawing::IsScalarAlmostEqual(posOffset[LEFT], vcldParam.posOffset[LEFT]) &&
            Drawing::IsScalarAlmostEqual(posOffset[TOP], vcldParam.posOffset[TOP]) &&
            Drawing::IsScalarAlmostEqual(posOffset[RIGHT], vcldParam.posOffset[RIGHT]) &&
            Drawing::IsScalarAlmostEqual(posOffset[BOTTOM], vcldParam.posOffset[BOTTOM]);
    }
};
} // namespace Rosen
} // namespace OHOS
#endif // RENDER_SERVICE_BASE_FEATURE_RS_VCLD_PARAM_H