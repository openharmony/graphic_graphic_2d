/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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
#include "color_space_object_convertor.h"

#include "js_color_space_utils.h"

namespace OHOS {
namespace ColorManager {
using namespace AbilityRuntime;

NativeValue* CreateJsColorSpaceObject(NativeEngine& engine, std::shared_ptr<ColorSpace>& colorSpace)
{
    NativeValue* objValue = engine.CreateObject();
    NativeObject* object = ConvertNativeValueTo<NativeObject>(objValue);

    std::unique_ptr<JsColorSpace> jsColorSpace = std::make_unique<JsColorSpace>(colorSpace);
    object->SetNativePointer(jsColorSpace.release(), JsColorSpace::Finalizer, nullptr);

    BindFunctions(engine, object);

    std::shared_ptr<NativeReference> jsColorSpaceRef;
    jsColorSpaceRef.reset(engine.CreateReference(objValue, 1));
    return objValue;
}

std::shared_ptr<ColorSpace> GetColorSpaceByJSObject(NativeObject* object)
{
    {
    if (object == nullptr) {
        CMLOGE("[NAPI]GetColorSpaceByJSObject::jsObject is nullptr");
        return nullptr;
    }
    auto jsColorSpace = reinterpret_cast<JsColorSpace*>(object->GetNativePointer());
    if (jsColorSpace == nullptr) {
        CMLOGE("[NAPI]GetColorSpaceByJSObject::jsColorSpace is nullptr");
        return nullptr;
    }
    return jsColorSpace->GetColorSpaceToken();
}
}
}  // namespace ColorManager
}  // namespace OHOS
