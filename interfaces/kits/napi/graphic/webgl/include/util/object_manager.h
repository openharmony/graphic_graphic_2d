/*
 * Copyright (c) 2021-2023 Huawei Device Co., Ltd.
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

#ifndef WEBGL_OBJECT_MANAGER_H
#define WEBGL_OBJECT_MANAGER_H

#include <map>

#include "context/webgl_rendering_context_basic_base.h"

namespace OHOS {
namespace Rosen {
class ObjectManager {
public:
    static constexpr std::size_t MAX_WEBGL_CONTEXT_COUNT = 64;

    static ObjectManager& GetInstance()
    {
        static ObjectManager manager;
        return manager;
    }

    ~ObjectManager() {}

    WebGLRenderingContextBasicBase* GetWebGLContext(bool webGL2, const std::string& contextId)
    {
        int index = webGL2 ? 1 : 0;
        auto it = webGLObjects_[index].find(contextId);
        if (it == webGLObjects_[index].end()) {
            return nullptr;
        }
        return static_cast<WebGLRenderingContextBasicBase*>(it->second);
    }

    bool AddWebGLObject(bool webGL2, const std::string& contextId, WebGLRenderingContextBasicBase* obj)
    {
        if (obj == nullptr) {
            return false;
        }
        int index = webGL2 ? 1 : 0;
        auto& objects = webGLObjects_[index];
        if (objects.find(contextId) != objects.end()) {
            return false;
        }
        if (webGLObjects_[0].size() + webGLObjects_[1].size() >= MAX_WEBGL_CONTEXT_COUNT) {
            return false;
        }
        return objects.insert({ contextId, obj }).second;
    }

    void DeleteWebGLObject(bool webGL2, WebGLRenderingContextBasicBase* obj)
    {
        int index = webGL2 ? 1 : 0;
        for (auto iter = webGLObjects_[index].begin(); iter != webGLObjects_[index].end(); iter++) {
            if (iter->second == obj) {
                webGLObjects_[index].erase(iter);
                break;
            }
        }
    }

private:
    ObjectManager() = default;
    ObjectManager(const ObjectManager&) = delete;
    ObjectManager& operator=(const ObjectManager&) = delete;
    std::map<std::string, WebGLRenderingContextBasicBase*> webGLObjects_[2] {};
};
} // namespace Rosen
} // namespace OHOS
#endif // WEBGL_OBJECT_MANAGER_H
