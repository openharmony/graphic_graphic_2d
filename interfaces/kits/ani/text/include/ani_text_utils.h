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

#ifndef OHOS_ANI_TEXT_UTILS_H
#define OHOS_ANI_TEXT_UTILS_H

#include <string>
#include <vector>

#include "ani.h"

#include "utils/text_log.h"

namespace OHOS::Rosen {
constexpr const char* NATIVE_OBJ = "nativeObj";
class AniTextUtils {
public:
    template<typename T>
    static T* GetNativeFromObj(ani_env* env, ani_object obj)
    {
        ani_status ret;
        ani_long nativeObj {};
        if ((ret = env->Object_GetFieldByName_Long(obj, NATIVE_OBJ, &nativeObj)) != ANI_OK) {
            TEXT_LOGE("[ANI] Object_GetField_Long fetch failed");
            return nullptr;
        }
        T* object = reinterpret_cast<T*>(nativeObj);
        if (!object) {
            TEXT_LOGE("[ANI] object is null");
            return nullptr;
        }
        return object;
    };
    static ani_object CreateAniUndefined(ani_env* env);
    static ani_object CreateAniObject(ani_env* env, const std::string name, const char* signature);
    static ani_object CreateAniArray(ani_env* env, size_t size);
    static std::string AniToStdStringUtf8(ani_env* env, ani_string str);
    static std::u16string AniToStdStringUtf16(ani_env* env, ani_string str);
    static bool ReadFile(const std::string& filePath, size_t dataLen, std::unique_ptr<uint8_t[]>& data);
    static bool SplitAbsoluteFontPath(std::string& absolutePath);
    static ani_status ReadOptionalField(ani_env* env, ani_object obj, const char* fieldName, ani_ref& ref);
    static ani_status ReadOptionalDoubleField(ani_env* env, ani_object obj, const char* fieldName, ani_double& value);
    static ani_status ReadOptionalStringField(ani_env* env, ani_object obj, const char* fieldName, std::string& str);
    static ani_status ReadOptionalBoolField(ani_env* env, ani_object obj, const char* fieldName, ani_boolean& value);
};
} // namespace OHOS::Rosen

#endif // OHOS_ANI_TEXT_UTILS_H