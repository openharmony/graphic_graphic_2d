/*
 * Copyright (c) 2023-2024 Huawei Device Co., Ltd.
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

#ifndef SKIA_FONT_MGR_H
#define SKIA_FONT_MGR_H

#include <memory>

#include "include/core/SkFontMgr.h"

#include "impl_interface/font_mgr_impl.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
class SkiaFontMgr : public FontMgrImpl {
public:
    static inline constexpr AdapterType TYPE = AdapterType::SKIA_ADAPTER;
    explicit SkiaFontMgr(sk_sp<SkFontMgr> skFontMgr);
    virtual ~SkiaFontMgr() = default;

    AdapterType GetType() const override
    {
        return AdapterType::SKIA_ADAPTER;
    }

    static std::shared_ptr<FontMgrImpl> CreateDefaultFontMgr();
#ifndef USE_TEXGINE
    static std::shared_ptr<FontMgrImpl> CreateDynamicFontMgr();
    void LoadDynamicFont(const std::string& familyName, const uint8_t* data, size_t dataLength) override;
    void LoadThemeFont(const std::string& familyName, const std::string& themeName,
        const uint8_t* data, size_t dataLength) override;
#endif
    Typeface* MatchFamilyStyleCharacter(const char familyName[], const FontStyle& fontStyle,
                                        const char* bcp47[], int bcp47Count,
                                        int32_t character) override;
    FontStyleSet* MatchFamily(const char familyName[]) const override;

    Typeface* MatchFamilyStyle(const char familyName[], const FontStyle& fontStyle) const override;

    int CountFamilies() const override;
    void GetFamilyName(int index, std::string& str) const override;
    FontStyleSet* CreateStyleSet(int index) const override;
private:
    sk_sp<SkFontMgr> skFontMgr_;
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif