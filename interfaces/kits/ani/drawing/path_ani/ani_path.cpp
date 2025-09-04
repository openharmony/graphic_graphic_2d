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

#include "ani_path.h"
#include "interop_js/arkts_esvalue.h"
#include "interop_js/arkts_interop_js_api.h"
#include "interop_js/hybridgref_ani.h"
#include "interop_js/hybridgref_napi.h"
#include "drawing/path_napi/js_path.h"

namespace OHOS::Rosen {
namespace Drawing {
const char* ANI_CLASS_PATH_NAME = "L@ohos/graphics/drawing/drawing/Path;";

ani_status AniPath::AniInit(ani_env *env)
{
    ani_class cls = nullptr;
    ani_status ret = env->FindClass(ANI_CLASS_PATH_NAME, &cls);
    if (ret != ANI_OK) {
        ROSEN_LOGE("[ANI] can't find class: %{public}s", ANI_CLASS_PATH_NAME);
        return ANI_NOT_FOUND;
    }

    std::array methods = {
        ani_native_function { "constructorNative", ":V", reinterpret_cast<void*>(Constructor) },
        ani_native_function { "constructorNative", "L@ohos/graphics/drawing/drawing/Path;:V",
            reinterpret_cast<void*>(ConstructorWithPath) },
        ani_native_function { "arcTo", "DDDDDD:V", reinterpret_cast<void*>(ArcTo) },
        ani_native_function { "reset", ":V", reinterpret_cast<void*>(Reset) },
    };

    ret = env->Class_BindNativeMethods(cls, methods.data(), methods.size());
    if (ret != ANI_OK) {
        ROSEN_LOGE("[ANI] bind methods fail: %{public}s", ANI_CLASS_PATH_NAME);
        return ANI_NOT_FOUND;
    }

    std::array staticMethods = {
        ani_native_function { "pathTransferStaticNative", nullptr, reinterpret_cast<void*>(PathTransferStatic) },
        ani_native_function { "getPathAddr", nullptr, reinterpret_cast<void*>(GetPathAddr) },
    };

    ret = env->Class_BindStaticNativeMethods(cls, staticMethods.data(), staticMethods.size());
    if (ret != ANI_OK) {
        ROSEN_LOGE("[ANI] bind static methods fail: %{public}s", ANI_CLASS_PATH_NAME);
        return ANI_NOT_FOUND;
    }

    return ANI_OK;
}

void AniPath::Constructor(ani_env* env, ani_object obj)
{
    std::shared_ptr<Path> path = std::make_shared<Path>();
    AniPath* aniPath = new AniPath(path);
    if (ANI_OK != env->Object_SetFieldByName_Long(obj, NATIVE_OBJ, reinterpret_cast<ani_long>(aniPath))) {
        ROSEN_LOGE("AniPath::Constructor failed create AniPath");
        delete aniPath;
        return;
    }
}

void AniPath::ConstructorWithPath(ani_env* env, ani_object obj, ani_object aniPathObj)
{
    auto aniPath = GetNativeFromObj<AniPath>(env, aniPathObj);
    if (aniPath == nullptr) {
        AniThrowError(env, "Invalid params. "); // message length must be a multiple of 4, for example 16, 20, etc
        return;
    }
    std::shared_ptr<Path> other = aniPath->GetPath();
    std::shared_ptr<Path> path = other == nullptr ? std::make_shared<Path>() : std::make_shared<Path>(*other);
    AniPath* newAniPath = new AniPath(path);
    if (ANI_OK != env->Object_SetFieldByName_Long(obj, NATIVE_OBJ, reinterpret_cast<ani_long>(newAniPath))) {
        ROSEN_LOGE("AniPath::Constructor failed create AniPath");
        delete newAniPath;
        return;
    }
}

void AniPath::Reset(ani_env* env, ani_object obj)
{
    auto aniPath = GetNativeFromObj<AniPath>(env, obj);
    if (aniPath == nullptr || aniPath->GetPath() == nullptr) {
        AniThrowError(env, "Invalid params. ");
        return;
    }

    aniPath->GetPath()->Reset();
}

void AniPath::ArcTo(ani_env* env, ani_object obj, ani_double x1, ani_double y1, ani_double x2, ani_double y2,
    ani_double startDeg, ani_double sweepDeg)
{
    auto aniPath = GetNativeFromObj<AniPath>(env, obj);
    if (aniPath == nullptr || aniPath->GetPath() == nullptr) {
        AniThrowError(env, "Invalid params. ");
        return;
    }

    aniPath->GetPath()->ArcTo(x1, y1, x2, y2, startDeg, sweepDeg);
    return;
}

ani_object AniPath::PathTransferStatic(
    ani_env* env, [[maybe_unused]]ani_object obj, ani_object output, ani_object input)
{
    void* unwrapResult = nullptr;
    bool success = arkts_esvalue_unwrap(env, input, &unwrapResult);
    if (!success) {
        ROSEN_LOGE("AniPath::PathTransferStatic failed to unwrap");
        return nullptr;
    }
    if (unwrapResult == nullptr) {
        ROSEN_LOGE("AniPath::PathTransferStatic unwrapResult is null");
        return nullptr;
    }
    auto jsPath = reinterpret_cast<JsPath*>(unwrapResult);
    if (jsPath->GetPathPtr() == nullptr) {
        ROSEN_LOGE("AniPath::PathTransferStatic jsPath is null");
        return nullptr;
    }

    auto aniPath = new AniPath(jsPath->GetPathPtr());
    if (ANI_OK != env->Object_SetFieldByName_Long(output, NATIVE_OBJ, reinterpret_cast<ani_long>(aniPath))) {
        ROSEN_LOGE("AniPath::PathTransferStatic failed create aniPath");
        delete aniPath;
        return nullptr;
    }
    return output;
}

ani_long AniPath::GetPathAddr(ani_env* env, [[maybe_unused]]ani_object obj, ani_object input)
{
    auto aniPath = GetNativeFromObj<AniPath>(env, input);
    if (aniPath == nullptr || aniPath->GetPath() == nullptr) {
        ROSEN_LOGE("AniPath::GetPathAddr aniPath is null");
        return 0;
    }
    return reinterpret_cast<ani_long>(aniPath->GetPathPtrAddr());
}

std::shared_ptr<Path>* AniPath::GetPathPtrAddr()
{
    return &path_;
}

std::shared_ptr<Path> AniPath::GetPath()
{
    return path_;
}

AniPath::~AniPath()
{
    path_ = nullptr;
}
} // namespace Drawing
} // namespace OHOS::Rosen