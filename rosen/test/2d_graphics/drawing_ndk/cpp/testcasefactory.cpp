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

#include "common/log_common.h"

namespace {
std::unordered_map<std::string, std::function<std::shared_ptr<TestBase>()>> FunctionalCpuMap = {
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
    { "pointsmaskfilter",
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
    { "anisotropic_hq",
        []() -> std::shared_ptr<TestBase> {
            return std::make_shared<Anisotropic>();
        } }, // 该用例OH_Drawing_SamplingOptionsCreate接口mode对应内容未开放,无法实现
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
    { "brushrest",
        []() -> std::shared_ptr<TestBase> { return std::make_shared<BrushReset>(TestBase::DRAW_STYLE_COMPLEX); } },
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

std::unordered_map<std::string, std::function<std::shared_ptr<TestBase>()>> TestCaseFactory::GetFunctionCpuCaseAll()
{
    return FunctionalCpuMap;
}
