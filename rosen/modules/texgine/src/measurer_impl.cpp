/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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
#include "texgine/utils/trace.h"
#include "text_converter.h"

namespace Texgine {
#define FIELDWIDTH1 2
#define FIELDWIDTH2 4
#define FIELDWIDTH3 6
#define NUMBEROFDIGITS1 8
#define NUMBEROFDIGITS2 16
#define NUMBEROFDIGITS3 24
#define TEXTLENGTH -1

namespace {
void DumpCharGroup(int32_t index, const CharGroup &cg, double glyphEm, hb_glyph_info_t &info, hb_glyph_position_t &position)
{
    auto u8str = TextConverter::ToUTF8(cg.chars_);
    auto ch = (cg.chars_.size() > 0) ? cg.chars_[0] : 0xffffff;
    LOG2EX_DEBUG() << std::setw(FIELDWIDTH1) << std::setfill('0') << index
        << std::hex << std::uppercase
        << ": " << std::right << std::setw(FIELDWIDTH2) << std::setfill(' ') << "\033[40m" << "'"
        << u8str.data() << "'" << "\033[0m"
        << " (char: 0x" << std::setw(FIELDWIDTH3) << std::setfill('0') << ch
        << ", codepoint: 0x" << std::setw(FIELDWIDTH2) << std::setfill('0') << info.codepoint
        << ", cluster: " << std::setw(FIELDWIDTH1) << std::setfill(' ') << std::dec << info.cluster << ")"
        << " {" << glyphEm * position.x_advance << ", " << glyphEm * position.y_advance
        << ", " << glyphEm * position.x_offset << ", " << glyphEm * position.y_offset << "}";
}
} // namespace

hb_blob_t *HbFaceReferenceTableTypeface(hb_face_t *face, hb_tag_t tag, void *context)
{
    std::shared_ptr<TexgineTypeface> typeface = std::make_shared<TexgineTypeface>(context);
    if (typeface->GetTypeface() == nullptr) {
        return nullptr;
    }

    const size_t tableSize = typeface->GetTableSize(tag);
    if (tableSize == 0) {
        return nullptr;
    }

    void *buffer = malloc(tableSize);
    if (buffer == nullptr) {
        return nullptr;
    }

    size_t actualSize = typeface->GetTableData(tag, 0, tableSize, buffer);
    if (tableSize != actualSize) {
        free(buffer);
        return nullptr;
    }

    return hb_blob_create(reinterpret_cast<char *>(buffer),
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

int MeasurerImpl::Measure(CharGroups &cgs)
{
    ScopedTrace scope("MeasurerImpl::Measure");
    LOGSCOPED(sl, LOG2EX_DEBUG(), "MeasurerImpl::Measure");
    struct MeasurerCacheKey key = {
        .text_ = text_,
        .style_ = style_,
        .locale_ = locale_,
        .rtl_ = rtl_,
        .size_ = size_,
        .startIndex_ = startIndex_,
        .endIndex_ = endIndex_,
        .letterSpacing_ = letterSpacing_,
        .wordSpacing_ = wordSpacing_,
    };

    if (fontFeatures_ == nullptr || fontFeatures_->GetFeatures().size() == 0) {
        auto it = cache_.find(key);
        if (it != cache_.end()) {
            cgs = it->second.cgs_.Clone();
            boundaries_ = it->second.boundaries_;
            return 0;
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
        LOG2EX(ERROR) << "Shape failed";
        return ret;
    }

    if (fontFeatures_ == nullptr || fontFeatures_->GetFeatures().size() == 0) {
        cache_[key] = {cgs.Clone(), boundaries_};
    }
    return 0;
}

void MeasurerImpl::SeekTypeface(std::list<struct MeasuringRun> &runs)
{
    ScopedTrace scope("MeasurerImpl::SeekTypeface");
    LOGSCOPED(sl, LOG2EX_DEBUG(), "typeface");
    int index = 0;
    for (auto runsit = runs.begin(); runsit != runs.end(); runsit++) {
        auto u16index = runsit->start;
        uint32_t cp = 0;
        std::shared_ptr<Typeface> lastTypeface = nullptr;
        while (u16index < runsit->end) {
            if (u16index >= text_.size()) {
                LOG2EX(ERROR) << "u16index overflow of text_";
                throw TEXGINE_EXCEPTION(ErrorStatus);
            }
            U16_NEXT(text_.data(), u16index, runsit->end, cp);
            bool has = lastTypeface && lastTypeface->Has(cp);
            LOG2EX_DEBUG(Logger::NoReturn) << "[" << std::setw(FIELDWIDTH1) << std::setfill('0') << index++
                << ": 0x" << std::setw(FIELDWIDTH3) << std::setfill('0') << std::hex << std::uppercase << cp << "]";
            if (has) {
                LOGCEX_DEBUG() << " cached";
                continue;
            }
            if (runsit->typeface) {
                index--;
                LOGCEX_DEBUG() << " new";
                auto next = runsit;
                runs.insert(++next, {
                    .start = u16index - U16_LENGTH(cp),
                    .end = runsit->end,
                    .script = runsit->script,
                });
                runsit->end = u16index - U16_LENGTH(cp);
                break;
            }
            char runScript[5] = {(char)(((runsit->script) >> NUMBEROFDIGITS3) & 0xFF),
                                 (char)(((runsit->script) >> NUMBEROFDIGITS2) & 0xFF),
                                 (char)(((runsit->script) >> NUMBEROFDIGITS1) & 0xFF),
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
    ScopedTrace scope("MeasurerImpl::SeekScript");
    LOGSCOPED(sl, LOG2EX_DEBUG(), "script");
    auto icuGetUnicodeFuncs = hb_unicode_funcs_create(hb_icu_get_unicode_funcs());
    if (icuGetUnicodeFuncs == nullptr) {
        LOG2EX(ERROR) << "hb_unicode_funcs_create return nullptr";
        throw TEXGINE_EXCEPTION(APIFailed);
    }

    runs.push_back({.start = startIndex_, .end = endIndex_});
    DoSeekScript(runs, icuGetUnicodeFuncs);
    hb_unicode_funcs_destroy(icuGetUnicodeFuncs);
}

void MeasurerImpl::DoSeekScript(std::list<struct MeasuringRun> &runs, hb_unicode_funcs_t *icuGetUnicodeFuncs)
{
    auto index = 0;
    for (auto it = runs.begin(); it != runs.end(); it++) {
        std::stringstream ss;
        ss << "[" << it->start << ", " << it->end << ")";
        LOGSCOPED(sl, LOG2EX_DEBUG(), ss.str());

        auto ri = it->start;
        uint32_t cp = 0;
        auto &script = it->script;
        while (ri < it->end) {
            if (ri >= text_.size()) {
                LOG2EX(ERROR) << "u16index overflow of text_";
                throw TEXGINE_EXCEPTION(ErrorStatus);
            }

            U16_NEXT(text_.data(), ri, it->end, cp);
            auto s = hb_unicode_script(icuGetUnicodeFuncs, cp);
            LOG2EX_DEBUG() << "[" << std::setw(FIELDWIDTH1) << std::setfill('0') << index++ << ": 0x"
                << std::setw(FIELDWIDTH3) << std::setfill('0') << std::hex << std::uppercase << cp << "]" << " " << s;
            if (script == HB_SCRIPT_INVALID) {
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
                runs.insert(++next, { .start = ri - U16_LENGTH(cp), .end = it->end });
                it->end = ri - U16_LENGTH(cp);
                break;
            }
        }

        if (it->end <= it->start) {
            LOG2EX(ERROR) << "run have error range";
            throw TEXGINE_EXCEPTION(ErrorStatus);
        }

        it->script = script;
    }
}
} // namespace Texgine
