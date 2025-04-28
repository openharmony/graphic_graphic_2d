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

#include "ani_drawing_utils.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {

ani_status AniThrowError(ani_env* env, const std::string& message)
{
    ani_class errCls;
    const char* className = "Lescompat/Error;";
    if (ANI_OK != env->FindClass(className, &errCls)) {
        ROSEN_LOGE("Not found %{public}s", className);
        return ANI_ERROR;
    }

    ani_object errObj;
    ani_method errCtor;
    if (ANI_OK != env->Class_FindMethod(errCls, "<ctor>", "Lstd/core/String;Lescompat/ErrorOptions;:V", &errCtor)) {
        ROSEN_LOGE("get errCtor Failed %{public}s", className);
        return ANI_ERROR;
    }

    ani_string result_string{};
    env->String_NewUTF8(message.c_str(), message.size(), &result_string);

    if (ANI_OK != env->Object_New(errCls, errCtor, &errObj, result_string)) {
        ROSEN_LOGE("Create Object Failedd %{public}s", className);
        return ANI_ERROR;
    }
    env->ThrowError(static_cast<ani_error>(errObj));

    return ANI_OK;
}

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS