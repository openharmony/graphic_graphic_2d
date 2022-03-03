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

#include "rosen_text/properties/font_collection_txt.h"
#include "rosen_text/ui/font_collection.h"

namespace rosen {
std::shared_ptr<FontCollection> FontCollection::instance = nullptr;

std::shared_ptr<FontCollection> FontCollection::GetInstance()
{
    if (instance == nullptr) {
        instance = std::make_shared<FontCollection>();
    }
    return instance;
}

FontCollection::FontCollection()
{
    collection_ = std::make_shared<FontCollectionTxt>();
}

FontCollection::~FontCollection()
{
}

void FontCollection::LoadSystemFont()
{
    collection_->LoadSystemFont();
}

std::shared_ptr<FontCollectionTxtBase> FontCollection::GetFontCollection()
{
    return collection_;
}

void FontCollection::RegisterTestFonts()
{
    collection_->RegisterTestFonts();
}

void FontCollection::LoadFontFromList(const uint8_t* font_data,
                                      int length,
                                      std::string family_name)
{
    collection_->LoadFontFromList(font_data, length, family_name);
}
} // namespace rosen
