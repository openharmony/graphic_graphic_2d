# Dirty Manager Feature Specification

## Overview

The `RSDirtyRegionManager` provides dirty region tracking and management for partial rendering optimization in the OpenHarmony graphics rendering pipeline. It tracks changed regions across frames, merges historical dirty regions, and supports advanced dirty region types for efficient GPU composition.

## Dirty Region Flowchart


### Dirty Region Processing Sequence

| Step | Method | Stage | Thread | Purpose |
|------|--------|--------|--------|---------|
| 1 | `SetCurrentFrameDirtyRect()` | Frame Start | Main | Reset current frame dirty region |
| 2 | `MergeDirtyRect()` | Prepare | Main | Add node dirty region to current frame |
| 3 | `MergeDirtyRect()` | Process | Main | Merge child dirty regions |
| 4 | `MergeHwcDirtyRect()` | Process | Main | Add HWC dirty region for virtual screens |
| 5 | `UpdateDirty()` | Frame End | Main | Push to history and merge based on bufferAge |
| 6 | `OnSync()` | Sync | Main→RT | Copy dirty manager to drawable syncDirtyManager_ |
| 7 | `Clear()` | Post-Sync | Main | Clear main thread dirty manager |
| 8 | `GetCurrentFrameDirtyRegion()` | OnDraw | Render | Get current frame dirty for rendering |
| 9 | `GetDirtyRegion()` | OnDraw | Render | Get merged historical dirty for damage |
| 10 | `GetAdvancedDirtyRegion()` | Damage | Render | Get multi-rect dirty for partial render |

## Dirty Region Types

| Type | Enum Value | Description | Usage |
|------|-------------|-------------|--------|
| **UPDATE_DIRTY_REGION** | 0 | Standard dirty region update | Default node changes |
| **OVERLAY_RECT** | 1 | Overlay layer dirty region | Overlay composition |
| **FILTER_RECT** | 2 | Filter effect dirty region | Blur/backdrop filters |
| **SHADOW_RECT** | 3 | Shadow dirty region | Shadow rendering |
| **PREPARE_CLIP_RECT** | 4 | Clip region during prepare | Clipping operations |
| **REMOVE_CHILD_RECT** | 5 | Child removal dirty region | Node tree changes |
| **RENDER_PROPERTIES_RECT** | 6 | Property change dirty region | Transform/opacity changes |
| **CANVAS_NODE_SKIP_RECT** | 7 | Canvas node skip region | Skipped canvas nodes |
| **OUTLINE_RECT** | 8 | Outline dirty region | Debug outline rendering |
| **SUBTREE_SKIP_RECT** | 9 | Subtree skip region | Skipped subtree rendering |
| **SUBTREE_SKIP_OUT_OF_PARENT_RECT** | 10 | Out-of-parent skip region | Culling optimization |

## Per-Screen Dirty Management

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                    Per-Screen Dirty Region Storage                          │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                             │
│  Display Dirty Manager (per screen):                                        │
│  ┌─────────────────────────────────────────────────────────────────────┐    │
│  │ RSScreenRenderNode::dirtyManager_                                   │    │
│  │ - Manages screen-level dirty regions                                │    │
│  │ - Merges surface dirty regions                                      │    │
│  │ - Handles virtual screen dirty regions                              │    │
│  └─────────────────────────────────────────────────────────────────────┘    │
│                                                                             │
│  Surface Dirty Manager (per surface):                                       │
│  ┌─────────────────────────────────────────────────────────────────────┐    │
│  │ RSSurfaceRenderNode::dirtyManager_                                  │    │
│  │ - Manages surface-level dirty regions                               │    │
│  │ - Tracks canvas node dirty regions                                  │    │
│  │ - Handles buffer age for partial rendering                          │    │
│  └─────────────────────────────────────────────────────────────────────┘    │
│                                                                             │
│  Cache Surface Dirty Manager (for caching):                                 │
│  ┌─────────────────────────────────────────────────────────────────────┐    │
│  │ RSSurfaceRenderNode::cacheSurfaceDirtyManager_                      │    │
│  │ - Tracks dirty regions for cached surfaces                          │    │
│  │ - Used for cache hit/miss detection                                 │    │
│  └─────────────────────────────────────────────────────────────────────┘    │
│                                                                             │
└─────────────────────────────────────────────────────────────────────────────┘

```

## Key Methods by Function

### Dirty Region Merging

| Method | Purpose | Thread |
|--------|---------|--------|
| `MergeDirtyRect(rect)` | Add rect to current frame dirty region | Main |
| `MergeDirtyRectIfIntersect(rect)` | Add rect only if intersects current | Main |
| `MergeDirtyRectAfterMergeHistory(rect)` | Add rect after history merge | Main |
| `MergeHwcDirtyRect(rect, nodeType)` | Add HWC dirty region (virtual screens) | Main |
| `IntersectDirtyRect(rect)` | Clip dirty region to rect | Main |

### Dirty Region Query

| Method | Purpose | Return Value |
|--------|---------|--------------|
| `GetCurrentFrameDirtyRegion()` | Get current frame dirty region | RectI |
| `GetDirtyRegion()` | Get merged historical dirty region | RectI |
| `GetDirtyRegionFlipWithinSurface()` | Get flipped dirty region (OpenGL) | RectI |
| `GetLatestDirtyRegion()` | Get latest dirty region from history | RectI |
| `IsCurrentFrameDirty()` | Check if current frame has dirty region | bool |
| `IsDirty()` | Check if merged region is dirty | bool |

### History Management

| Method | Purpose | Buffer Age |
|--------|---------|-------------|
| `UpdateDirty()` | Push current to history and merge | Uses bufferAge_ |
| `UpdateDirtyByAligned(alignedBits)` | Align and update dirty | Uses bufferAge_ |
| `SetBufferAge(age)` | Set buffer age for history merge | Input parameter |
| `GetUiLatestHistoryDirtyRegions(index)` | Get UI first dirty region | Uses history index |

### Surface Management

| Method | Purpose | Notes |
|--------|---------|-------|
| `SetSurfaceRect(rect)` | Set surface bounds | Updates surfaceRect_ |
| `SetSurfaceSize(width, height)` | Set surface dimensions | Calls SetSurfaceRect |
| `SetActiveSurfaceRect(rect)` | Set active surface rect | Tracks changes |
| `ClipDirtyRectWithinSurface()` | Clip dirty to surface bounds | Surface-relative coordinates |
| `ResetDirtyAsSurfaceSize()` | Reset dirty to full surface | Force full refresh |

### Advanced Dirty Region

| Method | Purpose | Advanced Type |
|--------|---------|----------------|
| `GetCurrentFrameAdvancedDirtyRegion()` | Get current advanced dirty regions | Vector<RectI> |
| `GetAdvancedDirtyRegion()` | Get merged advanced dirty regions | Vector<RectI> |
| `SetAdvancedDirtyRegionType(type)` | Set advanced dirty region type | DISABLED/ENABLED |
| `UpdateCurrentFrameAdvancedDirtyRegion(rect)` | Update current advanced region | Per-type tracking |

## Buffer Age and History

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                    Buffer Age and History Mechanism                         │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                             │
│  History Queue (max 10 entries):                                            │
│  ┌─────────────────────────────────────────────────────────────────────┐    │
│  │ dirtyHistory_: vector<RectI>                                        │    │
│  │ - Circular buffer with head pointer                                 │    │
│  │ - Stores past frame dirty regions                                   │    │
│  │ - Used for merging based on bufferAge                               │    │
│  └─────────────────────────────────────────────────────────────────────┘    │
│                                                                             │
│  Buffer Age Interpretation:                                                 │
│  ┌─────────────────────────────────────────────────────────────────────┐    │
│  │ bufferAge = 0: No history, use current frame only                   │    │
│  │ bufferAge = 1: Merge current + last frame                           │    │
│  │ bufferAge = 2: Merge current + last 2 frames                        │    │
│  │ bufferAge > 2: Merge current + last N frames                        │    │
│  │ bufferAge = -1: Full refresh (merge all history)                    │    │
│  └─────────────────────────────────────────────────────────────────────┘    │
│                                                                             │
│  Merge Logic:                                                               │
│  ┌─────────────────────────────────────────────────────────────────────┐    │
│  │ dirtyRegion_ = currentFrameDirtyRegion_                             │    │
│  │ for i = 0 to bufferAge-1:                                           │    │
│  │ dirtyRegion_ = Union(dirtyRegion_, dirtyHistory_[i])                │    │
│  └─────────────────────────────────────────────────────────────────────┘    │
│                                                                             │
└─────────────────────────────────────────────────────────────────────────────┘

```

## Dirty Region Alignment

| Method | Purpose | Alignment |
|--------|---------|------------|
| `GetPixelAlignedRect(rect, alignedBits)` | Align rect to pixel grid | Default 32 bits |
| `UpdateDirtyByAligned(alignedBits)` | Align current dirty before merge | Default 32 bits |
| `GetDirtyRegionFlipWithinSurface()` | Flip Y-axis for OpenGL | Left-bottom origin |

## Thread Safety

### Thread Access Pattern

```
┌─────────────────────────────────────────────────────────────────────────────┐
│ Thread Safety and Access Patterns                                           │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                             │
│  Main Thread Only (No Protection):                                          │
│  ┌─────────────────────────────────────────────────────────────────────┐    │
│  │ dirtyManager_ (RSScreenRenderNode/RSSurfaceRenderNode)              │    │
│  │   - MergeDirtyRect()                                                │    │
│  │   - MergeHwcDirtyRect()                                             │    │
│  │   - UpdateDirty()                                                   │    │
│  │   - Clear()                                                         │    │
│  │   - All prepare/process operations                                  │    │
│  └─────────────────────────────────────────────────────────────────────┘    │
│                                                                             │
│  Cross-Thread Synchronization:                                              │
│  ┌─────────────────────────────────────────────────────────────────────┐    │
│  │ OnSync() (Main Thread → Render Thread)                              │    │
│  │   - Atomic flag isSync_ for synchronization state                   │    │
│  │   - Full copy of dirty region data                                  │    │
│  │   - No concurrent access during sync                                │    │
│  └─────────────────────────────────────────────────────────────────────┘    │
│                                                                             │
│  Render Thread Only (After OnSync):                                         │
│  ┌─────────────────────────────────────────────────────────────────────┐    │
│  │ syncDirtyManager_ (Drawable)                                        │    │
│  │   - GetCurrentFrameDirtyRegion()                                    │    │
│  │   - GetDirtyRegion()                                                │    │
│  │   - GetAdvancedDirtyRegion()                                        │    │
│  │   - GetHwcDirtyRegion()                                             │    │
│  │   - All OnDraw/render operations                                    │    │
│  └─────────────────────────────────────────────────────────────────────┘    │
│                                                                             │
└─────────────────────────────────────────────────────────────────────────────┘

```

### Member Protection Summary

| Member | Protection | Thread Access | Notes |
|--------|------------|---------------|-------|
| `isSync_` | std::atomic<bool> | Both | Synchronization state flag |
| `dirtyHistory_` | No protection | Main only | Main thread only access |
| `currentFrameDirtyRegion_` | No protection | Main only | Cleared after sync |
| `dirtyRegion_` | No protection | Both (via sync) | Copied to RT via OnSync |
| `advancedDirtyRegion_` | No protection | Both (via sync) | Copied to RT via OnSync |
| `hwcDirtyRegion_` | No protection | Both (via sync) | Copied to RT via OnSync |
| `syncDirtyManager_` | No protection | RT only | Drawable-owned, immutable after sync |

### Synchronization Guarantee

1. **Main Thread**: Has exclusive access before OnSync()
2. **OnSync()**: Atomic synchronization, full data copy
3. **Render Thread**: Has exclusive access after OnSync()
4. **No Concurrent Access**: Main thread clears after sync, avoiding race conditions

## Virtual Screen Support

| Method | Purpose | Virtual Screen |
|--------|---------|----------------|
| `MergeDirtyHistoryInVirtual(age)` | Merge history for virtual screen | Uses bufferAge |
| `GetDirtyRegionInVirtual()` | Get merged dirty region | Returns mergedDirtyInVirtualScreen_ |
| `AccumulateVirtualExpandScreenDirtyRegions(rect)` | Accumulate expand screen dirty | Vector accumulation |
| `GetVirtualExpandScreenAccumulatedDirtyRegions()` | Get accumulated dirty regions | Returns vector |
| `ClearVirtualExpandScreenAccumulatedDirtyRegions()` | Clear accumulated regions | Reset vector |

## Debug Support

| Method | Purpose | Debug Type |
|--------|---------|-------------|
| `UpdateDirtyRegionInfoForDfx(id, nodeType, dirtyType, rect)` | Record dirty info for DFx | Per-node tracking |
| `GetDirtyRegionInfo(target, nodeType, dirtyType)` | Get dirty info for DFx | Returns map<NodeId, RectI> |
| `UpdateDebugRegionTypeEnable(dirtyDebugType)` | Enable debug region type | Debug visualization |
| `IsDebugRegionTypeEnable(var)` | Check if debug type enabled | Returns bool |

## Screen Type Comparison

| Screen Type | Dirty Manager Usage | Virtual Screen | Mirror Screen | Expand Screen |
|-------------|---------------------|----------------|----------------|---------------|
| **Physical Screen** | Display dirty manager only | No | No | No |
| **Virtual Screen** | Display + surface managers | Yes | No | Yes (expand mode) |
| **Mirror Screen** | Maps source dirty regions | Yes | Yes | No |
| **Expand Screen** | Accumulates dirty regions | Yes | No | Yes |

### Virtual Screen Dirty Management

```
┌─────────────────────────────────────────────────────────────────────────────┐
│              Virtual Screen Dirty Region Flow                               │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                             │
│  Virtual Screen Start                                                       │
│    │                                                                        │
│    ▼                                                                        │
│  ┌─────────────────────────────────────────────────────────────────────┐    │
│  │ Merge Dirty History in Virtual                                      │    │
│  │ - MergeDirtyHistoryInVirtual(bufferAge)                             │    │
│  │ - For each app window surface:                                      │    │
│  │ - Merge surface dirty history                                       │    │
│  │ - Merge display dirty history                                       │    │
│  └─────────────────────────────────────────────────────────────────────┘    │
│    │                                                                        │
│    ▼                                                                        │
│  ┌─────────────────────────────────────────────────────────────────────┐    │
│  │ Map Dirty Regions to Virtual Screen                                 │    │
│  │ - Get dirty region from source screen                               │    │
│  │ - Apply canvas matrix transformation                                │    │
│  │ - Map to virtual screen coordinate space                            │    │
│  └─────────────────────────────────────────────────────────────────────┘    │
│    │                                                                        │
│    ▼                                                                        │
│  ┌─────────────────────────────────────────────────────────────────────┐    │
│  │ Handle HWC Dirty Regions                                            │    │
│  │   - Get HWC dirty region from source                                │    │
│  │   - Get per-node-type HWC dirty regions                             │    │
│  │   - Filter by blacklist if security screen                          │    │
│  │   - Map to virtual screen coordinate space                          │    │
│  └─────────────────────────────────────────────────────────────────────┘    │
│    │                                                                        │
│    ▼                                                                        │
│  ┌─────────────────────────────────────────────────────────────────────┐    │
│  │ Expand Screen Dirty Accumulation                                    │    │
│  │   - AccumulateVirtualExpandScreenDirtyRegions(rect)                 │    │
│  │   - Store in virtualExpandScreenAccumDirtyRegions_ vector           │    │
│  │   - Used for expand screen mode                                     │    │
│  └─────────────────────────────────────────────────────────────────────┘    │
│    │                                                                        │
│    ▼                                                                        │
│  Virtual Screen End                                                         │
│                                                                             │
└─────────────────────────────────────────────────────────────────────────────┘

```

### Mirror Screen Dirty Mapping

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                Mirror Screen Dirty Region Mapping                           │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                             │
│  Mirror Screen Start                                                        │
│    │                                                                        │
│    ▼                                                                        │
│  ┌─────────────────────────────────────────────────────────────────────┐    │
│  │ Get Source Screen Dirty Regions                                     │    │
│  │   - Get mirrored screen drawable                                    │    │
│  │   - Get source screen dirty manager                                 │    │
│  │   - Call MergeDirtyHistoryInVirtual(bufferAge)                      │    │
│  └─────────────────────────────────────────────────────────────────────┘    │
│    │                                                                        │
│    ▼                                                                        │
│  ┌─────────────────────────────────────────────────────────────────────┐    │
│  │ Apply Canvas Matrix Transformation                                  │    │
│  │ - For each dirty region rect:                                       │    │
│  │ - Create temporary geometry object                                  │    │
│  │ - MapRect(rect, canvasMatrix)                                       │    │
│  │ - Transform to mirror screen coordinate space                       │    │
│  └─────────────────────────────────────────────────────────────────────┘    │
│    ▼                                                                        │
│  ┌─────────────────────────────────────────────────────────────────────┐    │
│  │ Handle Sampling Scale (if enabled)                                  │    │
│  │ - If isSamplingOn && samplingScale > 0:                             │    │
│  │ - Create scale matrix with samplingScale                            │    │
│  │ - Apply sampling translate offset                                   │    │
│  │ - Map rect with scale matrix                                        │    │
│  │ - MakeOutset with samplingDistance                                  │    │
│  └─────────────────────────────────────────────────────────────────────┘    │
│    │                                                                        │
│    ▼                                                                        │
│  ┌─────────────────────────────────────────────────────────────────────┐    │
│  │ Merge to Mirror Display Dirty Manager                               │    │
│  │ - Merge mapped dirty regions to display manager                     │    │
│  │ - Update display dirty manager with buffer age                      │    │
│  └─────────────────────────────────────────────────────────────────────┘    │
│    ▼                                                                        │
│  Mirror Screen End                                                          │
│                                                                             │
└─────────────────────────────────────────────────────────────────────────────┘

```

### Per-Screen Dirty Manager Comparison

| Aspect | Physical Screen | Virtual Screen | Mirror Screen |
|--------|-----------------|-----------------|----------------|
| **Display Manager** | Manages screen dirty | Maps from source | Maps from source |
| **Surface Managers** | Per-app window dirty | Per-app window dirty | None (uses source) |
| **Buffer Age** | From surface handler | From source screen | From source screen |
| **Matrix Transform** | None (identity) | Canvas matrix applied | Canvas matrix + sampling |
| **HWC Dirty Regions** | Direct use | Mapped from source | Mapped from source |
| **Expand Mode** | Not applicable | Accumulates dirty | Not applicable |

## Current Node Matrix Calculation

```
┌─────────────────────────────────────────────────────────────────────────────┐
│              Current Node Matrix Calculation Flow                           │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                             │
│  Node Draw Start                                                            │
│    │                                                                        │
│    ▼                                                                        │
│  ┌─────────────────────────────────────────────────────────────────────┐    │
│  │ Get Canvas Total Matrix                                             │    │
│  │ - canvas->GetTotalMatrix()                                          │    │
│  │ - Returns accumulated transform from root to current node           │    │
│  └─────────────────────────────────────────────────────────────────────┘    │
│    │                                                                        │
│    ▼                                                                        │
│  ┌─────────────────────────────────────────────────────────────────────┐    │
│  │ Calculate Visible Rect (if needed)                                  │    │
│  │ - CalVisibleRect(totalMatrix, clipRect, boundsRect)                 │    │
│  │ - Maps bounds rect to screen coordinate space                       │    │
│  │ - Applies clip rect intersection                                    │    │
│  └─────────────────────────────────────────────────────────────────────┘    │
│    │                                                                        │
│    ▼                                                                        │
│  Node Draw End                                                              │
│                                                                             │
└─────────────────────────────────────────────────────────────────────────────┘

```

### Parent-to-Child Matrix Transition Properties

| Property | Effect on Child Matrix | Dirty Region Impact |
|----------|------------------------|---------------------|
| **Transform (Translate/Scale/Rotate)** | Multiplies child matrix | Updates child bounds, triggers dirty |
| **Alpha (Opacity)** | No matrix change | Triggers dirty if alpha changes |
| **Clip Rect** | No matrix change | Clips child dirty region to clip bounds |
| **Bounds (Size/Position)** | Updates child matrix | Triggers dirty if bounds change |
| **Animation (Transform/Alpha)** | Animates matrix/alpha | Triggers dirty per frame during animation |
| **Visibility (Hidden/Shown)** | No matrix change | Triggers dirty on visibility change |
| **Surface Scale** | Multiplies surface matrix | Affects all children on surface |
| **CornerRadius** | No matrix change | Triggers dirty if radius changes |

### Matrix Composition Order

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                 Matrix Composition Hierarchy                                │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                             │
│  Root Matrix (Screen/Display)                                               │
│    │                                                                        │
│    ▼                                                                        │
│  ┌─────────────────────────────────────────────────────────────────────┐    │
│  │ Surface Matrix (Window/Surface)                                     │    │
│  │   - Surface bounds and position                                     │    │
│  │   - Surface scale and rotation                                      │    │
│  └─────────────────────────────────────────────────────────────────────┘    │
│    │                                                                        │
│    ▼                                                                        │
│  ┌─────────────────────────────────────────────────────────────────────┐    │
│  │ Canvas Node Matrix (Canvas/Effect)                                  │    │
│  │   - Canvas transform (translate/scale/rotate)                       │    │
│  │   - Canvas clip rect                                                │    │
│  └─────────────────────────────────────────────────────────────────────┘    │
│    │                                                                        │
│    ▼                                                                        │
│  ┌─────────────────────────────────────────────────────────────────────┐    │
│  │ Child Node Matrix (Nested Canvas/Effect)                            │    │
│  │   - Child transform (multiplied with parent)                        │    │
│  │   - Child clip rect (intersected with parent)                       │    │
│  └─────────────────────────────────────────────────────────────────────┘    │
│    │                                                                        │
│    ▼                                                                        │
│ Total Matrix = Root × Surface × Canvas × Child                              │
│                                                                             │
└─────────────────────────────────────────────────────────────────────────────┘

```

### Property Change Impact on Dirty Regions

| Property Change Type | Matrix Effect | Dirty Region Behavior |
|----------------------|---------------|----------------------|
| **Transform Change** | Matrix updated | Dirty region follows new bounds |
| **Alpha (Opacity) Change** | No matrix change | Dirty region remains static (full repaint) |
| **Bounds (Size/Position) Change** | Matrix updated | Dirty region follows new size/position |
| **Clip Bounds Change** | No matrix change | Dirty region intersected with clip rect |
| **Combined Transform+Alpha Change** | Matrix + alpha change | Dirty region follows bounds + full repaint |

**Key Concepts:**

- **Transform/Bounds Change**: Updates node's transformation matrix → recalculates absolute draw rect → dirty region follows new geometry
- **Alpha Change**: Only opacity changes, geometry unchanged → full repaint needed (old + new region)
- **Clip Bounds Change**: Clipping region updated, no matrix change → dirty region intersected with new clip bounds (rs_render_node.cpp)
- **prepareClipRect_ Propagation**: Clip rect passed down tree and intersected with each node's dirty region (rs_uni_render_visitor.cpp)

## Main Thread Integration

### Prepare/Process Stage
- `MergeDirtyRect()` - Add node dirty regions during traversal
- `MergeHwcDirtyRect()` - Add HWC dirty regions for virtual screens
- `UpdateDirty()` - Push to history at frame end
- `SetBufferAge()` - Set buffer age from surface handler

### OnSync Mechanism

```
┌─────────────────────────────────────────────────────────────────────────────┐
│ Main Thread to Render Thread Dirty Manager Synchronization                  │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                             │
│  Main Thread (Node):                                                        │
│  ┌─────────────────────────────────────────────────────────────────────┐    │
│  │ RSScreenRenderNode::dirtyManager_                                   │    │
│  │ RSSurfaceRenderNode::dirtyManager_                                  │    │
│  │ - Manages dirty regions during prepare/process                      │    │
│  │ - Accumulates dirty rects from child nodes                          │    │
│  │ - Updates history at frame end                                      │    │
│  └─────────────────────────────────────────────────────────────────────┘    │
│    │                                                                        │
│    │ OnSync() called during synchronization phase                           │
│    ▼                                                                        │
│  ┌─────────────────────────────────────────────────────────────────────┐    │
│  │ dirtyManager_->OnSync(syncDirtyManager)                             │    │
│  │                                                                     │    │
│  │ Copied Data:                                                        │    │
│  │   - dirtyRegion_ (merged historical)                                │    │
│  │   - currentFrameDirtyRegion_                                        │    │
│  │   - currentFrameAdvancedDirtyRegion_                                │    │
│  │   - hwcDirtyRegion_                                                 │    │
│  │   - typeHwcDirtyRegion_                                             │    │
│  │   - advancedDirtyRegion_                                            │    │
│  │   - surfaceRect_, activeSurfaceRect_                                │    │
│  │   - filterCollector_                                                │    │
│  │   - debugRect_                                                      │    │
│  └─────────────────────────────────────────────────────────────────────┘    │
│    │                                                                        │
│    │ Main thread dirty manager cleared after sync                           │
│    ▼                                                                        │
│  Render Thread (Drawable):                                                  │
│  ┌─────────────────────────────────────────────────────────────────────┐    │
│  │ RSScreenRenderNodeDrawable::syncDirtyManager_                       │    │
│  │ RSSurfaceRenderNodeDrawable::syncDirtyManager_                      │    │
│  │ - Created in drawable constructor                                   │    │
│  │ - Receives synced dirty regions from main thread                    │    │
│  │ - Used during OnDraw for partial rendering                          │    │
│  └─────────────────────────────────────────────────────────────────────┘    │
│                                                                             │
└─────────────────────────────────────────────────────────────────────────────┘

```

### Main Thread OnSync Implementation

**RSScreenRenderNode::OnSync() (rs_screen_render_node.cpp):**
- Gets screen params from staging render params
- Retrieves syncDirtyManager from render drawable
- Calls dirtyManager_->OnSync(syncDirtyManager) to copy to render thread
- Calls RSRenderNode::OnSync() for base class sync

**RSSurfaceRenderNode::OnSync() (rs_surface_render_node.cpp):**
- Merges skip frame dirty rect if present
- Updates UIFirst dirty rect with skip frame dirty
- Clears skip frame dirty rect after merge
- Retrieves syncDirtyManager from render drawable
- Calls dirtyManager_->OnSync(syncDirtyManager) to copy to render thread
- Calls RSRenderNode::OnSync() for base class sync

## Render Thread Drawable Integration

### Drawable syncDirtyManager_ Usage

| Drawable Type | syncDirtyManager_ | Usage |
|---------------|-------------------|-------|
| **RSScreenRenderNodeDrawable** | Per-screen dirty manager | Global dirty region, HWC dirty |
| **RSSurfaceRenderNodeDrawable** | Per-surface dirty manager | Surface dirty, partial render |
| **RSLogicalDisplayRenderNodeDrawable** | Inherited from screen | Virtual/mirror screen dirty |

### Render Thread Dirty Region Access

**OnDraw Usage (rs_surface_render_node_drawable.cpp):**
- Gets current frame dirty region from syncDirtyManager_
- Performs occlusion check using visible dirty region
- Skips rendering if surface is occluded and op dropped
- Gets merged history dirty for damage region calculation

**GetSyncDirtyManager() Implementation:**
- Returns syncDirtyManager_ member variable
- Provides render thread access to synced dirty data

### Render Thread Query Methods

| Method | Drawable Usage | Purpose |
|--------|----------------|---------|
| `GetCurrentFrameDirtyRegion()` | OnDraw occlusion check | Current frame dirty bounds |
| `GetDirtyRegion()` | Damage region calculation | Merged historical dirty |
| `GetDirtyRegionForQuickReject()` | Quick reject optimization | Multiple dirty rects for culling |
| `GetAdvancedDirtyRegion()` | Multi-rect damage region | Advanced partial render |
| `GetHwcDirtyRegion()` | HWC composition | Hardware composer dirty |
| `GetRectFlipWithinSurface()` | OpenGL coordinate flip | Y-axis flip for damage |

### Quick Reject Optimization

**Quick Reject for Culling (rs_surface_render_node_drawable.cpp):**
- Iterates through dirtyRegionForQuickReject rects
- Uses multiple dirty rects for efficient culling
- Avoids full surface redraw when only partial regions dirty

### RSUniRenderUtil Integration

**MergeDirtyHistory (rs_uni_render_util.cpp):**
- Gets dirty manager from screen drawable via GetSyncDirtyManager()
- Calls MergeDirtyHistoryForDrawable with bufferAge and params
- Iterates through GetAdvancedDirtyRegion() for multi-rect damage
- Creates Occlusion::Region for each dirty rect
- Updates globalDirtyRegion using OrSelf operation

### Cross-Thread Data Flow

```
┌─────────────────────────────────────────────────────────────────────────────┐
│ Dirty Region Cross-Thread Flow                                              │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                             │
│  Frame N - Main Thread:                                                     │
│  ┌─────────────────────────────────────────────────────────────────────┐    │
│  │ 1. Prepare: MergeDirtyRect() for each dirty node                    │    │
│  │ 2. Process: MergeHwcDirtyRect() for virtual screens                 │    │
│  │ 3. Update: UpdateDirty() with bufferAge                             │    │
│  │ 4. Sync: OnSync() copies to render thread drawable                  │    │
│  │ 5. Clear: Main thread dirty manager cleared                         │    │
│  └─────────────────────────────────────────────────────────────────────┘    │
│                                                                             │
│  Frame N - Render Thread:                                                   │
│  ┌─────────────────────────────────────────────────────────────────────┐    │
│  │ 1. OnDraw: Get syncDirtyManager_->GetCurrentFrameDirtyRegion()      │    │
│  │ 2. Occlusion: Check visibleDirtyRegion against dirty                │    │
│  │ 3. Render: Draw only dirty regions (partial render)                 │    │
│  │ 4. Damage: SetDamageRegion() for GPU/HWC                            │    │
│  └─────────────────────────────────────────────────────────────────────┘    │
│                                                                             │
│  Frame N+1 - Skip Frame (if applicable):                                    │
│  ┌─────────────────────────────────────────────────────────────────────┐    │
│  │ OnSkipSync(): Accumulate dirty to skipFrameDirtyRect_               │    │
│  │ Next OnSync(): Merge skipFrameDirtyRect_ to current dirty           │    │
│  └─────────────────────────────────────────────────────────────────────┘    │
│                                                                             │
└─────────────────────────────────────────────────────────────────────────────┘

```

### UpdateDisplayDirtyManager

**Logical Display Dirty Manager Update (rs_logical_display_render_node_drawable.cpp):**
- For mirror/virtual screens, updates dirty manager from source screen
- Calls MergeDirtyHistoryInVirtual(bufferAge) to merge history
- Handles HWC dirty regions from mirrored drawable
- Gets type-based HWC dirty regions for filtering

## New Features

### RSFilterDirtyCollector

**Purpose**: Collect filter dirty regions for cache invalidation.

| Method | Purpose |
|--------|---------|
| `CollectFilterDirtyRegionInfo(info, isFront)` | Collect filter dirty info |
| `OnSync(targetCollector)` | Sync to render thread |
| `Clear()` | Clear collected regions |

**Usage (rs_dirty_region_manager.h):**
- Returns reference to filterCollector_ member
- Provides access to filter dirty region collection

### UIFirst Dirty Region

**Purpose**: Track dirty regions for UIFirst (UI-first rendering optimization).

| Member | Purpose |
|--------|---------|
| `uifirstFrameDirtyRegion_` | Current frame UIFirst dirty |
| `hasUifirstChild_` | Flag for UIFirst child nodes |
| `SetUifirstFrameDirtyRect()` | Set UIFirst dirty rect |
| `GetUifirstFrameDirtyRegion()` | Get UIFirst dirty for sub-thread cache |

**Skip Frame Handling (rs_surface_render_node.cpp):**
- Checks if current frame dirty region is not empty
- Joins current dirty rect with skip frame dirty rect
- Accumulates dirty across skipped frames

### Advanced Dirty Region Types

| Type | Description | Usage |
|------|-------------|-------|
| `DISABLED` | Single merged dirty rect | Default partial render |
| `SET_ADVANCED_SURFACE_AND_DISPLAY` | Multi-rect dirty | Optimized damage region |
| `SET_ADVANCED_DISPLAY` | Display-level multi-rect | Screen-level optimization |

**Type Selection (rs_uni_render_util.cpp):**
- DISABLED: damageRegion = dirtyRegion.Or(globalDirtyRegion)
- SET_ADVANCED_SURFACE_AND_DISPLAY: MergeDirtyRects on union of dirty and global
- SET_ADVANCED_DISPLAY: MergeDirtyRects on global, then OR with dirty

### Layer Part Render Status

| Method | Purpose |
|--------|---------|
| `SetHasUifirstChild(bool)` | Mark node has UIFirst children |
| `HasUifirstChild()` | Check UIFirst child status |

**Update (rs_surface_render_node.cpp):**
- Checks if last frame UIFirst flag is not NONE or sub thread assignable
- Sets hasUifirstChild_ to true if condition met

### Partial Render Enable/Disable

| Member | Purpose |
|--------|---------|
| `isPartialRenderEnabled_` | Partial render enabled flag |
| `isEnabledChanged_` | Enable state changed flag |
| `SetPartialRenderEnabled()` | Set partial render state |
| `GetEnabledChanged()` | Check if state changed |

### Cacheable Filter Rect

| Method | Purpose |
|--------|---------|
| `UpdateCacheableFilterRect(rect)` | Add cacheable filter region |
| `IfCacheableFilterRectFullyCover(targetRect)` | Check full coverage |
| `IsCacheableFilterRectEmpty()` | Check if empty |
| `InvalidateFilterCacheRect()` | Invalidate cache rect |

### Visited Dirty Rects

| Method | Purpose |
|--------|---------|
| `UpdateVisitedDirtyRects(rects)` | Update visited app dirty regions |
| `GetIntersectedVisitedDirtyRect(absRect)` | Get intersected dirty with visited |

**Purpose**: Track visited dirty regions for occlusion optimization.

## Render Params Dirty Types

### RSRenderParamsDirtyType

| Type | Description | Usage |
|------|-------------|-------|
| `NO_DIRTY` | No dirty state | Clean state |
| `MATRIX_DIRTY` | Matrix changed | Transform update |
| `LAYER_INFO_DIRTY` | Layer info changed | HWC layer update |
| `BUFFER_INFO_DIRTY` | Buffer changed | Surface buffer update |
| `DRAWING_CACHE_TYPE_DIRTY` | Cache type changed | Drawing cache update |

**Render Params Dirty Check (rs_render_params.h):**
- `IsLayerDirty()` - Tests LAYER_INFO_DIRTY bit in dirtyType_
- `IsBufferDirty()` - Tests BUFFER_INFO_DIRTY bit in dirtyType_

### Layer Part Render Dirty Region

**Purpose**: Track dirty region for layer-part rendering (UIFirst optimization).

| Member | Type | Purpose |
|--------|------|---------|
| `layerPartRenderCurrentFrameDirtyRegion_` | RectI | Current frame dirty for layer part render |
| `layerPartRenderEnabled_` | bool | Layer part render enabled flag |

**Methods:**
- `SetLayerPartRenderEnabled(bool)` - Enable/disable layer part render
- `GetLayerPartRenderEnabled()` - Check if layer part render enabled
- `SetLayerPartRenderCurrentFrameDirtyRegion(RectI)` - Set current frame dirty
- `GetLayerPartRenderCurrentFrameDirtyRegion()` - Get current frame dirty

**Usage**: Layer-part rendering splits surface into multiple layers for parallel rendering.

### DirtyRegionInfoForDFX

**Purpose**: DFX (debug/trace) dirty region tracking.

| Member | Purpose |
|--------|---------|
| `oldDirty` | Previous frame dirty region |
| `oldDirtyInSurface` | Previous frame dirty in surface coordinates |

**Methods:**
- `SetOldDirtyInSurface(RectI)` - Set old dirty in surface coordinates
- `GetOldDirtyInSurface()` - Get old dirty in surface coordinates

### RenderGroup SubTree Dirty

**Purpose**: Track render group subtree dirty state.

| Method | Purpose |
|--------|---------|
| `SetRenderGroupSubTreeDirty(bool)` | Mark subtree dirty |
| `IsRenderGroupSubTreeDirty()` | Check subtree dirty state |

**Usage**: Render group caches entire subtree, dirty flag indicates need to update cache.

### NodeDirtyType Flags

| Type | Value | Description |
|------|-------|-------------|
| `NOT_DIRTY` | 0x0000 | No dirty |
| `GEOMETRY` | 0x0001 | Geometry changed |
| `BACKGROUND` | 0x0002 | Background changed |
| `CONTENT` | 0x0004 | Content changed |
| `FOREGROUND` | 0x0008 | Foreground changed |
| `OVERLAY` | 0x0010 | Overlay changed |
| `APPEARANCE` | 0x0020 | Appearance changed |

**Purpose**: Fine-grained dirty tracking for property-level updates.
