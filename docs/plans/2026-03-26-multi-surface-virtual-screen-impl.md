# Multi-Surface Virtual Screen Implementation Details

**Date:** 2026-03-26 (updated 2026-04-02)
**Author:** Design discussion with Claude

## Overview

This document describes the actual implementation of multi-surface rendering for virtual screens. The implementation uses `MultiSurfaceConfigs` (stored directly as a thread-safe screen property) rather than a separate wrapper class, keeping the design minimal and consistent with existing patterns.

## Key Design Rules (Must Follow)

1. **Single-surface mode**: No region is needed. Do not touch region logic.
2. **Multi-surface mirror mode**: No region is needed. Copy full frame to each surface.
3. **Multi-surface extend mode**: Regions are ONLY used for blitting images, NOT for specifying buffer config.
4. **SetVirtualScreenSurface**: Called only in single-surface scenario. No regions involved.

## Data Structures

### SurfaceRegionConfig

Defined in `rs_screen_info.h`:
```cpp
struct SurfaceRegionConfig {
    sptr<Surface> surface = nullptr;
    RectI region;  // x, y, width, height in virtual screen coordinates
};
```

### MultiSurfaceConfigs

Stored in `RSScreenProperty` via the `MULTI_SURFACE_CONFIGS` property type:
```cpp
using MultiSurfaceConfigs = std::vector<SurfaceRegionConfig>;
```

Property registration in `rs_screen_property.h`:
```cpp
DECLARE_PROPERTY_TYPE(ScreenPropertyType::MULTI_SURFACE_CONFIGS, MultiSurfaceConfigs, MultiSurfaceConfigs());
```

### SurfaceFrameConfig (Processor)

Per-surface frame tracking in `rs_uni_render_virtual_processor.h`:
```cpp
struct SurfaceFrameConfig {
    sptr<Surface> surface;
    std::unique_ptr<RSRenderFrame> frame;
    std::shared_ptr<RSPaintFilterCanvas> canvas;
    RectI region;
    bool isValid = false;
};
```

### VirtualScreenConfigs Extension

In `rs_screen.h`, the `VirtualScreenConfigs` struct has a new field:
```cpp
std::vector<SurfaceRegionConfig> surfaceConfigs; // only set for multi-surface virtual screens
```

## Component Responsibilities

| Component | File | Responsibility |
|-----------|------|---------------|
| **RSInterfaces** | `rs_interfaces.h/cpp` | Public API layer, delegates to RSRenderServiceClient |
| **RSRenderServiceClient** | `rs_render_service_client.h/cpp` | Client-side delegation + empty-vector early-out |
| **IPC Proxy** | `rs_client_to_service_connection_proxy.h/cpp` | Serializes vector<SurfaceRegionConfig> for IPC |
| **IPC Stub** | `rs_client_to_service_connection_stub.cpp` | Deserializes vector<SurfaceRegionConfig> from IPC |
| **Connection** | `rs_client_to_service_connection.h/cpp` | Pass-through between stub and agent |
| **Agent** | `rs_screen_manager_agent.h/cpp` | Pass-through between connection and manager |
| **RSScreenManager** | `rs_screen_manager.h/cpp` | Validation (uniqueness, virtual screen check) + delegation to RSScreen |
| **RSScreen** | `rs_screen.h/cpp` | Stores MultiSurfaceConfigs, updates producer surface, implements Add/Remove/Update/Set |
| **RSScreenProperty** | `rs_screen_property.h/cpp` | Thread-safe storage of MULTI_SURFACE_CONFIGS |
| **RSUniRenderVirtualProcessor** | `rs_uni_render_virtual_processor.h/cpp` | Multi-surface frame lifecycle: request, copy, blit, flush |
| **RSLogicalDisplayRenderNodeDrawable** | `rs_logical_display_render_node_drawable.h/cpp` | Offscreen render for extend mode, GetOffscreenImage() |

## API Design

### RSInterfaces (Public API)

```cpp
class RSInterfaces {
public:
    // Existing - backward compatible (converts to single-element vector internally)
    ScreenId CreateVirtualScreen(
        const std::string& name, uint32_t width, uint32_t height,
        sptr<Surface> surface,
        ScreenId associatedScreenId = 0, int32_t flags = 0,
        std::vector<NodeId> whiteList = {});

    // NEW: Multi-surface creation
    ScreenId CreateVirtualScreen(
        const std::string& name, uint32_t width, uint32_t height,
        const std::vector<SurfaceRegionConfig>& surfaceConfigs,
        ScreenId associatedScreenId = 0, int32_t flags = 0,
        std::vector<NodeId> whiteList = {});

    // NEW: Dynamic surface management (multi-surface virtual screens only)
    int32_t AddVirtualScreenSurface(
        ScreenId id, const std::vector<SurfaceRegionConfig>& surfaceConfigs);
    int32_t RemoveVirtualScreenSurface(
        ScreenId id, const std::vector<sptr<Surface>>& surfaces);
    int32_t UpdateVirtualScreenSurfaceRegion(
        ScreenId id, sptr<Surface> surface, const RectI& region);
    int32_t SetVirtualScreenSurfaces(
        ScreenId id, const std::vector<SurfaceRegionConfig>& surfaceConfigs);

    // Existing - single-surface only, no regions
    int32_t SetVirtualScreenSurface(ScreenId id, sptr<Surface> surface);
};
```

## IPC Transaction Codes — Separation Principle

Single-surface and multi-surface APIs use **separate IPC code ranges**. The two paths must never be mixed.

### Multi-Surface IPC Codes (`0x002010`–`0x002012`)

| Code | Name | Direction |
|------|------|-----------|
| 0x002010 | ADD_VIRTUAL_SCREEN_SURFACE | Client -> Service |
| 0x002011 | REMOVE_VIRTUAL_SCREEN_SURFACE | Client -> Service |
| 0x002012 | UPDATE_VIRTUAL_SCREEN_SURFACE_REGION | Client -> Service |

Note: `SET_VIRTUAL_SCREEN_SURFACES` (0x002002) replaces the old `SET_VIRTUAL_SCREEN_SURFACE` and works for both single and multi-surface scenarios.

### Separation Rules

1. `SET_VIRTUAL_SCREEN_SURFACE` (0x002002) is **single-surface only** — must not be called on multi-surface virtual screens
2. Multi-surface codes (`0x002011`–`0x002014`) are **multi-surface only** — must not be called on single-surface virtual screens
3. Shared operations (resolution, blacklist, remove, etc.) use the existing single-surface code range (`0x002000`–`0x00200F`)

### IPC Serialization Format

For Add and Set APIs (taking `vector<SurfaceRegionConfig>`):
```
[configCount: uint32_t]
[config[0].surface: sptr<Surface>]
[config[0].region: RectI (left, top, width, height)]
...
[config[N-1].surface: sptr<Surface>]
[config[N-1].region: RectI (left, top, width, height)]
```

For Remove: `[surface: sptr<Surface>]`
For Update: `[surface: sptr<Surface>] [region: RectI]`

## Validation and Error Handling

### Client-Side (RSRenderServiceClient)

Empty vector early-out for Add and Set APIs:
```cpp
if (surfaceConfigs.empty()) {
    RS_LOGW("RSRenderServiceClient::%{public}s: surfaceConfigs is empty.", __func__);
    return INVALID_ARGUMENTS;
}
```

### Manager-Side (RSScreenManager)

| Check | Where | Error |
|-------|-------|-------|
| Screen ID is virtual | `IsVirtualScreen(id)` | `SCREEN_NOT_FOUND` or `INVALID_ARGUMENTS` |
| Surface uniqueness across screens | `IsSurfaceUsedByOtherVirtualScreen()` | `SURFACE_ALREADY_IN_USE` |
| Empty configs in Set | Client-side guard in `RSRenderServiceClient` | Return `INVALID_ARGUMENTS` |
| Removing last surface | `RemoveVirtualScreenSurface()` | Reject, must keep at least one |

### Screen-Side (RSScreen)

`SetMultiSurfaceConfigs` updates producer surface (virtual screens can be created without surfaces, then surfaces added later):
```cpp
void RSScreen::SetMultiSurfaceConfigs(const MultiSurfaceConfigs& configs)
{
    UPDATE_PROPERTY(MultiSurfaceConfigs, configs);
    if (configs.front().surface != nullptr) {
        SetProducerSurface(configs.front().surface);
    }
}
```

`AddVirtualScreenSurface` takes a vector, adds non-duplicate entries:
```cpp
int32_t RSScreen::AddVirtualScreenSurface(
    const std::vector<SurfaceRegionConfig>& surfaceConfigs)
```

`RemoveVirtualScreenSurface` rejects removing the last surface.

## Rendering Pipeline

### Single-Surface Mode (Unchanged)

Uses original code path. No performance overhead.

### Mirror Mode Flow

```
InitForRenderThread (Processor)
    RequestFramesForAllSurfaces() for all surfaces (full size buffers)
    primarySurfaceIndex_ = first valid surface
    canvas_ = primary surface's canvas

ScreenDrawable::OnDraw(canvas_)
    Normal rendering (UNCHANGED)

PostProcess (Processor)
    CopyToSecondarySurfaces():
        snapshot = primary surface's GetImageSnapshot()
        For each secondary surface:
            canvas->DrawImage(snapshot, 0, 0)  // full copy, no region
    FlushAllSurfaces()
```

### Extend Mode Flow

```
DrawExpandDisplay (LogicalDisplayDrawable)
    PrepareOffscreenRender() - creates offscreen (full virtual screen size)
    RSRenderNodeDrawable::OnDraw(*curCanvas_) - render to offscreen
    GetOffscreenImage() -> offscreen snapshot
    virtualProcessor->BlitRegionsToSurfaces(offscreenImage):
        For each surface:
            srcRect = surface's region (from virtual screen coords)
            dstRect = full surface size
            canvas->DrawImageRect(snapshot, srcRect, dstRect, sampling)

PostProcess (Processor)
    FlushAllSurfaces()  // blitting already done in DrawExpandDisplay
```

### Buffer Configuration

ALL surfaces in multi-surface mode use the **full virtual screen dimensions** for their buffers. Regions are only relevant during blitting in extend mode:
```cpp
// In RequestFramesForAllSurfaces:
renderFrameConfig_.width = virtualScreenWidth_;
renderFrameConfig_.height = virtualScreenHeight_;
```

### PostProcess Decision Logic

```cpp
void RSUniRenderVirtualProcessor::PostProcess() {
    if (!isMultiSurfaceMode_) {
        renderFrame_->Flush();
        if (isMirror_) MergeMirrorFenceToHardwareEnabledDrawables();
        return;
    }
    if (validSurfaceCount <= 1) {
        FlushAllSurfaces();
        return;
    }
    if (isMirror_) {
        CopyToSecondarySurfaces();  // full copy, no regions
        FlushAllSurfaces();
    } else {
        // Extend mode: blitting already done in DrawExpandDisplay
        FlushAllSurfaces();
    }
}
```

## Files Modified

| File | Changes |
|------|---------|
| `screen_manager/rs_screen_info.h` | `SurfaceRegionConfig` struct |
| `screen_manager/rs_screen_property.h/cpp` | `MULTI_SURFACE_CONFIGS` property type, `DECLARE_PROPERTY_TYPE`, getter |
| `screen_manager/rs_screen_thread_safe_property.h/cpp` | Thread-safe get/set for MultiSurfaceConfigs |
| `screen_manager/rs_screen.h/cpp` | `VirtualScreenConfigs::surfaceConfigs` field, `MultiSurfaceConfigs` typedef, `SetMultiSurfaceConfigs`, `AddVirtualScreenSurface(vector)`, `RemoveVirtualScreenSurface`, `UpdateVirtualScreenSurfaceRegion`, `SetVirtualScreenSurfaces`, producer surface management |
| `screen_manager/rs_screen_manager.h/cpp` | Validation + delegation for Add/Remove/Update/Set, uniqueness checks |
| `render_thread/rs_uni_render_virtual_processor.h/cpp` | `SurfaceFrameConfig` struct, `RequestFramesForAllSurfaces`, `CopyToSecondarySurfaces`, `FlushAllSurfaces`, `BlitRegionsToSurfaces`, `PostProcess` multi-surface logic, `GetValidSurfaceCount`, `IsMultiSurfaceMode`, `GetPrimarySurfaceIndex` |
| `drawable/rs_logical_display_render_node_drawable.h/cpp` | `GetOffscreenImage()`, `HasOffscreenSurface()`, `DrawExpandDisplay` with optional processor param, multi-surface extend branch |
| `rs_interfaces.h/cpp` | All multi-surface public APIs |
| `rs_render_service_client.h/cpp` | Client-side delegation + empty vector early-out |
| IPC proxy/stub/connection/agent | Serialization and pass-through for all APIs |
| IPC interface codes | `ADD_VIRTUAL_SCREEN_SURFACE`, `REMOVE_VIRTUAL_SCREEN_SURFACE`, `UPDATE_VIRTUAL_SCREEN_SURFACE_REGION`, `SET_VIRTUAL_SCREEN_SURFACES` enum values |

## Null ProducerSurface Handling in Processor Init

Both `RSUniRenderVirtualProcessor::Init` and `RSVirtualScreenProcessor::Init` check `GetProducerSurface()` for null. In multi-surface mode, `GetProducerSurface()` returns null because no single surface is assigned to the screen property. The fix:

- If `producerSurface_` is null AND screen is in multi-surface mode → skip single-surface init, call `RequestFramesForAllSurfaces()` to set up per-surface frames
- If `producerSurface_` is null AND NOT multi-surface mode -> return false (original behavior preserved)

**Key files:**
- `rs_uni_render_virtual_processor.cpp` lines 119-141: multi-surface early return
- `rs_virtual_screen_processor.cpp` lines 48-58: multi-surface early return

### Capture Logic Safety
`RSLogicalDisplayRenderNodeDrawable::OnCapture` (line 318) already guards null producerSurface with `if (auto producerSurface = ...)`. When producerSurface is null in multi-surface mode, `virtualHasBuffer` stays false, and code falls through to either:
1. Render-from-scratch path (noBuffer=true) → safe fallback
2. `DrawHardwareEnabledNodes` with null virtualBuffer → falls back to `GetRSSurfaceHandlerOnDraw()` → safe

No code changes needed for capture logic.

## Backward Compatibility- Existing single-surface `CreateVirtualScreen(surface)` unchanged - converts to `{surface, {}}` (empty region) internally
- `GetProducerSurface()` still works - returns first surface from configs
- `SetVirtualScreenSurface()` unchanged - calls `SetProducerSurface(surface)` directly, no regions
- Single surface mode uses original code path (no performance overhead)
- No changes to `RSScreenRenderNodeDrawable::OnDraw()` flow
- Virtual screens can be created with no surface, surfaces added later via multi-surface APIs

## Delegation Chain

The full IPC delegation chain for each multi-surface API:

```
RSInterfaces
  -> RSRenderServiceClient (empty vector early-out)
    -> RSClientToServiceConnectionProxy (serialize vector)
      -> RSClientToServiceConnectionStub (deserialize)
        -> RSClientToServiceConnection (pass-through)
          -> RSScreenManagerAgent (pass-through)
            -> RSScreenManager (validation)
              -> RSScreen (SetMultiSurfaceConfigs / Add / Remove / Update)
```

## Permission Checks

All multi-surface IPC codes are verified in `RSIClientToServiceConnectionInterfaceCodeAccessVerifier::IsExclusiveVerification` using `IsSystemCalling()`:

| Code | Permission |
|------|-----------|
| SET_VIRTUAL_SCREEN_SURFACES (0x002002) | `IsSystemCalling()` |
| ADD_VIRTUAL_SCREEN_SURFACE (0x002010) | `IsSystemCalling()` |
| REMOVE_VIRTUAL_SCREEN_SURFACE (0x002011) | `IsSystemCalling()` |
| UPDATE_VIRTUAL_SCREEN_SURFACE_REGION (0x002012) | `IsSystemCalling()` |

## SurfaceRegionConfig Marshalling

`SurfaceRegionConfig` has a dedicated `RSMarshallingHelper` specialization for proper serialization:

- **Surface**: Serialized via `Parcel::WriteParcelable`/`ReadParcelable<Surface>()` (proper refcount handling)
- **RectI**: Serialized via existing `RSMarshallingHelper::Marshalling` specialization
- Guarded with `#ifndef ROSEN_CROSS_PLATFORM`
- Declared in `rs_marshalling_helper.h`, implemented in `rs_marshalling_helper.cpp` (platform/ohos)
- The `MULTI_SURFACE_CONFIGS` screen property uses `MARSHALL_CASE`/`UNMARSHALL_CASE` macros in `rs_screen_property.cpp`, which invoke the vector overload -> per-element `SurfaceRegionConfig` specialization

## Include Dependencies

`SurfaceRegionConfig` is defined in `rs_screen_info.h` under `#ifndef ROSEN_CROSS_PLATFORM`. Public callers get it through:
```
rs_interfaces.h -> rs_render_service_client.h -> rs_screen_info.h
```
