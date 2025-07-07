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

#ifndef OHOS_ROSEN_ANI_TYPEFACE_ARGUMENTS_H
#define OHOS_ROSEN_ANI_TYPEFACE_ARGUMENTS_H

#include "ani_drawing_utils.h"
#include "text/font_arguments.h"

struct TypefaceArgumentsHelper {
    struct Coordinate {
        uint32_t axis = 0;
        float value = 0.0;
    };

    int fontCollectionIndex = 0;
    std::vector<Coordinate> coordinate;
};

namespace OHOS::Rosen {
namespace Drawing {
class AniTypefaceArguments final {
public:
    AniTypefaceArguments() = default;
    ~AniTypefaceArguments() = default;

    static ani_status AniInit(ani_env *env);

    static void Constructor(ani_env* env, ani_object obj);

    TypefaceArgumentsHelper& GetTypefaceArgumentsHelper();
    static void ConvertToFontArguments(const TypefaceArgumentsHelper& typefaceArgumentsHelper,
        FontArguments fontArguments);
private:
    TypefaceArgumentsHelper typefaceArgumentsHelper_;
};
} // namespace Drawing
} // namespace OHOS::Rosen
#endif // OHOS_ROSEN_ANI_TYPEFACE_ARGUMENTS_H