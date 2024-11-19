/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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

#ifndef COLOR_PICKER_NAPI_H
#define COLOR_PICKER_NAPI_H

#include "effect_type.h"
#include "napi/native_api.h"
#include "napi/native_node_api.h"

namespace OHOS {

namespace Media {
class PixelMap;
}
namespace Rosen {
class ColorPicker;
enum class ImageType {
    TYPE_UNKOWN = 100,
    TYPE_PIXEL_MAP,
    TYPE_IMAGE_SOURCE,
};

struct ColorPickerAsyncContext;
class ColorPickerNapi {
public:
    ColorPickerNapi();
    ~ColorPickerNapi();
    static napi_value Init(napi_env env, napi_value exports);

private:
    // methods constructor
    static napi_value Constructor(napi_env env, napi_callback_info info);
    static void Destructor(napi_env env, void* nativeObject, void* finalize);

    // static mothod
    static std::unique_ptr<ColorPickerAsyncContext> InitializeAsyncContext(
        napi_env env, napi_status& status, napi_value* argValue, size_t argCount);
    static bool ProcessCallbackAndCoordinates(napi_env env, napi_value* argValue, size_t argCount,
        napi_value& result, std::unique_ptr<ColorPickerAsyncContext>& asyncContext);
    static napi_value CreateColorPicker(napi_env env, napi_callback_info info);
    static void CreateColorPickerFromPixelMapComplete(napi_env env, napi_status status, void* data);
    static napi_value GetScaledPixelMap(napi_env env, napi_callback_info info);
    static napi_value GetMainColor(napi_env env, napi_callback_info info);
    static napi_value GetMainColorSync(napi_env env, napi_callback_info info);
    static napi_value GetLargestProportionColor(napi_env env, napi_callback_info info);
    static napi_value GetHighestSaturationColor(napi_env env, napi_callback_info info);
    static napi_value GetAverageColor(napi_env env, napi_callback_info info);
    static napi_value IsBlackOrWhiteOrGrayColor(napi_env env, napi_callback_info info);
    static napi_value GetMorandiBackgroundColor(napi_env env, napi_callback_info info);
    static napi_value GetMorandiShadowColor(napi_env env, napi_callback_info info);
    static napi_value GetDeepenImmersionColor(napi_env env, napi_callback_info info);
    static napi_value GetImmersiveBackgroundColor(napi_env env, napi_callback_info info);
    static napi_value GetImmersiveForegroundColor(napi_env env, napi_callback_info info);
    static napi_value DiscriminatePitureLightDegree(napi_env env, napi_callback_info info);
    static napi_value GetReverseColor(napi_env env, napi_callback_info info);
    static napi_value GetTopProportionColors(napi_env env, napi_callback_info info);
    static ImageType ParserArgumentType(napi_env env, napi_value argv);

    // napi var
    napi_env env_ = nullptr;
    napi_ref wrapper_ = nullptr;

    // var for create ColorPicker
    static thread_local napi_ref sConstructor_;
    static thread_local std::shared_ptr<ColorPicker> sColorPicker_;

    // native var
    std::shared_ptr<ColorPicker> nativeColorPicker_;
};
} // namespace Rosen
} // namespace OHOS
#endif /* COLOR_PICKER_NAPI_H */
