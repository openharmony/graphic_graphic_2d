/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "rs_skia_memory_tracer.h"
#include <numeric>
namespace OHOS::Rosen {
constexpr uint32_t MEMUNIT_RATE = 1024;

SkiaMemoryTracer::SkiaMemoryTracer(const std::vector<ResourcePair>& resourceMap, bool itemizeType)
    : resourceMap_(resourceMap), itemizeType_(itemizeType), totalSize_("bytes", 0), purgeableSize_("bytes", 0)
{}

SkiaMemoryTracer::SkiaMemoryTracer(const char* categoryKey, bool itemizeType)
    : categoryKey_(categoryKey), itemizeType_(itemizeType), totalSize_("bytes", 0), purgeableSize_("bytes", 0)
{}

std::string SkiaMemoryTracer::SourceType2String(SOURCETYPE type)
{
    std::string sourceType;
    switch (type) {
        case SOURCE_OTHER :
            sourceType = "source_other";
            break;
        case SOURCE_RSCUSTOMMODIFIERDRAWABLE :
            sourceType = "source_rscustommodifierdrawable";
            break;
        case SOURCE_RSBEGINBLENDERDRAWABLE :
            sourceType = "source_rsbeginblenderdrawable";
            break;
        case SOURCE_RSSHADOWDRAWABLE :
            sourceType = "source_rsshadowdrawable";
            break;
        case SOURCE_RSBACKGROUNDIMAGEDRAWABLE :
            sourceType = "source_rsbackgroundimagedrawable";
            break;
        case SOURCE_RSUSEEFFECTDRAWABLE :
            sourceType = "source_rsuseeffectdrawable";
            break;
        case SOURCE_RSDYNAMICLIGHTUPDRAWABLE :
            sourceType = "source_rsdynamiclightupdrawable";
            break;
        case SOURCE_RSBINARIZATIONDRAWABLE :
            sourceType = "source_rsbinarizationdrawable";
            break;
        case SOURCE_RSCOLORFILTERDRAWABLE :
            sourceType = "source_rscolorfilterdrawable";
            break;
        case SOURCE_RSLIGHTUPEFFECTDRAWABLE :
            sourceType = "source_rslightupeffectdrawable";
            break;
        case SOURCE_RSDYNAMICDIMDRAWABLE :
            sourceType = "source_rsdynamicdimdrawable";
            break;
        case SOURCE_RSFOREGROUNDFILTERDRAWABLE :
            sourceType = "source_rsforegroundfilterdrawable";
            break;
        case SOURCE_RSFOREGROUNDFILTERRESTOREDRAWABLE :
            sourceType = "source_rsforegroundfilterrestoredrawable";
            break;
        case SOURCE_RSPIXELSTRETCHDRAWABLE :
            sourceType = "source_rspixelstretchdrawable";
            break;
        case SOURCE_RSPOINTLIGHTDRAWABLE :
            sourceType = "source_rspointlightdrawable";
            break;
        case SOURCE_RSPROPERTYDRAWABLE :
            sourceType = "source_rspropertydrawable";
            break;
        case SOURCE_RSFILTERDRAWABLE :
            sourceType = "source_rsfilterdrawable";
            break;
        case SOURCE_FINISHOFFSCREENRENDER :
            sourceType = "source_finishoffscreenrender";
            break;
        case SOURCE_DRAWSELFDRAWINGNODEBUFFER :
            sourceType = "source_drawselfdrawingnodebuffer";
            break;
        case SOURCE_ONCAPTURE :
            sourceType = "source_oncapture";
            break;
        case SOURCE_INITCACHEDSURFACE :
            sourceType = "source_initcachedsurface";
            break;
        default :
            sourceType = "";
            break;
    }
    return sourceType;
}

const char* SkiaMemoryTracer::MapName(const char* resourceName)
{
    for (auto& resource : resourceMap_) {
        if (SkStrContains(resourceName, resource.first)) {
            return resource.second;
        }
    }
    return nullptr;
}

void SkiaMemoryTracer::ProcessElement()
{
    if (!currentElement_.empty()) {
        // Only count elements that contain "size", other values just provide metadata.
        auto sizeResult = currentValues_.find("size");
        if (sizeResult != currentValues_.end()) {
            totalSize_.value += sizeResult->second.value;
            totalSize_.count++;
        } else {
            currentElement_.clear();
            currentValues_.clear();
            return;
        }

        // find the purgeable size if one exists
        auto purgeableResult = currentValues_.find("purgeable_size");
        if (purgeableResult != currentValues_.end()) {
            purgeableSize_.value += purgeableResult->second.value;
            purgeableSize_.count++;
        }

        // find the type if one exists
        std::string type;
        auto typeResult = currentValues_.find("type");
        if (typeResult != currentValues_.end()) {
            type = typeResult->second.units.c_str();
        } else if (itemizeType_) {
            type = "Other";
        } else {
            type = "";
        }

        // compute the type if we are itemizing or use the default "size" if we are not
        std::string key = (itemizeType_) ? type : sizeResult->first;

        // compute the top level element name using either the map or category key
        const char* resourceName = MapName(currentElement_.c_str());
        if (categoryKey_ != nullptr) {
            // find the category if one exists
            auto categoryResult = currentValues_.find(categoryKey_);
            if (categoryResult != currentValues_.end()) {
                resourceName = categoryResult->second.units.c_str();
            }
        }

        // if we don't have a resource name then we don't know how to label the
        // data and should abort.
        if (resourceName == nullptr) {
            resourceName = currentElement_.c_str();
        }

        auto result = results_.find(resourceName);
        if (result == results_.end()) {
            std::string strResourceName = resourceName;
            TraceValue sizeValue = sizeResult->second;
            std::unordered_map<std::string, TraceValue> typeItem{{key, sizeValue}};
            results_.insert({ strResourceName, typeItem });
        } else {
            auto& resourceValues = result->second;
            typeResult = resourceValues.find(key);
            if (typeResult == resourceValues.end()) {
                resourceValues.insert({ key, sizeResult->second });
            } else {
                typeResult->second.value += sizeResult->second.value;
                typeResult->second.count++;
            }
        }

        // find the source if one exists
        std::string sourceType;
        auto sourceIdResult = currentValues_.find("source");
        if (sourceIdResult != currentValues_.end()) {
            sourceType =
                SourceType2String(static_cast<SOURCETYPE>(static_cast<uint32_t>(sourceIdResult->second.value)));
        } else if (itemizeType_) {
            sourceType = "source_other";
        } else {
            sourceType = "";
        }

        std::string strResourceName = resourceName; // current tag
        TraceValue sizeValue = sizeResult->second; // current size info
        auto sourceResult = sourceTagResults_.find(sourceType);
        if (sourceResult == sourceTagResults_.end()) { // 1. add new source item
            std::unordered_map<std::string, TraceValue> typeItem{{key, sizeValue}};
            std::unordered_map<std::string,
                std::unordered_map<std::string, TraceValue>> tagItem{{strResourceName, typeItem}};
            sourceTagResults_.insert({sourceType, tagItem});
        } else { // 2.update source item
            auto& sourceValues = sourceResult->second;
            auto tagResult = sourceValues.find(strResourceName);
            if (tagResult == sourceValues.end()) { // 2.1 add new tag item in current source
                std::unordered_map<std::string, TraceValue> typeItem{{key, sizeValue}};
                sourceResult->second.insert({strResourceName, typeItem});
            } else { // 2.2 update tag item in current source
                auto& typeValues = tagResult->second;
                typeResult = typeValues.find(key);
                if (typeResult == typeValues.end()) { // 2.2.1 add new type item in current tag
                    typeValues.insert({key, sizeValue});
                } else { // 2.2.2 update type item in current tag
                    typeResult->second.value += sizeValue.value;
                    typeResult->second.count++;
                }
            }
        }
    }

    currentElement_.clear();
    currentValues_.clear();
}

void SkiaMemoryTracer::dumpNumericValue(const char* dumpName, const char* valueName, const char* units, uint64_t value)
{
    if (currentElement_ != dumpName) {
        ProcessElement();
        currentElement_ = dumpName;
    }
    currentValues_.insert({ valueName, { units, value } });
}

void SkiaMemoryTracer::LogOutput(DfxString& log)
{
    // process any remaining elements
    ProcessElement();

    for (const auto& sourceItem: sourceTagResults_) {
        if (itemizeType_) {
            log.AppendFormat("  %s:\n", sourceItem.first.c_str()); // source_xx
            for (const auto& tagItem: sourceItem.second) {
                log.AppendFormat("    %s:\n", tagItem.first.c_str()); // skia/sk_glyph_cache
                for (const auto& typeItem: tagItem.second) {
                    TraceValue traceValue = ConvertUnits(typeItem.second);
                    const char* entry = (traceValue.count > 1) ? "entries" : "entry";
                    log.AppendFormat("      %s: %.2f %s (%d %s)\n", typeItem.first.c_str(), traceValue.value,
                        traceValue.units.c_str(), traceValue.count, entry);
                }
            }
        } else {
            for (const auto& tagItem: sourceItem.second) {
                auto result = tagItem.second.find("size");
                if (result != tagItem.second.end()) {
                    TraceValue traceValue = ConvertUnits(result->second);
                    const char* entry = (traceValue.count > 1) ? "entries" : "entry";
                    log.AppendFormat("  %s: %.2f %s (%d %s)\n", tagItem.first.c_str(), traceValue.value,
                        traceValue.units.c_str(), traceValue.count, entry);
                }
            }
        }
    }
}

float SkiaMemoryTracer::GetGLMemorySize()
{
    ProcessElement();
    // exclude scratch memory
    // cause scratch memory is generated by animation and effect. which is not response by app
    size_t totalGpuSizeApp = 0;
    for (const auto& namedItem : results_) {
        if (namedItem.first == "Scratch" || namedItem.first == "skia/gr_text_blob_cache") {
            continue;
        }
        totalGpuSizeApp = std::accumulate(namedItem.second.begin(), namedItem.second.end(), totalGpuSizeApp,
            [](int total, const auto& typedValue) { return total + typedValue.second.value; });
    }
    return totalGpuSizeApp;
}

float SkiaMemoryTracer::GetGpuMemorySizeInMB()
{
    ProcessElement();
    return ConvertToMB(totalSize_);
}

void SkiaMemoryTracer::LogTotals(DfxString& log)
{
    TraceValue total = ConvertUnits(totalSize_);
    TraceValue purgeable = ConvertUnits(purgeableSize_);
    log.AppendFormat("    %.0f bytes, %.2f %s (%.2f %s is purgeable)\n", totalSize_.value, total.value,
        total.units.c_str(), purgeable.value, purgeable.units.c_str());
}

SkiaMemoryTracer::TraceValue SkiaMemoryTracer::ConvertUnits(const TraceValue& value)
{
    TraceValue output(value);
    if (output.units == SkString("bytes") && output.value >= MEMUNIT_RATE) {
        output.value = output.value / MEMUNIT_RATE;
        output.units = "KB";
    }
    if (output.units == SkString("KB") && output.value >= MEMUNIT_RATE) {
        output.value = output.value / MEMUNIT_RATE;
        output.units = "MB";
    }
    return output;
}

float SkiaMemoryTracer::ConvertToMB(const TraceValue& value)
{
    if (value.units == SkString("bytes")) {
        return value.value / MEMUNIT_RATE / MEMUNIT_RATE;
    }
    if (value.units == SkString("KB")) {
        return value.value / MEMUNIT_RATE;
    }
    return value.value;
}

} // namespace OHOS::Rosen