# Pointer Window Manager Feature Specification

## Overview

The `pointer_window_manager` provides pointer/hard cursor window management across multiple screens in the OpenHarmony graphics rendering pipeline. It handles cursor rendering, dirty region propagation, layer creation, and multi-screen visibility coordination.

## Screen Usage Flowchart

```
┌────────────────────────────────────────────────────────────────────────────┐
│                     Pointer Window Screen Interaction Flow                 │
├────────────────────────────────────────────────────────────────────────────┤
│                                                                            │
│  Frame Start                                                               │
│    │                                                                       │
│    ▼                                                                       │
│  ┌─────────────────────────────────────────────────────────────────────┐   │
│  │ UpdateHardCursorStatus(hardCursorNode, screenNode)                  │   │
│  │   - Check if screen supports hard cursor (IsHardCursorSupport())    │   │
│  │   - Check if TUI is enabled (TUI disables hard cursor)              │   │
│  │   - Set hard cursor status on node                                  │   │
│  └─────────────────────────────────────────────────────────────────────┘   │
│    │                                                                       │
│    ▼                                                                       │
│  ┌─────────────────────────────────────────────────────────────────────┐   │
│  │ CollectAllHardCursor(hardCursorNode, screenNode, displayNode)       │   │
│  │   - Check if node is hardware-enabled top surface                   │   │
│  │   - Check if node should paint or has hard cursor status            │   │
│  │   - Create drawable and add to hardCursorDrawableVec_               │   │
│  │   - Store (screenNodeId, displayNodeId, drawable) tuple             │   │
│  └─────────────────────────────────────────────────────────────────────┘   │
│    │                                                                       │
│    ▼                                                                       │
│  ┌─────────────────────────────────────────────────────────────────────┐   │
│  │ UpdatePointerDirtyToGlobalDirty(pointerWindow, screenNode)          │   │
│  │   - Get dirty manager from pointer window                           │   │
│  │   - If hard cursor status changed: merge last frame position        │   │
│  │   - Merge pointer dirty region to screen dirty region               │   │
│  │   - Set isNeedForceCommitByPointer_ flag                            │   │
│  └─────────────────────────────────────────────────────────────────────┘   │
│    │                                                                       │
│    ▼                                                                       │
│  ┌─────────────────────────────────────────────────────────────────────┐   │
│  │ Check Multi-Screen Visibility                                       │   │
│  │   - IsPointerInvisibleInMultiScreen()                               │   │
│  │   - Traverse all screens: check if any screen has                   │   │
│  │     - Empty type blacklist AND                                      │   │
│  │     - (IsMirrorScreen() OR IsVirtual())                             │   │
│  │   - If found: pointer is visible (return false)                     │   │
│  │   - If not found: pointer is invisible (return true)                │   │
│  └─────────────────────────────────────────────────────────────────────┘   │
│    │                                                                       │
│    ▼                                                                       │
│  ┌─────────────────────────────────────────────────────────────────────┐   │
│  │ Layer Creation (Render Thread)                                      │   │
│  │   - HardCursorCreateLayer(processor, screenNodeId)                  │   │
│  │   - GetHardCursorDrawable(screenNodeId) from vec                    │   │
│  │   - processor->CreateLayerForRenderThread(drawable)                 │   │
│  │   - Store commit result in hardCursorCommitResultMap_               │   │
│  └─────────────────────────────────────────────────────────────────────┘   │
│    │                                                                       │
│    ▼                                                                       │
│  ┌─────────────────────────────────────────────────────────────────────┐   │
│  │ Check Commit Need                                                   │   │
│  │   - GetHardCursorNeedCommit(screenNodeId)                           │   │
│  │   - Check if drawable has buffer                                    │   │
│  │   - Compare with last frame commit result                           │   │
│  │   - Return true if buffer status changed                            │   │
│  └─────────────────────────────────────────────────────────────────────┘   │
│    │                                                                       │
│    ▼                                                                       │
│  Frame End (ResetHardCursorDrawables())                                    │
│                                                                            │
└────────────────────────────────────────────────────────────────────────────┘
```

### Frame Processing Sequence

| Step | Method | Thread | Purpose |
|------|--------|--------|---------|
| 1 | `UpdateHardCursorStatus()` | Main | Check screen support and TUI, set cursor status |
| 2 | `CollectAllHardCursor()` | Main | Collect drawables, store screen/display IDs |
| 3 | `UpdatePointerDirtyToGlobalDirty()` | Main | Propagate dirty regions to screen |
| 4 | `IsPointerInvisibleInMultiScreen()` | Main | Check visibility across all screens |
| 5 | `HardCursorCreateLayer()` | Render | Create layers for each screen |
| 6 | `GetHardCursorNeedCommit()` | Render | Check if commit needed per screen |
| 7 | `ResetHardCursorDrawables()` | Render | Clear frame state |

## Screen Interaction Matrix

| Operation | Screen Node Role | Display Node Role | Multi-Screen Handling |
|-----------|------------------|-------------------|----------------------|
| **UpdateHardCursorStatus** | Check IsHardCursorSupport() | N/A | Per-screen check |
| **CollectAllHardCursor** | Store screenNodeId in tuple | Store displayNodeId in tuple | Collects per screen/display pair |
| **UpdatePointerDirtyToGlobalDirty** | Merge dirty to screen dirty manager | N/A | Per-screen dirty propagation |
| **IsPointerInvisibleInMultiScreen** | Traverse all screens | N/A | Global visibility check |
| **HardCursorCreateLayer** | Use screenNodeId to find drawable | N/A | Per-screen layer creation |
| **GetHardCursorNeedCommit** | Use screenNodeId for commit map lookup | N/A | Per-screen commit tracking |
| **GetHardCursorDrawable** | Match screenNodeId in vec | Match displayNodeId in vec | Returns drawable for specific screen |

## Screen Type Behavior

### Hard Cursor Support Logic

**CheckHardCursorSupport(screenNode)** - Complete support check combining:
- `screenNode->GetScreenProperty().IsHardCursorSupport()` - Hardware support capability
- `!RSPointerWindowManager::Instance().IsTuiEnabled()` - TUI mode disabled

**IsHardCursorSupport()** - Hardware-only support check on screen property

| Screen Type | Hard Cursor Support | Visibility Check | Notes |
|-------------|---------------------|-------------------|-------|
| **Physical Single Screen** | CheckHardCursorSupport() = IsHardCursorSupport() && !TUI | Visible by default | Normal cursor rendering on single physical display |
| **Physical Multiscreen** | CheckHardCursorSupport() = IsHardCursorSupport() && !TUI | Visible by default | Cursor rendering across multiple physical displays |
| **Virtual Screen** | CheckHardCursorSupport() = IsHardCursorSupport() && !TUI | Visible if type blacklist empty | Used for virtual displays |

## Per-Screen State Management

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                    Per-Screen State Storage                                 │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                             │
│  hardCursorDrawableVec_:                                                    │
│  ┌─────────────────────────────────────────────────────────────────────┐    │
│  │ Vector: std::tuple<screenNodeId, displayNodeId, drawable>           │    │
│  │ - Stores all hard cursor drawables for current frame                 │   │
│  │ - Cleared at frame end (ResetHardCursorDrawables())                 │    │
│  │ - Used for layer creation and commit checking                        │   │
│  └─────────────────────────────────────────────────────────────────────┘    │
│                                                                             │
│  hardCursorCommitResultMap_:                                                │
│  ┌─────────────────────────────────────────────────────────────────────┐    │
│  │ Map: screenNodeId → lastFrameCommitResult (bool)                    │    │
│  │ - Tracks last frame's layer creation success per screen             │    │
│  │ - Used to detect buffer status changes (GetHardCursorNeedCommit())  │    │
│  │ - Removed via RemoveCommitResult(screenNodeId)                       │   │
│  └─────────────────────────────────────────────────────────────────────┘    │
│                                                                             │
│  hardCursorNodeMap_:                                                        │
│  ┌─────────────────────────────────────────────────────────────────────┐    │
│  │ Map: nodeId → RSSurfaceRenderNode                                   │    │
│  │ - Stores all hard cursor nodes (global, not per-screen)             │    │
│  │ - Populated via SetSetHardCursorNodeInfo()                          │    │
│  │ - Used for HardCursorCreateLayerForDirect()                          │   │
│  └─────────────────────────────────────────────────────────────────────┘    │
│                                                                             │
└─────────────────────────────────────────────────────────────────────────────┘
```

## Key Methods by Screen Interaction

### Screen Query Operations

| Method | Purpose | Screen Usage |
|--------|---------|--------------|
| `CheckHardCursorSupport(screenNode)` | Check if screen supports hard cursor | Reads screen property |
| `GetScreenRenderNode(screenId)` | Retrieve screen node by ID | Traverses screen node map |
| `IsPointerInvisibleInMultiScreen()` | Check visibility across all screens | Traverses all screens |

### Screen Update Operations

| Method | Purpose | Screen Usage |
|--------|---------|--------------|
| `UpdateHardCursorStatus(node, screenNode)` | Update cursor status based on screen | Reads screen support flag |
| `UpdatePointerDirtyToGlobalDirty(pointerWindow, screenNode)` | Propagate dirty regions | Merges to screen dirty manager |
| `CollectAllHardCursor(node, screenNode, displayNode)` | Collect drawables for frame | Stores screen/display IDs |

### Screen Render Operations

| Method | Purpose | Screen Usage |
|--------|---------|--------------|
| `HardCursorCreateLayer(processor, screenNodeId)` | Create layer for screen | Finds drawable by screen ID |
| `GetHardCursorNeedCommit(screenNodeId)` | Check if commit needed | Checks commit map by screen ID |
| `RemoveCommitResult(screenNodeId)` | Clear commit state | Removes from commit map |

## Multi-Screen Coordination

### Visibility Decision Flow

```
┌─────────────────────────────────────────────────────────────────────────────┐
│              IsPointerInvisibleInMultiScreen() Logic                        │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                             │
│  Initialize: isPointerInvisible = true                                      │
│    │                                                                        │
│    ▼                                                                        │
│  ┌─────────────────────────────────────────────────────────────────────┐    │
│  │ Traverse all screen nodes via NodeMap.TraverseScreenNodes()       │      │
│  └─────────────────────────────────────────────────────────────────────┘    │
│    │                                                                        │
│    ▼                                                                        │
│  ┌─────────────────────────────────────────────────────────────────────┐    │
│  │ For each screen:                                                    │    │
│  │   1. Get screen property                                            │    │
│  │   2. Check if type blacklist is empty                              │     │
│  │   3. Check if screen is mirror OR virtual                           │    │
│  │   4. If both true:                                                  │    │
│  │        - isPointerInvisible = false                                  │   │
│  │        - Break (found visible screen)                               │    │
│  └─────────────────────────────────────────────────────────────────────┘    │
│    │                                                                        │
│    ▼                                                                        │
│  Return isPointerInvisible                                                  │
│  - true: No screen with empty blacklist AND (mirror OR virtual)             │
│  - false: At least one screen makes pointer visible                         │
│                                                                             │
└─────────────────────────────────────────────────────────────────────────────┘
```

### Per-Screen Dirty Region Propagation

```
┌─────────────────────────────────────────────────────────────────────────────┐
│           UpdatePointerDirtyToGlobalDirty() Screen Flow                     │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                             │
│  Input: pointerWindow, curScreenNode                                        │
│    │                                                                        │
│    ▼                                                                        │
│  ┌─────────────────────────────────────────────────────────────────────┐    │
│  │ Get dirty manager from pointer window                                │   │
│  └─────────────────────────────────────────────────────────────────────┘    │
│    │                                                                        │
│    ▼                                                                        │
│  ┌─────────────────────────────────────────────────────────────────────┐    │
│  │ If hard cursor status changed (current != last):                    │    │
│  │   - Get last frame surface position from screen node                │    │
│  │   - Merge position to screen dirty manager                          │    │
│  └─────────────────────────────────────────────────────────────────────┘    │
│    │                                                                        │
│    ▼                                                                        │
│  ┌─────────────────────────────────────────────────────────────────────┐    │
│  │ Get pointer window dirty region                                      │   │
│  │ If dirty region is not empty:                                       │    │
│  │   - Merge to screen HWC dirty region                                │    │
│  │   - Clear pointer dirty region                                      │    │
│  │   - Set isNeedForceCommitByPointer_ = true                          │    │
│  │ Else:                                                                │   │
│  │   - Set isNeedForceCommitByPointer_ = false                         │    │
│  └─────────────────────────────────────────────────────────────────────┘    │
│                                                                             │
└─────────────────────────────────────────────────────────────────────────────┘
```

## Thread Safety

| Member | Protection | Notes |
|--------|------------|-------|
| `mtx_` | std::mutex | Protects bound updates and rsNodeId |
| `isPointerEnableHwc_` | std::atomic<bool> | Thread-safe HWC enable flag |
| `isPointerCanSkipFrame_` | std::atomic<bool> | Thread-safe frame skip flag |
| `boundHasUpdate_` | std::atomic<bool> | Thread-safe bound update flag |
| `hardCursorDrawableVec_` | No explicit protection | Cleared at frame end, render thread only |
| `hardCursorCommitResultMap_` | No explicit protection | Per-screen tracking, render thread only |
| `hardCursorNodeMap_` | No explicit protection | Main thread updates |

## Optimization Rules

### Frame Skip Optimization

**Rule:** Skip frame commit when hard cursor buffer status unchanged

**Logic:**
1. Get hard cursor drawable for screen
2. Check if drawable has buffer (current frame)
3. Get last frame commit result from map
4. Compare current buffer status with last commit result
5. Skip frame if status unchanged, force commit if status changed

**Thread-safe skip flag:**
- Use atomic compare_exchange_weak to check and update skip flag

### Dirty Region Optimization

**Rule:** Only propagate dirty regions when hard cursor actually moves or changes

**Logic:**
1. Validate pointer window and screen node exist
2. Get dirty manager from pointer window
3. If hard cursor status changed (visible ↔ invisible):
   - Get last frame surface position from screen
   - Merge position to screen dirty manager
4. Get pointer window dirty region
5. If dirty region not empty:
   - Merge to screen HWC dirty region
   - Clear pointer dirty region
   - Set force commit flag
6. Otherwise, clear force commit flag

### Multi-Screen Visibility Optimization

**Rule:** Early exit when pointer visible on any screen

**Logic:**
1. Initialize pointer as invisible
2. Traverse all screen nodes
3. For each screen:
   - Check if type blacklist is empty
   - Check if screen is mirror OR virtual
   - If both conditions true: mark pointer visible and exit early
4. Return visibility result

### Bound Update Optimization

**Rule:** Only update pointer bounds when bound flag is set

**Logic:**
1. Lock mutex and check bound update flag
2. If flag not set, return early
3. Copy rsNodeId and bound data locally
4. Unlock mutex
5. Validate rsNodeId is valid
6. Get render node from node map
7. Set bounds on node properties
8. Mark node dirty and apply modifiers
9. Add to pending sync list
10. Update HWC node properties if surface node exists
11. Update HWC layer info if screen ID is valid

### Layer Creation Optimization

**Rule:** Skip layer creation when buffer already consumed

**Logic:**
1. Get hard cursor node map
2. For each hard cursor node:
   - Check if node is hardware-enabled top surface
   - Get surface handler, skip if null
   - Get render params, skip if null
   - If buffer not consumed and pre-buffer exists:
     - Set pre-buffer to nullptr
     - Add to pending sync list
   - Create layer via processor

## Integration Points

### Main Thread
- `SetHardCursorNodeInfo()` - Register hard cursor nodes
- `UpdatePointerDirtyToGlobalDirty()` - Propagate dirty regions
- `CollectAllHardCursor()` - Prepare drawables for frame
- `UpdateHardCursorStatus()` - Update cursor status per screen

### Render Thread
- `HardCursorCreateLayer()` - Create layers for each screen
- `GetHardCursorNeedCommit()` - Check commit requirement per screen
- `ResetHardCursorDrawables()` - Clear frame state

### Cross-Thread
- `SetHwcNodeBounds()` - Called from ipc thread, protected by mutex
- `UpdatePointerInfo()` - Updates node properties with mutex protection
