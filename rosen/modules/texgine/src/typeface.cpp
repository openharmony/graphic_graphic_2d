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
#include "texgine/utils/trace.h"

namespace OHOS {
namespace Rosen {
namespace TextEngine {
std::unique_ptr<Typeface> Typeface::MakeFromFile(const std::string &filename)
{
    auto st = TexgineTypeface::MakeFromFile(filename.c_str());
    if (st == nullptr || st->GetTypeface() == nullptr) {
        throw TEXGINE_EXCEPTION(APIFailed);
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
    LOG2EX(DEBUG) << "Parse Cmap: " << GetName();
    ScopedTrace scope("Typeface::InitCmap");
    auto tag = HB_TAG('c', 'm', 'a', 'p');
    if (typeface_ == nullptr || typeface_->GetTypeface() == nullptr) {
        LOG2EX(WARN) << "typeface_ is nullptr";
        return false;
    }

    auto size = typeface_->GetTableSize(tag);
    if (size <= 0) {
        LOG2EX(ERROR) << name_.ToString() << " haven't cmap";
        throw TEXGINE_EXCEPTION(APIFailed);
    }

    cmapData_ = std::make_unique<char[]>(size);
    auto retv = typeface_->GetTableData(tag, 0, size, cmapData_.get());
    if (size != retv) {
        LOG2EX(ERROR) << "getTableData failed size: " << size << ", retv: " << retv;
        throw TEXGINE_EXCEPTION(APIFailed);
    }

    hblob_ = hb_blob_create(reinterpret_cast<const char *>(cmapData_.get()),
            size, HB_MEMORY_MODE_WRITABLE, cmapData_.get(), nullptr);
    if (hblob_ == nullptr) {
        LOG2EX(ERROR) << "hblob_ is nullptr";
        throw TEXGINE_EXCEPTION(APIFailed);
    }

    scope.Finish();
    ScopedTrace scope2("Typeface::ParseCmap");
    auto retval = parser->Parse(hb_blob_get_data(hblob_, nullptr), hb_blob_get_length(hblob_));
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

    return cmapParser_->GetGlyphId(ch) != CmapParser::InvalidGlyphId;
}
} // namespace TextEngine
} // namespace Rosen
} // namespace OHOS
