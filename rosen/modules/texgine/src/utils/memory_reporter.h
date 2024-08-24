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

#ifndef ROSEN_MODULES_TEXGINE_SRC_UTILS_MEMORY_REPORTER_H
#define ROSEN_MODULES_TEXGINE_SRC_UTILS_MEMORY_REPORTER_H

#include <cstring>
#include <map>
#include <memory>
#include <set>
#include <string>
#include <vector>

#include "texgine/typography.h"
#include "texgine/utils/memory_usage_scope.h"
#include "variant_font_style_set.h"
#include "variant_span.h"

struct hb_blob_t;

namespace OHOS {
namespace Rosen {
namespace TextEngine {
#define DECLARE_CLASS_RMU(type) \
    void type::ReportMemoryUsage(const std::string &member, bool needThis) const

#define DECLARE_RMU(type) \
    void ReportMemoryUsage(const std::string &member, type const &that, bool needThis)

#define MEMORY_USAGE_REPORT(member) \
    OHOS::Rosen::TextEngine::ReportMemoryUsage(#member, currentVal.member, false)

#define MEMORY_USAGE_SCOPE(classname, var) \
    MemoryUsageScope s(std::string(classname) + " " + member); \
    if (needThis) { DoReportMemoryUsage("*this", sizeof(var)); } \
    [[maybe_unused]] const auto &currentVal = (var)

template<class T>
std::string GetTypeName()
{
    std::string nameT = __PRETTY_FUNCTION__;
    nameT = nameT.substr(nameT.find("T = "));
    nameT = nameT.substr(strlen("T = "),
        nameT.length() - strlen("T = ") - 1 >= 0 ? nameT.length() - strlen("T = ") - 1 >= 0 : 0);
    constexpr const char *str = "std::variant<std::shared_ptr<TextSpan>, std::shared_ptr<AnySpan> >";
    auto it = nameT.find(str);
    if (it != std::string::npos) {
        nameT = nameT.replace(it, strlen(str), "VariantSpan");
    }
    return nameT;
}

template<class T>
void ReportMemoryUsage(const std::string &member, const T &t, bool needThis = true);
template<class T>
void ReportMemoryUsage(const std::string &member, const std::vector<T> &vec, bool needThis = true);
template<class T>
void ReportMemoryUsage(const std::string &member, const std::shared_ptr<T> &sp, bool needThis = true);
template<class T>
void ReportMemoryUsage(const std::string &member, const std::unique_ptr<T> &up, bool needThis = true);
template<class K, class V>
void ReportMemoryUsage(const std::string &member, const std::map<K, V> &m, bool needThis = true);

struct StyledSpan;
class CmapParser;
class Ranges;
class Typeface;
class CharGroups;
class FontFeatures;

DECLARE_RMU(std::string);
DECLARE_RMU(struct LineMetrics);
DECLARE_RMU(struct TypographyStyle);
DECLARE_RMU(VariantSpan);
DECLARE_RMU(struct StyledSpan);
DECLARE_RMU(AnySpan);
DECLARE_RMU(FontProviders);
DECLARE_RMU(IFontProvider);
DECLARE_RMU(TextSpan);
DECLARE_RMU(Typeface);
DECLARE_RMU(hb_blob_t *);
DECLARE_RMU(CharGroups);
DECLARE_RMU(struct TextStyle);
DECLARE_RMU(CmapParser);
DECLARE_RMU(FontFeatures);
DECLARE_RMU(Ranges);

#ifdef TEXGINE_ENABLE_MEMORY
#define DEFINE_RMU(type) \
    template<> \
    DECLARE_RMU(type) \
    { \
        DoReportMemoryUsage(member, sizeof(that)); \
    }

DEFINE_RMU(uint16_t);
DEFINE_RMU(uint32_t);
DEFINE_RMU(int32_t);
DEFINE_RMU(double);
DEFINE_RMU(VariantFontStyleSet);
DEFINE_RMU(struct TextShadow);
DEFINE_RMU(struct Glyph);
#endif

template<class T>
void ReportMemoryUsage(const std::string &member, const T &t, bool needThis)
{
    DoReportMemoryUsage(member, sizeof(t));
}

template<class T>
void ReportMemoryUsage(const std::string &member, const std::vector<T> &that, bool needThis)
{
    auto nameT = GetTypeName<T>();
    MEMORY_USAGE_SCOPE("v<" + nameT + ">", that);
    // 16 is the vector`s base memory footprint
    int baseMemory = 16;
    if (that.capacity() != 0) {
        DoReportMemoryUsage("*external", baseMemory + sizeof(T) * (that.capacity() - that.size()));
        for (size_t i = 0; i < that.size(); i++) {
            ReportMemoryUsage(member + "[?]", that[i], true);
        }
    }
}

template<class T>
void ReportMemoryUsage(const std::string &member, const std::shared_ptr<T> &that, bool needThis)
{
    static std::set<void *> ptrs;
    if (ptrs.find((void *)(that.get())) != ptrs.end()) {
        return;
    }
    ptrs.insert((void *)(that.get()));

    auto nameT = GetTypeName<T>();
    if (that != nullptr) {
        ReportMemoryUsage(member, *that, true);
    }
}

template<class T>
void ReportMemoryUsage(const std::string &member, const std::unique_ptr<T> &that, bool needThis)
{
    static std::set<void *> ptrs;
    if (ptrs.find((void *)(that.get())) != ptrs.end()) {
        return;
    }
    ptrs.insert((void *)(that.get()));

    auto nameT = GetTypeName<T>();
    if (that != nullptr) {
        ReportMemoryUsage(member, *that, true);
    }
}

template<class K, class V>
void ReportMemoryUsage(const std::string &member, const std::map<K, V> &that, bool needThis)
{
    if (that.size()) {
        auto nameK = GetTypeName<K>();
        auto nameV = GetTypeName<V>();
        MEMORY_USAGE_SCOPE("m<" + nameK + ", " + nameV + ">", that);
        // in std::map, the size of key, if more than 8, it`s base memory footprint is 8 + 40
        // else base memory footprint is 40
        unsigned long memoryDelta = 8;
        unsigned long baseMemory = 40;
        unsigned long sizeLimit = 8;
        DoReportMemoryUsage("*external", static_cast<int>(
            that.size() * ((sizeof(K) < sizeLimit ? 0 : memoryDelta) + baseMemory + sizeof(K) + sizeof(V))));
    }
}
} // namespace TextEngine
} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_MODULES_TEXGINE_SRC_UTILS_MEMORY_REPORTER_H
