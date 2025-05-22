/*
 * Copyright (C) 2025 Huawei Device Co., Ltd.
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

#include "ani_drawing_utils.h"
#include "lattice_ani/ani_lattice.h"
#include "brush_ani/ani_brush.h"
#include "canvas_ani/ani_canvas.h"
#include "color_filter_ani/ani_color_filter.h"
#include "font_ani/ani_font.h"
#include "pen_ani/ani_pen.h"
#include "sampling_options_ani/ani_sampling_options.h"
#include "typeface_ani/ani_typeface.h"

extern "C" {
ANI_EXPORT ani_status ANI_Constructor(ani_vm* vm, uint32_t* result)
{
    ani_env* env;
    if (vm->GetEnv(ANI_VERSION_1, &env) != ANI_OK) {
        ROSEN_LOGE("ANI_Constructor Unsupported ANI_VERSION_1");
        return ANI_ERROR;
    }

    if (OHOS::Rosen::Drawing::AniBrush::AniInit(env) != ANI_OK ||
        OHOS::Rosen::Drawing::AniCanvas::AniInit(env) != ANI_OK ||
        OHOS::Rosen::Drawing::AniColorFilter::AniInit(env) != ANI_OK ||
        OHOS::Rosen::Drawing::AniFont::AniInit(env) != ANI_OK ||
        OHOS::Rosen::Drawing::AniPen::AniInit(env) != ANI_OK ||
        OHOS::Rosen::Drawing::AniSamplingOptions::AniInit(env) != ANI_OK ||
        OHOS::Rosen::Drawing::AniTypeface::AniInit(env) != ANI_OK ||
        OHOS::Rosen::Drawing::AniLattice::AniInit(env) != ANI_OK) {
        ROSEN_LOGE("[ANI_Constructor] Init failed");
        return ANI_ERROR;
    }

    *result = ANI_VERSION_1;
    return ANI_OK;
}
}