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

#ifndef SAFUZZ_RS_DISPLAY_NODE_COMMAND_UTILS_H
#define SAFUZZ_RS_DISPLAY_NODE_COMMAND_UTILS_H

#include "command/rs_display_node_command.h"

#include "command/rs_command_utils.h"

namespace OHOS {
namespace Rosen {
class RSDisplayNodeCommandUtils {
public:
    ADD_RANDOM_COMMAND_WITH_PARAM_2(RSDisplayNodeCreate, Uint64, RSDisplayNodeConfig);
    ADD_RANDOM_COMMAND_WITH_PARAM_2(RSDisplayNodeSetScreenId, Uint64, Uint64);
    ADD_RANDOM_COMMAND_WITH_PARAM_2(RSDisplayNodeSetSecurityDisplay, Uint64, Bool);
    ADD_RANDOM_COMMAND_WITH_PARAM_2(RSDisplayNodeSetDisplayMode, Uint64, RSDisplayNodeConfig);
    ADD_RANDOM_COMMAND_WITH_PARAM_2(RSDisplayNodeSetScreenRotation, Uint64, ScreenRotation);
    ADD_RANDOM_COMMAND_WITH_PARAM_2(RSDisplayNodeSetBootAnimation, Uint64, Bool);
    ADD_RANDOM_COMMAND_WITH_PARAM_1(RSDisplayNodeAddToTree, Uint64);
    ADD_RANDOM_COMMAND_WITH_PARAM_1(RSDisplayNodeRemoveFromTree, Uint64);
    ADD_RANDOM_COMMAND_WITH_PARAM_3(RSDisplayNodeSetNodePid, Uint64, Int32Vector, Int32);
};
} // namespace Rosen
} // namespace OHOS

#endif // SAFUZZ_RS_DISPLAY_NODE_COMMAND_UTILS_H
