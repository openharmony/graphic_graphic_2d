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

#include "ani_region.h"
#include "interop_js/arkts_esvalue.h"
#include "interop_js/arkts_interop_js_api.h"
#include "interop_js/hybridgref_ani.h"
#include "interop_js/hybridgref_napi.h"
#include "drawing/region_napi/js_region.h"
#include "path_ani/ani_path.h"

namespace OHOS::Rosen {
namespace Drawing {

ani_status AniRegion::AniInit(ani_env *env)
{
    ani_class cls = AniGlobalClass::GetInstance().region;
    if (cls == nullptr) {
        ROSEN_LOGE("[ANI] can't find class: %{public}s", ANI_CLASS_REGION_NAME);
        return ANI_NOT_FOUND;
    }

    std::array methods = {
        ani_native_function { "constructorNative", ":", reinterpret_cast<void*>(Constructor) },
        ani_native_function { "constructorNative", "C{@ohos.graphics.drawing.drawing.Region}:",
            reinterpret_cast<void*>(ConstructorWithRegion) },
        ani_native_function { "constructorNative", "iiii:", reinterpret_cast<void*>(ConstructorWithRect) },
        ani_native_function { "setEmpty", ":", reinterpret_cast<void*>(SetEmpty) },
        ani_native_function { "quickRejectRegion", nullptr, reinterpret_cast<void*>(QuickRejectRegion) },
        ani_native_function { "isComplex", nullptr, reinterpret_cast<void*>(IsComplex) },
        ani_native_function { "getBoundaryPath", nullptr, reinterpret_cast<void*>(GetBoundaryPath) },
        ani_native_function { "setRegion", nullptr, reinterpret_cast<void*>(SetRegion) },
        ani_native_function { "isEqual", nullptr, reinterpret_cast<void*>(IsEqual) },
        ani_native_function { "getBounds", nullptr, reinterpret_cast<void*>(GetBounds) },
        ani_native_function { "isEmpty", nullptr, reinterpret_cast<void*>(IsEmpty) },
        ani_native_function { "quickReject", "iiii:z", reinterpret_cast<void*>(QuickReject) },
        ani_native_function { "isRegionContained", "C{@ohos.graphics.drawing.drawing.Region}:z",
            reinterpret_cast<void*>(IsRegionContained) },
        ani_native_function { "op", "C{@ohos.graphics.drawing.drawing.Region}"
            "C{@ohos.graphics.drawing.drawing.RegionOp}:z", reinterpret_cast<void*>(Op) },
        ani_native_function { "setRect", "iiii:z", reinterpret_cast<void*>(SetRect) },
        ani_native_function { "setPath", "C{@ohos.graphics.drawing.drawing.Path}"
            "C{@ohos.graphics.drawing.drawing.Region}:z", reinterpret_cast<void*>(SetPath) },
        ani_native_function { "isPointContained", "ii:z", reinterpret_cast<void*>(IsPointContained) },
        ani_native_function { "isRect", ":z", reinterpret_cast<void*>(IsRect) },
        ani_native_function { "quickContains", "iiii:z", reinterpret_cast<void*>(QuickContains) },
        ani_native_function { "offset", "ii:", reinterpret_cast<void*>(Offset) },
    };

    ani_status ret = env->Class_BindNativeMethods(cls, methods.data(), methods.size());
    if (ret != ANI_OK) {
        ROSEN_LOGE("[ANI] bind methods fail: %{public}s", ANI_CLASS_REGION_NAME);
        return ANI_NOT_FOUND;
    }

    std::array staticMethods = {
        ani_native_function { "regionTransferStaticNative", nullptr, reinterpret_cast<void*>(RegionTransferStatic) },
        ani_native_function { "getRegionAddr", nullptr, reinterpret_cast<void*>(GetRegionAddr) },
    };

    ret = env->Class_BindStaticNativeMethods(cls, staticMethods.data(), staticMethods.size());
    if (ret != ANI_OK) {
        ROSEN_LOGE("[ANI] bind static methods fail: %{public}s", ANI_CLASS_REGION_NAME);
        return ANI_NOT_FOUND;
    }

    return ANI_OK;
}

void AniRegion::Constructor(ani_env* env, ani_object obj)
{
    std::shared_ptr<Region> region = std::make_shared<Region>();
    AniRegion* aniRegion = new AniRegion(region);
    if (ANI_OK != env->Object_SetField_Long(
        obj, AniGlobalField::GetInstance().regionNativeObj, reinterpret_cast<ani_long>(aniRegion))) {
        ROSEN_LOGE("AniRegion::Constructor failed create AniRegion");
        delete aniRegion;
        return;
    }
}

void AniRegion::ConstructorWithRegion(ani_env* env, ani_object obj, ani_object aniRegionObj)
{
    auto aniRegion = GetNativeFromObj<AniRegion>(env, aniRegionObj, AniGlobalField::GetInstance().regionNativeObj);
    if (aniRegion == nullptr) {
        AniThrowError(env, "Invalid params. "); // message length must be a multiple of 4, for example 16, 20, etc
        return;
    }
    std::shared_ptr<Region> other = aniRegion->GetRegion();
    std::shared_ptr<Region> region = other == nullptr ? std::make_shared<Region>() : std::make_shared<Region>(*other);
    AniRegion* otherAniRegion = new AniRegion(region);
    if (ANI_OK != env->Object_SetField_Long(
        obj, AniGlobalField::GetInstance().regionNativeObj, reinterpret_cast<ani_long>(otherAniRegion))) {
        ROSEN_LOGE("AniRegion::Constructor failed create AniRegion");
        delete otherAniRegion;
        return;
    }
}

void AniRegion::ConstructorWithRect(ani_env* env, ani_object obj, ani_int left, ani_int top, ani_int right,
    ani_int bottom)
{
    std::shared_ptr<Region> region = std::make_shared<Region>();
    AniRegion* aniRegion = new AniRegion(region);
    RectI rect = Drawing::RectI(left, top, right, bottom);
    aniRegion->GetRegion()->SetRect(rect);
    if (ANI_OK != env->Object_SetField_Long(
        obj, AniGlobalField::GetInstance().regionNativeObj, reinterpret_cast<ani_long>(aniRegion))) {
        ROSEN_LOGE("AniRegion::Constructor failed create AniRegion");
        delete aniRegion;
        return;
    }
}

void AniRegion::SetEmpty(ani_env* env, ani_object obj)
{
    auto aniRegion = GetNativeFromObj<AniRegion>(env, obj, AniGlobalField::GetInstance().regionNativeObj);
    if (aniRegion == nullptr || aniRegion->GetRegion() == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "AniRegion::SetEmpty invalid params: obj. ");
        return;
    }

    aniRegion->GetRegion()->SetEmpty();
}

ani_boolean AniRegion::QuickReject(ani_env* env, ani_object obj, ani_int left, ani_int top, ani_int right,
    ani_int bottom)
{
    auto aniRegion = GetNativeFromObj<AniRegion>(env, obj, AniGlobalField::GetInstance().regionNativeObj);
    if (aniRegion == nullptr || aniRegion->GetRegion() == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "AniRegion::QuickReject aniRegion is nullptr.");
        return false;
    }
    Drawing::RectI rectI = Drawing::RectI(left, top, right, bottom);
    return aniRegion->GetRegion()->QuickReject(rectI);
}

ani_boolean AniRegion::SetRect(ani_env* env, ani_object obj, ani_int left, ani_int top, ani_int right,
    ani_int bottom)
{
    auto aniRegion = GetNativeFromObj<AniRegion>(env, obj, AniGlobalField::GetInstance().regionNativeObj);
    if (aniRegion == nullptr || aniRegion->GetRegion() == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "AniRegion::SetRect aniRegion is nullptr.");
        return false;
    }
    Drawing::RectI rectI = Drawing::RectI(left, top, right, bottom);
    return aniRegion->GetRegion()->SetRect(rectI);
}

ani_boolean AniRegion::IsRegionContained(ani_env* env, ani_object obj, ani_object aniRegionObj)
{
    auto aniRegion = GetNativeFromObj<AniRegion>(env, obj, AniGlobalField::GetInstance().regionNativeObj);
    if (aniRegion == nullptr || aniRegion->GetRegion() == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "AniRegion::IsRegionContained aniRegion is nullptr.");
        return false;
    }
    auto otherAniRegion = GetNativeFromObj<AniRegion>(env, aniRegionObj, AniGlobalField::GetInstance().regionNativeObj);
    if (otherAniRegion == nullptr || otherAniRegion->GetRegion() == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "AniRegion::IsRegionContained other aniRegion is nullptr.");
        return false;
    }

    return aniRegion->GetRegion()->IsRegionContained(*otherAniRegion->GetRegion());
}

ani_boolean AniRegion::QuickRejectRegion(ani_env* env, ani_object obj, ani_object aniRegionobj)
{
    auto aniRegion = GetNativeFromObj<AniRegion>(env, obj, AniGlobalField::GetInstance().regionNativeObj);
    if (aniRegion == nullptr || aniRegion->GetRegion() == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "AniRegion::QuickRejectRegion aniRegion is nullptr.");
        return false;
    }

    auto otherAniRegion = GetNativeFromObj<AniRegion>(env, aniRegionobj, AniGlobalField::GetInstance().regionNativeObj);
    if (otherAniRegion == nullptr || otherAniRegion->GetRegion() == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "AniRegion::QuickRejectRegion other aniRegion is nullptr.");
        return false;
    }

    return aniRegion->GetRegion()->QuickReject(*otherAniRegion->GetRegion());
}

ani_boolean AniRegion::IsComplex(ani_env* env, ani_object obj)
{
    auto aniRegion = GetNativeFromObj<AniRegion>(env, obj, AniGlobalField::GetInstance().regionNativeObj);
    if (aniRegion == nullptr || aniRegion->GetRegion() == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "AniRegion::IsComplex aniRegion is nullptr.");
        return false;
    }

    return aniRegion->GetRegion()->IsComplex();
}

ani_object AniRegion::GetBoundaryPath(ani_env* env, ani_object obj)
{
    auto aniRegion = GetNativeFromObj<AniRegion>(env, obj, AniGlobalField::GetInstance().regionNativeObj);
    if (aniRegion == nullptr || aniRegion->GetRegion() == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "AniRegion::GetBoundaryPath aniRegion is nullptr.");
        return CreateAniUndefined(env);
    }
    std::shared_ptr<Path> path = std::make_shared<Path>();
    aniRegion->GetRegion()->GetBoundaryPath(path.get());

    ani_object aniObj = CreateAniObject(env, AniGlobalClass::GetInstance().path,
        AniGlobalMethod::GetInstance().pathCtor);
    if (IsUndefined(env, aniObj)) {
        ROSEN_LOGE("AniRegion::GetBoundaryPath failed cause aniObj is undefined");
        return aniObj;
    }
    AniPath* newAniPath = new AniPath(path);
    ani_status ret = env->Object_SetField_Long(
        aniObj, AniGlobalField::GetInstance().pathNativeObj, reinterpret_cast<ani_long>(newAniPath));
    if (ret != ANI_OK) {
        ROSEN_LOGE("AniRegion::GetBoundaryPath create new path failed %{public}d", ret);
        delete newAniPath;
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid param path.");
        return CreateAniUndefined(env);
    }
    return aniObj;
}

void AniRegion::SetRegion(ani_env* env, ani_object obj, ani_object aniRegionobj)
{
    auto aniRegion = GetNativeFromObj<AniRegion>(env, obj, AniGlobalField::GetInstance().regionNativeObj);
    if (aniRegion == nullptr || aniRegion->GetRegion() == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "AniRegion::SetRegion aniRegion is nullptr.");
        return;
    }

    auto otherAniRegion = GetNativeFromObj<AniRegion>(env, aniRegionobj, AniGlobalField::GetInstance().regionNativeObj);
    if (otherAniRegion == nullptr || otherAniRegion->GetRegion() == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "AniRegion::SetRegion other aniRegion is nullptr.");
        return;
    }
    aniRegion->GetRegion()->SetRegion(*otherAniRegion->GetRegion());
}

ani_boolean AniRegion::IsEqual(ani_env* env, ani_object obj, ani_object aniOtherobj)
{
    auto aniRegion = GetNativeFromObj<AniRegion>(env, obj, AniGlobalField::GetInstance().regionNativeObj);
    if (aniRegion == nullptr || aniRegion->GetRegion() == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "AniRegion::SetRegion aniRegion is nullptr.");
        return false;
    }

    auto otherAniRegion = GetNativeFromObj<AniRegion>(env, aniOtherobj, AniGlobalField::GetInstance().regionNativeObj);
    if (otherAniRegion == nullptr || otherAniRegion->GetRegion() == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "AniRegion::SetRegion other aniRegion is nullptr.");
        return false;
    }

    return aniRegion->GetRegion()->operator == (*otherAniRegion->GetRegion());
}

ani_boolean AniRegion::IsEmpty(ani_env* env, ani_object obj)
{
    auto aniRegion = GetNativeFromObj<AniRegion>(env, obj, AniGlobalField::GetInstance().regionNativeObj);
    if (aniRegion == nullptr || aniRegion->GetRegion() == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "AniRegion::IsEmpty aniRegion is nullptr.");
        return false;
    }

    return aniRegion->GetRegion()->IsEmpty();
}

ani_object AniRegion::GetBounds(ani_env* env, ani_object obj)
{
    auto aniRegion = GetNativeFromObj<AniRegion>(env, obj, AniGlobalField::GetInstance().regionNativeObj);
    if (aniRegion == nullptr || aniRegion->GetRegion() == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "AniRegion::GetBounds aniRegion is nullptr.");
        return CreateAniUndefined(env);
    }

    Drawing::RectI src = aniRegion->GetRegion()->GetBounds();
    Drawing::Rect rect = Drawing::Rect(src.GetLeft(), src.GetTop(), src.GetRight(), src.GetBottom());

    ani_object aniRectObj;
    ani_status ret = CreateRectObj(env, rect, aniRectObj);
    if (ret != ANI_OK) {
        ROSEN_LOGE("Set AniRectObj From rect failed.");
        return CreateAniUndefined(env);
    }
    return aniRectObj;
}

ani_boolean AniRegion::Op(ani_env* env, ani_object obj, ani_object aniRegionObj, ani_enum_item aniRegionOp)
{
    auto aniRegion = GetNativeFromObj<AniRegion>(env, obj, AniGlobalField::GetInstance().regionNativeObj);
    if (aniRegion == nullptr || aniRegion->GetRegion() == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "AniRegion::Op aniRegion is nullptr.");
        return false;
    }
    auto otherAniRegion = GetNativeFromObj<AniRegion>(env, aniRegionObj, AniGlobalField::GetInstance().regionNativeObj);
    if (otherAniRegion == nullptr || otherAniRegion->GetRegion() == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "AniRegion::Op other aniRegion is nullptr.");
        return false;
    }

    ani_int regionOp;
    if (ANI_OK != env->EnumItem_GetValue_Int(aniRegionOp, &regionOp)) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "AniRegion::Op get RegionOp enum failed.");
        return false;
    }

    RegionOp op = static_cast<RegionOp>(regionOp);
    if (op < RegionOp::DIFFERENCE || op > RegionOp::REPLACE) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "regionOp is error param.");
        return false;
    }

    return aniRegion->GetRegion()->Op(*otherAniRegion->GetRegion(), op);
}

ani_boolean AniRegion::SetPath(ani_env* env, ani_object obj, ani_object aniPathObj, ani_enum_item aniClipRegion)
{
    auto aniRegion = GetNativeFromObj<AniRegion>(env, obj, AniGlobalField::GetInstance().regionNativeObj);
    if (aniRegion == nullptr || aniRegion->GetRegion() == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "AniRegion::SetPath aniRegion is nullptr.");
        return false;
    }
    auto aniPath = GetNativeFromObj<AniPath>(env, aniPathObj, AniGlobalField::GetInstance().pathNativeObj);
    if (aniPath == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "AniRegion::SetPath incorrect param0.");
        return false;
    }
    if (aniPath->GetPath() == nullptr) {
        ROSEN_LOGE("AniRegion::SetPath aniPath is nullptr");
        return false;
    }

    auto clip = GetNativeFromObj<AniRegion>(env, aniClipRegion, AniGlobalField::GetInstance().regionNativeObj);
    if (clip == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "AniRegion::SetPath incorrect param1.");
        return false;
    }
    if (clip->GetRegion() == nullptr) {
        ROSEN_LOGE("AniRegion::SetPath aniPath is nullptr");
        return false;
    }
    return aniRegion->GetRegion()->SetPath(*aniPath->GetPath(), *clip->GetRegion());
}

ani_boolean AniRegion::IsPointContained(ani_env* env, ani_object obj, ani_int dx, ani_int dy)
{
    auto aniRegion = GetNativeFromObj<AniRegion>(env, obj, AniGlobalField::GetInstance().regionNativeObj);
    if (aniRegion == nullptr || aniRegion->GetRegion() == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "AniRegion::IsPointContained aniRegion is nullptr.");
        return false;
    }
    return aniRegion->GetRegion()->Contains(dx, dy);
}

ani_boolean AniRegion::IsRect(ani_env* env, ani_object obj)
{
    auto aniRegion = GetNativeFromObj<AniRegion>(env, obj, AniGlobalField::GetInstance().regionNativeObj);
    if (aniRegion == nullptr || aniRegion->GetRegion() == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "AniRegion::IsRect aniRegion is nullptr.");
        return false;
    }
    return aniRegion->GetRegion()->IsRect();
}

ani_boolean AniRegion::QuickContains(ani_env* env, ani_object obj, ani_int left, ani_int top, ani_int right,
    ani_int bottom)
{
    auto aniRegion = GetNativeFromObj<AniRegion>(env, obj, AniGlobalField::GetInstance().regionNativeObj);
    if (aniRegion == nullptr || aniRegion->GetRegion() == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "AniRegion::QuickContains aniRegion is nullptr.");
        return false;
    }
    RectI rectI = Drawing::RectI(left, top, right, bottom);
    return aniRegion->GetRegion()->QuickContains(rectI);
}

void AniRegion::Offset(ani_env* env, ani_object obj, ani_int dx, ani_int dy)
{
    auto aniRegion = GetNativeFromObj<AniRegion>(env, obj, AniGlobalField::GetInstance().regionNativeObj);
    if (aniRegion == nullptr || aniRegion->GetRegion() == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "AniRegion::Offset aniRegion is nullptr.");
        return;
    }
    aniRegion->GetRegion()->Translate(dx, dy);
}

ani_object AniRegion::RegionTransferStatic(
    ani_env* env, [[maybe_unused]]ani_object obj, ani_object output, ani_object input)
{
    void* unwrapResult = nullptr;
    bool success = arkts_esvalue_unwrap(env, input, &unwrapResult);
    if (!success) {
        ROSEN_LOGE("AniRegion::RegionTransferStatic failed to unwrap");
        return CreateAniUndefined(env);
    }
    if (unwrapResult == nullptr) {
        ROSEN_LOGE("AniRegion::RegionTransferStatic unwrapResult is null");
        return CreateAniUndefined(env);
    }
    auto jsRegion = reinterpret_cast<JsRegion*>(unwrapResult);
    if (jsRegion->GetRegionPtr() == nullptr) {
        ROSEN_LOGE("AniRegion::RegionTransferStatic jsRegion is null");
        return CreateAniUndefined(env);
    }

    auto aniRegion = new AniRegion(jsRegion->GetRegionPtr());
    if (ANI_OK != env->Object_SetField_Long(
        output, AniGlobalField::GetInstance().regionNativeObj, reinterpret_cast<ani_long>(aniRegion))) {
        ROSEN_LOGE("AniFont::RegionTransferStatic failed create aniFont");
        delete aniRegion;
        return CreateAniUndefined(env);
    }
    return output;
}

ani_long AniRegion::GetRegionAddr(ani_env* env, [[maybe_unused]]ani_object obj, ani_object input)
{
    auto aniRegion = GetNativeFromObj<AniRegion>(env, input, AniGlobalField::GetInstance().regionNativeObj);
    if (aniRegion == nullptr || aniRegion->GetRegion() == nullptr) {
        ROSEN_LOGE("AniRegion::GetRegionAddr aniRegion is null");
        return 0;
    }
    return reinterpret_cast<ani_long>(aniRegion->GetRegionPtrAddr());
}

std::shared_ptr<Region>* AniRegion::GetRegionPtrAddr()
{
    return &region_;
}

std::shared_ptr<Region> AniRegion::GetRegion()
{
    return region_;
}

AniRegion::~AniRegion()
{
    region_ = nullptr;
}
} // namespace Drawing
} // namespace OHOS::Rosen