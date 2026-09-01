/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include "ani_record_cmd_utils.h"

#include "ani_drawing_transfer_util.h"
#include "canvas_ani/ani_canvas.h"
#include "draw/canvas.h"
#include "interop_js/arkts_esvalue.h"
#include "interop_js/arkts_interop_js_api.h"
#include "interop_js/hybridgref_ani.h"
#include "interop_js/hybridgref_napi.h"
#include "record_cmd_ani/ani_record_cmd.h"
#include "recording/record_cmd.h"

namespace OHOS::Rosen {
namespace Drawing {

static const std::array g_methods = {
    ani_native_function { "constructorNative", ":", reinterpret_cast<void*>(AniRecordCmdUtils::Constructor) },
    ani_native_function { "beginRecording", "ii:C{@ohos.graphics.drawing.drawing.Canvas}",
        reinterpret_cast<void*>(AniRecordCmdUtils::BeginRecording) },
    ani_native_function { "finishRecording", ":C{@ohos.graphics.drawing.drawing.RecordCmd}",
        reinterpret_cast<void*>(AniRecordCmdUtils::FinishRecording) },
    ani_native_function { "getHeight", ":i", reinterpret_cast<void*>(AniRecordCmdUtils::GetHeight) },
    ani_native_function { "getWidth", ":i", reinterpret_cast<void*>(AniRecordCmdUtils::GetWidth) },
};

ani_status AniRecordCmdUtils::AniInit(ani_env *env)
{
    ani_class cls = AniGlobalClass::GetInstance().recordCmdUtils;
    if (cls == nullptr) {
        ROSEN_LOGE("[ANI] can't find class: %{public}s", ANI_CLASS_RECORD_CMD_UTILS_NAME);
        return ANI_NOT_FOUND;
    }
    ani_status ret = env->Class_BindNativeMethods(cls, g_methods.data(), g_methods.size());
    if (ret != ANI_OK) {
        ROSEN_LOGE("[ANI] bind methods fail: %{public}s", ANI_CLASS_RECORD_CMD_UTILS_NAME);
        return ANI_NOT_FOUND;
    }
    return ANI_OK;
}

void AniRecordCmdUtils::Constructor(ani_env* env, ani_object obj)
{
    std::shared_ptr<RSRecordCmdUtils> rsRecordCmdUtils = std::make_shared<RSRecordCmdUtils>();
    AniRecordCmdUtils* aniRecordCmdUtils = new AniRecordCmdUtils(rsRecordCmdUtils);
    if (ANI_OK != env->Object_SetField_Long(
        obj, AniGlobalField::GetInstance().recordCmdUtilsNativeObj, reinterpret_cast<ani_long>(aniRecordCmdUtils))) {
        ROSEN_LOGE("AniRecordCmdUtils::Constructor failed create aniRecordCmdUtils");
        delete aniRecordCmdUtils;
        return;
    }
}

AniRecordCmdUtils::~AniRecordCmdUtils()
{
    rsRecordCmdUtils_ = nullptr;
}

ani_object AniRecordCmdUtils::BeginRecording(ani_env* env, ani_object obj, ani_int width, ani_int height)
{
    auto* aniRecordCmdUtils = GetNativeFromObj<AniRecordCmdUtils>(env, obj,
        AniGlobalField::GetInstance().recordCmdUtilsNativeObj);
    if (aniRecordCmdUtils == nullptr || aniRecordCmdUtils->GetRSRecordCmdUtils() == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params. ");
        return CreateAniUndefined(env);
    }

    if (width <= 0 || height <= 0) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_PARAM_VERIFICATION_FAILED, "Invalid params.");
        return CreateAniUndefined(env);
    }

    auto bounds = Drawing::Rect(0, 0, width, height);
    Drawing::Canvas *canvas = aniRecordCmdUtils->GetRSRecordCmdUtils()->BeginRecording(bounds);
    if (canvas == nullptr) {
        ROSEN_LOGE("AniRecordCmdUtils::BeginRecording BeginRecording failed");
        ThrowBusinessError(
            env, DrawingErrorCode::ERROR_INVALID_PARAM, "Insufficient memory, failed to create the canvas");
        return CreateAniUndefined(env);
    }

    return AniCanvas::CreateAniCanvas(env, canvas);
}

ani_object AniRecordCmdUtils::FinishRecording(ani_env* env, ani_object obj)
{
    auto* aniRecordCmdUtils = GetNativeFromObj<AniRecordCmdUtils>(env, obj,
        AniGlobalField::GetInstance().recordCmdUtilsNativeObj);
    if (aniRecordCmdUtils == nullptr || aniRecordCmdUtils->GetRSRecordCmdUtils() == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params. ");
        return CreateAniUndefined(env);
    }

    std::shared_ptr<Drawing::RecordCmd> recordCmd = aniRecordCmdUtils->GetRSRecordCmdUtils()->FinishRecording();
    if (recordCmd == nullptr) {
        ROSEN_LOGE("AniRecordCmdUtils::FinishRecording FinishRecording failed");
        return CreateAniUndefined(env);
    }

    AniRecordCmd* aniRecordCmd = new AniRecordCmd(recordCmd);
    ani_object aniObj = CreateAniObject(env, AniGlobalClass::GetInstance().recordCmd,
        AniGlobalMethod::GetInstance().recordCmdCtorWithPtr, reinterpret_cast<ani_long>(aniRecordCmd));
    if (IsUndefined(env, aniObj)) {
        ROSEN_LOGE("AniRecordCmdUtils::FinishRecording failed create aniRecordCmd");
        delete aniRecordCmd;
        return CreateAniUndefined(env);
    }
    return aniObj;
}

ani_int AniRecordCmdUtils::GetHeight(ani_env* env, ani_object obj)
{
    auto* aniRecordCmdUtils = GetNativeFromObj<AniRecordCmdUtils>(env, obj,
        AniGlobalField::GetInstance().recordCmdUtilsNativeObj);
    if (aniRecordCmdUtils == nullptr || aniRecordCmdUtils->GetRSRecordCmdUtils() == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params. ");
        return 0;
    }

    return aniRecordCmdUtils->GetRSRecordCmdUtils()->GetHeight();
}

ani_int AniRecordCmdUtils::GetWidth(ani_env* env, ani_object obj)
{
    auto* aniRecordCmdUtils = GetNativeFromObj<AniRecordCmdUtils>(env, obj,
        AniGlobalField::GetInstance().recordCmdUtilsNativeObj);
    if (aniRecordCmdUtils == nullptr || aniRecordCmdUtils->GetRSRecordCmdUtils() == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params. ");
        return 0;
    }

    return aniRecordCmdUtils->GetRSRecordCmdUtils()->GetWidth();
}

std::shared_ptr<RSRecordCmdUtils> AniRecordCmdUtils::GetRSRecordCmdUtils()
{
    return rsRecordCmdUtils_;
}

} // namespace Drawing
} // namespace OHOS::Rosen
