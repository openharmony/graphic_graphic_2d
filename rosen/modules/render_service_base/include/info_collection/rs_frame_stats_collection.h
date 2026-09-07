/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef RS_FRAME_STATS_COLLECTION_H
#define RS_FRAME_STATS_COLLECTION_H

#include <atomic>
#include <mutex>
#include <map>
#include <vector>

#include "singleton.h"

#include "common/rs_common_def.h"

namespace OHOS {
namespace Rosen {

struct FrameStatsCounter {
    enum class Global : size_t {
        RSMainThread_Base = 0,
        RSUniRenderThread_Base = 100,
        RSSubThread_Base = 200,
        RSRenderComposer_Base = 300,
        COUNT = 500
    };

    enum class RSMainThread : size_t {
        TotalFrames = 0,
        DirectCompositionFrames,
        NothingToUpdateFrames,
        AnimationRunningEmpty,
    };

    enum class RSUniRenderThread : size_t {
        TotalFrames = 0,
        SkipCommitFrames,
        SkipForceCommitFrames,
        DrawImageGPUCount,
        UpdateCacheSurface,
    };

    enum class RSSubThread : size_t {
        TotalFrames = 0,
    };

    enum class RSRenderComposer : size_t {
        GpuComposeRedrawFrames = 0,
        CommitAndGetReleaseFence,
        PerLayerRate_Begin, // 2D block starts here
    };

    static constexpr size_t RATE_COUNT = 15;
    static constexpr size_t LAYER_SLOTS = 5; // track layers 1,2,3,4,5+
    static constexpr size_t PER_LAYER_RATE_TOTAL = LAYER_SLOTS * RATE_COUNT;

    // Rate index mapping (aligned with IDEAL_PERIOD)
    static size_t RateSlot(uint32_t rate)
    {
        static constexpr uint32_t rates[] = { 10, 15, 20, 24, 30, 36, 40, 45, 48, 60, 72, 80, 90, 120, 144 };
        for (size_t i = 0; i < RATE_COUNT; ++i) {
            if (rates[i] == rate) {
                return i;
            }
        }
        return 0; // default to 10Hz
    }

    static size_t LayerSlot(size_t layerCount)
    {
        if (layerCount == 0 || layerCount >= LAYER_SLOTS) {
            return LAYER_SLOTS - 1;
        }
        return layerCount - 1;
    }

    static size_t PerLayerRateIndex(size_t layerCount, uint32_t rate)
    {
        size_t idx = static_cast<size_t>(RSRenderComposer::PerLayerRate_Begin)
            + LayerSlot(layerCount) * RATE_COUNT + RateSlot(rate);
        return static_cast<size_t>(Global::RSRenderComposer_Base) + idx;
    }

    // Convert category enum value to global array index.
    static size_t ToIndex(RSMainThread c)
    {
        return static_cast<size_t>(Global::RSMainThread_Base) + static_cast<size_t>(c);
    }

    static size_t ToIndex(RSUniRenderThread c)
    {
        return static_cast<size_t>(Global::RSUniRenderThread_Base) + static_cast<size_t>(c);
    }

    static size_t ToIndex(RSSubThread c)
    {
        return static_cast<size_t>(Global::RSSubThread_Base) + static_cast<size_t>(c);
    }

    static size_t ToIndex(RSRenderComposer c)
    {
        return static_cast<size_t>(Global::RSRenderComposer_Base) + static_cast<size_t>(c);
    }
};

// FrameStats switch levels, controlled by persist.sys.graphic.openFrameStats
enum class FrameStatsLevel : int32_t {
    Disabled = 0,
    Basic = 1,
    Full = 2,
};

// Target detail category for per-surfacenode counters
enum class FrameStatsDetail : int32_t {
    UniRenderThread = 0,
    MainThread = 1,
};

// Per-app snapshot: fixed counters + per-surfacenode counters
struct AppFrameStats {
    std::vector<uint64_t> fixedCounters;
    std::map<std::string, std::map<std::string, uint64_t>> uniRenderThreadDetailStats;
    std::map<std::string, std::map<std::string, uint64_t>> mainThreadDetailStats;
};

struct RSB_EXPORT FrameStatsEntry {
    std::string name;
    uint64_t count = 0;
    FrameStatsEntry() = default;
    explicit FrameStatsEntry(const std::string& name_, uint64_t count_ = 0) : name(name_), count(count_) {}
};

class RSB_EXPORT RSFrameStatsCollection {
public:
    static RSFrameStatsCollection& GetInstance();

    static bool IsEnabled()
    {
        return enabledLevel_ > FrameStatsLevel::Disabled;
    }

    static FrameStatsLevel GetLevel()
    {
        return enabledLevel_;
    }

    static bool IsLevelAtLeast(FrameStatsLevel level)
    {
        return enabledLevel_ >= level;
    }

    // Atomically increment a counter by 1. Lock-free on hot path.
    // Uses relaxed memory order for minimal overhead
    void Increment(size_t counter);

    // Increment a per-surfacenode counter by value. Lazy-registers surfacenode on first call.
    // Only effective when IsLevelAtLeast(Full).
    // detail: UniRenderThread -> uniRenderThreadDetailStats_, MainThread -> mainThreadDetailStats_
    void IncrementBySurfaceNode(const std::string& surfaceNodeName, const std::string& counterName,
        uint64_t value = 1, FrameStatsDetail detail = FrameStatsDetail::UniRenderThread);

    // Increment a per-surfacenode drawable slot counter. Converts slot to string name internally.
    // Only effective when IsLevelAtLeast(Full).
    void IncrementDrawableBySurfaceNode(const std::string& surfaceNodeName, int8_t slot, uint64_t value = 1);

    // Get a snapshot of all counter values.
    // Thread-safe. Returns entries in registration order
    std::vector<FrameStatsEntry> GetFrameStats() const;

    // Get a single counter value. Returns 0 if out of range.
    uint64_t GetCounterValue(size_t counter) const;

    // Reset all counters to 0. Thread-safe.
    void ResetFrameStats();

    // Dump all counters to a formatted string.
    // Includes current foreground app and per-app history.
    std::string DumpFrameStats() const;

    // Set the current foreground app from a vector of "pkgName:pid:appType" format params.
    // When the foreground app changes, current counters are snapshotted into
    // per-app history before resetting. Thread-safe.
    void SetForegroundApp(const std::vector<std::string>& params);

    // Get the current foreground app name. Returns empty string if not set.
    std::string GetForegroundApp() const;

    // Get a snapshot of counter values for a specific app from history.
    // Returns empty vector if the app has no recorded history.
    std::vector<FrameStatsEntry> GetFrameStatsByApp(const std::string& appName) const;

private:
    RSFrameStatsCollection();
    ~RSFrameStatsCollection() noexcept;
    RSFrameStatsCollection(const RSFrameStatsCollection&) = delete;
    RSFrameStatsCollection(const RSFrameStatsCollection&&) = delete;
    RSFrameStatsCollection& operator=(const RSFrameStatsCollection&) = delete;
    RSFrameStatsCollection& operator=(const RSFrameStatsCollection&&) = delete;

    void InitCounterNames();
    static std::string ParsePkgName(const std::vector<std::string>& params);
    void SnapshotAndResetCountersLocked(const std::string& AppName);

    static const FrameStatsLevel enabledLevel_;

    std::vector<std::string> counterNames_;
    std::vector<std::atomic<uint64_t>*> counters_;
    mutable std::mutex registrationMtx_;

    // Per-surfacenode UniRenderThread detail counters (Full level only): surfacenodeName->{counterName->value}
    std::map<std::string, std::map<std::string, uint64_t>> uniRenderThreadDetailStats_;
    // Per-surfacenode MainThread detail counters (Full level only)
    std::map<std::string, std::map<std::string, uint64_t>> mainThreadDetailStats_;

    std::string foregroundApp_;
    std::map<std::string, AppFrameStats> appStatsHistory_;
};
} // namespace Rosen
} // namespace OHOS
#endif