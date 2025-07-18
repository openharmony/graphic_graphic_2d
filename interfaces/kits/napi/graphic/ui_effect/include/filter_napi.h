/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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
#ifndef OHOS_UIEFFECT_FILTER_NAPI_H
#define OHOS_UIEFFECT_FILTER_NAPI_H

#include <hilog/log.h>
#include <iostream>

#include "filter/include/filter.h"
#include "filter/include/filter_bezier_warp_para.h"
#include "filter/include/filter_blur_para.h"
#include "filter/include/filter_color_gradient_para.h"
#include "filter/include/filter_content_light_para.h"
#include "filter/include/filter_direction_light_para.h"
#include "filter/include/filter_dispersion_para.h"
#include "filter/include/filter_displacement_distort_para.h"
#include "filter/include/filter_distort_para.h"
#include "filter/include/filter_edge_light_para.h"
#include "filter/include/filter_fly_out_para.h"
#include "filter/include/filter_hdr_para.h"
#include "filter/include/filter_mask_transition_para.h"
#include "filter/include/filter_pixel_stretch_para.h"
#include "filter/include/filter_radius_gradient_blur_para.h"
#include "filter/include/filter_variable_radius_blur_para.h"
#include "filter/include/filter_water_ripple_para.h"
#include "mask/include/mask.h"
#include "napi/native_api.h"
#include "napi/native_node_api.h"

namespace OHOS {
namespace Rosen {

#undef LOG_DOMAIN
#define LOG_DOMAIN 0xD001405

#undef LOG_TAG
#define LOG_TAG "UiEffect_Filter"
#define FILTER_LOG_D(fmt, ...) HILOG_DEBUG(LOG_CORE, fmt, ##__VA_ARGS__)
#define FILTER_LOG_I(fmt, ...) HILOG_INFO(LOG_CORE, fmt, ##__VA_ARGS__)
#define FILTER_LOG_E(fmt, ...) HILOG_ERROR(LOG_CORE, fmt, ##__VA_ARGS__)

class FilterNapi {
public:
    FilterNapi();
    ~FilterNapi();
    static napi_value Init(napi_env env, napi_value exports);

private:
    static thread_local napi_ref sConstructor_;
    static napi_value Constructor(napi_env env, napi_callback_info info);
    static void Destructor(napi_env env, void* nativeObject, void* finalize);

    static napi_value CreateFilter(napi_env env, napi_callback_info info);
    static napi_value SetBezierWarp(napi_env env, napi_callback_info info);
    static napi_value SetBlur(napi_env env, napi_callback_info info);
    static napi_value SetPixelStretch(napi_env env, napi_callback_info info);
    static napi_value SetWaterRipple(napi_env env, napi_callback_info info);
    static napi_value SetFlyOut(napi_env env, napi_callback_info info);
    static napi_value SetRadiusGradientBlurPara(napi_env env, napi_callback_info info);
    static napi_value SetDistort(napi_env env, napi_callback_info info);
    static napi_value SetColorGradient(napi_env env, napi_callback_info info);
    static napi_value SetDisplacementDistort(napi_env env, napi_callback_info info);
    static napi_value SetEdgeLight(napi_env env, napi_callback_info info);
    static napi_value SetMaskDirectionLight(napi_env env, napi_callback_info info);
    static napi_value SetMaskDispersion(napi_env env, napi_callback_info info);
    static Drawing::TileMode ParserArgumentType(napi_env env, napi_value argv);
    static GradientDirection ParserGradientDirection(napi_env env, napi_value argv);
    static napi_value SetHDRBrightnessRatio(napi_env env, napi_callback_info info);
    static napi_value SetContentLight(napi_env env, napi_callback_info info);
    static napi_value SetMaskTransition(napi_env env, napi_callback_info info);
    static napi_value SetVariableRadiusBlur(napi_env env, napi_callback_info info);

    static void RegisterFilterParaUnmarshallingCallback();

    static float GetSpecialValue(napi_env env, napi_value argValue);
    static uint32_t GetSpecialIntValue(napi_env env, napi_value argValue);
    static bool GetSpecialBoolValue(napi_env env, napi_value argValue, bool defaultValue);
    static bool GetColorGradientArray(napi_env env, napi_value* argValue, std::shared_ptr<ColorGradientPara>& para,
        uint32_t arraySize);

    std::shared_ptr<Filter> m_FilterObj = nullptr;
};
} // namespace Rosen
} // namespace OHOS
#endif // OHOS_UIEFFECT_FILTER_NAPI_H
