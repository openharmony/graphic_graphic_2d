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

#include "typeface.h"

#include <hb.h>

#include "texgine_exception.h"
#include "texgine/utils/exlog.h"
#ifdef LOGGER_ENABLE_SCOPE
#include "texgine/utils/trace.h"
#endif

namespace OHOS {
namespace Rosen {
namespace TextEngine {
#define DYNAMIC_FONT_WEIGHT 6
std::unique_ptr<Typeface> Typeface::MakeFromFile(const std::string &filename)
{
    auto st = TexgineTypeface::MakeFromFile(filename.c_str());
    if (st == nullptr || st->GetTypeface() == nullptr) {
        throw TEXGINE_EXCEPTION(API_FAILED);
    }

    return std::make_unique<Typeface>(st);
}

Typeface::Typeface(std::shared_ptr<TexgineTypeface> tf)
    : typeface_(tf)
{
    if (typeface_ != nullptr) {
        typeface_->GetFamilyName(&name_);
    }
}

Typeface::~Typeface()
{
    if (hblob_ != nullptr) {
        hb_blob_destroy(hblob_);
    }
}

std::string Typeface::GetName()
{
    return name_.ToString();
}

bool Typeface::ParseCmap(const std::shared_ptr<CmapParser> &parser)
{
    LOGEX_FUNC_LINE_DEBUG(DEBUG) << "Parse Cmap: " << GetName();
#ifdef LOGGER_ENABLE_SCOPE
    ScopedTrace scope("Typeface::InitCmap");
#endif
    auto tag = HB_TAG('c', 'm', 'a', 'p');
    if (typeface_ == nullptr || typeface_->GetTypeface() == nullptr) {
        LOGEX_FUNC_LINE(WARN) << "typeface_ is nullptr";
        return false;
    }

    auto size = typeface_->GetTableSize(tag);
    if (size <= 0) {
        LOGEX_FUNC_LINE(ERROR) << name_.ToString() << " haven't cmap";
        throw TEXGINE_EXCEPTION(API_FAILED);
    }

    cmapData_ = std::make_unique<char[]>(size);
    auto retv = typeface_->GetTableData(tag, 0, size, cmapData_.get());
    if (size != retv) {
        LOGEX_FUNC_LINE(ERROR) << "getTableData failed size: " << size << ", retv: " << retv;
        throw TEXGINE_EXCEPTION(API_FAILED);
    }

    hblob_ = hb_blob_create(reinterpret_cast<const char *>(cmapData_.get()),
        size, HB_MEMORY_MODE_WRITABLE, cmapData_.get(), nullptr);
    if (hblob_ == nullptr) {
        LOGEX_FUNC_LINE(ERROR) << "hblob_ is nullptr";
        throw TEXGINE_EXCEPTION(API_FAILED);
    }

#ifdef LOGGER_ENABLE_SCOPE
    scope.Finish();
    ScopedTrace scope2("Typeface::ParseCmap");
#endif
    const char* data = hb_blob_get_data(hblob_, nullptr);
    unsigned int length = hb_blob_get_length(hblob_);
    auto retval = parser->Parse(data, length);
    return retval == 0;
}

bool Typeface::Has(uint32_t ch)
{
    while (cmapParser_ == nullptr) {
        auto name = GetName();
        auto it = cmapCache_.find(name);
        if (it != cmapCache_.end()) {
            cmapParser_ = cmapCache_[name];
            break;
        }

        auto parser = std::make_shared<CmapParser>();
        if (ParseCmap(parser)) {
            cmapParser_ = parser;
            cmapCache_[name] = parser;
        } else {
            cmapCache_.erase(name);
            return false;
        }
    }

    return cmapParser_->GetGlyphId(ch) != CmapParser::INVALID_GLYPH_ID;
}

void Typeface::ComputeFakeryItalic(bool isItalic)
{
    isFakeItalic_ = isItalic;
}

bool Typeface::DetectionItalic() const
{
    return isFakeItalic_;
}

void Typeface::ComputeFakery(int wantedWeight)
{
    bool isFakeBold = wantedWeight >= DYNAMIC_FONT_WEIGHT;
    isFakeBold_ = isFakeBold;
}

bool Typeface::DetectionFakeBold() const
{
    return isFakeBold_;
}
} // namespace TextEngine
} // namespace Rosen
} // namespace OHOS
