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

| Screen Type | Hard Cursor Support | Visibility Check | Notes |
|-------------|---------------------|-------------------|-------|
| **Physical Screen** | Depends on IsHardCursorSupport() | Visible by default | Normal cursor rendering |
| **Virtual Screen** | Depends on IsHardCursorSupport() | Visible if type blacklist empty | Used for virtual displays |
| **Mirror Screen** | Depends on IsHardCursorSupport() | Visible if type blacklist empty | Mirrors source screen |
| **TUI Mode** | Disabled (always false) | N/A | TUI disables hard cursor globally |

## Per-Screen State Management

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                    Per-Screen State Storage                                │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                             │
│  hardCursorDrawableVec_:                                                   │
│  ┌─────────────────────────────────────────────────────────────────────┐   │
│  │ Vector: std::tuple<screenNodeId, displayNodeId, drawable>           │   │
│  │ - Stores all hard cursor drawables for current frame                 │   │
│  │ - Cleared at frame end (ResetHardCursorDrawables())                 │   │
│  │ - Used for layer creation and commit checking                        │   │
│  └─────────────────────────────────────────────────────────────────────┘   │
│                                                                             │
│  hardCursorCommitResultMap_:                                               │
│  ┌─────────────────────────────────────────────────────────────────────┐   │
│  │ Map: screenNodeId → lastFrameCommitResult (bool)                    │   │
│  │ - Tracks last frame's layer creation success per screen             │   │
│  │ - Used to detect buffer status changes (GetHardCursorNeedCommit())  │   │
│  │ - Removed via RemoveCommitResult(screenNodeId)                       │   │
│  └─────────────────────────────────────────────────────────────────────┘   │
│                                                                             │
│  hardCursorNodeMap_:                                                       │
│  ┌─────────────────────────────────────────────────────────────────────┐   │
│  │ Map: nodeId → RSSurfaceRenderNode                                   │   │
│  │ - Stores all hard cursor nodes (global, not per-screen)             │   │
│  │ - Populated via SetSetHardCursorNodeInfo()                          │   │
│  │ - Used for HardCursorCreateLayerForDirect()                          │   │
│  └─────────────────────────────────────────────────────────────────────┘   │
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
│  Initialize: isPointerInvisible = true                                     │
│    │                                                                       │
│    ▼                                                                       │
│  ┌─────────────────────────────────────────────────────────────────────┐   │
│  │ Traverse all screen nodes via NodeMap.TraverseScreenNodes()       │   │
│  └─────────────────────────────────────────────────────────────────────┘   │
│    │                                                                       │
│    ▼                                                                       │
│  ┌─────────────────────────────────────────────────────────────────────┐   │
│  │ For each screen:                                                    │   │
│  │   1. Get screen property                                            │   │
│  │   2. Check if type blacklist is empty                              │   │
│  │   3. Check if screen is mirror OR virtual                           │   │
│  │   4. If both true:                                                  │   │
│  │        - isPointerInvisible = false                                  │   │
│  │        - Break (found visible screen)                               │   │
│  └─────────────────────────────────────────────────────────────────────┘   │
│    │                                                                       │
│    ▼                                                                       │
│  Return isPointerInvisible                                                │
│  - true: No screen with empty blacklist AND (mirror OR virtual)         │
│  - false: At least one screen makes pointer visible                      │
│                                                                             │
└─────────────────────────────────────────────────────────────────────────────┘
```

### Per-Screen Dirty Region Propagation

```
┌─────────────────────────────────────────────────────────────────────────────┐
│           UpdatePointerDirtyToGlobalDirty() Screen Flow                    │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                             │
│  Input: pointerWindow, curScreenNode                                       │
│    │                                                                       │
│    ▼                                                                       │
│  ┌─────────────────────────────────────────────────────────────────────┐   │
│  │ Get dirty manager from pointer window                                │   │
│  └─────────────────────────────────────────────────────────────────────┘   │
│    │                                                                       │
│    ▼                                                                       │
│  ┌─────────────────────────────────────────────────────────────────────┐   │
│  │ If hard cursor status changed (current != last):                    │   │
│  │   - Get last frame surface position from screen node                │   │
│  │   - Merge position to screen dirty manager                          │   │
│  └─────────────────────────────────────────────────────────────────────┘   │
│    │                                                                       │
│    ▼                                                                       │
│  ┌─────────────────────────────────────────────────────────────────────┐   │
│  │ Get pointer window dirty region                                      │   │
│  │ If dirty region is not empty:                                       │   │
│  │   - Merge to screen HWC dirty region                                │   │
│  │   - Clear pointer dirty region                                      │   │
│  │   - Set isNeedForceCommitByPointer_ = true                          │   │
│  │ Else:                                                                │   │
│  │   - Set isNeedForceCommitByPointer_ = false                         │   │
│  └─────────────────────────────────────────────────────────────────────┘   │
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

**Implementation:**
```cpp
bool GetHardCursorNeedCommit(NodeId screenNodeId)
{
    auto hardCursorDrawable = GetHardCursorDrawable(screenNodeId);
    bool hasBuffer = false;
    if (hardCursorDrawable != nullptr) {
        auto surfaceParams = static_cast<RSSurfaceRenderParams*>(hardCursorDrawable->GetRenderParams().get());
        hasBuffer = surfaceParams->GetBuffer() != nullptr;
    }
    auto iter = hardCursorCommitResultMap_.find(screenNodeId);
    bool lastCommitResult = iter != hardCursorCommitResultMap_.end() && iter->second;
    return hasBuffer != lastCommitResult;
}
```

**Logic:**
- `hasBuffer`: Current frame has buffer (true/false)
- `lastCommitResult`: Previous frame layer creation success (true/false)
- **Skip frame if:** `hasBuffer == lastCommitResult` (no change)
- **Force commit if:** `hasBuffer != lastCommitResult` (status changed)

**Thread-safe skip flag:**
```cpp
bool IsPointerCanSkipFrameCompareChange(bool flag, bool changeFlag)
{
    bool expectChanged = flag;
    return isPointerCanSkipFrame_.compare_exchange_weak(expectChanged, changeFlag);
}
```

### Dirty Region Optimization

**Rule:** Only propagate dirty regions when hard cursor actually moves or changes

**Implementation:**
```cpp
void UpdatePointerDirtyToGlobalDirty(std::shared_ptr<RSSurfaceRenderNode>& pointWindow,
    std::shared_ptr<RSScreenRenderNode>& curScreenNode)
{
    if (pointWindow == nullptr || curScreenNode == nullptr) {
        return;
    }
    auto dirtyManager = pointWindow->GetDirtyManager();
    if (dirtyManager && pointWindow->GetHardCursorStatus()) {
        if (!pointWindow->GetHardCursorLastStatus()) {
            RectI lastFrameSurfacePos = curScreenNode->GetLastFrameSurfacePos(pointWindow->GetId());
            curScreenNode->GetDirtyManager()->MergeDirtyRect(lastFrameSurfacePos);
        }
        auto pointerWindowDirtyRegion = dirtyManager->GetCurrentFrameDirtyRegion();
        if (!pointerWindowDirtyRegion.IsEmpty()) {
            curScreenNode->GetDirtyManager()->MergeHwcDirtyRect(
                pointerWindowDirtyRegion, pointWindow->GetSurfaceNodeType());
            dirtyManager->SetCurrentFrameDirtyRect(RectI());
            isNeedForceCommitByPointer_ = true;
        } else {
            isNeedForceCommitByPointer_ = false;
        }
    }
}
```

**Optimization points:**
1. **Status change detection:** Merge last frame position only when cursor status changes (visible ↔ invisible)
2. **Empty dirty check:** Only propagate non-empty dirty regions
3. **Clear after merge:** Reset pointer dirty region after merging to screen
4. **Force commit flag:** Set `isNeedForceCommitByPointer_` only when dirty region exists

### Multi-Screen Visibility Optimization

**Rule:** Early exit when pointer visible on any screen

**Implementation:**
```cpp
bool IsPointerInvisibleInMultiScreen() const
{
    bool isPointerInvisible = true;
    auto& nodeMap = RSMainThread::Instance()->GetContext().GetNodeMap();
    nodeMap.TraverseScreenNodes(
        [&isPointerInvisible](const std::shared_ptr<RSScreenRenderNode>& node) {
        if (!node) {
            return;
        }
        const auto& screenProperty = node->GetScreenProperty();
        if (screenProperty.GetTypeBlackList().empty() && (node->IsMirrorScreen() || screenProperty.IsVirtual())) {
            isPointerInvisible = false;
            return;
        }
    });
    return isPointerInvisible;
}
```

**Optimization points:**
1. **Early exit:** Return false immediately when any screen makes pointer visible
2. **Simple condition:** Empty blacklist AND (mirror OR virtual) = visible
3. **Default invisible:** Assume invisible until proven otherwise

### Bound Update Optimization

**Rule:** Only update pointer bounds when bound flag is set

**Implementation:**
```cpp
void UpdatePointerInfo()
{
    int64_t rsNodeId = 0;
    BoundParam boundTemp = {0.0f, 0.0f, 0.0f, 0.0f};
    {
        std::lock_guard<std::mutex> lock(mtx_);
        if (!BoundHasUpdateCompareChange(true, false)) {
            return;
        }
        rsNodeId = GetRsNodeId();
        boundTemp = GetBound();
    }

    if (rsNodeId <= 0) {
        return;
    }

    auto node = RSMainThread::Instance()->GetContext().GetNodeMap().GetRenderNode(rsNodeId);
    if (node == nullptr) {
        return;
    }
    auto& properties = node->GetMutableRenderProperties();
    properties.SetBounds({boundTemp.x, boundTemp.y, boundTemp.z, boundTemp.w});
    node->SetDirty();
    properties.OnApplyModifiers();
    node->OnApplyModifiers();
    node->AddToPendingSyncList();

    auto surfaceNode = node->ReinterpretCastTo<RSSurfaceRenderNode>();
    if (surfaceNode == nullptr) {
        return;
    }
    RSUniHwcComputeUtil::UpdateHwcNodeProperty(surfaceNode);

    if (surfaceNode->GetScreenId() != INVALID_SCREEN_ID) {
        auto node = GetScreenRenderNode(surfaceNode->GetScreenId());
        if (!node) {
            return;
        }
        auto transform = RSUniHwcComputeUtil::GetLayerTransform(*surfaceNode);
        surfaceNode->UpdateHwcNodeLayerInfo(transform, isPointerEnableHwc_);
    }
}
```

**Optimization points:**
1. **Atomic flag check:** `BoundHasUpdateCompareChange(true, false)` - only proceed if flag was true
2. **Mutex scope:** Lock only for reading bound data, unlock before updates
3. **Early returns:** Null checks at each step
4. **Minimal updates:** Only set dirty and apply modifiers when bounds changed

### Layer Creation Optimization

**Rule:** Skip layer creation when buffer already consumed

**Implementation:**
```cpp
void HardCursorCreateLayerForDirect(std::shared_ptr<RSProcessor> processor)
{
    auto& hardCursorNodeMap = GetHardCursorNode();
    for (auto& [_, hardCursorNode] :  hardCursorNodeMap) {
        if (hardCursorNode && hardCursorNode->IsHardwareEnabledTopSurface()) {
            auto surfaceHandler = hardCursorNode->GetRSSurfaceHandler();
            if (!surfaceHandler) {
                continue;
            }
            auto params = static_cast<RSSurfaceRenderParams *>(hardCursorNode->GetStagingRenderParams().get());
            if (!params) {
                continue;
            }
            if (!surfaceHandler->IsCurrentFrameBufferConsumed() && params->GetPreBuffer() != nullptr) {
                params->SetPreBuffer(nullptr, nullptr);
                hardCursorNode->AddToPendingSyncList();
            }
            processor->CreateLayer(*hardCursorNode, *params);
        }
    }
}
```

**Optimization points:**
1. **Buffer consumed check:** Only update pre-buffer if not consumed
2. **Pre-buffer cleanup:** Set to nullptr to avoid stale buffer references
3. **Continue on null:** Skip nodes without handler or params

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
- `SetHwcNodeBounds()` - Called from input thread, protected by mutex
- `UpdatePointerInfo()` - Updates node properties with mutex protection
