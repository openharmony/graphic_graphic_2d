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

#ifndef ROSEN_RENDER_SERVICE_BASE_RS_IPC_INTERFACE_CODE_UNDERLYING_TYPE_H
#define ROSEN_RENDER_SERVICE_BASE_RS_IPC_INTERFACE_CODE_UNDERLYING_TYPE_H

#include <stdint.h>
#include <string>

#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {
/*
 * Standardized definition of the underlying type of IPC interface code enum classes
 */
using CodeUnderlyingType = uint32_t;

/*
 * cast the enum value of an interface code to its standardized underlying type
 */
template<typename CodeEnumType>
inline CodeUnderlyingType CastEnumToUnderlying(CodeEnumType enumCode)
{
    return static_cast<CodeUnderlyingType>(enumCode);
}

/*
 * check whether the underlying type of CodeEnumType accords with standardized definition
 */
template<typename CodeEnumType>
inline void CheckCodeUnderlyingTypeStandardized(const std::string& CodeEnumTypeName = "RSInterfaceCode")
{
    constexpr bool isStandardized = std::is_same<std::underlying_type_t<CodeEnumType>, CodeUnderlyingType>::value;
    if (!isStandardized) {
        RS_LOGW("The underlying type of %{public}s does not accord with standardized definition.",
            CodeEnumTypeName.c_str());
    }
}
} // namespace Rosen
} // namespace OHOS
#endif // ROSEN_RENDER_SERVICE_BASE_RS_IPC_INTERFACE_CODE_UNDERLYING_TYPE_H
