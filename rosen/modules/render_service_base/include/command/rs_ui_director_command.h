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

#ifndef ROSEN_RENDER_SERVICE_BASE_COMMAND_RS_UI_DIRECTOR_COMMAND_H
#define ROSEN_RENDER_SERVICE_BASE_COMMAND_RS_UI_DIRECTOR_COMMAND_H

#include "command/rs_command_templates.h"
#include "common/rs_macros.h"

namespace OHOS {
namespace Rosen {

enum RSUIDirectorCommandType : uint16_t {
    GO_CREATE = 0,
    GO_RESUME,
    GO_FOREGROUND,
    GO_BACKGROUND,
    GO_STOP,
    GO_DESTROY,
};

class RSB_EXPORT RSUIDirectorCommandHelper {
public:
    static void GoCreate(RSContext& context, NodeId nodeId, uint64_t token);
    static void GoResume(RSContext& context, NodeId nodeId, uint64_t token);
    static void GoForeground(RSContext& context, NodeId nodeId, uint64_t token);
    static void GoBackground(RSContext& context, NodeId nodeId, uint64_t token);
    static void GoStop(RSContext& context, NodeId nodeId, uint64_t token);
    static void GoDestroy(RSContext& context, NodeId nodeId, uint64_t token);
};

ADD_COMMAND(RSUIDirectorGoCreate,
    ARG(PERMISSION_APP, UI_DIRECTOR, GO_CREATE,
        RSUIDirectorCommandHelper::GoCreate, NodeId, uint64_t))
ADD_COMMAND(RSUIDirectorGoResume,
    ARG(PERMISSION_APP, UI_DIRECTOR, GO_RESUME,
        RSUIDirectorCommandHelper::GoResume, NodeId, uint64_t))
ADD_COMMAND(RSUIDirectorGoForeground,
    ARG(PERMISSION_APP, UI_DIRECTOR, GO_FOREGROUND,
        RSUIDirectorCommandHelper::GoForeground, NodeId, uint64_t))
ADD_COMMAND(RSUIDirectorGoBackground,
    ARG(PERMISSION_APP, UI_DIRECTOR, GO_BACKGROUND,
        RSUIDirectorCommandHelper::GoBackground, NodeId, uint64_t))
ADD_COMMAND(RSUIDirectorGoStop,
    ARG(PERMISSION_APP, UI_DIRECTOR, GO_STOP,
        RSUIDirectorCommandHelper::GoStop, NodeId, uint64_t))
ADD_COMMAND(RSUIDirectorGoDestroy,
    ARG(PERMISSION_APP, UI_DIRECTOR, GO_DESTROY,
        RSUIDirectorCommandHelper::GoDestroy, NodeId, uint64_t))
} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_RENDER_SERVICE_BASE_COMMAND_RS_UI_DIRECTOR_COMMAND_H