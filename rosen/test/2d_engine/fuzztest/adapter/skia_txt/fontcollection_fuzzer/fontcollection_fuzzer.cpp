/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "fontcollection_fuzzer.h"

#include <cstddef>
#include <memory>

#include "font_collection.h"
#include "get_object.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
void OHDrawingFontcollectionFuzz1(const uint8_t* data, size_t size)
{
    std::shared_ptr<OHOS::Rosen::FontCollection> fontCollection = OHOS::Rosen::FontCollection::Create();
    fontCollection->DisableFallback();
    fontCollection->DisableSystemFont();
    const uint8_t* LoadFontData = nullptr;
    std::shared_ptr<OHOS::Rosen::Drawing::Typeface> typeface = fontCollection->LoadFont("familyname", LoadFontData, 0);
    typeface = fontCollection->LoadThemeFont("familynametest", LoadFontData, 0);
    std::shared_ptr<Drawing::FontMgr> fontMgr = fontCollection->GetFontMgr();
}
void OHDrawingFontcollectionFuzz2(const uint8_t* data, size_t size)
{
    std::shared_ptr<OHOS::Rosen::FontCollection> fontCollection = OHOS::Rosen::FontCollection::From(nullptr);
    std::shared_ptr<OHOS::Rosen::Drawing::Typeface> typeface1 = OHOS::Rosen::Drawing::Typeface::MakeDefault();
    OHOS::Rosen::Drawing::Typeface::RegisterCallBackFunc(
        [](std::shared_ptr<OHOS::Rosen::Drawing::Typeface> typeface) { return false; });
    fontCollection->RegisterTypeface(typeface1);
    OHOS::Rosen::Drawing::Typeface::RegisterCallBackFunc(
        [](std::shared_ptr<OHOS::Rosen::Drawing::Typeface> typeface) { return typeface != nullptr; });
    fontCollection->RegisterTypeface(typeface1);
    fontCollection->RegisterTypeface(nullptr);
}
void OHDrawingFontcollectionFuzz3(const uint8_t* data, size_t size)
{
    std::shared_ptr<OHOS::Rosen::FontCollection> fontCollection = OHOS::Rosen::FontCollection::From(nullptr);
    fontCollection->ClearCaches();
    std::shared_ptr<Drawing::FontMgr> fontMgr = fontCollection->GetFontMgr();
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::Drawing::OHDrawingFontcollectionFuzz1(data, size);
    OHOS::Rosen::Drawing::OHDrawingFontcollectionFuzz2(data, size);
    OHOS::Rosen::Drawing::OHDrawingFontcollectionFuzz3(data, size);
    return 0;
}
