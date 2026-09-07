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

#ifndef OHOS_ROSEN_ANI_RECORD_CMD_UTILS_H
#define OHOS_ROSEN_ANI_RECORD_CMD_UTILS_H

#include "ani_drawing_utils.h"
#include "pipeline/rs_record_cmd_utils.h"

namespace OHOS::Rosen {
namespace Drawing {

class AniRecordCmdUtils final {
public:
    explicit AniRecordCmdUtils(std::shared_ptr<RSRecordCmdUtils> rsRecordCmdUtils = nullptr)
        : rsRecordCmdUtils_(rsRecordCmdUtils) {}
    ~AniRecordCmdUtils();

    static ani_status AniInit(ani_env *env);

    static void Constructor(ani_env* env, ani_object obj);
    static ani_object BeginRecording(ani_env* env, ani_object obj, ani_int width, ani_int height);
    static ani_object FinishRecording(ani_env* env, ani_object obj);
    static ani_int GetHeight(ani_env* env, ani_object obj);
    static ani_int GetWidth(ani_env* env, ani_object obj);

    std::shared_ptr<RSRecordCmdUtils> GetRSRecordCmdUtils();

private:
    std::shared_ptr<RSRecordCmdUtils> rsRecordCmdUtils_ = nullptr;
};

} // namespace Drawing
} // namespace OHOS::Rosen
#endif // OHOS_ROSEN_ANI_RECORD_CMD_UTILS_H
