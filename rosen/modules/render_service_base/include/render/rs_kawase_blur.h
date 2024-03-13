/*
 * Copyright (c) 2023-2023 Huawei Device Co., Ltd.
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
#ifndef RENDER_SERVICE_CLIENT_CORE_RENDER_RS_KAWASE_BLUR_H
#define RENDER_SERVICE_CLIENT_CORE_RENDER_RS_KAWASE_BLUR_H

#include "utils/kawase_blur_utils.h"

namespace OHOS {
namespace Rosen {
class KawaseBlurFilter {
public:
    static bool ApplyDrawingKawaseBlur(Drawing::Canvas& canvas, Drawing::Brush& brush,
        const std::shared_ptr<Drawing::Image>& image, const Drawing::KawaseParameters& blurParam);
};
} // namespace Rosen
} // namespace OHOS
#endif // RENDER_SERVICE_CLIENT_CORE_RENDER_RS_KAWASE_BLUR_H
