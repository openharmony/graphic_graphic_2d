/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#ifndef FRAMEWORKS_WMSERVER_SRC_WMLAYOUT_SCSS_PARSER_DRIVER_H
#define FRAMEWORKS_WMSERVER_SRC_WMLAYOUT_SCSS_PARSER_DRIVER_H

#include <iostream>
#include <map>
#include <string>

#include "scanner.h"
#include "parser.hpp"

namespace OHOS {
class Driver {
public:
    Driver();
    virtual ~Driver() = default;

    int parse(std::istream &infile);

    struct CSSBlock {
        std::map<std::string, std::string> declares;
        std::map<std::string, struct CSSBlock> blocks;
        struct CSSBlock *parent;
    } global;
    struct CSSBlock *current = &global;

private:
    Parser parser;
    Scanner scanner;
};
} // namespace OHOS

#endif // FRAMEWORKS_WMSERVER_SRC_WMLAYOUT_SCSS_PARSER_DRIVER_H
