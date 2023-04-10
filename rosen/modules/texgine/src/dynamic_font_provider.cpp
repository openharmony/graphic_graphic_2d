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

#include "texgine/dynamic_font_provider.h"

#include <iostream>
#include <mutex>

#include "dynamic_font_style_set.h"
#include "texgine_exception.h"
#include "texgine_stream.h"
#include "texgine/utils/exlog.h"
#include "typeface.h"
#include "variant_font_style_set.h"

namespace Texgine {
#define PARAMETERERROR 1;
#define APIERROR 2;

std::shared_ptr<DynamicFontProvider> DynamicFontProvider::Create() noexcept(true)
{
    return std::shared_ptr<DynamicFontProvider>(new DynamicFontProvider());
}

int DynamicFontProvider::LoadFont(const std::string &familyName, const void *data, size_t datalen) noexcept(true)
{
    if (data == nullptr) {
        LOG2EX(ERROR) << "data is nullptr!";
        return PARAMETERERROR;
    }

    if (datalen == 0) {
        LOG2EX(ERROR) << "datalen is 0!";
        return PARAMETERERROR;
    }

    auto stream = TexgineMemoryStream::MakeCopy(data, datalen);
    if (stream == nullptr) {
        LOG2EX(ERROR) << "stream is nullptr!";
        return APIERROR;
    }

    auto texgineTypeface = TexgineTypeface::MakeFromStream(std::move(stream));
    if (texgineTypeface == nullptr) {
        LOG2EX(ERROR) << "texgineTypeface is nullptr!";
        return APIERROR;
    }

    auto typeface = std::make_unique<Typeface>(texgineTypeface);
    LOG2EX_DEBUG() << "load font name:" << familyName;
    auto dfss = std::make_shared<DynamicFontStyleSet>(std::move(typeface));
    fontStyleSetMap_[familyName] = std::make_shared<VariantFontStyleSet>(dfss);
    return 0;
}

std::shared_ptr<VariantFontStyleSet> DynamicFontProvider::MatchFamily(const std::string &familyName) noexcept(true)
{
    for (auto &[name, fss] : fontStyleSetMap_) {
        if (familyName == name) {
            return fss;
        }
    }
    return nullptr;
}
} // namespace Texgine
