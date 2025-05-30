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

#include "ani_path_iterator.h"
#include "draw/path.h"
#include "path_ani/ani_path.h"

namespace OHOS::Rosen {
namespace Drawing {
const char* ANI_CLASS_PATH_ITERATOR_NAME = "L@ohos/graphics/drawing/drawing/PathIterator;";

ani_status AniPathIterator::AniInit(ani_env *env)
{
    ani_class cls = nullptr;
    ani_status ret = env->FindClass(ANI_CLASS_PATH_ITERATOR_NAME, &cls);
    if (ret != ANI_OK) {
        ROSEN_LOGE("[ANI] can't find class: %{public}s", ANI_CLASS_PATH_ITERATOR_NAME);
        return ANI_NOT_FOUND;
    }

    std::array methods = {
        ani_native_function { "constructorNative", "L@ohos/graphics/drawing/drawing/Path;:V",
            reinterpret_cast<void*>(ConstructorWithPath) },
    };

    ret = env->Class_BindNativeMethods(cls, methods.data(), methods.size());
    if (ret != ANI_OK) {
        ROSEN_LOGE("[ANI] bind methods fail: %{public}s", ANI_CLASS_PATH_ITERATOR_NAME);
        return ANI_NOT_FOUND;
    }

    return ANI_OK;
}

void AniPathIterator::ConstructorWithPath(ani_env* env, ani_object obj, ani_object aniPathObj)
{
    auto aniPath = GetNativeFromObj<AniPath>(env, aniPathObj);
    if (aniPath == nullptr) {
        AniThrowError(env, "Invalid params. "); // message length must be a multiple of 4, for example 16, 20, etc
        return;
    }

    AniPathIterator* newAniPathIterator = new AniPathIterator(aniPath->GetPath());
    if (ANI_OK != env->Object_SetFieldByName_Long(obj, NATIVE_OBJ, reinterpret_cast<ani_long>(newAniPathIterator))) {
        ROSEN_LOGE("AniPathIterator::Constructor failed create AniPathIterator");
        delete newAniPathIterator;
        return;
    }
}

PathIterator& AniPathIterator::GetPathIterator()
{
    return pathIterator_;
}
} // namespace Drawing
} // namespace OHOS::Rosen