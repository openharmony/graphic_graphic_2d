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
#ifndef UIEFFECT_FILTER_PARA_H
#define UIEFFECT_FILTER_PARA_H

#include <map>
#include <iostream>

namespace OHOS {
namespace Rosen {

enum class TileMode : uint32_t {
    CLAMP = 0,
    REPEAT = 1,
    MIRROR = 2,
    DECAL = 3,
};

const std::map<std::string, TileMode> STRING_TO_JS_MAP = {
    { "CLAMP", TileMode::CLAMP },
    { "REPEAT", TileMode::REPEAT },
    { "MIRROR", TileMode::MIRROR },
    { "DECAL", TileMode::DECAL },
};

class FilterPara {
public:
    enum ParaType {
        NONE,
        BLUR,
        PIXEL_STRETCH,
    };

    FilterPara()  = default;
    virtual ~FilterPara() = default;

    ParaType type_;
};
} // namespace Rosen
} // namespace OHOS
#endif // UIEFFECT_FILTER_PARA_H