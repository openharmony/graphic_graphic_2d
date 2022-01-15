/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#ifndef WEBGL_SHADER_PRECISION_FORMAT_H_OBJECT_SOURCE_H
#define WEBGL_SHADER_PRECISION_FORMAT_H_OBJECT_SOURCE_H


#include <map>
#include "../context/webgl_rendering_context_basic_base.h"

#ifdef __cplusplus
extern "C" {
#endif

namespace OHOS {
namespace Rosen {
class ObjectSource {
public:
    static ObjectSource& GetInstance()
    {
        static ObjectSource source;
        return source;
    }

    ~ObjectSource() {}

    std::map<int, std::string>& GetObjectMap()
    {
        return objects;
    }

private:
    ObjectSource() = default;
    ObjectSource(const ObjectSource&) = delete;
    ObjectSource& operator=(const ObjectSource&) = delete;
    std::map<int, std::string> objects;
};
} // namespace Rosen
} // namespace OHOS

#ifdef __cplusplus
}
#endif

#endif // WEBGL_SHADER_PRECISION_FORMAT_H_OBJECT_SOURCE_H
