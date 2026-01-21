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

#include "render/rs_typeface_cache.h"
#include "text/typeface.h"
#include "transaction/rs_interfaces.h"
#include "utils/typeface_map.h"

namespace OHOS::Rosen {
class TypefaceAutoRegister {
public:
    TypefaceAutoRegister()
    {
#ifdef ARKUI_X_ENABLE
        auto registerCB = [](std::shared_ptr<Drawing::Typeface> tf) { return 1; };
        auto destroyedCB = [](uint32_t id) {};
#else
        auto registerCB = [](std::shared_ptr<Drawing::Typeface> tf) {
            return Rosen::RSInterfaces::GetInstance().RegisterTypeface(tf);
        };
        auto destroyedCB = [](uint32_t id) { Rosen::RSInterfaces::GetInstance().UnRegisterTypeface(id); };
#endif
        auto uniqueIdCB = [](uint64_t id) {
            return TypefaceMap::GetTypefaceByUniqueId(RSTypefaceCache::GetTypefaceId(id));
        };
        Drawing::Typeface::RegisterCallBackFunc(registerCB);
        Drawing::Typeface::RegisterOnTypefaceDestroyed(destroyedCB);
        Drawing::Typeface::RegisterUniqueIdCallBack(uniqueIdCB);
    }

    ~TypefaceAutoRegister()
    {
        Drawing::Typeface::RegisterCallBackFunc(nullptr);
        Drawing::Typeface::RegisterOnTypefaceDestroyed(nullptr);
        Drawing::Typeface::RegisterUniqueIdCallBack(nullptr);
    }
};

static TypefaceAutoRegister g_typefaceAutoRegister;
} // namespace OHOS::Rosen