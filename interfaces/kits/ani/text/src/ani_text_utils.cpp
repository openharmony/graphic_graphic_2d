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

#include "ani_text_utils.h"

namespace OHOS::Rosen {

ani_object AniTextUtils::CreateAniUndefined(ani_env* env)
{
    ani_ref aniRef;
    env->GetUndefined(&aniRef);
    return static_cast<ani_object>(aniRef);
}

ani_object AniTextUtils::CreateAniObject(ani_env* env, const std::string name, const char* signature, ani_long addr)
{
    ani_class cls;
    if (env->FindClass(name.c_str(), &cls) != ANI_OK) {
        TEXT_LOGE("[ANI] not found %{public}s", name.c_str());
        return CreateAniUndefined(env);
    }
    ani_method ctor;
    if (env->Class_FindMethod(cls, "<ctor>", signature, &ctor) != ANI_OK) {
        TEXT_LOGE("[ANI] get ctor failed %{public}s", name.c_str());
        return CreateAniUndefined(env);
    }
    ani_object obj = {};
    if (env->Object_New(cls, ctor, &obj, addr) != ANI_OK) {
        TEXT_LOGE("[ANI] create object failed %{public}s", name.c_str());
        return CreateAniUndefined(env);
    }
    return obj;
}
ani_object AniTextUtils::CreateAniArray(ani_env* env, size_t size)
{
    ani_class arrayCls;
    if (env->FindClass("Lescompat/Array;", &arrayCls) != ANI_OK) {
        TEXT_LOGE("[ANI] FindClass Lescompat/Array; failed");
        return CreateAniUndefined(env);
    }
    ani_method arrayCtor;
    if (env->Class_FindMethod(arrayCls, "<ctor>", "I:V", &arrayCtor) != ANI_OK) {
        TEXT_LOGE("[ANI] Class_FindMethod <ctor> Failed");
        return CreateAniUndefined(env);
    }
    ani_object arrayObj = nullptr;
    if (env->Object_New(arrayCls, arrayCtor, &arrayObj, size) != ANI_OK) {
        TEXT_LOGE("[ANI] Object_New Array Failed");
        return CreateAniUndefined(env);
    }
    return arrayObj;
}
} // namespace OHOS::Rosen