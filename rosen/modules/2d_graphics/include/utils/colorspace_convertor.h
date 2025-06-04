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

#ifndef DRAWING_COLORSPACE_CONVERTOR_H
#define DRAWING_COLORSPACE_CONVERTOR_H

#include "effect/color_space.h"
#include "color_space.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
class DRAWING_API ColorSpaceConvertor {
public:
    static std::shared_ptr<OHOS::Rosen::Drawing::ColorSpace> ColorSpaceConvertToDrawingColorSpace(
        std::shared_ptr<OHOS::ColorManager::ColorSpace> colorspace);
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif