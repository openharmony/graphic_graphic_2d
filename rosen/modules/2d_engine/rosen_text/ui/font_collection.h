/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.. All rights reserved.
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

#ifndef ROSEN_TEXT_UI_FONT_COLLECTION_H_
#define ROSEN_TEXT_UI_FONT_COLLECTION_H_

#include <memory>

#include "rosen_text/properties/font_collection_base.h"
#include "rosen_text/properties/font_collection_txt_base.h"

namespace rosen {
class FontCollection : public FontCollectionBase {
public:
    static std::shared_ptr<FontCollection> GetInstance();
    FontCollection();
    ~FontCollection();
    std::shared_ptr<FontCollectionTxtBase> GetFontCollection() override;
    void RegisterTestFonts() override;
    void LoadFontFromList(const uint8_t* font_data,
                          int length,
                          std::string family_name) override;
    void LoadSystemFont();
private:
    std::shared_ptr<FontCollectionTxtBase> collection_;
    FontCollection(const FontCollection&) = delete;
    FontCollection& operator=(const FontCollection&) = delete;
    static std::shared_ptr<FontCollection> instance;
};
} // namespace rosen
#endif // ROSEN_TEXT_UI_FONT_COLLECTION_H_
