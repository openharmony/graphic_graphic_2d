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

#ifndef COLOR_GAMUT_PARAM_H
#define COLOR_GAMUT_PARAM_H

#include "feature_param.h"

namespace OHOS::Rosen {
class ColorGamutParam : public FeatureParam {
public:
    ColorGamutParam() = default;
    ~ColorGamutParam() = default;

    bool IsCoveredSurfaceCloseP3() const;
    bool IsSLRCloseP3() const;

protected:
    void SetCoveredSurfaceCloseP3(bool isEnable);
    void SetSLRCloseP3(bool isEnable);

private:
    bool isCoveredSurfaceCloseP3_ = true;
    bool isSLRCloseP3_ = true;

    friend class ColorGamutParamParse;
};
} // namespace OHOS::Rosen
#endif // COLOR_GAMUT_PARAM_H