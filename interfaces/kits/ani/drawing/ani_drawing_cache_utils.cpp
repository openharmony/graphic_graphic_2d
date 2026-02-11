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

#include "ani_drawing_cache_utils.h"
#include "ani_drawing_log.h"
#include "ani_drawing_common.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {

namespace {
constexpr DescriptorConfig INT_CTOR{ "<ctor>", "i:" };
constexpr DescriptorConfig DOUBLE_CTOR{ "<ctor>", "d:" };
constexpr DescriptorConfig CANVAS_CTOR{ "<ctor>", "C{@ohos.multimedia.image.image.PixelMap}:"};
constexpr DescriptorConfig COLOR_FILTER_CTOR{ "<ctor>", ":" };
constexpr DescriptorConfig IMAGE_FILTER_CTOR{ "<ctor>", ":"};
constexpr DescriptorConfig LATTICE_CTOR{ "<ctor>", ":"};
constexpr DescriptorConfig MASK_FILTER_CTOR{ "<ctor>", ":"};
constexpr DescriptorConfig PATH_EFFECT_CTOR{ "<ctor>", ":"};
constexpr DescriptorConfig SHADER_EFFECT_CTOR{ "<ctor>", ":"};
constexpr DescriptorConfig SHADOW_LAYER_CTOR{ "<ctor>", ":"};
constexpr DescriptorConfig TEXT_BLOB_CTOR{ "<ctor>", ":"};
constexpr DescriptorConfig MATRIX_CTOR{ "<ctor>", ":" };
constexpr DescriptorConfig PATH_CTOR{ "<ctor>", ":" };
constexpr DescriptorConfig PATH_ITERATOR_CTOR{ "<ctor>", "C{@ohos.graphics.drawing.drawing.Path}:" };
constexpr DescriptorConfig ROUND_RECT_CTOR{ "<ctor>", ":"};
constexpr DescriptorConfig TYPEFACE_CTOR{ "<ctor>", ":"};
constexpr DescriptorConfig FONT_METRICS_CTOR{ "<ctor>", "iddddddddddddddd:" };
constexpr DescriptorConfig POINT_CTOR{ "<ctor>", "dd:" };
constexpr DescriptorConfig RECT_CTOR{ "<ctor>", "dddd:" };
constexpr DescriptorConfig COLOR_CTOR{ "<ctor>", "iiii:" };
constexpr DescriptorConfig COLOR4F_CTOR{ "<ctor>", "dddd:" };
constexpr DescriptorConfig BIND_NATIVE_PTR{ "bindNativePtr", "l:" };
constexpr DescriptorConfig INT_GET{ "toInt", ":i"};
constexpr DescriptorConfig DOUBLE_GET{ "toDouble", ":d"};
constexpr DescriptorConfig BOOLEAN_GET{ "toBoolean", ":z"};
constexpr DescriptorConfig RESOURCE_GET_ID{ "<get>id", ":l"};
constexpr DescriptorConfig RESOURCE_GET_PARAMS{ "<get>params", ":C{std.core.Array}"};
constexpr DescriptorConfig RESOURCE_GET_TYPE{ "<get>type", ":C{std.core.Int}"};
constexpr DescriptorConfig COLOR_GET_ALPHA{ "<get>alpha", ":i" };
constexpr DescriptorConfig COLOR_GET_RED{ "<get>red", ":i" };
constexpr DescriptorConfig COLOR_GET_GREEN{ "<get>green", ":i" };
constexpr DescriptorConfig COLOR_GET_BLUE{ "<get>blue", ":i" };
constexpr DescriptorConfig COLOR4F_GET_ALPHA{ "<get>alpha", ":d" };
constexpr DescriptorConfig COLOR4F_GET_RED{ "<get>red", ":d" };
constexpr DescriptorConfig COLOR4F_GET_GREEN{ "<get>green", ":d" };
constexpr DescriptorConfig COLOR4F_GET_BLUE{ "<get>blue", ":d" };
constexpr DescriptorConfig FONTFEATURE_GET_NAME{ "<get>name", ":C{std.core.String}"};
constexpr DescriptorConfig FONTFEATURE_GET_VALUE{"<get>value", ":d"};
constexpr DescriptorConfig RECT_GET_LEFT{ "<get>left", ":d" };
constexpr DescriptorConfig RECT_GET_TOP{ "<get>top", ":d" };
constexpr DescriptorConfig RECT_GET_RIGHT{ "<get>right", ":d" };
constexpr DescriptorConfig RECT_GET_BOTTOM{ "<get>bottom", ":d" };
constexpr DescriptorConfig RECT_SET_LEFT{ "<set>left", "d:" };
constexpr DescriptorConfig RECT_SET_TOP{ "<set>top", "d:" };
constexpr DescriptorConfig RECT_SET_RIGHT{ "<set>right", "d:" };
constexpr DescriptorConfig RECT_SET_BOTTOM{ "<set>bottom", "d:" };
constexpr DescriptorConfig POINT_GET_X{ "<get>x", ":d" };
constexpr DescriptorConfig POINT_GET_Y{ "<get>y", ":d" };
constexpr DescriptorConfig POINT_SET_X{ "<set>x", "d:" };
constexpr DescriptorConfig POINT_SET_Y{ "<set>y", "d:" };
constexpr DescriptorConfig POINT3D_GET_X{ "<get>x", ":d"};
constexpr DescriptorConfig POINT3D_GET_Y{ "<get>y", ":d"};
constexpr DescriptorConfig POINT3D_GET_Z{ "<get>z", ":d"};
constexpr DescriptorConfig RUN_BUFFER_GET_GLYPH{ "<get>glyph", ":i" };
constexpr DescriptorConfig RUN_BUFFER_GET_POSITION_X{ "<get>positionX", ":d" };
constexpr DescriptorConfig RUN_BUFFER_GET_POSITION_Y{ "<get>positionY", ":d" };
}
ani_class AniFindClass(ani_env* env, const char* descriptor)
{
    ani_class cls = nullptr;
    ani_status status = env->FindClass(descriptor, &cls);
    if (status != ANI_OK) {
        ROSEN_LOGE("Failed to find class: %{public}s, status %{public}d", descriptor, status);
        return nullptr;
    }
    ani_ref ref = nullptr;
    status = env->GlobalReference_Create(cls, &ref);
    if (status != ANI_OK) {
        ROSEN_LOGE("Failed to create global reference for class: %{public}s, status %{public}d",
            descriptor, status);
        return nullptr;
    }
    return static_cast<ani_class>(ref);
}

ani_enum AniFindEnum(ani_env* env, const char* descriptor)
{
    ani_enum enumType = nullptr;
    ani_status status = env->FindEnum(descriptor, &enumType);
    if (status != ANI_OK) {
        ROSEN_LOGE("Failed to find enum: %{public}s, status %{public}d", descriptor, status);
        return nullptr;
    }
    ani_ref ref = nullptr;
    status = env->GlobalReference_Create(enumType, &ref);
    if (status != ANI_OK) {
        ROSEN_LOGE("Failed to create global reference for enum: %{public}s, status %{public}d", descriptor, status);
        return nullptr;
    }
    return static_cast<ani_enum>(ref);
}

ani_method AniFindMethod(ani_env* env, ani_class cls, const DescriptorConfig& config)
{
    if (cls == nullptr) {
        ROSEN_LOGE("Failed to find method: cls is null");
        return nullptr;
    }
    ani_method method = nullptr;
    ani_status status = env->Class_FindMethod(cls, config.methodDes.data(), config.paramDes.data(), &method);
    if (status != ANI_OK) {
        ROSEN_LOGE("Failed to find method: %{public}s %{public}s, status %{public}d",
            config.methodDes.data(), config.paramDes.data(), status);
        return nullptr;
    }
    return method;
}

ani_field AniFindField(ani_env* env, ani_class cls, const char* filedName = "nativeObj")
{
    if (cls == nullptr) {
        ROSEN_LOGE("Failed to find field: cls is null");
        return nullptr;
    }
    ani_field field = nullptr;
    ani_status status = env->Class_FindField(cls, filedName, &field);
    if (status != ANI_OK) {
        ROSEN_LOGE("Failed to find field: find nativeObj failed");
        return nullptr;
    }
    return field;
}

void AniGlobalClass::InitBaseClass(ani_env* env)
{
    intCls = AniFindClass(env, ANI_INT_STRING);
    doubleCls = AniFindClass(env, ANI_DOUBLE_STRING);
    booleanCls = AniFindClass(env, ANI_BOOLEAN_STRING);
    strCls = AniFindClass(env, ANI_STRING_STRING);
    arrayCls = AniFindClass(env, ANI_ARRAY_STRING);
    businessErrorCls = AniFindClass(env, ANI_BUSINESS_ERROR_STRING);
    resourceCls = AniFindClass(env, ANI_GLOBAL_RESOURCE_STRING);
}

void AniGlobalClass::InitDrawingClass(ani_env* env)
{
    brush = AniFindClass(env, ANI_CLASS_BRUSH_NAME);
    colorSpaceManager = AniFindClass(env, ANI_CLASS_COLOR_SPACE_MANAGER_NAME);
    samplingOptions = AniFindClass(env, ANI_CLASS_SAMPLING_OPTIONS_NAME);
    canvas = AniFindClass(env, ANI_CLASS_CANVAS_NAME);
    colorFilter = AniFindClass(env, ANI_CLASS_COLORFILTER_NAME);
    pen = AniFindClass(env, ANI_CLASS_PEN_NAME);
    typeface = AniFindClass(env, ANI_CLASS_TYPEFACE_NAME);
    font = AniFindClass(env, ANI_CLASS_FONT_NAME);
    lattice = AniFindClass(env, ANI_CLASS_ANI_LATTICE_NAME);
    matrix = AniFindClass(env, ANI_CLASS_MATRIX_NAME);
    maskFilter = AniFindClass(env, ANI_CLASS_MASKFILTER_NAME);
    pathEffect = AniFindClass(env, ANI_CLASS_PATHEFFECT_NAME);
    imageFilter = AniFindClass(env, ANI_CLASS_IMAGEFILTER_NAME);
    pathIterator = AniFindClass(env, ANI_CLASS_PATH_ITERATOR_NAME);
    path = AniFindClass(env, ANI_CLASS_PATH_NAME);
    shaderEffect = AniFindClass(env, ANI_CLASS_SHADER_EFFECT_NAME);
    shadowLayer = AniFindClass(env, ANI_CLASS_SHADOW_LAYER_NAME);
    textBlob = AniFindClass(env, ANI_CLASS_TEXT_BLOB_NAME);
    rectUtils = AniFindClass(env, ANI_CLASS_RECT_UTILS_NAME);
    region = AniFindClass(env, ANI_CLASS_REGION_NAME);
    roundRect = AniFindClass(env, ANI_CLASS_ROUND_RECT_NAME);
    tool = AniFindClass(env, ANI_CLASS_TOOL_NAME);
    typefaceArguments = AniFindClass(env, ANI_CLASS_TYPEFACE_ARGUMENTS_NAME);
}

void AniGlobalClass::InitInterfaceClass(ani_env* env)
{
    color = AniFindClass(env, ANI_CLASS_COLOR_NAME);
    color4f = AniFindClass(env, ANI_CLASS_COLOR4F_NAME);
    fontMetrics = AniFindClass(env, ANI_CLASS_FONT_METRICS_NAME);
    point = AniFindClass(env, ANI_CLASS_POINT_NAME);
    rect = AniFindClass(env, ANI_CLASS_RECT_NAME);
    colorInterface = AniFindClass(env, ANI_INTERFACE_COLOR_NAME);
    color4fInterface = AniFindClass(env, ANI_INTERFACE_COLOR4F_NAME);
    pointInterface = AniFindClass(env, ANI_INTERFACE_POINT_NAME);
    rectInterface = AniFindClass(env, ANI_INTERFACE_RECT_NAME);
    point3dInterface = AniFindClass(env, ANI_INTERFACE_POINT3D_NAME);
    runBufferInterface = AniFindClass(env, ANI_INTERFACE_TEXT_BLOB_RUN_BUFFER_NAME);
    fontFeatureInterface = AniFindClass(env, ANI_INTERFACE_FONTFEATURE_NAME);
}

void AniGlobalClass::Init(ani_env* env)
{
    InitBaseClass(env);
    InitDrawingClass(env);
    InitInterfaceClass(env);
}

void AniGlobalEnum::Init(ani_env* env)
{
    pathIteratorVerb = AniFindEnum(env, ANI_ENUM_PATH_ITERATOR_VERB);
    fontHinting = AniFindEnum(env, ANI_FONT_HINTING_NAME);
    fontEdging = AniFindEnum(env, ANI_FONT_EDGING_NAME);
    capStyle = AniFindEnum(env, ANI_CLASS_CAP_STYLE_NAME);
    joinStyle = AniFindEnum(env, ANI_CLASS_JOIN_STYLE_NAME);
    colorEnum = AniFindEnum(env, ANI_COLOR_ENUM_STRING);
    pathFillType = AniFindEnum(env, ANI_ENUM_PATH_FILL_TYPE);
}

void AniGlobalMethod::InitCtorMethod(ani_env* env)
{
    intCtor = AniFindMethod(env, AniGlobalClass::GetInstance().intCls, INT_CTOR);
    doubleCtor = AniFindMethod(env, AniGlobalClass::GetInstance().doubleCls, DOUBLE_CTOR);
    canvasCtor = AniFindMethod(env, AniGlobalClass::GetInstance().canvas, CANVAS_CTOR);
    colorFilterCtor = AniFindMethod(env, AniGlobalClass::GetInstance().colorFilter, COLOR_FILTER_CTOR);
    matrixCtor = AniFindMethod(env, AniGlobalClass::GetInstance().matrix, MATRIX_CTOR);
    pathCtor = AniFindMethod(env, AniGlobalClass::GetInstance().path, PATH_CTOR);
    pathIteratorCtor = AniFindMethod(env, AniGlobalClass::GetInstance().pathIterator, PATH_ITERATOR_CTOR);
    roundRectCtor = AniFindMethod(env, AniGlobalClass::GetInstance().roundRect, ROUND_RECT_CTOR);
    typefaceCtor = AniFindMethod(env, AniGlobalClass::GetInstance().typeface, TYPEFACE_CTOR);
    imageFilterCtor = AniFindMethod(env, AniGlobalClass::GetInstance().imageFilter, IMAGE_FILTER_CTOR);
    latticeCtor = AniFindMethod(env, AniGlobalClass::GetInstance().lattice, LATTICE_CTOR);
    maskFilterCtor = AniFindMethod(env, AniGlobalClass::GetInstance().maskFilter, MASK_FILTER_CTOR);
    pathEffectCtor = AniFindMethod(env, AniGlobalClass::GetInstance().pathEffect, PATH_EFFECT_CTOR);
    shaderEffectCtor = AniFindMethod(env, AniGlobalClass::GetInstance().shaderEffect, SHADER_EFFECT_CTOR);
    shadowLayerCtor = AniFindMethod(env, AniGlobalClass::GetInstance().shadowLayer, SHADOW_LAYER_CTOR);
    textBlobCtor = AniFindMethod(env, AniGlobalClass::GetInstance().textBlob, TEXT_BLOB_CTOR);
    colorCtor = AniFindMethod(env, AniGlobalClass::GetInstance().color, COLOR_CTOR);
    color4fCtor = AniFindMethod(env, AniGlobalClass::GetInstance().color4f, COLOR4F_CTOR);
    fontMetricsCtor = AniFindMethod(env, AniGlobalClass::GetInstance().fontMetrics, FONT_METRICS_CTOR);
    pointCtor = AniFindMethod(env, AniGlobalClass::GetInstance().point, POINT_CTOR);
    rectCtor = AniFindMethod(env, AniGlobalClass::GetInstance().rect, RECT_CTOR);
}

void AniGlobalMethod::InitBindNativeMethod(ani_env* env)
{
    colorFilterBindNative = AniFindMethod(env, AniGlobalClass::GetInstance().colorFilter, BIND_NATIVE_PTR);
    typefaceBindNative = AniFindMethod(env, AniGlobalClass::GetInstance().typeface, BIND_NATIVE_PTR);
    latticeBindNative = AniFindMethod(env, AniGlobalClass::GetInstance().lattice, BIND_NATIVE_PTR);
    maskFilterBindNative = AniFindMethod(env, AniGlobalClass::GetInstance().maskFilter, BIND_NATIVE_PTR);
    pathEffectBindNative = AniFindMethod(env, AniGlobalClass::GetInstance().pathEffect, BIND_NATIVE_PTR);
    imageFilterBindNative = AniFindMethod(env, AniGlobalClass::GetInstance().imageFilter, BIND_NATIVE_PTR);
    shaderEffectBindNative = AniFindMethod(env, AniGlobalClass::GetInstance().shaderEffect, BIND_NATIVE_PTR);
    shadowLayerBindNative = AniFindMethod(env, AniGlobalClass::GetInstance().shadowLayer, BIND_NATIVE_PTR);
    textBlobBindNative = AniFindMethod(env, AniGlobalClass::GetInstance().textBlob, BIND_NATIVE_PTR);
}

void AniGlobalMethod::InitGetSetMethod(ani_env* env)
{
    intGet = AniFindMethod(env, AniGlobalClass::GetInstance().intCls, INT_GET);
    doubleGet = AniFindMethod(env, AniGlobalClass::GetInstance().doubleCls, DOUBLE_GET);
    booleanGet = AniFindMethod(env, AniGlobalClass::GetInstance().booleanCls, BOOLEAN_GET);
    resourceGetId = AniFindMethod(env, AniGlobalClass::GetInstance().resourceCls, RESOURCE_GET_ID);
    resourceGetParams = AniFindMethod(env, AniGlobalClass::GetInstance().resourceCls, RESOURCE_GET_PARAMS);
    resourceGetType = AniFindMethod(env, AniGlobalClass::GetInstance().resourceCls, RESOURCE_GET_TYPE);

    colorGetAlpha = AniFindMethod(env, AniGlobalClass::GetInstance().colorInterface, COLOR_GET_ALPHA);
    colorGetRed = AniFindMethod(env, AniGlobalClass::GetInstance().colorInterface, COLOR_GET_RED);
    colorGetGreen = AniFindMethod(env, AniGlobalClass::GetInstance().colorInterface, COLOR_GET_GREEN);
    colorGetBlue = AniFindMethod(env, AniGlobalClass::GetInstance().colorInterface, COLOR_GET_BLUE);
    color4fGetAlpha = AniFindMethod(env, AniGlobalClass::GetInstance().color4fInterface, COLOR4F_GET_ALPHA);
    color4fGetRed = AniFindMethod(env, AniGlobalClass::GetInstance().color4fInterface, COLOR4F_GET_RED);
    color4fGetGreen = AniFindMethod(env, AniGlobalClass::GetInstance().color4fInterface, COLOR4F_GET_GREEN);
    color4fGetBlue = AniFindMethod(env, AniGlobalClass::GetInstance().color4fInterface, COLOR4F_GET_BLUE);
    fontFeatureGetName = AniFindMethod(env, AniGlobalClass::GetInstance().fontFeatureInterface, FONTFEATURE_GET_NAME);
    fontFeatureGetValue = AniFindMethod(env, AniGlobalClass::GetInstance().fontFeatureInterface, FONTFEATURE_GET_VALUE);
    rectGetLeft = AniFindMethod(env, AniGlobalClass::GetInstance().rectInterface, RECT_GET_LEFT);
    rectGetTop = AniFindMethod(env, AniGlobalClass::GetInstance().rectInterface, RECT_GET_TOP);
    rectGetRight = AniFindMethod(env, AniGlobalClass::GetInstance().rectInterface, RECT_GET_RIGHT);
    rectGetBottom = AniFindMethod(env, AniGlobalClass::GetInstance().rectInterface, RECT_GET_BOTTOM);
    rectSetLeft = AniFindMethod(env, AniGlobalClass::GetInstance().rectInterface, RECT_SET_LEFT);
    rectSetTop = AniFindMethod(env, AniGlobalClass::GetInstance().rectInterface, RECT_SET_TOP);
    rectSetRight = AniFindMethod(env, AniGlobalClass::GetInstance().rectInterface, RECT_SET_RIGHT);
    rectSetBottom = AniFindMethod(env, AniGlobalClass::GetInstance().rectInterface, RECT_SET_BOTTOM);
    pointGetX = AniFindMethod(env, AniGlobalClass::GetInstance().pointInterface, POINT_GET_X);
    pointGetY = AniFindMethod(env, AniGlobalClass::GetInstance().pointInterface, POINT_GET_Y);
    pointSetX = AniFindMethod(env, AniGlobalClass::GetInstance().pointInterface, POINT_SET_X);
    pointSetY = AniFindMethod(env, AniGlobalClass::GetInstance().pointInterface, POINT_SET_Y);
    point3dGetX = AniFindMethod(env, AniGlobalClass::GetInstance().point3dInterface, POINT3D_GET_X);
    point3dGetY = AniFindMethod(env, AniGlobalClass::GetInstance().point3dInterface, POINT3D_GET_Y);
    point3dGetZ = AniFindMethod(env, AniGlobalClass::GetInstance().point3dInterface, POINT3D_GET_Z);
    runBufferGetGlyph = AniFindMethod(env,  AniGlobalClass::GetInstance().runBufferInterface, RUN_BUFFER_GET_GLYPH);
    runBufferGetPositionX = AniFindMethod(
        env, AniGlobalClass::GetInstance().runBufferInterface, RUN_BUFFER_GET_POSITION_X);
    runBufferGetPositionY = AniFindMethod(
        env, AniGlobalClass::GetInstance().runBufferInterface, RUN_BUFFER_GET_POSITION_Y);
}

void AniGlobalMethod::Init(ani_env* env)
{
    InitCtorMethod(env);
    InitBindNativeMethod(env);
    InitGetSetMethod(env);
}

void AniGlobalField::Init(ani_env* env)
{
    brushNativeObj = AniFindField(env, AniGlobalClass::GetInstance().brush);
    colorSpaceManagerNativeobj = AniFindField(env, AniGlobalClass::GetInstance().colorSpaceManager, "nativePtr");
    samplingOptionsNativeObj = AniFindField(env, AniGlobalClass::GetInstance().samplingOptions);
    canvasNativeObj = AniFindField(env, AniGlobalClass::GetInstance().canvas);
    colorFilterNativeObj = AniFindField(env, AniGlobalClass::GetInstance().colorFilter);
    penNativeObj = AniFindField(env, AniGlobalClass::GetInstance().pen);
    typefaceNativeObj = AniFindField(env, AniGlobalClass::GetInstance().typeface);
    fontNativeObj = AniFindField(env, AniGlobalClass::GetInstance().font);
    latticeNativeObj = AniFindField(env, AniGlobalClass::GetInstance().lattice);
    matrixNativeObj = AniFindField(env, AniGlobalClass::GetInstance().matrix);
    maskFilterNativeObj = AniFindField(env, AniGlobalClass::GetInstance().maskFilter);
    pathEffectNativeObj = AniFindField(env, AniGlobalClass::GetInstance().pathEffect);
    imageFilterNativeObj = AniFindField(env, AniGlobalClass::GetInstance().imageFilter);
    pathIteratorNativeObj = AniFindField(env, AniGlobalClass::GetInstance().pathIterator);
    pathNativeObj = AniFindField(env, AniGlobalClass::GetInstance().path);
    shaderEffectNativeObj = AniFindField(env, AniGlobalClass::GetInstance().shaderEffect);
    shadowLayerNativeObj = AniFindField(env, AniGlobalClass::GetInstance().shadowLayer);
    textBlobNativeObj = AniFindField(env, AniGlobalClass::GetInstance().textBlob);
    rectUtilsNativeObj = AniFindField(env, AniGlobalClass::GetInstance().rectUtils);
    regionNativeObj = AniFindField(env, AniGlobalClass::GetInstance().region);
    roundRectNativeObj = AniFindField(env, AniGlobalClass::GetInstance().roundRect);
    toolNativeObj = AniFindField(env, AniGlobalClass::GetInstance().tool);
    typefaceArgumentsNativeObj = AniFindField(env, AniGlobalClass::GetInstance().typefaceArguments);
}

ani_status InitAniGlobalRef(ani_env* env)
{
    AniGlobalClass::GetInstance().Init(env);
    AniGlobalEnum::GetInstance().Init(env);
    AniGlobalMethod::GetInstance().Init(env);
    AniGlobalField::GetInstance().Init(env);
    return ANI_OK;
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS