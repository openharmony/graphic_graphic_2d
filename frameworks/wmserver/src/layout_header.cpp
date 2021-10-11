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

#include "layout_header.h"

namespace OHOS::WMServer {
std::ostream &operator <<(std::ostream &os, const struct layout &layout)
{
    os << layout.x << " " << layout.y << " " << layout.w << " " << layout.h;
    return os;
}

std::ostream &operator <<(std::ostream &os, const enum Layout::PositionType &type)
{
    os << static_cast<int32_t>(type);
    return os;
}

std::ostream &operator <<(std::ostream &os, const enum Layout::XPositionType &type)
{
    os << static_cast<int32_t>(type);
    return os;
}

std::ostream &operator <<(std::ostream &os, const enum Layout::YPositionType &type)
{
    os << static_cast<int32_t>(type);
    return os;
}

std::ostream &operator <<(std::ostream &os, const struct Layout &layout)
{
    os << std::boolalpha
        << layout.isZIndexSetting << " " << layout.zIndex << " "
        << layout.isPositionTypeSetting << " " << layout.positionType << " "
        << layout.isPositionXTypeSetting << " " << layout.pTypeX << " "
        << layout.isPositionYTypeSetting << " " << layout.pTypeY << " "
        << layout.isLayoutSetting << " " << layout.layout;
    return os;
}

std::istream &operator >>(std::istream &is, struct layout &layout)
{
    is >> layout.x >> layout.y >> layout.w >> layout.h;
    return is;
}

std::istream &operator >>(std::istream &is, enum Layout::PositionType &type)
{
    int32_t enumType;
    is >> enumType;
    type = static_cast<enum Layout::PositionType>(enumType);
    return is;
}

std::istream &operator >>(std::istream &is, enum Layout::XPositionType &type)
{
    int32_t enumType;
    is >> enumType;
    type = static_cast<enum Layout::XPositionType>(enumType);
    return is;
}

std::istream &operator >>(std::istream &is, enum Layout::YPositionType &type)
{
    int32_t enumType;
    is >> enumType;
    type = static_cast<enum Layout::YPositionType>(enumType);
    return is;
}

std::istream &operator >>(std::istream &is, struct Layout &layout)
{
    is >> std::boolalpha
        >> layout.isZIndexSetting >> layout.zIndex
        >> layout.isPositionTypeSetting >> layout.positionType
        >> layout.isPositionXTypeSetting >> layout.pTypeX
        >> layout.isPositionYTypeSetting >> layout.pTypeY
        >> layout.isLayoutSetting >> layout.layout;
    return is;
}
} // namespace OHOS::WMServer
