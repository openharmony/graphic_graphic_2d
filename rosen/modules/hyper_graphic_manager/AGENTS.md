# HGM (Hyper Graphic Manager) Module Knowledge Base

## OVERVIEW

Hyper Graphic Manager module for OpenHarmony graphics subsystem. Dynamically adjusts display refresh rate (30/60/90/120Hz) via a policy-driven voting framework. Compiles as `libhyper_graphic_manager` shared library.

## STRUCTURE

```
hyper_graphic_manager/
├── core/
│   ├── frame_rate_manager/     # Central orchestrator + sub-managers (HgmFrameRateManager)
│   ├── hgm_screen_manager/     # HgmCore singleton, HgmScreen, HgmScreenInfo
│   ├── config/                 # XML parser, logging macros, IPC callback manager, user defines
│   ├── utils/                  # Shared types, voter primitives, state machine, timers, LRU cache
│   ├── soft_vsync_manager/     # Per-app software VSync rate division
│   └── native_display_soloist/ # Public C API (OH_DisplaySoloist) for app frame rate control
└── frame_rate_vote/            # RSFrameRateVote, RSVideoFrameRateVote (surface buffer voting)
```

## WHERE TO LOOK

| Task | Location | Notes |
|------|----------|-------|
| Add a new vote source | `core/frame_rate_manager/hgm_frame_rate_manager.h` | Add Handle*Event + VOTER_* in hgm_voter.h |
| Change vote merging logic | `core/utils/hgm_frame_voter.cpp` | ProcessVote/ProcessVoteIter |
| Add new XML config field | `core/utils/hgm_command.h` (struct) → `core/config/xml_parser.cpp` (parse) | PolicyConfigData struct + parser |
| Modify multi-app strategy | `core/frame_rate_manager/hgm_multi_app_strategy.cpp` | UseMax/FollowFocus/UseStrategyNum |
| Change touch idle behavior | `core/frame_rate_manager/hgm_idle_detector.cpp` + `hgm_touch_manager.cpp` | Guaranteed plan + touch state machine |
| Adjust LTPO/DVSync behavior | `core/frame_rate_manager/hgm_frame_rate_manager.cpp` | ProcessLtpoVote, DVSyncTaskProcessor |
| Modify soft VSync per-app rates | `core/soft_vsync_manager/hgm_soft_vsync_manager.cpp` | CollectFrameRateChange |
| Add/modify screen config | `core/hgm_screen_manager/hgm_core.cpp` | Init, AddScreen, SetLtpoConfig |
| Change refresh rate calculation | `core/frame_rate_manager/hgm_frame_rate_manager.cpp` | CalcRefreshRate |
| IPC callback registration | `core/config/hgm_config_callback_manager.cpp` | 3 callback types |
| Test a new feature | `rosen/test/hyper_graphic_manager/unittest/` | Inherit HgmTestBase, use mock_policy_config_visitor.h |

## CODE MAP

| Symbol | Type | Location | Role |
|--------|------|----------|------|
| HgmCore | Singleton class | hgm_screen_manager/hgm_core.h | Top-level entry. Owns HgmFrameRateManager, XMLParser, screens |
| HgmFrameRateManager | Class | frame_rate_manager/hgm_frame_rate_manager.h | Central orchestrator. Owns 8 sub-components |
| HgmFrameVoter | Class | utils/hgm_frame_voter.h | Vote aggregation + LTPO merge |
| HgmVoter | Class | utils/hgm_voter.h | Low-level vote delivery + priority merge |
| VoteInfo | Struct | utils/hgm_voter.h | Single vote: name/min/max/pid |
| HgmMultiAppStrategy | Class | frame_rate_manager/hgm_multi_app_strategy.h | Multi-app conflict resolution |
| HgmIdleDetector | Class | frame_rate_manager/hgm_idle_detector.h | Surface/animation idle detection |
| HgmTouchManager | Class | frame_rate_manager/hgm_touch_manager.h | Touch FSM (inherits HgmStateMachine) |
| HgmPointerManager | Class | frame_rate_manager/hgm_pointer_manager.h | Pointer FSM |
| HgmSoftVSyncManager | Class | soft_vsync_manager/hgm_soft_vsync_manager.h | Per-app VSync rate division |
| HgmVSyncGeneratorController | Class | frame_rate_manager/hgm_vsync_generator_controller.h | VSync rate/offset control |
| HgmEnergyConsumptionPolicy | Singleton | frame_rate_manager/hgm_energy_consumption_policy.h | Power-saving FPS reduction |
| PolicyConfigData | Struct | utils/hgm_command.h | All XML config data (StrategyConfig, ScreenSetting, etc.) |
| PolicyConfigVisitor | Abstract | utils/hgm_command.h | Config query interface |
| XMLParser | Class | config/xml_parser.h | Parses hgm_policy_config.xml |
| HgmScreen | Class (RefBase) | hgm_screen_manager/hgm_screen.h | Screen abstraction with modes |

## CONVENTIONS

- **Naming**: Files `hgm_*.h/cpp`, classes `Hgm*`, enums UPPER_SNAKE_CASE, members trailing `_` (legacy `m` prefix on HgmCore members)
- **Namespace**: `OHOS::Rosen` (newer files) or nested `namespace OHOS { namespace Rosen {` (older files)
- **Logging**: `HGM_LOGD/I/W/E` macros from `config/hgm_log.h` with `%{public}s` privacy format
- **Error returns**: `int32_t` with `HgmErrCode` values (`EXEC_SUCCESS=0`, `HGM_ERROR=-1`), or `bool`
- **Smart pointers**: `sptr<>/wptr<>` for RefBase IPC objects, `shared_ptr`/`unique_ptr` for internal
- **Thread safety**: `std::atomic<T>` for lock-free state, `std::mutex` + `std::lock_guard` for critical sections

## ANTI-PATTERNS

- **DO NOT** access `PolicyConfigData` fields directly from frame_rate_manager/ — use `PolicyConfigVisitor`
- **DO NOT** call `HgmFrameRateManager` directly from render_service — use `HgmContext` (service) or `HgmRenderContext` (render process) facades
- **DO NOT** modify `currRefreshRate_` without `pendingMutex_` lock
- **DO NOT** skip `HGM_LOGE` before error returns

## UNIQUE STYLES

- Vote priority system: voters_ vector order determines precedence in HgmFrameVoter
- XML-driven policy: all strategies/scenes/modes configured in the XML file returned by `GetHgmXmlPath`.
- 4 dynamic touch modes: TOUCH_DISENABLED, TOUCH_ENABLED, TOUCH_EXT_ENABLED, TOUCH_EXT_ENABLED_LTPO_FIRST
- Pimpl pattern for vendor extension: `HgmUserDefine` → `HgmUserDefineImpl` (friend of HgmCore and HgmFrameRateManager)

## COMMANDS

```bash
# Build HGM library
hb build graphic_2d -i --build-target //foundation/graphic/graphic_2d/rosen/modules/hyper_graphic_manager:libhyper_graphic_manager

# Build all HGM unit tests
hb build graphic_2d -t --build-target //foundation/graphic/graphic_2d/rosen/test/hyper_graphic_manager/unittest:unittest

# Build specific test (frame rate manager)
hb build graphic_2d -t --build-target //foundation/graphic/graphic_2d/rosen/test/hyper_graphic_manager/unittest:frame_rate_manager_test
```

## NOTES

- HgmCore is the singleton root — all external access starts from `HgmCore::Instance()`
- External modules (render_service) interact via facades: `HgmContext` (service process) and `HgmRenderContext` (render process), never directly
- Per-frame integration: `mainLoop_()` → `HgmRenderContext::NotifyRpHgmFrameRate()` → IPC → `HgmContext::ProcessHgmFrameRate()`
- IPC callbacks: 3 types (OnHgmConfigChanged, OnHgmRefreshRateModeChanged, OnHgmRefreshRateUpdate) through `RSIHgmConfigChangeCallback`
- Test infrastructure uses `-Dprivate=public` cflags for direct member access
- Vendor extension point: `graphic_2d_hgm_configs.vendor_root` in GN adds extension sources/maps
