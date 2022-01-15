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

#ifndef ROSENRENDER_ROSEN_WEBGL_OBJECT_MANAGER
#define ROSENRENDER_ROSEN_WEBGL_OBJECT_MANAGER

#include <map>
#include "../context/webgl_rendering_context_basic_base.h"

#ifdef __cplusplus
extern "C" {
#endif

namespace OHOS {
namespace Rosen {
class ObjectManager {
public:
    static ObjectManager& GetInstance()
    {
        static ObjectManager manager;
        return manager;
    }

    ~ObjectManager() {}

    std::map<std::string, WebGLRenderingContextBasicBase *>& GetWebgl1ObjectMap()
    {
        return webgl1Objects;
    }

    std::map<std::string, WebGLRenderingContextBasicBase *>& GetWebgl2ObjectMap()
    {
        return webgl2Objects;
    }

private:
    ObjectManager() = default;
    ObjectManager(const ObjectManager&) = delete;
    ObjectManager& operator=(const ObjectManager&) = delete;
    std::map<std::string, WebGLRenderingContextBasicBase *> webgl1Objects;
    std::map<std::string, WebGLRenderingContextBasicBase *> webgl2Objects;
};
} // namespace Rosen
} // namespace OHOS

#ifdef __cplusplus
}
#endif

#endif // ROSENRENDER_ROSEN_WEBGL_OBJECT_MANAGER
