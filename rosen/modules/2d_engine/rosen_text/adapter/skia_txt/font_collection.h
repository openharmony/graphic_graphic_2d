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

#ifndef ROSEN_TEXT_ADAPTER_TXT_FONT_COLLECTION_H
#define ROSEN_TEXT_ADAPTER_TXT_FONT_COLLECTION_H

#include "rosen_text/font_collection.h"

#include "txt/font_collection.h"
#ifdef USE_GRAPHIC_TEXT_GINE
#include "txt/asset_font_manager.h"
#endif

#include "text/font_mgr.h"

namespace OHOS {
namespace Rosen {
namespace AdapterTxt {
class FontCollection : public ::OHOS::Rosen::FontCollection {
public:
    explicit FontCollection(std::shared_ptr<txt::FontCollection> fontCollection = nullptr);

    std::shared_ptr<txt::FontCollection> Get();

    void DisableFallback() override;
    void DisableSystemFont() override;
    void LoadFont(const std::string &familyName, const uint8_t *data, size_t datalen) override;
    void LoadThemeFont(const std::string &familyName, const uint8_t *data, size_t datalen) override;

private:
    std::shared_ptr<txt::FontCollection> fontCollection_ = nullptr;
    std::shared_ptr<Drawing::FontMgr> dfmanager_ = nullptr;
    bool disableSystemFont_ = false;
};
} // namespace AdapterTxt
} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_TEXT_ADAPTER_TXT_FONT_COLLECTION_H
