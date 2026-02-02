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

ani_status AniPathIterator::AniInit(ani_env *env)
{
    ani_class cls = AniGlobalClass::GetInstance().pathIterator;
    if (cls == nullptr) {
        ROSEN_LOGE("[ANI] can't find class: %{public}s", ANI_CLASS_PATH_ITERATOR_NAME);
        return ANI_NOT_FOUND;
    }

    std::array methods = {
        ani_native_function { "constructorNative", "C{@ohos.graphics.drawing.drawing.Path}:",
            reinterpret_cast<void*>(ConstructorWithPath) },
        ani_native_function { "next", nullptr, reinterpret_cast<void*>(Next) },
        ani_native_function { "hasNext", nullptr, reinterpret_cast<void*>(HasNext) },
        ani_native_function { "peek", nullptr, reinterpret_cast<void*>(Peek) },
    };

    ani_status ret = env->Class_BindNativeMethods(cls, methods.data(), methods.size());
    if (ret != ANI_OK) {
        ROSEN_LOGE("[ANI] bind methods fail: %{public}s", ANI_CLASS_PATH_ITERATOR_NAME);
        return ANI_NOT_FOUND;
    }

    return ANI_OK;
}

void AniPathIterator::ConstructorWithPath(ani_env* env, ani_object obj, ani_object aniPathObj)
{
    auto aniPath = GetNativeFromObj<AniPath>(env, aniPathObj, AniGlobalField::GetInstance().pathNativeObj);
    if (aniPath == nullptr || aniPath->GetPath() == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
        return;
    }

    AniPathIterator* newAniPathIterator = new AniPathIterator(*aniPath->GetPath());
    ani_status ret = env->Object_SetField_Long(
        obj, AniGlobalField::GetInstance().pathIteratorNativeObj, reinterpret_cast<ani_long>(newAniPathIterator));
    if (ret != ANI_OK) {
        ROSEN_LOGE("AniPathIterator::Constructor failed create AniPathIterator");
        delete newAniPathIterator;
        return;
    }
}

Drawing::PathVerb AniPathIterator::NextInternal()
{
    if (done_) {
        return Drawing::PathVerb::DONE;
    }
    Drawing::PathVerb verb = pathIterator_.Next(points_);
    if (verb == Drawing::PathVerb::CONIC) {
        float weight = pathIterator_.ConicWeight();
        points_[MAX_PAIRS_PATHVERB - 1].SetX(weight);
    } else if (verb == Drawing::PathVerb::DONE) {
        done_ = true;
    }
    return verb;
}

PathVerb AniPathIterator::GetReturnVerb(const PathVerb& cachedVerb)
{
    switch (cachedVerb) {
        case PathVerb::MOVE: return PathVerb::MOVE;
        case PathVerb::LINE: return PathVerb::LINE;
        case PathVerb::QUAD: return PathVerb::QUAD;
        case PathVerb::CONIC: return PathVerb::CONIC;
        case PathVerb::CUBIC: return PathVerb::CUBIC;
        case PathVerb::CLOSE: return PathVerb::CLOSE;
        case PathVerb::DONE: return PathVerb::DONE;
        default: return NextInternal();
    }
}

ani_object AniPathIterator::OnNext(ani_env* env, ani_array aniPointArray, ani_object aniOffsetObj)
{
    int32_t offset = 0;
    ani_status ret = ANI_OK;
    if (!IsUndefined(env, aniOffsetObj)) {
        ani_int aniOffset = 0;
        if (ANI_OK != env->Object_CallMethod_Int(
            aniOffsetObj, AniGlobalMethod::GetInstance().intGet, &aniOffset)) {
            ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid param offset.");
            return CreateAniUndefined(env);
        }
        if (static_cast<int32_t>(aniOffset) < 0) {
            ROSEN_LOGE("AniPathIterator::Next offset invalid %{public}d.", aniOffset);
            ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid param offset size.");
            return CreateAniUndefined(env);
        }
        offset = static_cast<int32_t>(aniOffset);
    }

    ani_size aniLength;
    ret = env->Array_GetLength(aniPointArray, &aniLength);
    if (ret != ANI_OK) {
        ROSEN_LOGE("AniPathIterator::Next points array invalid. ret: %{public}d", ret);
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid param points.");
        return CreateAniUndefined(env);
    }

    if (static_cast<int32_t>(aniLength) < (offset + MAX_PAIRS_PATHVERB)) {
        ROSEN_LOGE("AniPathIterator::Next array size is incorrect. length: %{public}zu", aniLength);
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid param points array length.");
        return CreateAniUndefined(env);
    }

    PathVerb returnVerb = GetReturnVerb(verb_);
    verb_ = PathVerb::UNINIT;

    for (uint32_t i = 0; i < MAX_PAIRS_PATHVERB; i++) {
        if (!SetPointToAniPointArrayWithIndex(env, points_[i], aniPointArray, i + offset)) {
            return CreateAniUndefined(env);
        }
    }

    ani_enum_item enumItem;
    bool result = CreateAniEnumByEnumIndex(
        env, AniGlobalEnum::GetInstance().pathIteratorVerb, static_cast<ani_size>(returnVerb), enumItem);
    if (!result) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid path verb.");
        return CreateAniUndefined(env);
    }
    return enumItem;
}

ani_object AniPathIterator::Next(ani_env* env, ani_object obj, ani_array aniPointArray, ani_object aniOffsetObj)
{
    auto aniPathIterator = GetNativeFromObj<AniPathIterator>(env, obj,
        AniGlobalField::GetInstance().pathIteratorNativeObj);
    if (aniPathIterator == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
        return CreateAniUndefined(env);
    }
    return aniPathIterator->OnNext(env, aniPointArray, aniOffsetObj);
}

ani_boolean AniPathIterator::OnHasNext(ani_env* env)
{
    if (verb_ == PathVerb::UNINIT) {
        verb_ = NextInternal();
    }
    return verb_ != PathVerb::DONE;
}

ani_boolean AniPathIterator::HasNext(ani_env* env, ani_object obj)
{
    auto aniPathIterator = GetNativeFromObj<AniPathIterator>(env, obj,
        AniGlobalField::GetInstance().pathIteratorNativeObj);
    if (aniPathIterator == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
        return false;
    }
    return aniPathIterator->OnHasNext(env);
}

ani_enum_item AniPathIterator::OnPeek(ani_env* env, PathIterator& pathIterator)
{
    PathVerb pathVerb;
    if (done_) {
        pathVerb = PathVerb::DONE;
    } else {
        pathVerb = pathIterator.Peek();
    }
    ani_enum_item enumItem;
    bool result = CreateAniEnumByEnumIndex(
        env, AniGlobalEnum::GetInstance().pathIteratorVerb, static_cast<ani_size>(pathVerb), enumItem);
    if (!result) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid path verb.");
        return nullptr;
    }
    return enumItem;
}

ani_enum_item AniPathIterator::Peek(ani_env* env, ani_object obj)
{
    auto aniPathIterator = GetNativeFromObj<AniPathIterator>(env, obj,
        AniGlobalField::GetInstance().pathIteratorNativeObj);
    if (aniPathIterator == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
        return nullptr;
    }
    return aniPathIterator->OnPeek(env, aniPathIterator->GetPathIterator());
}

PathIterator& AniPathIterator::GetPathIterator()
{
    return pathIterator_;
}
} // namespace Drawing
} // namespace OHOS::Rosen