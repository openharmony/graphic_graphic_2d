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

#include "utils/memory_reporter.h"

#include <cassert>
#include <mutex>
#include <sstream>
#include <stack>

#include "hb.h"
#include "line_metrics.h"
#include "texgine/any_span.h"
#include "texgine/dynamic_font_provider.h"
#include "texgine/theme_font_provider.h"
#include "texgine/system_font_provider.h"
#include "typeface.h"
#include "typography_impl.h"

namespace OHOS {
namespace Rosen {
namespace TextEngine {
//Enable memory test
#ifdef TEXGINE_ENABLE_MEMORY
class MemoryReporter {
public:
    static MemoryReporter &GetInstance();
    void Enter(const std::string &scopeType);
    void Exit();
    void Report(const std::string &member, double usage);

private:
    MemoryReporter() = default;
    ~MemoryReporter();
    MemoryReporter(const MemoryReporter &) = delete;
    MemoryReporter(MemoryReporter &&) = delete;
    MemoryReporter& operator=(const MemoryReporter &) = delete;
    MemoryReporter& operator=(MemoryReporter &&) = delete;

    uint64_t now_ = 0;
    std::vector<std::pair<uint64_t, std::string>> contents_;
    std::stack<std::string> scopeStack_ = {};
};

MemoryReporter &MemoryReporter::GetInstance()
{
    static MemoryReporter instance;
    return instance;
}

void MemoryReporter::Enter(const std::string &scopeType)
{
    scopeStack_.push(scopeType);
    contents_.push_back(std::make_pair(now_, "B" + scopeStack_.top()));
}

void MemoryReporter::Exit()
{
    scopeStack_.pop();
    contents_.push_back(std::make_pair(now_, "E"));
}

void MemoryReporter::Report(const std::string &member, double usage)
{
    contents_.push_back(std::make_pair(now_, "B" + scopeStack_.top() + "::" + member));
    now_ += usage;
    contents_.push_back(std::make_pair(now_, "Cusage|" + std::to_string(now_)));
    contents_.push_back(std::make_pair(now_, "E"));
}

MemoryReporter::~MemoryReporter()
{
    auto fp = fopen("texgine_usage.trace", "w");
    if (fp == nullptr) {
        return;
    }

    fprintf(fp, "# tracer: nop\n");
    for (const auto &[timeUs, content] : contents_) {
        fprintf(fp, "texgine-1 (1) [000] .... %.6lf: tracing_mark_write: %c|1|%s\n",
                timeUs / 1e6, content[0], content.substr(1).c_str());
    }

    fclose(fp);
}

MemoryUsageScope::MemoryUsageScope(const std::string &name)
{
    MemoryReporter::GetInstance().Enter(name);
}

MemoryUsageScope::~MemoryUsageScope()
{
    MemoryReporter::GetInstance().Exit();
}

void DoReportMemoryUsage(const std::string &name, int usage)
{
    MemoryReporter::GetInstance().Report(name, usage);
}
#else
MemoryUsageScope::MemoryUsageScope(const std::string &name)
{
}

MemoryUsageScope::~MemoryUsageScope()
{
}

void DoReportMemoryUsage(const std::string &name, int usage)
{
}
#endif

DECLARE_RMU(std::string)
{
    /*
     * The capacity of string 16, 24 is the the critical value of string type memory usage
     * If need calculate this pointer:
     * If the capacity of string less than 16, it`s memory usage is 32
     * If the capacity of string more than 16 and less than 24, it`s memory usage is 64
     * If the capacity of string more than 24, it`s memory is 80 + (capacity - 64) / 16 * 16
     * If don`t need calculate this pointer:
     * If the capacity of string >= 16 and < 24, it`s memory usage is 32
     * else it`s memory usage is 32 + (capacity - 24) / 16 * 16
     */
    unsigned long fisrtCapacity = 16;
    unsigned long secondCapacity = 24;
    unsigned long baseMemory = 16;
    unsigned long fisrtMemory = 32;
    unsigned long secondMemory = 64;
    unsigned long thirdMemory = 80;

    if (needThis) {
        if (that.capacity() < fisrtCapacity) {
            DoReportMemoryUsage(member, fisrtMemory);
        } else if (that.capacity() < secondCapacity) {
            DoReportMemoryUsage(member, secondMemory);
        } else {
            DoReportMemoryUsage(member, thirdMemory + (that.capacity() - secondCapacity) / fisrtCapacity * baseMemory);
        }
        return;
    }

    if (that.capacity() >= fisrtCapacity && that.capacity() < secondCapacity) {
        DoReportMemoryUsage(member, fisrtMemory);
    } else {
        DoReportMemoryUsage(member, fisrtMemory + (that.capacity() - secondCapacity) / fisrtCapacity * baseMemory);
    }
}

DECLARE_RMU(struct LineMetrics)
{
    MEMORY_USAGE_SCOPE("LineMetrics", that);
    MEMORY_USAGE_REPORT(lineSpans);
}

DECLARE_RMU(struct TypographyStyle)
{
    MEMORY_USAGE_SCOPE("Typography::Style", that);
    MEMORY_USAGE_REPORT(fontFamilies);
    MEMORY_USAGE_REPORT(ellipsis);
    MEMORY_USAGE_REPORT(lineStyle.fontFamilies);
    MEMORY_USAGE_REPORT(locale);
}

DECLARE_RMU(VariantSpan)
{
    if (auto ts = that.TryToTextSpan(); ts != nullptr) {
        ReportMemoryUsage(member, *ts, needThis);
    }

    if (auto as = that.TryToAnySpan(); as != nullptr) {
        ReportMemoryUsage(member, *as, needThis);
    }
}

DECLARE_RMU(AnySpan)
{
    that.ReportMemoryUsage(member, needThis);
}

DECLARE_CLASS_RMU(AnySpan)
{
    MEMORY_USAGE_SCOPE("AnySpan", *this);
}

DECLARE_RMU(Typography)
{
    that.ReportMemoryUsage(member, needThis);
}

DECLARE_CLASS_RMU(TypographyImpl)
{
    MEMORY_USAGE_SCOPE("TypographyImpl", *this);
    MEMORY_USAGE_REPORT(lineMetrics_);
    MEMORY_USAGE_REPORT(typographyStyle_);
    MEMORY_USAGE_REPORT(spans_);
    MEMORY_USAGE_REPORT(fontProviders_);
}

DECLARE_RMU(FontProviders)
{
    MEMORY_USAGE_SCOPE("FontProviders", that);
    MEMORY_USAGE_REPORT(providers_);
    MEMORY_USAGE_REPORT(fontStyleSetCache_);
}

DECLARE_RMU(IFontProvider)
{
    that.ReportMemoryUsage(member, needThis);
}

DECLARE_CLASS_RMU(IFontProvider)
{
    MEMORY_USAGE_SCOPE("IFontProvider", *this);
}

DECLARE_CLASS_RMU(DynamicFontProvider)
{
    MEMORY_USAGE_SCOPE("DynamicFontProvider", *this);
    MEMORY_USAGE_REPORT(fontStyleSetMap_);
}

DECLARE_CLASS_RMU(ThemeFontProvider)
{
    MEMORY_USAGE_SCOPE("ThemeFontProvider", *this);
    MEMORY_USAGE_REPORT(themeFontStyleSet_);
}

DECLARE_CLASS_RMU(SystemFontProvider)
{
    MEMORY_USAGE_SCOPE("SystemFontProvider", *this);
}

DECLARE_RMU(TextSpan)
{
    MEMORY_USAGE_SCOPE("TextSpan", that);
    MEMORY_USAGE_REPORT(typeface_);
    MEMORY_USAGE_REPORT(u16vect_);
    MEMORY_USAGE_REPORT(glyphWidths_);
    MEMORY_USAGE_REPORT(cgs_);
}

DECLARE_RMU(Typeface)
{
    MEMORY_USAGE_SCOPE("Typeface", that);
    MEMORY_USAGE_REPORT(hblob_);
    MEMORY_USAGE_REPORT(cmapParser_);
}

DECLARE_RMU(hb_blob_t *)
{
    if (that == nullptr) {
        return;
    }
    DoReportMemoryUsage(member, hb_blob_get_length(const_cast<hb_blob_t *>(that)));
}

DECLARE_RMU(CharGroups)
{
    MEMORY_USAGE_SCOPE("CharGroups", that);
    MEMORY_USAGE_REPORT(pcgs_);
}

DECLARE_RMU(struct TextStyle)
{
    MEMORY_USAGE_SCOPE("TextSpan::Style", that);
    MEMORY_USAGE_REPORT(fontFamilies);
    MEMORY_USAGE_REPORT(shadows);
    MEMORY_USAGE_REPORT(fontFeature);
}

DECLARE_RMU(CmapParser)
{
    MEMORY_USAGE_SCOPE("CmapParser", that);
    MEMORY_USAGE_REPORT(ranges_);
}

DECLARE_RMU(CharGroup)
{
    MEMORY_USAGE_SCOPE("CharGroup", that);
    MEMORY_USAGE_REPORT(chars);
    MEMORY_USAGE_REPORT(glyphs);
    MEMORY_USAGE_REPORT(typeface);
}

DECLARE_RMU(FontFeatures)
{
    MEMORY_USAGE_SCOPE("FontFeatures", that);
    MEMORY_USAGE_REPORT(features_);
}

DECLARE_RMU(Ranges)
{
    MEMORY_USAGE_SCOPE("Ranges", that);
    MEMORY_USAGE_REPORT(ranges_);
    MEMORY_USAGE_REPORT(singles_);
}

#ifdef TEXGINE_ENABLE_MEMORY
DEFINE_RMU(struct Ranges::Range);
#endif
} // namespace TextEngine
} // namespace Rosen
} // namespace OHOS
