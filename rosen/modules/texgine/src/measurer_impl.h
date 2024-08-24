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

#ifndef ROSEN_MODULES_TEXGINE_SRC_MEASURER_IMPL_H
#define ROSEN_MODULES_TEXGINE_SRC_MEASURER_IMPL_H

#include <iomanip>
#include <list>
#include <queue>
#include <mutex>

#include <hb.h>
#include <hb-icu.h>
#include <unicode/utf16.h>
#include <unicode/uchar.h>

#include "measurer.h"

namespace OHOS {
namespace Rosen {
namespace TextEngine {
struct MeasuringRun {
    size_t start;
    size_t end;
    hb_script_t script = HB_SCRIPT_INVALID;
    std::shared_ptr<Typeface> typeface = nullptr;
};

class MeasurerImpl : public Measurer {
public:
    MeasurerImpl(const std::vector<uint16_t> &text, const FontCollection &fontCollection);

    /*
     * @brief Get word boundary of text
     */
    const std::vector<Boundary> &GetWordBoundary() const override;

    /*
     * @brief Measure font
     * @param cgs The output parameter, after measurer will generate char groups
     * @return 0 is measurer successed
     *         1 is measurer failed
     */
    int Measure(CharGroups &cgs) override;

    /*
     * @brief Seeks typeface for text, this should be private, now is for UT testing
     * @param runs Input and output parameter, intermediate products of measurement
     */
    void SeekTypeface(std::list<struct MeasuringRun> &runs);

    /*
     * @brief Seeks script for text, this should be private, now is for UT testing
     * @param runs Input and output parameter, intermediate products of measurement
     */
    void SeekScript(std::list<struct MeasuringRun> &runs);

    /*
     * @brief Text shaping, after this, information such as the width and height of the
     *         text will be obtain. This should be private, now is for UT testing
     * @param cgs The output parameter, char groups of text
     * @param runs Input and output parameter, intermediate products of measurement
     * @param boundary The word boundarys of text
     * @return 0 is shape successed
     *         1 is shape failed
     */
    int Shape(CharGroups &cgs, std::list<struct MeasuringRun> &runs, std::vector<Boundary> boundaries);

    /*
     * @brief Generate font features required for shaping.
     *        This should be private, now is for UT testing.
     * @param fontFeatures The output parameter. Will passed to harfbuzz
     * @param ff Font features user want
     */
    void GenerateHBFeatures(std::vector<hb_feature_t> &fontFeatures, const FontFeatures* ff);

private:
    struct MeasurerCacheKey {
        std::vector<uint16_t> text = {};
        FontStyles style;
        std::string locale = "";
        bool rtl = false;
        double size = 16.0; // default TextStyle fontSize_
        size_t startIndex = 0;
        size_t endIndex = 0;
        double letterSpacing = 0;
        double wordSpacing = 0;

        bool operator <(const struct MeasurerCacheKey &rhs) const
        {
            if (startIndex != rhs.startIndex) {
                return startIndex < rhs.startIndex;
            }
            if (endIndex != rhs.endIndex) {
                return endIndex < rhs.endIndex;
            }
            if (text != rhs.text) {
                return text < rhs.text;
            }
            if (rtl != rhs.rtl) {
                return rtl < rhs.rtl;
            }
            if (size != rhs.size) {
                return size < rhs.size;
            }
            if (style != rhs.style) {
                return style < rhs.style;
            }
            if (locale != rhs.locale) {
                return locale < rhs.locale;
            }
            if (letterSpacing != rhs.letterSpacing) {
                return letterSpacing < rhs.letterSpacing;
            }
            if (wordSpacing != rhs.wordSpacing) {
                return wordSpacing < rhs.wordSpacing;
            }
            return false;
        }
    };
    struct MeasurerCacheVal {
        CharGroups cgs;
        std::vector<Boundary> boundaries = {};
    };

    void DoSeekScript(std::list<struct MeasuringRun> &runs, hb_unicode_funcs_t* icuGetUnicodeFuncs);
    int DoShape(CharGroups &cgs, MeasuringRun &run, size_t &index);
    int GetGlyphs(CharGroups &cgs, MeasuringRun &run, size_t &index, hb_buffer_t* hbuffer,
        std::shared_ptr<TextEngine::Typeface> typeface);
    void DoCgsByCluster(std::map<uint32_t, TextEngine::CharGroup> &cgsByCluster);
    void HbDestroy(hb_buffer_t* hbuffer, hb_font_t* hfont, hb_face_t* hface, hb_unicode_funcs_t* icuGetUnicodeFuncs);
    void UpdateCache();
    void GetInitKey(struct MeasurerCacheKey &key) const;
    static inline std::mutex mutex_;
    static inline std::map<struct MeasurerCacheKey, struct MeasurerCacheVal> cache_;
    std::vector<Boundary> boundaries_ = {};
};

hb_blob_t* HbFaceReferenceTableTypeface(hb_face_t* face, hb_tag_t tag, void* context);
} // namespace TextEngine
} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_MODULES_TEXGINE_SRC_MEASURER_IMPL_H