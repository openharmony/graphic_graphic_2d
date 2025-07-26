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

#ifndef OHOS_ANI_COLOR_SPACE_MANAGER_H
#define OHOS_ANI_COLOR_SPACE_MANAGER_H

#include "color_space.h"
#include "ani.h"

namespace OHOS {
namespace ColorManager {
class AniColorSpaceManager {
public:
    explicit AniColorSpaceManager(const std::shared_ptr<ColorSpace>& colorSpace) : colorSpaceToken_(colorSpace) {};
    ~AniColorSpaceManager() {};
    
    static ani_status AniColorSpaceManagerInit(ani_env *env);
    static ani_object CreateByColorSpace(ani_env* env, ani_enum_item enumObj);
    static ani_object CreateByColorSpacePrimaries(ani_env* env, ani_object aniPrimaries, ani_double gamma);
        
    static ani_enum_item GetColorSpaceName(ani_env *env, ani_object obj);
    static ani_ref GetWhitePoint(ani_env *env, ani_object obj);
    static ani_double GetGamma(ani_env *env, ani_object obj);
    static bool ParseColorSpacePrimaries(ani_env *env, ani_object obj, ColorSpacePrimaries& primaries);
    static AniColorSpaceManager* unwrap(ani_env *env, ani_object object);
    
    inline const std::shared_ptr<ColorSpace>& GetColorSpaceToken() const
    {
        return colorSpaceToken_;
    }

private:
    ani_enum_item OnGetColorSpaceName(ani_env *env, ani_object obj);
    ani_ref OnGetWhitePoint(ani_env *env, ani_object obj);
    ani_double OnGetGamma(ani_env *env, ani_object obj);

    std::shared_ptr<ColorSpace> colorSpaceToken_;
};
} // namespace ColorManager
} // namespace OHOS

#endif // OHOS_ANI_COLOR_SPACE_MANAGER_H
