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

#ifndef GRAPHIC_2D_CONFIGURE_COMMAND_H
#define GRAPHIC_2D_CONFIGURE_COMMAND_H

#include <inttypes.h>
#include <string>
#include <unordered_map>
#include <vector>

namespace OHOS::Rosen {

enum G2cErrCode {
    G2C_ERROR = -1,

    G2C_EXEC_SUCCESS = 0,

    G2C_NO_SCREEN = 100,
    G2C_BASE_REMOVE_FAILED,

    G2C_XML_PARSER_CREATE_FAIL = 200,
    G2C_XML_FILE_LOAD_FAIL,
    G2C_XML_GET_ROOT_FAIL,
    G2C_XML_PARSE_INTERNAL_FAIL,

    G2C_SCREEN_MODE_EXIST,
    G2C_SCREEN_PARAM_ERROR,
    G2C_FINAL_RANGE_NOT_VALID,
};

enum G2cXmlNode {
    G2C_XML_UNDEFINED = 0,
    G2C_XML_PARAM,
    G2C_XML_PARAMS,
};

class Graphic2dConfigData {
public:
    Graphic2dConfigData() = default;
    ~Graphic2dConfigData() = default;
    std::unordered_map<std::string, std::string> graphic2dConfig_;
};
}
#endif // GRAPHIC_2D_CONFIGURE_COMMAND_H
