/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.. All rights reserved.
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

#include "txt/platform.h"

#ifndef OHOS_STANDARD_SYSTEM
#include "third_party/skia/src/ports/SkFontMgr_ohos.h"
#endif

namespace OHOS {
namespace Rosen {
namespace SPText {
std::vector<std::string> GetDefaultFontFamilies()
{
#ifdef OHOS_STANDARD_SYSTEM
    return { "OhosThemeFont", "HarmonyOS-Sans" };
#else
    return "sans-serif";
#endif
}

#ifndef USE_ROSEN_DRAWING
sk_sp<SkFontMgr> GetDefaultFontManager()
{
#ifdef OHOS_STANDARD_SYSTEM
    return SkFontMgr::RefDefault();
#else
    return SkFontMgr_New_OHOS();
#endif
}
#else
std::shared_ptr<Drawing::FontMgr> GetDefaultFontManager()
{
    return Drawing::FontMgr::CreateDefaultFontMgr();
}
#endif
} // namespace SPText
} // namespace Rosen
} // namespace OHOS
