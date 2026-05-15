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

#ifndef RENDER_SERVICE_BASE_COMMON_RS_TUNNEL_LAYER_UTILS_H
#define RENDER_SERVICE_BASE_COMMON_RS_TUNNEL_LAYER_UTILS_H

#include <cstdint>

#include <parameters.h>

#include "surface_type.h"

namespace OHOS {
namespace Rosen {
inline constexpr const char* TUNNEL_DEBUG_PREFIX = "TUNNEL_DEBUG ";

inline bool IsNewTunnelEnabled()
{
    return system::GetBoolParameter("rosen.debug.new_tunnel", false);
}

inline constexpr bool IsNewTunnelProperty(uint32_t property)
{
    return (property & TUNNEL_PROP_BUFFER_ADDR) == TUNNEL_PROP_BUFFER_ADDR;
}

inline constexpr bool IsNewTunnelLayerValid(uint64_t tunnelLayerId, uint32_t property)
{
    return tunnelLayerId != 0 && IsNewTunnelProperty(property);
}
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_BASE_COMMON_RS_TUNNEL_LAYER_UTILS_H
