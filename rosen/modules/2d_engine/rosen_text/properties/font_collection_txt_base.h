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

#ifndef ROSEN_TEXT_PROPERTIES_FONT_COLLECTION_TXT_BASE_H
#define ROSEN_TEXT_PROPERTIES_FONT_COLLECTION_TXT_BASE_H

#include <memory>
#include <vector>

namespace rosen {
class FontCollectionTxtBase {
public:
    FontCollectionTxtBase() {};

    virtual ~FontCollectionTxtBase() {};

    virtual void RegisterTestFonts() = 0;

    virtual void LoadFontFromList(const uint8_t* font_data,
                                  int length,
                                  std::string family_name) = 0;
    virtual void LoadSystemFont() = 0;
};
} // namespace rosen
#endif // ROSEN_TEXT_PROPERTIES_FONT_COLLECTION_TXT_BASE_H
