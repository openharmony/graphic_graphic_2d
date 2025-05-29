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

#ifndef PREVALIDATE_PARAM_H
#define PREVALIDATE_PARAM_H

#include "feature_param.h"

namespace OHOS::Rosen {
class PrevalidateParam : public FeatureParam {
public:
    PrevalidateParam() = default;
    ~PrevalidateParam() = default;

    static bool IsPrevalidateEnable();

protected:
    static void SetPrevalidateEnable(bool isEnable);

private:
    inline static bool isPrevalidateEnable_ = true;

    friend class PrevalidateParamParse;
};
} // namespace OHOS::Rosen
#endif // PREVALIDATE_PARAM_H