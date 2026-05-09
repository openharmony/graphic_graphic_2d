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

#ifndef FONT_MGR_H
#define FONT_MGR_H

#include <memory>
#include <cstdint>
#include <vector>

#include "impl_interface/font_mgr_impl.h"
#include "text/font_style.h"
#include "text/font_style_set.h"
#include "text/typeface.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
enum FontCheckCode {
    SUCCESSED                  = 0, /** no error */
    ERROR_PARSE_CONFIG_FAILED  = 1, /** failed to parse the JSON configuration file */
    ERROR_TYPE_OTHER           = 2  /** other reasons, such as empty input parameters or other internal reasons */
};

struct FontByteArray {
public:
    FontByteArray(std::unique_ptr<uint8_t[]> data, uint32_t dataLen)
        : strData(std::move(data)), strLen(dataLen) {}
    std::unique_ptr<uint8_t[]> strData; // A byte array in UTF-16BE encoding
    uint32_t strLen;
};

struct FontFallbackInfo {
    std::shared_ptr<Typeface> typeface;
    std::vector<uint16_t> glyphIds;
};

class DRAWING_API FontMgr {
public:
    explicit FontMgr(std::shared_ptr<FontMgrImpl> fontMgrImpl) noexcept;
    virtual ~FontMgr() = default;

    /**
     * @brief   Create a default fontMgr.
     * @return  A shared pointer to default fontMgr.
     */
    static std::shared_ptr<FontMgr> CreateDefaultFontMgr();

    /**
     * @brief   Create a dynamic fontMgr.
     * @return  A shared pointer to dynamic fontMgr.
     */
    static std::shared_ptr<FontMgr> CreateDynamicFontMgr();

    /**
     * @brief             Load dynamic font typeface.
     * @param familyName  Font family name.
     * @param data        Font data.
     * @param dataLength  The size of font data.
     * @return  A pointer to typeface.
     */
    Typeface* LoadDynamicFont(const std::string& familyName, const uint8_t* data, size_t dataLength);

    /**
     * @brief             Load dynamic font typeface.
     * @param familyName  Font family name.
     * @param typeface    Typeface
     * @return whether the dynamic font was successfully loaded
    */
    bool LoadDynamicFont(const std::string& familyName, std::shared_ptr<Drawing::Typeface> typeface);

    /**
     * @brief             Load theme font typeface.
     * @param familyName  Font family name.
     * @param themeName   Theme name.
     * @param data        Font data.
     * @param dataLength  The size of font data.
     * @return  A pointer to typeface.
     */
    Typeface* LoadThemeFont(const std::string& familyName, const std::string& themeName,
        const uint8_t* data, size_t dataLength);

    /*
     * @brief             Load theme font typeface.
     * @param themeName   Theme name.
     * @param typeface    Typeface
    */
    void LoadThemeFont(const std::string& themeName, std::shared_ptr<Drawing::Typeface> typeface);

    /**
     * @brief             Use the system fallback to find a typeface for the given character.
     * @param familyName  A const char array of familyName.
     * @param fontStyle   FontStyle.
     * @param bcp47       Is a combination of ISO 639, 15924, and 3166-1 codes.
     * @param bcp47Count  Length of bcp47.
     * @param character   The given character.
     * @return            If find, return typeface. else, return nullptr.
     */
    Typeface* MatchFamilyStyleCharacter(const char familyName[], const FontStyle& fontStyle,
                                        const char* bcp47[], int bcp47Count,
                                        int32_t character) const;

    /**
     * @brief             Resolve typeface and glyph IDs for each codepoint in a string.
     *
     * Two-phase matching strategy:
     *   Phase 1 (prior typeface): If @p prior is provided, every codepoint is probed
     *            with it via Font::UnicharToGlyph.  Codepoints that produce a non-zero
     *            glyph ID are assigned to the prior typeface immediately.
     *   Phase 2 (system fallback): Remaining unmatched codepoints are resolved through
     *            MatchFamilyStyleCharacter.  Each newly discovered typeface is also used
     *            to probe the remaining unmatched codepoints (batch optimisation), so
     *            that one MatchFamilyStyleCharacter call can cover many codepoints.
     *
     * Codepoints that remain unmatched after both phases appear in the output as a run
     * with typeface == nullptr and glyphId == 0.
     *
     * The output is a vector of FontFallbackInfo, where consecutive codepoints that
     * share the same typeface (including nullptr) are merged into a single run.
     *
     * @param codepoints  UTF-32 codepoints (UCS-4, one codepoint per int32_t).
     * @param prior       A preferred typeface to try first for all codepoints.
     *                    May be nullptr, in which case Phase 1 is skipped entirely.
     * @param fontStyle   Font style used for system fallback matching.
     * @param bcp47       BCP 47 locale tags (e.g. { "zh-Hant" }) that influence
     *                    which fallback font is chosen for locale-dependent scripts.
     * @param familyName  Font family name hint passed to MatchFamilyStyleCharacter,
     *                    or nullptr for the system default.
     * @return            A vector of FontFallbackInfo runs ordered by position.
     *                    Each run contains a typeface (may be nullptr for unmatched
     *                    codepoints) and the corresponding glyph IDs (0 if unmatched).
     *                    Returns an empty vector when @p codepoints is empty or
     *                    the font manager is not initialised.
     */
    std::vector<FontFallbackInfo> GetFallbacksForString(const std::vector<int32_t>& codepoints,
        const std::shared_ptr<Typeface>& prior = nullptr, const FontStyle& fontStyle = {},
        const std::vector<std::string>& bcp47 = {}, const char* familyName = nullptr) const;

    /**
     * @brief             Find a fontStyleSet for the given familyName.
     * @param familyName  A const char array of familyName.
     * @return            If find, return fontStyleSet. else, return nullptr.
     */
    FontStyleSet* MatchFamily(const char familyName[]) const;

    template<typename T>
    T* GetImpl() const
    {
        if (fontMgrImpl_) {
            return fontMgrImpl_->DowncastingTo<T>();
        }
        return nullptr;
    }

    /**
     * @brief             Find the corresponding font based on the style and font name
     * @param familyName  The name of the font you want to apply
     * @param fontStyle   The font style you want to achieve
     * @return            Returns the corresponding font
     */
    Typeface* MatchFamilyStyle(const char familyName[], const FontStyle& fontStyle) const;

    int CountFamilies() const;

    void GetFamilyName(int index, std::string& str) const;

    FontStyleSet* CreateStyleSet(int index) const;

    /**
     * @brief             Get the fullname of font
     * @param fontFd      The file descriptor for the font file
     * @param fullnameVec Read the font fullname list
     * @return            Returns Whether the fullnameVec was successfully obtained, 0 means success,
     *                    see FontCheckCode for details
     */
    int GetFontFullName(int fontFd, std::vector<FontByteArray>& fullnameVec);

    /**
     * @brief             Parse the Installed font configuration file and get the font path list
     * @param configPath  The path to the configuration file
     * @param fontPathVec Read a list of font file paths
     * @return            Returns Whether the configuration file is parsed successfully, 0 means success,
     *                    see FontCheckCode for details
     */
    int ParseInstallFontConfig(const std::string& configPath, std::vector<std::string>& fontPathVec);
private:
    std::shared_ptr<FontMgrImpl> fontMgrImpl_;
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif