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

#ifndef SPOC_PARAM_H
#define SPOC_PARAM_H

#include "feature_param.h"

namespace OHOS::Rosen {
class OcclusionCullingParam : public FeatureParam {
public:
    OcclusionCullingParam() = default;
    ~OcclusionCullingParam() = default;

    static bool IsStencilPixelOcclusionCullingEnable();
    static bool IsIntraAppControlsLevelOcclusionCullingEnable();
protected:
    static void SetStencilPixelOcclusionCullingEnable(bool isEnable);
    static void SetIntraAppControlsLevelOcclusionCullingEnable(bool isEnable);
private:
    inline static bool stencilPixelOcclusionCullingEnable_ = false;
    inline static bool intraAppControlsLevelOcclusionCullingEnable_ = false;
    friend class OcclusionCullingParamParse;
};
} // namespace OHOS::Rosen
#endif // SPOC_PARAM_H