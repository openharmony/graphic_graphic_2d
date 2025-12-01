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

#include "ani_drawing_cache_utils.h"
#include "ani_drawing_log.h"
#include "ani_drawing_common.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {

namespace {
constexpr DescriptorConfig FONT_METRICS_CTOR{ "<ctor>", "iddddddddddddddd:" };
constexpr DescriptorConfig POINT_CTOR{ "<ctor>", "dd:" };
constexpr DescriptorConfig RECT_CTOR{ "<ctor>", "dddd:" };
constexpr DescriptorConfig COLOR_CTOR{ "<ctor>", "iiii:" };
constexpr DescriptorConfig COLOR_GET_ALPHA{ "%%get-alpha", ":i" };
constexpr DescriptorConfig COLOR_GET_RED{ "%%get-red", ":i" };
constexpr DescriptorConfig COLOR_GET_GREEN{ "%%get-green", ":i" };
constexpr DescriptorConfig COLOR_GET_BLUE{ "%%get-blue", ":i" };
constexpr DescriptorConfig RECT_GET_LEFT{ "%%get-left", ":d" };
constexpr DescriptorConfig RECT_GET_TOP{ "%%get-top", ":d" };
constexpr DescriptorConfig RECT_GET_RIGHT{ "%%get-right", ":d" };
constexpr DescriptorConfig RECT_GET_BOTTOM{ "%%get-bottom", ":d" };
constexpr DescriptorConfig POINT_GET_X{ "%%get-x", ":d" };
constexpr DescriptorConfig POINT_GET_Y{ "%%get-y", ":d" };
}
ani_class AniFindClass(ani_env* env, const char* descriptor)
{
    ani_class cls = nullptr;
    ani_status status = env->FindClass(descriptor, &cls);
    if (status != ANI_OK) {
        ROSEN_LOGE("[Drawing] AniFindClass FindClass failed class: %{public}s, status %{public}d", descriptor, status);
        return nullptr;
    }
    ani_ref ref = nullptr;
    status = env->GlobalReference_Create(cls, &ref);
    if (status != ANI_OK) {
        ROSEN_LOGE("[Drawing] AniFindClass GlobalReference_Create failed class: %{public}s, status %{public}d",
            descriptor, status);
        return nullptr;
    }
    return static_cast<ani_class>(ref);
}

ani_method AniFindMethod(ani_env* env, ani_class cls, const DescriptorConfig& config)
{
    if (cls == nullptr) {
        ROSEN_LOGE("Failed to find method: cls is null");
        return nullptr;
    }
    ani_method method = nullptr;
    ani_status status = env->Class_FindMethod(cls, std::string(config.methodDes).c_str(),
        std::string(config.paramDes).c_str(), &method);
    if (status != ANI_OK) {
        ROSEN_LOGE("Failed to find method: %{public}s %{public}s, status %{public}d",
            std::string(config.methodDes).c_str(), std::string(config.paramDes).c_str(), status);
        return nullptr;
    }
    return method;
}

void AniGlobalClass::Init(ani_env* env)
{
    color = AniFindClass(env, ANI_CLASS_COLOR_NAME);
    fontMetrics = AniFindClass(env, ANI_CLASS_FONT_METRICS_NAME);
    point = AniFindClass(env, ANI_CLASS_POINT_NAME);
    rect = AniFindClass(env, ANI_CLASS_RECT_NAME);
    colorInterface = AniFindClass(env, ANI_INTERFACE_COLOR_NAME);
    pointInterface = AniFindClass(env, ANI_INTERFACE_POINT_NAME);
    rectInterface = AniFindClass(env, ANI_INTERFACE_RECT_NAME);
}

void AniGlobalMethod::Init(ani_env* env)
{
    colorCtor = AniFindMethod(env, AniGlobalClass::GetInstance().color, COLOR_CTOR);
    fontMetricsCtor = AniFindMethod(env, AniGlobalClass::GetInstance().fontMetrics, FONT_METRICS_CTOR);
    pointCtor = AniFindMethod(env, AniGlobalClass::GetInstance().point, POINT_CTOR);
    rectCtor = AniFindMethod(env, AniGlobalClass::GetInstance().rect, RECT_CTOR);

    colorGetAlpha = AniFindMethod(env, AniGlobalClass::GetInstance().colorInterface, COLOR_GET_ALPHA);
    colorGetRed = AniFindMethod(env, AniGlobalClass::GetInstance().colorInterface, COLOR_GET_RED);
    colorGetGreen = AniFindMethod(env, AniGlobalClass::GetInstance().colorInterface, COLOR_GET_GREEN);
    colorGetBlue = AniFindMethod(env, AniGlobalClass::GetInstance().colorInterface, COLOR_GET_BLUE);
    rectGetLeft = AniFindMethod(env, AniGlobalClass::GetInstance().rectInterface, RECT_GET_LEFT);
    rectGetTop = AniFindMethod(env, AniGlobalClass::GetInstance().rectInterface, RECT_GET_TOP);
    rectGetRight = AniFindMethod(env, AniGlobalClass::GetInstance().rectInterface, RECT_GET_RIGHT);
    rectGetBottom = AniFindMethod(env, AniGlobalClass::GetInstance().rectInterface, RECT_GET_BOTTOM);
    pointGetX = AniFindMethod(env, AniGlobalClass::GetInstance().pointInterface, POINT_GET_X);
    pointGetY = AniFindMethod(env, AniGlobalClass::GetInstance().pointInterface, POINT_GET_Y);
}

ani_status InitAniGlobalRef(ani_env* env)
{
    AniGlobalClass::GetInstance().Init(env);
    AniGlobalMethod::GetInstance().Init(env);
    return ANI_OK;
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS