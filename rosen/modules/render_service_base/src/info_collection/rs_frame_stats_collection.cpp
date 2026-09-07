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

#include "info_collection/rs_frame_stats_collection.h"

#ifdef ROSEN_OHOS
#include <parameters.h>
#endif
#include <sstream>

#include "drawable/rs_drawable.h"

namespace OHOS {
namespace Rosen {

#ifdef ROSEN_OHOS
const FrameStatsLevel RSFrameStatsCollection::enabledLevel_ =
    static_cast<FrameStatsLevel>(OHOS::system::GetIntParameter("persist.sys.graphic.openFrameStats", 0));
#else
const FrameStatsLevel RSFrameStatsCollection::enabledLevel_ = FrameStatsLevel::Disabled;
#endif

namespace {
const char* DrawableSlotToString(int8_t slot)
{
    static const char* names[] = {
        "SAVE_ALL",                    // 0
        "MASK",                        // 1
        "TRANSITION",                  // 2
        "ENV_FOREGROUND_COLOR",        // 3
        "COLOR_PICKER",                // 4
        "SPATIAL_EFFECT",              // 5
        "MATERIAL_FILTER",             // 6
        "SHADOW",                      // 7
        "FOREGROUND_FILTER",           // 8
        "OUTLINE",                     // 9
        "MATERIAL_SHADER",            // 10
        "SAVE_CLIP_TO_BOUNDS",        // 11
        "BG_SAVE_BOUNDS",             // 12
        "CLIP_TO_BOUNDS",             // 13
        "BLENDER",                    // 14
        "BACKGROUND_COLOR",           // 15
        "BACKGROUND_SHADER",          // 16
        "BACKGROUND_NG_SHADER",       // 17
        "BACKGROUND_IMAGE",           // 18
        "BACKGROUND_FILTER",          // 19
        "USE_EFFECT",                 // 20
        "BACKGROUND_STYLE",           // 21
        "DYNAMIC_LIGHT_UP",           // 22
        "ENV_FOREGROUND_COLOR_STRATEGY", // 23
        "BG_RESTORE_BOUNDS",          // 24
        "SAVE_FRAME",                 // 25
        "FRAME_OFFSET",               // 26
        "CLIP_TO_FRAME",              // 27
        "CUSTOM_CLIP_TO_FRAME",       // 28
        "CONTENT_STYLE",              // 29
        "CHILDREN",                   // 30
        "FOREGROUND_STYLE",           // 31
        "RESTORE_FRAME",              // 32
        "FG_SAVE_BOUNDS",             // 33
        "FG_CLIP_TO_BOUNDS",          // 34
        "BINARIZATION",               // 35
        "COLOR_FILTER",               // 36
        "LIGHT_UP_EFFECT",            // 37
        "DYNAMIC_DIM",                // 38
        "COMPOSITING_FILTER",         // 39
        "FOREGROUND_COLOR",           // 40
        "FOREGROUND_SHADER",          // 41
        "FG_RESTORE_BOUNDS",          // 42
        "COVERAGE_NG_SHADER",         // 43
        "BORDER",                     // 44
        "OVERLAY",                    // 45
        "PARTICLE_EFFECT",            // 46
        "PIXEL_STRETCH",              // 47
        "RESTORE_CLIP_TO_BOUNDS",     // 48
        "OVERLAY_NG_SHADER",          // 49
        "RESTORE_BLENDER",            // 50
        "RESTORE_FOREGROUND_FILTER",  // 51
        "RESTORE_ALL",                // 52
    };
    constexpr int8_t count = static_cast<int8_t>(RSDrawableSlot::MAX);
    if (slot >= 0 && slot < count) {
        return names[static_cast<size_t>(slot)];
    }
    return "UNKNOWN_DRAWABLE";
}
} // namespace

RSFrameStatsCollection& RSFrameStatsCollection::GetInstance()
{
    static RSFrameStatsCollection instance;
    return instance;
}

RSFrameStatsCollection::RSFrameStatsCollection()
{
    InitCounterNames();
}

RSFrameStatsCollection::~RSFrameStatsCollection() noexcept
{
    for (auto* counter : counters_) {
        delete counter;
    }
    counters_.clear();
}

void RSFrameStatsCollection::InitCounterNames()
{
    const size_t count = static_cast<size_t>(FrameStatsCounter::Global::COUNT);
    counterNames_.resize(count);
    counters_.resize(count, nullptr);

    for (size_t i = 0; i < count; ++i) {
        counters_[i] = new (std::nothrow) std::atomic<uint64_t>(0);
    }

    // RSMainThread counters
    counterNames_[FrameStatsCounter::ToIndex(FrameStatsCounter::RSMainThread::TotalFrames)] =
        "RSMainThread|TotalFrames";
    counterNames_[FrameStatsCounter::ToIndex(FrameStatsCounter::RSMainThread::DirectCompositionFrames)] =
        "RSMainThread|DirectCompositionFrames";
    counterNames_[FrameStatsCounter::ToIndex(FrameStatsCounter::RSMainThread::NothingToUpdateFrames)] =
        "RSMainThread|NothingToUpdateFrames";
    counterNames_[FrameStatsCounter::ToIndex(FrameStatsCounter::RSMainThread::AnimationRunningEmpty)] =
        "RSMainThread|AnimationRunningEmpty";

    // RSUniRenderThread counters
    counterNames_[FrameStatsCounter::ToIndex(FrameStatsCounter::RSUniRenderThread::TotalFrames)] =
        "RSUniRenderThread|TotalFrames";
    counterNames_[FrameStatsCounter::ToIndex(FrameStatsCounter::RSUniRenderThread::SkipCommitFrames)] =
        "RSUniRenderThread|SkipCommitFrames";
    counterNames_[FrameStatsCounter::ToIndex(FrameStatsCounter::RSUniRenderThread::SkipForceCommitFrames)] =
        "RSUniRenderThread|SkipForceCommitFrames";
    counterNames_[FrameStatsCounter::ToIndex(FrameStatsCounter::RSUniRenderThread::DrawImageGPUCount)] =
        "RSUniRenderThread|DrawImageGPUCount";
    counterNames_[FrameStatsCounter::ToIndex(FrameStatsCounter::RSUniRenderThread::UpdateCacheSurface)] =
        "RSUniRenderThread|UpdateCacheSurface";

    // RSRenderComposer counters
    counterNames_[FrameStatsCounter::ToIndex(FrameStatsCounter::RSRenderComposer::GpuComposeRedrawFrames)] =
        "RSRenderComposer|GpuComposeRedrawFrames";
    counterNames_[FrameStatsCounter::ToIndex(FrameStatsCounter::RSRenderComposer::CommitAndGetReleaseFence)] =
        "RSRenderComposer|CommitAndGetReleaseFence";

    // Per-layer rate counters (dynamic indexing)
    static constexpr uint32_t rates[] = { 10, 15, 20, 24, 30, 36, 40, 45, 48, 60, 72, 80, 90, 120, 144 };
    for (size_t slot = 0; slot < FrameStatsCounter::LAYER_SLOTS; ++slot) {
        // slot 0..3 -> Layers1..4, slot 4 -> Layers5+
        std::string layerLabel = (slot < FrameStatsCounter::LAYER_SLOTS - 1)
            ? "Layers" + std::to_string(slot + 1)
            : "Layers5+";
        for (size_t r = 0; r < FrameStatsCounter::RATE_COUNT; ++r) {
            size_t idx = static_cast<size_t>(FrameStatsCounter::Global::RSRenderComposer_Base)
                + static_cast<size_t>(FrameStatsCounter::RSRenderComposer::PerLayerRate_Begin)
                + slot * FrameStatsCounter::RATE_COUNT + r;
            counterNames_[idx] = "RSRenderComposer|" + layerLabel + "|" + std::to_string(rates[r]) + "Hz";
        }
    }

    // RSSubThread counters
    counterNames_[FrameStatsCounter::ToIndex(FrameStatsCounter::RSSubThread::TotalFrames)] =
        "RSSubThread|TotalFrames";
}

void RSFrameStatsCollection::Increment(size_t counter)
{
    if (counter < counters_.size()) {
        counters_[counter]->fetch_add(1, std::memory_order_relaxed);
    }
}

void RSFrameStatsCollection::IncrementBySurfaceNode(
    const std::string& surfaceNodeName, const std::string& counterName, uint64_t value,
    FrameStatsDetail detail)
{
    if (surfaceNodeName.empty() || counterName.empty()) {
        return;
    }
    std::lock_guard<std::mutex> lock(registrationMtx_);
    if (detail == FrameStatsDetail::MainThread) {
        mainThreadDetailStats_[surfaceNodeName][counterName] += value;
    } else {
        uniRenderThreadDetailStats_[surfaceNodeName][counterName] += value;
    }
}

void RSFrameStatsCollection::IncrementDrawableBySurfaceNode(
    const std::string& surfaceNodeName, int8_t slot, uint64_t value)
{
    if (surfaceNodeName.empty()) {
        return;
    }
    std::lock_guard<std::mutex> lock(registrationMtx_);
    mainThreadDetailStats_[surfaceNodeName][DrawableSlotToString(slot)] += value;
}

std::vector<FrameStatsEntry> RSFrameStatsCollection::GetFrameStats() const
{
    std::lock_guard<std::mutex> lock(registrationMtx_);
    std::vector<FrameStatsEntry> result;
    result.reserve(counterNames_.size());
    for (size_t i = 0; i < counterNames_.size(); ++i) {
        if (counterNames_[i].empty()) {
            continue;
        }
        FrameStatsEntry entry;
        entry.name = counterNames_[i];
        entry.count = counters_[i]->load(std::memory_order_relaxed);
        result.push_back(entry);
    }

    return result;
}

uint64_t RSFrameStatsCollection::GetCounterValue(size_t counter) const
{
    if (counter >= counters_.size()) {
        return 0;
    }
    return counters_[counter]->load(std::memory_order_relaxed);
}

void RSFrameStatsCollection::ResetFrameStats()
{
    std::lock_guard<std::mutex> lock(registrationMtx_);
    for (size_t i = 0; i < counters_.size(); ++i) {
        counters_[i]->store(0, std::memory_order_relaxed);
    }
    uniRenderThreadDetailStats_.clear();
    mainThreadDetailStats_.clear();
    appStatsHistory_.clear();
    foregroundApp_.clear();
}

namespace {
// Dump one section of counters with multi-level pipe grouping.
// Names like "Category|SubCat|Leaf" produce nested [] groups:
//   [Category]
//     [SubCat]
//       Leaf: value
// Counters with value 0 are skipped.
uint64_t ComputeRenderComposerTotal(const std::vector<std::string>& names,
    const std::vector<uint64_t>& values)
{
    uint64_t total = 0;
    for (size_t i = 0; i < names.size(); ++i) {
        if (names[i].empty()) {
            continue;
        }
        // Only sum RSRenderComposer rate counters (names with 3 pipe-separated parts)
        if (names[i].find("RSRenderComposer|") != 0) {
            continue;
        }
        auto sep1 = names[i].find('|');
        if (sep1 == std::string::npos) {
            continue;
        }
        std::string rest = names[i].substr(sep1 + 1);
        auto sep2 = rest.find('|');
        if (sep2 == std::string::npos) {
            continue;
        }
        if (i < values.size()) {
            total += values[i];
        }
    }
    return total;
}

void DumpCounters(std::ostringstream& oss, const std::vector<std::string>& names,
    const std::vector<uint64_t>& values, const std::string& indent)
{
    // Pre-compute RSRenderComposer TotalFrames so it can be printed before the
    // per-layer-rate sub-categories, consistent with RSMainThread/RSUniRenderThread.
    uint64_t renderComposerTotal = ComputeRenderComposerTotal(names, values);
    bool renderComposerTotalPrinted = false;
    std::string currentCategory;
    std::string currentSubCategory;
    for (size_t i = 0; i < names.size(); ++i) {
        if (names[i].empty()) {
            continue;
        }
        if (i >= values.size() || values[i] == 0) {
            continue;
        }
        // Split "Category|SubCategory|Leaf" into parts
        uint64_t val = values[i];
        auto sep1 = names[i].find('|');
        std::string category = (sep1 != std::string::npos) ? names[i].substr(0, sep1) : "(default)";
        std::string rest = (sep1 != std::string::npos) ? names[i].substr(sep1 + 1) : names[i];
        auto sep2 = rest.find('|');
        std::string subCategory = (sep2 != std::string::npos) ? rest.substr(0, sep2) : "";
        std::string leaf = (sep2 != std::string::npos) ? rest.substr(sep2 + 1) : rest;

        // Category changed
        if (category != currentCategory) {
            currentCategory = category;
            currentSubCategory.clear();
            oss << "\n" << indent << "[" << category << "]\n";
            if (category == "RSRenderComposer" && renderComposerTotal > 0) {
                oss << indent << "  TotalFrames: " << renderComposerTotal << "\n";
                renderComposerTotalPrinted = true;
            }
        }
        // Sub-category changed
        if (subCategory != currentSubCategory) {
            currentSubCategory = subCategory;
            if (!subCategory.empty()) {
                oss << indent << " [" << subCategory << "]\n";
            }
        }
        // Print value
        if (subCategory.empty()) {
            oss << indent << "  " << leaf << ": " << val << "\n";
        } else {
            oss << indent << "    " << leaf << ": " << val << "\n";
        }
    }
    // If RSRenderComposer had no sub-category entries, still print TotalFrames after fixed counters
    if (!renderComposerTotalPrinted && renderComposerTotal > 0) {
        if (currentCategory != "RSRenderComposer") {
            oss << "\n" << indent << "[RSRenderComposer]\n";
        }
        oss << indent << "  TotalFrames: " << renderComposerTotal << "\n";
    }
}

bool IsSpecialSurfaceName(const std::string& surfaceNodeName)
{
    static constexpr const char* specialNodes[] = { "ScreenNode", "AnimationTrace" };
    for (const auto* specialNode : specialNodes) {
        if (surfaceNodeName == specialNode) {
            return true;
        }
    }
    return false;
}

void AppendDetailEntries(std::vector<std::string>& names, std::vector<uint64_t>& values,
    const std::map<std::string, std::map<std::string, uint64_t>>& surfaceNodeStats, const char* category)
{
    // Dump special nodes first with direct prefix (no "SurfaceNode:" wrapper)
    for (const auto& [surfaceNodeName, counters] : surfaceNodeStats) {
        if (!IsSpecialSurfaceName(surfaceNodeName)) {
            continue;
        }
        for (const auto& [counterName, val] : counters) {
            names.push_back(std::string(category) + "|" + surfaceNodeName + "|" + counterName);
            values.push_back(val);
        }
    }
    // Dump remaining surfacenodes with "SurfaceNode:" prefix
    for (const auto& [surfaceNodeName, counters] : surfaceNodeStats) {
        if (IsSpecialSurfaceName(surfaceNodeName)) {
            continue;
        }
        for (const auto& [counterName, val] : counters) {
            names.push_back(std::string(category) + "|SurfaceNode:" + surfaceNodeName + "|" + counterName);
            values.push_back(val);
        }
    }
}

void MergeDetailStats(std::map<std::string, std::map<std::string, uint64_t>>& dest,
    std::map<std::string, std::map<std::string, uint64_t>>& src)
{
    for (auto& [surfaceNodeName, counters] : src) {
        auto it = dest.find(surfaceNodeName);
        if (it == dest.end()) {
            dest[surfaceNodeName] = std::move(counters);
        } else {
            for (const auto& [counterName, val] : counters) {
                it->second[counterName] += val;
            }
        }
    }
}

} // namespace

std::string RSFrameStatsCollection::DumpFrameStats() const
{
    // Snapshot data under lock, format string outside lock to minimize lock hold time
    std::vector<std::string> names;
    std::vector<uint64_t> currentValues;
    std::map<std::string, std::map<std::string, uint64_t>> uniRenderThreadDetail;
    std::map<std::string, std::map<std::string, uint64_t>> mainThreadDetail;
    std::string foregroundApp;
    std::map<std::string, AppFrameStats> appStatsHistory;
    {
        std::lock_guard<std::mutex> lock(registrationMtx_);
        names = counterNames_;
        currentValues.resize(counters_.size(), 0);
        for (size_t i = 0; i < counters_.size(); ++i) {
            currentValues[i] = counters_[i]->load(std::memory_order_relaxed);
        }
        if (enabledLevel_ >= FrameStatsLevel::Full) {
            uniRenderThreadDetail = uniRenderThreadDetailStats_;
            mainThreadDetail = mainThreadDetailStats_;
        }
        foregroundApp = foregroundApp_;
        appStatsHistory = appStatsHistory_;
    }
    std::ostringstream oss;
    oss << "\n-- FrameStats --\n";
    oss << "  Level: " << static_cast<int32_t>(enabledLevel_) << "\n";
    // Current foreground app
    oss << "\n  Foreground App: " << (foregroundApp.empty() ? "(unknown)" : foregroundApp) << "\n";
    {
        std::vector<std::string> dumpNames = names;
        std::vector<uint64_t> dumpValues = currentValues;
        if (enabledLevel_ >= FrameStatsLevel::Full) {
            AppendDetailEntries(dumpNames, dumpValues, uniRenderThreadDetail, "RSUniRenderThread-Detail");
            AppendDetailEntries(dumpNames, dumpValues, mainThreadDetail, "RSMainThread-Detail");
        }
        DumpCounters(oss, dumpNames, dumpValues, "    ");
    }
    // Per-app history (current foreground app's history is merged into live counters)
    if (!appStatsHistory.empty()) {
        oss << "\n  -- Per-App History --\n";
        for (const auto& [appName, appStats] : appStatsHistory) {
            oss << "\n  [" << (appName.empty() ? "(unknown)" : appName) << "]\n";
            std::vector<std::string> dumpNames = names;
            std::vector<uint64_t> dumpValues = appStats.fixedCounters;
            if (enabledLevel_ >= FrameStatsLevel::Full) {
                AppendDetailEntries(dumpNames, dumpValues, appStats.uniRenderThreadDetailStats,
                    "RSUniRenderThread-Detail");
                AppendDetailEntries(dumpNames, dumpValues, appStats.mainThreadDetailStats,
                    "RSMainThread-Detail");
            }
            DumpCounters(oss, dumpNames, dumpValues, "    ");
        }
        oss << "  -- End Per-App History --\n";
    }
    // Aggregate all apps: sum current foreground counters + all history
    {
        std::vector<uint64_t> aggregate(currentValues.size(), 0);
        for (size_t i = 0; i < currentValues.size(); ++i) {
            aggregate[i] = currentValues[i];
        }
        for (const auto& [appName, appStats] : appStatsHistory) {
            for (size_t i = 0; i < appStats.fixedCounters.size() && i < aggregate.size(); ++i) {
                aggregate[i] += appStats.fixedCounters[i];
            }
        }
        oss << "\n  -- All Apps Total --\n";
        DumpCounters(oss, names, aggregate, "    ");
        oss << "  -- End All Apps Total --\n";
    }
    oss << "  -- End FrameStats --\n";
    return oss.str();
}

std::string RSFrameStatsCollection::ParsePkgName(const std::vector<std::string>& params)
{
    if (params.empty()) {
        return "";
    }
    // Select the best candidate: filter out sceneboard when multiple entries exist
    std::string selected;
    if (params.size() > 1) {
        for (auto it = params.begin(); it != params.end(); ++it) {
            auto colonPos = it->find(':');
            std::string pkgName = (colonPos != std::string::npos) ? it->substr(0, colonPos) : *it;
            if (pkgName.find("com.ohos.sceneboard") == std::string::npos) {
                selected = *it;
                break;
            }
        }
        if (selected.empty()) {
            selected = params.front();
        }
    } else {
        selected = params.front();
    }
    auto colonPos = selected.find(':');
    if (colonPos == std::string::npos) {
        return selected;
    }
    return selected.substr(0, colonPos);
}

void RSFrameStatsCollection::SnapshotAndResetCountersLocked(const std::string& appName)
{
    if (counters_.empty()) {
        return;
    }
    // Always reset counters
    std::vector<uint64_t> snapshot;
    snapshot.reserve(counters_.size());
    for (size_t i = 0; i < counters_.size(); ++i) {
        snapshot.push_back(counters_[i]->exchange(0, std::memory_order_relaxed));
    }
    // Always reset per-surfacenode counters
    std::map<std::string, std::map<std::string, uint64_t>> uniRenderThreadDetailStats;
    std::map<std::string, std::map<std::string, uint64_t>> mainThreadDetailStats;
    if (enabledLevel_ >= FrameStatsLevel::Full) {
        uniRenderThreadDetailStats = uniRenderThreadDetailStats_;
        uniRenderThreadDetailStats_.clear();
        mainThreadDetailStats = mainThreadDetailStats_;
        mainThreadDetailStats_.clear();
    }
    // Merge into existing history for this app
    auto it = appStatsHistory_.find(appName);
    if (it == appStatsHistory_.end()) {
        AppFrameStats appStats;
        appStats.fixedCounters = std::move(snapshot);
        appStats.uniRenderThreadDetailStats = std::move(uniRenderThreadDetailStats);
        appStats.mainThreadDetailStats = std::move(mainThreadDetailStats);
        appStatsHistory_[appName] = std::move(appStats);
    } else {
        auto& existing = it->second;
        for (size_t i = 0; i < existing.fixedCounters.size() && i < snapshot.size(); ++i) {
            existing.fixedCounters[i] += snapshot[i];
        }
        MergeDetailStats(existing.uniRenderThreadDetailStats, uniRenderThreadDetailStats);
        MergeDetailStats(existing.mainThreadDetailStats, mainThreadDetailStats);
    }
}

void RSFrameStatsCollection::SetForegroundApp(const std::vector<std::string>& params)
{
    std::string newApp = ParsePkgName(params);
    std::lock_guard<std::mutex> lock(registrationMtx_);
    if (foregroundApp_ == newApp) {
        return;
    }
    // Snapshot current counters under old app name, then reset
    SnapshotAndResetCountersLocked(foregroundApp_);
    foregroundApp_ = std::move(newApp);
    // Merge new foreground app's history back into live counters,
    // so the same app only appears in one place (ForegroundApp section).
    auto it = appStatsHistory_.find(foregroundApp_);
    if (it != appStatsHistory_.end()) {
        for (size_t i = 0; i < it->second.fixedCounters.size() && i < counters_.size(); ++i) {
            uint64_t historyVal = it->second.fixedCounters[i];
            if (historyVal > 0) {
                counters_[i]->fetch_add(historyVal, std::memory_order_relaxed);
            }
        }
        if (enabledLevel_ >= FrameStatsLevel::Full) {
            MergeDetailStats(uniRenderThreadDetailStats_, it->second.uniRenderThreadDetailStats);
            MergeDetailStats(mainThreadDetailStats_, it->second.mainThreadDetailStats);
        }
        appStatsHistory_.erase(it);
    }
}

std::string RSFrameStatsCollection::GetForegroundApp() const
{
    std::lock_guard<std::mutex> lock(registrationMtx_);
    return foregroundApp_;
}

std::vector<FrameStatsEntry> RSFrameStatsCollection::GetFrameStatsByApp(
    const std::string& appName) const
{
    std::lock_guard<std::mutex> lock(registrationMtx_);

    auto it = appStatsHistory_.find(appName);
    if (it == appStatsHistory_.end()) {
        return {};
    }
    const auto& appStats = it->second;
    std::vector<FrameStatsEntry> result;
    result.reserve(counterNames_.size());
    for (size_t i = 0; i < counterNames_.size() && i < appStats.fixedCounters.size(); ++i) {
        if (counterNames_[i].empty()) {
            continue;
        }
        result.emplace_back(counterNames_[i], appStats.fixedCounters[i]);
    }
    return result;
}
} // namespace Rosen
} // namespace OHOS