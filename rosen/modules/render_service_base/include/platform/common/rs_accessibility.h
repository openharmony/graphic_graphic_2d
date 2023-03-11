/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#ifndef RENDER_SERVICE_CLIENT_CORE_PIPELINE_RS_ACCESSIBILITY_H
#define RENDER_SERVICE_CLIENT_CORE_PIPELINE_RS_ACCESSIBILITY_H

#include <functional>

#include "common/rs_macros.h"

namespace OHOS {
namespace Rosen {
class RSB_EXPORT RSAccessibility {
public:
    static RSAccessibility &GetInstance();
    RSAccessibility() = default;
    virtual ~RSAccessibility() = default;

    using OnHighContrastChange = std::function<void(bool newHighContrast)>;
    virtual void ListenHighContrastChange(OnHighContrastChange callback) = 0;

private:
    RSAccessibility(RSAccessibility &&) = delete;
    RSAccessibility(const RSAccessibility &) = delete;
    RSAccessibility &operator =(RSAccessibility &&) = delete;
    RSAccessibility &operator =(const RSAccessibility &) = delete;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_PIPELINE_RS_ACCESSIBILITY_H
