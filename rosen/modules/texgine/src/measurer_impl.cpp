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

#include "measurer_impl.h"

#include "texgine_exception.h"
#include "texgine/utils/exlog.h"
#ifdef LOGGER_ENABLE_SCOPE
#include "texgine/utils/trace.h"
#endif
#include "text_converter.h"

namespace OHOS {
namespace Rosen {
namespace TextEngine {
#define INVALID_TEXT_LENGTH (-1)
#define SUCCESSED 0
#define FAILED 1
#define POLL_MECHANISM 4999
#define POLL_NUM 1000
constexpr static uint8_t FIRST_BYTE = 24;
constexpr static uint8_t SECOND_BYTE = 16;
constexpr static uint8_t THIRD_BYTE = 8;
static std::string g_detectionName;

namespace {
void DumpCharGroup(int32_t index, const CharGroup &cg, double glyphEm,
    const hb_glyph_info_t &info, const hb_glyph_position_t &position)
{
    auto uTF8Str = TextConverter::ToUTF8(cg.chars);
    // 0xffffff is the default char when the cg is null or invalid
    auto ch = (cg.chars.size() > 0) ? cg.chars[0] : 0xffffff;
    // the follow numbers is to align log
    // 2 & 4 means output width，0 means fill with 0
    LOGEX_FUNC_LINE_DEBUG() << std::setw(2) << std::setfill('0') << index <<
        std::hex << std::uppercase <<
        ": " << std::right << std::setw(4) << std::setfill(' ') << "\033[40m" << "'" <<
        uTF8Str.data() << "'" << "\033[0m" <<
        " (char: 0x" << std::setw(6) << std::setfill('0') << ch <<
        ", codepoint: 0x" << std::setw(4) << std::setfill('0') << info.codepoint <<
        ", cluster: " << std::setw(2) << std::setfill(' ') << std::dec << info.cluster << ")" <<
        " {" << glyphEm * position.x_advance << ", " << glyphEm * position.y_advance <<
        ", " << glyphEm * position.x_offset << ", " << glyphEm * position.y_offset << "}";
}
} // namespace

hb_blob_t* HbFaceReferenceTableTypeface(hb_face_t* face, hb_tag_t tag, void* context)
{
    if (context == nullptr) {
        return nullptr;
    }

    std::shared_ptr<TexgineTypeface> typeface = std::make_shared<TexgineTypeface>(context);
    if (typeface->GetTypeface() == nullptr) {
        return nullptr;
    }

    // 584420 is five times of the tag`s max size now
    const size_t maxSize = 584420;
    const size_t tableSize = typeface->GetTableSize(tag);
    if (tableSize == 0 || tableSize > maxSize) {
        return nullptr;
    }

    void* buffer = malloc(tableSize);
    if (buffer == nullptr) {
        return nullptr;
    }

    size_t actualSize = typeface->GetTableData(tag, 0, tableSize, buffer);
    if (tableSize != actualSize) {
        free(buffer);
        buffer = nullptr;
        return nullptr;
    }

    return hb_blob_create(reinterpret_cast<char*>(buffer),
                          tableSize, HB_MEMORY_MODE_WRITABLE, buffer, free);
}

std::unique_ptr<Measurer> Measurer::Create(const std::vector<uint16_t> &text, const FontCollection &fontCollection)
{
    return std::make_unique<MeasurerImpl>(text, fontCollection);
}

MeasurerImpl::MeasurerImpl(const std::vector<uint16_t> &text, const FontCollection &fontCollection)
    : Measurer(text, fontCollection)
{
}

const std::vector<Boundary> &MeasurerImpl::GetWordBoundary() const
{
    return boundaries_;
}

void MeasurerImpl::UpdateCache()
{
    auto iter = cache_.begin();
    for (size_t index = 0; index < POLL_NUM; index++) {
        cache_.erase(iter++);
    }
}

void MeasurerImpl::GetInitKey(struct MeasurerCacheKey &key) const
{
    key.text = text_;
    key.style = style_;
    key.locale = locale_;
    key.rtl = rtl_;
    key.size = size_;
    key.startIndex = startIndex_;
    key.endIndex = endIndex_;
    key.letterSpacing = letterSpacing_;
    key.wordSpacing = wordSpacing_;
}

int MeasurerImpl::Measure(CharGroups &cgs)
{
#ifdef LOGGER_ENABLE_SCOPE
    ScopedTrace scope("MeasurerImpl::Measure");
#endif
    LOGSCOPED(sl, LOGEX_FUNC_LINE_DEBUG(), "MeasurerImpl::Measure");
    MeasurerCacheKey key;
    GetInitKey(key);
    if (fontFeatures_ == nullptr || fontFeatures_->GetFeatures().size() == 0) {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = cache_.find(key);
        if (it != cache_.end()) {
            cgs = it->second.cgs.Clone();
            boundaries_ = it->second.boundaries;
            if (g_detectionName != cgs.GetTypefaceName()) {
                cache_.erase(key);
            } else if (cache_.size() > POLL_MECHANISM) {
                UpdateCache();
            }
            return SUCCESSED;
        }
    }

    WordBreaker wb;
    wb.SetLocale(icu::Locale::createFromName(locale_.c_str()));
    wb.SetRange(0, text_.size());
    boundaries_ = wb.GetBoundary(text_);

    std::list<struct MeasuringRun> runs;
    SeekScript(runs);
    SeekTypeface(runs);
    if (auto ret = Shape(cgs, runs, boundaries_); ret) {
        LOGEX_FUNC_LINE(ERROR) << "Shape failed";
        return ret;
    }

    if (fontFeatures_ == nullptr || fontFeatures_->GetFeatures().size() == 0) {
        if (cgs.CheckCodePoint()) {
            g_detectionName = cgs.GetTypefaceName();
            struct MeasurerCacheVal value = {cgs.Clone(), boundaries_};
            std::lock_guard<std::mutex> lock(mutex_);
            cache_[key] = value;
        }
    }
    return SUCCESSED;
}

void MeasurerImpl::SeekTypeface(std::list<struct MeasuringRun> &runs)
{
#ifdef LOGGER_ENABLE_SCOPE
    ScopedTrace scope("MeasurerImpl::SeekTypeface");
#endif
    LOGSCOPED(sl, LOGEX_FUNC_LINE_DEBUG(), "typeface");
    for (auto runsit = runs.begin(); runsit != runs.end(); runsit++) {
        if (runsit->end > text_.size()) {
            LOGEX_FUNC_LINE(ERROR) << "runsit->end overflow of text_";
            break;
        }
        size_t utf16Index = static_cast<size_t>(runsit->start);
        uint32_t cp = 0;
        std::shared_ptr<Typeface> lastTypeface = nullptr;
        while (utf16Index < runsit->end) {
            U16_NEXT(text_.data(), utf16Index, runsit->end, cp);
            if (lastTypeface && lastTypeface->Has(cp)) {
                LOGCEX_DEBUG() << " cached";
                continue;
            }
            if (runsit->typeface) {
                LOGCEX_DEBUG() << " new";
                auto next = runsit;
                struct MeasuringRun run = {.start = utf16Index - U16_LENGTH(cp), .end = runsit->end,
                    .script = runsit->script};
                runs.insert(++next, run);
                runsit->end = utf16Index - U16_LENGTH(cp);
                break;
            }

            char runScript[5] = {(char)(((runsit->script) >> FIRST_BYTE) & 0xFF),
                                 (char)(((runsit->script) >> SECOND_BYTE) & 0xFF),
                                 (char)(((runsit->script) >> THIRD_BYTE) & 0xFF),
                                 (char)((runsit->script) & 0xFF), '\0'};
            lastTypeface = fontCollection_.GetTypefaceForChar(cp, style_, runScript, locale_);
            if (lastTypeface == nullptr) {
                LOGCEX_DEBUG() << " no typeface";
                continue;
            }

            LOGCEX_DEBUG() << " found at " << lastTypeface->GetName();
            runsit->typeface = lastTypeface;
        }
    }
}

void MeasurerImpl::SeekScript(std::list<struct MeasuringRun> &runs)
{
#ifdef LOGGER_ENABLE_SCOPE
    ScopedTrace scope("MeasurerImpl::SeekScript");
#endif
    LOGSCOPED(sl, LOGEX_FUNC_LINE_DEBUG(), "script");
    auto icuGetUnicodeFuncs = hb_unicode_funcs_create(hb_icu_get_unicode_funcs());
    if (icuGetUnicodeFuncs == nullptr) {
        LOGEX_FUNC_LINE(ERROR) << "hb_unicode_funcs_create return nullptr";
        throw TEXGINE_EXCEPTION(API_FAILED);
    }

    struct MeasuringRun run = {.start = startIndex_, .end = endIndex_};
    runs.push_back(run);
    DoSeekScript(runs, icuGetUnicodeFuncs);
    hb_unicode_funcs_destroy(icuGetUnicodeFuncs);
}

void MeasurerImpl::DoSeekScript(std::list<struct MeasuringRun> &runs, hb_unicode_funcs_t* icuGetUnicodeFuncs)
{
    int index = 0;
    for (auto it = runs.begin(); it != runs.end(); it++) {
        std::stringstream ss;
        ss << "[" << it->start << ", " << it->end << ")";
        LOGSCOPED(sl, LOGEX_FUNC_LINE_DEBUG(), ss.str());

        size_t utf16Index = it->start;
        uint32_t cp = 0;
        auto &script = it->script;
        while (utf16Index < it->end) {
            if (utf16Index >= text_.size()) {
                LOGEX_FUNC_LINE(ERROR) << "u16index overflow of text_";
                throw TEXGINE_EXCEPTION(ERROR_STATUS);
            }

            U16_NEXT(text_.data(), utf16Index, it->end, cp);
            auto s = hb_unicode_script(icuGetUnicodeFuncs, cp);
            // 2 & 6 means output width，0 means fill with 0
            LOGEX_FUNC_LINE_DEBUG() << "[" << std::setw(2) << std::setfill('0') << index++ << ": 0x"
                << std::setw(6) << std::setfill('0') << std::hex << std::uppercase << cp << "]" << " " << s;
            if (script == static_cast<size_t>(HB_SCRIPT_INVALID)) {
                script = s;
            }

            if (s == script) {
                continue;
            }

            if (script == HB_SCRIPT_INHERITED || script == HB_SCRIPT_COMMON) {
                script = s;
            } else if (s == HB_SCRIPT_INHERITED || s == HB_SCRIPT_COMMON) {
                continue;
            } else {
                index--;
                auto next = it;
                struct MeasuringRun run = {.start = utf16Index - U16_LENGTH(cp), .end = it->end};
                runs.insert(++next, run);
                it->end = utf16Index - U16_LENGTH(cp);
                break;
            }
        }

        if (it->end <= it->start) {
            LOGEX_FUNC_LINE(ERROR) << "run have error range";
            throw TEXGINE_EXCEPTION(ERROR_STATUS);
        }

        it->script = script;
    }
}

int MeasurerImpl::Shape(CharGroups &cgs, std::list<struct MeasuringRun> &runs, std::vector<Boundary> boundaries)
{
#ifdef LOGGER_ENABLE_SCOPE
    ScopedTrace scope("MeasurerImpl::Shape");
#endif
    cgs = CharGroups::CreateEmpty();
    LOGSCOPED(sl, LOGEX_FUNC_LINE_DEBUG(), "shape");
    size_t index = 0;
    for (auto &run : runs) {
        if (run.end <= run.start) {
            LOGEX_FUNC_LINE(ERROR) << "run have error range";
            throw TEXGINE_EXCEPTION(ERROR_STATUS);
        }

        std::stringstream ss;
        ss << "[" << run.start << ", " << run.end << ")";
        LOGSCOPED(sl, LOGEX_FUNC_LINE_DEBUG(), ss.str());

        if (DoShape(cgs, run, index)) {
            return FAILED;
        }
    }

    for (auto &[start, end] : boundaries) {
        const auto &wordcgs = cgs.GetSubFromU16RangeAll(start, end);
        auto cg = wordcgs.Get(static_cast<int32_t>(wordcgs.GetNumberOfCharGroup()) - 1);
        bool isWhitespace = (u_isWhitespace(cg.chars[0]) == 1);
        if (isWhitespace) {
            cg.invisibleWidth += wordSpacing_;
        }
        cg.isWordEnd = true;
    }
    return SUCCESSED;
}

int MeasurerImpl::DoShape(CharGroups &cgs, MeasuringRun &run, size_t &index)
{
    auto typeface = run.typeface;
    if (typeface == nullptr) {
        LOGEX_FUNC_LINE(ERROR) << "there is no typeface";
        return FAILED;
    }

    auto hbuffer = hb_buffer_create();
    if (!hbuffer) {
        LOGEX_FUNC_LINE(ERROR) << "hbuffer is nullptr";
        return FAILED;
    }

    if (text_.empty()) {
        LOGEX_FUNC_LINE(ERROR) << "text is nullptr";
        return FAILED;
    }
    hb_buffer_add_utf16(hbuffer, text_.data(), text_.size(), run.start, run.end - run.start);
    hb_buffer_set_direction(hbuffer, rtl_ ? HB_DIRECTION_RTL : HB_DIRECTION_LTR);
    auto icuGetUnicodeFuncs = hb_unicode_funcs_create(hb_icu_get_unicode_funcs());
    if (!icuGetUnicodeFuncs) {
        LOGEX_FUNC_LINE(ERROR) << "icuGetUnicodeFuncs is nullptr";
        HbDestroy(hbuffer, nullptr, nullptr, nullptr);
        return FAILED;
    }

    hb_buffer_set_unicode_funcs(hbuffer, icuGetUnicodeFuncs);
    hb_buffer_set_script(hbuffer, run.script);
    hb_buffer_set_language(hbuffer, hb_language_from_string(locale_.c_str(), INVALID_TEXT_LENGTH));

    auto hface = hb_face_create_for_tables(HbFaceReferenceTableTypeface, typeface->Get()->GetTypeface().get(), 0);
    if (!hface) {
        LOGEX_FUNC_LINE(ERROR) << "hface is nullptr";
        HbDestroy(hbuffer, nullptr, nullptr, icuGetUnicodeFuncs);
        return FAILED;
    }

    auto hfont = hb_font_create(hface);
    if (!hfont) {
        LOGEX_FUNC_LINE(ERROR) << "hfont is nullptr";
        HbDestroy(hbuffer, nullptr, hface, icuGetUnicodeFuncs);
        return FAILED;
    }

    std::vector<hb_feature_t> ff;
    GenerateHBFeatures(ff, fontFeatures_);
    hb_shape(hfont, hbuffer, ff.data(), ff.size());

    if (GetGlyphs(cgs, run, index, hbuffer, typeface)) {
        HbDestroy(hbuffer, hfont, hface, icuGetUnicodeFuncs);
        return FAILED;
    }

    HbDestroy(hbuffer, hfont, hface, icuGetUnicodeFuncs);
    return SUCCESSED;
}

void MeasurerImpl::HbDestroy(hb_buffer_t* hbuffer, hb_font_t* hfont, hb_face_t* hface,
    hb_unicode_funcs_t* icuGetUnicodeFuncs)
{
    if (hbuffer) {
        hb_buffer_destroy(hbuffer);
    }

    if (hfont) {
        hb_font_destroy(hfont);
    }

    if (hface) {
        hb_face_destroy(hface);
    }

    if (icuGetUnicodeFuncs) {
        hb_unicode_funcs_destroy(icuGetUnicodeFuncs);
    }
}

int MeasurerImpl::GetGlyphs(CharGroups &cgs, MeasuringRun &run, size_t &index, hb_buffer_t* hbuffer,
    std::shared_ptr<TextEngine::Typeface> typeface)
{
    uint32_t ng = 0u;
    auto hginfos = hb_buffer_get_glyph_infos(hbuffer, &ng);
    if (!hginfos) {
        LOGEX_FUNC_LINE(ERROR) << "hginfos is nullptr";
        return FAILED;
    }

    auto hgpositions = hb_buffer_get_glyph_positions(hbuffer, nullptr);
    if (!hgpositions) {
        LOGEX_FUNC_LINE(ERROR) << "hgpositions is nullptr";
        return FAILED;
    }

    LOGEX_FUNC_LINE_DEBUG() << "ng: " << ng;
    auto upe = typeface->Get()->GetUnitsPerEm();
    if (!upe) {
        LOGEX_FUNC_LINE(ERROR) << "upe is 0";
        return FAILED;
    }

    auto glyphEm = 1.0 * size_ / upe;
    std::map<uint32_t, CharGroup> cgsByCluster{{run.end, {}}};
    for (uint32_t i = 0; i < ng; i++) {
        auto &cg = cgsByCluster[hginfos[i].cluster];
        struct Glyph glyph = {
            .codepoint = hginfos[i].codepoint,
            .advanceX = glyphEm * hgpositions[i].x_advance,
            .advanceY = glyphEm * hgpositions[i].y_advance,
            .offsetX = glyphEm * hgpositions[i].x_offset,
            .offsetY = glyphEm * hgpositions[i].y_offset
        };
        cg.glyphs.push_back(glyph);
        cg.typeface = typeface;
    }

    DoCgsByCluster(cgsByCluster);
    cgsByCluster.erase(run.end);
    for (uint32_t i = 0; i < ng; i++) {
        DumpCharGroup(index++, cgsByCluster[hginfos[i].cluster], glyphEm, hginfos[i], hgpositions[i]);
    }

    for (const auto &[cluster, cg] : cgsByCluster) {
        cgs.PushBack(cg);
    }

    if (rtl_) {
        cgs.ReverseAll();
    }
    return SUCCESSED;
}

void MeasurerImpl::DoCgsByCluster(std::map<uint32_t, TextEngine::CharGroup> &cgsByCluster)
{
    for (auto it = cgsByCluster.begin(); it != cgsByCluster.end(); it++) {
        auto start = (it++)->first;
        if (it == cgsByCluster.end()) {
            break;
        }

        auto end = (it--)->first;
        if (start > text_.size() || end > text_.size()) {
            LOGEX_FUNC_LINE(ERROR) << "cgsByCluster is not align with text_";
            throw TEXGINE_EXCEPTION(ERROR_STATUS);
        }

        it->second.chars.insert(it->second.chars.end(), text_.begin() + start, text_.begin() + end);
        if (it->second.IsHardBreak()) {
            continue;
        }
        it->second.visibleWidth = 0;
        for (const auto &glyph : it->second.glyphs) {
            it->second.visibleWidth += glyph.advanceX;
        }

        it->second.invisibleWidth = letterSpacing_;
        if (u_isWhitespace(it->second.chars[0]) != 0) {
            it->second.invisibleWidth += it->second.visibleWidth;
            it->second.visibleWidth = 0;
        }
    }
}

void MeasurerImpl::GenerateHBFeatures(std::vector<hb_feature_t> &fontFeatures, const FontFeatures* ff)
{
    LOGSCOPED(sl, LOGEX_FUNC_LINE_DEBUG(), "GenerateHBFeatures");
    if (ff == nullptr) {
        return;
    }

    auto features = ff->GetFeatures();
    if (features.empty()) {
        return;
    }

    for (auto &[ft, fv] : features) {
        hb_feature_t hf;
        if (hb_feature_from_string(ft.c_str(), ft.size(), &hf)) {
            hf.value = static_cast<size_t>(fv);
            fontFeatures.push_back(hf);
        }

        LOGEX_FUNC_LINE_DEBUG() << "feature tag:" << hf.tag << ", feature value:" << hf.value;
    }
}
} // namespace TextEngine
} // namespace Rosen
} // namespace OHOS
