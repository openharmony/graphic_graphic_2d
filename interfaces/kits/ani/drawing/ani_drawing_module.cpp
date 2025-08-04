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
#include "matrix_ani/ani_matrix.h"
#include "path_ani/ani_path.h"
#include "path_iterator_ani/ani_path_iterator.h"
#include "pen_ani/ani_pen.h"
#include "region_ani/ani_region.h"
#include "round_rect_ani/ani_round_rect.h"
#include "sampling_options_ani/ani_sampling_options.h"
#include "typeface_ani/ani_typeface.h"
#include "typeface_arguments_ani/ani_typeface_arguments.h"

const char* ANI_CLASS_CLEANER_NAME = "L@ohos/graphics/drawing/drawing/Cleaner;";

template <typename T>
void SafeDelete(ani_long& ptr)
{
    if (ptr == 0) {
        ROSEN_LOGE("SafeDelete ptr is 0");
        return;
    }
    T* pointer = reinterpret_cast<T*>(ptr);
    delete pointer;
    pointer = nullptr;
    ptr = 0;
}

static void Clean(ani_env* env, ani_object object)
{
    ani_long ptr;
    ani_status ret = env->Object_GetFieldByName_Long(object, "ptr", &ptr);
    if (ret != ANI_OK) {
        ROSEN_LOGE("Clean can't find ptr");
        return;
    }
    ani_ref stringRef = nullptr;
    ret = env->Object_GetFieldByName_Ref(object, "className", &stringRef);
    if (ret != ANI_OK) {
        ROSEN_LOGE("Clean can't find className");
        return;
    }

    std::string className = OHOS::Rosen::Drawing::CreateStdString(env, reinterpret_cast<ani_string>(stringRef));
    using DeleteFunc = void (*)(ani_long&);
    static const std::unordered_map<std::string, DeleteFunc> deleteMap = {
        {"Brush", SafeDelete<OHOS::Rosen::Drawing::AniBrush>},
        {"Canvas", SafeDelete<OHOS::Rosen::Drawing::AniCanvas>},
        {"ColorFilter", SafeDelete<OHOS::Rosen::Drawing::AniColorFilter>},
        {"Font", SafeDelete<OHOS::Rosen::Drawing::AniFont>},
        {"Lattice", SafeDelete<OHOS::Rosen::Drawing::AniLattice>},
        {"Matrix", SafeDelete<OHOS::Rosen::Drawing::AniMatrix>},
        {"Path", SafeDelete<OHOS::Rosen::Drawing::AniPath>},
        {"PathIterator", SafeDelete<OHOS::Rosen::Drawing::AniPathIterator>},
        {"Pen", SafeDelete<OHOS::Rosen::Drawing::AniPen>},
        {"Region", SafeDelete<OHOS::Rosen::Drawing::AniRegion>},
        {"RoundRect", SafeDelete<OHOS::Rosen::Drawing::AniRoundRect>},
        {"SamplingOptions", SafeDelete<OHOS::Rosen::Drawing::AniSamplingOptions>},
        {"Typeface", SafeDelete<OHOS::Rosen::Drawing::AniTypeface>},
        {"TypefaceArguments", SafeDelete<OHOS::Rosen::Drawing::AniTypefaceArguments>}};

    auto it = deleteMap.find(className);
    if (it != deleteMap.end()) {
        it->second(ptr);
    } else {
        ROSEN_LOGE("Clean can't find className: %{public}s in deleteMap", className.c_str());
    }
}

static ani_status AniCleanerInit(ani_env* env)
{
    ani_class cls = nullptr;
    ani_status ret = env->FindClass(ANI_CLASS_CLEANER_NAME, &cls);
    if (ret != ANI_OK) {
        ROSEN_LOGE("[ANI] can't find class: %{public}s", ANI_CLASS_CLEANER_NAME);
        return ANI_NOT_FOUND;
    }

    std::array methods = {
        ani_native_function{"clean", ":V", reinterpret_cast<void*>(Clean)},
    };

    ret = env->Class_BindNativeMethods(cls, methods.data(), methods.size());
    if (ret != ANI_OK) {
        ROSEN_LOGE("[ANI] bind methods fail: %{public}s", ANI_CLASS_CLEANER_NAME);
        return ANI_NOT_FOUND;
    }

    return ANI_OK;
}

extern "C" {
ANI_EXPORT ani_status ANI_Constructor(ani_vm* vm, uint32_t* result)
{
    ani_env* env;
    if (vm->GetEnv(ANI_VERSION_1, &env) != ANI_OK) {
        ROSEN_LOGE("ANI_Constructor Unsupported ANI_VERSION_1");
        return ANI_ERROR;
    }

    if (AniCleanerInit(env) != ANI_OK ||
        OHOS::Rosen::Drawing::AniBrush::AniInit(env) != ANI_OK ||
        OHOS::Rosen::Drawing::AniCanvas::AniInit(env) != ANI_OK ||
        OHOS::Rosen::Drawing::AniColorFilter::AniInit(env) != ANI_OK ||
        OHOS::Rosen::Drawing::AniFont::AniInit(env) != ANI_OK ||
        OHOS::Rosen::Drawing::AniPen::AniInit(env) != ANI_OK ||
        OHOS::Rosen::Drawing::AniSamplingOptions::AniInit(env) != ANI_OK ||
        OHOS::Rosen::Drawing::AniTypeface::AniInit(env) != ANI_OK ||
        OHOS::Rosen::Drawing::AniLattice::AniInit(env) != ANI_OK ||
        OHOS::Rosen::Drawing::AniMatrix::AniInit(env) != ANI_OK ||
        OHOS::Rosen::Drawing::AniPath::AniInit(env) != ANI_OK ||
        OHOS::Rosen::Drawing::AniPathIterator::AniInit(env) != ANI_OK ||
        OHOS::Rosen::Drawing::AniRegion::AniInit(env) != ANI_OK ||
        OHOS::Rosen::Drawing::AniRoundRect::AniInit(env) != ANI_OK ||
        OHOS::Rosen::Drawing::AniTypefaceArguments::AniInit(env) != ANI_OK) {
        ROSEN_LOGE("[ANI_Constructor] Init failed");
        return ANI_ERROR;
    }

    *result = ANI_VERSION_1;
    return ANI_OK;
}
}