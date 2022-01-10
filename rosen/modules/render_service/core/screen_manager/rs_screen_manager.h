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

#ifndef RS_SCREEN_MANAGER
#define RS_SCREEN_MANAGER

#include <cstdint>
#include <memory>
#include <mutex>
#include <queue>
#include <unordered_map>

#include <hdi_backend.h>
#include <hilog/log.h>
#include <ipc_callbacks/screen_change_callback.h>
#include <iremote_object.h>
#include <refbase.h>
#include <screen_manager/rs_screen_props.h>
#include <screen_manager/rs_screen_mode_info.h>
#include <screen_manager/rs_screen_capability.h>
#include <screen_manager/rs_screen_data.h>
#include <screen_manager/screen_types.h>
#include <surface.h>

#include "rs_screen.h"

namespace OHOS {
namespace Rosen {
enum class ScreenState : uint8_t {
    HDI_OUTPUT_ENABLE,
    PRODUCER_SURFACE_ENABLE,
    DISABLED,
    NOT_EXISTED,
    UNKNOWN,
};

struct ScreenInfo {
    uint32_t width = 0;
    uint32_t height = 0;
    ScreenState state = ScreenState::UNKNOWN;
};

class RSScreenManager : public RefBase {
public:
    RSScreenManager() = default;
    virtual ~RSScreenManager() noexcept = default;

    virtual bool Init() noexcept = 0;

    // get default/primary screen id.
    virtual ScreenId GetDefaultScreenId() const = 0;

    virtual void SetDefaultScreenId(ScreenId id) = 0;

    virtual void SetScreenMirror(ScreenId id, ScreenId toMirror) = 0;

    virtual ScreenId CreateVirtualScreen(
        const std::string &name,
        uint32_t width,
        uint32_t height,
        sptr<Surface> surface,
        ScreenId mirrorId,
        int flags) = 0;

    virtual void RemoveVirtualScreen(ScreenId id) = 0;

    virtual void SetScreenActiveMode(ScreenId id, uint32_t modeId) = 0;

    virtual void SetScreenPowerStatus(ScreenId id, ScreenPowerStatus status) = 0;

    virtual void GetScreenActiveMode(ScreenId id, RSScreenModeInfo& screenModeInfo) const = 0;

    virtual std::vector<RSScreenModeInfo> GetScreenSupportedModes(ScreenId id) const = 0;

    virtual RSScreenCapability GetScreenCapability(ScreenId id) const = 0;

    virtual ScreenPowerStatus GetScreenPowerStatus(ScreenId id) const = 0;

    virtual RSScreenData GetScreenData(ScreenId id) const = 0;

    virtual ScreenInfo QueryScreenInfo(ScreenId id) const = 0;

    // Can only be called after QueryScreenState and the state is ScreenState::PRODUCER_SURFACE_ENABLE;
    virtual sptr<Surface> GetProducerSurface(ScreenId id) const = 0;

    // Can only be called after QueryScreenState and the state is ScreenState::HDI_OUTPUT_ENABLE;
    virtual std::shared_ptr<HdiOutput> GetOutput(ScreenId id) const = 0;

    virtual void AddScreenChangeCallback(const sptr<RSIScreenChangeCallback> &callback) = 0;

    virtual void OnRemoteScreenChangeCallbackDied(const wptr<IRemoteObject> &remote) = 0;

    virtual void ProcessScreenHotPlugEvents() = 0;

    virtual void DisplayDump(std::string& dumpString) = 0;

    virtual void SurfaceDump(std::string& dumpString) = 0;
};

sptr<RSScreenManager> CreateOrGetScreenManager();

namespace impl {
struct ScreenHotPlugEvent {
    std::shared_ptr<HdiOutput> output;
    bool connected = false;
};

class RSScreenManager : public OHOS::Rosen::RSScreenManager {
public:
    static sptr<OHOS::Rosen::RSScreenManager> GetInstance() noexcept;

    // noncopyable
    RSScreenManager(const RSScreenManager &) = delete;
    RSScreenManager &operator=(const RSScreenManager &) = delete;

    bool Init() noexcept override;

    ScreenId GetDefaultScreenId() const override
    {
        return defaultScreenId_;
    }

    void SetDefaultScreenId(ScreenId id) override;

    void SetScreenMirror(ScreenId id, ScreenId toMirror) override;

    ScreenId CreateVirtualScreen(
        const std::string &name,
        uint32_t width,
        uint32_t height,
        sptr<Surface> surface,
        ScreenId mirrorId,
        int32_t flags) override;

    void RemoveVirtualScreen(ScreenId id) override;

    void SetScreenActiveMode(ScreenId id, uint32_t modeId) override;

    void SetScreenPowerStatus(ScreenId id, ScreenPowerStatus status) override;

    void GetScreenActiveMode(ScreenId id, RSScreenModeInfo& screenModeInfo) const override;

    std::vector<RSScreenModeInfo> GetScreenSupportedModes(ScreenId id) const override;

    RSScreenCapability GetScreenCapability(ScreenId id) const override;

    ScreenPowerStatus GetScreenPowerStatus(ScreenId id) const override;

    RSScreenData GetScreenData(ScreenId id) const  override;

    ScreenInfo QueryScreenInfo(ScreenId id) const override;

    sptr<Surface> GetProducerSurface(ScreenId id) const override;

    std::shared_ptr<HdiOutput> GetOutput(ScreenId id) const override;

    void AddScreenChangeCallback(const sptr<RSIScreenChangeCallback> &callback) override;

    void OnRemoteScreenChangeCallbackDied(const wptr<IRemoteObject> &remote) override;

    void ProcessScreenHotPlugEvents() override;

    void DisplayDump(std::string& dumpString) override;

    void SurfaceDump(std::string& dumpString) override;

private:
    RSScreenManager();
    ~RSScreenManager() noexcept override;

    // TODO: fixme -- domain 0 only for debug.
    static constexpr HiviewDFX::HiLogLabel LOG_LABEL = { LOG_CORE, 0, "RSScreenManager" };

    static void OnHotPlug(std::shared_ptr<HdiOutput> &output, bool connected, void *data);
    void OnHotPlugEvent(std::shared_ptr<HdiOutput> &output, bool connected);
    void ProcessScreenConnectedLocked(std::shared_ptr<HdiOutput> &output);
    void ProcessScreenDisConnectedLocked(std::shared_ptr<HdiOutput> &output);
    void HandleDefaultScreenDisConnectedLocked();
    std::vector<ScreenHotPlugEvent> pendingHotPlugEvents_;

    void GetScreenActiveModeLocked(ScreenId id, RSScreenModeInfo& screenModeInfo) const;
    std::vector<RSScreenModeInfo> GetScreenSupportedModesLocked(ScreenId id) const;
    RSScreenCapability GetScreenCapabilityLocked(ScreenId id) const;
    ScreenPowerStatus GetScreenPowerStatuslocked(ScreenId id) const;

    void RemoveVirtualScreenLocked(ScreenId id);
    ScreenId GenerateVirtualScreenIdLocked();
    void ReuseVirtualScreenIdLocked(ScreenId id);

    mutable std::mutex mutex_;
    HdiBackend *composer_ = nullptr;
    ScreenId defaultScreenId_ = INVALID_SCREEN_ID;
    std::unordered_map<ScreenId, std::unique_ptr<OHOS::Rosen::RSScreen>> screens_;
    std::queue<ScreenId> freeVirtualScreenIds_;
    uint32_t maxVirtualScreenNum_ = 0;
    std::vector<sptr<RSIScreenChangeCallback>> screenChangeCallbacks_;

    static std::once_flag createFlag_;
    static sptr<OHOS::Rosen::RSScreenManager> instance_;
};
} // namespace impl
} // namespace Rosen
} // namespace OHOS
#endif // RS_SCREEN_MANAGER
