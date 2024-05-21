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
#ifndef UIEFFECT_EFFECT_BRIGHTNESS_BLENDER_H
#define UIEFFECT_EFFECT_BRIGHTNESS_BLENDER_H

#include "common/rs_vector3.h"
#include "blender.h"

namespace OHOS {
namespace Rosen {
class BrightnessBlender : public Blender {
public:
    BrightnessBlender()
    {
        this->blenderType_ = Blender::BRIGHTNESS_BLENDER;
    }
    ~BrightnessBlender() {}

    float cubicRate_;
    float quadRate_;
    float linearRate_;
    float degree_;
    float saturation_;
    Vector3f positiveCoeff_;
    Vector3f negetiveCoeff_;
    float fraction_;
};
} // namespace Rosen
} // namespace OHOS
#endif // UIEFFECT_EFFECT_BRIGHTNESS_BLENDER_H