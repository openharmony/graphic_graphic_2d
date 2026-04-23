# Color Picker Implementation

## Overview

The Color Picker system provides automatic color extraction from node content for adaptive UI theming. It supports multiple strategies (CONTRAST, CLIENT_CALLBACK) and uses GPU-accelerated color extraction with fence-based synchronization.

## Architecture

```
┌─────────────────────────────────────────────────────────────────────┐
│                     RSColorPickerDrawable                        │
│  (RSDrawable - manages lifecycle and state machine)              │
└─────────────────────────┬───────────────────────────────────────┘
                          │ uses
                          ▼
┌─────────────────────────────────────────────────────────────────────┐
│                  IColorPickerManager                            │
│  (Interface - defines color picker behavior contract)               │
└───────────────┬───────────────────────────────┬───────────────┘
                │                               │
      implements                           implements
                ▼                               ▼
┌─────────────────────────────┐   ┌───────────────────────────────┐
│   RSColorPickerManager     │   │   ColorPickAltManager        │
│  (CONTRAST strategy)       │   │  (CLIENT_CALLBACK strategy)  │
│  - Black/white output      │   │  - Notifies client of        │
│  - Smooth transitions      │   │    luminance zone changes    │
└────────────────┬────────────┘   └────────────┬──────────────────┘
                 │                            │
                 └───────────┬────────────────┘
                             │ uses
                             ▼
             ┌───────────────────────────────┐
             │    RSColorPickerUtils       │
             │  (GPU operations & utilities)  │
             └───────────┬───────────────┘
                         │
                         ▼
             ┌───────────────────────────────┐
             │    RSColorPickerThread      │
             │  (Async GPU task execution)  │
             └───────────────────────────────┘
```

## State Machine

Controls when color picks occur to prevent excessive GPU work:

```
┌─────────────┐                              ┌──────────────┐
│  PREPARING  │ ──ScheduleColorPickIfReady──>│   SCHEDULED  │
│             │    (return delay, caller     │              │
│             │     posts delayed task)      │              │
└──────┬──────┘                               └──────┬───────┘
       │                                             │
       │                                      delayed task
       ▼                                             ▼
┌──────────────────┐                     ┌──────────────────┐
│  PREPARING       │ <────────────────────│ COLOR_PICK_THIS  │
│                  │  (after pick)        │     _FRAME       │
└──────────────────┘                      └──────────────────┘
```

**States:**
- `PREPARING`: Idle, ready to schedule when dirty region intersects
- `SCHEDULED`: Waiting for delayed task to execute
- `COLOR_PICK_THIS_FRAME`: Execute color pick this frame

## Execution Flow

```
Main Thread (Prepare Phase)
├─ PrepareColorPickers() - called every frame
│  ├─ CollectColorPickerNodeIds() - find all ColorPicker nodes
│  └─ For each node:
│     ├─ PrepareColorPickerFrame() - handle state transitions
│     └─ If dirty region intersects:
│        └─ ScheduleColorPickIfReady() → PostTask(COLOR_PICK_THIS_FRAME, delay)
│
Main Thread (Delayed Task)
└─ Set state to COLOR_PICK_THIS_FRAME, mark node dirty, request vsync

Render Thread (Draw Phase)
└─ OnDraw()
   ├─ Get interpolated color, apply to canvas
   └─ If needColorPick_:
       └─ ExtractSnapshotAndScheduleColorPick()
          ├─ Try RSHeteroColorPicker (GPU path)
          └─ Fallback to ScheduleColorPickWithSemaphore()

Color Picker Thread
└─ ExecColorPick()
   ├─ Wait on SyncFence for GPU completion
   ├─ PickColor() - extract pixel
   └─ HandleColorUpdate() - update color
```

## GPU Synchronization

Uses fence-based synchronization:
1. Create Vulkan semaphore, flush surface with it
2. Get fence fd from semaphore
3. Post task with `ColorPickerInfo` containing fence fd
4. Wait on `SyncFence` before accessing texture

## HWC Integration

When color picker needs to pick (`COLOR_PICK_THIS_FRAME`), HWC is disabled for intersecting surfaces via `HandleColorPickerHwcDisable()`.

## IPC Callback (CLIENT_CALLBACK Strategy)

```
Application: RegisterColorPickerCallback()
                    ↓
Render Thread: Extract color via ColorPickAltManager
                    ↓
ColorPicker Thread: HandleColorUpdate() → NotifyClient()
                    ↓
Main Thread: SendColorPickerCallback() → IPC
                    ↓
Client: Application callback invoked
```
