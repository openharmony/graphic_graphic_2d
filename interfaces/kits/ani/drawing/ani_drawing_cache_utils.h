/*
 * Copyright (C) 2025 Huawei Device Co., Ltd.
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

#ifndef OHOS_ANI_DRAWING_CACHE_UTILS_H
#define OHOS_ANI_DRAWING_CACHE_UTILS_H

#include <ani.h>
#include <string>
#include <string_view>

namespace OHOS {
namespace Rosen {
namespace Drawing {

ani_status InitAniGlobalRef(ani_env* env);

struct DescriptorConfig {
    std::string_view methodDes;
    std::string_view paramDes;
};

ani_class AniFindClass(ani_env* env, const char* descriptor);

ani_method AniFindMethod(ani_env* env, ani_class cls, const DescriptorConfig& config);

class AniGlobalClass {
public:
    static AniGlobalClass& GetInstance()
    {
        static AniGlobalClass instance;
        return instance;
    }

    void Init(ani_env* env);

    ani_class fontMetrics;
    ani_class point;
    ani_class rect;
    ani_class color;
    ani_class colorInterface;
    ani_class pointInterface;
    ani_class rectInterface;

private:
    AniGlobalClass() = default;
    ~AniGlobalClass() = default;
    AniGlobalClass(const AniGlobalClass&) = delete;
    AniGlobalClass& operator=(const AniGlobalClass&) = delete;
    AniGlobalClass(AniGlobalClass&&) = delete;
    AniGlobalClass& operator=(AniGlobalClass&&) = delete;
};

class AniGlobalMethod {
public:
    static AniGlobalMethod& GetInstance()
    {
        static AniGlobalMethod instance;
        return instance;
    }

    void Init(ani_env* env);

    ani_method fontMetricsCtor;
    ani_method pointCtor;
    ani_method rectCtor;
    ani_method colorCtor;

    ani_method colorGetAlpha;
    ani_method colorGetRed;
    ani_method colorGetGreen;
    ani_method colorGetBlue;
    ani_method rectGetLeft;
    ani_method rectGetTop;
    ani_method rectGetRight;
    ani_method rectGetBottom;
    ani_method pointGetX;
    ani_method pointGetY;

private:
    AniGlobalMethod() = default;
    ~AniGlobalMethod() = default;
    AniGlobalMethod(const AniGlobalMethod&) = delete;
    AniGlobalMethod& operator=(const AniGlobalMethod&) = delete;
    AniGlobalMethod(AniGlobalMethod&&) = delete;
    AniGlobalMethod& operator=(AniGlobalMethod&&) = delete;
};

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif // OHOS_ANI_DRAWING_CACHE_UTILS_H