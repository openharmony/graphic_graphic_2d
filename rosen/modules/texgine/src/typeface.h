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

#ifndef ROSEN_MODULES_TEXGINE_SRC_TYPEFACE_H
#define ROSEN_MODULES_TEXGINE_SRC_TYPEFACE_H

#include <iostream>
#include <memory>

#include "opentype_parser/cmap_parser.h"
#include "texgine_string.h"
#include "texgine_typeface.h"

struct hb_blob_t;

namespace OHOS {
namespace Rosen {
namespace TextEngine {
class Typeface {
public:
    static std::shared_ptr<Typeface> MakeDefault();
    static std::unique_ptr<Typeface> MakeFromFile(const std::string &filename);
    Typeface(std::shared_ptr<TexgineTypeface> tf);
    ~Typeface();

    std::string GetName();
    bool Has(uint32_t ch);
    std::shared_ptr<TexgineTypeface> Get() const { return typeface_; }

    void ComputeFakeryItalic(bool isItalic);
    bool DetectionItalic() const;
    bool DetectionFakeBold() const;
    void ComputeFakery(int wantedWeight);

private:
    friend void ReportMemoryUsage(const std::string &member, const Typeface &str, bool needThis);

    bool ParseCmap(const std::shared_ptr<CmapParser> &parser);

    std::shared_ptr<TexgineTypeface> typeface_ = nullptr;
    TexgineString name_;
    hb_blob_t *hblob_ = nullptr;
    std::shared_ptr<CmapParser> cmapParser_ = nullptr;
    std::unique_ptr<char[]> cmapData_ = nullptr;
    bool isFakeItalic_ = false;
    bool isFakeBold_ = false;
    static inline std::map<std::string, std::shared_ptr<CmapParser>> cmapCache_;
};
} // namespace TextEngine
} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_MODULES_TEXGINE_SRC_TYPEFACE_H
