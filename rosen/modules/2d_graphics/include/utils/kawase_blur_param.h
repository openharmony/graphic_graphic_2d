/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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
#ifndef KAWASE_BLUR_PARAM_H
#define KAWASE_BLUR_PARAM_H

#include "draw/canvas.h"
#include "effect/color_filter.h"
#include "image/image.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
struct KawaseParameters {
    Drawing::Rect src;
    Drawing::Rect dst;
    int radius = 0;
    std::shared_ptr<Drawing::ColorFilter> colorFilter = nullptr;
    float alpha = 0.f;
};

struct KawaseProperties {
    bool useAF = false;
    bool useKawaseOriginal = false;
    bool useRandomColor = true;
    float randomColorFactor = 1.75f;
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif // KAWASE_BLUR_PARAM_H