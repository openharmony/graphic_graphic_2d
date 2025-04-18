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

#include <fstream>
#include <sstream>

namespace OHOS::Rosen {

ani_object AniTextUtils::CreateAniUndefined(ani_env* env)
{
    ani_ref aniRef;
    env->GetUndefined(&aniRef);
    return static_cast<ani_object>(aniRef);
}

ani_object AniTextUtils::CreateAniObject(ani_env* env, const std::string name, const char* signature)
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
    if (env->Object_New(cls, ctor, &obj) != ANI_OK) {
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

std::string AniTextUtils::AniToStdStringUtf8(ani_env* env, ani_string str)
{
    ani_size strSize;
    if (ANI_OK != env->String_GetUTF8Size(str, &strSize)) {
        TEXT_LOGE("[ANI] String_GetUTF8Size Failed");
        return "";
    }

    std::vector<char> buffer(strSize + 1);
    char* utf8Buffer = buffer.data();

    ani_size bytesWritten = 0;
    if (ANI_OK != env->String_GetUTF8(str, utf8Buffer, strSize + 1, &bytesWritten)) {
        TEXT_LOGE("[ANI] String_GetUTF8 Failed");
        return "";
    }

    utf8Buffer[bytesWritten] = '\0';
    std::string content = std::string(utf8Buffer);
    return content;
}

bool AniTextUtils::ReadFile(const std::string& filePath, std::string& data)
{
    char realPath[PATH_MAX] = { 0 };
    if (realpath(filePath.c_str(), realPath) == nullptr) {
        TEXT_LOGE("Invalid filePath %{public}s", filePath.c_str());
        return false;
    }

    std::ifstream file(realPath);
    if (!file.is_open()) {
        TEXT_LOGE("Failed to open file:%{public}s", filePath.c_str());
        return false;
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    data = buffer.str();
    return true;
}

bool AniTextUtils::SplitAbsoluteFontPath(std::string& absolutePath)
{
    auto iter = absolutePath.find_first_of(':');
    if (iter == std::string::npos) {
        TEXT_LOGE("Failed to find separator in path:%{public}s", absolutePath.c_str());
        return false;
    }
    std::string head = absolutePath.substr(0, iter);
    if ((head == "file" && absolutePath.size() > sizeof("file://") - 1)) {
        absolutePath = absolutePath.substr(iter + 3); // 3 means skip "://"
        // the file format is like "file://system/fonts...",
        return true;
    }

    return false;
}
} // namespace OHOS::Rosen