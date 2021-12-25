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

#ifndef FRAMEWORKS_ANIMATION_SERVER_SERVER_INCLUDE_ROTATION_ANIMATION_H
#define FRAMEWORKS_ANIMATION_SERVER_SERVER_INCLUDE_ROTATION_ANIMATION_H

#include <memory>

#include <graphic_common.h>
#include <shader.h>
#include <texture.h>

#include "array.h"

namespace OHOS {
struct RotationAnimationParam {
    std::shared_ptr<struct Array> data;
    int32_t width; // data width
    int32_t height; // data height
    int64_t startTime; // ms
    int64_t duration; // ms
    double degree;
};

class RotationAnimation {
public:
    GSError Init(struct RotationAnimationParam &param);
    bool Draw();

private:
    std::unique_ptr<Texture> texture_ = nullptr;
    std::unique_ptr<Shader> shader_ = nullptr;
    struct RotationAnimationParam param_ = {};
};
} // namespace OHOS

#endif // FRAMEWORKS_ANIMATION_SERVER_SERVER_INCLUDE_ROTATION_ANIMATION_H
