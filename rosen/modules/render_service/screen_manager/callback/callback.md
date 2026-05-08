# Screen Callback Manager Specification

## Overview

The `callback` sub-module provides two-level event callback dispatch for screen events. Core Listener receives low-level HDI events for RS internal modules; Agent Listener receives high-level events for external modules (DMS/SCB etc.).

## Callback Dispatch Architecture

```
┌──────────────────────────────────────────────────────────────────────────────────┐
│                      Two-Level Callback Dispatch Architecture                    │
├──────────────────────────────────────────────────────────────────────────────────┤
│                                                                                  │
│  ┌─────────────────────────────────────────────────────────────────────────────┐ │
│  │                        Screen Event Source                                  │ │
│  │   ├── RSScreenPreprocessor (HDI hotplug/refresh/HwcDead/HwcEvent/VBlank)   │ │
│  │   ├── RSScreenManager (virtual screen, backlight, blacklist, active screen) │ │
│  │   └── RSScreen (property change via onPropertyChange_ callback)            │ │
│  └──────────────────────────┬──────────────────────────────────────────────────┘ │
│                              │                                                   │
│                              ▼                                                   │
│  ┌─────────────────────────────────────────────────────────────────────────────┐ │
│  │                     RSScreenCallbackManager                                  │ │
│  │         ┌──────────────────────────────────────────────────┐                 │ │
│  │         │  coreListener_ (single, RS internal)              │                 │ │
│  │         │  ├── OnScreenConnected → returns IRemoteObject    │                 │ │
│  │         │  ├── OnScreenDisconnected                         │                 │ │
│  │         │  ├── OnScreenPropertyChanged                      │                 │ │
│  │         │  ├── OnScreenRefresh                              │                 │ │
│  │         │  ├── OnHwcRestored / OnHwcDead                    │                 │ │
│  │         │  ├── OnVBlankIdle                                 │                 │ │
│  │         │  ├── OnVirtualScreenConnected / Disconnected      │                 │ │
│  │         │  ├── OnActiveScreenIdChanged                      │                 │ │
│  │         │  ├── OnScreenBacklightChanged                     │                 │ │
│  │         │  ├── OnGlobalBlacklistChanged                     │                 │ │
│  │         │  └── OnHwcEvent                                   │                 │ │
│  │         └──────────────────────────────────────────────────┘                 │ │
│  │         ┌──────────────────────────────────────────────────┐                 │ │
│  │         │  agentListeners_ (multiple, DMS/BootAnimation etc.)│               │ │
│  │         │  ├── OnScreenConnected(id, reason, remoteConn)    │                 │ │
│  │         │  ├── OnScreenDisconnected(id, reason)             │                 │ │
│  │         │  ├── OnScreenSwitchingNotify(status)              │                 │ │
│  │         │  ├── OnHwcEvent(deviceId, eventId, eventData)    │                 │ │
│  │         │  └── OnActiveScreenIdChanged(activeScreenId)      │                 │ │
│  │         └──────────────────────────────────────────────────┘                 │ │
│  │         ┌──────────────────────────────────────────────────┐                 │ │
│  │         │  clientToRenderConns_                             │                 │ │
│  │         │  ScreenId → IRemoteObject (from coreListener     │                 │ │
│  │         │  OnScreenConnected return value)                  │                 │ │
│  │         └──────────────────────────────────────────────────┘                 │ │
│  └─────────────────────────────────────────────────────────────────────────────┘ │
│                                                                                  │
└──────────────────────────────────────────────────────────────────────────────────┘
```

## Event Notification Matrix

```
┌──────────────────────────────────────────────────────────────────────────────────┐
│                      Event Notification Matrix                                   │
├──────────────────────────────────────────────────────────────────────────────────┤
│                                                                                  │
│  ┌─────────────────────────────────────────────────────────────────────────────┐ │
│  │ Notify Method              │ Core Listener         │ Agent Listener          │ │
│  ├────────────────────────────┼───────────────────────┼────────────────────────┤ │
│  │ NotifyScreenConnected      │ OnScreenConnected     │ OnScreenConnected      │ │
│  │                            │ (returns IRemoteObj)  │ (id, reason, conn)     │ │
│  │ NotifyScreenDisconnected   │ OnScreenDisconnected  │ OnScreenDisconnected   │ │
│  │                            │                       │ (id, reason)           │ │
│  │ NotifyHwcRestored          │ OnHwcRestored         │ OnScreenConnected      │ │
│  │                            │                       │ (reason=HWCDEAD)       │ │
│  │ NotifyHwcDead              │ OnHwcDead             │ OnScreenDisconnected   │ │
│  │                            │                       │ (reason=HWCDEAD)       │ │
│  │ NotifyScreenPropertyUpdated│ OnScreenPropertyChanged│ No                    │ │
│  │ NotifyScreenRefresh        │ OnScreenRefresh       │ No                     │ │
│  │ NotifyVBlankIdle           │ OnVBlankIdle          │ No                     │ │
│  │ NotifyVirtualScreenConnected│ OnVirtualScreenConnected│ No                   │ │
│  │ NotifyVirtualScreenDisconnected│ OnVirtualScreenDisconnected│ No              │ │
│  │ NotifyActiveScreenIdChanged│ OnActiveScreenIdChanged│ OnActiveScreenIdChanged│ │
│  │ NotifyScreenBacklightChanged│ OnScreenBacklightChanged│ No                   │ │
│  │ NotifyGlobalBlacklistChanged│ OnGlobalBlacklistChanged│ No                   │ │
│  │ NotifyHwcEvent             │ OnHwcEvent            │ OnHwcEvent             │ │
│  │ NotifySwitchingCallback    │ No                    │ OnScreenSwitchingNotify │ │
│  └─────────────────────────────────────────────────────────────────────────────┘ │
│                                                                                  │
└──────────────────────────────────────────────────────────────────────────────────┘
```

## clientToRenderConns Mechanism

```
┌──────────────────────────────────────────────────────────────────────────────────┐
│                      clientToRenderConns Data Flow                               │
├──────────────────────────────────────────────────────────────────────────────────┤
│                                                                                  │
│  NotifyScreenConnected(event)                                                    │
│    │                                                                             │
│    ▼                                                                             │
│  ┌─────────────────────────────────────────────────────────────────────────────┐ │
│  │ 1. Call coreListener_->OnScreenConnected(event.id, event.output,            │ │
│  │    event.property)                                                          │ │
│  │    → coreListener returns sptr<IRemoteObject> (Client-to-Render connection) │ │
│  │                                                                              │ │
│  │ 2. Store: clientToRenderConns_[event.id] = IRemoteObject                    │ │
│  │    (protected by clientToRenderMtx_)                                         │ │
│  │                                                                              │ │
│  │ 3. Notify agent listeners: OnScreenConnected(id, reason, conn)              │ │
│  │    → conn = GetClientToRenderConnection(id)                                  │ │
│  │                                                                              │ │
│  │ 4. NotifyScreenDisconnected → clientToRenderConns_.erase(screenId)          │ │
│  └─────────────────────────────────────────────────────────────────────────────┘ │
│                                                                                  │
└──────────────────────────────────────────────────────────────────────────────────┘
```

## Architecture Components

### RSScreenCallbackManager

**Location:** `rosen/modules/render_service/screen_manager/callback/rs_screen_callback_manager.h`

**Responsibilities:**
- Manage core listener (single instance for RS internal) and agent listeners (multiple for external)
- Dispatch screen events to both listener levels
- Maintain Client-to-Render connection mapping from core listener return values
- Handle targeted notification to specific agent listener

**Key Methods:**
```cpp
// Listener management
void SetCoreListener(const sptr<RSIScreenManagerListener>& listener);
void AddAgentListener(const sptr<RSIScreenManagerAgentListener>& listener);
void RemoveAgentListener(const sptr<RSIScreenManagerAgentListener>& listener);

// Screen presence events
void NotifyScreenConnected(const ScreenPresenceEvent& event);
void NotifyScreenDisconnected(ScreenId screenId);
void NotifyHwcRestored(const ScreenPresenceEvent& event);
void NotifyHwcDead(ScreenId id);

// Property & refresh events
void NotifyScreenPropertyUpdated(ScreenId id, ScreenPropertyType type,
    const sptr<ScreenPropertyBase>& property);
void NotifyScreenRefresh(ScreenId id);
void NotifyVBlankIdle(ScreenId id, uint64_t ns);

// Virtual screen events
void NotifyVirtualScreenConnected(ScreenId id, ScreenId associatedScreenId,
    sptr<RSScreenProperty> property);
void NotifyVirtualScreenDisconnected(ScreenId id);

// Active screen & backlight
void NotifyActiveScreenIdChanged(ScreenId activeScreenId);
void NotifyScreenBacklightChanged(ScreenId id, uint32_t level);
void NotifyGlobalBlacklistChanged(const std::unordered_set<NodeId>& globalBlackList);

// HWC event & switching
void NotifyHwcEvent(uint32_t deviceId, uint32_t eventId,
    const std::vector<int32_t>& eventData);
void NotifySwitchingCallback(bool status);

// Targeted notification
void NotifyScreenConnectedToAgentListener(ScreenId id, ScreenChangeReason reason,
    sptr<RSIScreenManagerAgentListener> agentListener);
void NotifyActiveScreenIdChangedToAgentListener(ScreenId activeScreenId,
    sptr<RSIScreenManagerAgentListener> agentListener);

// Connection query
sptr<IRemoteObject> GetClientToRenderConnection(ScreenId id) const;
```

**ScreenPresenceEvent:**
```cpp
struct ScreenPresenceEvent {
    ScreenId id;
    std::shared_ptr<HdiOutput> output;
    const sptr<RSScreenProperty> property;
};
```

**Key Members:**
| Member | Type | Description |
|--------|------|-------------|
| `coreListener_` | `sptr<RSIScreenManagerListener>` | Core listener instance |
| `agentListeners_` | `std::vector<sptr<RSIScreenManagerAgentListener>>` | Agent listener list |
| `clientToRenderConns_` | `std::unordered_map<ScreenId, sptr<IRemoteObject>>` | Client-to-Render connection mapping |
| `agentMtx_` | `std::mutex` | Agent listener list guard |
| `clientToRenderMtx_` | `std::mutex` | Connection mapping guard |