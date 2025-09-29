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
        Drawing::Typeface::RegisterCallBackFunc([](std::shared_ptr<Drawing::Typeface> tf) {
            return Rosen::RSInterfaces::GetInstance().RegisterTypeface(tf);
        });

        Drawing::Typeface::RegisterOnTypefaceDestroyed(
            [](uint32_t id) { Rosen::RSInterfaces::GetInstance().UnRegisterTypeface(id); });

        Drawing::Typeface::RegisterUniqueIdCallBack(
            [](uint64_t id) { return TypefaceMap::GetTypeface(RSTypefaceCache::GetTypefaceId(id)); });
    }

    ~TypefaceAutoRegister()
    {
        Drawing::Typeface::RegisterCallBackFunc(nullptr);
        Drawing::Typeface::RegisterOnTypefaceDestroyed(nullptr);
        Drawing::Typeface::RegisterUniqueIdCallBack(nullptr);
    }
};

#ifndef ARKUI_X_ENABLE
// Prohibiting resigter the callback function in advance when arkui-x use custom's font
TypefaceAutoRegister g_typefaceAutoRegister;
#endif
} // namespace OHOS::Rosen