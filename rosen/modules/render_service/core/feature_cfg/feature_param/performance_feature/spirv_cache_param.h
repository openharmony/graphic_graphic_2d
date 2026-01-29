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

#ifndef SPIRV_CACHE_PARAM_H
#define SPIRV_CACHE_PARAM_H

#include <string>
#include "feature_param.h"

namespace OHOS::Rosen {
class SpirvCacheParam : public FeatureParam {
public:
    SpirvCacheParam() = default;
    ~SpirvCacheParam() = default;

    static std::string GetSpirvCacheSize();

protected:
    static void SetSpirvCacheSize(std::string size);

private:
    inline static std::string spirvCacheSize_ = "5"; // MB

    friend class SpirvCacheParamParse;
};
} // namespace OHOS::Rosen
#endif // SPIRV_CACHE_PARAM_H