# Fold Screen Manager Specification

## Overview

The `fold` sub-module provides fold screen management via posture sensor. It detects fold angle to determine FoldState (FOLDED/EXPAND) and switches active screen between inner screen (id=0) and external screen. Uses conditional compilation (`RS_SUBSCRIBE_SENSOR_ENABLE`) for sensor support.

## FoldState Decision Graph

```
┌──────────────────────────────────────────────────────────────────────────────────┐
│                       FoldState Angle Decision                                   │
├──────────────────────────────────────────────────────────────────────────────────┤
│                                                                                  │
│  Posture Sensor Data (angle)                                                     │
│    │                                                                             │
│    ▼                                                                             │
│  ┌──────────────────────────────┐                                                │
│  │ angle in valid range [0, 180]?                                                 │
│  └──────┬──────────────┬────────┘                                                │
│    Yes  │              │  No                                                     │
│         ▼              ▼                                                          │
│  ┌──────────────────┐ ┌──────────────────┐                                       │
│  │ TransferAngleTo  │ │ Log warning      │                                       │
│  │ ScreenState()    │ │ (invalid angle)  │                                       │
│  └──────┬───────────┘ └──────────────────┘                                       │
│         │                                                                        │
│         ▼                                                                        │
│  ┌─────────────────────────────────────────────────────────────────────────────┐ │
│  │ angle ≤ 25° ?                                                                │ │
│  │   ├── Yes → FoldState::FOLDED → activeScreenId = externalScreenId_         │ │
│  │   └── No  → FoldState::EXPAND → activeScreenId = innerScreenId_ (0)        │ │
│  └─────────────────────────────────────────────────────────────────────────────┘ │
│         │                                                                        │
│         ▼                                                                        │
│  ┌─────────────────────────────────────────────────────────────────────────────┐ │
│  │ activeScreenId_ changed?                                                     │ │
│  │   ├── Yes → screenPreprocessor_.NotifyActiveScreenIdChanged(targetScreenId) │ │
│  │   └── No  → No action                                                       │ │
│  └─────────────────────────────────────────────────────────────────────────────┘ │
│                                                                                  │
└──────────────────────────────────────────────────────────────────────────────────┘
```

## Sensor Lifecycle Graph

```
┌──────────────────────────────────────────────────────────────────────────────────┐
│                       Sensor Lifecycle Management                                │
├──────────────────────────────────────────────────────────────────────────────────┤
│                                                                                  │
│  Init()                                                                          │
│    │                                                                             │
│    ▼                                                                             │
│  ┌─────────────────────────────────────────────────────────────────────────────┐ │
│  │ RegisterSensorCallback()                                                     │ │
│  │   ├── Bind HandlePostureData as sensor callback                             │ │
│  │   ├── SubscribeSensor(SENSOR_TYPE_ID_POSTURE, &sensorUser_)                 │ │
│  │   ├── SetBatch(interval=4000000ns)                                           │ │
│  │   ├── ActivateSensor(SENSOR_TYPE_ID_POSTURE)                                │ │
│  │   └── Retry up to 5 times on failure (1ms wait between retries)            │ │
│  └─────────────────────────────────────────────────────────────────────────────┘ │
│    │                                                                             │
│    ▼                                                                             │
│  ┌─────────────────────────────────────────────────────────────────────────────┐ │
│  │ WatchSystemProperty("bootevent.boot.completed", OnBootComplete)             │ │
│  │   └── When boot completed → UnRegisterSensorCallback()                     │ │
│  │       ├── DeactivateSensor(SENSOR_TYPE_ID_POSTURE)                          │ │
│  │       └── UnsubscribeSensor(SENSOR_TYPE_ID_POSTURE)                         │ │
│  │       (DMS takes over fold control after boot)                               │ │
│  └─────────────────────────────────────────────────────────────────────────────┘ │
│                                                                                  │
└──────────────────────────────────────────────────────────────────────────────────┘
```

## Architecture Components

### RSFoldScreenManager

**Location:** `rosen/modules/render_service/screen_manager/product_feature/fold/rs_fold_screen_manager.h`

**Responsibilities:**
- Detect fold angle via posture sensor (SENSOR_TYPE_ID_POSTURE)
- Determine FoldState (FOLDED/EXPAND) based on angle threshold
- Switch activeScreenId_ between inner (id=0) and external screen
- Notify active screen change via RSScreenPreprocessor
- Register/unregister sensor callback, watch boot completion property

**FoldState Enum:**
```cpp
enum class FoldState : uint32_t {
    UNKNOW,
    FOLDED,   // angle ≤ 25° → use external screen
    EXPAND    // angle > 25° → use inner screen
};
```

**Key Methods:**
```cpp
void Init();  // RegisterSensorCallback + WatchSystemProperty
ScreenId GetActiveScreenId();  // returns activeScreenId_ (or INVALID_SCREEN_ID without sensor)
void SetExternalScreenId(ScreenId externalScreenId);  // set once, immutable after first assignment

#ifdef RS_SUBSCRIBE_SENSOR_ENABLE
void RegisterSensorCallback();
void UnRegisterSensorCallback();
static void OnBootComplete(const char* key, const char* value, void* context);
void HandlePostureData(const SensorEvent* const event);
void HandleSensorData(float angle);  // main thread, decide fold state
#endif
```

**Key Members (RS_SUBSCRIBE_SENSOR_ENABLE):**
```cpp
RSScreenPreprocessor& screenPreprocessor_;
std::shared_ptr<AppExecFwk::EventHandler> mainHandler_;
SensorUser sensorUser_;
bool hasRegisterSensorCallback_ = false;
mutable std::mutex registerSensorMutex_;
const ScreenId innerScreenId_ = 0;
ScreenId externalScreenId_ = INVALID_SCREEN_ID;
ScreenId activeScreenId_ = INVALID_SCREEN_ID;
mutable std::mutex activeScreenIdAssignedMutex_;
```

**Key Constants:**
| Constant | Value | Description |
|----------|-------|-------------|
| `ANGLE_MIN_VAL` | 0.0 | Min valid angle |
| `ANGLE_MAX_VAL` | 180.0 | Max valid angle |
| `OPEN_HALF_FOLDED_MIN_THRESHOLD` | 25.0 | Fold state threshold |
| `WAIT_FOR_ACTIVE_SCREEN_ID_TIMEOUT` | 1000 | Active screen ID wait timeout (ms) |
| `POSTURE_INTERVAL` | 4000000 | Sensor sampling interval (ns) |
| `BOOTEVENT_BOOT_COMPLETED` | `"bootevent.boot.completed"` | Boot completed property name |