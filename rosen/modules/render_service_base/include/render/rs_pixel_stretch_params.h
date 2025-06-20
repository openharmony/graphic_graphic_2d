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
#ifndef RENDER_SERVICE_BASE_RENDER_RENDER_RS_PIXEL_STRETCH_PARAMS_H
#define RENDER_SERVICE_BASE_RENDER_RENDER_RS_PIXEL_STRETCH_PARAMS_H

#include <cstdint>
#include "common/rs_macros.h"

namespace OHOS {
namespace Rosen {

class RSB_EXPORT RSPixelStretchParams {
public:
    explicit RSPixelStretchParams() {}
    RSPixelStretchParams(float offsetX, float offsetY, float offsetZ, float offsetW,
                         int tileMode, float width, float height) :
                         offsetX_(offsetX), offsetY_(offsetY), offsetZ_(offsetZ), offsetW_(offsetW),
                         tileMode_(tileMode), width_(width), height_(height) {}

    ~RSPixelStretchParams() = default;

    // offset percent
    float offsetX_ = 0.0;
    float offsetY_ = 0.0;
    float offsetZ_ = 0.0;
    float offsetW_ = 0.0;

    int tileMode_ = 0;
    float width_ = 0.0;
    float height_ = 0.0;
};

} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_BASE_RENDER_RENDER_RS_PIXEL_STRETCH_PARAMS_H
