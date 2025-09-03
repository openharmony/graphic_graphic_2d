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

#ifndef OHOS_TEXT_ANI_FONT_COLLECTION_H
#define OHOS_TEXT_ANI_FONT_COLLECTION_H

#include <ani.h>
#include <memory>

#include "font_collection.h"
#include "resource_manager.h"

namespace OHOS::Text::ANI {
class AniFontCollection final {
public:
    AniFontCollection();
    AniFontCollection(std::shared_ptr<OHOS::Rosen::FontCollection> fc);
    static ani_object GetGlobalInstance(ani_env* env, ani_class cls);
    static void LoadFontSync(ani_env* env, ani_object obj, ani_string name, ani_object path);
    static void ClearCaches(ani_env* env, ani_object obj);
    static void Constructor(ani_env* env, ani_object object);
    static ani_status AniInit(ani_vm* vm, uint32_t* result);
    std::shared_ptr<OHOS::Rosen::FontCollection> GetFontCollection();
    static ani_object NativeTransferStatic(ani_env* env, ani_class cls, ani_object input);
    static ani_object NativeTransferDynamic(ani_env* env, ani_class cls, ani_long nativeObj);

private:
    std::shared_ptr<OHOS::Rosen::FontCollection> fontCollection_{nullptr};
};
} // namespace OHOS::Text::ANI
#endif // OHOS_TEXT_ANI_FONT_COLLECTION_H