# Multi-Surface Rendering for Virtual Screens

**Date:** 2026-03-26 (updated 2026-04-02)
**Author:** Design discussion with Claude

## Overview

This document describes the design for supporting multi-surface rendering on virtual screens, including both mirror and extend modes.

### Goals

1. **Virtual Mirror**: Render the same output to multiple producer surfaces
2. **Virtual Extend**: Split display into multiple surfaces, each handling a rectangular region

### Non-Goals

- Physical screen multi-surface (out of scope)
- Different refresh rates per surface (synchronized only)

## Requirements Summary

| Requirement | Decision |
|-------------|----------|
| Mirror mode | Duplicate full frames to all surfaces |
| Extend mode regions | Arbitrary rect regions per surface |
| API style | New multi-surface API + dynamic add/remove/set |
| Synchronization | All surfaces synchronized together |
| Error handling | Skip failed surfaces, continue with others |

## Key Design Rules

1. **Single-surface mode**: No region is needed at all. Do not touch region logic.
2. **Multi-surface mirror mode**: No region is needed. Copy full frame to each surface.
3. **Multi-surface extend mode**: Regions are only used for blitting images, NOT for specifying buffer config.
4. **SetVirtualScreenSurface**: Called only in single-surface scenario. No regions involved.

## Architecture

### Core Components

```
┌─────────────────────────────────────────────────────────────────┐
│                        Client API Layer                         │
│  RSInterfaces::CreateVirtualScreen(vector<SurfaceRegionConfig>)│
│  RSInterfaces::AddVirtualScreenSurface(...)                     │
│  RSInterfaces::RemoveVirtualScreenSurface(...)                  │
│  RSInterfaces::UpdateVirtualScreenSurfaceRegion(...)            │
│  RSInterfaces::SetVirtualScreenSurfaces(...)                    │
└─────────────────────────────────────────────────────────────────┘
                              ↓
┌─────────────────────────────────────────────────────────────────┐
│                      Screen Manager Layer                       │
│  RSScreenManager - validates uniqueness, delegates to RSScreen  │
│  RSScreen - stores MultiSurfaceConfigs, updates producer surface│
└─────────────────────────────────────────────────────────────────┘
                              ↓
┌─────────────────────────────────────────────────────────────────┐
│                      Rendering Layer                            │
│  RSUniRenderVirtualProcessor - multi-surface frame management   │
│  RSLogicalDisplayRenderNodeDrawable - offscreen render (reuse)  │
└─────────────────────────────────────────────────────────────────┘
```

### Data Structures

```cpp
// Surface region configuration
struct SurfaceRegionConfig {
    sptr<Surface> surface;
    RectI region;  // x, y, width, height in virtual screen coords
};

// Stored in RSScreenProperty via MULTI_SURFACE_CONFIGS property type
using MultiSurfaceConfigs = std::vector<SurfaceRegionConfig>;

// Per-surface frame tracking in the processor
struct SurfaceFrameConfig {
    sptr<Surface> surface;
    std::unique_ptr<RSRenderFrame> frame;
    std::shared_ptr<RSPaintFilterCanvas> canvas;
    RectI region;
    bool isValid = false;
};
```

## API Design

### RSInterfaces (Client API)

```cpp
class RSInterfaces {
public:
    // Existing - backward compatible (converts to single-element vector internally)
    ScreenId CreateVirtualScreen(
        const std::string& name,
        uint32_t width, uint32_t height,
        sptr<Surface> surface,
        ScreenId associatedScreenId = 0,
        int32_t flags = 0,
        std::vector<NodeId> whiteList = {});

    // NEW: Multi-surface creation
    ScreenId CreateVirtualScreen(
        const std::string& name,
        uint32_t width, uint32_t height,
        const std::vector<SurfaceRegionConfig>& surfaceConfigs,
        ScreenId associatedScreenId = 0,
        int32_t flags = 0,
        std::vector<NodeId> whiteList = {});

    // NEW: Dynamic surface management
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

### RSScreen Property Extension

```cpp
// In rs_screen_property.h
enum class ScreenPropertyType : uint32_t {
    ...
    MULTI_SURFACE_CONFIGS,  // vector<SurfaceRegionConfig>
};

// Accessors
std::vector<SurfaceRegionConfig> GetMultiSurfaceConfigs() const;

// In RSScreen class
using MultiSurfaceConfigs = std::vector<SurfaceRegionConfig>;
void SetMultiSurfaceConfigs(const MultiSurfaceConfigs& configs);
MultiSurfaceConfigs GetMultiSurfaceConfigs() const;
```

### VirtualScreenConfigs Extension

```cpp
struct VirtualScreenConfigs {
    // ... existing fields ...
    std::vector<SurfaceRegionConfig> surfaceConfigs; // only set for multi-surface virtual screens
};
```

## Rendering Pipeline

### Buffer Configuration

All surfaces in multi-surface mode use the **full virtual screen dimensions** for their buffers. Regions are only relevant during blitting in extend mode.

```cpp
// In RequestFramesForAllSurfaces:
// ALL surfaces get full virtual screen buffer size
renderFrameConfig_.width = virtualScreenWidth_;
renderFrameConfig_.height = virtualScreenHeight_;
```

### Mirror Mode Flow

```
InitForRenderThread (Processor)
    └── RequestFramesForAllSurfaces() for all surfaces (full size buffers)
    └── primarySurfaceIndex_ = first valid surface
    └── canvas_ = primary surface's canvas

ScreenDrawable::OnDraw(canvas_)
    └── Normal rendering (UNCHANGED)

PostProcess (Processor)
    └── CopyToSecondarySurfaces():
        └── snapshot = primary surface's GetImageSnapshot()
        └── For each secondary surface:
            └── canvas->DrawImage(snapshot, 0, 0)  // full copy, no region
    └── FlushAllSurfaces()
```

### Extend Mode Flow

```
DrawExpandDisplay (LogicalDisplayDrawable)
    └── PrepareOffscreenRender() - creates offscreen (full virtual screen size)
    └── RSRenderNodeDrawable::OnDraw(*curCanvas_) - render to offscreen
    └── GetOffscreenImage() -> offscreen snapshot
    └── virtualProcessor->BlitRegionsToSurfaces(offscreenImage):
        └── For each surface:
            └── srcRect = surface's region (from virtual screen coords)
            └── dstRect = full surface size
            └── canvas->DrawImageRect(snapshot, srcRect, dstRect, sampling)

PostProcess (Processor)
    └── FlushAllSurfaces()  // blitting already done in DrawExpandDisplay
```

### PostProcess Decision Logic

```cpp
void RSUniRenderVirtualProcessor::PostProcess() {
    if (!isMultiSurfaceMode_) {
        // Single surface - original behavior
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

## Component Responsibilities

| Component | Responsibility |
|-----------|---------------|
| **RSUniRenderVirtualProcessor** | RequestFrame for all surfaces (full size), snapshot copy, flush, fence handling |
| **RSLogicalDisplayRenderNodeDrawable** | Offscreen render (existing), GetOffscreenImage() for extend mode |
| **RSScreen** | Store MultiSurfaceConfigs, update producer surface on SetMultiSurfaceConfigs |
| **RSScreenManager** | Validate surface uniqueness across screens, lifecycle management |
| **RSScreenProperty** | Thread-safe storage of MULTI_SURFACE_CONFIGS |
| **IPC layers** | Proxy/Stub serialization of vector<SurfaceRegionConfig> |
| **RSMarshallingHelper** | SurfaceRegionConfig marshalling specialization (WriteParcelable for Surface, RSMarshallingHelper for RectI) |

## Error Handling

| Case | Handling |
|------|----------|
| All surfaces fail | Log error, skip frame, no crash |
| One surface fails | Skip that surface, continue with others |
| Surface removed mid-render | SurfaceFrame invalidated before next frame |
| Empty vector in Add/Set | Return early at client layer, no IPC |
| Remove last surface | Reject at RSScreen level, must keep at least one |
| SetMultiSurfaceConfigs with empty vector | Allowed — clears all surfaces |

## Backward Compatibility

- Existing single-surface `CreateVirtualScreen(surface)` unchanged - converts to single-element vector internally
- `GetProducerSurface()` still works - returns first surface from configs
- `SetVirtualScreenSurface()` unchanged - calls `SetProducerSurface()` directly, no regions
- Single surface mode uses original code path (no performance overhead)
- No changes to `RSScreenRenderNodeDrawable::OnDraw()` flow
- Virtual screens can be created with no surface, surfaces added later via multi-surface APIs

## IPC Transaction Codes — Separation Principle

Single-surface and multi-surface APIs use **separate IPC code ranges**. The two paths must never be mixed:

### Single-Surface IPC Codes (`0x002000`–`0x00200F`)

| Code | Name | Scope |
|------|------|-------|
| 0x002000 | CREATE_VIRTUAL_SCREEN | Single-surface creation |
| 0x002001 | SET_VIRTUAL_SCREEN_RESOLUTION | Both |
| 0x002002 | SET_VIRTUAL_SCREEN_SURFACE | Single-surface only |
| 0x002003 | SET_VIRTUAL_SCREEN_BLACKLIST | Both |
| 0x002004 | ADD_VIRTUAL_SCREEN_BLACKLIST | Both |
| 0x002005 | REMOVE_VIRTUAL_SCREEN_BLACKLIST | Both |
| 0x002006 | SET_VIRTUAL_SCREEN_SECURITY_EXEMPTION_LIST | Both |
| 0x002007 | REMOVE_VIRTUAL_SCREEN | Both |
| 0x002008 | GET_VIRTUAL_SCREEN_RESOLUTION | Both |
| 0x002009 | RESIZE_VIRTUAL_SCREEN | Both |
| 0x00200A | SET_VIRTUAL_SCREEN_USING_STATUS | Both |
| 0x00200B | SET_VIRTUAL_SCREEN_REFRESH_RATE | Both |
| 0x00200C | SET_VIRTUAL_SCREEN_STATUS | Both |
| 0x00200D | SET_VIRTUAL_SCREEN_TYPE_BLACKLIST | Both |
| 0x00200E | ADD_VIRTUAL_SCREEN_WHITELIST | Both |
| 0x00200F | REMOVE_VIRTUAL_SCREEN_WHITELIST | Both |

### Multi-Surface IPC Codes (`0x002010`–`0x002012`)

| Code | Name | Scope |
|------|------|-------|
| 0x002010 | ADD_VIRTUAL_SCREEN_SURFACE | Multi-surface only |
| 0x002011 | REMOVE_VIRTUAL_SCREEN_SURFACE | Multi-surface only |
| 0x002012 | UPDATE_VIRTUAL_SCREEN_SURFACE_REGION | Multi-surface only |

Note: `SET_VIRTUAL_SCREEN_SURFACES` (0x002002) replaces the old `SET_VIRTUAL_SCREEN_SURFACE` and works for both single and multi-surface scenarios.

### Rules

1. `SET_VIRTUAL_SCREEN_SURFACES` (0x002002) replaces the old `SET_VIRTUAL_SCREEN_SURFACE` and works for both single and multi-surface scenarios
2. Multi-surface codes (`0x002010`–`0x002012`) are **multi-surface only** — must not be called on single-surface virtual screens
3. Shared operations (resolution, blacklist, remove, etc.) use the existing single-surface code range

## Permission Checks

All multi-surface IPC codes require `IsSystemCalling()` verification in `RSIClientToServiceConnectionInterfaceCodeAccessVerifier`:

| Code | Permission Check |
|------|-----------------|
| ADD_VIRTUAL_SCREEN_SURFACE (0x002010) | `IsSystemCalling()` |
| REMOVE_VIRTUAL_SCREEN_SURFACE (0x002011) | `IsSystemCalling()` |
| UPDATE_VIRTUAL_SCREEN_SURFACE_REGION (0x002012) | `IsSystemCalling()` |
| SET_VIRTUAL_SCREEN_SURFACES (0x002002) | `IsSystemCalling()` |

## Marshalling

`SurfaceRegionConfig` has a dedicated `RSMarshallingHelper` specialization for proper serialization:

- **Surface**: Serialized via `Parcel::WriteParcelable`/`ReadParcelable<Surface>()` (proper refcount handling)
- **RectI**: Serialized via `RSMarshallingHelper::Marshalling` (existing specialization)
- Guarded with `#ifndef ROSEN_CROSS_PLATFORM`
- Declared in `rs_marshalling_helper.h`, implemented in `rs_marshalling_helper.cpp`
- The `MULTI_SURFACE_CONFIGS` screen property uses `MARSHALL_CASE`/`UNMARSHALL_CASE` macros which invoke the vector overload → per-element `SurfaceRegionConfig` specialization

## Null ProducerSurface Handling

Multi-surface virtual screens keep `producerSurface_` null — surfaces live in `multiSurfaceConfigs`. Both processor Init functions handle this:

- **RSUniRenderVirtualProcessor::Init** (`rs_uni_render_virtual_processor.cpp`): If `producerSurface_` is null AND `IsMultiSurfaceMode()`, calls `RequestFramesForAllSurfaces()` to set up per-surface frames, then uses the primary surface's canvas and frame for backward compatibility
- **RSVirtualScreenProcessor::Init** (`rs_virtual_screen_processor.cpp`): Same pattern — early return to multi-surface setup

Capture logic in `RSLogicalDisplayRenderNodeDrawable::OnCapture` is already null-safe — the `if (auto producerSurface = ...)` guard prevents dereference, and `DrawHardwareEnabledNodes` falls back to `GetRSSurfaceHandlerOnDraw()` when no virtual buffer is available.

## Files Modified

| File | Changes |
|------|---------|
| `screen_manager/rs_screen_info.h` | `SurfaceRegionConfig` struct |
| `screen_manager/rs_screen_property.h/cpp` | `MULTI_SURFACE_CONFIGS` property type, getter |
| `screen_manager/rs_screen.h/cpp` | MultiSurfaceConfigs typedef, Add/Remove/Update/Set, producer surface management |
| `screen_manager/rs_screen_manager.h/cpp` | Validation + delegation for Add/Remove/Update/Set |
| `screen_manager/rs_screen_thread_safe_property.h/cpp` | Thread-safe get/set for MultiSurfaceConfigs |
| `render_thread/rs_uni_render_virtual_processor.h/cpp` | SurfaceFrameConfig, RequestFramesForAllSurfaces, CopyToSecondarySurfaces, FlushAllSurfaces, BlitRegionsToSurfaces, PostProcess multi-surface logic |
| `drawable/rs_logical_display_render_node_drawable.h/cpp` | GetOffscreenImage(), DrawExpandDisplay multi-surface extend branch |
| `rs_interfaces.h/cpp` | All multi-surface public APIs |
| `rs_render_service_client.h/cpp` | Client-side delegation + empty vector early-out |
| IPC proxy/stub/connection/agent | Serialization and pass-through for all APIs |
| IPC interface codes | New enum values for ADD/REMOVE/UPDATE/SET |
