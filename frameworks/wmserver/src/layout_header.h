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

#ifndef FRAMEWORKS_WMSERVER_SRC_LAYOUT_HEADER_H
#define FRAMEWORKS_WMSERVER_SRC_LAYOUT_HEADER_H

#include <stdint.h>

struct layout {
    double x;
    double y;
    double w;
    double h;
};

#ifdef __cplusplus
#include <cstddef>
#include <fstream>

namespace OHOS::WMServer {
struct Layout {
    bool isZIndexSetting;
    int32_t zIndex;
    bool isPositionTypeSetting;
    enum class PositionType {
        RELATIVE,
        STATIC, // as status bar
        FIXED,
    } positionType;
    bool isPositionXTypeSetting;
    enum class XPositionType {
        UNSET, // unset
        LFT,   // left
        MID,   // middle
        RGH,   // right
    } pTypeX;
    bool isPositionYTypeSetting;
    enum class YPositionType {
        UNSET, // unset
        TOP,   // top
        MID,   // middle
        BTM,   // bottom
    } pTypeY;
    bool isLayoutSetting;
    struct layout layout;
};

std::ostream &operator <<(std::ostream &os, const struct layout &layout);
std::ostream &operator <<(std::ostream &os, const enum Layout::PositionType &type);
std::ostream &operator <<(std::ostream &os, const enum Layout::XPositionType &type);
std::ostream &operator <<(std::ostream &os, const enum Layout::YPositionType &type);
std::ostream &operator <<(std::ostream &os, const struct Layout &layout);

std::istream &operator >>(std::istream &is, struct layout &layout);
std::istream &operator >>(std::istream &is, enum Layout::PositionType &type);
std::istream &operator >>(std::istream &is, enum Layout::XPositionType &type);
std::istream &operator >>(std::istream &is, enum Layout::YPositionType &type);
std::istream &operator >>(std::istream &is, struct Layout &layout);
} // namespace OHOS::WMServer
#endif // __cplusplus

#endif // FRAMEWORKS_WMSERVER_SRC_LAYOUT_HEADER_H
