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
#ifndef GRAPHICS_EFFECT_GE_EXTERNAL_DYNAMIC_LOADER_H
#define GRAPHICS_EFFECT_GE_EXTERNAL_DYNAMIC_LOADER_H

#include <cstdint>

namespace OHOS {
namespace Rosen {
class GEExternalDynamicLoader {
public:
    GEExternalDynamicLoader(const GEExternalDynamicLoader&) = delete;
    GEExternalDynamicLoader operator=(const GEExternalDynamicLoader&) = delete;
    virtual ~GEExternalDynamicLoader();

    static GEExternalDynamicLoader& GetInstance();

    void* CreateGEXObjectByType(uint32_t type, uint32_t len, void* param);

private:
    using CreateGEXObjectByTypeFunc = void* (*)(uint32_t, uint32_t, void*);

    GEExternalDynamicLoader();

    void* libHandle_ = nullptr;
    CreateGEXObjectByTypeFunc createObjectFunc_ = nullptr;
};
} // namespace Rosen
} // namespace OHOS

#endif // GRAPHICS_EFFECT_GE_EXTERNAL_DYNAMIC_LOADER_H
