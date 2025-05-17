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

#include "js_drawing_init.h"

#include "brush_napi/js_brush.h"
#include "canvas_napi/js_canvas.h"
#include "color_filter_napi/js_color_filter.h"
#include "enum_napi/js_enum.h"
#include "font_napi/js_font.h"
#include "font_napi/js_typeface.h"
#include "font_napi/js_typefacearguments.h"
#include "image_filter_napi/js_image_filter.h"
#include "lattice_napi/js_lattice.h"
#include "mask_filter_napi/js_mask_filter.h"
#include "matrix_napi/js_matrix.h"
#include "path_effect_napi/js_path_effect.h"
#include "path_iterator_napi/js_path_iterator.h"
#include "path_napi/js_path.h"
#include "pen_napi/js_pen.h"
#include "region_napi/js_region.h"
#include "roundRect_napi/js_roundrect.h"
#include "sampling_options_napi/js_sampling_options.h"
#include "shader_effect_napi/js_shader_effect.h"
#include "shadow_layer_napi/js_shadow_layer.h"
#include "text_blob_napi/js_text_blob.h"
#include "tool_napi/js_tool.h"

namespace OHOS::Rosen {
namespace Drawing {
napi_value DrawingInit(napi_env env, napi_value exportObj)
{
    JsFont::Init(env, exportObj);
    JsPath::Init(env, exportObj);
    JsTypeface::Init(env, exportObj);
    JsTypeFaceArguments::Init(env, exportObj);
    JsCanvas::Init(env, exportObj);
    JsEnum::Init(env, exportObj);
    JsColorFilter::Init(env, exportObj);
    JsTextBlob::Init(env, exportObj);
    JsPen::Init(env, exportObj);
    JsBrush::Init(env, exportObj);
    JsImageFilter::Init(env, exportObj);
    JsSamplingOptions::Init(env, exportObj);
    JsMaskFilter::Init(env, exportObj);
    JsLattice::Init(env, exportObj);
    JsPathEffect::Init(env, exportObj);
    JsPathIterator::Init(env, exportObj);
    JsRegion::Init(env, exportObj);
    JsRoundRect::Init(env, exportObj);
    JsShaderEffect::Init(env, exportObj);
    JsShadowLayer::Init(env, exportObj);
    JsMatrix::Init(env, exportObj);
    JsTool::Init(env, exportObj);
    return exportObj;
}
} // namespace Drawing
} // namespace OHOS::Rosen
