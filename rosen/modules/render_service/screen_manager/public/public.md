# Screen Manager Agent Specification

## Overview

The `public` sub-module defines listener interfaces and the agent proxy layer for external modules. `RSIScreenManagerListener` is the core listener interface for RS internal modules; `RSIScreenManagerAgentListener` is the agent listener interface for DMS/SCB etc. `RSScreenManagerAgent` wraps `RSScreenManager` API calls and manages virtual screen lifecycle per agent.

## Agent Architecture

```
┌──────────────────────────────────────────────────────────────────────────────────┐
│                        Agent Proxy Layer Architecture                             │
├──────────────────────────────────────────────────────────────────────────────────┤
│                                                                                  │
│  ┌─────────────────────────────────────────────────────────────────────────────┐ │
│  │                        External Module (DMS/SCB)                            │ │
│  │                    ┌──────────────────────────────────┐                     │ │
│  │                    │   IPC Callbacks                   │                     │ │
│  │                    │   ├── RSIScreenChangeCallback      │                     │ │
│  │                    │   ├── RSIScreenSwitchingNotifyCallback│                 │ │
│  │                    │   ├── RSIExposedEventCallback       │                     │ │
│  │                    │   └── RSIActiveScreenIdChangedCallback│                 │ │
│  │                    └──────────────────────────────────┘                     │ │
│  └──────────────────────────┬──────────────────────────────────────────────────┘ │
│                              │                                                   │
│                              ▼                                                   │
│  ┌─────────────────────────────────────────────────────────────────────────────┐ │
│  │                     RSScreenManagerAgent                                    │ │
│  │         ┌──────────────────────────────────────────────────┐                 │ │
│  │         │  screenManager_ (sptr<RSScreenManager>)          │                 │ │
│  │         │  agentListener_ (sptr<RSScreenManagerAgentListener)│               │ │
│  │         │  virtualScreenIds_ (unordered_set<ScreenId>)     │                 │ │
│  │         └──────────────────────────────────────────────────┘                 │ │
│  │                                                                              │ │
│  │  Lifecycle:                                                                  │ │
│  │  ├── Constructor: create AgentListener + RegisterAgentListener              │ │
│  │  └── Destructor: UnRegisterAgentListener                                     │ │
│  │                                                                              │ │
│  │  API forwarding: all methods check screenManager_ != null → delegate call   │ │
│  │  Size validation: blacklist/whitelist/exemption ≤ MAX_SPECIAL_LAYER_NUM     │ │
│  │  Virtual screen tracking: virtualScreenIds_ tracks created screens          │ │
│  └─────────────────────────────────────────────────────────────────────────────┘ │
│                              │                                                   │
│                              ▼                                                   │
│  ┌─────────────────────────────────────────────────────────────────────────────┐ │
│  │                   RSScreenManagerAgentListener                               │ │
│  │         ┌──────────────────────────────────────────────────┐                 │ │
│  │         │  Implements RSIScreenManagerAgentListener         │                 │ │
│  │         │  ├── OnScreenConnected → screenChangeCallback_    │                 │ │
│  │         │  │   → OnScreenChanged(id, CONNECTED, reason, conn)│              │ │
│  │         │  ├── OnScreenDisconnected → screenChangeCallback_ │                 │ │
│  │         │  │   → OnScreenChanged(id, DISCONNECTED, reason)  │               │ │
│  │         │  ├── OnScreenSwitchingNotify                       │                 │ │
│  │         │  │   → screenSwitchingNotifyCallback_              │                 │ │
│  │         │  ├── OnHwcEvent                                    │                 │ │
│  │         │  │   → exposedEventCallbacks_ (EXT_SCREEN_UNSUPPORT)│              │ │
│  │         │  ├── OnActiveScreenIdChanged                       │                 │ │
│  │         │  │   → activeScreenIdChangedCallback_              │                 │ │
│  │         └──────────────────────────────────────────────────┘                 │ │
│  └─────────────────────────────────────────────────────────────────────────────┘ │
│                                                                                  │
└──────────────────────────────────────────────────────────────────────────────────┘
```

## Agent HwcEvent Decision

```
┌──────────────────────────────────────────────────────────────────────────────────┐
│                    Agent HwcEvent Decision Flow                                  │
├──────────────────────────────────────────────────────────────────────────────────┤
│                                                                                  │
│  OnHwcEvent(deviceId, eventId, eventData)                                        │
│    │                                                                             │
│    ▼                                                                             │
│  ┌──────────────────────────────┐                                                │
│  │ eventId == HWCEVENT_EXT_SCREEN_NOT_SUPPORT ?                                  │
│  └──────┬──────────────┬────────┘                                                │
│    Yes  │              │  No                                                     │
│         ▼              ▼                                                          │
│  ┌──────────────────┐ ┌──────────────────┐                                       │
│  │ Find exposedEvent │ │ No action        │                                       │
│  │ Callbacks_        │ │                  │                                       │
│  │ [EXT_SCREEN_      │ │                  │                                       │
│  │  UNSUPPORT]       │ │                  │                                       │
│  │ → OnDisplayEvent  │ │                  │                                       │
│  │   (RSExtScreen    │ │                  │                                       │
│  │    UnsupportData) │ │                  │                                       │
│  └──────────────────┘ └──────────────────┘                                       │
│                                                                                  │
└──────────────────────────────────────────────────────────────────────────────────┘
```

## Architecture Components

### 1. RSIScreenManagerListener

**Location:** `rosen/modules/render_service/screen_manager/public/rs_iscreen_manager_listener.h`

**Responsibilities:**
- Core listener interface for RS internal modules
- Receives all screen events including low-level HDI events
- OnScreenConnected returns IRemoteObject for Client-to-Render connection

**Key Methods:**
```cpp
virtual sptr<IRemoteObject> OnScreenConnected(ScreenId id,
    const std::shared_ptr<HdiOutput>& output,
    const sptr<RSScreenProperty>& property) = 0;
virtual void OnScreenDisconnected(ScreenId id) = 0;
virtual void OnHwcRestored(ScreenId id,
    const std::shared_ptr<HdiOutput>& output,
    const sptr<RSScreenProperty>& property) = 0;
virtual void OnHwcDead(ScreenId id) = 0;
virtual void OnScreenPropertyChanged(ScreenId id, ScreenPropertyType type,
    const sptr<ScreenPropertyBase>& property) = 0;
virtual void OnScreenRefresh(ScreenId id) = 0;
virtual void OnVBlankIdle(ScreenId id, uint64_t ns) = 0;
virtual void OnVirtualScreenConnected(ScreenId id, ScreenId associatedScreenId,
    const sptr<RSScreenProperty>& property) = 0;
virtual void OnVirtualScreenDisconnected(ScreenId id) = 0;
virtual void OnHwcEvent(uint32_t deviceId, uint32_t eventId,
    const std::vector<int32_t>& eventData) = 0;
virtual void OnActiveScreenIdChanged(ScreenId activeScreenId) = 0;
virtual void OnScreenBacklightChanged(ScreenId id, uint32_t level) = 0;
virtual void OnGlobalBlacklistChanged(const std::unordered_set<NodeId>& globalBlackList) = 0;
```

### 2. RSIScreenManagerAgentListener

**Location:** `rosen/modules/render_service/screen_manager/public/rs_iscreen_manager_agent_listener.h`

**Responsibilities:**
- Agent listener interface for external modules (DMS/SCB)
- Receives high-level events with additional context (reason, IPC connection)

**Key Methods:**
```cpp
virtual void OnScreenConnected(ScreenId id, ScreenChangeReason reason,
    sptr<IRemoteObject> remoteConn) = 0;
virtual void OnScreenDisconnected(ScreenId id, ScreenChangeReason reason) = 0;
virtual void OnScreenSwitchingNotify(bool status) = 0;
virtual void OnHwcEvent(uint32_t deviceId, uint32_t eventId,
    const std::vector<int32_t>& eventData) = 0;
virtual void OnActiveScreenIdChanged(ScreenId activeScreenId) = 0;
```

### 3. RSScreenManagerAgentListener

**Location:** `rosen/modules/render_service/screen_manager/public/rs_screen_manager_agent.h`

**Responsibilities:**
- Implements RSIScreenManagerAgentListener
- Holds multiple IPC callback objects for different event types
- OnHwcEvent: special handling for HWCEVENT_EXT_SCREEN_NOT_SUPPORT → triggers EXT_SCREEN_UNSUPPORT exposed event

**Key Members:**
```cpp
sptr<RSIScreenChangeCallback> screenChangeCallback_;
sptr<RSIScreenSwitchingNotifyCallback> screenSwitchingNotifyCallback_;
std::unordered_map<RSExposedEventType, sptr<RSIExposedEventCallback>> exposedEventCallbacks_;
sptr<RSIActiveScreenIdChangedCallback> activeScreenIdChangedCallback_;
```

**Key Methods:**
```cpp
void OnScreenConnected(ScreenId id, ScreenChangeReason reason,
    sptr<IRemoteObject> remoteConn) override;
void OnScreenDisconnected(ScreenId id, ScreenChangeReason reason) override;
void OnScreenSwitchingNotify(bool status) override;
void OnHwcEvent(uint32_t deviceId, uint32_t eventId,
    const std::vector<int32_t>& eventData) override;
void OnActiveScreenIdChanged(ScreenId activeScreenId) override;

void SetScreenChangeCallback(sptr<RSIScreenChangeCallback> callback);
void SetExposedEventCallback(const RSExposedEventType type,
    const sptr<RSIExposedEventCallback> callback);
void SetScreenSwitchingNotifyCallback(sptr<RSIScreenSwitchingNotifyCallback> callback);
void SetActiveScreenIdChangedCallback(sptr<RSIActiveScreenIdChangedCallback> callback);
```

### 4. RSScreenManagerAgent

**Location:** `rosen/modules/render_service/screen_manager/public/rs_screen_manager_agent.h`

**Responsibilities:**
- Proxy layer for external modules to access RSScreenManager APIs
- Manage virtual screen lifecycle per agent (virtualScreenIds_)
- Auto-register/unregister AgentListener on construction/destruction
- Validate size limits for blacklist/whitelist/exemption operations

**Key Members:**
```cpp
sptr<RSScreenManager> screenManager_;
sptr<RSScreenManagerAgentListener> agentListener_;
std::unordered_set<ScreenId> virtualScreenIds_;
std::mutex mutex_;
```

**Key Methods:**
```cpp
// Lifecycle
RSScreenManagerAgent(sptr<RSScreenManager> screenManager);
~RSScreenManagerAgent();

// Callback registration
int32_t SetScreenChangeCallback(const sptr<RSIScreenChangeCallback>& callback);
int32_t SetExposedEventCallback(const RSExposedEventType type,
    const sptr<RSIExposedEventCallback>& callback);
int32_t SetScreenSwitchingNotifyCallback(sptr<RSIScreenSwitchingNotifyCallback> callback);
int32_t SetActiveScreenIdChangedCallback(sptr<RSIActiveScreenIdChangedCallback> callback);

// Virtual screen
ScreenId CreateVirtualScreen(const RSVirtualScreenOption& vsOption, int32_t& errorCode);
void RemoveVirtualScreen(ScreenId id);
void CleanVirtualScreens();  // batch cleanup all agent-created virtual screens

// Screen query & set — all forward to screenManager_
ErrCode GetDefaultScreenId(uint64_t& screenId);
ErrCode GetActiveScreenId(uint64_t& screenId);
void SetScreenPowerStatus(ScreenId id, ScreenPowerStatus status);
void SetScreenBacklight(ScreenId id, uint32_t level);
int32_t SetScreenColorGamut(ScreenId id, int32_t modeIdx);
int32_t SetScreenHDRFormat(ScreenId id, int32_t modeIdx);
// ... and all other screen management APIs
```