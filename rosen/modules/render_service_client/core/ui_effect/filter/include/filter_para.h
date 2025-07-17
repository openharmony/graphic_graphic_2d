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

#include <iostream>
#include <memory>
#include <map>
#include <functional>
#include <mutex>

#include <parcel.h>

#include "common/rs_macros.h"

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

class RSC_EXPORT FilterPara {
public:
    enum ParaType {
        NONE,
        BLUR,
        PIXEL_STRETCH,
        WATER_RIPPLE,
        FLY_OUT,
        DISTORT,
        RADIUS_GRADIENT_BLUR,
        DISPLACEMENT_DISTORT,
        COLOR_GRADIENT,
        SOUND_WAVE,
        EDGE_LIGHT,
        BEZIER_WARP,
        DISPERSION,
        HDR_BRIGHTNESS_RATIO,
        CONTENT_LIGHT,
        MASK_TRANSITION,
        DIRECTION_LIGHT,
        VARIABLE_RADIUS_BLUR,
    };

    static constexpr size_t UNMARSHALLING_MAX_VECTOR_SIZE = 65535;
    using UnmarshallingFunc = std::function<bool (Parcel&, std::shared_ptr<FilterPara>&)>;

    FilterPara()  = default;
    virtual ~FilterPara() = default;

    virtual bool Marshalling(Parcel& parcel) const;

    static bool RegisterUnmarshallingCallback(uint16_t type, UnmarshallingFunc func);

    [[nodiscard]] static bool Unmarshalling(Parcel& parcel, std::shared_ptr<FilterPara>& val);

    virtual std::shared_ptr<FilterPara> Clone() const;

    static bool IsWhitelistPara(uint16_t type);

    ParaType GetParaType()
    {
        return type_;
    }
protected:
    ParaType type_;
};
} // namespace Rosen
} // namespace OHOS
#endif // UIEFFECT_FILTER_PARA_H
