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

#include "ani_record_cmd.h"

#include "ani_drawing_transfer_util.h"
#include "drawing/record_cmd_napi/js_record_cmd.h"
#include "interop_js/arkts_esvalue.h"
#include "interop_js/arkts_interop_js_api.h"
#include "interop_js/hybridgref_ani.h"
#include "interop_js/hybridgref_napi.h"

namespace OHOS::Rosen {
namespace Drawing {

static const std::array g_methods = {
    ani_native_function { "constructorNative", ":", reinterpret_cast<void*>(AniRecordCmd::Constructor) },
};

ani_status AniRecordCmd::AniInit(ani_env *env)
{
    ani_class cls = AniGlobalClass::GetInstance().recordCmd;
    if (cls == nullptr) {
        ROSEN_LOGE("[ANI] can't find class: %{public}s", ANI_CLASS_RECORD_CMD_NAME);
        return ANI_NOT_FOUND;
    }
    ani_status ret = env->Class_BindNativeMethods(cls, g_methods.data(), g_methods.size());
    if (ret != ANI_OK) {
        ROSEN_LOGE("[ANI] bind methods fail: %{public}s", ANI_CLASS_RECORD_CMD_NAME);
        return ANI_NOT_FOUND;
    }
    return ANI_OK;
}

void AniRecordCmd::Constructor(ani_env* env, ani_object obj)
{
    std::shared_ptr<RecordCmd> recordCmd = std::make_shared<RecordCmd>(nullptr, Drawing::Rect());
    AniRecordCmd* aniRecordCmd = new AniRecordCmd(recordCmd);
    if (ANI_OK != env->Object_SetField_Long(
        obj, AniGlobalField::GetInstance().recordCmdNativeObj, reinterpret_cast<ani_long>(aniRecordCmd))) {
        ROSEN_LOGE("AniRecordCmd::Constructor failed create aniRecordCmd");
        delete aniRecordCmd;
        return;
    }
}

AniRecordCmd::~AniRecordCmd()
{
    recordCmd_ = nullptr;
}

std::shared_ptr<RecordCmd> AniRecordCmd::GetRecordCmd()
{
    return recordCmd_;
}

} // namespace Drawing
} // namespace OHOS::Rosen
