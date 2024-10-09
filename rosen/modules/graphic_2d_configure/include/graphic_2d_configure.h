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

#ifndef GRAPHIC_2D_CONFIGURE_H
#define GRAPHIC_2D_CONFIGURE_H

#include <cstdint>
#include <cinttypes>
#include <functional>
#include <memory>
#include <thread>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "graphic_2d_xml_parser.h"

namespace OHOS::Rosen {
class Graphic2dConfigure final {
public:
    static Graphic2dConfigure& Instance();
    void parserTest();

private:
    Graphic2dConfigure();
    ~Graphic2dConfigure() = default;
    Graphic2dConfigure(const Graphic2dConfigure&) =delete;
    Graphic2dConfigure(const Graphic2dConfigure&&) =delete;
    Graphic2dConfigure& operator=(const Graphic2dConfigure&) =delete;
    Graphic2dConfigure& operator=(const Graphic2dConfigure&&) =delete;

    void Init();
    int32_t InitXmlConfig();

    static constexpr char CONFIG_FILE_PRODUCT[] = "/sys_prod/etc/graphic/hgm_policy_config.xml";
    std::unique_ptr<Graphic2dXmlParser> mParser_ = nullptr;
    std::unique_ptr<Graphic2dConfigData> mConfigData_ = nullptr;
};
}
#endif // GRAPHIC_2D_CONFIGURE_H