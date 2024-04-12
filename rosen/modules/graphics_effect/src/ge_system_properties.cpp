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

#include "ge_system_properties.h"

namespace OHOS {
namespace Rosen {

std::string GESystemProperties::GetEventProperty(const std::string &paraName)
{
    return system::GetParameter(paraName, "0");
}

bool GESystemProperties::GetBoolSystemProperty(const char *name, bool defaultValue)
{
    static CachedHandle g_Handle = CachedParameterCreate(name, defaultValue ? "1" : "0");
    int changed = 0;
    const char *enable = CachedParameterGetChanged(g_Handle, &changed);
    return ConvertToInt(enable, defaultValue ? 1 : 0) != 0;
}

int GESystemProperties::ConvertToInt(const char *originValue, int defaultValue)
{
    return originValue == nullptr ? defaultValue : std::atoi(originValue);
}

} // namespace Rosen
} // namespace OHOS
