/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#ifndef RENDER_SERVICE_MAIN_RENDER_SERVER_RS_RENDER_MODE_CONFIG_PARSER_H
#define RENDER_SERVICE_MAIN_RENDER_SERVER_RS_RENDER_MODE_CONFIG_PARSER_H

#include <libxml/parser.h>
#include <libxml/tree.h>
#include <map>

#include "rs_render_mode_config.h"

namespace OHOS {
namespace Rosen {
enum RenderModeParseErrCode {
    RENDER_MODE_PARSE_ERROR = -1,
    RENDER_MODE_PARSE_EXEC_SUCCESS = 0,
    RENDER_MODE_PARSE_FILE_LOAD_FAIL = 200,
    RENDER_MODE_PARSE_GET_ROOT_FAIL,
    RENDER_MODE_PARSE_GET_CHILD_FAIL,
    RENDER_MODE_PARSE_INTERNAL_FAIL,
};

class RSRenderModeConfigParser {
public:
    explicit RSRenderModeConfigParser() : xmlDocument_(nullptr) {}
    ~RSRenderModeConfigParser();

    std::shared_ptr<const RenderModeConfig> BuildRenderConfig();
    
private:
    int32_t LoadConfigurations();
    int32_t Parse(RenderModeConfigBuilder& builder);
    bool ParseInternal(xmlNode& node, RenderModeConfigBuilder& builder);
    std::string ExtractPropertyValue(const std::string& propName, xmlNode& node);
    int32_t ParseGroup(xmlNode& node, RenderModeConfigBuilder& builder, GroupId defaultGroupId);

    xmlDoc* xmlDocument_ = nullptr;
    std::vector<std::string> sysPath_ = {"/system/variant/phone/base/", "/system/variant/tablet/base/",
        "/system/variant/pc/base/", "/system/variant/watch/base/", "/system/variant/tv/base/",
        "/system/variant/car/base/", "/system/variant/smarthomehost/base/"};
    std::string prodPath_ = "/sys_prod/";

    bool isMultiProcessModeEnabled_ = false;
};

} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_MAIN_RENDER_SERVER_RS_RENDER_MODE_CONFIG_PARSER_H
