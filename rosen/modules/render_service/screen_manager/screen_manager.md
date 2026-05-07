# Screen Manager Specification

## Overview

The `screen_manager` module provides screen lifecycle management for the OpenHarmony Render Service. It handles physical and virtual screen creation, destruction, property configuration, event callback dispatch, and fold screen support. The module serves as the bridge between HDI Composer and the rendering pipeline.

## Screen Type Classification

```
┌──────────────────────────────────────────────────────────────────────────────────┐
│                          Screen Type Classification                               │
├──────────────────────────────────────────────────────────────────────────────────┤
│                                                                                  │
│  RSScreenType Enum:                                                              │
│  ┌─────────────────────────────────────────────────────────────────────────────┐ │
│  │ BUILT_IN_TYPE_SCREEN = 0  │ EXTERNAL_TYPE_SCREEN = 1 │ VIRTUAL_TYPE_SCREEN = 2│ │
│  │ UNKNOWN_TYPE_SCREEN = 3   │                                               │ │
│  └─────────────────────────────────────────────────────────────────────────────┘ │
│                                                                                  │
│  ┌─────────────────────────────────────────────────────────────────────────────┐ │
│  │ Physical Screen (IsVirtual = false)                                         │ │
│  │   ├── Built-in Screen (BUILT_IN_TYPE_SCREEN)                                │ │
│  │   │     ConnectionType: DISPLAY_CONNECTION_TYPE_INTERNAL                    │ │
│  │   │     Interface: MIPI / LCD / PANEL                                       │ │
│  │   │     Features: backlight control, power status, fold screen support      │ │
│  │   │     Used as: default screen, inner screen in fold device                │ │
│  │   │                                                                          │ │
│  │   └── External Screen (EXTERNAL_TYPE_SCREEN)                                │ │
│  │   │     ConnectionType: DISPLAY_CONNECTION_TYPE_EXTERNAL                    │ │
│  │   │     Interface: HDMI / DP / eDP / VGA etc.                               │ │
│  │   │     Features: backlight control, power status, skip frame default       │ │
│  │   │     Used as: external monitor, outer screen in fold device              │ │
│  └─────────────────────────────────────────────────────────────────────────────┘ │
│                                                                                  │
│  ┌─────────────────────────────────────────────────────────────────────────────┐ │
│  │ Virtual Screen (IsVirtual = true)                                            │ │
│  │   ├── Mirror Screen (associatedScreenId != INVALID)                          │ │
│  │   │     Renders content from associated physical screen                      │ │
│  │   │     Features: canvas rotation, scale mode, visible rect, security mask   │ │
│  │   │     whitelist/blacklist filtering per window                             │ │
│  │   │     Used as: screen casting/mirroring to remote display                 │ │
│  │   │                                                                          │ │
│  │   ├── Extension Screen (standalone virtual display)                          │ │
│  │   │     Independent content, not mirroring any physical screen               │ │
│  │   │     Has own Producer Surface for content injection                       │ │
│  │   │     Features: whitelist/blacklist, security exemption, skip window       │ │
│  │   │     Used as: virtual display for multi-window or testing                 │ │
│  │   │                                                                          │ │
│  │   └── Heterogeneous Source Screen (异源屏)                                   │ │
│  │   │     Content from different source process via Producer Surface           │ │
│  │   │     Virtual screen flags control security layer behavior                 │ │
│  │   │     Features: security exemption list, security mask                     │ │
│  │   │     Used as: cross-process rendering, security content display           │ │
│  └─────────────────────────────────────────────────────────────────────────────┘ │
│                                                                                  │
│  ScreenConnectionType:                                                           │
│  ┌─────────────────────────────────────────────────────────────────────────────┐ │
│  │ DISPLAY_CONNECTION_TYPE_INTERNAL = 0  (Built-in, MIPI interface)            │ │
│  │ DISPLAY_CONNECTION_TYPE_EXTERNAL = 1  (External, HDMI/DP etc.)              │ │
│  │ INVALID_DISPLAY_CONNECTION_TYPE       (Virtual screen)                       │ │
│  └─────────────────────────────────────────────────────────────────────────────┘ │
│                                                                                  │
└──────────────────────────────────────────────────────────────────────────────────┘
```

## Architecture Overview

```
┌──────────────────────────────────────────────────────────────────────────────────┐
│                          Screen Manager Module Architecture                      │
├──────────────────────────────────────────────────────────────────────────────────┤
│                                                                                  │
│  ┌─────────────────────────────────────────────────────────────────────────────┐ │
│  │                    Core Listener (RSRenderService::ScreenManagerListener)    │ │
│  │                    ┌──────────────────────────────────────────┐               │ │
│  │                    │  Implements RSIScreenManagerListener      │               │ │
│  │                    │  Singleton, registered during Init        │               │ │
│  │                    │  Holds RSRenderService reference          │               │ │
│  │                    │                                          │               │ │
│  │                    │  Role: Bridge between ScreenManager      │               │ │
│  │                    │  and RenderService internals:             │               │ │
│  │                    │  ├── OnScreenConnected → create Composer │               │ │
│  │                    │  │   output, return Client-to-Render      │               │ │
│  │                    │  │   IRemoteObject connection              │               │ │
│  │                    │  ├── OnScreenDisconnected → cleanup      │               │ │
│  │                    │  │   Composer output, remove render node  │               │ │
│  │                    │  ├── OnScreenPropertyChanged → update    │               │ │
│  │                    │  │   RSRenderThreadParams (resolution,    │               │ │
│  │                    │  │   color gamut, HDR, power status etc.) │               │ │
│  │                    │  ├── OnScreenRefresh → request composite │               │ │
│  │                    │  │   from RenderEngine                    │               │ │
│  │                    │  ├── OnVBlankIdle → start VSync sampler  │               │ │
│  │                    │  ├── OnHwcRestored → reconnect outputs   │               │ │
│  │                    │  ├── OnHwcDead → cleanup Composer        │               │ │
│  │                    │  │   resources                            │               │ │
│  │                    │  ├── OnVirtualScreenConnected/Disconnected│               │ │
│  │                    │  │   → create/remove render nodes        │               │ │
│  │                    │  ├── OnActiveScreenIdChanged → update    │               │ │
│  │                    │  │   render pipeline active screen        │               │ │
│  │                    │  ├── OnScreenBacklightChanged → notify   │               │ │
│  │                    │  │   render thread backlight change        │               │ │
│  │                    │  └──────────────────────────────────────────┘               │ │
│  └─────────────────────────────────────────────────────────────────────────────┘ │
│                                                                                  │
│  ┌─────────────────────────────────────────────────────────────────────────────┐ │
│  │                         External Modules (DMS/SCB)                         │ │
│  │                    ┌──────────────────────────┐                            │ │
│  │                    │  RSScreenManagerAgent     │                            │ │
│  │                    │  + AgentListener          │                            │ │
│  │                    └──────────────┬───────────┘                            │ │
│  └───────────────────────────┼────────────────────────────────────────────────┘ │
│                                      │ API forwarding                         │
│                                      ▼                                        │
│  ┌─────────────────────────────────────────────────────────────────────────────┐ │
│  │                          RSScreenManager (Core)                            │ │
│  │                    ┌──────────────────────────────────┐                    │ │
│  │                    │  screens_ map                     │                    │ │
│  │                    │  ┌─────────┐  ┌─────────┐       │                    │ │
│  │                    │  │RSScreen │  │RSScreen │ ...   │                    │ │
│  │                    │  │(phys 0) │  │(virt 1) │       │                    │ │
│  │                    │  └─────────┘  └─────────┘       │                    │ │
│  │                    └──────────────────────────────────┘                    │ │
│  │         ┌──────────────┐  ┌───────────────────┐  ┌──────────────────┐     │ │
│  │         │CallbackManager│  │ScreenPreprocessor │  │FoldScreenManager│     │ │
│  │         └──────────────┘  └───────────────────┘  └──────────────────┘     │ │
│  └─────────────────────────────────────────────────────────────────────────────┘ │
│                                      │                                          │
│                                      │ ScheduleTask (main thread)               │
│                                      ▼                                          │
│  ┌─────────────────────────────────────────────────────────────────────────────┐ │
│  │                      RSScreenPreprocessor (HDI Layer)                      │ │
│  │         ┌──────────────────────────────────────────────┐                   │ │
│  │         │  HdiBackend (Composer)                        │                   │ │
│  │         │  ├── RegScreenHotplug                         │                   │ │
│  │         │  ├── RegScreenRefresh                         │                   │ │
│  │         │  ├── RegHwcDeadListener                       │                   │ │
│  │         │  ├── RegHwcEventCallback                      │                   │ │
│  │         │  └── RegScreenVBlankIdleCallback              │                   │ │
│  │         └──────────────────────────────────────────────┘                   │ │
│  └─────────────────────────────────────────────────────────────────────────────┘ │
│                                                                                  │
│  ┌─────────────────────────────────────────────────────────────────────────────┐ │
│  │                       RSScreen (Per-Screen)                                │ │
│  │         ┌──────────────────────────────────────────────────────────────────┐│ │
│  │         │  Physical Screen          │  Virtual Screen                      ││ │
│  │         │  ├── HdiScreen (HDI)      │  ├── Producer Surface               ││ │
│  │         │  ├── Built-in / External  │  ├── Mirror / Extension / 异源       ││ │
│  │         │  │   classified by         │  │   classified by                  ││ │
│  │         │  │   ConnectionType        │  │   associatedScreenId & flags     ││ │
│  │         │  └────────────────────────┼────────────────────────────────────┘ ││ │
│  │         │  RSScreenThreadSafeProperty (shared by both)                       ││ │
│  │         │  UPDATE_PROPERTY → onPropertyChange_ callback                     ││ │
│  │         └──────────────────────────────────────────────────────────────────┘│ │
│  └─────────────────────────────────────────────────────────────────────────────┘ │
│                                                                                  │
└──────────────────────────────────────────────────────────────────────────────────┘
```
┌──────────────────────────────────────────────────────────────────────────────────┐
│                          Screen Manager Module Architecture                      │
├──────────────────────────────────────────────────────────────────────────────────┤
│                                                                                  │
│  ┌─────────────────────────────────────────────────────────────────────────────┐ │
│  │                         External Modules (DMS/SCB)                         │ │
│  │                    ┌──────────────────────────┐                            │ │
│  │                    │  RSScreenManagerAgent     │                            │ │
│  │                    │  + AgentListener          │                            │ │
│  │                    └──────────────┬───────────┘                            │ │
│  └───────────────────────────────────┼────────────────────────────────────────┘ │
│                                      │ API forwarding                         │
│                                      ▼                                        │
│  ┌─────────────────────────────────────────────────────────────────────────────┐ │
│  │                          RSScreenManager (Core)                            │ │
│  │                    ┌──────────────────────────────────┐                    │ │
│  │                    │  screens_ map                     │                    │ │
│  │                    │  ┌─────────┐  ┌─────────┐       │                    │ │
│  │                    │  │RSScreen │  │RSScreen │ ...   │                    │ │
│  │                    │  │(phys 0) │  │(virt 1) │       │                    │ │
│  │                    │  └─────────┘  └─────────┘       │                    │ │
│  │                    └──────────────────────────────────┘                    │ │
│  │         ┌──────────────┐  ┌───────────────────┐  ┌──────────────────┐     │ │
│  │         │CallbackManager│  │ScreenPreprocessor │  │FoldScreenManager│     │ │
│  │         └──────────────┘  └───────────────────┘  └──────────────────┘     │ │
│  └─────────────────────────────────────────────────────────────────────────────┘ │
│                                      │                                          │
│                                      │ ScheduleTask (main thread)               │
│                                      ▼                                          │
│  ┌─────────────────────────────────────────────────────────────────────────────┐ │
│  │                      RSScreenPreprocessor (HDI Layer)                      │ │
│  │         ┌──────────────────────────────────────────────┐                   │ │
│  │         │  HdiBackend (Composer)                        │                   │ │
│  │         │  ├── RegScreenHotplug                         │                   │ │
│  │         │  ├── RegScreenRefresh                         │                   │ │
│  │         │  ├── RegHwcDeadListener                       │                   │ │
│  │         │  ├── RegHwcEventCallback                      │                   │ │
│  │         │  └── RegScreenVBlankIdleCallback              │                   │ │
│  │         └──────────────────────────────────────────────┘                   │ │
│  └─────────────────────────────────────────────────────────────────────────────┘ │
│                                                                                  │
│  ┌─────────────────────────────────────────────────────────────────────────────┐ │
│  │                       RSScreen (Per-Screen)                                │ │
│  │         ┌──────────────────────────────────────────────┐                   │ │
│  │         │  HdiScreen (physical)  │  Surface (virtual)  │                   │ │
│  │         │  RSScreenThreadSafeProperty                   │                   │ │
│  │         │  UPDATE_PROPERTY → onPropertyChange_ callback │                   │ │
│  │         └──────────────────────────────────────────────┘                   │ │
│  └─────────────────────────────────────────────────────────────────────────────┘ │
│                                                                                  │
└──────────────────────────────────────────────────────────────────────────────────┘
```

## Event Flow Graph

### Physical Screen Hotplug Flow

```
┌──────────────────────────────────────────────────────────────────────────────────┐
│                      Physical Screen Hotplug Event Flow                          │
├──────────────────────────────────────────────────────────────────────────────────┤
│                                                                                  │
│  HDI Hotplug Callback                                                            │
│    │                                                                             │
│    ▼                                                                             │
│  ┌──────────────────────────────┐                                                │
│  │ RSScreenPreprocessor::OnHotPlug │  (static)                                    │
│  └──────────────┬───────────────┘                                                │
│                 │                                                                │
│                 ▼                                                                │
│  ┌──────────────────────────────┐                                                │
│  │ OnHotPlugEvent(output, conn) │  (instance)                                    │
│  │ → save to pendingHotPlugEvents_                                               │
│  └──────────────┬───────────────┘                                                │
│                 │ ScheduleTask                                                    │
│                 ▼                                                                 │
│  ┌─────────────────────────────────────────────────────────────────────────────┐ │
│  │ ProcessScreenHotPlugEvents (main thread)                                     │ │
│  │   ├── ConfigureScreenConnected                                               │ │
│  │   │     ├── screenManager_.ProcessScreenConnected(id)                        │ │
│  │   │     ├── callbackMgr_.NotifyScreenConnected(event)                        │ │
│  │   │     │     ├── coreListener_->OnScreenConnected → return IRemoteObject    │ │
│  │   │     │     └── agentListeners → OnScreenConnected(id, reason, conn)      │ │
│  │   │     └── [if isHwcDead_] callbackMgr_.NotifyHwcRestored(event)           │ │
│  │   ├── ConfigureScreenDisconnected                                            │ │
│  │   │     ├── screenManager_.ProcessScreenDisConnected(id)                     │ │
│  │   │     └── callbackMgr_.NotifyScreenDisconnected(id)                        │ │
│  │   └── screenManager_.ProcessPendingConnections()                             │ │
│  │         → restore power/backlight/correction properties                      │ │
│  └─────────────────────────────────────────────────────────────────────────────┘ │
│                                                                                  │
└──────────────────────────────────────────────────────────────────────────────────┘
```

### HWC Dead Recovery Flow

```
┌──────────────────────────────────────────────────────────────────────────────────┐
│                      HWC Dead Recovery Event Flow                                │
├──────────────────────────────────────────────────────────────────────────────────┤
│                                                                                  │
│  HDI HwcDead Callback                                                            │
│    │                                                                             │
│    ▼                                                                             │
│  ┌─────────────────────────────────────────────────────────────────────────────┐ │
│  │ OnHwcDeadEvent (main thread via ScheduleTask)                                │ │
│  │   ├── isHwcDead_ = true                                                      │ │
│  │   ├── screenManager_.OnHwcDeadEvent(retScreens)                              │ │
│  │   │     → remove all physical screens from screens_                          │ │
│  │   │     → defaultScreenId_ = INVALID_SCREEN_ID                               │ │
│  │   ├── callbackMgr_.NotifyHwcDead(id) for each removed screen                 │ │
│  │   ├── composer_->ResetDevice()                                               │ │
│  │   └── Re-Init() (re-register all HDI callbacks)                              │ │
│  └─────────────────────────────────────────────────────────────────────────────┘ │
│                                                                                  │
│  Subsequent hotplug events use NotifyHwcRestored instead of NotifyScreenConnected │
│                                                                                  │
└──────────────────────────────────────────────────────────────────────────────────┘
```

### Property Change Notification Flow

```
┌──────────────────────────────────────────────────────────────────────────────────┐
│                    Screen Property Change Notification Flow                       │
├──────────────────────────────────────────────────────────────────────────────────┤
│                                                                                  │
│  RSScreen::Set* or UPDATE_PROPERTY                                               │
│    │                                                                             │
│    ▼                                                                             │
│  ┌─────────────────────────────────────────────────────────────────────────────┐ │
│  │ RSScreenThreadSafeProperty::Set*                                             │ │
│  │   → unique_lock<shared_mutex> for write                                      │ │
│  │   → property_->Set<ScreenPropertyType::*>(value)                             │ │
│  │   → returns ResType = {type, sptr<ScreenPropertyBase>}                      │ │
│  └──────────────┬───────────────┘                                                │
│                 │                                                                │
│                 ▼                                                                │
│  ┌──────────────────────────────┐                                                │
│  │ RSScreen::NotifyScreenPropertyChange(prop)                                     │
│  │   → onPropertyChange_(id, type, property) callback                            │
│  └──────────────┬───────────────┘                                                │
│                 │                                                                │
│                 ▼                                                                │
│  ┌─────────────────────────────────────────────────────────────────────────────┐ │
│  │ RSScreenPreprocessor::NotifyScreenPropertyChanged(id, type, property)         │ │
│  │   → ScheduleTask to main thread                                               │ │
│  └──────────────┬───────────────┘                                                │
│                 │                                                                │
│                 ▼                                                                │
│  ┌──────────────────────────────┐                                                │
│  │ RSScreenCallbackManager::NotifyScreenPropertyUpdated                          │
│  │   → coreListener_->OnScreenPropertyChanged(id, type, property)                │
│  └──────────────────────────────┘                                                │
│                                                                                  │
└──────────────────────────────────────────────────────────────────────────────────┘
```

## Architecture Components

### Core Listener (RSRenderService::ScreenManagerListener)

The coreListener is the single `RSIScreenManagerListener` instance registered during `RSRenderService::Init()`. It bridges `RSScreenManager` with the render pipeline internals. Its implementation is `RSRenderService::ScreenManagerListener` located at `rosen/modules/render_service/main/render_server/rs_render_service.h`.

**Core Listener Event Handling:**

| Event | Handler Behavior |
|-------|-----------------|
| `OnScreenConnected(id, output, property)` | Create Composer output for the screen, return `Client-to-Render` IRemoteObject connection. This connection enables IPC between render client process and render service. |
| `OnScreenDisconnected(id)` | Cleanup Composer output, remove screen render node from render pipeline |
| `OnHwcRestored(id, output, property)` | Reconnect Composer output after HWC recovery, similar to OnScreenConnected |
| `OnHwcDead(id)` | Cleanup all Composer resources for the dead screen |
| `OnScreenPropertyChanged(id, type, property)` | Update `RSRenderThreadParams` — propagate resolution, color gamut, HDR format, power status, skip frame interval, canvas rotation etc. to render thread |
| `OnScreenRefresh(id)` | Request composite from RenderEngine for the specified screen |
| `OnVBlankIdle(id, ns)` | Start VSync sampler (for adaptive refresh rate) |
| `OnVirtualScreenConnected(id, associatedScreenId, property)` | Create virtual screen render node in render pipeline |
| `OnVirtualScreenDisconnected(id)` | Remove virtual screen render node |
| `OnActiveScreenIdChanged(activeScreenId)` | Update render pipeline's active screen, switch VSync source |
| `OnScreenBacklightChanged(id, level)` | Notify render thread of backlight change (for brightness adaptation) |
| `OnGlobalBlacklistChanged(globalBlackList)` | Update global blacklist in render pipeline for security layer filtering |
| `OnHwcEvent(deviceId, eventId, eventData)` | Forward HWC events to render pipeline for processing |

The `Client-to-Render` IRemoteObject returned by `OnScreenConnected` is stored in `RSScreenCallbackManager::clientToRenderConns_` and passed to Agent listeners, enabling IPC communication between render client and render service for each screen.

---

### 1. RSScreenManager

**Location:** `rosen/modules/render_service/screen_manager/rs_screen_manager.h`

**Responsibilities:**
- Global entry point for screen management
- Manage all physical and virtual screen creation, destruction, and property queries
- Hold screen instance map (screens_), callback manager, preprocessor, fold screen manager
- Manage global blacklist, power status cache, backlight cache, fold screen status

**Key Methods:**
```cpp
// Init & Registration
bool Init(const std::shared_ptr<AppExecFwk::EventHandler>& mainHandler) noexcept;
void RegisterCoreListener(const sptr<RSIScreenManagerListener>& listener);
void RegisterAgentListener(const sptr<RSIScreenManagerAgentListener>& listener);

// Screen connection
void ProcessScreenConnected(ScreenId id);
void ProcessScreenDisConnected(ScreenId id);
void ProcessPendingConnections();
void OnHwcDeadEvent(std::map<ScreenId, std::shared_ptr<RSScreen>>& retScreens);

// Virtual screen
ScreenId CreateVirtualScreen(const std::string& name, uint32_t width, uint32_t height,
    sptr<Surface> surface, ScreenId associatedScreenId = 0, int32_t flags = 0,
    std::vector<uint64_t> whiteList = {});
void RemoveVirtualScreen(ScreenId id);

// Property set/get (delegate to RSScreen)
uint32_t SetScreenActiveMode(ScreenId id, uint32_t modeId);
void SetScreenPowerStatus(ScreenId id, ScreenPowerStatus status);
void SetScreenBacklight(ScreenId id, uint32_t level);
int32_t SetScreenColorGamut(ScreenId id, int32_t modeIdx);
int32_t SetScreenHDRFormat(ScreenId id, int32_t modeIdx);

// Blacklist/whitelist
int32_t SetVirtualScreenBlackList(ScreenId id, const std::vector<NodeId>& blackList);
int32_t AddVirtualScreenWhiteList(ScreenId id, const std::vector<NodeId>& whiteList);

// Fold screen
void UpdateFoldScreenConnectStatusLocked(ScreenId screenId, bool connected);
uint64_t JudgeVSyncEnabledScreenWhilePowerStatusChanged(
    ScreenId screenId, ScreenPowerStatus status, uint64_t enabledScreenId);

// Debug
void DisplayDump(std::string& dumpString);
```

**Key Constants:**
| Constant | Value | Description |
|----------|-------|-------------|
| `MAX_VIRTUAL_SCREEN_NUM` | 64 | Max virtual screen count |
| `MAX_VIRTUAL_SCREEN_WIDTH` | 65536 | Max virtual screen width |
| `MAX_VIRTUAL_SCREEN_HEIGHT` | 65536 | Max virtual screen height |
| `MAX_VIRTUAL_SCREEN_REFRESH_RATE` | 120 | Max virtual screen refresh rate |
| `ORIGINAL_FOLD_SCREEN_AMOUNT` | 2 | Original fold screen count |

### 2. RSScreen

**Location:** `rosen/modules/render_service/screen_manager/rs_screen.h`

**Responsibilities:**
- Abstract single screen instance (physical or virtual)
- Encapsulate HDI interaction for physical screens (HdiScreen)
- Manage screen properties via RSScreenThreadSafeProperty
- Notify property changes via onPropertyChange_ callback
- Support ROG resolution, sampling scale, active rect, HDR AI detection

**Key Methods:**
```cpp
// Physical screen init
explicit RSScreen(ScreenId id);      // creates HdiScreen, init mode/capability/HDR/power
explicit RSScreen(const VirtualScreenConfigs& configs);  // virtual screen init

// Property change macro
#define UPDATE_PROPERTY(name, value)            \
    do {                                        \
        auto prop = property_.Set##name(value); \
        NotifyScreenPropertyChange(prop);       \
    } while (0)

// Mode/resolution
uint32_t SetActiveMode(uint32_t modeId);
int32_t SetResolution(uint32_t width, uint32_t height);
void SetRogResolution(uint32_t width, uint32_t height);

// Power & backlight
int32_t SetPowerStatus(uint32_t powerStatus);
void SetScreenBacklight(uint32_t level);

// Color & HDR
int32_t SetScreenColorGamut(int32_t modeIdx);
int32_t SetScreenHDRFormat(int32_t modeIdx);
int32_t SetScreenColorSpace(GraphicCM_ColorSpaceType colorSpace);

// Virtual screen specific
void ResizeVirtualScreen(uint32_t width, uint32_t height);
bool SetVirtualMirrorScreenCanvasRotation(bool canvasRotation);
bool SetVirtualMirrorScreenScaleMode(ScreenScaleMode scaleMode);

// Blacklist/whitelist
void SetBlackList(const std::unordered_set<NodeId>& blackList);
void AddWhiteList(const std::vector<NodeId>& whiteList);
void SetSecurityExemptionList(const std::vector<uint64_t>& securityExemptionList);
int32_t SetSecurityMask(std::shared_ptr<Media::PixelMap> securityMask);

// Active rect (for super fold display)
uint32_t SetScreenActiveRect(const Rect& activeRect);

// Notification
using OnPropertyChangeCallback = std::function<void(ScreenId, ScreenPropertyType,
    const sptr<ScreenPropertyBase>&)>;
void SetOnPropertyChangedCallback(OnPropertyChangeCallback callback);
```

**VirtualScreenConfigs:**
```cpp
struct VirtualScreenConfigs {
    ScreenId id;
    ScreenId associatedScreenId;
    std::string name;
    uint32_t width;
    uint32_t height;
    sptr<Surface> surface;
    GraphicPixelFormat pixelFormat;
    int32_t flags;
    std::unordered_set<uint64_t> whiteList;
};
```

### 3. RSScreenPreprocessor

**Location:** `rosen/modules/render_service/screen_manager/rs_screen_preprocessor.h`

**Responsibilities:**
- Register HDI callbacks (hotplug, refresh, HwcDead, HwcEvent, VBlankIdle)
- Buffer hotplug events and dispatch to main thread via ScheduleTask
- Handle HWC dead recovery (reset device, re-init)
- Forward notifications to RSScreenCallbackManager

**Key Methods:**
```cpp
static void OnHotPlug(std::shared_ptr<HdiOutput>& output, bool connected, void* data);
static void OnRefresh(ScreenId id, void* data);
static void OnHwcDead(void* data);
static void OnHwcEvent(uint32_t deviceId, uint32_t eventId,
    const std::vector<int32_t>& eventData, void* data);
static void OnScreenVBlankIdle(uint32_t devId, uint64_t ns, void* data);

bool Init() noexcept;  // register all HDI callbacks

void NotifyVirtualScreenConnected(ScreenId newId, ScreenId associatedScreenId,
    sptr<RSScreenProperty> property);
void NotifyVirtualScreenDisconnected(ScreenId id);
void NotifyActiveScreenIdChanged(ScreenId activeScreenId);
void NotifyScreenPropertyChanged(ScreenId id, ScreenPropertyType type,
    const sptr<ScreenPropertyBase>& property);

void ScheduleTask(std::function<void()> task);  // PostTask to mainHandler with IMMEDIATE priority
```

**ScreenHotPlugEvent:**
```cpp
struct ScreenHotPlugEvent {
    std::shared_ptr<HdiOutput> output;
    bool connected = false;
};
```

### 4. RSScreenThreadSafeProperty

**Location:** `rosen/modules/render_service/screen_manager/rs_screen_thread_safe_property.h`

**Responsibilities:**
- Read-write lock thread-safe wrapper for RSScreenProperty
- All Set* methods use unique_lock, all Get* methods use shared_lock
- Simple atomic fields (ID, IsVirtual) are not locked
- Set* methods return ResType for property change notification

**Key Methods:**
```cpp
using ResType = std::pair<ScreenPropertyType, sptr<ScreenPropertyBase>>;

ResType SetResolution(std::pair<uint32_t, uint32_t> resolution);
ResType SetPhysicalModeParams(uint32_t phyWidth, uint32_t phyHeight, uint32_t refreshRate);
ResType SetScreenColorGamut(ScreenColorGamut colorGamut);
ResType SetSkipFrameOption(uint32_t skipFrameInterval, uint32_t expectedRefreshRate,
    SkipFrameStrategy skipFrameStrategy);
ResType SetBlackList(const std::unordered_set<NodeId>& blackList);
ResType AddBlackList(const std::vector<NodeId>& blackList);
ResType SetProducerSurface(sptr<Surface> producerSurface);

sptr<RSScreenProperty> Clone() const;  // deep copy for safe external access
```

**Supported Property Types:**
ID, NAME, IS_VIRTUAL, RENDER_RESOLUTION, PHYSICAL_RESOLUTION_REFRESHRATE, OFFSET, SAMPLING_OPTION, COLOR_GAMUT, GAMUT_MAP, STATE, CORRECTION, CANVAS_ROTATION, AUTO_BUFFER_ROTATION, ACTIVE_RECT_OPTION, SKIP_FRAME_OPTION, PIXEL_FORMAT, HDR_FORMAT, VISIBLE_RECT_OPTION, WHITE_LIST, BLACK_LIST, TYPE_BLACK_LIST, SECURITY_EXEMPTION_LIST, SECURITY_MASK, ENABLE_SKIP_WINDOW, POWER_STATUS, SCREEN_TYPE, CONNECTION_TYPE, PRODUCER_SURFACE, SCALE_MODE, SCREEN_STATUS, VIRTUAL_SEC_LAYER_OPTION, IS_HARD_CURSOR_SUPPORT, SUPPORTED_COLOR_GAMUTS, DISABLE_POWER_OFF_RENDER_CONTROL, SCREEN_SWITCH_STATUS, SCREEN_FRAME_GRAVITY, IS_MAIN_SCREEN

### 5. TouchScreen

**Location:** `rosen/modules/render_service/screen_manager/touch_screen.h`

**Responsibilities:**
- dlopen dynamic loading wrapper for touch screen extension library
- Load `libthp_extra_innerapi.z.so` and get SetFeatureConfig/SetAftConfig symbols
- Uses DelayedSingleton pattern, global macro: `TOUCH_SCREEN`

**Key Methods:**
```cpp
void InitTouchScreen();  // dlopen + dlsym

using TsSetFeatureConfig = int32_t (*)(int32_t, const char *);
using TsSetAftConfig = int32_t (*)(const char *);

int32_t SetFeatureConfig(int32_t feature, const char *config);
int32_t SetAftConfig(const char *config);
```

## Thread Safety

```
┌──────────────────────────────────────────────────────────────────────────────────┐
│                          Thread Safety Design                                    │
├──────────────────────────────────────────────────────────────────────────────────┤
│                                                                                  │
│  ┌─────────────────────────────────────────────────────────────────────────────┐ │
│  │ Mutex                  │ Protected Data                    │ Lock Type       │ │
│  ├────────────────────────┼──────────────────────────────────┼─────────────────┤ │
│  │ screenMapMutex_        │ screens_                         │ std::mutex      │ │
│  │ hotPlugAndConnectMutex │ pendingConnectedIds_             │ std::mutex      │ │
│  │ powerStatusMutex_      │ screenPowerStatus_               │ std::mutex      │ │
│  │ backLightAndCorrection │ screenBacklight_, screenCorrection│ std::shared_mutex│ │
│  │ virtualScreenIdMutex_  │ freeVirtualScreenIds_            │ std::mutex      │ │
│  │ globalBlackListMutex_  │ globalBlackList_                 │ std::mutex      │ │
│  │ renderControlMutex_    │ disableRenderControlScreens_     │ std::mutex      │ │
│  │ propertyMutex_         │ property_ (RSScreenThreadSafe)   │ std::shared_mutex│ │
│  │ agentMtx_              │ agentListeners_                 │ std::mutex      │ │
│  │ clientToRenderMtx_     │ clientToRenderConns_            │ std::mutex      │ │
│  └─────────────────────────────────────────────────────────────────────────────┘ │
│                                                                                  │
│  Design:                                                                         │
│  ├── Write paths: unique_lock<shared_mutex> (exclusive)                         │
│  ├── Read paths:  shared_lock<shared_mutex> (concurrent)                        │
│  ├── Simple atomic fields (ID, IsVirtual): no lock needed                       │
│  └── ScheduleTask ensures all HDI events processed on main thread               │
│                                                                                  │
└──────────────────────────────────────────────────────────────────────────────────┘
```

## BUILD.gn

- Output: shared library `screen_manager_sources`
- Part: `graphic_2d`, Subsystem: `graphic`
- Sources: `rs_screen_callback_manager.cpp`, `rs_screen_manager_agent.cpp`, `rs_screen_preprocessor.cpp`, `rs_screen.cpp`, `rs_screen_manager.cpp`, `rs_screen_thread_safe_property.cpp`
- Dependencies: `libcomposer`, `librender_service_base`, `socketpair`
- Security: PAC return protection, CFI control flow integrity