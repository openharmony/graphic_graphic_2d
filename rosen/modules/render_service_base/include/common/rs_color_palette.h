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

#ifndef RENDER_SERVICE_CLIENT_CORE_COMMON_RS_COLOR_PALETTE_H
#define RENDER_SERVICE_CLIENT_CORE_COMMON_RS_COLOR_PALETTE_H

#include "common/rs_color.h"
#include "common/rs_macros.h"

namespace OHOS {
namespace Rosen {
struct RgbPalette {
    RgbPalette() = delete;
    static const RSColor& Black()
    {
        static RSColor val { 0, 0, 0 };
        return val;
    };

    static const RSColor& Blue()
    {
        static RSColor val { 0, 0, 255 };
        return val;
    };

    static const RSColor& Cyan()
    {
        static RSColor val { 0, 255, 255 };
        return val;
    };

    static const RSColor& DarkGray()
    {
        static RSColor val { 68, 68, 68 };
        return val;
    };

    static const RSColor& Gray()
    {
        static RSColor val { 136, 136, 136 };
        return val;
    };

    static const RSColor& Green()
    {
        static RSColor val { 0, 255, 0 };
        return val;
    };

    static const RSColor& LightGray()
    {
        static RSColor val { 204, 204, 204 };
        return val;
    };

    static const RSColor& Magenta()
    {
        static RSColor val { 255, 0, 255 };
        return val;
    };

    static const RSColor& Red()
    {
        static RSColor val { 255, 0, 0 };
        return val;
    };

    static const RSColor& Transparent()
    {
        static RSColor val { 0, 0, 0, 0 };
        return val;
    };

    static const RSColor& White()
    {
        static RSColor val { 255, 255, 255 };
        return val;
    };
};
} // namespace Rosen
} // namespace OHOS
#endif // RENDER_SERVICE_CLIENT_CORE_COMMON_RS_COLOR_PALETTE_H
