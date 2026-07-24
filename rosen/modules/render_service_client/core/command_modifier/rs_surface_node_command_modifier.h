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

#ifndef RENDER_SERVICE_CLIENT_CORE_COMMAND_MODIFIER_RS_SURFACE_NODE_COMMAND_MODIFIER_H
#define RENDER_SERVICE_CLIENT_CORE_COMMAND_MODIFIER_RS_SURFACE_NODE_COMMAND_MODIFIER_H

#include <memory>

#include "command_modifier/rs_command_modifier.h"
#include "common/rs_common_def.h"
#include "ipc_callbacks/buffer_available_callback.h"
#include "iremote_stub.h"
#include "screen_manager/screen_types.h"

namespace OHOS {
namespace Rosen {

// Only to bridge std::function<void()> to sptr<RSIBufferAvailableCallback>
class RSBufferAvailableCallbackWrapper : public IRemoteStub<RSIBufferAvailableCallback> {
public:
    explicit RSBufferAvailableCallbackWrapper(std::function<void()> callback) : callback_(callback)
    {
    }
    ~RSBufferAvailableCallbackWrapper() override = default;

    void OnBufferAvailable() override
    {
        if (callback_) {
            callback_();
        }
    }

    const std::function<void()>& GetCallback() const
    {
        return callback_;
    }
private:
    std::function<void()> callback_;
};

class RSNode;

struct LeashPersistentIdCmdParam {
    LeashPersistentId leashPersistentId_;
};

class LeashPersistentIdCmdModifier : public RSCmdModifier {
public:
    static inline constexpr auto Type = RSCmdModifierType::LEASH_PERSISTENT_ID;

    LeashPersistentIdCmdModifier(std::weak_ptr<RSNode> node, const LeashPersistentIdCmdParam& param)
        : RSCmdModifier(std::move(node)), param_(param)
    {
    }

    RSCmdModifierType GetType() const override
    {
        return Type;
    }

    bool SetParam(const LeashPersistentIdCmdParam& param)
    {
        param_ = param;
        return true;
    }

    void UpdateToRender() override;

    void DumpParam(std::string& out) const override;

    const LeashPersistentIdCmdParam& GetParam() const
    {
        return param_;
    }

private:
    LeashPersistentIdCmdParam param_;
};

struct SecurityLayerCmdParam {
    bool isSecurityLayer_;
};

class SecurityLayerCmdModifier : public RSCmdModifier {
public:
    static inline constexpr auto Type = RSCmdModifierType::IS_SECURITY_LAYER;

    SecurityLayerCmdModifier(std::weak_ptr<RSNode> node, const SecurityLayerCmdParam& param)
        : RSCmdModifier(std::move(node)), param_(param)
    {
    }

    RSCmdModifierType GetType() const override
    {
        return Type;
    }

    bool SetParam(const SecurityLayerCmdParam& param)
    {
        param_ = param;
        return true;
    }

    void UpdateToRender() override;

    void DumpParam(std::string& out) const override;

    const SecurityLayerCmdParam& GetParam() const
    {
        return param_;
    }

private:
    SecurityLayerCmdParam param_;
};

struct SkipLayerCmdParam {
    bool isSkipLayer_;
};

class SkipLayerCmdModifier : public RSCmdModifier {
public:
    static inline constexpr auto Type = RSCmdModifierType::IS_SKIP_LAYER;

    SkipLayerCmdModifier(std::weak_ptr<RSNode> node, const SkipLayerCmdParam& param)
        : RSCmdModifier(std::move(node)), param_(param)
    {
    }

    RSCmdModifierType GetType() const override
    {
        return Type;
    }

    bool SetParam(const SkipLayerCmdParam& param)
    {
        param_ = param;
        return true;
    }

    void UpdateToRender() override;

    void DumpParam(std::string& out) const override;

    const SkipLayerCmdParam& GetParam() const
    {
        return param_;
    }

private:
    SkipLayerCmdParam param_;
};

struct SnapshotSkipLayerCmdParam {
    bool isSnapshotSkipLayer_;
};

class SnapshotSkipLayerCmdModifier : public RSCmdModifier {
public:
    static inline constexpr auto Type = RSCmdModifierType::IS_SNAPSHOT_SKIP_LAYER;

    SnapshotSkipLayerCmdModifier(std::weak_ptr<RSNode> node, const SnapshotSkipLayerCmdParam& param)
        : RSCmdModifier(std::move(node)), param_(param)
    {
    }

    RSCmdModifierType GetType() const override
    {
        return Type;
    }

    bool SetParam(const SnapshotSkipLayerCmdParam& param)
    {
        param_ = param;
        return true;
    }

    void UpdateToRender() override;

    void DumpParam(std::string& out) const override;

    const SnapshotSkipLayerCmdParam& GetParam() const
    {
        return param_;
    }

private:
    SnapshotSkipLayerCmdParam param_;
};

struct HasFingerprintCmdParam {
    bool hasFingerprint_;
};

class HasFingerprintCmdModifier : public RSCmdModifier {
public:
    static inline constexpr auto Type = RSCmdModifierType::HAS_FINGER_PRINT;

    HasFingerprintCmdModifier(std::weak_ptr<RSNode> node, const HasFingerprintCmdParam& param)
        : RSCmdModifier(std::move(node)), param_(param)
    {
    }

    RSCmdModifierType GetType() const override
    {
        return Type;
    }

    bool SetParam(const HasFingerprintCmdParam& param)
    {
        param_ = param;
        return true;
    }

    void UpdateToRender() override;

    void DumpParam(std::string& out) const override;

    const HasFingerprintCmdParam& GetParam() const
    {
        return param_;
    }

private:
    HasFingerprintCmdParam param_;
};

struct ColorSpaceCmdParam {
    GraphicColorGamut colorSpace_;
};

class ColorSpaceCmdModifier : public RSCmdModifier {
public:
    static inline constexpr auto Type = RSCmdModifierType::COLOR_SPACE;

    ColorSpaceCmdModifier(std::weak_ptr<RSNode> node, const ColorSpaceCmdParam& param)
        : RSCmdModifier(std::move(node)), param_(param)
    {
    }

    RSCmdModifierType GetType() const override
    {
        return Type;
    }

    bool SetParam(const ColorSpaceCmdParam& param)
    {
        param_ = param;
        return true;
    }

    void UpdateToRender() override;

    void DumpParam(std::string& out) const override;

    const ColorSpaceCmdParam& GetParam() const
    {
        return param_;
    }

private:
    ColorSpaceCmdParam param_;
};

struct AbilityBGAlphaCmdParam {
    uint8_t abilityBGAlpha_;
};

class AbilityBGAlphaCmdModifier : public RSCmdModifier {
public:
    static inline constexpr auto Type = RSCmdModifierType::ABILITY_BG_ALPHA;

    AbilityBGAlphaCmdModifier(std::weak_ptr<RSNode> node, const AbilityBGAlphaCmdParam& param)
        : RSCmdModifier(std::move(node)), param_(param)
    {
    }

    RSCmdModifierType GetType() const override
    {
        return Type;
    }

    bool SetParam(const AbilityBGAlphaCmdParam& param)
    {
        param_ = param;
        return true;
    }

    void UpdateToRender() override;

    void DumpParam(std::string& out) const override;

    const AbilityBGAlphaCmdParam& GetParam() const
    {
        return param_;
    }

private:
    AbilityBGAlphaCmdParam param_;
};

struct NotifyUIBufferAvailableCmdParam {
    bool notifyUIBufferAvailable_;
};

class NotifyUIBufferAvailableCmdModifier : public RSCmdModifier {
public:
    static inline constexpr auto Type = RSCmdModifierType::NOTIFY_UI_BUFFER_AVL;

    NotifyUIBufferAvailableCmdModifier(std::weak_ptr<RSNode> node, const NotifyUIBufferAvailableCmdParam& param)
        : RSCmdModifier(std::move(node)), param_(param)
    {
    }

    RSCmdModifierType GetType() const override
    {
        return Type;
    }

    bool SetParam(const NotifyUIBufferAvailableCmdParam& param)
    {
        param_ = param;
        return true;
    }

    void UpdateToRender() override;

    void DumpParam(std::string& out) const override;

    const NotifyUIBufferAvailableCmdParam& GetParam() const
    {
        return param_;
    }

private:
    NotifyUIBufferAvailableCmdParam param_;
};

struct ContainerWindowCmdParam {
    bool hasContainerWindow_;
    RRect containerWindowRrect_;
};

class ContainerWindowCmdModifier : public RSCmdModifier {
public:
    static inline constexpr auto Type = RSCmdModifierType::SET_CONTAINER_WINDOW;

    ContainerWindowCmdModifier(std::weak_ptr<RSNode> node, const ContainerWindowCmdParam& param)
        : RSCmdModifier(std::move(node)), param_(param)
    {
    }

    RSCmdModifierType GetType() const override
    {
        return Type;
    }

    bool SetParam(const ContainerWindowCmdParam& param)
    {
        param_ = param;
        return true;
    }

    void UpdateToRender() override;

    void DumpParam(std::string& out) const override;

    const ContainerWindowCmdParam& GetParam() const
    {
        return param_;
    }

private:
    ContainerWindowCmdParam param_;
};

struct FreezeCmdParam {
    bool isFreeze_;
    bool isMarkedByUI_;
};

class FreezeCmdModifier : public RSCmdModifier {
public:
    static inline constexpr auto Type = RSCmdModifierType::SF_IS_FREEZE;

    FreezeCmdModifier(std::weak_ptr<RSNode> node, const FreezeCmdParam& param)
        : RSCmdModifier(std::move(node)), param_(param)
    {
    }

    RSCmdModifierType GetType() const override
    {
        return Type;
    }

    bool SetParam(const FreezeCmdParam& param)
    {
        param_ = param;
        return true;
    }

    void UpdateToRender() override;

    void DumpParam(std::string& out) const override;

    const FreezeCmdParam& GetParam() const
    {
        return param_;
    }

private:
    FreezeCmdParam param_;
};

struct HardwareEnableHintCmdParam {
    bool isHardwareEnabled_;
};

class HardwareEnableHintCmdModifier : public RSCmdModifier {
public:
    static inline constexpr auto Type = RSCmdModifierType::HARDWARE_ENABLE_HINT;

    HardwareEnableHintCmdModifier(std::weak_ptr<RSNode> node, const HardwareEnableHintCmdParam& param)
        : RSCmdModifier(std::move(node)), param_(param)
    {
    }

    RSCmdModifierType GetType() const override
    {
        return Type;
    }

    bool SetParam(const HardwareEnableHintCmdParam& param)
    {
        param_ = param;
        return true;
    }

    void UpdateToRender() override;

    void DumpParam(std::string& out) const override;

    const HardwareEnableHintCmdParam& GetParam() const
    {
        return param_;
    }

private:
    HardwareEnableHintCmdParam param_;
};

struct HardwareEnabledCmdParam {
    bool isHardwareEnabled_;
    SelfDrawingNodeType isSelfDrawingNodeType_;
    bool isDynamicHardwareEnable_;
};

class HardwareEnabledCmdModifier : public RSCmdModifier {
public:
    static inline constexpr auto Type = RSCmdModifierType::HARDWARE_ENABLED;

    HardwareEnabledCmdModifier(std::weak_ptr<RSNode> node, const HardwareEnabledCmdParam& param)
        : RSCmdModifier(std::move(node)), param_(param)
    {
    }

    RSCmdModifierType GetType() const override
    {
        return Type;
    }

    bool SetParam(const HardwareEnabledCmdParam& param)
    {
        param_ = param;
        return true;
    }

    void UpdateToRender() override;

    RSCmdModifier::UpdateResult UpdateToRenderWithResult() override;

    void DumpParam(std::string& out) const override;

    const HardwareEnabledCmdParam& GetParam() const
    {
        return param_;
    }

private:
    HardwareEnabledCmdParam param_;
};

struct BootAnimationCmdParam {
    bool isBootAnimation_;
};

class BootAnimationCmdModifier : public RSCmdModifier {
public:
    static inline constexpr auto Type = RSCmdModifierType::SF_IS_BOOT_ANIMATION;

    BootAnimationCmdModifier(std::weak_ptr<RSNode> node, const BootAnimationCmdParam& param)
        : RSCmdModifier(std::move(node)), param_(param)
    {
    }

    RSCmdModifierType GetType() const override
    {
        return Type;
    }

    bool SetParam(const BootAnimationCmdParam& param)
    {
        param_ = param;
        return true;
    }

    void UpdateToRender() override;

    void DumpParam(std::string& out) const override;

    const BootAnimationCmdParam& GetParam() const
    {
        return param_;
    }

private:
    BootAnimationCmdParam param_;
};

struct GlobalPositionEnabledCmdParam {
    bool isGlobalPositionEnabled_;
};

class GlobalPositionEnabledCmdModifier : public RSCmdModifier {
public:
    static inline constexpr auto Type = RSCmdModifierType::IS_GLOBAL_POSITION_ENABLED;

    GlobalPositionEnabledCmdModifier(std::weak_ptr<RSNode> node, const GlobalPositionEnabledCmdParam& param)
        : RSCmdModifier(std::move(node)), param_(param)
    {
    }

    RSCmdModifierType GetType() const override
    {
        return Type;
    }

    bool SetParam(const GlobalPositionEnabledCmdParam& param)
    {
        param_ = param;
        return true;
    }

    void UpdateToRender() override;

    void DumpParam(std::string& out) const override;

    const GlobalPositionEnabledCmdParam& GetParam() const
    {
        return param_;
    }

private:
    GlobalPositionEnabledCmdParam param_;
};

struct ClonedNodeInfoCmdParam {
    NodeId clonedNodeId_;
    bool clonedNeedOffscreen_;
    bool clonedIsRelated_;
};

class ClonedNodeInfoCmdModifier : public RSCmdModifier {
public:
    static inline constexpr auto Type = RSCmdModifierType::CLONED_NODE_INFO;

    ClonedNodeInfoCmdModifier(std::weak_ptr<RSNode> node, const ClonedNodeInfoCmdParam& param)
        : RSCmdModifier(std::move(node)), param_(param)
    {
    }

    RSCmdModifierType GetType() const override
    {
        return Type;
    }

    bool SetParam(const ClonedNodeInfoCmdParam& param)
    {
        param_ = param;
        return true;
    }

    void UpdateToRender() override;

    void DumpParam(std::string& out) const override;

    const ClonedNodeInfoCmdParam& GetParam() const
    {
        return param_;
    }

private:
    ClonedNodeInfoCmdParam param_;
};

struct ForceUIFirstCmdParam {
    bool forceUIFirst_;
};

class ForceUIFirstCmdModifier : public RSCmdModifier {
public:
    static inline constexpr auto Type = RSCmdModifierType::FORCE_UI_FIRST;

    ForceUIFirstCmdModifier(std::weak_ptr<RSNode> node, const ForceUIFirstCmdParam& param)
        : RSCmdModifier(std::move(node)), param_(param)
    {
    }

    RSCmdModifierType GetType() const override
    {
        return Type;
    }

    bool SetParam(const ForceUIFirstCmdParam& param)
    {
        param_ = param;
        return true;
    }

    void UpdateToRender() override;

    void DumpParam(std::string& out) const override;

    const ForceUIFirstCmdParam& GetParam() const
    {
        return param_;
    }

private:
    ForceUIFirstCmdParam param_;
};

struct AncoFlagsCmdParam {
    uint32_t ancoFlags_;
};

class AncoFlagsCmdModifier : public RSCmdModifier {
public:
    static inline constexpr auto Type = RSCmdModifierType::ANCO_FLAGS;

    AncoFlagsCmdModifier(std::weak_ptr<RSNode> node, const AncoFlagsCmdParam& param)
        : RSCmdModifier(std::move(node)), param_(param)
    {
    }

    RSCmdModifierType GetType() const override
    {
        return Type;
    }

    bool SetParam(const AncoFlagsCmdParam& param)
    {
        param_ = param;
        return true;
    }

    void UpdateToRender() override;

    void DumpParam(std::string& out) const override;

    const AncoFlagsCmdParam& GetParam() const
    {
        return param_;
    }

private:
    AncoFlagsCmdParam param_;
};

struct SkipDrawCmdParam {
    bool skipDraw_;
};

class SkipDrawCmdModifier : public RSCmdModifier {
public:
    static inline constexpr auto Type = RSCmdModifierType::IS_SKIP_DRAW;

    SkipDrawCmdModifier(std::weak_ptr<RSNode> node, const SkipDrawCmdParam& param)
        : RSCmdModifier(std::move(node)), param_(param)
    {
    }

    RSCmdModifierType GetType() const override
    {
        return Type;
    }

    bool SetParam(const SkipDrawCmdParam& param)
    {
        param_ = param;
        return true;
    }

    void UpdateToRender() override;

    void DumpParam(std::string& out) const override;

    const SkipDrawCmdParam& GetParam() const
    {
        return param_;
    }

private:
    SkipDrawCmdParam param_;
};

struct WatermarkEnabledCmdParam {
    std::string watermarkName_;
    bool watermarkEnabled_;
};

class WatermarkEnabledCmdModifier : public RSCmdModifier {
public:
    static inline constexpr auto Type = RSCmdModifierType::WATER_MARK_ENABLED;

    WatermarkEnabledCmdModifier(std::weak_ptr<RSNode> node, const WatermarkEnabledCmdParam& param)
        : RSCmdModifier(std::move(node)), param_(param)
    {
    }

    RSCmdModifierType GetType() const override
    {
        return Type;
    }

    bool SetParam(const WatermarkEnabledCmdParam& param)
    {
        param_ = param;
        return true;
    }

    void UpdateToRender() override;

    void DumpParam(std::string& out) const override;

    const WatermarkEnabledCmdParam& GetParam() const
    {
        return param_;
    }

private:
    WatermarkEnabledCmdParam param_;
};

struct AbilityStateCmdParam {
    RSSurfaceNodeAbilityState abilityState_;
};

class AbilityStateCmdModifier : public RSCmdModifier {
public:
    static inline constexpr auto Type = RSCmdModifierType::ABILITY_STATE;

    AbilityStateCmdModifier(std::weak_ptr<RSNode> node, const AbilityStateCmdParam& param)
        : RSCmdModifier(std::move(node)), param_(param)
    {
    }

    RSCmdModifierType GetType() const override
    {
        return Type;
    }

    bool SetParam(const AbilityStateCmdParam& param)
    {
        param_ = param;
        return true;
    }

    void UpdateToRender() override;

    void DumpParam(std::string& out) const override;

    const AbilityStateCmdParam& GetParam() const
    {
        return param_;
    }

private:
    AbilityStateCmdParam param_;
};

struct HidePrivacyContentCmdParam {
    bool hidePrivacyContent_;
};

class HidePrivacyContentCmdModifier : public RSCmdModifier {
public:
    static inline constexpr auto Type = RSCmdModifierType::SET_HIDE_PRIVACY_CONTENT;

    HidePrivacyContentCmdModifier(std::weak_ptr<RSNode> node, const HidePrivacyContentCmdParam& param)
        : RSCmdModifier(std::move(node)), param_(param)
    {
    }

    RSCmdModifierType GetType() const override
    {
        return Type;
    }

    bool SetParam(const HidePrivacyContentCmdParam& param)
    {
        param_ = param;
        return true;
    }

    void UpdateToRender() override;
    UpdateResult UpdateToRenderWithResult() override;

    void DumpParam(std::string& out) const override;

    const HidePrivacyContentCmdParam& GetParam() const
    {
        return param_;
    }

private:
    HidePrivacyContentCmdParam param_;
};

struct ApiCompatibleVersionCmdParam {
    uint32_t apiCompatibleVersion_;
};

class ApiCompatibleVersionCmdModifier : public RSCmdModifier {
public:
    static inline constexpr auto Type = RSCmdModifierType::API_COMPAT_VERSION;

    ApiCompatibleVersionCmdModifier(std::weak_ptr<RSNode> node, const ApiCompatibleVersionCmdParam& param)
        : RSCmdModifier(std::move(node)), param_(param)
    {
    }

    RSCmdModifierType GetType() const override
    {
        return Type;
    }

    bool SetParam(const ApiCompatibleVersionCmdParam& param)
    {
        param_ = param;
        return true;
    }

    void UpdateToRender() override;

    void DumpParam(std::string& out) const override;

    const ApiCompatibleVersionCmdParam& GetParam() const
    {
        return param_;
    }

private:
    ApiCompatibleVersionCmdParam param_;
};

struct VirtualDisplayIdCmdParam {
    uint32_t virtualDisplayId_;
};

class VirtualDisplayIdCmdModifier : public RSCmdModifier {
public:
    static inline constexpr auto Type = RSCmdModifierType::VIRTUAL_DISPLAY_ID;

    VirtualDisplayIdCmdModifier(std::weak_ptr<RSNode> node, const VirtualDisplayIdCmdParam& param)
        : RSCmdModifier(std::move(node)), param_(param)
    {
    }

    RSCmdModifierType GetType() const override
    {
        return Type;
    }

    bool SetParam(const VirtualDisplayIdCmdParam& param)
    {
        param_ = param;
        return true;
    }

    void UpdateToRender() override;

    void DumpParam(std::string& out) const override;

    const VirtualDisplayIdCmdParam& GetParam() const
    {
        return param_;
    }

private:
    VirtualDisplayIdCmdParam param_;
};

struct AttachToWindowContainerCmdParam {
    ScreenId attachToWindowContainer_;
};

class AttachToWindowContainerCmdModifier : public RSCmdModifier {
public:
    static inline constexpr auto Type = RSCmdModifierType::ATTACH_TO_WINDOW_CONTAINER;

    AttachToWindowContainerCmdModifier(std::weak_ptr<RSNode> node, const AttachToWindowContainerCmdParam& param)
        : RSCmdModifier(std::move(node)), param_(param)
    {
    }

    RSCmdModifierType GetType() const override
    {
        return Type;
    }

    bool SetParam(const AttachToWindowContainerCmdParam& param)
    {
        param_ = param;
        return true;
    }

    void UpdateToRender() override;

    void DumpParam(std::string& out) const override;

    const AttachToWindowContainerCmdParam& GetParam() const
    {
        return param_;
    }

private:
    AttachToWindowContainerCmdParam param_;
};

struct RegionToBeMagnifiedCmdParam {
    Vector4<int> regionToBeMagnified_;
};

class RegionToBeMagnifiedCmdModifier : public RSCmdModifier {
public:
    static inline constexpr auto Type = RSCmdModifierType::REGION_TO_BE_MAGNIFIED;

    RegionToBeMagnifiedCmdModifier(std::weak_ptr<RSNode> node, const RegionToBeMagnifiedCmdParam& param)
        : RSCmdModifier(std::move(node)), param_(param)
    {
    }

    RSCmdModifierType GetType() const override
    {
        return Type;
    }

    bool SetParam(const RegionToBeMagnifiedCmdParam& param)
    {
        param_ = param;
        return true;
    }

    void UpdateToRender() override;

    void DumpParam(std::string& out) const override;

    const RegionToBeMagnifiedCmdParam& GetParam() const
    {
        return param_;
    }

private:
    RegionToBeMagnifiedCmdParam param_;
};

struct DetachFromWindowContainerCmdParam {
    ScreenId detachFromWindowContainer_;
};

class DetachFromWindowContainerCmdModifier : public RSCmdModifier {
public:
    static inline constexpr auto Type = RSCmdModifierType::DETACH_TO_WINDOW_CONTAINER;

    DetachFromWindowContainerCmdModifier(std::weak_ptr<RSNode> node, const DetachFromWindowContainerCmdParam& param)
        : RSCmdModifier(std::move(node)), param_(param)
    {
    }

    RSCmdModifierType GetType() const override
    {
        return Type;
    }

    bool SetParam(const DetachFromWindowContainerCmdParam& param)
    {
        param_ = param;
        return true;
    }

    void UpdateToRender() override;

    void DumpParam(std::string& out) const override;

    const DetachFromWindowContainerCmdParam& GetParam() const
    {
        return param_;
    }

private:
    DetachFromWindowContainerCmdParam param_;
};

struct CompositeLayerCmdParam {
    bool isCompositeLayer_;
    uint32_t isMarkedByUI_;
};

class CompositeLayerCmdModifier : public RSCmdModifier {
public:
    static inline constexpr auto Type = RSCmdModifierType::UPDATE_COMPOSITE_LAYER;

    CompositeLayerCmdModifier(std::weak_ptr<RSNode> node, const CompositeLayerCmdParam& param)
        : RSCmdModifier(std::move(node)), param_(param)
    {
    }

    RSCmdModifierType GetType() const override
    {
        return Type;
    }

    bool SetParam(const CompositeLayerCmdParam& param)
    {
        param_ = param;
        return true;
    }

    void UpdateToRender() override;
    UpdateResult UpdateToRenderWithResult() override;

    void DumpParam(std::string& out) const override;

    const CompositeLayerCmdParam& GetParam() const
    {
        return param_;
    }

private:
    CompositeLayerCmdParam param_;
};

struct StaticCachedCmdParam {
    bool isStaticCached_;
};

class StaticCachedCmdModifier : public RSCmdModifier {
public:
    static inline constexpr auto Type = RSCmdModifierType::SET_STATIC_CACHED;

    StaticCachedCmdModifier(std::weak_ptr<RSNode> node, const StaticCachedCmdParam& param)
        : RSCmdModifier(std::move(node)), param_(param)
    {
    }

    RSCmdModifierType GetType() const override
    {
        return Type;
    }

    bool SetParam(const StaticCachedCmdParam& param)
    {
        param_ = param;
        return true;
    }

    void UpdateToRender() override;

    void DumpParam(std::string& out) const override;

    const StaticCachedCmdParam& GetParam() const
    {
        return param_;
    }

private:
    StaticCachedCmdParam param_;
};

struct FrameGravityNewVersionEnabledCmdParam {
    bool isFrameGravityNewVersionEnabled_;
};

class FrameGravityNewVersionEnabledCmdModifier : public RSCmdModifier {
public:
    static inline constexpr auto Type = RSCmdModifierType::IS_FRAME_GRAVITY_NEW_VER_ENABLED;

    FrameGravityNewVersionEnabledCmdModifier(std::weak_ptr<RSNode> node,
        const FrameGravityNewVersionEnabledCmdParam& param)
        : RSCmdModifier(std::move(node)), param_(param)
    {
    }

    RSCmdModifierType GetType() const override
    {
        return Type;
    }

    bool SetParam(const FrameGravityNewVersionEnabledCmdParam& param)
    {
        param_ = param;
        return true;
    }

    void UpdateToRender() override;

    void DumpParam(std::string& out) const override;

    const FrameGravityNewVersionEnabledCmdParam& GetParam() const
    {
        return param_;
    }

private:
    FrameGravityNewVersionEnabledCmdParam param_;
};

struct SurfaceBufferOpaqueCmdParam {
    bool isSurfaceBufferOpaque_;
};

class SurfaceBufferOpaqueCmdModifier : public RSCmdModifier {
public:
    static inline constexpr auto Type = RSCmdModifierType::IS_SURFACE_BUFFER_OPAQUE;

    SurfaceBufferOpaqueCmdModifier(std::weak_ptr<RSNode> node, const SurfaceBufferOpaqueCmdParam& param)
        : RSCmdModifier(std::move(node)), param_(param)
    {
    }

    RSCmdModifierType GetType() const override
    {
        return Type;
    }

    bool SetParam(const SurfaceBufferOpaqueCmdParam& param)
    {
        param_ = param;
        return true;
    }

    void UpdateToRender() override;

    void DumpParam(std::string& out) const override;

    const SurfaceBufferOpaqueCmdParam& GetParam() const
    {
        return param_;
    }

private:
    SurfaceBufferOpaqueCmdParam param_;
};

struct ContainerWindowTransparentCmdParam {
    bool isContainerWindowTransparent_;
};

class ContainerWindowTransparentCmdModifier : public RSCmdModifier {
public:
    static inline constexpr auto Type = RSCmdModifierType::IS_CONTAINER_WINDOW_TRANSPARENT;

    ContainerWindowTransparentCmdModifier(std::weak_ptr<RSNode> node, const ContainerWindowTransparentCmdParam& param)
        : RSCmdModifier(std::move(node)), param_(param)
    {
    }

    RSCmdModifierType GetType() const override
    {
        return Type;
    }

    bool SetParam(const ContainerWindowTransparentCmdParam& param)
    {
        param_ = param;
        return true;
    }

    void UpdateToRender() override;

    void DumpParam(std::string& out) const override;

    const ContainerWindowTransparentCmdParam& GetParam() const
    {
        return param_;
    }

private:
    ContainerWindowTransparentCmdParam param_;
};

struct SurfaceDefaultSizeCmdParam {
    float width_;
    float height_;
};

class SurfaceDefaultSizeCmdModifier : public RSCmdModifier {
public:
    static inline constexpr auto Type = RSCmdModifierType::UPDATE_SURFACE_DEFAULT_SIZE;

    SurfaceDefaultSizeCmdModifier(std::weak_ptr<RSNode> node, const SurfaceDefaultSizeCmdParam& param)
        : RSCmdModifier(std::move(node)), param_(param)
    {
    }

    RSCmdModifierType GetType() const override
    {
        return Type;
    }

    bool SetParam(const SurfaceDefaultSizeCmdParam& param)
    {
        param_ = param;
        return true;
    }

    void UpdateToRender() override;

    void DumpParam(std::string& out) const override;

    const SurfaceDefaultSizeCmdParam& GetParam() const
    {
        return param_;
    }

private:
    SurfaceDefaultSizeCmdParam param_;
};

struct IsDepthResourceCmdParam {
    bool isDepthResource_;
};

class IsDepthResourceCmdModifier : public RSCmdModifier {
public:
    static inline constexpr auto Type = RSCmdModifierType::IS_DEPTH_RESOURCE;

    IsDepthResourceCmdModifier(std::weak_ptr<RSNode> node, const IsDepthResourceCmdParam& param)
        : RSCmdModifier(std::move(node)), param_(param)
    {
    }

    RSCmdModifierType GetType() const override
    {
        return Type;
    }

    bool SetParam(const IsDepthResourceCmdParam& param)
    {
        param_ = param;
        return true;
    }

    void UpdateToRender() override;

    void DumpParam(std::string& out) const override;

    const IsDepthResourceCmdParam& GetParam() const
    {
        return param_;
    }

private:
    IsDepthResourceCmdParam param_;
};

struct ForceHardwareAndFixRotationCmdParam {
    bool flag_;
};

class ForceHardwareAndFixRotationCmdModifier : public RSCmdModifier {
public:
    static inline constexpr auto Type = RSCmdModifierType::FORCE_HARDWARE_AND_FIX_ROTATION;

    ForceHardwareAndFixRotationCmdModifier(std::weak_ptr<RSNode> node, const ForceHardwareAndFixRotationCmdParam& param)
        : RSCmdModifier(std::move(node)), param_(param)
    {
    }

    RSCmdModifierType GetType() const override
    {
        return Type;
    }

    bool SetParam(const ForceHardwareAndFixRotationCmdParam& param)
    {
        param_ = param;
        return true;
    }

    void UpdateToRender() override;

    void DumpParam(std::string& out) const override;

    const ForceHardwareAndFixRotationCmdParam& GetParam() const
    {
        return param_;
    }

private:
    ForceHardwareAndFixRotationCmdParam param_;
};

struct AppRotationCorrectionCmdParam {
    ScreenRotation appRotationCorrection_;
};

class AppRotationCorrectionCmdModifier : public RSCmdModifier {
public:
    static inline constexpr auto Type = RSCmdModifierType::APP_ROTATION_CORRECTION;

    AppRotationCorrectionCmdModifier(std::weak_ptr<RSNode> node, const AppRotationCorrectionCmdParam& param)
        : RSCmdModifier(std::move(node)), param_(param)
    {
    }

    RSCmdModifierType GetType() const override
    {
        return Type;
    }

    bool SetParam(const AppRotationCorrectionCmdParam& param)
    {
        param_ = param;
        return true;
    }

    void UpdateToRender() override;

    void DumpParam(std::string& out) const override;

    const AppRotationCorrectionCmdParam& GetParam() const
    {
        return param_;
    }

private:
    AppRotationCorrectionCmdParam param_;
};

struct HDRTypeCmdParam {
    uint32_t hdrType_;
};

class HDRTypeCmdModifier : public RSCmdModifier {
public:
    static inline constexpr auto Type = RSCmdModifierType::HDR_TYPE;

    HDRTypeCmdModifier(std::weak_ptr<RSNode> node, const HDRTypeCmdParam& param)
        : RSCmdModifier(std::move(node)), param_(param)
    {
    }

    RSCmdModifierType GetType() const override
    {
        return Type;
    }

    bool SetParam(const HDRTypeCmdParam& param)
    {
        param_ = param;
        return true;
    }

    void UpdateToRender() override;

    void DumpParam(std::string& out) const override;

    const HDRTypeCmdParam& GetParam() const
    {
        return param_;
    }

private:
    HDRTypeCmdParam param_;
};

struct DarkColorModeCmdParam {
    bool isDarkColorMode_;
};

class DarkColorModeCmdModifier : public RSCmdModifier {
public:
    static inline constexpr auto Type = RSCmdModifierType::SET_DARK_COLOR_MODE;

    DarkColorModeCmdModifier(std::weak_ptr<RSNode> node, const DarkColorModeCmdParam& param)
        : RSCmdModifier(std::move(node)), param_(param)
    {
    }

    RSCmdModifierType GetType() const override
    {
        return Type;
    }

    bool SetParam(const DarkColorModeCmdParam& param)
    {
        param_ = param;
        return true;
    }

    void UpdateToRender() override;

    void DumpParam(std::string& out) const override;

    const DarkColorModeCmdParam& GetParam() const
    {
        return param_;
    }

private:
    DarkColorModeCmdParam param_;
};

} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_COMMAND_MODIFIER_RS_SURFACE_NODE_COMMAND_MODIFIER_H
