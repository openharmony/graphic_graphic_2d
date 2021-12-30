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

#ifndef RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_MOTION_PATH_OPTION_H
#define RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_MOTION_PATH_OPTION_H

#include <string>

#include "animation/rs_animation_common.h"
#include "common/rs_macros.h"

namespace OHOS {
namespace Rosen {

class RS_EXPORT RSMotionPathOption {
public:
    RSMotionPathOption(const std::string& path) : path_(path) {}

    virtual ~RSMotionPathOption() = default;

    std::string GetPath() const
    {
        return path_;
    }

    void SetBeginFraction(float beginFraction)
    {
        beginFraction_ = beginFraction;
    }

    float GetBeginFraction() const
    {
        return beginFraction_;
    }

    void SetEndFraction(float endFraction)
    {
        endFraction_ = endFraction;
    }

    float GetEndFraction() const
    {
        return endFraction_;
    }

    void SetRotationMode(const RotationMode& rotationMode)
    {
        rotationMode_ = rotationMode;
    }

    RotationMode GetRotationMode() const
    {
        return rotationMode_;
    }

private:
    std::string path_;
    float beginFraction_ { FRACTION_MIN };
    float endFraction_ { FRACTION_MAX };
    RotationMode rotationMode_ { RotationMode::ROTATE_NONE };
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_MOTION_PATH_OPTION_H
