/*
 * Copyright (c) 2021-2023 Huawei Device Co., Ltd.
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

#ifndef ROSEN_RENDER_SERVICE_PROXY_COMMAND_RS_PROXY_NODE_COMMAND_H
#define ROSEN_RENDER_SERVICE_PROXY_COMMAND_RS_PROXY_NODE_COMMAND_H

#include "command/rs_command_templates.h"
#include "common/rs_macros.h"

namespace OHOS {
namespace Rosen {

enum RSProxyNodeCommandType : uint16_t {
    PROXY_NODE_CREATE,
    PROXY_NODE_RESET_CONTEXT_VARIABLE_CACHE,
    REMOVE_MODIFIERS,
};

class RSB_EXPORT ProxyNodeCommandHelper {
public:
    static void Create(RSContext& context, NodeId id, NodeId target);
    static void ResetContextVariableCache(RSContext& context, NodeId id);
};

ADD_COMMAND(RSProxyNodeCreate, ARG(PROXY_NODE, PROXY_NODE_CREATE, ProxyNodeCommandHelper::Create, NodeId, NodeId))
ADD_COMMAND(RSProxyNodeResetContextVariableCache,
    ARG(PROXY_NODE, PROXY_NODE_RESET_CONTEXT_VARIABLE_CACHE, ProxyNodeCommandHelper::ResetContextVariableCache, NodeId))

} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_RENDER_SERVICE_PROXY_COMMAND_RS_PROXY_NODE_COMMAND_H
