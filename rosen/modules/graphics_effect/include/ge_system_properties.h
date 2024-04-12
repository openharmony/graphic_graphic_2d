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
#ifndef GRAPHICS_EFFECT_SYSTEM_PROPERTIES_H
#define GRAPHICS_EFFECT_SYSTEM_PROPERTIES_H

#include <cstdlib>
#include <atomic>
#include <string>
#include <vector>

#include <parameter.h>
#include <parameters.h>
#include "param/sys_param.h"
#include "utils/system_properties.h"

namespace OHOS {
namespace Rosen {

class GESystemProperties final {
public:
    ~GESystemProperties() = default;

    static std::string GetEventProperty(const std::string &paraName)
    {
        return system::GetParameter(paraName, "0");
    }

    static bool GetBoolSystemProperty(const char *name, bool defaultValue)
    {
        static CachedHandle g_Handle = CachedParameterCreate(name, defaultValue ? "1" : "0");
        int changed = 0;
        const char *enable = CachedParameterGetChanged(g_Handle, &changed);
        return ConvertToInt(enable, defaultValue ? 1 : 0) != 0;
    }

    static int ConvertToInt(const char *originValue, int defaultValue)
    {
        return originValue == nullptr ? defaultValue : std::atoi(originValue);
    }

    static bool GetKawaseOriginalEnabled()
    {
        static bool kawaseOriginalEnabled =
            std::atoi((system::GetParameter("persist.sys.graphic.kawaseOriginalEnable", "0")).c_str()) != 0;
        return kawaseOriginalEnabled;
    }

    static float GetKawaseRandomColorFactor()
    {
        static float randomFactor =
            std::atof((system::GetParameter("persist.sys.graphic.kawaseFactor", "1.75")).c_str());
        return randomFactor;
    }

    static bool GetRandomColorEnabled()
    {
        static bool randomColorEnabled =
            std::atoi((system::GetParameter("persist.sys.graphic.randomColorEnable", "1")).c_str()) != 0;
        return randomColorEnabled;
    }

private:
    GESystemProperties() = default;
};

}  // namespace Rosen
}  // namespace OHOS

#endif  // GRAPHICS_EFFECT_SYSTEM_PROPERTIES_H