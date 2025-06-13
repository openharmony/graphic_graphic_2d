/*
 * Copyright (c) 2024-2025 Huawei Device Co., Ltd.
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

#ifndef DRAWING_HELPER_H
#define DRAWING_HELPER_H

#include <memory>

#include "drawing_types.h"
#include "effect/color_filter.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {

template<typename T>
struct NativeHandle {
    std::shared_ptr<T> value = nullptr;
};

class Helper {
public:
    template<typename T1, typename T2>
    static T2 CastTo(T1 type) { return reinterpret_cast<T2>(type); }

    static std::shared_ptr<ColorFilter> ColorFilterCToCpp(OH_Drawing_ColorFilter* cColorFilter)
    {
        if (cColorFilter == nullptr) {
            return nullptr;
        }
        auto colorFilterHandle = Helper::CastTo<OH_Drawing_ColorFilter*, NativeHandle<ColorFilter>*>(cColorFilter);
        return colorFilterHandle->value;
    }

    static OH_Drawing_ColorFilter* ColorFilterCppToC(std::shared_ptr<ColorFilter>* colorFilter)
    {
        if (colorFilter == nullptr) {
            return nullptr;
        }
        NativeHandle<ColorFilter>* handle = reinterpret_cast<NativeHandle<ColorFilter>*>(colorFilter);
        auto cColorFilter = Helper::CastTo<NativeHandle<ColorFilter>*, OH_Drawing_ColorFilter*>(handle);
        return cColorFilter;
    }
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif // DRAWING_HELPER_H