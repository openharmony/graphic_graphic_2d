# Dirty Manager Feature Specification

## Overview

The `RSDirtyRegionManager` provides dirty region tracking and management for partial rendering optimization in the OpenHarmony graphics rendering pipeline. It tracks changed regions across frames, merges historical dirty regions, and supports advanced dirty region types for efficient GPU composition.

## Dirty Region Flowchart


### Dirty Region Processing Sequence

| Step | Method | Stage | Purpose |
|------|--------|--------|---------|
| 1 | `SetCurrentFrameDirtyRect()` | Frame Start | Reset current frame dirty region |
| 2 | `MergeDirtyRect()` | Prepare/Process | Add node dirty region to current frame |
| 3 | `MergeHwcDirtyRect()` | Process | Add HWC dirty region for virtual screens |
| 4 | `UpdateDirty()` | Frame End | Push to history and merge based on bufferAge |
| 5 | `OnSync()` | Sync | Synchronize to render thread |

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

| Member | Protection | Notes |
|--------|------------|-------|
| `isSync_` | std::atomic<bool> | Synchronization flag |
| `dirtyHistory_` | No explicit protection | Main thread only |
| `currentFrameDirtyRegion_` | No explicit protection | Main thread only |
| `dirtyRegion_` | No explicit protection | Main thread only |

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

### Animation Impact on Dirty Regions

| Animation Type | Matrix Effect | Dirty Region Behavior |
|----------------|---------------|----------------------|
| **Transform Animation** | Matrix changes per frame | Dirty region follows animated bounds |
| **Alpha Animation** | No matrix change | Dirty region remains static (full repaint) |
| **Bounds Animation** | Matrix changes per frame | Dirty region follows animated size/position |
| **Clip Animation** | No matrix change | Dirty region clipped to animated clip rect |
| **Combined Transform+Alpha** | Matrix + alpha change | Dirty region follows bounds + full repaint |

## Integration Points

### Main Thread (Prepare/Process)
- `MergeDirtyRect()` - Add node dirty regions during traversal
- `MergeHwcDirtyRect()` - Add HWC dirty regions for virtual screens
- `UpdateDirty()` - Push to history at frame end
- `SetBufferAge()` - Set buffer age from surface handler

### Render Thread
- `OnSync()` - Synchronize dirty manager to render thread
- `GetCurrentFrameDirtyRegion()` - Get current frame dirty for rendering
- `GetDirtyRegion()` - Get merged dirty for composition

### Cross-Thread
- `syncDirtyManager_` - Render thread copy of dirty manager
- `UpdateDisplayDirtyManager()` - Update display dirty manager from surface
