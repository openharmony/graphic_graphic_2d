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

#ifndef FONT_CONFIG_H
#define FONT_CONFIG_H

#include <json/json.h>
#include <string>
#include <vector>
#include <include/core/SkString.h>

namespace OHOS {
namespace Rosen {
namespace TextEngine {
class FontConfig {
public:
    explicit FontConfig(const char* fname = nullptr);
    virtual ~FontConfig() = default;
    void Dump() const;
    std::vector<std::string> GetFontSet() const;

private:
    int ParseConfig(const char* fname);
    int ParseFont(const Json::Value& root);
    int CheckConfigFile(const char* fname, Json::Value& root) const;
    static char* GetFileData(const char* fname, int& size);
    std::vector<std::string> fontSet_;
};
} // namespace TextEngine
} // namespace Rosen
} // namespace OHOS
#endif /* FONT_CONFIG_H */
