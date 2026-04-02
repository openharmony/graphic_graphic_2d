# Special Layer Feature Specification

## Overview

The `special_layer` feature provides a robust and comprehensive layer classification and rendering control system for the OpenHarmony graphics rendering pipeline. It enables selective rendering behavior for security, protected, skip, and blacklist/whitelist layers across multiple screens. The feature is production-ready with the understanding that thread safety and comprehensive screen type testing should be prioritized in follow-up work.

## Layer Type Specification Graph

### Special Layer Types

```
┌─────────────────────────────────────────────────────────────────────────┐
│                        SpecialLayerType Enum                            │
├─────────────────────────────────────────────────────────────────────────┤
│                                                                         │
│  Instance Flags (Lower 10 bits - IS_*):                               │
│  ┌─────────────────────────────────────────────────────────────────────┐   │
│  │ NONE = 0                                                          │   │
│  │ SECURITY = 0x00000001     ← Security layer (sensitive content)   │   │
│  │ SKIP = 0x00000002         ← Skip layer (don't render)          │   │
│  │ PROPROTECTED = 0x00000004  ← Protected layer (DRM)              │   │
│  │ SNAPSHOT_SKIP = 0x00000008 ← Skip in snapshots                  │   │
│  │ IS_BLACK_LIST = 0x00000080  ← Blacklist layer                  │   │
│  │ IS_WHITE_LIST = 0x00000100  ← Whitelist layer                  │   │
│  └─────────────────────────────────────────────────────────────────────┘   │
│                                                                         │
│  Presence Flags (Upper bits - HAS_*):                                   │
│  ┌─────────────────────────────────────────────────────────────────────┐   │
│  │ HAS_SECURITY = SECURITY << 10     (0x00000400)                   │   │
│  │ HAS_SKIP = SKIP << 10             (0x00000800)                   │   │
│  │ HAS_PROTECTED = PROTECTED << 10   (0x00001000)                   │   │
│  │ HAS_SNAPSHOT_SKIP = SNAPSHOT_SKIP << 10 (0x00002000)              │   │
│  │ HAS_BLACK_LIST = IS_BLACK_LIST << 10 (0x00020000)                 │   │
│  │ HAS_WHITE_LIST = IS_WHITE_LIST << 10 (0x00040000)                 │   │
│  └─────────────────────────────────────────────────────────────────────┘   │
│                                                                         │
└─────────────────────────────────────────────────────────────────────────┘
```

### Rendering Behavior Matrix

**1. Normal Display (IsSecurityDisplay() = false):**
- All layer types render normally with no special handling
- No layer type is skipped in normal display context

**2. Security Display (IsSecurityDisplay() = true):**
- Some types render differently based on virtual/physical screen:
  - **Whole screen rendering:** SECURITY (DRAW_BLACK), SKIP (SKIP_DRAW)
  - **Per-surface/window rendering:** BLACK_LIST, WHITE_LIST (SkipperDrawByWhiteList() check)

**3. Rendering Scope:**
- **Whole screen rendering:** SECURITY, SKIP, SNAPSHOT_SKIP (apply to entire screen)
- **Per-surface/window rendering:** BLACK_LIST, WHITE_LIST (checked per surface/window)
```
┌─────────────────────────────────────────────────────────────────────────────┐
│                     Layer Rendering Decision Matrix                        │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                             │
│  Context: Normal Display (IsSecurityDisplay() = false)                      │
│  ┌─────────────────────────────────────────────────────────────────────────┐   │
│  │ Layer Type          │ Draw Behavior │ Reason                          │   │
│  ├─────────────────────┼──────────────┼─────────────────────────────────┤   │
│  │ NONE                │ Normal draw  │ Default rendering                │   │
│  │ SECURITY            │ Normal draw  │ No special handling              │   │
│  │ SKIP                │ Normal draw  │ No special handling              │   │
│  │ PROTECTED           │ Normal draw  │ DRM handling elsewhere           │   │
│  │ SNAPSHOT_SKIP       │ Normal draw  │ Only applies to snapshots       │   │
│  │ BLACK_LIST          │ Normal draw  │ No special handling              │   │
│  │ WHITE_LIST          │ Normal draw  │ No special handling              │   │
│  └─────────────────────────────────────────────────────────────────────────┘   │
│                                                                             │
│  Context: Security Display (IsSecurityDisplay() = true)                       │
│  ┌─────────────────────────────────────────────────────────────────────────┐   │
│  │ Layer Type          │ Draw Behavior │ Reason                          │   │
│  ├─────────────────────┼──────────────┼─────────────────────────────────┤   │
│  │ NONE                │ Normal draw  │ Default rendering                │   │
│  │ SECURITY            (whole screen)│ Security layer → black mask     │   │
│  │                    │ DRAW_BLACK   │ (unless security exemption)       │   │
│  │ SKIP                (whole screen)│ Skip layer → no rendering       │   │
│  │                    │ SKIP_DRAW    │                                 │   │
│  │ PROTECTED           │ Normal draw  │ DRM handling elsewhere           │   │
│  │ SNAPSHOT_SKIP       │ Normal draw  │ Only applies to snapshots       │   │
│  │ BLACK_LIST          (per-surface)│ GetBlackList() check    │   │
│  │                    │ Skip draw    │ (virtual/physical screen)        │   │
│  │ WHITE_LIST          (per-surface)│ SkipDrawByWhiteList() check    │   │
│  │                    │ Skip draw    │ (virtual screen)        │   │
│  └─────────────────────────────────────────────────────────────────────────┘   │
│                                                                             │
│  Context: Snapshot/Capture (isSnapshot_ = true)                             │
│  ┌─────────────────────────────────────────────────────────────────────────┐   │
│  │ Layer Type          │ Draw (scope)│ Reason                          │   │
│  ├─────────────────────┼──────────────┼─────────────────────────────────┤   │
│  │ NONE                │ Normal draw  │ Default rendering                │   │
│  │ SECURITY            (whole screen)│ Security → black/white mask     │   │
│  │                    │ DRAW_BLACK   │ (multi-surface)                  │   │
│  │                    │ DRAW_WHITE   │ (single-surface, no blur/self)  │   │
│  │ SKIP                (whole screen)│ Skip layer → no rendering       │   │
│  │                    │ SKIP_DRAW    │                                 │   │
│  │ PROTECTED           │ Normal draw  │ DRM handling elsewhere           │   │
│  │ SNAPSHOT_SKIP       (whole screen)│ Skip in snapshots              │   │
│  │                    │ SKIP_DRAW    │                                 │   │
│  │ BLACK_LIST          │ Skip draw    │ In blacklist                    │   │
│  │ WHITE_LIST          │ Normal draw  │ In whitelist                    │   │
│  └─────────────────────────────────────────────────────────────────────────┘   │
│                                                                             │
└─────────────────────────────────────────────────────────────────────────────┘
```
### Rendering Scope Classification

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                      Rendering Scope by Layer Type                         │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                             │
│  Whole Screen Rendering (screen-wide mask/fill):                            │
│  ┌─────────────────────────────────────────────────────────────────────────┐   │
│  │ Layer Type          │ Rendering Scope │ Implementation                 │   │
│  ├─────────────────────┼─────────────────┼─────────────────────────────┤   │
│  │ SECURITY            │ Whole screen   │ DRAW_BLACK/WHITE mask fill    │   │
│  │                    │                │ (security display or snapshot)   │   │
│  │ SKIP                │ Whole screen   │ SKIP_DRAW (no rendering)      │   │
│  │                    │                │ (security display)               │   │
│  │ SNAPSHOT_SKIP       │ Whole screen   │ SKIP_DRAW (no rendering)      │   │
│  │                    │                │ (snapshot/capture)             │   │
│  └─────────────────────────────────────────────────────────────────────────┘   │
│                                                                             │
│  Surface/Window Rendering (per-surface check):                                │
│  ┌─────────────────────────────────────────────────────────────────────────┐   │
│  │ Layer Type          │ Rendering Scope │ Implementation                 │   │
│  ├─────────────────────┼─────────────────┼─────────────────────────────┤   │
│  │ BLACK_LIST          │ Per-surface   │ GetBlackList() check    │   │
│  │                    │                │ (security display)               │   │
│  │ WHITE_LIST          │ Per-surface   │ SkipDrawByWhiteList() check    │   │
│  │                    │                │ (security display)               │   │
│  └─────────────────────────────────────────────────────────────────────────┘   │
│                                                                             │
│  Normal Rendering (no special handling):                                      │
│  ┌─────────────────────────────────────────────────────────────────────────┐   │
│  │ Layer Type          │ Rendering Scope │ Implementation                 │   │
│  ├─────────────────────┼─────────────────┼─────────────────────────────┤   │
│  │ NONE                │ Normal        │ Default rendering                │   │
│  │ PROTECTED           │ Normal        │ DRM handling elsewhere           │   │
│  │ SECURITY (normal)   │ Normal        │ No special handling              │   │
│  │ SKIP (normal)       │ Normal        │ No special handling              │   │
│  │ SNAPSHOT_SKIP (norm)│ Normal        │ No special handling              │   │
│  │ BLACK_LIST (norm)   │ Normal        │ No special handling              │   │
│  │ WHITE_LIST (norm)   │ Normal        │ No special handling              │   │
│  └─────────────────────────────────────────────────────────────────────────┘   │
│                                                                             │
└─────────────────────────────────────────────────────────────────────────────┘
```

### SkipDrawByWhiteList() Behavior

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                 SkipDrawByWhiteList() Implementation                       │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                             │
│  Called When: IsSecurityDisplay() = true                                    │
│                                                                             │
│  Decision Logic:                                                             │
│  ┌─────────────────────────────────────────────────────────────────────────┐   │
│  │ 1. Check if sub-thread drawing                                        │   │
│  │   - If parallel canvas, return false (don't skip)                   │   │
│  │                                                                     │   │
│  │ 2. Check if node is in whitelist                                       │   │
│  │   - If node is whitelist node, return false (don't skip)              │   │
│  │   - Check if virtual screen whitelist is empty                            │   │
│  │   - Check if node or parent is whitelist root ID                         │   │
│  │                                                                     │   │
│  │ 3. Check if node's child is in whitelist                               │   │
│  │   - If child is in whitelist, only draw children                      │   │
│  │   - Skip parent node rendering                                          │   │
│  │                                                                     │   │
│  │ 4. Return true (skip draw)                                            │   │
│  └─────────────────────────────────────────────────────────────────────────┘   │
│                                                                             │
│  Scope: Per-surface/window check                                             │
│  - Each surface/window is checked individually                                 │
│  - Parent-child whitelist relationships are considered                             │
│  - Only applies in security display context                                     │
│                                                                             │
└─────────────────────────────────────────────────────────────────────────────┘
```

### Screen Type Interaction

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                      Screen Type Classification                              │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                             │
│  Screen Types:                                                              │
│  ┌─────────────────────────────────────────────────────────────────────────┐   │
│  │ Normal Display                                                           │   │
│  │   - IsSecurityDisplay() = false                                         │   │
│  │   - Renders ALL layers normally (no special handling)                      │   │
│  │   - No layer type is skipped in normal display                           │   │
│  └─────────────────────────────────────────────────────────────────────────┘   │
│                                                                             │
│  ┌─────────────────────────────────────────────────────────────────────────┐   │
│  │ Security Display                                                        │   │
│  │   - IsSecurityDisplay() = true                                          │   │
│  │   - SECURITY layers → DRAW_BLACK (whole screen mask)                   │   │
│  │   - SKIP layers → SKIP_DRAW (whole screen, no rendering)               │   │
│  │   - BLACK_LIST layers → GetBlackList() (per-surface check)     │   │
│  │   - WHITE_LIST layers → SkipDrawByWhiteList() (per-surface check)     │   │
│  │   - Security exemption can override black mask behavior                   │   │
│  └─────────────────────────────────────────────────────────────────────────┘   │
│                                                                             │
│  ┌─────────────────────────────────────────────────────────────────────────┐   │
│  │ Mirror Display                                                          │   │
│  │   - Has mirror source display                                           │   │
│  │   - Checks intersection with special layers                               │   │
│  │   - Virtual screen rect intersection test                                │   │
│  │   - Can have security exemption                                        │   │
│  └─────────────────────────────────────────────────────────────────────────┘   │
│                                                                             │
│  ┌─────────────────────────────────────────────────────────────────────────┐   │
│  │ Snapshot/Capture Display                                               │   │
│  │   - isSnapshot_ = true                                                │   │
│  │   - SECURITY layers → black/white mask (whole screen)                   │   │
│  │   - SKIP layers → SKIP_DRAW (whole screen, no rendering)               │   │
│  │   - SNAPSHOT_SKIP layers → SKIP_DRAW (whole screen)                      │   │
│  │   - BLACK_LIST layers → Skip draw (in blacklist)                        │   │
│  │   - CAPTURE_WINDOW_NAME special handling for SKIP layers                │   │
│  └─────────────────────────────────────────────────────────────────────────┘   │
│                                                                             │
└─────────────────────────────────────────────────────────────────────────────┘
```

### DrawType Enum

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                           DrawType Enum                                   │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                             │
│  enum class DrawType {                                                      │
│      NONE,       // Normal drawing without special handling                    │
│      DRAW_WHITE, // Draw white mask (security layers in snapshot,           │
│                  // single surface, no blur/self-capture)                   │
│      DRAW_BLACK, // Draw black mask (security layers in security display      │
│                  // or multi-surface snapshot)                             │
│      SKIP_DRAW   // Skip drawing without any rendering operation              │
│  };                                                                        │
│                                                                             │
└─────────────────────────────────────────────────────────────────────────────┘
```

### Key Decision Points

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                      Rendering Decision Flowchart                             │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                             │
│  Start                                                                     │
│    │                                                                       │
│    ▼                                                                       │
│  ┌─────────────────┐                                                       │
│  │ IsSecurityDisplay()? │                                                       │
│  └────────┬────────┘                                                       │
│           │                                                                 │
│     Yes   │   No                                                           │
│  ┌────────┴────────┐                                                       │
│  │                 │                                                       │
│  ▼                 ▼                                                       │
│  GetDrawTypeIn    Normal Draw                                              │
│  SecurityDisplay()  (all layers)                                            │
│  │                 │                                                       │
│  ▼                 ▼                                                       │
│  ┌─────────────────────────────────────────────────────────────────────┐   │
│  │ Check layer type and return appropriate DrawType:                     │   │
│  │   - SECURITY: DRAW_BLACK (whole screen mask)                       │   │
│  │   - SKIP: SKIP_DRAW (whole screen)                                │   │
│  │   - BLACK_LIST: GetBlackList() (per-surface)              │   │
│  │   - WHITE_LIST: SkipDrawByWhiteList() (per-surface)              │   │
│  │   - Others: NONE (normal draw)                                      │   │
│  └─────────────────────────────────────────────────────────────────────┘   │
│           │                                                                 │
│           ▼                                                                 │
│  ┌─────────────────────────────────────────────────────────────────────┐   │
│  │ Apply DrawType:                                                    │   │
│  │   - NONE: Normal rendering                                         │   │
│  │   - DRAW_BLACK: Fill with black color (whole screen)                │   │
│  │   - DRAW_WHITE: Fill with white color (whole screen)                │   │
│  │   - SKIP_DRAW: Skip rendering entirely (whole screen)                │   │
│  └─────────────────────────────────────────────────────────────────────┘   │
│                                                                             │
└─────────────────────────────────────────────────────────────────────────────┘
```
```
┌─────────────────────────────────────────────────────────────────────────────┐
│                    Snapshot/Capture Decision Flowchart                      │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                             │
│  Start (isSnapshot_ = true)                                              │
│    │                                                                       │
│    ▼                                                                       │
│  ┌─────────────────────────────────────────────────────────────────────┐   │
│  │ Check layer type and return appropriate DrawType:                     │   │
│  │   - SECURITY: DRAW_BLACK (multi-surface) or DRAW_WHITE (single)   │   │
│  │   - SKIP: SKIP_DRAW (whole screen)                                │   │
│  │   - SNAPSHOT_SKIP: SKIP_DRAW (whole screen)                        │   │
│  │   - BLACK_LIST: Skip draw (in blacklist)                            │   │
│  │   - Others: NONE (normal draw)                                      │   │
│  └─────────────────────────────────────────────────────────────────────┘   │
│           │                                                                 │
│           ▼                                                                 │
│  ┌─────────────────────────────────────────────────────────────────────┐   │
│  │ Apply DrawType:                                                    │   │
│  │   - NONE: Normal rendering                                         │   │
│  │   - DRAW_BLACK: Fill with black color (whole screen)                │   │
│  │   - DRAW_WHITE: Fill with white color (whole screen)                │   │
│  │   - SKIP_DRAW: Skip rendering entirely (whole screen)                │   │
│  └─────────────────────────────────────────────────────────────────────┘   │
│                                                                             │
└─────────────────────────────────────────────────────────────────────────────┘
```

## Architecture Components

### 1. RSSpecialLayerManager
**Location:** `rosen/modules/render_service_base/include/common/rs_special_layer_manager.h`

**Responsibilities:**
- Track special layer types per node instance
- Manage node IDs for each special layer type
- Support screen-aware special layer tracking
- Maintain whitelist root ID stack

**Key Methods:**
```cpp
// Instance-based layer management
bool Set(uint32_t type, bool enable);
bool Find(uint32_t type) const;
void AddIds(uint32_t type, NodeId id);
void RemoveIds(uint32_t type, NodeId id);

// Screen-aware operations
bool SetWithScreen(ScreenId screenId, uint32_t type, bool enable);
bool FindWithScreen(ScreenId screenId, uint32_t type) const;
void AddIdsWithScreen(ScreenId screenId, uint32_t type, NodeId id);
void RemoveIdsWithScreen(ScreenId screenId, uint32_t type, NodeId id);

// Static whitelist management
static void SetWhiteListRootId(LeashPersistentId id);
static void ResetWhiteListRootId(LeashPersistentId id);
static bool IsWhiteListRootIdsEmpty();
```

### 2. ScreenSpecialLayerInfo
**Location:** `rosen/modules/render_service_base/include/common/rs_special_layer_manager.h`

**Responsibilities:**
- Global registry for screen-specific special layer information
- Track which screens have which special layer types
- Maintain global blacklist

**Key Methods:**
```cpp
static void Update(SpecialLayerType type, ScreenId screenId, const std::unordered_set<NodeId>& nodeIds);
static void ClearByScreenId(ScreenId screenId);
static std::unordered_set<ScreenId> QueryEnableScreen(SpecialLayerType type, std::pair<NodeId, LeashPersistentId> id);
static void SetGlobalBlackList(const std::unordered_set<NodeId>& globalBlackList);
```

### 3. RSSpecialLayerUtils
**Location:** `rosen/modules/render_service/core/feature/special_layer/rs_special_layer_utils.h`

**Responsibilities:**
- Core rendering decision logic
- Screen-special layer intersection checking
- Draw type determination for different contexts

**Key Methods:**
```cpp
// Rendering decisions
static DrawType GetDrawTypeInSecurityDisplay(const RSSurfaceRenderParams& surfaceParams, const RSRenderThreadParams& uniParams);
static DrawType GetDrawTypeInSnapshot(const RSSurfaceRenderParams& surfaceParams);

// Intersection checking
static void CheckSpecialLayerIntersectMirrorDisplay(const RSLogicalDisplayRenderNode& mirrorNode, RSLogicalDisplayRenderNode& sourceNode);

// State queries
static DisplaySpecialLayerState GetSpecialLayerStateInVisibleRect(RSLogicalDisplayRenderParams* displayParams, RSScreenRenderParams* screenParams);
static DisplaySpecialLayerState GetSpecialLayerStateInSubTree(RSLogicalDisplayRenderParams& displayParams, RSScreenRenderParams* screenParams);

// Blacklist/whitelist management
static std::unordered_set<NodeId> GetMergeBlackList(const RSScreenProperty& screenProperty);
static void UpdateScreenSpecialLayer(const RSScreenProperty& screenProperty);
```

### 4. AutoSpecialLayerStateRecover
**Location:** `rosen/modules/render_service_base/include/common/rs_special_layer_manager.h`

**Responsibilities:**
- RAII wrapper for automatic whitelist root ID state recovery
- Ensures whitelist state is properly restored after scope exit

## Security Considerations

### Positive Aspects

1. **Security Layer Isolation**
   - SECURITY layers explicitly marked
   - Black mask rendering in security displays
   - White mask rendering in snapshots

2. **Access Control**
   - Blacklist/whitelist mechanism
   - Global blacklist support
   - Screen-specific access control

3. **Snapshot Protection**
   - SNAPSHOT_SKIP layer type
   - Security layer masking in snapshots
   - CAPTURE_WINDOW_NAME special handling

## Performance Impact

### Memory

- **Per-node:** RSSpecialLayerManager instance (~100-200 bytes)
- **Global:** ScreenSpecialLayerInfo static maps (depends on active screens/layers)
- **Stack:** whiteListRootIds_ stack (typically < 10 entries)

### CPU

- **Bitmask operations:** O(1)
- **Map lookups:** O(1) average
- **FindScreenHasType:** O(n) where n = number of screens
- **AddIds/RemoveIds:** O(1) for insert/erase

### Rendering Pipeline

- **Minimal overhead** in hot paths
- **Used primarily** for conditional rendering decisions
- **Whitelist checks** at draw time (line 367 in rs_render_node_drawable.cpp)

## Compatibility

### Backward Compatibility

- **New feature** - no existing code to break
- **Well-isolated** in render_service_base module
- **Optional usage** - only used when needed

### Forward Compatibility

- **Extensible enum** design allows adding new types
- **Bitmask system** supports future type additions
- **Template-based operations** work with new types

## [TBC]Integration Points

### 1. Render Node Drawable
**File:** `rosen/modules/render_service/core/drawable/rs_render_node_drawable.cpp:367`

```cpp
// Check if whitelist root IDs are present for drawing decisions
return !RSSpecialLayerManager::IsWhiteListRootIdsEmpty();
```

### 2. Screen Render Node Drawable
**File:** `rosen/modules/render_service/core/drawable/rs_screen_render_node_drawable.cpp:540`

```cpp
void RSScreenRenderNodeDrawable::OnDraw(Drawing::Canvas& canvas)
{
    RSSpecialLayerManager::ClearWhiteListRootIds();  // Reset at frame start
    // ... rest of draw logic
}
```

### 3. Surface Render Node Drawable
**File:** `rosen/modules/render_service/core/drawable/rs_surface_render_node_drawable.cpp:1267-1268`

```cpp
auto drawType = captureParam.isSnapshot_ ? 
    RSSpecialLayerUtils::GetDrawTypeInSnapshot(surfaceParams) :
    RSSpecialLayerUtils::GetDrawTypeInSecurityDisplay(surfaceParams, uniParams);
```

### 4. Canvas/Effect Render Node Drawables
**Files:** 
- `rosen/modules/render_service/core/drawable/rs_canvas_render_node_drawable.cpp:116,194`
- `rosen/modules/render_service/core/drawable/rs_effect_render_node_drawable.cpp:72`
- `rosen/modules/render_service/core/drawable/rs_canvas_drawing_render_node_drawable.cpp:124`

```cpp
// Whitelist check in security display
if (LIKELY(uniParam) && uniParam->IsSecurityDisplay() && 
    RSRenderNodeDrawable::SkipDrawByWhiteList(canvas)) {
    return;
}
```