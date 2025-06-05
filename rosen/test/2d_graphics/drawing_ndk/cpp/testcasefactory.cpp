/*
 * Copyright (c) 2024-2025 Huawei Device Co., Ltd.
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

#include "testcasefactory.h"

#include "dm/aa_rect_modes.h"
#include "dm/add_arc.h"
#include "dm/alpha_gradients.h"
#include "dm/alpha_image.h"
#include "dm/anisotropic.h"
#include "dm/arcofzorro.h"
#include "dm/bitmaprect.h"
#include "dm/blur_circles.h"
#include "dm/blur_large_rrects.h"
#include "dm/bugfix7792.h"
#include "dm/circular_arcs.h"
#include "dm/clip_cubic.h"
#include "dm/clipped_cubic.h"
#include "dm/conic_paths.h"
#include "dm/convex_paths.h"
#include "dm/dashing.h"
#include "dm/drawbitmaprect.h"
#include "dm/fill_types_persp.h"
#include "dm/filltypes.h"
#include "dm/font_regen.h"
#include "dm/gradient_dirty_laundry.h"
#include "dm/largeclippedpath.h"
#include "dm/luma_filter.h"
#include "dm/newsurface.h"
#include "dm/onebadarc.h"
#include "dm/path_interior.h"
#include "dm/points.h"
#include "dm/points_mask_filter.h"
#include "dm/shadowutils.h"
#include "dm/skbug_8955.h"
#include "dm/stroke_rect_shader.h"
#include "dm/strokes.h"
#include "function/brush_null_test.h"
#include "function/canvas_test.h"
#include "function/filter_test.h"
#include "function/font_measure_text_test.h"
#include "function/function_path_test.h"
#include "function/function_pen_test.h"
#include "function/path_test.h"
#include "function/rect_test.h"
#include "function/round_rect_test.h"
#include "function/path_effect_test.h"
#include "interface/bitmap_test.h"
#include "interface/brush_test.h"
#include "interface/canvas_test.h"
#include "interface/color_test.h"
#include "interface/font_test.h"
#include "interface/mask_filter_test.h"
#include "interface/matrix_test.h"
#include "interface/memory_stream_test.h"
#include "interface/path_test.h"
#include "interface/pen_test.h"
#include "interface/sample_option_test.h"
#include "interface/shader_effect_test.h"
#include "interface/surface_test.h"
#include "interface/text_blob_test.h"
#include "interface/typeface_test.h"
#include "performance/canvas_clip_path_test.h"
#include "performance/canvas_draw_image_rect.h"
#include "performance/canvas_draw_image_rect_with_src.h"
#include "performance/canvas_draw_performance.h"
#include "performance/canvas_draw_performance_c_operation_switch.h"
#include "performance/canvas_draw_performance_c_property.h"
#include "performance/canvas_draw_shadow_test.h"
#include "performance/canvas_draw_text_blob_text.h"
#include "performance/performance.h"
#include "reliability/bitmap_test.h"
#include "reliability/pen_exception_test.h"
#include "reliability/reliability_brush_test.h"
#include "stability/bitmap_test.h"
#include "stability/brush_test.h"
#include "stability/canvas_test.h"
#include "stability/color_test.h"
#include "stability/filter_test.h"
#include "stability/mask_filter_test.h"
#include "stability/matrix_test.h"
#include "stability/memory_stream_test.h"
#include "stability/path_effect_test.h"
#include "stability/path_test.h"
#include "stability/pen_test.h"
#include "stability/point_test.h"
#include "stability/rect_test.h"
#include "stability/region_test.h"
#include "stability/round_rect_test.h"
#include "stability/sampling_options_test.h"
#include "stability/shader_effect_test.h"
#include "stability/shadow_layer_test.h"
#include "stability/surface_test.h"
#include "stability/text_blob_test.h"
#include "stability/typeface_test.h"
#include "performance/path_effect_performance.h"
#include "common/log_common.h"

namespace {
std::unordered_map<std::string, std::function<std::shared_ptr<TestBase>()>> FunctionalCpuMap = {
    // Reliability
    { "reliabilityDeleteBitmap",
        []() -> std::shared_ptr<TestBase> { return std::make_shared<ReliabilityDeleteBitmap>(false); } },
    { "reliabilityDeletePixel",
        []() -> std::shared_ptr<TestBase> { return std::make_shared<ReliabilityDeletePixel>(false); } },
    { "reliabilityDeleteBitmapJoinThread",
        []() -> std::shared_ptr<TestBase> { return std::make_shared<ReliabilityDeleteBitmapJoinThread>(false); } },
    { "reliabilityDeletePixelJoinThread",
        []() -> std::shared_ptr<TestBase> { return std::make_shared<ReliabilityDeletePixelJoinThread>(false); } },
    { "reliabilityDetachPen", []() -> std::shared_ptr<TestBase> { return std::make_shared<ReliabilityDetachPen>(); } },
    { "reliabilityDestroyPen",
        []() -> std::shared_ptr<TestBase> { return std::make_shared<ReliabilityDestroyPen>(); } },
    { "reliabilitySetShaderEffect",
        []() -> std::shared_ptr<TestBase> { return std::make_shared<ReliabilitySetShaderEffect>(); } },
    { "reliabilitySetShadowLayer",
        []() -> std::shared_ptr<TestBase> { return std::make_shared<ReliabilitySetShadowLayer>(); } },
    { "reliabilitySetPathEffect",
        []() -> std::shared_ptr<TestBase> { return std::make_shared<ReliabilitySetPathEffect>(); } },
    { "reliabilitySetFilter", []() -> std::shared_ptr<TestBase> { return std::make_shared<ReliabilitySetFilter>(); } },
    { "reliabilityImageFilterDestroy",
        []() -> std::shared_ptr<TestBase> { return std::make_shared<ReliabilityImageFilterDestroy>(); } },
    { "reliabilityMaskFilterDestroy",
        []() -> std::shared_ptr<TestBase> { return std::make_shared<ReliabilityMaskFilterDestroy>(); } },
    { "reliabilityColorFilterDestroy",
        []() -> std::shared_ptr<TestBase> { return std::make_shared<ReliabilityColorFilterDestroy>(); } },
    { "reliabilityThreadDetachPen",
        []() -> std::shared_ptr<TestBase> { return std::make_shared<ReliabilityThreadDetachPen>(); } },
    { "reliabilityThreadDestroyPen",
        []() -> std::shared_ptr<TestBase> { return std::make_shared<ReliabilityThreadDestroyPen>(); } },
    { "reliabilityThreadDestroyShaderEffect",
        []() -> std::shared_ptr<TestBase> { return std::make_shared<ReliabilityThreadDestroyShaderEffect>(); } },
    { "reliabilityThreadDestroyShadowLayer",
        []() -> std::shared_ptr<TestBase> { return std::make_shared<ReliabilityThreadDestroyShadowLayer>(); } },
    { "reliabilityThreadDestroyPathEffect",
        []() -> std::shared_ptr<TestBase> { return std::make_shared<ReliabilityThreadDestroyPathEffect>(); } },
    { "reliabilityThreadDestroyFilter",
        []() -> std::shared_ptr<TestBase> { return std::make_shared<ReliabilityThreadDestroyFilter>(); } },
    { "reliabilityThreadDestroyImageFilter",
        []() -> std::shared_ptr<TestBase> { return std::make_shared<ReliabilityThreadDestroyImageFilter>(); } },
    { "reliabilityThreadDestroyMaskFilter",
        []() -> std::shared_ptr<TestBase> { return std::make_shared<ReliabilityThreadDestroyMaskFilter>(); } },
    { "reliabilityThreadDestroyColorFilter",
        []() -> std::shared_ptr<TestBase> { return std::make_shared<ReliabilityThreadDestroyColorFilter>(); } },
    { "reliabilityDetachBrush",
        []() -> std::shared_ptr<TestBase> { return std::make_shared<ReliabilityDetachBrush>(); } },
    { "reliabilityAttachBrush",
        []() -> std::shared_ptr<TestBase> { return std::make_shared<ReliabilityAttachBrush>(); } },
    { "reliabilityBrushSetShaderEffect",
        []() -> std::shared_ptr<TestBase> { return std::make_shared<ReliabilityBrushSetShaderEffect>(); } },
    { "reliabilityBrushSetShadowLayer",
        []() -> std::shared_ptr<TestBase> { return std::make_shared<ReliabilityBrushSetShadowLayer>(); } },
    { "reliabilityBrushSetFilter",
        []() -> std::shared_ptr<TestBase> { return std::make_shared<ReliabilityBrushSetFilter>(); } },
    { "reliabilityBrushSetImageFilter",
        []() -> std::shared_ptr<TestBase> { return std::make_shared<ReliabilityBrushSetImageFilter>(); } },
    { "reliabilityBrushSetMaskFilter",
        []() -> std::shared_ptr<TestBase> { return std::make_shared<ReliabilityBrushSetMaskFilter>(); } },
    { "reliabilityBrushSetColorFilter",
        []() -> std::shared_ptr<TestBase> { return std::make_shared<ReliabilityBrushSetColorFilter>(); } },
    { "reliabilityDetachBrushJoinThread",
        []() -> std::shared_ptr<TestBase> { return std::make_shared<ReliabilityDetachBrushJoinThread>(); } },
    { "reliabilityAttachBrushJoinThread",
        []() -> std::shared_ptr<TestBase> { return std::make_shared<ReliabilityAttachBrushJoinThread>(); } },
    { "reliabilityBrushSetShaderEffectJoinThread",
        []() -> std::shared_ptr<TestBase> { return std::make_shared<ReliabilityBrushSetShaderEffectJoinThread>(); } },
    { "reliabilityBrushSetShadowLayerJoinThread",
        []() -> std::shared_ptr<TestBase> { return std::make_shared<ReliabilityBrushSetShadowLayerJoinThread>(); } },
    { "reliabilityBrushSetFilterJoinThread",
        []() -> std::shared_ptr<TestBase> { return std::make_shared<ReliabilityBrushSetFilterJoinThread>(); } },
    { "reliabilityBrushSetImageFilterJoinThread",
        []() -> std::shared_ptr<TestBase> { return std::make_shared<ReliabilityBrushSetImageFilterJoinThread>(); } },
    { "reliabilityBrushSetMaskFilterJoinThread",
        []() -> std::shared_ptr<TestBase> { return std::make_shared<ReliabilityBrushSetMaskFilterJoinThread>(); } },
    { "reliabilityBrushSetColorFilterJoinThread",
        []() -> std::shared_ptr<TestBase> { return std::make_shared<ReliabilityBrushSetColorFilterJoinThread>(); } },
    // DM
    { "aarectmodes", []() -> std::shared_ptr<TestBase> { return std::make_shared<AARectModes>(); } },
    { "blurcircles", []() -> std::shared_ptr<TestBase> { return std::make_shared<BlurCircles>(); } },
    { "blur_large_rrects", []() -> std::shared_ptr<TestBase> { return std::make_shared<BlurLargeRrects>(); } },
    { "addarc_meas", []() -> std::shared_ptr<TestBase> { return std::make_shared<AddArcMeas>(); } },
    { "addarc", []() -> std::shared_ptr<TestBase> { return std::make_shared<AddArc>(); } },
    { "badapple",
        []() -> std::shared_ptr<TestBase> { return std::make_shared<BadApple>(); } }, // 字体相关问题，函数缺失
    { "alpha_image_alpha_tint", []() -> std::shared_ptr<TestBase> { return std::make_shared<AlphaImageAlphaTint>(); } },
    { "shadowutils",
        []() -> std::shared_ptr<TestBase> {
            return std::make_shared<ShadowUtils>(ShadowUtils::K_NO_OCCLUDERS);
        } }, // maprect 接口计算不对
    { "shadowutils_occl",
        []() -> std::shared_ptr<TestBase> {
            return std::make_shared<ShadowUtils>(ShadowUtils::K_OCCLUDERS);
        } }, // maprect 接口计算不对
    { "shadowutils_gray",
        []() -> std::shared_ptr<TestBase> {
            return std::make_shared<ShadowUtils>(ShadowUtils::K_GRAY_SCALE);
        } }, // maprect 接口计算不对
    { "circular_arc_stroke_matrix",
        []() -> std::shared_ptr<TestBase> { return std::make_shared<CircularArcStrokeMatrix>(); } },
    { "clippedcubic", []() -> std::shared_ptr<TestBase> { return std::make_shared<ClippedCubic>(); } },
    { "largeclippedpath_winding",
        []() -> std::shared_ptr<TestBase> { return std::make_shared<LargeClippedPath>(PATH_FILL_TYPE_WINDING); } },
    { "largeclippedpath_evenodd",
        []() -> std::shared_ptr<TestBase> { return std::make_shared<LargeClippedPath>(PATH_FILL_TYPE_EVEN_ODD); } },
    { "filltypes", []() -> std::shared_ptr<TestBase> { return std::make_shared<FillType>(); } },
    { "dashing", []() -> std::shared_ptr<TestBase> { return std::make_shared<Dashing>(); } },
    { "pathinterior", []() -> std::shared_ptr<TestBase> { return std::make_shared<PathInterior>(); } },
    { "dashing2", []() -> std::shared_ptr<TestBase> { return std::make_shared<Dashing2>(); } },
    { "alphagradients", []() -> std::shared_ptr<TestBase> { return std::make_shared<AlphaGradients>(); } },
    { "dashing4", []() -> std::shared_ptr<TestBase> { return std::make_shared<Dashing4>(); } },
    { "convexpaths", []() -> std::shared_ptr<TestBase> { return std::make_shared<ConvexPaths>(); } },
    { "gradient_dirty_laundry",
        []() -> std::shared_ptr<TestBase> { return std::make_shared<Gradients>(); } }, // 内部形状和颜色差异较大
    { "arcofzorro", []() -> std::shared_ptr<TestBase> { return std::make_shared<ArcOfZorro>(); } },
    { "stroke_rect_shader", []() -> std::shared_ptr<TestBase> { return std::make_shared<StrokeRectShader>(); } },
    { "lumafilter",
        []() -> std::shared_ptr<TestBase> { return std::make_shared<LumaFilter>(); } }, // 字体粗细、位置有差异
    { "points_maskfilter",
        []() -> std::shared_ptr<TestBase> {
            return std::make_shared<PointsMaskFilter>();
        } }, // drawpoints argb参数存在部分偏差
    { "clipcubic", []() -> std::shared_ptr<TestBase> { return std::make_shared<ClipCubic>(); } },
    { "filltypespersp", []() -> std::shared_ptr<TestBase> { return std::make_shared<FillTypePersp>(); } },
    { "strokes_poly", []() -> std::shared_ptr<TestBase> { return std::make_shared<Strokes2>(); } },
    { "bug7792", []() -> std::shared_ptr<TestBase> { return std::make_shared<BugFix7792>(); } },
    { "points", []() -> std::shared_ptr<TestBase> { return std::make_shared<Points>(); } },
    { "alpha_image",
        []() -> std::shared_ptr<TestBase> { return std::make_shared<AlphaImage>(); } }, // 第二个三角形虚化不对
    { "conicpaths",
        []() -> std::shared_ptr<TestBase> {
            return std::make_shared<ConicPaths>();
        } }, // 有部分线条cpu出图部分缺失,gpu正常出图，颜色为黑色
    { "onebadarc",
        []() -> std::shared_ptr<TestBase> { return std::make_shared<OneBadArc>(); } }, // 完全按照逻辑所画出的图形不一致
    { "skbug_8955",
        []() -> std::shared_ptr<TestBase> {
            return std::make_shared<SkBug8955>();
        } }, // font.textToGlyphs、font.getPos接口缺失
    { "surfacenew", []() -> std::shared_ptr<TestBase> { return std::make_shared<NewSurfaceGM>(); } },
    { "bitmaprect",
        []() -> std::shared_ptr<TestBase> {
            return std::make_shared<DrawBitmapRect2>();
        } }, // OH_Drawing_CanvasDrawRect接口有问题内部逻辑并未用画笔而是用画刷
    { "bigbitmaprect", []() -> std::shared_ptr<TestBase> { return std::make_shared<DrawBitmapRect4>(false); } },
    { "canvasquickrejectpath",
        []() -> std::shared_ptr<TestBase> { return std::make_shared<FunctionCanvasQuickRejectPath>(); } },
    { "canvasquickrejectrect",
        []() -> std::shared_ptr<TestBase> { return std::make_shared<FunctionCanvasQuickRejectRect>(); } },
    { "canvasdrawarcwithcenter",
        []() -> std::shared_ptr<TestBase> { return std::make_shared<FunctionCanvasDrawArcWithCenter>(); } },
    { "canvasdrawnestedroundrect",
        []() -> std::shared_ptr<TestBase> { return std::make_shared<FunctionCanvasDrawNestedRoundRect>(); } },
    { "canvasdrawpixelmapnine",
        []() -> std::shared_ptr<TestBase> { return std::make_shared<FunctionCanvasDrawPixelMapNine>(); } },
    { "anisotropic_hq",
        []() -> std::shared_ptr<TestBase> {
            return std::make_shared<Anisotropic>();
        } }, // 该用例OH_Drawing_SamplingOptionsCreate接口mode对应内容未开放,无法实现
    // SUB_BASIC_GRAPHICS_FUNCTION_DRAWING_NDK_RECT_0201
    { "functionRectCopy", []() -> std::shared_ptr<TestBase> { return std::make_shared<FunctionRectCopy>(); } },
    // SUB_BASIC_GRAPHICS_FUNCTION_DRAWING_NDK_RECT_0201
    { "functionRectSetAndGet",
        []() -> std::shared_ptr<TestBase> { return std::make_shared<FunctionRectSetAndGet>(); } },
    // SUB_BASIC_GRAPHICS_FUNCTION_DRAWING_NDK_RECT_0201
    { "functionRectIntersect",
        []() -> std::shared_ptr<TestBase> { return std::make_shared<FunctionRectIntersect>(); } },
    // SUB_BASIC_GRAPHICS_FUNCTION_DRAWING_NDK_REGION_0401
    { "functionRegionSetRect",
        []() -> std::shared_ptr<TestBase> { return std::make_shared<FunctionRegionSetRect>(); } },
    // SUB_BASIC_GRAPHICS_FUNCTION_DRAWING_NDK_ROUND_RECT_0201
    { "functionRoundRectOffset",
        []() -> std::shared_ptr<TestBase> { return std::make_shared<FunctionRoundRectOffset>(); } },
    // SUB_BASIC_GRAPHICS_FUNCTION_DRAWING_NDK_ROUND_RECT_0301
    { "functionRoundRectCreate",
        []() -> std::shared_ptr<TestBase> { return std::make_shared<FunctionRoundRectCreate>(); } },
    // Function_null
    // SUB_BASIC_GRAPHICS_FUNCTION_DRAWING_NDK_BRUSH_1001
    { "functionBrushSetColor",
        []() -> std::shared_ptr<TestBase> { return std::make_shared<FunctionBrushSetColor>(); } },
    // SUB_BASIC_GRAPHICS_FUNCTION_DRAWING_NDK_BRUSH_1301
    { "functionBrushSetShaderEffect",
        []() -> std::shared_ptr<TestBase> { return std::make_shared<FunctionBrushSetShaderEffect>(); } },
    { "functionBrushSetColor4f",
        []() -> std::shared_ptr<TestBase> { return std::make_shared<FunctionBrushSetColor4f>(); } },
    { "functionBrushGetAlphaFloat",
        []() -> std::shared_ptr<TestBase> { return std::make_shared<FunctionBrushGetAlphaFloat>(); } },
    { "functionBrushGetRedFloat",
        []() -> std::shared_ptr<TestBase> { return std::make_shared<FunctionBrushGetRedFloat>(); } },
    { "functionBrushGetBlueFloat",
        []() -> std::shared_ptr<TestBase> { return std::make_shared<FunctionBrushGetBlueFloat>(); } },
    { "functionBrushGetGreenFloat",
        []() -> std::shared_ptr<TestBase> { return std::make_shared<FunctionBrushGetGreenFloat>(); } },
    // SUB_BASIC_GRAPHICS_FUNCTION_DRAWING_NDK_CANVAS_3601
    { "functionCanvasDrawLine",
        []() -> std::shared_ptr<TestBase> { return std::make_shared<FunctionCanvasDrawLine>(); } },
    // SUB_BASIC_GRAPHICS_FUNCTION_DRAWING_NDK_CANVAS_3801
    { "functionCanvasDrawRegion",
        []() -> std::shared_ptr<TestBase> { return std::make_shared<FunctionCanvasDrawRegion>(); } },
    // SUB_BASIC_GRAPHICS_FUNCTION_DRAWING_NDK_CANVAS_3901
    { "functionCanvasDrawPoint",
        []() -> std::shared_ptr<TestBase> { return std::make_shared<FunctionCanvasDrawPoint>(); } },
    // SUB_BASIC_GRAPHICS_FUNCTION_DRAWING_NDK_CANVAS_4001
    { "functionCanvasDrawPoints",
        []() -> std::shared_ptr<TestBase> { return std::make_shared<FunctionCanvasDrawPoints>(); } },
    // SUB_BASIC_GRAPHICS_FUNCTION_DRAWING_NDK_CANVAS_4101
    { "functionCanvasDrawOval",
        []() -> std::shared_ptr<TestBase> { return std::make_shared<FunctionCanvasDrawOval>(); } },
    // SUB_BASIC_GRAPHICS_FUNCTION_DRAWING_NDK_CANVAS_4201
    { "functionCanvasClipRoundRect",
        []() -> std::shared_ptr<TestBase> { return std::make_shared<FunctionCanvasClipRoundRect>(); } },
    // SUB_BASIC_GRAPHICS_FUNCTION_DRAWING_NDK_CANVAS_4202
    { "functionCanvasClipRoundRect",
        []() -> std::shared_ptr<TestBase> { return std::make_shared<FunctionCanvasClipRoundRect>(); } },
    // SUB_BASIC_GRAPHICS_FUNCTION_DRAWING_NDK_CANVAS_4202
    { "functionCanvasClipRoundRect2",
        []() -> std::shared_ptr<TestBase> { return std::make_shared<FunctionCanvasClipRoundRect2>(); } },
    // SUB_BASIC_GRAPHICS_FUNCTION_DRAWING_NDK_CANVAS_4301
    { "functionCanvasSkew", []() -> std::shared_ptr<TestBase> { return std::make_shared<FunctionCanvasSkew>(); } },
    // SUB_BASIC_GRAPHICS_FUNCTION_DRAWING_NDK_CANVAS_4501
    { "functionCanvasDrawImageRectWithSrc",
        []() -> std::shared_ptr<TestBase> { return std::make_shared<FunctionCanvasDrawImageRectWithSrc>(); } },
    // SUB_BASIC_GRAPHICS_FUNCTION_DRAWING_NDK_CANVAS_4701
    { "functionCanvasDrawVertices",
        []() -> std::shared_ptr<TestBase> {
            return std::make_shared<FunctionCanvasDrawVertices>(OH_Drawing_BlendMode::BLEND_MODE_SRC);
        } },
    { "functionCanvasDrawVertices2",
        []() -> std::shared_ptr<TestBase> {
            return std::make_shared<FunctionCanvasDrawVertices>(OH_Drawing_BlendMode::BLEND_MODE_DST);
        } },
    { "functionCanvasDrawVertices3",
        []() -> std::shared_ptr<TestBase> {
            return std::make_shared<FunctionCanvasDrawVertices>(OH_Drawing_BlendMode::BLEND_MODE_SRC_OVER);
        } },
    { "functionCanvasDrawVertices4",
        []() -> std::shared_ptr<TestBase> {
            return std::make_shared<FunctionCanvasDrawVertices>(OH_Drawing_BlendMode::BLEND_MODE_DST_OVER);
        } },
    { "functionCanvasDrawVertices5",
        []() -> std::shared_ptr<TestBase> {
            return std::make_shared<FunctionCanvasDrawVertices>(OH_Drawing_BlendMode::BLEND_MODE_SRC_IN);
        } },
    { "functionCanvasDrawVertices6",
        []() -> std::shared_ptr<TestBase> {
            return std::make_shared<FunctionCanvasDrawVertices>(OH_Drawing_BlendMode::BLEND_MODE_DST_IN);
        } },
    { "functionCanvasDrawVertices7",
        []() -> std::shared_ptr<TestBase> {
            return std::make_shared<FunctionCanvasDrawVertices>(OH_Drawing_BlendMode::BLEND_MODE_SRC_OUT);
        } },
    { "functionCanvasDrawVertices8",
        []() -> std::shared_ptr<TestBase> {
            return std::make_shared<FunctionCanvasDrawVertices>(OH_Drawing_BlendMode::BLEND_MODE_DST_OUT);
        } },
    { "functionCanvasDrawVertices9",
        []() -> std::shared_ptr<TestBase> {
            return std::make_shared<FunctionCanvasDrawVertices>(OH_Drawing_BlendMode::BLEND_MODE_SRC_ATOP);
        } },
    { "functionCanvasDrawVertices10",
        []() -> std::shared_ptr<TestBase> {
            return std::make_shared<FunctionCanvasDrawVertices>(OH_Drawing_BlendMode::BLEND_MODE_DST_ATOP);
        } },
    { "functionCanvasDrawVertices11",
        []() -> std::shared_ptr<TestBase> {
            return std::make_shared<FunctionCanvasDrawVertices>(OH_Drawing_BlendMode::BLEND_MODE_XOR);
        } },
    { "functionCanvasDrawVertices12",
        []() -> std::shared_ptr<TestBase> {
            return std::make_shared<FunctionCanvasDrawVertices>(OH_Drawing_BlendMode::BLEND_MODE_PLUS);
        } },
    { "functionCanvasDrawVertices13",
        []() -> std::shared_ptr<TestBase> {
            return std::make_shared<FunctionCanvasDrawVertices>(OH_Drawing_BlendMode::BLEND_MODE_MODULATE);
        } },
    { "functionCanvasDrawVertices14",
        []() -> std::shared_ptr<TestBase> {
            return std::make_shared<FunctionCanvasDrawVertices>(OH_Drawing_BlendMode::BLEND_MODE_SCREEN);
        } },
    { "functionCanvasDrawVertices15",
        []() -> std::shared_ptr<TestBase> {
            return std::make_shared<FunctionCanvasDrawVertices>(OH_Drawing_BlendMode::BLEND_MODE_OVERLAY);
        } },
    { "functionCanvasDrawVertices16",
        []() -> std::shared_ptr<TestBase> {
            return std::make_shared<FunctionCanvasDrawVertices>(OH_Drawing_BlendMode::BLEND_MODE_DARKEN);
        } },
    { "functionCanvasDrawVertices17",
        []() -> std::shared_ptr<TestBase> {
            return std::make_shared<FunctionCanvasDrawVertices>(OH_Drawing_BlendMode::BLEND_MODE_LIGHTEN);
        } },
    { "functionCanvasDrawVertices18",
        []() -> std::shared_ptr<TestBase> {
            return std::make_shared<FunctionCanvasDrawVertices>(OH_Drawing_BlendMode::BLEND_MODE_COLOR_DODGE);
        } },
    { "functionCanvasDrawVertices19",
        []() -> std::shared_ptr<TestBase> {
            return std::make_shared<FunctionCanvasDrawVertices>(OH_Drawing_BlendMode::BLEND_MODE_COLOR_BURN);
        } },
    { "functionCanvasDrawVertices20",
        []() -> std::shared_ptr<TestBase> {
            return std::make_shared<FunctionCanvasDrawVertices>(OH_Drawing_BlendMode::BLEND_MODE_HARD_LIGHT);
        } },
    { "functionCanvasDrawVertices21",
        []() -> std::shared_ptr<TestBase> {
            return std::make_shared<FunctionCanvasDrawVertices>(OH_Drawing_BlendMode::BLEND_MODE_SOFT_LIGHT);
        } },
    { "functionCanvasDrawVertices22",
        []() -> std::shared_ptr<TestBase> {
            return std::make_shared<FunctionCanvasDrawVertices>(OH_Drawing_BlendMode::BLEND_MODE_DIFFERENCE);
        } },
    { "functionCanvasDrawVertices23",
        []() -> std::shared_ptr<TestBase> {
            return std::make_shared<FunctionCanvasDrawVertices>(OH_Drawing_BlendMode::BLEND_MODE_EXCLUSION);
        } },
    { "functionCanvasDrawVertices24",
        []() -> std::shared_ptr<TestBase> {
            return std::make_shared<FunctionCanvasDrawVertices>(OH_Drawing_BlendMode::BLEND_MODE_MULTIPLY);
        } },
    { "functionCanvasDrawVertices25",
        []() -> std::shared_ptr<TestBase> {
            return std::make_shared<FunctionCanvasDrawVertices>(OH_Drawing_BlendMode::BLEND_MODE_HUE);
        } },
    { "functionCanvasDrawVertices26",
        []() -> std::shared_ptr<TestBase> {
            return std::make_shared<FunctionCanvasDrawVertices>(OH_Drawing_BlendMode::BLEND_MODE_SATURATION);
        } },
    { "functionCanvasDrawVertices27",
        []() -> std::shared_ptr<TestBase> {
            return std::make_shared<FunctionCanvasDrawVertices>(OH_Drawing_BlendMode::BLEND_MODE_COLOR);
        } },
    { "functionCanvasDrawVertices28",
        []() -> std::shared_ptr<TestBase> {
            return std::make_shared<FunctionCanvasDrawVertices>(OH_Drawing_BlendMode::BLEND_MODE_LUMINOSITY);
        } },
    { "functionCanvasIsClipEmpty",
        []() -> std::shared_ptr<TestBase> { return std::make_shared<FunctionCanvasIsClipEmpty>(); } },
    { "functionCanvasClipRegion",
        []() -> std::shared_ptr<TestBase> { return std::make_shared<FunctionCanvasClipRegion>(); } },
    // SUB_BASIC_GRAPHICS_FUNCTION_DRAWING_NDK_FILTER_0301
    { "functionFilterSetMaskFilter",
        []() -> std::shared_ptr<TestBase> { return std::make_shared<FunctionFilterSetMaskFilter>(); } },
    // SUB_BASIC_GRAPHICS_FUNCTION_DRAWING_NDK_FILTER_0401
    { "functionFilterSetColorFilter",
        []() -> std::shared_ptr<TestBase> { return std::make_shared<FunctionFilterSetColorFilter>(); } },
    // SUB_BASIC_GRAPHICS_FUNCTION_DRAWING_NDK_FONT_2901
    { "functionFontMeasureText",
        []() -> std::shared_ptr<TestBase> { return std::make_shared<FunctionFontMeasureText>(); } },
    // SUB_BASIC_GRAPHICS_FUNCTION_DRAWING_NDK_PATH_2601
    { "functionPathReset", []() -> std::shared_ptr<TestBase> { return std::make_shared<FunctionPathReset>(); } },
    // SUB_BASIC_GRAPHICS_FUNCTION_DRAWING_NDK_PATH_2701
    { "functionPathAddOval", []() -> std::shared_ptr<TestBase> { return std::make_shared<FunctionPathAddOval>(); } },
    // SUB_BASIC_GRAPHICS_FUNCTION_DRAWING_NDK_PATH_2801
    { "functionPathTransformWithPerspectiveClip",
        []() -> std::shared_ptr<TestBase> { return std::make_shared<FunctionPathTransformWithPerspectiveClip>(); } },
    // SUB_BASIC_GRAPHICS_FUNCTION_DRAWING_NDK_PATH_2901
    { "functionPathAddPathWithOffsetAndMode",
        []() -> std::shared_ptr<TestBase> { return std::make_shared<FunctionPathAddPathWithOffsetAndMode>(); } },
    // SUB_BASIC_GRAPHICS_FUNCTION_DRAWING_NDK_PATH_3001
    { "functionPathAddPathWithMode",
        []() -> std::shared_ptr<TestBase> { return std::make_shared<FunctionPathAddPathWithMode>(); } },
    // SUB_BASIC_GRAPHICS_FUNCTION_DRAWING_NDK_PATH_3101
    { "functionPathAddPathWithMatrixAndMode",
        []() -> std::shared_ptr<TestBase> { return std::make_shared<FunctionPathAddPathWithMatrixAndMode>(); } },
    // SUB_BASIC_GRAPHICS_FUNCTION_DRAWING_NDK_PATH_3201
    { "functionPathAddOvalWithInitialPoint",
        []() -> std::shared_ptr<TestBase> { return std::make_shared<FunctionPathAddOvalWithInitialPoint>(); } },
    /// SUB_BASIC_GRAPHICS_FUNCTION_DRAWING_NDK_PATH_3301
    { "functionPathAddRectWithInitialCornerTest",
        []() -> std::shared_ptr<TestBase> { return std::make_shared<FunctionPathAddRectWithInitialCornerTest>(); } },
    // SUB_BASIC_GRAPHICS_FUNCTION_DRAWING_NDK_PATH_3401
    { "functionPathRCubicToTest",
        []() -> std::shared_ptr<TestBase> { return std::make_shared<FunctionPathRCubicToTest>(); } },
    // SUB_BASIC_GRAPHICS_FUNCTION_DRAWING_NDK_PATH_3501
    { "functionPathRConicToTest",
        []() -> std::shared_ptr<TestBase> { return std::make_shared<FunctionPathRConicToTest>(); } },
    // SUB_BASIC_GRAPHICS_FUNCTION_DRAWING_NDK_PATH_3601
    { "functionPathRQuadToTest",
        []() -> std::shared_ptr<TestBase> { return std::make_shared<FunctionPathRQuadToTest>(); } },
    // SUB_BASIC_GRAPHICS_FUNCTION_DRAWING_NDK_PATH_3701
    { "functionPathRLineToTest",
        []() -> std::shared_ptr<TestBase> { return std::make_shared<FunctionPathRLineToTest>(); } },
    // SUB_BASIC_GRAPHICS_FUNCTION_DRAWING_NDK_PATH_3801
    { "functionPathRMoveToTest",
        []() -> std::shared_ptr<TestBase> { return std::make_shared<FunctionPathRMoveToTest>(); } },
    // SUB_BASIC_GRAPHICS_FUNCTION_DRAWING_NDK_PATH_3901
    { "functionPathConicToTest",
        []() -> std::shared_ptr<TestBase> { return std::make_shared<FunctionPathConicToTest>(); } },
    // SUB_BASIC_GRAPHICS_FUNCTION_DRAWING_NDK_PATH_4001
    { "functionPathArcToTest",
        []() -> std::shared_ptr<TestBase> { return std::make_shared<FunctionPathArcToTest>(); } },
    // SUB_BASIC_GRAPHICS_FUNCTION_DRAWING_NDK_PEN_2001
    { "functionPenSetShaderEffectNullTest",
        []() -> std::shared_ptr<TestBase> { return std::make_shared<FunctionPenSetShaderEffectNullTest>(); } },
    { "functionPenSetColor4fNullTest",
        []() -> std::shared_ptr<TestBase> { return std::make_shared<FunctionPenSetColor4fNullTest>(); } },
    { "functionPenGetAlphaFloatNullTest",
        []() -> std::shared_ptr<TestBase> { return std::make_shared<FunctionPenGetAlphaFloatNullTest>(); } },
    { "functionPenGetRedFloatNullTest",
        []() -> std::shared_ptr<TestBase> { return std::make_shared<FunctionPenGetRedFloatNullTest>(); } },
    { "functionPenGetBlueFloatNullTest",
        []() -> std::shared_ptr<TestBase> { return std::make_shared<FunctionPenGetBlueFloatNullTest>(); } },
    { "functionPenGetGreenFloatNullTest",
        []() -> std::shared_ptr<TestBase> { return std::make_shared<FunctionPenGetGreenFloatNullTest>(); } },

    // path effect
    { "patheffectcreatecornerpatheffect",
        []() -> std::shared_ptr<TestBase> {
            return std::make_shared<FunctionPathEffectCreateCornerPathEffect>(false);
        } },
    { "patheffectcreatediscretepatheffect",
        []() -> std::shared_ptr<TestBase> {
            return std::make_shared<FunctionPathEffectCreateDiscretePathEffect>(false);
        } },
    { "patheffectcreatecomposepatheffect",
        []() -> std::shared_ptr<TestBase> {
            return std::make_shared<FunctionPathEffectCreateComposePathEffect>(false);
        } },
    { "patheffectcreatepathdasheffect",
        []() -> std::shared_ptr<TestBase> {
            return std::make_shared<FunctionPathEffectCreatePathDashEffect>(false);
        } },
    { "patheffectcreatesumpatheffect",
        []() -> std::shared_ptr<TestBase> {
            return std::make_shared<FunctionPathEffectCreateSumPathEffect>(false);
        } },
};

std::unordered_map<std::string, std::function<std::shared_ptr<TestBase>()>> PerformanceCpuMap = {

    // interface canvas
    { "canvas_drawrect",
        []() -> std::shared_ptr<TestBase> { return std::make_shared<CanvasDrawRect>(TestBase::DRAW_STYLE_COMPLEX); } },
    { "canvas_rotate",
        []() -> std::shared_ptr<TestBase> { return std::make_shared<CanvasRotate>(TestBase::DRAW_STYLE_COMPLEX); } },
    { "canvas_drawimagerect",
        []() -> std::shared_ptr<TestBase> {
            return std::make_shared<CanvasDrawImageRect>(TestBase::DRAW_STYLE_COMPLEX);
        } },
    { "canvas_translate",
        []() -> std::shared_ptr<TestBase> { return std::make_shared<CanvasTranslate>(TestBase::DRAW_STYLE_COMPLEX); } },
    { "canvas_scale",
        []() -> std::shared_ptr<TestBase> { return std::make_shared<CanvasScale>(TestBase::DRAW_STYLE_COMPLEX); } },
    { "canvas_clear",
        []() -> std::shared_ptr<TestBase> { return std::make_shared<CanvasClear>(TestBase::DRAW_STYLE_COMPLEX); } },
    { "canvas_setmatrix",
        []() -> std::shared_ptr<TestBase> { return std::make_shared<CanvasSetMatrix>(TestBase::DRAW_STYLE_COMPLEX); } },
    { "canvas_resetmatrix",
        []() -> std::shared_ptr<TestBase> {
            return std::make_shared<CanvasResetMatrix>(TestBase::DRAW_STYLE_COMPLEX);
        } },
    { "canvas_readpixels",
        []() -> std::shared_ptr<TestBase> {
            return std::make_shared<CanvasReadPixels>(TestBase::DRAW_STYLE_COMPLEX);
        } },
    { "canvas_readpixelstobitmap",
        []() -> std::shared_ptr<TestBase> {
            return std::make_shared<CanvasReadPixelsToBitmap>(TestBase::DRAW_STYLE_COMPLEX);
        } },
    { "canvas_drawimagerectwithsrc",
        []() -> std::shared_ptr<TestBase> {
            return std::make_shared<CanvasDrawImageRectWithSrc>(TestBase::DRAW_STYLE_COMPLEX);
        } },
    { "canvas_drawshadow",
        []() -> std::shared_ptr<TestBase> {
            return std::make_shared<CanvasDrawShadow>(TestBase::DRAW_STYLE_COMPLEX);
        } },
    { "canvas_skew",
        []() -> std::shared_ptr<TestBase> { return std::make_shared<CanvasSkew>(TestBase::DRAW_STYLE_COMPLEX); } },
    { "canvas_drawvertices",
        []() -> std::shared_ptr<TestBase> {
            return std::make_shared<CanvasDrawVertices>(TestBase::DRAW_STYLE_COMPLEX);
        } },
    { "canvas_drawbitmaprect",
        []() -> std::shared_ptr<TestBase> {
            return std::make_shared<CanvasDrawBitmapRect>(TestBase::DRAW_STYLE_COMPLEX);
        } },
    { "canvas_drawcircle",
        []() -> std::shared_ptr<TestBase> {
            return std::make_shared<CanvasDrawCircle>(TestBase::DRAW_STYLE_COMPLEX);
        } },
    { "canvas_drawoval",
        []() -> std::shared_ptr<TestBase> { return std::make_shared<CanvasDrawOval>(TestBase::DRAW_STYLE_COMPLEX); } },
    { "canvas_drawarc",
        []() -> std::shared_ptr<TestBase> { return std::make_shared<CanvasDrawArc>(TestBase::DRAW_STYLE_COMPLEX); } },
    { "canvas_drawroundrect",
        []() -> std::shared_ptr<TestBase> {
            return std::make_shared<CanvasDrawRoundRect>(TestBase::DRAW_STYLE_COMPLEX);
        } },
    { "canvas_drawtextblob",
        []() -> std::shared_ptr<TestBase> {
            return std::make_shared<CanvasDrawTextBlob>(TestBase::DRAW_STYLE_COMPLEX);
        } },
    { "canvas_drawcliprect",
        []() -> std::shared_ptr<TestBase> { return std::make_shared<CanvasClipRect>(TestBase::DRAW_STYLE_COMPLEX); } },
    { "canvas_drawcliproundrect",
        []() -> std::shared_ptr<TestBase> {
            return std::make_shared<CanvasClipRoundRect>(TestBase::DRAW_STYLE_COMPLEX);
        } },
    { "canvas_drawclippath",
        []() -> std::shared_ptr<TestBase> { return std::make_shared<CanvasClipPath>(TestBase::DRAW_STYLE_COMPLEX); } },
    { "canvas_drawpath",
        []() -> std::shared_ptr<TestBase> { return std::make_shared<CanvasDrawPath>(TestBase::DRAW_STYLE_COMPLEX); } },
    { "canvas_drawbitmap",
        []() -> std::shared_ptr<TestBase> {
            return std::make_shared<CanvasDrawBitmap>(TestBase::DRAW_STYLE_COMPLEX);
        } },
    { "canvas_drawlayer",
        []() -> std::shared_ptr<TestBase> { return std::make_shared<CanvasDrawLayer>(TestBase::DRAW_STYLE_COMPLEX); } },
    { "canvas_drawpoints",
        []() -> std::shared_ptr<TestBase> {
            return std::make_shared<CanvasDrawPoints>(TestBase::DRAW_STYLE_COMPLEX);
        } },
    { "canvas_drawline",
        []() -> std::shared_ptr<TestBase> { return std::make_shared<CanvasDrawLine>(TestBase::DRAW_STYLE_COMPLEX); } },
    { "canvas_drawbackground",
        []() -> std::shared_ptr<TestBase> {
            return std::make_shared<CanvasDrawBackground>(TestBase::DRAW_STYLE_COMPLEX);
        } },
    { "canvas_drawregion",
        []() -> std::shared_ptr<TestBase> {
            return std::make_shared<CanvasDrawRegion>(TestBase::DRAW_STYLE_COMPLEX);
        } },
    { "canvas_restoretocount",
        []() -> std::shared_ptr<TestBase> {
            return std::make_shared<CanvasRestoreToCount>(TestBase::DRAW_STYLE_COMPLEX);
        } },
    { "canvas_save",
        []() -> std::shared_ptr<TestBase> { return std::make_shared<CanvasSave>(TestBase::DRAW_STYLE_COMPLEX); } },
    { "canvas_drawpixelmaprect",
        []() -> std::shared_ptr<TestBase> {
            return std::make_shared<CanvasDrawPixelMapRect>(TestBase::DRAW_STYLE_COMPLEX);
        } }, // OH_PixelMap_CreatePixelMap传入参数napi_env env无法获取，无法实现
    { "canvas_gettotalmatrix",
        []() -> std::shared_ptr<TestBase> {
            return std::make_shared<CanvasGetTotalMatrix>(TestBase::DRAW_STYLE_COMPLEX);
        } },
    { "canvas_restore",
        []() -> std::shared_ptr<TestBase> { return std::make_shared<CanvasRestore>(TestBase::DRAW_STYLE_COMPLEX); } },
    { "canvas_getlocalclipbounds",
        []() -> std::shared_ptr<TestBase> {
            return std::make_shared<CanvasGetLocalClipBounds>(TestBase::DRAW_STYLE_COMPLEX);
        } },
    { "canvas_getsavecount",
        []() -> std::shared_ptr<TestBase> {
            return std::make_shared<CanvasGetSaveCount>(TestBase::DRAW_STYLE_COMPLEX);
        } },
    { "canvas_getheight",
        []() -> std::shared_ptr<TestBase> { return std::make_shared<CanvasGetHeight>(TestBase::DRAW_STYLE_COMPLEX); } },
    { "canvas_getwidth",
        []() -> std::shared_ptr<TestBase> { return std::make_shared<CanvasGetWidth>(TestBase::DRAW_STYLE_COMPLEX); } },
    { "canvas_quickrejectpath",
        []() -> std::shared_ptr<TestBase> { return std::make_shared<PerformanceCanvasQuickRejectPath>(); } },
    { "canvas_quickrejectrect",
        []() -> std::shared_ptr<TestBase> { return std::make_shared<PerformanceCanvasQuickRejectRect>(); } },
    { "canvas_drawarcwithcenter",
        []() -> std::shared_ptr<TestBase> { return std::make_shared<PerformanceCanvasDrawArcWithCenter>(); } },
    { "canvas_drawnestedroundrect",
        []() -> std::shared_ptr<TestBase> { return std::make_shared<PerformanceCanvasDrawNestedRoundRect>(); } },
    { "canvas_drawpixelmapnine",
        []() -> std::shared_ptr<TestBase> { return std::make_shared<PerformanceCanvasDrawImageNine>(); } },

    // path
    { "path_create",
        []() -> std::shared_ptr<TestBase> { return std::make_shared<PathCreate>(TestBase::DRAW_STYLE_COMPLEX); } },
    { "path_lineto",
        []() -> std::shared_ptr<TestBase> { return std::make_shared<PathLineTo>(TestBase::DRAW_STYLE_COMPLEX); } },
    { "path_arcto",
        []() -> std::shared_ptr<TestBase> { return std::make_shared<PathArcTo>(TestBase::DRAW_STYLE_COMPLEX); } },
    { "path_quadto",
        []() -> std::shared_ptr<TestBase> { return std::make_shared<PathQuadTo>(TestBase::DRAW_STYLE_COMPLEX); } },
    { "path_copy",
        []() -> std::shared_ptr<TestBase> { return std::make_shared<PathCopy>(TestBase::DRAW_STYLE_COMPLEX); } },
    { "path_effectcreatedashpatheffect",
        []() -> std::shared_ptr<TestBase> {
            return std::make_shared<PathEffectCreateDashPathEffect>(TestBase::DRAW_STYLE_COMPLEX);
        } },
    { "path_addovalwithinitialpoint",
        []() -> std::shared_ptr<TestBase> {
            return std::make_shared<PathAddOvalWithInitialPoint>(TestBase::DRAW_STYLE_COMPLEX);
        } },
    { "path_addarc",
        []() -> std::shared_ptr<TestBase> { return std::make_shared<PathAddArc>(TestBase::DRAW_STYLE_COMPLEX); } },
    { "path_addpathwithmatrixandmode",
        []() -> std::shared_ptr<TestBase> {
            return std::make_shared<PathAddPathWithMatrixAndMode>(TestBase::DRAW_STYLE_COMPLEX);
        } },
    { "path_conicto",
        []() -> std::shared_ptr<TestBase> { return std::make_shared<PathConicTo>(TestBase::DRAW_STYLE_COMPLEX); } },
    { "path_transformwithclip",
        []() -> std::shared_ptr<TestBase> {
            return std::make_shared<PathTransformWithPerspectiveClip>(TestBase::DRAW_STYLE_COMPLEX, false);
        } },
    { "path_setfilltype",
        []() -> std::shared_ptr<TestBase> {
            return std::make_shared<PathSetFillType>(
                TestBase::DRAW_STYLE_COMPLEX, OH_Drawing_PathFillType::PATH_FILL_TYPE_WINDING);
        } },
    { "path_getlength",
        []() -> std::shared_ptr<TestBase> {
            return std::make_shared<PathGetLength>(TestBase::DRAW_STYLE_COMPLEX, true);
        } },
    { "path_close",
        []() -> std::shared_ptr<TestBase> { return std::make_shared<PathClose>(TestBase::DRAW_STYLE_COMPLEX); } },
    { "path_offset",
        []() -> std::shared_ptr<TestBase> { return std::make_shared<PathOffset>(TestBase::DRAW_STYLE_COMPLEX); } },
    { "path_reset",
        []() -> std::shared_ptr<TestBase> { return std::make_shared<PathReset>(TestBase::DRAW_STYLE_COMPLEX); } },
    { "path_transform",
        []() -> std::shared_ptr<TestBase> { return std::make_shared<PathTransform>(TestBase::DRAW_STYLE_COMPLEX); } },
    { "path_addoval",
        []() -> std::shared_ptr<TestBase> { return std::make_shared<PathAddOval>(TestBase::DRAW_STYLE_COMPLEX); } },
    { "path_contain",
        []() -> std::shared_ptr<TestBase> { return std::make_shared<PathContains>(TestBase::DRAW_STYLE_COMPLEX); } },
    { "path_cubicto",
        []() -> std::shared_ptr<TestBase> { return std::make_shared<PathCubicTo>(TestBase::DRAW_STYLE_COMPLEX); } },
    { "path_rmoveto",
        []() -> std::shared_ptr<TestBase> { return std::make_shared<PathRMoveTo>(TestBase::DRAW_STYLE_COMPLEX); } },
    { "path_rlineto",
        []() -> std::shared_ptr<TestBase> { return std::make_shared<PathRLineTo>(TestBase::DRAW_STYLE_COMPLEX); } },
    { "path_rquadto",
        []() -> std::shared_ptr<TestBase> { return std::make_shared<PathRQuadTo>(TestBase::DRAW_STYLE_COMPLEX); } },
    { "path_rconicto",
        []() -> std::shared_ptr<TestBase> { return std::make_shared<PathRConicTo>(TestBase::DRAW_STYLE_COMPLEX); } },
    { "path_rcubicto",
        []() -> std::shared_ptr<TestBase> { return std::make_shared<PathRCubicTo>(TestBase::DRAW_STYLE_COMPLEX); } },
    { "path_addrect",
        []() -> std::shared_ptr<TestBase> { return std::make_shared<PathAddRect>(TestBase::DRAW_STYLE_COMPLEX); } },
    { "path_addrect_withcorner",
        []() -> std::shared_ptr<TestBase> {
            return std::make_shared<PathAddRectWithInitialCorner>(TestBase::DRAW_STYLE_COMPLEX);
        } },
    { "path_addroundrect",
        []() -> std::shared_ptr<TestBase> {
            return std::make_shared<PathAddRoundRect>(TestBase::DRAW_STYLE_COMPLEX);
        } },
    { "path_addpath",
        []() -> std::shared_ptr<TestBase> { return std::make_shared<PathAddPath>(TestBase::DRAW_STYLE_COMPLEX); } },
    { "path_addpathwithmode",
        []() -> std::shared_ptr<TestBase> {
            return std::make_shared<PathAddPathWithMode>(
                TestBase::DRAW_STYLE_COMPLEX, OH_Drawing_PathAddMode::PATH_ADD_MODE_APPEND);
        } },
    { "path_addpathwithoffsetmode",
        []() -> std::shared_ptr<TestBase> {
            return std::make_shared<PathAddPathWithOffsetAndMode>(
                TestBase::DRAW_STYLE_COMPLEX, OH_Drawing_PathAddMode::PATH_ADD_MODE_APPEND);
        } },

    // path effect
    { "patheffect_createcornerpatheffect",
        []() -> std::shared_ptr<TestBase> {
            return std::make_shared<PerformancePathEffectCreateDiscretePathEffect>(false);
        } },
    { "patheffect_creatediscretepatheffect",
        []() -> std::shared_ptr<TestBase> {
            return std::make_shared<PerformancePathEffectCreateCornerPathEffect>(false);
        } },
    { "patheffect_createcomposepatheffect",
        []() -> std::shared_ptr<TestBase> {
            return std::make_shared<PerformancePathEffectCreateComposePathEffect>(false);
        } },
    { "patheffect_createpathdasheffect",
        []() -> std::shared_ptr<TestBase> {
            return std::make_shared<PerformancePathEffectCreatePathDashEffect>(false);
        } },
    { "patheffect_createsumpatheffect",
        []() -> std::shared_ptr<TestBase> {
            return std::make_shared<PerformancePathEffectCreateSumPathEffect>(false);
        } },

    // textblob
    { "textblob_createbuilder",
        []() -> std::shared_ptr<TestBase> {
            return std::make_shared<TextBlobBuilderCreate>(TestBase::DRAW_STYLE_COMPLEX);
        } },
    { "textblob_createfrom_text",
        []() -> std::shared_ptr<TestBase> {
            return std::make_shared<TextBlobCreateFromText>(TestBase::DRAW_STYLE_COMPLEX);
        } },
    { "textblob_createfrom_postext",
        []() -> std::shared_ptr<TestBase> {
            return std::make_shared<TextBlobCreateFromPosText>(TestBase::DRAW_STYLE_COMPLEX);
        } },
    { "textblob_createfrom_string",
        []() -> std::shared_ptr<TestBase> {
            return std::make_shared<TextBlobCreateFromString>(TestBase::DRAW_STYLE_COMPLEX);
        } },
    { "textblob_getbounds",
        []() -> std::shared_ptr<TestBase> {
            return std::make_shared<TextBlobGetBounds>(TestBase::DRAW_STYLE_COMPLEX);
        } },
    { "textblob_builderallocrunpos",
        []() -> std::shared_ptr<TestBase> {
            return std::make_shared<TextBlobBuilderAllocRunPos>(TestBase::DRAW_STYLE_COMPLEX);
        } },

    // shader_effect
    { "shader_effectcreatelineargradient",
        []() -> std::shared_ptr<TestBase> {
            return std::make_shared<ShaderEffectCreateLinearGradient>(TestBase::DRAW_STYLE_COMPLEX);
        } },
    { "shader_effectcreateradialgradient",
        []() -> std::shared_ptr<TestBase> {
            return std::make_shared<ShaderEffectCreateRadialGradient>(TestBase::DRAW_STYLE_COMPLEX);
        } },
    { "shader_effectcreateimageshader",
        []() -> std::shared_ptr<TestBase> {
            return std::make_shared<ShaderEffectCreateImageShader>(TestBase::DRAW_STYLE_COMPLEX);
        } },
    { "shader_effectcreatesweepgradient",
        []() -> std::shared_ptr<TestBase> {
            return std::make_shared<ShaderEffectCreateSweepGradient>(TestBase::DRAW_STYLE_COMPLEX);
        } },

    // typeface
    { "typeface_createdefault",
        []() -> std::shared_ptr<TestBase> {
            return std::make_shared<TypefaceCreateDefault>(TestBase::DRAW_STYLE_COMPLEX);
        } },
    { "typeface_createfromfile",
        []() -> std::shared_ptr<TestBase> {
            return std::make_shared<TypefaceCreateFromFile>(TestBase::DRAW_STYLE_COMPLEX);
        } },
    { "typeface_createfromstream",
        []() -> std::shared_ptr<TestBase> {
            return std::make_shared<TypefaceCreateFromStream>(TestBase::DRAW_STYLE_COMPLEX);
        } },

    // matrix
    { "matrix_reset",
        []() -> std::shared_ptr<TestBase> { return std::make_shared<MatrixReset>(TestBase::DRAW_STYLE_COMPLEX); } },
    { "matrix_concat",
        []() -> std::shared_ptr<TestBase> { return std::make_shared<MatrixConcat>(TestBase::DRAW_STYLE_COMPLEX); } },
    { "matrix_invert",
        []() -> std::shared_ptr<TestBase> { return std::make_shared<MatrixInvert>(TestBase::DRAW_STYLE_COMPLEX); } },
    { "matrix_setpolytopoly",
        []() -> std::shared_ptr<TestBase> {
            return std::make_shared<MatrixSetPolyToPoly>(TestBase::DRAW_STYLE_COMPLEX);
        } },
    { "matrix_prerotate",
        []() -> std::shared_ptr<TestBase> { return std::make_shared<MatrixPreRotate>(TestBase::DRAW_STYLE_COMPLEX); } },
    { "matrix_postscale",
        []() -> std::shared_ptr<TestBase> { return std::make_shared<MatrixPostScale>(TestBase::DRAW_STYLE_COMPLEX); } },
    { "matrix_posttranslate",
        []() -> std::shared_ptr<TestBase> {
            return std::make_shared<MatrixPostTranslate>(TestBase::DRAW_STYLE_COMPLEX);
        } },
    { "matrix_is_equal",
        []() -> std::shared_ptr<TestBase> { return std::make_shared<MatrixIsEqual>(TestBase::DRAW_STYLE_COMPLEX); } },

    // mask_filter
    { "mask_filter_createblur",
        []() -> std::shared_ptr<TestBase> {
            return std::make_shared<MaskFilterCreateBlur>(TestBase::DRAW_STYLE_COMPLEX);
        } },
    // color
    { "color_filtercreateblendmode",
        []() -> std::shared_ptr<TestBase> {
            return std::make_shared<ColorFilterCreateBlendMode>(TestBase::DRAW_STYLE_COMPLEX);
        } },
    { "color_filtercreatecompose",
        []() -> std::shared_ptr<TestBase> {
            return std::make_shared<ColorFilterCreateCompose>(TestBase::DRAW_STYLE_COMPLEX);
        } },
    { "color_filtercreatematrix",
        []() -> std::shared_ptr<TestBase> {
            return std::make_shared<ColorFilterCreateMatrix>(TestBase::DRAW_STYLE_COMPLEX);
        } },
    { "color_filtercreatelineartosrgbgamma",
        []() -> std::shared_ptr<TestBase> {
            return std::make_shared<ColorFilterCreateLinearToSrgbGamma>(TestBase::DRAW_STYLE_COMPLEX);
        } },
    { "color_filtercreatesrgbgammatolinear",
        []() -> std::shared_ptr<TestBase> {
            return std::make_shared<ColorFilterCreateSrgbGammaToLinear>(TestBase::DRAW_STYLE_COMPLEX);
        } },
    { "color_filtercreateluma",
        []() -> std::shared_ptr<TestBase> {
            return std::make_shared<ColorFilterCreateLuma>(TestBase::DRAW_STYLE_COMPLEX);
        } },
    { "color_spacecreatesrgb",
        []() -> std::shared_ptr<TestBase> {
            return std::make_shared<ColorSpaceCreateSrgb>(TestBase::DRAW_STYLE_COMPLEX);
        } },
    { "color_spacecreatesrgblinear",
        []() -> std::shared_ptr<TestBase> {
            return std::make_shared<ColorSpaceCreateSrgbLinear>(TestBase::DRAW_STYLE_COMPLEX);
        } },
    // bitmap and image
    { "bitmap_readpixels",
        []() -> std::shared_ptr<TestBase> {
            return std::make_shared<BitmapReadPixels>(
                TestBase::DRAW_STYLE_COMPLEX, COLOR_FORMAT_RGBA_8888, ALPHA_FORMAT_OPAQUE);
        } },
    { "image_buildfrombitmap",
        []() -> std::shared_ptr<TestBase> {
            return std::make_shared<ImageBuildFromBitmap>(TestBase::DRAW_STYLE_COMPLEX);
        } },
    { "bitmap_build",
        []() -> std::shared_ptr<TestBase> { return std::make_shared<BitmapBuild>(TestBase::DRAW_STYLE_COMPLEX); } },
    { "bitmap_createfrompixels",
        []() -> std::shared_ptr<TestBase> {
            return std::make_shared<BitmapCreateFromPixels>(TestBase::DRAW_STYLE_COMPLEX);
        } },

    // textblob
    { "textblob_builderallocrunpos",
        []() -> std::shared_ptr<TestBase> {
            return std::make_shared<TextBlobBuilderAllocRunPos>(TestBase::DRAW_STYLE_COMPLEX);
        } },

    // pen
    { "pen_reset",
        []() -> std::shared_ptr<TestBase> { return std::make_shared<PenReset>(TestBase::DRAW_STYLE_COMPLEX); } },
    { "pen_set_color4f",
        []() -> std::shared_ptr<TestBase> { return std::make_shared<PenSetColor4f>(TestBase::DRAW_STYLE_COMPLEX); } },
    { "pen_get_alpha_float",
        []() -> std::shared_ptr<TestBase> {
            return std::make_shared<PenGetAlphaFloat>(TestBase::DRAW_STYLE_COMPLEX);
        } },
    { "pen_get_red_float",
        []() -> std::shared_ptr<TestBase> { return std::make_shared<PenGetRedFloat>(TestBase::DRAW_STYLE_COMPLEX); } },
    { "pen_get_blue_float",
        []() -> std::shared_ptr<TestBase> { return std::make_shared<PenGetBlueFloat>(TestBase::DRAW_STYLE_COMPLEX); } },
    { "pen_get_green_float",
        []() -> std::shared_ptr<TestBase> {
            return std::make_shared<PenGetGreenFloat>(TestBase::DRAW_STYLE_COMPLEX);
        } },

    // sampling_option
    { "sampling_options_create",
        []() -> std::shared_ptr<TestBase> {
            return std::make_shared<SamplingOptionsCreate>(TestBase::DRAW_STYLE_COMPLEX,
                OH_Drawing_FilterMode::FILTER_MODE_LINEAR, OH_Drawing_MipmapMode::MIPMAP_MODE_NEAREST);
        } },

    // memory_stream
    { "memory_stream_create",
        []() -> std::shared_ptr<TestBase> {
            return std::make_shared<MemoryStreamCreate>(TestBase::DRAW_STYLE_COMPLEX, true);
        } },
    { "surface_create_from_gpu_context",
        []() -> std::shared_ptr<TestBase> {
            return std::make_shared<SurfaceCreateFromGpuContext>(TestBase::DRAW_STYLE_COMPLEX, false);
        } }, // 只能用gpu来画，用cpu会闪退

    // font
    { "fontcounttext",
        []() -> std::shared_ptr<TestBase> { return std::make_shared<FontCountText>(TestBase::DRAW_STYLE_COMPLEX); } },

    // brush
    { "brushreset",
        []() -> std::shared_ptr<TestBase> { return std::make_shared<BrushReset>(TestBase::DRAW_STYLE_COMPLEX); } },
    { "brush_set_color4f",
        []() -> std::shared_ptr<TestBase> { return std::make_shared<BrushSetColor4f>(TestBase::DRAW_STYLE_COMPLEX); } },
    { "brush_get_alpha_float",
        []() -> std::shared_ptr<TestBase> {
            return std::make_shared<BrushGetAlphaFloat>(TestBase::DRAW_STYLE_COMPLEX);
        } },
    { "brush_get_red_float",
        []() -> std::shared_ptr<TestBase> {
            return std::make_shared<BrushGetRedFloat>(TestBase::DRAW_STYLE_COMPLEX);
        } },
    { "brush_get_blue_float",
        []() -> std::shared_ptr<TestBase> {
            return std::make_shared<BrushGetBlueFloat>(TestBase::DRAW_STYLE_COMPLEX);
        } },
    { "brush_get_green_float",
        []() -> std::shared_ptr<TestBase> {
            return std::make_shared<BrushGetGreenFloat>(TestBase::DRAW_STYLE_COMPLEX);
        } },
    // todo: test count is 10,SUB_BASIC_GRAPHICS_SPECIAL_PERFORMANCE_C_OPSWITCH_0200
    { "opsWitchTest",
        []() -> std::shared_ptr<TestBase> {
            return std::make_shared<PerformanceDrawOpsWitch>((DRAWING_TYPE_ATTACH_BOTH));
        } },
    // todo: test count is 10,SUB_BASIC_GRAPHICS_SPECIAL_PERFORMANCE_C_OPSWITCH_0200
    { "opsWitchTestPen",
        []() -> std::shared_ptr<TestBase> {
            return std::make_shared<PerformanceDrawOpsWitch>(DRAWING_TYPE_ATTACH_PEN);
        } },
    // todo: test count is 10,SUB_BASIC_GRAPHICS_SPECIAL_PERFORMANCE_C_OPSWITCH_0200
    { "opsWitchTestBrush",
        []() -> std::shared_ptr<TestBase> {
            return std::make_shared<PerformanceDrawOpsWitch>(DRAWING_TYPE_ATTACH_BRUSH);
        } },
    // todo: test count is 100,SUB_BASIC_GRAPHICS_SPECIAL_PERFORMANCE_C_OPSWITCH_0100
    { "opsWitchLineAndPath",
        []() -> std::shared_ptr<TestBase> {
            return std::make_shared<PerformanceDrawLineAndPath>(DRAWING_TYPE_ATTACH_BOTH);
        } },
    // todo: test count is 100,SUB_BASIC_GRAPHICS_SPECIAL_PERFORMANCE_C_OPSWITCH_0100
    { "opsWitchLineAndPathPen",
        []() -> std::shared_ptr<TestBase> {
            return std::make_shared<PerformanceDrawLineAndPath>(DRAWING_TYPE_ATTACH_PEN);
        } },
    // todo: test count is 100,SUB_BASIC_GRAPHICS_SPECIAL_PERFORMANCE_C_OPSWITCH_0100
    { "opsWitchLineAndPathBrush",
        []() -> std::shared_ptr<TestBase> {
            return std::make_shared<PerformanceDrawLineAndPath>(DRAWING_TYPE_ATTACH_BRUSH);
        } },
    // todo: test count is 10,SUB_BASIC_GRAPHICS_SPECIAL_PERFORMANCE_C_PROPERTY_1700
    { "opsWitchDrawVerticesBrush",
        []() -> std::shared_ptr<TestBase> {
            return std::make_shared<PerformanceCCanvasDrawVerticesBrush>(DRAWING_TYPE_ATTACH_BRUSH);
        } },
    // SUB_BASIC_GRAPHICS_SPECIAL_PERFORMANCE_C_PROPERTY_1800
    { "opsWitchReadPixels",
        []() -> std::shared_ptr<TestBase> {
            return std::make_shared<PerformanceCCanvasReadPixels>(DRAWING_TYPE_ATTACH_BOTH);
        } },
    // SUB_BASIC_GRAPHICS_SPECIAL_PERFORMANCE_C_PROPERTY_1800
    { "opsWitchReadPixelsPen",
        []() -> std::shared_ptr<TestBase> {
            return std::make_shared<PerformanceCCanvasReadPixels>(DRAWING_TYPE_ATTACH_PEN);
        } },
    // SUB_BASIC_GRAPHICS_SPECIAL_PERFORMANCE_C_PROPERTY_1800
    { "opsWitchReadPixelsBrush",
        []() -> std::shared_ptr<TestBase> {
            return std::make_shared<PerformanceCCanvasReadPixels>(DRAWING_TYPE_ATTACH_BRUSH);
        } },
    // SUB_BASIC_GRAPHICS_SPECIAL_PERFORMANCE_C_PROPERTY_1900
    { "opsWitchReadPixelsToBitmap",
        []() -> std::shared_ptr<TestBase> {
            return std::make_shared<PerformanceCCanvasReadPixelsToBitmap>(DRAWING_TYPE_ATTACH_BOTH);
        } },
    // SUB_BASIC_GRAPHICS_SPECIAL_PERFORMANCE_C_PROPERTY_1900
    { "opsWitchReadPixelsToBitmapPen",
        []() -> std::shared_ptr<TestBase> {
            return std::make_shared<PerformanceCCanvasReadPixelsToBitmap>(DRAWING_TYPE_ATTACH_PEN);
        } },
    // SUB_BASIC_GRAPHICS_SPECIAL_PERFORMANCE_C_PROPERTY_1900
    { "opsWitchReadPixelsToBitmapBrush",
        []() -> std::shared_ptr<TestBase> {
            return std::make_shared<PerformanceCCanvasReadPixelsToBitmap>(DRAWING_TYPE_ATTACH_BRUSH);
        } },
    // SUB_BASIC_GRAPHICS_SPECIAL_PERFORMANCE_C_PROPERTY_2000
    { "opsWitchLargeImageLoadDrawImageRectWithSrc",
        []() -> std::shared_ptr<TestBase> {
            return std::make_shared<PerformanceCCanvasLargeImageLoad>(
                DRAWING_TYPE_ATTACH_BOTH, LARGE_IMAGE_LOAD_FUNCTION_CANVAS_DRAW_IMAGE_RECT_WITH_SRC);
        } },
    // SUB_BASIC_GRAPHICS_SPECIAL_PERFORMANCE_C_PROPERTY_2000
    { "opsWitchLargeImageLoadDrawImageRectWithSrcPen",
        []() -> std::shared_ptr<TestBase> {
            return std::make_shared<PerformanceCCanvasLargeImageLoad>(
                DRAWING_TYPE_ATTACH_PEN, LARGE_IMAGE_LOAD_FUNCTION_CANVAS_DRAW_IMAGE_RECT_WITH_SRC);
        } },
    // SUB_BASIC_GRAPHICS_SPECIAL_PERFORMANCE_C_PROPERTY_2000
    { "opsWitchLargeImageLoadDrawImageRectWithSrcBrush",
        []() -> std::shared_ptr<TestBase> {
            return std::make_shared<PerformanceCCanvasLargeImageLoad>(
                DRAWING_TYPE_ATTACH_BRUSH, LARGE_IMAGE_LOAD_FUNCTION_CANVAS_DRAW_IMAGE_RECT_WITH_SRC);
        } },
    // SUB_BASIC_GRAPHICS_SPECIAL_PERFORMANCE_C_PROPERTY_2000
    { "opsWitchLargeImageLoadDrawImageRect",
        []() -> std::shared_ptr<TestBase> {
            return std::make_shared<PerformanceCCanvasLargeImageLoad>(
                DRAWING_TYPE_ATTACH_BOTH, LARGE_IMAGE_LOAD_FUNCTION_CANVAS_DRAW_IMAGE_RECT);
        } },
    // SUB_BASIC_GRAPHICS_SPECIAL_PERFORMANCE_C_PROPERTY_2000
    { "opsWitchLargeImageLoadDrawImageRectPen",
        []() -> std::shared_ptr<TestBase> {
            return std::make_shared<PerformanceCCanvasLargeImageLoad>(
                DRAWING_TYPE_ATTACH_PEN, LARGE_IMAGE_LOAD_FUNCTION_CANVAS_DRAW_IMAGE_RECT);
        } },
    // SUB_BASIC_GRAPHICS_SPECIAL_PERFORMANCE_C_PROPERTY_2000
    { "opsWitchLargeImageLoadDrawImageRectBrush",
        []() -> std::shared_ptr<TestBase> {
            return std::make_shared<PerformanceCCanvasLargeImageLoad>(
                DRAWING_TYPE_ATTACH_BRUSH, LARGE_IMAGE_LOAD_FUNCTION_CANVAS_DRAW_IMAGE_RECT);
        } },
    // SUB_BASIC_GRAPHICS_SPECIAL_PERFORMANCE_C_PROPERTY_2000
    { "opsWitchLargeImageLoadReadPixels",
        []() -> std::shared_ptr<TestBase> {
            return std::make_shared<PerformanceCCanvasLargeImageLoad>(
                DRAWING_TYPE_ATTACH_BOTH, LARGE_IMAGE_LOAD_FUNCTION_CANVAS_READ_PIXELS);
        } },
    // SUB_BASIC_GRAPHICS_SPECIAL_PERFORMANCE_C_PROPERTY_2000
    { "opsWitchLargeImageLoadReadPixelsPen",
        []() -> std::shared_ptr<TestBase> {
            return std::make_shared<PerformanceCCanvasLargeImageLoad>(
                DRAWING_TYPE_ATTACH_PEN, LARGE_IMAGE_LOAD_FUNCTION_CANVAS_READ_PIXELS);
        } },
    // SUB_BASIC_GRAPHICS_SPECIAL_PERFORMANCE_C_PROPERTY_2000
    { "opsWitchLargeImageLoadReadPixelsBrush",
        []() -> std::shared_ptr<TestBase> {
            return std::make_shared<PerformanceCCanvasLargeImageLoad>(
                DRAWING_TYPE_ATTACH_BRUSH, LARGE_IMAGE_LOAD_FUNCTION_CANVAS_READ_PIXELS);
        } },
    // SUB_BASIC_GRAPHICS_SPECIAL_PERFORMANCE_C_PROPERTY_1300
    { "canvas_drawshadowline",
        []() -> std::shared_ptr<TestBase> {
            return std::make_shared<PerformanceCanvasDrawShadowLine>(DRAWING_TYPE_ATTACH_BOTH);
        } },
    // SUB_BASIC_GRAPHICS_SPECIAL_PERFORMANCE_C_PROPERTY_1300
    { "canvas_drawshadowlinepen",
        []() -> std::shared_ptr<TestBase> {
            return std::make_shared<PerformanceCanvasDrawShadowLine>(DRAWING_TYPE_ATTACH_PEN);
        } },
    // SUB_BASIC_GRAPHICS_SPECIAL_PERFORMANCE_C_PROPERTY_1300
    { "canvas_drawshadowlinebrush",
        []() -> std::shared_ptr<TestBase> {
            return std::make_shared<PerformanceCanvasDrawShadowLine>(DRAWING_TYPE_ATTACH_BRUSH);
        } },
    // SUB_BASIC_GRAPHICS_SPECIAL_PERFORMANCE_C_PROPERTY_1400
    { "canvas_drawshadowcurve",
        []() -> std::shared_ptr<TestBase> {
            return std::make_shared<PerformanceCanvasDrawShadowCurve>(DRAWING_TYPE_ATTACH_BOTH);
        } },
    // SUB_BASIC_GRAPHICS_SPECIAL_PERFORMANCE_C_PROPERTY_1400
    { "canvas_drawshadowcurvepen",
        []() -> std::shared_ptr<TestBase> {
            return std::make_shared<PerformanceCanvasDrawShadowCurve>(DRAWING_TYPE_ATTACH_PEN);
        } },
    // SUB_BASIC_GRAPHICS_SPECIAL_PERFORMANCE_C_PROPERTY_1400
    { "canvas_drawshadowcurvebrush",
        []() -> std::shared_ptr<TestBase> {
            return std::make_shared<PerformanceCanvasDrawShadowCurve>(DRAWING_TYPE_ATTACH_BRUSH);
        } },
    // SUB_BASIC_GRAPHICS_SPECIAL_PERFORMANCE_C_PROPERTY_1100
    { "canvas_drawclippathline",
        []() -> std::shared_ptr<TestBase> {
            return std::make_shared<PerformanceCanvasClipPathLine>(DRAWING_TYPE_ATTACH_BOTH);
        } },
    // SUB_BASIC_GRAPHICS_SPECIAL_PERFORMANCE_C_PROPERTY_1100
    { "canvas_drawclippathlinepen",
        []() -> std::shared_ptr<TestBase> {
            return std::make_shared<PerformanceCanvasClipPathLine>(DRAWING_TYPE_ATTACH_PEN);
        } },
    // SUB_BASIC_GRAPHICS_SPECIAL_PERFORMANCE_C_PROPERTY_1100
    { "canvas_drawclippathlinebrush",
        []() -> std::shared_ptr<TestBase> {
            return std::make_shared<PerformanceCanvasClipPathLine>(DRAWING_TYPE_ATTACH_BRUSH); } },
    // SUB_BASIC_GRAPHICS_SPECIAL_PERFORMANCE_C_PROPERTY_1200
    { "canvas_drawclippathcurve",
        []() -> std::shared_ptr<TestBase> {
            return std::make_shared<PerformanceCanvasClipPathCurve>(DRAWING_TYPE_ATTACH_BOTH); } },
    // SUB_BASIC_GRAPHICS_SPECIAL_PERFORMANCE_C_PROPERTY_1200
    { "canvas_drawclippathcurvepen",
        []() -> std::shared_ptr<TestBase> {
            return std::make_shared<PerformanceCanvasClipPathCurve>(DRAWING_TYPE_ATTACH_PEN); } },
    // SUB_BASIC_GRAPHICS_SPECIAL_PERFORMANCE_C_PROPERTY_1200
    { "canvas_drawclippathcurvebrush",
        []() -> std::shared_ptr<TestBase> {
            return std::make_shared<PerformanceCanvasClipPathCurve>(DRAWING_TYPE_ATTACH_BRUSH); } },
    //SUB_BASIC_GRAPHICS_SPECIAL_PERFORMANCE_C_PROPERTY_1500
    { "canvas_drawImageRectWithSrc",
        []() -> std::shared_ptr<TestBase> {
            return std::make_shared<PerformanceCanvasDrawImageRectWithSrc>(DRAWING_TYPE_ATTACH_BOTH); } },
    { "canvas_drawImageRectWithSrcpen",
        []() -> std::shared_ptr<TestBase> {
            return std::make_shared<PerformanceCanvasDrawImageRectWithSrc>(DRAWING_TYPE_ATTACH_PEN); } },
    { "canvas_drawImageRectWithSrcbrush",
        []() -> std::shared_ptr<TestBase> {
            return std::make_shared<PerformanceCanvasDrawImageRectWithSrc>(DRAWING_TYPE_ATTACH_BRUSH); } },
    // SUB_BASIC_GRAPHICS_SPECIAL_PERFORMANCE_C_PROPERTY_1600
    { "canvas_ohdrawImageRect",
        []() -> std::shared_ptr<TestBase> {
            return std::make_shared<PerformanceCanvasDrawImageRect>(DRAWING_TYPE_ATTACH_BOTH); } },
    // SUB_BASIC_GRAPHICS_SPECIAL_PERFORMANCE_C_PROPERTY_1600
    { "canvas_ohdrawImageRectpen",
        []() -> std::shared_ptr<TestBase> {
            return std::make_shared<PerformanceCanvasDrawImageRect>(DRAWING_TYPE_ATTACH_PEN); } },
    // SUB_BASIC_GRAPHICS_SPECIAL_PERFORMANCE_C_PROPERTY_1600
    { "canvas_ohdrawImageRectbrush",
        []() -> std::shared_ptr<TestBase> {
            return std::make_shared<PerformanceCanvasDrawImageRect>(DRAWING_TYPE_ATTACH_BRUSH); } },
    // SUB_BASIC_GRAPHICS_SPECIAL_PERFORMANCE_C_PROPERTY_0900
    { "canvas_drawTextBlobLong",
        []() -> std::shared_ptr<TestBase> {
            return std::make_shared<PerformanceCanvasDrawTextBlobLong>(DRAWING_TYPE_ATTACH_BOTH); } },
    // SUB_BASIC_GRAPHICS_SPECIAL_PERFORMANCE_C_PROPERTY_0900
    { "canvas_drawTextBlobLongPen",
        []() -> std::shared_ptr<TestBase> {
            return std::make_shared<PerformanceCanvasDrawTextBlobLong>(DRAWING_TYPE_ATTACH_PEN); } },
    // SUB_BASIC_GRAPHICS_SPECIAL_PERFORMANCE_C_PROPERTY_0900
    { "canvas_drawTextBlobLongBrush",
        []() -> std::shared_ptr<TestBase> {
            return std::make_shared<PerformanceCanvasDrawTextBlobLong>(DRAWING_TYPE_ATTACH_BRUSH); } },
    // SUB_BASIC_GRAPHICS_SPECIAL_PERFORMANCE_C_PROPERTY_1000
    { "canvas_drawTextBlobMax",
        []() -> std::shared_ptr<TestBase> {
            return std::make_shared<PerformanceCanvasDrawTextBlobMax>(DRAWING_TYPE_ATTACH_BOTH); } },
    // SUB_BASIC_GRAPHICS_SPECIAL_PERFORMANCE_C_PROPERTY_1000
    { "canvas_drawTextBlobMaxPen",
        []() -> std::shared_ptr<TestBase> {
            return std::make_shared<PerformanceCanvasDrawTextBlobMax>(DRAWING_TYPE_ATTACH_PEN); } },
    // SUB_BASIC_GRAPHICS_SPECIAL_PERFORMANCE_C_PROPERTY_1000
    { "canvas_drawTextBlobMaxBrush",
        []() -> std::shared_ptr<TestBase> {
            return std::make_shared<PerformanceCanvasDrawTextBlobMax>(DRAWING_TYPE_ATTACH_BRUSH); } },
    // SUB_BASIC_GRAPHICS_SPECIAL_PERFORMANCE_C_PROPERTY_0100
    { "canvas_draw_points_performance",
        []() -> std::shared_ptr<TestBase> {
            return std::make_shared<PerformanceDrawPoints>(DRAWING_TYPE_ATTACH_BOTH); } },
    { "canvas_draw_points_performance_pen",
        []() -> std::shared_ptr<TestBase> {
            return std::make_shared<PerformanceDrawPoints>(DRAWING_TYPE_ATTACH_PEN);
        } },
    { "canvas_draw_points_performance_brush",
        []() -> std::shared_ptr<TestBase> {
            return std::make_shared<PerformanceDrawPoints>(DRAWING_TYPE_ATTACH_BRUSH);
        } },
    // SUB_BASIC_GRAPHICS_SPECIAL_PERFORMANCE_C_PROPERTY_0200
    { "canvas_draw_broken_line_performance",
        []() -> std::shared_ptr<TestBase> {
            return std::make_shared<PerformanceDrawBrokenLine>(DRAWING_TYPE_ATTACH_BOTH); } },
    { "canvas_draw_broken_line_performance_pen",
        []() -> std::shared_ptr<TestBase> {
            return std::make_shared<PerformanceDrawBrokenLine>(DRAWING_TYPE_ATTACH_PEN);
        } },
    { "canvas_draw_broken_line_performance_brush",
        []() -> std::shared_ptr<TestBase> {
            return std::make_shared<PerformanceDrawBrokenLine>(DRAWING_TYPE_ATTACH_BRUSH);
        } },
    // SUB_BASIC_GRAPHICS_SPECIAL_PERFORMANCE_C_PROPERTY_0300
    { "canvas_draw_curve_line_performance",
        []() -> std::shared_ptr<
                 TestBase> { return std::make_shared<PerformanceDrawCurveLine>(DRAWING_TYPE_ATTACH_BOTH); } },
    { "canvas_draw_curve_line_performance_pen",
        []() -> std::shared_ptr<TestBase> {
            return std::make_shared<PerformanceDrawCurveLine>(DRAWING_TYPE_ATTACH_PEN);
        } },
    { "canvas_draw_curve_line_performance_brush",
        []() -> std::shared_ptr<TestBase> {
            return std::make_shared<PerformanceDrawCurveLine>(DRAWING_TYPE_ATTACH_BRUSH);
        } },
    // SUB_BASIC_GRAPHICS_SPECIAL_PERFORMANCE_C_PROPERTY_0400
    { "canvas_draw_region_performance",
        []() -> std::shared_ptr<TestBase> {
            return std::make_shared<PerformanceDrawRegion>(DRAWING_TYPE_ATTACH_BOTH); } },
    { "canvas_draw_region_performance_pen",
        []() -> std::shared_ptr<TestBase> {
            return std::make_shared<PerformanceDrawRegion>(DRAWING_TYPE_ATTACH_PEN);
        } },
    { "canvas_draw_region_performance_brush",
        []() -> std::shared_ptr<TestBase> {
            return std::make_shared<PerformanceDrawRegion>(DRAWING_TYPE_ATTACH_BRUSH);
        } },
    // SUB_BASIC_GRAPHICS_SPECIAL_PERFORMANCE_C_PROPERTY_0500
    { "canvas_draw_bit_map_performance",
        []() -> std::shared_ptr<TestBase> {
            return std::make_shared<PerformanceDrawBitMap>(DRAWING_TYPE_ATTACH_BOTH); } },
    { "canvas_draw_bit_map_performance_pen",
        []() -> std::shared_ptr<TestBase> {
            return std::make_shared<PerformanceDrawBitMap>(DRAWING_TYPE_ATTACH_PEN);
        } },
    { "canvas_draw_bit_map_performance_brush",
        []() -> std::shared_ptr<TestBase> {
            return std::make_shared<PerformanceDrawBitMap>(DRAWING_TYPE_ATTACH_BRUSH);
        } },
    // SUB_BASIC_GRAPHICS_SPECIAL_PERFORMANCE_C_PROPERTY_0600
    { "canvas_draw_bit_map_rect_performance",
        []() -> std::shared_ptr<TestBase> {
            return std::make_shared<PerformanceDrawBitMapRect>(DRAWING_TYPE_ATTACH_BOTH); } },
    { "canvas_draw_bit_map_rect_performance_pen",
        []() -> std::shared_ptr<TestBase> {
            return std::make_shared<PerformanceDrawBitMapRect>(DRAWING_TYPE_ATTACH_PEN);
        } },
    { "canvas_draw_bit_map_rect_performance_brush",
        []() -> std::shared_ptr<TestBase> {
            return std::make_shared<PerformanceDrawBitMapRect>(DRAWING_TYPE_ATTACH_BRUSH);
        } },
    // SUB_BASIC_GRAPHICS_SPECIAL_PERFORMANCE_C_PROPERTY_0700
    { "canvas_draw_image_performance",
        []() -> std::shared_ptr<TestBase> {
            return std::make_shared<PerformanceDrawImage>(DRAWING_TYPE_ATTACH_BOTH); } },
    { "canvas_draw_image_performance_pen",
        []() -> std::shared_ptr<TestBase> {
            return std::make_shared<PerformanceDrawImage>(DRAWING_TYPE_ATTACH_PEN); } },
    { "canvas_draw_image_performance_brush",
        []() -> std::shared_ptr<TestBase> {
            return std::make_shared<PerformanceDrawImage>(DRAWING_TYPE_ATTACH_BRUSH);
        } },
    // SUB_BASIC_GRAPHICS_SPECIAL_PERFORMANCE_C_PROPERTY_0800
    { "canvas_draw_piexl_map_performance",
        []() -> std::shared_ptr<TestBase> {
            return std::make_shared<PerformanceDrawPixelMap>(DRAWING_TYPE_ATTACH_BOTH); } },
    { "canvas_draw_piexl_map_performance_pen",
        []() -> std::shared_ptr<TestBase> {
            return std::make_shared<PerformanceDrawPixelMap>(DRAWING_TYPE_ATTACH_PEN);
        } },
    { "canvas_draw_piexl_map_performance_brush",
        []() -> std::shared_ptr<TestBase> {
            return std::make_shared<PerformanceDrawPixelMap>(DRAWING_TYPE_ATTACH_BRUSH);
        } },
};

std::unordered_map<std::string, std::function<std::shared_ptr<TestBase>()>> StabilityCpuMap = {
    { "aarectmodes", []() -> std::shared_ptr<TestBase> { return std::make_shared<AARectModes>(); } },
    // SUB_BASIC_GRAPHICS_SPECIAL_MEMORY_NDK_DRAWING_TYPEFACE_0100
    { "typefaceCreateDefaultStability",
        []() -> std::shared_ptr<TestBase> { return std::make_shared<StabilityTypefaceCreateDefault>(); } },
    // SUB_BASIC_GRAPHICS_SPECIAL_MEMORY_NDK_DRAWING_TYPEFACE_0200
    { "typefaceCreateFromFileStability",
        []() -> std::shared_ptr<TestBase> { return std::make_shared<StabilityTypefaceCreateFromFile>(); } },
    // SUB_BASIC_GRAPHICS_SPECIAL_MEMORY_NDK_DRAWING_TYPEFACE_0300
    { "typefaceCreateFromStreamStability",
        []() -> std::shared_ptr<TestBase> { return std::make_shared<StabilityTypefaceCreateFromStream>(); } },
    // SUB_BASIC_GRAPHICS_SPECIAL_MEMORY_NDK_DRAWING_TEXT_BLOB_0100
    { "stabilityTextBlobBuilderCreate",
        []() -> std::shared_ptr<TestBase> { return std::make_shared<StabilityTextBlobBuilderCreate>(); } },
    // SUB_BASIC_GRAPHICS_SPECIAL_MEMORY_NDK_DRAWING_TEXT_BLOB_0200
    { "stabilityTextBlobCreateFromText",
        []() -> std::shared_ptr<TestBase> { return std::make_shared<StabilityTextBlobCreateFromText>(); } },
    // SUB_BASIC_GRAPHICS_SPECIAL_MEMORY_NDK_DRAWING_TEXT_BLOB_0400
    { "stabilityTextBlobCreateFromString",
        []() -> std::shared_ptr<TestBase> { return std::make_shared<StabilityTextBlobCreateFromString>(); } },
    // SUB_BASIC_GRAPHICS_SPECIAL_MEMORY_NDK_DRAWING_TEXT_BLOB_0300
    { "stabilityTextBlobCreateFromPosText",
        []() -> std::shared_ptr<TestBase> { return std::make_shared<StabilityTextBlobCreateFromPosText>(); } },
    // SUB_BASIC_GRAPHICS_SPECIAL_MEMORY_NDK_DRAWING_TEXT_BLOB_0500
    { "stabilityTextBlobBuilderAllocRunPos",
        []() -> std::shared_ptr<TestBase> { return std::make_shared<StabilityTextBlobBuilderAllocRunPos>(); } },
    // SUB_BASIC_GRAPHICS_SPECIAL_MEMORY_NDK_DRAWING_TEXT_BLOB_0600
    { "stabilityTextBlobRandomFunc",
        []() -> std::shared_ptr<TestBase> { return std::make_shared<StabilityTextBlobRandomFunc>(); } },
    // SUB_BASIC_GRAPHICS_SPECIAL_MEMORY_NDK_DRAWING_SURFACE_0100
    { "stabilitySurfaceCreateFromGpuContext",
        []() -> std::shared_ptr<TestBase> { return std::make_shared<StabilitySurfaceCreateFromGpuContext>(); } },
    // SUB_BASIC_GRAPHICS_SPECIAL_MEMORY_NDK_DRAWING_SURFACE_0200
    { "stabilitySurfaceFuncAll",
        []() -> std::shared_ptr<TestBase> { return std::make_shared<StabilitySurfaceFuncAll>(); } },
    // SUB_BASIC_GRAPHICS_SPECIAL_MEMORY_NDK_DRAWING_SHADOW_LAYER_0100
    { "stabilityShadowLayerCreate",
        []() -> std::shared_ptr<TestBase> { return std::make_shared<StabilityShadowLayerCreate>(); } },
    // SUB_BASIC_GRAPHICS_SPECIAL_MEMORY_NDK_DRAWING_SHADER_EFFECT_0100
    { "stabilityShaderEffectCreateColorShader",
        []() -> std::shared_ptr<TestBase> { return std::make_shared<StabilityShaderEffectCreateColorShader>(); } },
    // SUB_BASIC_GRAPHICS_SPECIAL_MEMORY_NDK_DRAWING_SHADER_EFFECT_0200
    { "stabilityShaderEffectCreateLinearGradient",
        []() -> std::shared_ptr<TestBase> { return std::make_shared<StabilityShaderEffectCreateLinearGradient>(); } },
    // SUB_BASIC_GRAPHICS_SPECIAL_MEMORY_NDK_DRAWING_SHADER_EFFECT_0300
    { "stabilityShaderEffectCreateLinearGradientWithLocalMatrix",
        []() -> std::shared_ptr<TestBase> {
            return std::make_shared<StabilityShaderEffectCreateLinearGradientWithLocalMatrix>();
        } },
    // SUB_BASIC_GRAPHICS_SPECIAL_MEMORY_NDK_DRAWING_SHADER_EFFECT_0400
    { "stabilityShaderEffectCreateRadialGradient",
        []() -> std::shared_ptr<TestBase> { return std::make_shared<StabilityShaderEffectCreateRadialGradient>(); } },
    // SUB_BASIC_GRAPHICS_SPECIAL_MEMORY_NDK_DRAWING_SHADER_EFFECT_0500
    { "stabilityShaderEffectCreateRadialGradientWithLocalMatrix",
        []() -> std::shared_ptr<TestBase> {
            return std::make_shared<StabilityShaderEffectCreateRadialGradientWithLocalMatrix>();
        } },
    // SUB_BASIC_GRAPHICS_SPECIAL_MEMORY_NDK_DRAWING_SHADER_EFFECT_0600
    { "stabilityShaderEffectCreateSweepGradient",
        []() -> std::shared_ptr<TestBase> { return std::make_shared<StabilityShaderEffectCreateSweepGradient>(); } },
    // SUB_BASIC_GRAPHICS_SPECIAL_MEMORY_NDK_DRAWING_SHADER_EFFECT_0700
    { "stabilityShaderEffectCreateImageShader",
        []() -> std::shared_ptr<TestBase> { return std::make_shared<StabilityShaderEffectCreateImageShader>(); } },
    // SUB_BASIC_GRAPHICS_SPECIAL_MEMORY_NDK_DRAWING_SHADER_EFFECT_0800
    { "stabilityShaderEffectCreateTwoPointConicalGradient",
        []() -> std::shared_ptr<TestBase> {
            return std::make_shared<StabilityShaderEffectCreateTwoPointConicalGradient>();
        } },
    // SUB_BASIC_GRAPHICS_SPECIAL_MEMORY_NDK_DRAWING_SAMPLING_OPTIONS_0100
    { "samplingOptionsCreateStability",
        []() -> std::shared_ptr<TestBase> { return std::make_shared<StabilitySamplingOptionsCreate>(); } },
    // SUB_BASIC_GRAPHICS_SPECIAL_MEMORY_NDK_DRAWING_ROUND_RECT_0100
    { "roundRectCreateStability",
        []() -> std::shared_ptr<TestBase> { return std::make_shared<StabilityRoundRectCreate>(); } },
    // SUB_BASIC_GRAPHICS_SPECIAL_MEMORY_NDK_DRAWING_ROUND_RECT_0200
    { "roundRectAllStability",
        []() -> std::shared_ptr<TestBase> { return std::make_shared<StabilityRoundRectAll>(); } },
    // SUB_BASIC_GRAPHICS_SPECIAL_MEMORY_NDK_DRAWING_ROUND_RECT_0300
    { "roundRectRandomFuncStability",
        []() -> std::shared_ptr<TestBase> { return std::make_shared<StabilityRoundRectRandomFunc>(); } },
    // SUB_BASIC_GRAPHICS_SPECIAL_MEMORY_NDK_DRAWING_REGION_0100
    { "regionCreateStability",
        []() -> std::shared_ptr<TestBase> { return std::make_shared<StabilityRegionCreate>(); } },
    // SUB_BASIC_GRAPHICS_SPECIAL_MEMORY_NDK_DRAWING_REGION_0200
    { "regionAllStability", []() -> std::shared_ptr<TestBase> { return std::make_shared<StabilityRegionAll>(); } },
    // SUB_BASIC_GRAPHICS_SPECIAL_MEMORY_NDK_DRAWING_REGION_0300
    { "regionRandomFuncStability",
        []() -> std::shared_ptr<TestBase> { return std::make_shared<StabilityRegionRandomFunc>(); } },
    // SUB_BASIC_GRAPHICS_SPECIAL_MEMORY_NDK_DRAWING_PEN_0100
    { "penCreateStability", []() -> std::shared_ptr<TestBase> { return std::make_shared<StabilityPenCreate>(); } },
    // SUB_BASIC_GRAPHICS_SPECIAL_MEMORY_NDK_DRAWING_PEN_0200
    { "penCopyStability", []() -> std::shared_ptr<TestBase> { return std::make_shared<StabilityPenCopy>(); } },
    // SUB_BASIC_GRAPHICS_SPECIAL_MEMORY_NDK_DRAWING_PEN_0300
    { "penAllFunc", []() -> std::shared_ptr<TestBase> { return std::make_shared<StabilityPenAllFunc>(); } },
    // SUB_BASIC_GRAPHICS_SPECIAL_MEMORY_NDK_DRAWING_PEN_0400
    { "penRandomFunc", []() -> std::shared_ptr<TestBase> { return std::make_shared<StabilityPenRandomFunc>(); } },
    // SUB_BASIC_GRAPHICS_SPECIAL_MEMORY_NDK_DRAWING_MASK_FILTER_0100
    { "maskFilterCreateBlurStability",
        []() -> std::shared_ptr<TestBase> { return std::make_shared<StabilityMaskFilterCreateBlur>(); } },
    // SUB_BASIC_GRAPHICS_SPECIAL_MEMORY_NDK_DRAWING_MATRIX_0100
    { "matrixCreateStability",
        []() -> std::shared_ptr<TestBase> { return std::make_shared<StabilityMatrixCreate>(); } },
    // SUB_BASIC_GRAPHICS_SPECIAL_MEMORY_NDK_DRAWING_MATRIX_0200
    { "matrixCreateRotationStability",
        []() -> std::shared_ptr<TestBase> { return std::make_shared<StabilityMatrixCreateRotation>(); } },
    // SUB_BASIC_GRAPHICS_SPECIAL_MEMORY_NDK_DRAWING_MATRIX_0300
    { "matrixCreateScaleStability",
        []() -> std::shared_ptr<TestBase> { return std::make_shared<StabilityMatrixCreateScale>(); } },
    // SUB_BASIC_GRAPHICS_SPECIAL_MEMORY_NDK_DRAWING_MATRIX_0400
    { "matrixCreateTranslationStability",
        []() -> std::shared_ptr<TestBase> { return std::make_shared<StabilityMatrixCreateTranslation>(); } },
    // SUB_BASIC_GRAPHICS_SPECIAL_MEMORY_NDK_DRAWING_MATRIX_0500
    { "matrixAllStability", []() -> std::shared_ptr<TestBase> { return std::make_shared<StabilityMatrixAll>(); } },
    // SUB_BASIC_GRAPHICS_SPECIAL_MEMORY_NDK_DRAWING_MATRIX_0600
    { "matrixRandomStability",
        []() -> std::shared_ptr<TestBase> { return std::make_shared<StabilityMatrixRandom>(); } },
    // SUB_BASIC_GRAPHICS_SPECIAL_MEMORY_NDK_DRAWING_MEMORY_STREAM_0100
    { "memoryStreamCreateStability",
        []() -> std::shared_ptr<TestBase> { return std::make_shared<StabilityMemoryStreamCreate>(); } },
    // SUB_BASIC_GRAPHICS_SPECIAL_MEMORY_NDK_DRAWING_PATH_EFFECT_0100
    { "createDashPathEffectStability",
        []() -> std::shared_ptr<TestBase> { return std::make_shared<StabilityCreateDashPathEffect>(); } },
    // SUB_BASIC_GRAPHICS_SPECIAL_MEMORY_NDK_DRAWING_PATH_0100
    { "pathCreateStability", []() -> std::shared_ptr<TestBase> { return std::make_shared<StabilityPathCreate>(); } },
    // SUB_BASIC_GRAPHICS_SPECIAL_MEMORY_NDK_DRAWING_PATH_0200
    { "pathCopyStability", []() -> std::shared_ptr<TestBase> { return std::make_shared<StabilityPathCopy>(); } },
    // SUB_BASIC_GRAPHICS_SPECIAL_MEMORY_NDK_DRAWING_PATH_0300
    { "pathAllStability", []() -> std::shared_ptr<TestBase> { return std::make_shared<StabilityPathAll>(); } },
    // SUB_BASIC_GRAPHICS_SPECIAL_MEMORY_NDK_DRAWING_PATH_0400
    { "pathRandomStability", []() -> std::shared_ptr<TestBase> { return std::make_shared<StabilityPathRandom>(); } },
    // SUB_BASIC_GRAPHICS_SPECIAL_MEMORY_NDK_DRAWING_POINT_0100
    { "pointCreateStability", []() -> std::shared_ptr<TestBase> { return std::make_shared<StabilityPointCreate>(); } },
    // SUB_BASIC_GRAPHICS_SPECIAL_MEMORY_NDK_DRAWING_RECT_0100
    { "rectCreateStability", []() -> std::shared_ptr<TestBase> { return std::make_shared<StabilityRectCreate>(); } },
    // SUB_BASIC_GRAPHICS_SPECIAL_MEMORY_NDK_DRAWING_RECT_0200
    { "rectCopyStability", []() -> std::shared_ptr<TestBase> { return std::make_shared<StabilityRectCopy>(); } },
    // SUB_BASIC_GRAPHICS_SPECIAL_MEMORY_NDK_DRAWING_RECT_0300
    { "rectAllStability", []() -> std::shared_ptr<TestBase> { return std::make_shared<StabilityRectAll>(); } },
    // SUB_BASIC_GRAPHICS_SPECIAL_MEMORY_NDK_DRAWING_RECT_0400
    { "rectRandomStability", []() -> std::shared_ptr<TestBase> { return std::make_shared<StabilityRectRandom>(); } },
    // SUB_BASIC_GRAPHICS_SPECIAL_STABLE_NDK_DRAWING_BITMAP_0100
    { "stabilityBitmapCreate",
        []() -> std::shared_ptr<TestBase> { return std::make_shared<StabilityBitmapCreate>(); } },
    // SUB_BASIC_GRAPHICS_SPECIAL_STABLE_NDK_DRAWING_BITMAP_0200
    { "stabilityBitmapCreateFromPixels",
        []() -> std::shared_ptr<TestBase> { return std::make_shared<StabilityBitmapCreateFromPixels>(); } },
    // SUB_BASIC_GRAPHICS_SPECIAL_STABLE_NDK_DRAWING_BITMAP_0300
    { "stabilityBitmapInit", []() -> std::shared_ptr<TestBase> { return std::make_shared<StabilityBitmapInit>(); } },
    // SUB_BASIC_GRAPHICS_SPECIAL_STABLE_NDK_DRAWING_BITMAP_0400
    { "stabilityBitmapRandInvoke",
        []() -> std::shared_ptr<TestBase> { return std::make_shared<StabilityBitmapRandInvoke>(); } },
    // SUB_BASIC_GRAPHICS_SPECIAL_MEMORY_NDK_DRAWING_IMAGE_0100
    { "stabilityImageCreate", []() -> std::shared_ptr<TestBase> { return std::make_shared<StabilityImageCreate>(); } },
    // SUB_BASIC_GRAPHICS_SPECIAL_MEMORY_NDK_DRAWING_IMAGE_0200
    { "stabilityImageInvoke", []() -> std::shared_ptr<TestBase> { return std::make_shared<StabilityImageInvoke>(); } },
    // SUB_BASIC_GRAPHICS_SPECIAL_MEMORY_NDK_DRAWING_IMAGE_0300
    { "stabilityImageRandInvoke",
        []() -> std::shared_ptr<TestBase> { return std::make_shared<StabilityImageRandInvoke>(); } },
    // SUB_BASIC_GRAPHICS_SPECIAL_STABLE_NDK_DRAWING_BRUSH_0100
    { "stabilityBrushCreate", []() -> std::shared_ptr<TestBase> { return std::make_shared<StabilityBrushCreate>(); } },
    // SUB_BASIC_GRAPHICS_SPECIAL_STABLE_NDK_DRAWING_BRUSH_0200
    { "stabilityBrushCopy", []() -> std::shared_ptr<TestBase> { return std::make_shared<StabilityBrushCopy>(); } },
    // SUB_BASIC_GRAPHICS_SPECIAL_STABLE_NDK_DRAWING_BRUSH_0300
    { "stabilityBrushRandInvoke",
        []() -> std::shared_ptr<TestBase> { return std::make_shared<StabilityBrushRandInvoke>(); } },
    // SUB_BASIC_GRAPHICS_SPECIAL_STABLE_NDK_DRAWING_CANVAS_0100
    { "stabilityCanvasCreate",
        []() -> std::shared_ptr<TestBase> { return std::make_shared<StabilityCanvasCreate>(); } },
    // SUB_BASIC_GRAPHICS_SPECIAL_STABLE_NDK_DRAWING_CANVAS_0200
    { "stabilityCanvasInvoke",
        []() -> std::shared_ptr<TestBase> { return std::make_shared<StabilityCanvasInvoke>(); } },
    // SUB_BASIC_GRAPHICS_SPECIAL_STABLE_NDK_DRAWING_CANVAS_0300
    { "stabilityCanvasSave", []() -> std::shared_ptr<TestBase> { return std::make_shared<StabilityCanvasSave>(); } },
    // SUB_BASIC_GRAPHICS_SPECIAL_STABLE_NDK_DRAWING_CANVAS_0400
    { "stabilityCanvasAttach",
        []() -> std::shared_ptr<TestBase> { return std::make_shared<StabilityCanvasAttach>(); } },
    // SUB_BASIC_GRAPHICS_SPECIAL_STABLE_NDK_DRAWING_CANVAS_0500
    { "stabilityCanvasRandInvoke",
        []() -> std::shared_ptr<TestBase> { return std::make_shared<StabilityCanvasRandInvoke>(); } },
    // SUB_BASIC_GRAPHICS_SPECIAL_STABLE_NDK_DRAWING_COLORFILTER_0100
    { "stabilityColorFilterCreateBlendMode",
        []() -> std::shared_ptr<TestBase> { return std::make_shared<StabilityColorFilterCreateBlendMode>(); } },
    // SUB_BASIC_GRAPHICS_SPECIAL_STABLE_NDK_DRAWING_COLORFILTER_0200
    { "stabilityColorFilterCreateCompose",
        []() -> std::shared_ptr<TestBase> { return std::make_shared<StabilityColorFilterCreateCompose>(); } },
    // SUB_BASIC_GRAPHICS_SPECIAL_STABLE_NDK_DRAWING_COLORFILTER_0300
    { "stabilityColorFilterCreateMatrix",
        []() -> std::shared_ptr<TestBase> { return std::make_shared<StabilityColorFilterCreateMatrix>(); } },
    // SUB_BASIC_GRAPHICS_SPECIAL_STABLE_NDK_DRAWING_COLORFILTER_0400
    { "stabilityColorFilterCreateLinearToSrgbGamma",
        []() -> std::shared_ptr<TestBase> { return std::make_shared<StabilityColorFilterCreateLinearToSrgbGamma>(); } },
    // SUB_BASIC_GRAPHICS_SPECIAL_STABLE_NDK_DRAWING_COLORFILTER_0500
    { "StabilityColorFilterCreateSrgbGammaToLinear",
        []() -> std::shared_ptr<TestBase> { return std::make_shared<StabilityColorFilterCreateSrgbGammaToLinear>(); } },
    { "stabilityColorFilterCreateLuma",
        []() -> std::shared_ptr<TestBase> { return std::make_shared<StabilityColorFilterCreateLuma>(); } },
    // SUB_BASIC_GRAPHICS_SPECIAL_STABLE_NDK_DRAWING_COLORSPACE_0100
    { "stabilityColorSpaceCreateSrgb",
        []() -> std::shared_ptr<TestBase> { return std::make_shared<StabilityColorSpaceCreateSrgb>(); } },
    // SUB_BASIC_GRAPHICS_SPECIAL_STABLE_NDK_DRAWING_COLORSPACE_0200
    { "StabilityColorSpaceCreateSrgbLinear",
        []() -> std::shared_ptr<TestBase> { return std::make_shared<StabilityColorSpaceCreateSrgbLinear>(); } },
    // SUB_BASIC_GRAPHICS_SPECIAL_MEMORY_NDK_DRAWING_FILTEER_0100
    { "stabilityFilterCreate",
        []() -> std::shared_ptr<TestBase> { return std::make_shared<StabilityFilterCreate>(); } },
    // SUB_BASIC_GRAPHICS_SPECIAL_MEMORY_NDK_DRAWING_FILTEER_0200
    { "stabilityFilterInvoke",
        []() -> std::shared_ptr<TestBase> { return std::make_shared<StabilityFilterInvoke>(); } },
    // SUB_BASIC_GRAPHICS_SPECIAL_MEMORY_NDK_DRAWING_FILTEER_0300
    { "stabilityFilterRandInvoke",
        []() -> std::shared_ptr<TestBase> { return std::make_shared<StabilityFilterRandInvoke>(); } },
    // SUB_BASIC_GRAPHICS_SPECIAL_MEMORY_NDK_DRAWING_IMAGE_FILTER_0100
    { "stabilityImageFilterCreateBlur",
        []() -> std::shared_ptr<TestBase> { return std::make_shared<StabilityImageFilterCreateBlur>(); } },
    // SUB_BASIC_GRAPHICS_SPECIAL_MEMORY_NDK_DRAWING_IMAGE_FILTER_0200
    { "stabilityImageFilterCreateFromColorFilter",
        []() -> std::shared_ptr<TestBase> { return std::make_shared<StabilityImageFilterCreateFromColorFilter>(); } },
    // SUB_BASIC_GRAPHICS_SPECIAL_MEMORY_NDK_DRAWING_GPU_CONTEXT_0100
    { "stabilityGpuContextCreate",
        []() -> std::shared_ptr<TestBase> { return std::make_shared<StabilityGpuContextCreate>(); } },
};
} // namespace

std::shared_ptr<TestBase> TestCaseFactory::GetFunctionCpuCase(std::string caseName)
{
    if (FunctionalCpuMap.find(caseName) == FunctionalCpuMap.end()) {
        DRAWING_LOGE("caseName error");
        return nullptr;
    }

    return FunctionalCpuMap.find(caseName)->second();
}

std::shared_ptr<TestBase> TestCaseFactory::GetPerformanceCpuCase(std::string caseName)
{
    if (PerformanceCpuMap.find(caseName) == PerformanceCpuMap.end()) {
        DRAWING_LOGE("caseName error");
        return nullptr;
    }

    return PerformanceCpuMap.find(caseName)->second();
}

std::shared_ptr<TestBase> TestCaseFactory::GetStabilityCpuCase(std::string caseName)
{
    if (StabilityCpuMap.find(caseName) == StabilityCpuMap.end()) {
        DRAWING_LOGE("caseName error");
        return nullptr;
    }

    return StabilityCpuMap.find(caseName)->second();
}

std::unordered_map<std::string, std::function<std::shared_ptr<TestBase>()>> TestCaseFactory::GetFunctionCpuCaseAll()
{
    return FunctionalCpuMap;
}

std::unordered_map<std::string, std::function<std::shared_ptr<TestBase>()>> TestCaseFactory::GetStabilityCpuCaseAll()
{
    return StabilityCpuMap;
}
