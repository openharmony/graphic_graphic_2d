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

#ifndef OHOS_ROSEN_ANI_TOOL_H
#define OHOS_ROSEN_ANI_TOOL_H

#include "ani_drawing_utils.h"

#ifdef ROSEN_OHOS
#include "resource_manager.h"
#endif

namespace OHOS::Rosen {
namespace Drawing {
#ifdef ROSEN_OHOS
struct ResourceInfo {
    int32_t resId = 0;
    int32_t type = 0;
    std::vector<std::string> params;
};

enum class ResourceType {
    COLOR = 10001,
    STRING = 10003,
    INTEGER = 10007,
    RAWFILE = 30000,
};
#endif
class AniTool final {
public:
    explicit AniTool() = default;
    ~AniTool() = default;

    static ani_status AniInit(ani_env *env);

    static ani_object MakeColorFromResourceColor(ani_env* env, ani_object obj, ani_object resourceColor);

#ifdef ROSEN_OHOS
    static std::shared_ptr<Global::Resource::ResourceManager> GetResourceManager();
    static bool GetResourceInfo(ani_env* env, ani_object obj, ResourceInfo& info);
    static bool GetResourceRawFileDataBuffer(std::unique_ptr<uint8_t[]>&& buffer, size_t& len, ResourceInfo& info);
    static uint32_t HandleIncorrectColor(const std::string& newColorStr);
    static bool GetResourceColor(ani_env* env, ani_object obj, uint32_t& result);
    static uint32_t GetColorNumberResult(uint32_t origin);
    static bool GetColorStringResult(std::string colorStr, uint32_t& result);
    static bool MatchColorWithMagic(std::string& colorStr, uint32_t& result);
    static bool MatchColorWithMagicMini(std::string& colorStr, uint32_t& result);
    static bool MatchColorWithRGB(const std::string& colorStr, uint32_t& result);
    static bool MatchColorWithRGBA(const std::string& colorStr, uint32_t& result);
    static bool FastCheckColorType(const std::string& colorStr, const std::string& expectPrefix,
        const std::vector<size_t>& expectLengths);
    static bool HandleRGBValue(int value, int& result);
    static bool IsOpacityValid(double value);
    static bool GetColorObjectResult(ani_env* env, ani_object value, uint32_t& result);
#endif
private:
    static bool IsStringObject(ani_env* env, ani_object obj);
    static bool IsColorEnum(ani_env* env, ani_object obj);
    static bool IsIntObject(ani_env* env, ani_object obj);
    static bool IsResourceObject(ani_env *env, ani_object object);
    static bool GetTypeParam(ani_env* env, ani_object obj, const char* name, int32_t& result);
    static bool GetParamsArray(ani_env* env, ani_object obj, const char* name, std::vector<std::string>& result);
    static std::vector<std::string> AniToStdVectorString(ani_env* env, ani_array array);
    static bool IsUndefinedObject(ani_env* env, ani_ref objectRef);
};
} // namespace Drawing
} // namespace OHOS::Rosen
#endif // OHOS_ROSEN_ANI_TOOL_H