/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#ifndef VIRTUAL_SCREEN_PARALLEL_PARAM_H
#define VIRTUAL_SCREEN_PARALLEL_PARAM_H

#include "feature_param.h"

namespace OHOS::Rosen {
class VirtualScreenParallelParam : public FeatureParam {
public:
    VirtualScreenParallelParam() = default;
    ~VirtualScreenParallelParam() = default;
    static bool IsVirtualScreenParallelEnabled();

protected:
    static void SetVirtualScreenParallelEnabled(bool enabled);

private:
    inline static bool useVirtualScreenParallelEnabled_ = false;

    friend class VirtualScreenParallelParamParse;
};
} // namespace OHOS::Rosen
#endif // VIRTUAL_SCREEN_PARALLEL_PARAM_H