/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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

#ifndef OHOS_COLOR_SPACE_OBJECT_CONVERTOR_H
#define OHOS_COLOR_SPACE_OBJECT_CONVERTOR_H

#include <memory>

#include "color_space.h"
#include "napi/native_api.h"
#include "napi/native_node_api.h"

namespace OHOS {
namespace ColorManager {
napi_value CreateJsColorSpaceObject(napi_env env, std::shared_ptr<ColorSpace>& colorSpace);
napi_value CreateJsSendableColorSpaceObject(napi_env env, std::shared_ptr<ColorSpace>& colorSpace);
std::shared_ptr<ColorSpace> GetColorSpaceByJSObject(napi_env env, napi_value object);
}  // namespace ColorManager
}  // namespace OHOS

#endif // OHOS_COLOR_SPACE_OBJECT_CONVERTOR_H
