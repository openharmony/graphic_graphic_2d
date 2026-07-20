# Occlusion Culling Feature Specification

## Overview

The occlusion culling feature provides multi-level occlusion detection for rendering optimization in OpenHarmony graphics pipeline:

1. **Surface/App Level Occlusion**: Window-level occlusion tracking (visible region calculation)
2. **Canvas/Pixel Level Occlusion**: Intra-app occlusion using lightweight tree (OcclusionNode)
3. **Pixel-Level Stencil Occlusion**: GPU stencil buffer testing for fine-grained culling

It skips rendering of occluded nodes to reduce GPU workload and improve performance.

## Surface/App Level Occlusion Process

### Basic Occlusion Flow

```
┌─────────────────────────────────────────────────────────────────────────────┐
│ Surface/App Level Occlusion (Top-to-Bottom Traversal)                       │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                             │
│  Prepare Stage - Main Thread:                                               │
│                                                                             │
│  Top Surface (App Window 1 - Opaque)                                        │
│    │                                                                        │
│    ▼                                                                        │
│  ┌─────────────────────────────────────────────────────────────────────┐    │
│  │ CalculateOpaqueAndTransparentRegion()                               │    │
│  │ - Calculate opaque region based on bounds, corner radius            │    │
│  │ - Consider container window transparency                            │    │
│  │ - opaqueRegion_ = bounds - transparentAreas                         │    │
│  │ - ResetSurfaceOpaqueRegion() if needed                              │    │
│  └─────────────────────────────────────────────────────────────────────┘    │
│    │                                                                        │
│    ▼                                                                        │
│  ┌─────────────────────────────────────────────────────────────────────┐    │
│  │ Accumulate Opaque Region                                            │    │
│  │ - accumulatedOcclusionRegion_.OrSelf(opaqueRegion_)                 │    │
│  │ - Accumulated from top to bottom                                    │    │
│  │ - Skip layer affects occlusion calculation                          │    │
│  └─────────────────────────────────────────────────────────────────────┘    │
│                                                                             │
│  Middle Surface (App Window 2 - Semi-transparent)                           │
│    │                                                                        │
│    ▼                                                                        │
│  ┌─────────────────────────────────────────────────────────────────────┐    │
│  │ UpdateNodeVisibleRegion()                                           │    │
│  │ - selfDrawRegion = GetSurfaceOcclusionRect()                        │    │
│  │ - visibleRegion = selfDrawRegion - accumulatedOcclusionRegion       │    │
│  │ - If visibleRegion.IsEmpty(): surface is fully occluded             │    │
│  │ - SetVisibleRegion(visibleRegion)                                   │    │
│  │ - SetVisibleRegionInVirtual(...)                                    │    │
│  │ - SetVisibleRegionBehindWindow(...)                                 │    │
│  └─────────────────────────────────────────────────────────────────────┘    │
│                                                                             │
│  Bottom Surface (App Window 3 - Occluded)                                   │
│    │                                                                        │
│    ▼                                                                        │
│  ┌─────────────────────────────────────────────────────────────────────┐    │
│  │ visibleRegion = Empty (fully occluded by upper opaque windows)      │    │
│  │ - Skip subtree prepare                                              │    │
│  │ - Skip rendering in render thread                                   │    │
│  │ - Performance: No GPU workload for occluded surface                 │    │
│  └─────────────────────────────────────────────────────────────────────┘    │
│                                                                             │
└─────────────────────────────────────────────────────────────────────────────┘

```

### Occlusion Participation Rules

| Surface Type | Participate in Occlusion | Reason |
|--------------|-------------------------|--------|
| **App Main Window (Opaque)** | Yes | Primary occlusion contributor |
| **App Main Window (Transparent)** | No | Doesn't occlude below surfaces |
| **Leash Window** | Conditional | Based on focus state |
| **Self-Drawing Node** | No | System component, always visible |
| **Ability Component** | No | System component, always visible |
| **First-Level Cross Node** | No | Cross-display, special handling |
| **Focused Window** | Conditional | Focus may bypass occlusion |

**Participation Check (rs_uni_render_visitor.cpp):**
- Main window type only (not leash/window types)
- Not first-level cross node in expand screen
- Not in animation scenes (unless specific animation occlusion)
- Not rotating
- Not sub surface node
- Not transparent (unless need draw behind window)

### Opaque Region Calculation

**Factors Affecting Opaque Region:**

1. **Corner Radius**: Rounded corners create transparent areas
2. **Container Window**: Container window transparency affects opaque region
3. **Background Alpha**: Surface background opacity determines opaque area
4. **Behind Window**: Need draw behind window affects occlusion region

**Calculation (rs_surface_render_node.cpp):**
- `CheckAndUpdateOpaqueRegion()` updates opaque region based on screen rotation and container transparency
- `GetSurfaceOcclusionRect()` returns the rect used for occlusion calculation
- Corner radius reduces opaque region (rounded corners are not opaque)

## Visible Region

### Visible Region Types

| Type | Variable | Purpose | Scope |
|------|----------|---------|-------|
| **visibleRegion_** | `Occlusion::Region` | Main visible region after occlusion | Physical/Virtual screen |
| **visibleRegionInVirtual_** | `Occlusion::Region` | Visible region in virtual screen context | Virtual screen only |
| **visibleRegionBehindWindow_** | `Occlusion::Region` | Visible region considering behind-window content | Filter effects |
| **extendVisibleRegion_** | `Occlusion::Region` | Extended visible region (union with additional) | GPU optimization |

### Visible Region Calculation Formula

**Main visible region:**
- selfDrawRegion = GetSurfaceOcclusionRect bounds
- visibleRegion_ = selfDrawRegion SUB accumulatedOcclusionRegion

**Virtual screen visible region:**
- visibleRegionInVirtual_ = selfDrawRegion SUB occlusionRegionWithoutSkipLayer

**Behind window visible region:**
- visibleRegionBehindWindow_ = visibleRegion_ SUB accumulatedOcclusionRegionBehindWindow

### When Visible Region is Empty

**Conditions:**
1. Surface completely covered by opaque regions above
2. Occlusion enabled (`IsOcclusionEnabled()` returns true)
3. Not first-level cross-display node

**Impact:**
- Main thread: Skip subtree prepare traversal
- Render thread: Skip surface rendering (OnDraw returns early)
- Performance: Significant GPU workload reduction

## Visible Dirty Region

### Definition

Visible dirty region = Intersection of dirty region AND visible region

Represents the actual damaged area that needs to be rendered after considering occlusion.

### Calculation (rs_surface_render_node_drawable.cpp)

**Step 1: Get dirty region from dirty manager**
- Iterate through GetDirtyRegionForQuickReject rects
- Create Occlusion::Region for each rect
- OrSelf to merge into dirtyRegion

**Step 2: Calculate visible dirty region**
- Get visibleRegion from surfaceParams
- For cross-display nodes: visibleDirtyRegion = dirtyRegion (ignore occlusion)
- For normal nodes: visibleDirtyRegion = dirtyRegion AND visibleRegion (intersection)

**Step 3: Skip check**
- If visibleDirtyRegion.IsEmpty(): skip rendering

### Visible Dirty Region Usage

| Stage | Usage | Purpose |
|-------|-------|---------|
| **OnDraw (Render Thread)** | Occlusion skip check | Skip if empty visible dirty region |
| **Damage Region** | GPU damage calculation | Only render visible dirty areas |
| **Quick Reject** | Culling optimization | Use visible dirty for efficient culling |

### Cross-Session: Dirty Manager ↔ Visible Region

**Dirty Manager** (from dirty_manager_feature.md):
- Tracks dirty regions: current frame dirty, historical dirty
- Provides `GetDirtyRegionForQuickReject()` for occlusion culling

**Visible Region** (from occlusion culling):
- Tracks non-occluded portion of surface
- Provides `GetVisibleRegion()` for intersection calculation

**Visible Dirty Region** (intersection):
- Combines dirty tracking + occlusion detection
- Final region for rendering decisions
- Formula: `visibleDirtyRegion = dirtyRegion AND visibleRegion`

## Canvas/Pixel Level Occlusion Culling

### Occlusion Tree Structure

```
┌─────────────────────────────────────────────────────────────────────────────┐
│ Canvas/Pixel Level Occlusion Tree (OcclusionNode)                           │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                             │
│ RSOcclusionHandler (root)                                                   │
│  ┌─────────────────────────────────────────────────────────────────────┐    │
│  │ rootNodeId_: NodeId of root occlusion node                          │    │
│  │ rootOcclusionNode_: shared_ptr<OcclusionNode>                       │    │
│  │ occlusionNodes_: map<NodeId, shared_ptr<OcclusionNode>>             │    │
│  │ culledNodes_: unordered_set<NodeId> (culled this frame)             │    │
│  │ culledEntireSubtree_: unordered_set<NodeId> (entire subtree)        │    │
│  │ subTreeSkipPrepareNodes_: unordered_set<uint64_t>                   │    │
│  └─────────────────────────────────────────────────────────────────────┘    │
│                                                                             │
│ OcclusionNode (per canvas/effect node)                                      │
│  ┌─────────────────────────────────────────────────────────────────────┐    │
│  │ nodeId_: Unique node identifier                                     │    │
│  │ bounds_: Node bounding rect (allBounds_, clipRect_)                 │    │
│  │ children_: Vector of child occlusion nodes                          │    │
│  │ parent_: Weak pointer to parent node                                │    │
│  │ isOccluded_: Whether node is occluded                               │    │
│  │ isOutOfParentRect_: Whether node is outside parent bounds           │    │
│  │ isTransparent_, isOpaque_: Transparency/opaque flags                │    │
│  │ participatingInOcclusion_: Whether participates in occlusion        │    │
│  │ isSubTreeIgnored_: Whether subtree is ignored                       │    │
│  └─────────────────────────────────────────────────────────────────────┘    │
│                                                                             │
└─────────────────────────────────────────────────────────────────────────────┘

```

### Canvas/Pixel Occlusion Flow

```
┌─────────────────────────────────────────────────────────────────────────────┐
│ Canvas/Pixel Occlusion Culling Frame Flow                                   │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                             │
│  Frame Start                                                                │
│    │                                                                        │
│    ▼                                                                        │
│  ┌─────────────────────────────────────────────────────────────────────┐    │
│  │ Collect Occlusion Nodes (Prepare Stage)                             │    │
│  │ - CollectNode(node): Add node to occlusion tree                     │    │
│  │ - CollectSubTree(node, needPrepare): Collect subtree nodes          │    │
│  │ - CollectNodeProperties(): bounds, transparency, participating      │    │
│  │ - ForwardOrderInsert(): Insert in forward rendering order           │    │
│  └─────────────────────────────────────────────────────────────────────┘    │
│    │                                                                        │
│    ▼                                                                        │
│  ┌─────────────────────────────────────────────────────────────────────┐    │
│  │ Process Off-Tree Nodes                                              │    │
│  │ - ProcessOffTreeNodes(ids): Remove nodes not in render tree         │    │
│  │ - RemoveSubTree(): Remove node and all descendants                  │    │
│  └─────────────────────────────────────────────────────────────────────┘    │
│    │                                                                        │
│    ▼                                                                        │
│  ┌─────────────────────────────────────────────────────────────────────┐    │
│  │ Update Node Properties                                              │    │
│  │ - UpdateChildrenOutOfRectInfo(): Track out-of-rect children         │    │
│  │ - UpdateSkippedSubTreeProp(): Update skipped subtree properties     │    │
│  │ - CalculateNodeAllBounds(): Compute total bounds                    │    │
│  └─────────────────────────────────────────────────────────────────────┘    │
│    │                                                                        │
│    ▼                                                                        │
│  ┌─────────────────────────────────────────────────────────────────────┐    │
│  │ Calculate Frame Occlusion                                           │    │
│  │ - CalculateFrameOcclusion(): Main occlusion detection               │    │
│  │ - DetectOcclusion(): Recursively detect occluded nodes              │    │
│  │ - Mark culled nodes in culledNodes_ set                             │    │
│  │ - Mark entire subtree in culledEntireSubtree_ set                   │    │
│  └─────────────────────────────────────────────────────────────────────┘    │
│    │                                                                        │
│    ▼                                                                        │
│  ┌─────────────────────────────────────────────────────────────────────┐    │
│  │ Process Stage: Use Culled Nodes                                     │    │
│  │ - TakeCulledNodes(): Get set of culled nodes                        │    │
│  │ - TakeCulledEntireSubtree(): Get set of culled subtrees             │    │
│  │ - Skip rendering for culled nodes                                   │    │
│  └─────────────────────────────────────────────────────────────────────┘    │
│    │                                                                        │
│    ▼                                                                        │
│  Frame End                                                                  │
│                                                                             │
└─────────────────────────────────────────────────────────────────────────────┘

```

### Occlusion Detection Algorithm

**DetectOcclusion (rs_occlusion_node.cpp):**
- Traverse occlusion tree in forward rendering order
- Accumulate occlusion region from opaque nodes
- For each node:
  - Calculate visible region: `selfBounds - accumulatedOcclusion`
  - If visible region empty → mark as culled
  - If opaque and participating → add to accumulated occlusion
  - Recursively process children

## Pixel-Level Stencil Occlusion

### Stencil Test Flow

```
┌─────────────────────────────────────────────────────────────────────────────┐
│ Stencil Test Rendering Flow                                                 │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                             │
│  Node Draw Start                                                            │
│    │                                                                        │
│    ▼                                                                        │
│  ┌─────────────────────────────────────────────────────────────────────┐    │
│  │ Check Occlusion Culling Enabled                                     │    │
│  │ - IsOcclusionCullingEnabled()                                       │    │
│  │ - IsStencilPixelOcclusionCullingEnabled()                           │    │
│  └─────────────────────────────────────────────────────────────────────┘    │
│    │                                                                        │
│    ▼                                                                        │
│  ┌─────────────────────────────────────────────────────────────────────┐    │
│  │ Quick Reject Check                                                  │    │
│  │ - QuickReject(canvas, localDrawRect)                                │    │
│  │ - Returns true if node is occluded                                  │    │
│  └─────────────────────────────────────────────────────────────────────┘    │
│    │                                                                        │
│    ▼                                                                        │
│  ┌─────────────────────────────────────────────────────────────────────┐    │
│  │ If Quick Reject = true: Skip Rendering                              │    │
│  │ - Don't draw occluded node                                          │    │
│  │ - Save GPU time                                                     │    │
│  └─────────────────────────────────────────────────────────────────────┘    │
│    │                                                                        │
│    ▼                                                                        │
│  ┌─────────────────────────────────────────────────────────────────────┐    │
│  │ If Quick Reject = false: Draw with Stencil Test                     │    │
│  │ - Set stencil func: EQUAL, ref=stencilVal                           │    │
│  │ - Draw node                                                         │    │
│  │ - Only draw where stencil buffer matches                            │    │
│  └─────────────────────────────────────────────────────────────────────┘    │
│    │                                                                        │
│    ▼                                                                        │
│  Node Draw End                                                              │
│                                                                             │
└─────────────────────────────────────────────────────────────────────────────┘

```

### Stencil Occlusion Methods

| Method | Purpose | Thread |
|--------|---------|--------|
| `SetOcclusionCullingEnabled(enabled)` | Enable occlusion culling for drawable | Render |
| `IsOcclusionCullingEnabled()` | Check if occlusion culling enabled | Render |
| `QuickReject(canvas, rect)` | Skip rendering if occluded | Render |
| `GetStencilVal()` | Get stencil value for testing | Render |

## Key Methods by Function

### Surface/App Level Occlusion

| Method | Purpose | Stage |
|--------|---------|--------|
| `CalculateOpaqueAndTransparentRegion()` | Calculate opaque region | Prepare |
| `UpdateNodeVisibleRegion()` | Update visible region | Prepare |
| `SetVisibleRegion()` | Set visible region on surface | Prepare |
| `GetVisibleRegion()` | Query visible region | Prepare/Process |
| `CheckAndUpdateOpaqueRegion()` | Update opaque region | Prepare |

### Canvas/Pixel Level Occlusion

| Method | Purpose | Stage |
|--------|---------|--------|
| `CollectNode(node)` | Add node to occlusion tree | Prepare |
| `CollectSubTree(node, needPrepare)` | Collect subtree nodes | Prepare |
| `ProcessOffTreeNodes(ids)` | Handle removed nodes | Prepare |
| `UpdateChildrenOutOfRectInfo(node)` | Track out-of-rect children | Prepare |
| `CalculateFrameOcclusion()` | Determine occluded nodes | Prepare |
| `TakeCulledNodes()` | Get culled nodes set | Process |
| `TakeCulledEntireSubtree()` | Get culled subtree set | Process |

### Visible Dirty Region

| Method | Purpose | Thread |
|--------|---------|--------|
| `CalculateVisibleDirtyRegion()` | Calculate visible dirty region | Render |
| `GetVisibleDirtyRegion()` | Query visible dirty region | Render |
| `SetVisibleDirtyRegion()` | Set visible dirty region | Render |
| `IsVisibleDirtyRegionEmpty()` | Check if visible dirty empty | Render |

## Thread Safety

| Member | Protection | Thread Access |
|--------|------------|---------------|
| `occlusionNodes_` | No protection | Main only |
| `culledNodes_` | No protection | Main only |
| `accumulatedOcclusionRegion_` | No protection | Main only |
| `visibleRegion_` | Synced via params | Main→Render (sync) |
| `visibleDirtyRegion_` | No protection | Render only |

## Integration Points

### Main Thread (Prepare Stage)

**Surface/App Level:**
- `CalculateOpaqueAndTransparentRegion()` - Calculate opaque region for each surface
- `UpdateNodeVisibleRegion()` - Update visible region after occlusion
- Accumulate opaque regions from top to bottom

**Canvas/Pixel Level:**
- `CollectNode()` - Build occlusion tree
- `CollectSubTree()` - Collect subtree nodes
- `ProcessOffTreeNodes()` - Handle removed nodes
- `CalculateFrameOcclusion()` - Determine occluded nodes

### Render Thread (Process Stage)

**Visible Region Check:**
- `GetVisibleRegion()` - Get visible region from params
- `IsVisibleDirtyRegionEmpty()` - Check occlusion skip condition

**Visible Dirty Region:**
- `CalculateVisibleDirtyRegion()` - Intersect dirty + visible
- Skip rendering if visible dirty region is empty

**Canvas/Pixel Level:**
- `TakeCulledNodes()` - Get culled nodes for skipping
- `QuickReject()` - Skip rendering if occluded

**Pixel-Level Stencil:**
- `SetOcclusionCullingEnabled()` - Enable occlusion for drawable
- Stencil buffer testing for fine-grained culling

### Cross-Thread Data Flow

**Visible Region Sync:**
- Main thread: `SetVisibleRegion()` on surface node
- Sync: `OnSync()` copies visible region to render params
- Render thread: `GetVisibleRegion()` from params

**Visible Dirty Region:**
- Dirty Manager: `GetDirtyRegionForQuickReject()` (synced via OnSync)
- Visible Region: `GetVisibleRegion()` (synced via params)
- Render Thread: `CalculateVisibleDirtyRegion()` = intersection
