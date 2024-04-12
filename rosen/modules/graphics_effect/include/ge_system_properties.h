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
#ifndef GRAPHICS_EFFECT_GE_SYSTEM_PROPERTIES_H
#define GRAPHICS_EFFECT_GE_SYSTEM_PROPERTIES_H

#include <atomic>
#include <cstdlib>
#include <parameter.h>
#include <parameters.h>
#include <string>
#include <vector>

#include "param/sys_param.h"

#include "utils/system_properties.h"

namespace OHOS {
namespace Rosen {

class GESystemProperties final {
public:
    ~GESystemProperties() = default;

    static std::string GetEventProperty(const std::string &paraName);
    static bool GetBoolSystemProperty(const char *name, bool defaultValue);
    static int ConvertToInt(const char *originValue, int defaultValue);

private:
    GESystemProperties() = default;
};

} // namespace Rosen
} // namespace OHOS

#endif // GRAPHICS_EFFECT_GE_SYSTEM_PROPERTIES_H
