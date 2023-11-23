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

#include "texgine/theme_font_provider.h"

#include <iostream>
#include <mutex>

#include "dynamic_font_style_set.h"
#include "texgine_exception.h"
#include "texgine_stream.h"
#include "texgine/utils/exlog.h"
#include "typeface.h"
#include "variant_font_style_set.h"

namespace OHOS {
namespace Rosen {
namespace TextEngine {
#define SUCCESS 0
#define PARAMETERERROR 1
#define APIERROR 2

std::shared_ptr<ThemeFontProvider> ThemeFontProvider::GetInstance() noexcept(true)
{
    if (tfp == nullptr) {
        static std::mutex mutex;
        std::lock_guard<std::mutex> lock(mutex);
        if (tfp == nullptr) {
            tfp.reset(new ThemeFontProvider());
        }
    }
    return tfp;
}

int ThemeFontProvider::LoadFont(const std::string &familyName, const void *data, size_t datalen) noexcept(true)
{
    if (familyName.empty() || data == nullptr) {
        familyName_ = "";
        themeFontStyleSet_ = nullptr;
        return SUCCESS;
    }

    if (datalen == 0) {
        LOGEX_FUNC_LINE(ERROR) << "datalen is 0!";
        return PARAMETERERROR;
    }

    auto stream = TexgineMemoryStream::MakeCopy(data, datalen);
    if (stream == nullptr) {
        LOGEX_FUNC_LINE(ERROR) << "stream is nullptr!";
        return APIERROR;
    }

    auto texgineTypeface = TexgineTypeface::MakeFromStream(std::move(stream));
    if (texgineTypeface == nullptr) {
        LOGEX_FUNC_LINE(ERROR) << "texgineTypeface is nullptr!";
        return APIERROR;
    }

    auto typeface = std::make_unique<Typeface>(texgineTypeface);
    LOGEX_FUNC_LINE_DEBUG() << "load font name:" << familyName;
    auto dfss = std::make_shared<DynamicFontStyleSet>(std::move(typeface));
    familyName_ = familyName;
    themeFontStyleSet_.reset(new VariantFontStyleSet(dfss));
    return SUCCESS;
}

std::shared_ptr<VariantFontStyleSet> ThemeFontProvider::MatchFamily(const std::string &familyName) noexcept(true)
{
    if (themeFontStyleSet_ != nullptr && (familyName == familyName_ || familyName.empty())) {
        return themeFontStyleSet_;
    }
    return nullptr;
}
} // namespace TextEngine
} // namespace Rosen
} // namespace OHOS
