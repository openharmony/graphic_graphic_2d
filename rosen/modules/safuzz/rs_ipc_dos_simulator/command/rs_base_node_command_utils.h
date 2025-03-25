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

#ifndef SAFUZZ_RS_BASE_NODE_COMMAND_UTILS_H
#define SAFUZZ_RS_BASE_NODE_COMMAND_UTILS_H

#include "command/rs_base_node_command.h"

#include "command/rs_command_utils.h"

namespace OHOS {
namespace Rosen {
class RSBaseNodeCommandUtils {
public:
    ADD_RANDOM_COMMAND_WITH_PARAM_1(RSBaseNodeDestroy, Uint64);
    ADD_RANDOM_COMMAND_WITH_PARAM_3(RSBaseNodeAddChild, Uint64, Uint64, Int32);
    ADD_RANDOM_COMMAND_WITH_PARAM_3(RSBaseNodeMoveChild, Uint64, Uint64, Int32);
    ADD_RANDOM_COMMAND_WITH_PARAM_2(RSBaseNodeRemoveChild, Uint64, Uint64);
    ADD_RANDOM_COMMAND_WITH_PARAM_3(RSBaseNodeAddCrossParentChild, Uint64, Uint64, Int32);
    ADD_RANDOM_COMMAND_WITH_PARAM_3(RSBaseNodeRemoveCrossParentChild, Uint64, Uint64, Uint64);
    ADD_RANDOM_COMMAND_WITH_PARAM_2(RSBaseNodeSetIsCrossNode, Uint64, Bool);
    ADD_RANDOM_COMMAND_WITH_PARAM_5(RSBaseNodeAddCrossScreenChild, Uint64, Uint64, Uint64, Int32, Bool);
    ADD_RANDOM_COMMAND_WITH_PARAM_2(RSBaseNodeRemoveCrossScreenChild, Uint64, Uint64);
    ADD_RANDOM_COMMAND_WITH_PARAM_1(RSBaseNodeRemoveFromTree, Uint64);
    ADD_RANDOM_COMMAND_WITH_PARAM_1(RSBaseNodeClearChild, Uint64);
};
} // namespace Rosen
} // namespace OHOS

#endif // SAFUZZ_RS_BASE_NODE_COMMAND_UTILS_H
