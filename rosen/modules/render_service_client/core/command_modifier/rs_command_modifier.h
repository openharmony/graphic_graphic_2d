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

#ifndef RENDER_SERVICE_CLIENT_CORE_COMMAND_MODIFIER_RS_COMMAND_MODIFIER_H
#define RENDER_SERVICE_CLIENT_CORE_COMMAND_MODIFIER_RS_COMMAND_MODIFIER_H

#include <memory>
#include <variant>

#include "command/rs_command.h"
#include "common/rs_common_def.h"

namespace OHOS {
namespace Rosen {

class RSNode;

// Modifier type enumeration for sending commands to render service.
// When pushing to foreground, CmdModifiers will be iterated to restore node properties.
enum class RSCmdModifierType : uint16_t {
    // RSNode
    NODE_NAME = 0,
    OCCLUSION_CULLING_STATUS,
    IS_P3COLOR,
    DRAW_REGION,
    USE_CMDLIST_DRAWREGION,
    MARK_NODE_GROUP,
    EXCLUDE_NODE_GROUP,
    MARK_NODE_SINGLE_FRAME_COMPOSER,
    IS_REPAINT_BOUNDARY,
    MARK_OPINC_NODE,
    IS_UIFIRST_NODE,
    IS_FORCE_UIFIRST_NODE,
    SYNC_DRAW_NODE_TYPE,
    UI_FIRST_SWITCH,
    OUT_OF_PARENT,
    IS_CROSS_NODE,
    IS_DEPTH_BACKGROUND,
    MARK_LAYER_PART_RENDER,

    // RSCanvasNode
    HDR_PRESENT,
    COLOR_GAMUT,
    IS_FREEZE,
    CLEAR_RECORDING,
    FINISH_RECORD,
    DRAW_ON_NODE,

    // RSSurfaceNode
    LEASH_PERSISTENT_ID,
    IS_SECURITY_LAYER,
    IS_SKIP_LAYER,
    IS_SNAPSHOT_SKIP_LAYER,
    HAS_FINGER_PRINT,
    COLOR_SPACE,
    ABILITY_BG_ALPHA,
    NOTIFY_UI_BUFFER_AVL,
    SET_CONTAINER_WINDOW,
    SF_IS_FREEZE,
    SF_IS_BOOT_ANIMATION,
    IS_GLOBAL_POSITION_ENABLED,
    CLONED_NODE_INFO,
    FORCE_UI_FIRST,
    ANCO_FLAGS,
    IS_SKIP_DRAW,
    WATER_MARK_ENABLED,
    ABILITY_STATE,
    API_COMPAT_VERSION,
    HARDWARE_ENABLED,
    HARDWARE_ENABLE_HINT,
    VIRTUAL_DISPLAY_ID,
    ATTACH_TO_WINDOW_CONTAINER,
    REGION_TO_BE_MAGNIFIED,
    DETACH_TO_WINDOW_CONTAINER,
    IS_FRAME_GRAVITY_NEW_VER_ENABLED,
    IS_SURFACE_BUFFER_OPAQUE,
    IS_CONTAINER_WINDOW_TRANSPARENT,
    UPDATE_COMPOSITE_LAYER,
    SET_STATIC_CACHED,
    SET_BUFFER_AVAILABLE,
    SET_HARDWARE_ENABLED,
    SET_HIDE_PRIVACY_CONTENT,
    UPDATE_SURFACE_DEFAULT_SIZE,
    IS_DEPTH_RESOURCE,
    FORCE_HARDWARE_AND_FIX_ROTATION,
    APP_ROTATION_CORRECTION,
    HDR_TYPE,
    SET_DARK_COLOR_MODE,

    // RSDepthNode
    SET_DEPTH_SPACE_TYPE,

    // RSEffectNode
    PRE_FREEZE,

    // RSRootNode
    ATTACH_ROOT_NODE,

    // RSWindowKeyFrameNode
    WKF_IS_FREEZE,
    WKF_LINKED_NODE_ID,

    MAX,
};

class RSCmdModifier : public std::enable_shared_from_this<RSCmdModifier> {
public:
    using UpdateResult = std::variant<bool, RSInterfaceErrorCode>; // All possible return value types

    RSCmdModifier(std::weak_ptr<RSNode> node) : node_(std::move(node))
    {
    }
    virtual ~RSCmdModifier() = default;

    virtual void UpdateToRender() = 0; // Must call AddCommand

    virtual UpdateResult UpdateToRenderWithResult() // Can use command, IPC, or other methods
    {
        UpdateToRender();
        return false;
    }

    virtual RSCmdModifierType GetType() const = 0;

    virtual void DumpParam(std::string& out) const
    {
        out += "{}";
    }

protected:
    std::shared_ptr<RSNode> GetNode() const
    {
        return node_.lock();
    }

    bool AddCommand(std::unique_ptr<RSCommand>& command, bool isRenderServiceCommand = false) const;
    bool AddCommand(std::unique_ptr<RSCommand>& command, bool isRenderServiceCommand,
                    FollowType followType, NodeId nodeId) const;

    std::weak_ptr<RSNode> node_;
};

} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_COMMAND_MODIFIER_RS_COMMAND_MODIFIER_H
