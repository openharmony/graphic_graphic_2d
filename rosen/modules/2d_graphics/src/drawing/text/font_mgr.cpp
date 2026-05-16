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

#include "text/font_mgr.h"

#include <numeric>

#include "impl_factory.h"
#include "impl_interface/font_mgr_impl.h"
#include "text/font.h"
#include "utils/log.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
namespace {
struct Match {
    std::shared_ptr<Typeface> typeface;
    uint16_t glyphId = 0;
};

void ProbePriorTypeface(const std::vector<int32_t>& codepoints, const std::shared_ptr<Typeface>& prior,
    std::vector<Match>& matches, std::vector<size_t>& unmatchedIndices)
{
    if (prior == nullptr) {
        unmatchedIndices.resize(codepoints.size());
        std::iota(unmatchedIndices.begin(), unmatchedIndices.end(), 0);
        return;
    }
    Font font(prior, 0, 0, 0);
    size_t hit = 0;
    for (size_t i = 0; i < codepoints.size(); i++) {
        uint16_t gid = font.UnicharToGlyph(codepoints[i]);
        if (gid != 0) {
            matches[i] = { prior, gid };
            hit += 1;
        } else {
            unmatchedIndices.push_back(i);
        }
    }
    LOGD("Succeed in probing prior typeface, matched %{public}zu/%{public}zu codepoints, typeface=%{public}s",
        hit, codepoints.size(), prior->GetFamilyName().c_str());
}

// Collapse the per-codepoint match list into consecutive runs sharing the same typeface.
// Entries with nullptr typeface are preserved as runs with glyphId == 0.
std::vector<FontFallbackInfo> MergeMatchesToRuns(const std::vector<Match>& matches)
{
    std::vector<FontFallbackInfo> results;
    Typeface* prevRaw = nullptr;
    for (auto& m : matches) {
        Typeface* curRaw = m.typeface.get();
        if (results.empty() || curRaw != prevRaw) {
            results.emplace_back();
            results.back().typeface = m.typeface;
            prevRaw = curRaw;
        }
        results.back().glyphIds.push_back(m.glyphId);
    }
    return results;
}
} // namespace

FontMgr::FontMgr(std::shared_ptr<FontMgrImpl> fontMgrImpl) noexcept : fontMgrImpl_(fontMgrImpl) {}

std::shared_ptr<FontMgr> FontMgr::CreateDefaultFontMgr()
{
    return std::make_shared<FontMgr>(ImplFactory::CreateDefaultFontMgrImpl());
}

std::shared_ptr<FontMgr> FontMgr::CreateDynamicFontMgr()
{
    return std::make_shared<FontMgr>(ImplFactory::CreateDynamicFontMgrImpl());
}

Typeface* FontMgr::LoadDynamicFont(const std::string& familyName, const uint8_t* data, size_t dataLength)
{
    if (fontMgrImpl_) {
        return fontMgrImpl_->LoadDynamicFont(familyName, data, dataLength);
    }
    return nullptr;
}

bool FontMgr::LoadDynamicFont(const std::string& familyName, std::shared_ptr<Typeface> typeface)
{
    if (fontMgrImpl_) {
        return fontMgrImpl_->LoadDynamicFont(familyName, typeface);
    }
    return false;
}

Typeface* FontMgr::LoadThemeFont(const std::string& familyName, const std::string& themeName,
    const uint8_t* data, size_t dataLength)
{
    if (fontMgrImpl_) {
        return fontMgrImpl_->LoadThemeFont(familyName, themeName, data, dataLength);
    }
    return nullptr;
}

void FontMgr::LoadThemeFont(const std::string& themeName, std::shared_ptr<Typeface> typeface)
{
    if (fontMgrImpl_) {
        fontMgrImpl_->LoadThemeFont(themeName, typeface);
    }
    return;
}

Typeface* FontMgr::MatchFamilyStyleCharacter(const char familyName[], const FontStyle& fontStyle,
                                             const char* bcp47[], int bcp47Count,
                                             int32_t character) const
{
    if (fontMgrImpl_) {
        return fontMgrImpl_->MatchFamilyStyleCharacter(familyName, fontStyle, bcp47, bcp47Count, character);
    }
    return nullptr;
}

std::vector<FontFallbackInfo> FontMgr::GetFallbacksForString(const std::vector<int32_t>& codepoints,
    const std::shared_ptr<Typeface>& prior, const FontStyle& fontStyle, const std::vector<std::string>& bcp47,
    const char* familyName) const
{
    if (fontMgrImpl_ == nullptr) {
        LOGE("Failed to get fallbacks for string, fontMgrImpl_ is null");
        return {};
    }
    if (codepoints.empty()) {
        return {};
    }

    std::vector<Match> matches(codepoints.size());
    // Phase 1: probe every codepoint with the caller-supplied typeface.
    std::vector<size_t> unmatchedIndices;
    unmatchedIndices.reserve(codepoints.size());
    ProbePriorTypeface(codepoints, prior, matches, unmatchedIndices);

    // Phase 2: system fallback via MatchFamilyStyleCharacter for remaining codepoints.
    std::vector<const char*> bcp47Ptrs(bcp47.size());
    std::transform(bcp47.begin(), bcp47.end(), bcp47Ptrs.begin(), [](const std::string& s) { return s.c_str(); });

    size_t next = 0;
    while (next < unmatchedIndices.size()) {
        size_t idx = unmatchedIndices[next++];
        Typeface* rawTp = fontMgrImpl_->MatchFamilyStyleCharacter(familyName, fontStyle,
            bcp47Ptrs.empty() ? nullptr : bcp47Ptrs.data(), static_cast<int>(bcp47Ptrs.size()), codepoints[idx]);
        if (rawTp == nullptr) {
            LOGW("Failed to find typeface for codepoint U+%04X", codepoints[idx]);
            continue;
        }
        auto matchedTf = std::shared_ptr<Typeface>(rawTp);
        Font font(matchedTf, 0, 0, 0);
        uint16_t gid = font.UnicharToGlyph(codepoints[idx]);
        matches[idx] = gid != 0 ? Match { matchedTf, gid } : Match { nullptr, 0 };

        size_t write = next;
        for (size_t i = next; i < unmatchedIndices.size(); i++) {
            gid = font.UnicharToGlyph(codepoints[unmatchedIndices[i]]);
            if (gid != 0) {
                matches[unmatchedIndices[i]] = { matchedTf, gid };
            } else {
                unmatchedIndices[write++] = unmatchedIndices[i];
            }
        }
        unmatchedIndices.resize(write);
    }
    return MergeMatchesToRuns(matches);
}

FontStyleSet* FontMgr::MatchFamily(const char familyName[]) const
{
    if (fontMgrImpl_) {
        return fontMgrImpl_->MatchFamily(familyName);
    }
    return nullptr;
}

Typeface* FontMgr::MatchFamilyStyle(const char familyName[], const FontStyle& fontStyle) const
{
    if (fontMgrImpl_) {
        return fontMgrImpl_->MatchFamilyStyle(familyName, fontStyle);
    }
    return nullptr;
}

int FontMgr::CountFamilies() const
{
    if (fontMgrImpl_ == nullptr) {
        return 0;
    }
    return fontMgrImpl_->CountFamilies();
}

void FontMgr::GetFamilyName(int index, std::string& str) const
{
    if (fontMgrImpl_ == nullptr) {
        return;
    }
    fontMgrImpl_->GetFamilyName(index, str);
}

FontStyleSet* FontMgr::CreateStyleSet(int index) const
{
    if (fontMgrImpl_ == nullptr) {
        return nullptr;
    }
    return fontMgrImpl_->CreateStyleSet(index);
}

int FontMgr::GetFontFullName(int fontFd, std::vector<FontByteArray>& fullnameVec)
{
    if (fontMgrImpl_ == nullptr) {
        return ERROR_TYPE_OTHER;
    }
    return fontMgrImpl_->GetFontFullName(fontFd, fullnameVec);
};

int FontMgr::ParseInstallFontConfig(const std::string& configPath, std::vector<std::string>& fontPathVec)
{
    if (fontMgrImpl_ == nullptr) {
        return ERROR_TYPE_OTHER;
    }
    return fontMgrImpl_->ParseInstallFontConfig(configPath, fontPathVec);
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
