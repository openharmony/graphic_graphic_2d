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
#include "path_napi/js_path.h"
#include "pen_napi/js_pen.h"
#include "text_blob_napi/js_text_blob.h"

namespace OHOS::Rosen {
namespace Drawing {
napi_value DrawingInit(napi_env env, napi_value exportObj)
{
    JsFont::Init(env, exportObj);
    JsPath::Init(env, exportObj);
    JsTypeface::Init(env, exportObj);
    JsCanvas::Init(env, exportObj);
    JsEnum::Init(env, exportObj);
    JsColorFilter::Init(env, exportObj);
    JsTextBlob::Init(env, exportObj);
    JsPen::Init(env, exportObj);
    JsBrush::Init(env, exportObj);
    return exportObj;
}
} // namespace Drawing
} // namespace OHOS::Rosen
