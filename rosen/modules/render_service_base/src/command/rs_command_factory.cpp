/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "command/rs_command_factory.h"

#include "platform/common/rs_log.h"

#ifdef ROSEN_OHOS
// manually instantiate all RScommands (this is when the registry happens)
#define ROSEN_INSTANTIATE_COMMAND_TEMPLATE
// node
#include "command/rs_base_node_command.h"
#include "command/rs_canvas_node_command.h"
#include "command/rs_display_node_command.h"
#include "command/rs_node_command.h"
#include "command/rs_root_node_command.h"
#include "command/rs_surface_node_command.h"
// animation
#include "command/rs_animation_command.h"
// read showing property commands
#include "command/rs_node_showing_command.h"
#undef ROSEN_INSTANTIATE_COMMAND_TEMPLATE

namespace OHOS {
namespace Rosen {

namespace {
inline uint32_t MakeKey(uint16_t commandType, uint16_t commandSubType)
{
    // 16: concat two uint16 into uint32
    return ((uint32_t)commandType << 16) | commandSubType;
}
} // namespace

RSCommandFactory& RSCommandFactory::Instance()
{
    static RSCommandFactory instance;
    return instance;
}

void RSCommandFactory::Register(uint16_t type, uint16_t subtype, UnmarshallingFunc func)
{
    auto result = unmarshallingFuncLUT_.try_emplace(MakeKey(type, subtype), func);
    if (!result.second) {
        ROSEN_LOGE("RSCommandFactory::Register, Duplicate command & sub_command detected! type: %d subtype: %d", type,
            subtype);
    }
}

UnmarshallingFunc RSCommandFactory::GetUnmarshallingFunc(uint16_t type, uint16_t subtype)
{
    auto it = unmarshallingFuncLUT_.find(MakeKey(type, subtype));
    if (it == unmarshallingFuncLUT_.end()) {
        ROSEN_LOGE("RSCommandFactory::GetUnmarshallingFunc, Func is not found, type=%d subtype=%d", type, subtype);
        return nullptr;
    }
    return it->second;
}
#endif // ROSEN_OHOS

} // namespace Rosen
} // namespace OHOS
