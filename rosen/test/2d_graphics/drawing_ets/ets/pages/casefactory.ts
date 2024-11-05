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

import {TestBase, StyleType} from './testbase';
import { CanvasDrawRect, CanvasDrawLine, CanvasDrawPath, CanvasDrawPoint, CanvasDrawImage, CanvasDrawCircle, CanvasDrawTextBlob,
  CanvasDrawPixelMapMesh, CanvasDrawColor, CanvasSetColor, CanvasSetMatrix, CanvasResetMatrix, CanvasClipRoundRect, CanvasIsClipEmpty,
  FontIsSubpixel, FontIsLinearMetrics, FontIsEmbolden, FontGetSkewX, FontGetScaleX, FontGetHinting, FontGetEdging,
  BrushGetColorFilter, BrushSetImageFilter, PenGetColorFilter, PenSetImageFilter,
  CreateBlurImageFilter, CreateColorImageFilter, CanvasDrawImageRect, CanvasDrawImageRectWithSrc, CanvasClipRegion,CanvasClear,
  CanvasDrawShadow,CanvasCreateLattice} from '../testcase/interface/canvastest';
import { PathLineTo, PathArcTo, PathQuadTo, PathCubicTo,PathClose, PathReset, PathIsClosed, PathGetPositionAndTangent, PathGetMatrix, PathBuildFromSvgString, PathConstructor } from '../testcase/interface/pathtest';
import { MatrixGetValue, MatrixPostRotate, MatrixPostTranslate, MatrixReset, MatrixGetAll, MatrixSetPolyToPoly, MatrixSetRectToRect, MatrixPreScale, MatrixPreTranslate, MatrixPreRotate, MatrixPostScale, MatrixMapPoints, MatrixMapRect } from '../testcase/interface/matrixtest';
import { MakeFromRunBuffer, MakeFromString, TextBlobBounds, MakeFromPosText, MakeUniqueId} from '../testcase/interface/textblobtest';
import {PerformanceCanvasDrawRect, PerformanceCanvasDrawCircle, PerformanceCanvasDrawLine, PerformanceCanvasDrawTextBlob, PerformanceCanvasDrawPath,
  PerformanceCanvasPoint} from '../testcase/interface/performancetest';
import { RoundRectSetCornerTest, RoundRectGetCornerTest, RoundRectOffsetTest } from '../testcase/interface/roundrecttest';
import { PenGetFillPath } from '../testcase/interface/pentest'
import { PenBrushGetHexColor } from '../testcase/interface/gethexcolortest'
import { ColorFilterCreateMatrix } from '../testcase/interface/colorfiltertests';
import { PerformanceBrushSetColor,PerformanceBrushGetColor,PerformanceBrushSetAntiAlias,PerformanceBrushSetAlpha,PerformanceBrushGetAlpha,
  PerformanceBrushSetColorFilter, PerformanceBrushSetMaskFilter, PerformanceBrushSetImageFilter, PerformanceBrushSetShadowLayer,
  PerformanceBrushSetBlendMode, PerformanceBrushReset, PerformanceBrushGetColorFilter, PerformanceBrushSetShaderEffect,
  PerformanceBrushIsAntiAlias, PerformanceBrushConstructor } from '../testcase/interface/performancebrushtest';
import { PerformanceColorFilterCreateBlendModeColorFilter, PerformanceColorFilterCreateComposeColorFilter,
  PerformanceColorFilterCreateLinearToSRGBGamma, PerformanceColorFilterCreateLumaColorFilter, PerformanceColorFilterCreateMatrixColorFilter,
  PerformanceColorFilterCreateSRGBGammaToLinear } from '../testcase/interface/performancecolorfiltertest';
import { PerformanceFontCountText, PerformanceFontEnableEmbolden, PerformanceFontEnableLinearMetrics, PerformanceFontEnableSubpixel,
  PerformanceFontGetEdging, PerformanceFontGetHinting, PerformanceFontGetMetrics, PerformanceFontGetScaleX, PerformanceFontGetSkewX,
  PerformanceFontGetWidths, PerformanceFontIsBaselineSnap, PerformanceFontIsEmbeddedBitmaps, PerformanceFontIsEmbolden,
  PerformanceFontIsForceAutoHinting, PerformanceFontIsLinearMetrics, PerformanceFontIsSubpixel, PerformanceFontMeasureSingleCharacter,
  PerformanceFontMeasureText, PerformanceFontSetBaselineSnap, PerformanceFontSetEdging, PerformanceFontSetEmbeddedBitmaps,
  PerformanceFontSetForceAutoHinting, PerformanceFontSetHinting, PerformanceFontSetScaleX, PerformanceFontSetSize, PerformanceFontSetSkewX,
  PerformanceFontTextToGlyphs, PerformanceFontGetSize, PerformanceFontGetTypeface, PerformanceFontSetTypeface } from '../testcase/interface/performancefonttest';
import { PerformanceTypeFaceCreateBlurMaskFilter } from '../testcase/interface/performancemaskfiltertest';
import { PerformancePathEffectCreateCornerPathEffect, PerformancePathEffectCreateDashPathEffect } from '../testcase/interface/performancepatheffecttest';
import { PerformanceTextBlobMakeFromRunBuffer, PerformanceTextBlobMakeFromString } from '../testcase/interface/performancetextblobtest';
import { PerformanceTypeFaceGetFamilyName, PerformanceTypeFaceMakeFromFile } from '../testcase/interface/performancetypefacetest';
import { PerformanceRegionIsPointContained, PerformanceRegionIsRegionContained,
  PerformanceRegionOp, PerformanceRegionQuickReject, PerformanceRegionSetPath,
  PerformanceRegionSetRect } from '../testcase/interface/performanceregiontest';
import { PerformancePenGetAlpha, PerformancePenGetCapStyle, PerformancePenGetColor,
  PerformancePenGetColorFilter, PerformancePenGetFillPath, PerformancePenGetJoinStyler,
  PerformancePenGetMiterLimit, PerformancePenGetWidth, PerformancePenIsAntiAlias,
  PerformancePenReset, PerformancePenSetAlpha, PerformancePenSetAntiAlias,
  PerformancePenSetBlendMode, PerformancePenSetCapStyle, PerformancePenSetColor,
  PerformancePenSetColorFilter, PerformancePenSetDither, PerformancePenSetImageFilter,
  PerformancePenSetJoinStyler, PerformancePenSetMaskFilter, PerformancePenSetMiterLimit,
  PerformancePenSetPathEffect, PerformancePenSetShaderEffect, PerformancePenSetShadowLayer,
  PerformancePenSetStrokeWidth } from '../testcase/interface/performancepentest';
import { PerformanceShadowLayerCreate } from '../testcase/interface/performanceshadowlayertest';

const TAG = '[DrawingTest]';

export class CaseFactory {
  static FunctionMap: Map<string, Function> = new Map(
    [
      ['canvasdrawshadow', () => {return new CanvasDrawShadow();}],
      ['canvascreatelattice', () => {return new CanvasCreateLattice();}],
      ['canvascliproundrect', () => { return new CanvasClipRoundRect(); }],
      ['canvasclear', () => { return new CanvasClear(); }],
      ['canvasdrawrect', () => { return new CanvasDrawRect(); }],
      ['canvasdrawcircle', () => { return new CanvasDrawCircle(); }],
      ['canvasdrawcolor', () => { return new CanvasDrawColor(); }],
      ['canvasdrawpoint', () => { return new CanvasDrawPoint(); }],
      ['canvasdrawpath', () => { return new CanvasDrawPath(); }],
      ['canvasdrawline', () => { return new CanvasDrawLine(); }],
      ['canvasdrawtextblob', () => { return new CanvasDrawTextBlob(); }],
      ['canvasdrawimage', () => { return new CanvasDrawImage(); }],
      ['canvassetcolor', () => { return new CanvasSetColor(); }],
      ['canvasdrawimagerect', () => { return new CanvasDrawImageRect(); }],
      ['canvasdrawimagerectwithsrc', () => { return new CanvasDrawImageRectWithSrc(); }],
      ['canvasdrawsubpixelfont', () => { return new FontIsSubpixel(); }],
      ['canvasdrawlinearmetricsfont', () => { return new FontIsLinearMetrics(); }],
      ['canvasdrawemboldenfont', () => { return new FontIsEmbolden(); }],
      ['canvasdrawskewxfont', () => { return new FontGetSkewX(); }],
      ['canvasdrawscalexfont', () => { return new FontGetScaleX(); }],
      ['canvasdrawhintingfont', () => { return new FontGetHinting(); }],
      ['canvasdrawedgingfont', () => { return new FontGetEdging(); }],
      ['canvassetmatrix', () => { return new CanvasSetMatrix(); }],
      ['canvasresetmatrix', () => { return new CanvasResetMatrix(); }],
      ['canvasisclipempty', () => { return new CanvasIsClipEmpty(); }],
      ['canvasclipregion', () => { return new CanvasClipRegion(); }],
      ['textblob_createfrom_string', () => { return new MakeFromString(StyleType.DRAW_STYLE_COMPLEX); }],
      ['brushgetcolorfilter', () => { return new BrushGetColorFilter(); }],
      ['brushsetimagefilter', () => { return new BrushSetImageFilter(); }],
      ['pengetcolorfilter', () => { return new PenGetColorFilter(); }],
      ['pensetimagefilter', () => { return new PenSetImageFilter(); }],
      ['createblurimagefilter', () => { return new CreateBlurImageFilter(); }],
      ['createcolorimagefilter', () => { return new CreateColorImageFilter(); }],

      ['textblob_createfrom_postext', () => { return new MakeFromPosText(StyleType.DRAW_STYLE_COMPLEX); }],

      ['roundrectsetcorner', () => { return new RoundRectSetCornerTest(); }],
      ['roundrectgetcorner', () => { return new RoundRectGetCornerTest(); }],
      ['roundrectoffset', () => { return new RoundRectOffsetTest(); }],

      ['pathisclosed', () => { return new PathIsClosed(StyleType.DRAW_STYLE_COMPLEX); }],
      ['pathgetPositionAndTangent', () => { return new PathGetPositionAndTangent(StyleType.DRAW_STYLE_COMPLEX); }],
      ['pathgetmatrix', () => { return new PathGetMatrix(StyleType.DRAW_STYLE_COMPLEX); }],
      ['pathbuildfromsvgstring', () => { return new PathBuildFromSvgString(StyleType.DRAW_STYLE_COMPLEX); }],
      ['pathconstructor', () => { return new PathConstructor(); }],

      ['matrixgetvalue', () => { return new MatrixGetValue(); }],
      ['matrixgetall', () => { return new MatrixGetAll(); }],
      ['matrixpostrotate', () => { return new MatrixPostRotate(); }],
      ['matrixposttranslate', () => { return new MatrixPostTranslate(); }],
      ['matrixreset', () => { return new MatrixReset(); }],
      ['matrix_setpolytopoly', () => { return new MatrixSetPolyToPoly(); }],
      ['matrix_setrecttorect', () => { return new MatrixSetRectToRect(); }],
      ['matrix_prescale', () => { return new MatrixPreScale(); }],
      ['matrix_pretranslate', () => { return new MatrixPreTranslate(); }],
      ['matrix_prerotate', () => { return new MatrixPreRotate(); }],
      ['matrix_postscale', () => { return new MatrixPostScale(); }],
      ['textblob_unique_id', () => { return new MakeUniqueId(); }],
      ['textblob_createfrom_string', () => { return new MakeFromString(StyleType.DRAW_STYLE_COMPLEX); }],
      ['pengetfillpath', () => { return new PenGetFillPath(); }],
      ['penbrushgethexcolor', ()=> { return new PenBrushGetHexColor(); }],
      ['matrix_maprect', () => { return new MatrixMapRect(); }],
      ['matrixmappoints', () => { return new MatrixMapPoints(); }],
      ['colorfilter_creatematrix', () => { return new ColorFilterCreateMatrix(); }],
    ]
  );
  static PerformanceMap: Map<string, Function> = new Map(
    [
      ['canvasdrawshadow', () => {return new CanvasDrawShadow();}],
      ['canvascreatelattice', () => {return new CanvasCreateLattice();}],
      ['canvas_cliproundrect', () => { return new CanvasClipRoundRect(); }],
      ['canvasclear', () => { return new CanvasClear(); }],// 1000次耗时378ms
      ['canvas_drawrect', () => { return new CanvasDrawRect(StyleType.DRAW_STYLE_COMPLEX); }], // 1000次耗时109ms
      ['canvas_drawline', () => { return new CanvasDrawLine(StyleType.DRAW_STYLE_COMPLEX); }], // 1000次耗时347ms
      ['canvas_drawpath', () => { return new CanvasDrawPath(StyleType.DRAW_STYLE_COMPLEX); }], // 100次耗时506ms
      ['canvas_drawpoints', () => { return new CanvasDrawPoint(StyleType.DRAW_STYLE_COMPLEX); }], // 1000次耗时47ms
      ['canvas_drawbitmap', () => { return new CanvasDrawImage(StyleType.DRAW_STYLE_COMPLEX); }], // 1000次耗时2ms
      ['canvas_drawimagerect', () => { return new CanvasDrawImageRect(StyleType.DRAW_STYLE_COMPLEX); }],
      ['canvasdrawimagerectwithsrc', () => { return new CanvasDrawImageRectWithSrc(StyleType.DRAW_STYLE_COMPLEX); }],
      ['canvas_drawcircle', () => { return new CanvasDrawCircle(StyleType.DRAW_STYLE_COMPLEX); }], // 1000次耗时1016ms
      ['canvas_drawtextblob', () => { return new CanvasDrawTextBlob(StyleType.DRAW_STYLE_COMPLEX); }], // 1000次耗时125ms
      ['canvas_drawvertices', () => { return new CanvasDrawPixelMapMesh(StyleType.DRAW_STYLE_COMPLEX); }], // 1000次耗时261ms
      ['canvas_clipregion', () => { return new CanvasClipRegion(); }],
      ['canvas_isclipempty', () => { return new CanvasIsClipEmpty(); }],
      ['canvas_setmatrix', () => { return new CanvasSetMatrix(); }],
      ['canvas_resetmatrix', () => { return new CanvasResetMatrix(); }],
      ['path_lineto', () => { return new PathLineTo(StyleType.DRAW_STYLE_COMPLEX); }], // 1000次耗时70ms
      ['path_arcto', () => { return new PathArcTo(StyleType.DRAW_STYLE_COMPLEX); }], // 1000次耗时42ms
      ['path_quadto', () => { return new PathQuadTo(StyleType.DRAW_STYLE_COMPLEX); }], // 1000次耗时156ms
      ['path_cubicto', () => { return new PathCubicTo(StyleType.DRAW_STYLE_COMPLEX); }], // 1000次耗时174ms
      ['path_close', () => { return new PathClose(StyleType.DRAW_STYLE_COMPLEX); }], // 1000次耗时625ms
      ['path_reset', () => { return new PathReset(StyleType.DRAW_STYLE_COMPLEX); }], // 1000次耗时816ms
      ['path_isClosed', () => { return new PathIsClosed(StyleType.DRAW_STYLE_COMPLEX); }],
      ['path_getPositionAndTangent', () => { return new PathGetPositionAndTangent(StyleType.DRAW_STYLE_COMPLEX); }],
      ['path_getMatrix', () => { return new PathGetMatrix(StyleType.DRAW_STYLE_COMPLEX); }],
      ['path_buildFromSVGString', () => { return new PathBuildFromSvgString(StyleType.DRAW_STYLE_COMPLEX); }],
      ['path_constructor', () => { return new PathConstructor(); }],
      ['textblob_createbuilder', () => { return new MakeFromRunBuffer(StyleType.DRAW_STYLE_COMPLEX); }], // 1000次耗时339ms
      ['textblob_createfrom_string', () => { return new MakeFromString(StyleType.DRAW_STYLE_COMPLEX); }], // 1000次耗时261ms
      ['textblob_getbounds', () => { return new TextBlobBounds(StyleType.DRAW_STYLE_COMPLEX); }], // 1000次耗时365ms
      ['textblob_createfrom_postext', () => { return new MakeFromPosText(StyleType.DRAW_STYLE_COMPLEX); }], // 1000次耗时271ms
      ['textblob_unique_id', () => { return new MakeUniqueId(); }],
      ['performance_pengetfillpath', () => { return new PenGetFillPath(); }],
      ['penbrushgethexcolor', () => { return new PenBrushGetHexColor(); }],
      ['font_isfakeboldtext', () => { return new FontIsEmbolden(StyleType.DRAW_STYLE_COMPLEX); }],
      ['font_gettextskewx', () => { return new FontGetSkewX(StyleType.DRAW_STYLE_COMPLEX); }],
      ['font_getscalex', () => { return new FontGetScaleX(StyleType.DRAW_STYLE_COMPLEX); }],
      ['font_gethinting', () => { return new FontGetHinting(StyleType.DRAW_STYLE_COMPLEX); }],
      ['font_getedging', () => { return new FontGetEdging(StyleType.DRAW_STYLE_COMPLEX); }],

      ['font_issubpixel', () => { return new FontIsSubpixel(StyleType.DRAW_STYLE_COMPLEX); }],
      ['font_islinearmetrics', () => { return new FontIsLinearMetrics(StyleType.DRAW_STYLE_COMPLEX); }],
      ['font_isfakeboldtext', () => { return new FontIsEmbolden(StyleType.DRAW_STYLE_COMPLEX); }],
      ['font_gettextskewx', () => { return new FontGetSkewX(StyleType.DRAW_STYLE_COMPLEX); }],
      ['font_getscalex', () => { return new FontGetScaleX(StyleType.DRAW_STYLE_COMPLEX); }],
      ['font_gethinting', () => { return new FontGetHinting(StyleType.DRAW_STYLE_COMPLEX); }],
      ['font_getedging', () => { return new FontGetEdging(StyleType.DRAW_STYLE_COMPLEX); }],
      ['performance_canvas_drawRect', () => { return new PerformanceCanvasDrawRect(StyleType.DRAW_STYLE_PERFORMANCE_TEST); }],
      ['performance_canvas_drawCircle', () => { return new PerformanceCanvasDrawCircle(StyleType.DRAW_STYLE_PERFORMANCE_TEST); }],
      ['performance_canvas_drawLine', () => { return new PerformanceCanvasDrawLine(StyleType.DRAW_STYLE_PERFORMANCE_TEST); }],
      ['performance_canvas_drawTextBlob', () => { return new PerformanceCanvasDrawTextBlob(StyleType.DRAW_STYLE_PERFORMANCE_TEST); }],
      ['performance_canvas_drawPoint', () => { return new PerformanceCanvasPoint(StyleType.DRAW_STYLE_PERFORMANCE_TEST); }],
      ['performance_canvas_drawPath', () => { return new PerformanceCanvasDrawPath(StyleType.DRAW_STYLE_PERFORMANCE_TEST); }],
      ['brush_get_color_filter', () => { return new BrushGetColorFilter(StyleType.DRAW_STYLE_COMPLEX); }],
      ['brush_set_image_filter', () => { return new BrushSetImageFilter(StyleType.DRAW_STYLE_COMPLEX); }],
      ['pen_get_color_filter', () => { return new PenGetColorFilter(StyleType.DRAW_STYLE_COMPLEX); }],
      ['pen_set_image_filter', () => { return new PenSetImageFilter(StyleType.DRAW_STYLE_COMPLEX); }],
      ['create_blur_image_filter', () => { return new CreateBlurImageFilter(StyleType.DRAW_STYLE_COMPLEX); }],
      ['create_color_image_filter', () => { return new CreateColorImageFilter(StyleType.DRAW_STYLE_COMPLEX); }],

      ['roundrect_setcorner', () => { return new RoundRectSetCornerTest(); }],
      ['roundrect_getcorner', () => { return new RoundRectGetCornerTest(); }],
      ['roundrect_offset', () => { return new RoundRectOffsetTest(); }],

      ['matrix_getvalue', () => { return new MatrixGetValue(); }],
      ['matrix_getall', () => { return new MatrixGetAll(); }],
      ['matrix_postrotate', () => { return new MatrixPostRotate(); }],
      ['matrix_posttranslate', () => { return new MatrixPostTranslate(); }],
      ['matrix_reset', () => { return new MatrixReset(); }],
      ['matrix_setpolytopoly', () => { return new MatrixSetPolyToPoly(); }],
      ['matrix_setrecttorect', () => { return new MatrixSetRectToRect(); }],
      ['matrix_prescale', () => { return new MatrixPreScale(); }],
      ['matrix_pretranslate', () => { return new MatrixPreTranslate(); }],
      ['matrix_prerotate', () => { return new MatrixPreRotate(); }],
      ['matrix_postscale', () => { return new MatrixPostScale(); }],
      ['matrix_maprect', () => { return new MatrixMapRect(); }],
      ['matrix_mappoints', () => { return new MatrixMapPoints(); }],
      ['color_filter_create_matrix', () => { return new ColorFilterCreateMatrix(); }],
      ['performance_colorFilter_createMatrixColorFilter', () => { return new PerformanceColorFilterCreateMatrixColorFilter(StyleType.DRAW_STYLE_PERFORMANCE_TEST); }],
      ['performance_brush_constructor', () => { return new PerformanceBrushConstructor(StyleType.DRAW_STYLE_PERFORMANCE_TEST); }],
      ['performance_brush_setColor', () => { return new PerformanceBrushSetColor(StyleType.DRAW_STYLE_PERFORMANCE_TEST); }],
      ['performance_brush_getColor', () => { return new PerformanceBrushGetColor(StyleType.DRAW_STYLE_PERFORMANCE_TEST); }],
      ['performance_brush_setAntiAlias', () => { return new PerformanceBrushSetAntiAlias(StyleType.DRAW_STYLE_PERFORMANCE_TEST); }],
      ['performance_brush_setAlpha', () => { return new PerformanceBrushSetAlpha(StyleType.DRAW_STYLE_PERFORMANCE_TEST); }],
      ['performance_brush_getAlpha', () => { return new PerformanceBrushGetAlpha(StyleType.DRAW_STYLE_PERFORMANCE_TEST); }],
      ['performance_brush_setColorFilter', () => { return new PerformanceBrushSetColorFilter(StyleType.DRAW_STYLE_PERFORMANCE_TEST); }],
      ['performance_brush_getColorFilter', () => { return new PerformanceBrushGetColorFilter(StyleType.DRAW_STYLE_PERFORMANCE_TEST); }],
      ['performance_brush_setMaskFilter', () => { return new PerformanceBrushSetMaskFilter(StyleType.DRAW_STYLE_PERFORMANCE_TEST); }],
      ['performance_brush_setImageFilter', () => { return new PerformanceBrushSetImageFilter(StyleType.DRAW_STYLE_PERFORMANCE_TEST); }],
      ['performance_brush_setShadowLayer', () => { return new PerformanceBrushSetShadowLayer(StyleType.DRAW_STYLE_PERFORMANCE_TEST); }],
      ['performance_brush_setShaderEffect', () => { return new PerformanceBrushSetShaderEffect(StyleType.DRAW_STYLE_PERFORMANCE_TEST); }],
      ['performance_brush_setBlendMode', () => { return new PerformanceBrushSetBlendMode(StyleType.DRAW_STYLE_PERFORMANCE_TEST); }],
      ['performance_brush_isAntiAlias', () => { return new PerformanceBrushIsAntiAlias(StyleType.DRAW_STYLE_PERFORMANCE_TEST); }],
      ['performance_brush_reset', () => { return new PerformanceBrushReset(StyleType.DRAW_STYLE_PERFORMANCE_TEST); }],
      ['performance_colorFilter_createBlendModeColorFilter', () => { return new PerformanceColorFilterCreateBlendModeColorFilter(StyleType.DRAW_STYLE_PERFORMANCE_TEST); }],
      ['performance_colorFilter_createComposeColorFilter', () => { return new PerformanceColorFilterCreateComposeColorFilter(StyleType.DRAW_STYLE_PERFORMANCE_TEST); }],
      ['performance_colorFilter_createLinearToSRGBGamma', () => { return new PerformanceColorFilterCreateLinearToSRGBGamma(StyleType.DRAW_STYLE_PERFORMANCE_TEST); }],
      ['performance_colorFilter_createSRGBGammaToLinear', () => { return new PerformanceColorFilterCreateSRGBGammaToLinear(StyleType.DRAW_STYLE_PERFORMANCE_TEST); }],
      ['performance_colorFilter_createLumaColorFilter', () => { return new PerformanceColorFilterCreateLumaColorFilter(StyleType.DRAW_STYLE_PERFORMANCE_TEST); }],
      ['performance_font_enableSubpixel', () => { return new PerformanceFontEnableSubpixel(StyleType.DRAW_STYLE_PERFORMANCE_TEST); }],
      ['performance_font_enableEmbolden', () => { return new PerformanceFontEnableEmbolden(StyleType.DRAW_STYLE_PERFORMANCE_TEST); }],
      ['performance_font_enableLinearMetrics', () => { return new PerformanceFontEnableLinearMetrics(StyleType.DRAW_STYLE_PERFORMANCE_TEST); }],
      ['performance_font_setBaselineSnap', () => { return new PerformanceFontSetBaselineSnap(StyleType.DRAW_STYLE_PERFORMANCE_TEST); }],
      ['performance_font_setEmbeddedBitmaps', () => { return new PerformanceFontSetEmbeddedBitmaps(StyleType.DRAW_STYLE_PERFORMANCE_TEST); }],
      ['performance_font_setForceAutoHinting', () => { return new PerformanceFontSetForceAutoHinting(StyleType.DRAW_STYLE_PERFORMANCE_TEST); }],
      ['performance_font_setSize', () => { return new PerformanceFontSetSize(StyleType.DRAW_STYLE_PERFORMANCE_TEST); }],
      ['performance_font_setTypeface', () => { return new PerformanceFontSetTypeface(StyleType.DRAW_STYLE_PERFORMANCE_TEST); }],
      ['performance_font_getTypeface', () => { return new PerformanceFontGetTypeface(StyleType.DRAW_STYLE_PERFORMANCE_TEST); }],
      ['performance_font_getSize', () => { return new PerformanceFontGetSize(StyleType.DRAW_STYLE_PERFORMANCE_TEST); }],
      ['performance_font_getMetrics', () => { return new PerformanceFontGetMetrics(StyleType.DRAW_STYLE_PERFORMANCE_TEST); }],
      ['performance_font_getWidths', () => { return new PerformanceFontGetWidths(StyleType.DRAW_STYLE_PERFORMANCE_TEST); }],
      ['performance_font_isBaselineSnap', () => { return new PerformanceFontIsBaselineSnap(StyleType.DRAW_STYLE_PERFORMANCE_TEST); }],
      ['performance_font_isEmbeddedBitmaps', () => { return new PerformanceFontIsEmbeddedBitmaps(StyleType.DRAW_STYLE_PERFORMANCE_TEST); }],
      ['performance_font_isForceAutoHinting', () => { return new PerformanceFontIsForceAutoHinting(StyleType.DRAW_STYLE_PERFORMANCE_TEST); }],
      ['performance_font_measureSingleCharacter', () => { return new PerformanceFontMeasureSingleCharacter(StyleType.DRAW_STYLE_PERFORMANCE_TEST); }],
      ['performance_font_measureText', () => { return new PerformanceFontMeasureText(StyleType.DRAW_STYLE_PERFORMANCE_TEST); }],
      ['performance_font_setScaleX', () => { return new PerformanceFontSetScaleX(StyleType.DRAW_STYLE_PERFORMANCE_TEST); }],
      ['performance_font_setSkewX', () => { return new PerformanceFontSetSkewX(StyleType.DRAW_STYLE_PERFORMANCE_TEST); }],
      ['performance_font_setEdging', () => { return new PerformanceFontSetEdging(StyleType.DRAW_STYLE_PERFORMANCE_TEST); }],
      ['performance_font_setHinting', () => { return new PerformanceFontSetHinting(StyleType.DRAW_STYLE_PERFORMANCE_TEST); }],
      ['performance_font_countText', () => { return new PerformanceFontCountText(StyleType.DRAW_STYLE_PERFORMANCE_TEST); }],
      ['performance_font_isSubpixel', () => { return new PerformanceFontIsSubpixel(StyleType.DRAW_STYLE_PERFORMANCE_TEST); }],
      ['performance_font_isLinearMetrics', () => { return new PerformanceFontIsLinearMetrics(StyleType.DRAW_STYLE_PERFORMANCE_TEST); }],
      ['performance_font_getSkewX', () => { return new PerformanceFontGetSkewX(StyleType.DRAW_STYLE_PERFORMANCE_TEST); }],
      ['performance_font_fontIsEmbolden', () => { return new PerformanceFontIsEmbolden(StyleType.DRAW_STYLE_PERFORMANCE_TEST); }],
      ['performance_font_getScaleX', () => { return new PerformanceFontGetScaleX(StyleType.DRAW_STYLE_PERFORMANCE_TEST); }],
      ['performance_font_getHinting', () => { return new PerformanceFontGetHinting(StyleType.DRAW_STYLE_PERFORMANCE_TEST); }],
      ['performance_font_getEdging', () => { return new PerformanceFontGetEdging(StyleType.DRAW_STYLE_PERFORMANCE_TEST); }],
      ['performance_font_textToGlyphs', () => { return new PerformanceFontTextToGlyphs(StyleType.DRAW_STYLE_PERFORMANCE_TEST); }],
      ['performance_maskfilter_createblurmaskfilter', () => { return new PerformanceTypeFaceCreateBlurMaskFilter(); }],
      ['performance_patheffect_createdashpatheffect', () => { return new PerformancePathEffectCreateDashPathEffect(); }],
      ['performance_patheffect_createcornerpatheffect', () => { return new PerformancePathEffectCreateCornerPathEffect(); }],
      ['performance_textblob_makefromstring', () => { return new PerformanceTextBlobMakeFromString(); }],
      ['performance_textblob_makefromrunbuffer', () => { return new PerformanceTextBlobMakeFromRunBuffer(); }],
      ['performance_textblob_makefrompostext', () => { return new PerformanceTextBlobMakeFromRunBuffer(); }],
      ['performance_typeface_getfamilyname', () => { return new PerformanceTypeFaceGetFamilyName(); }],
      ['performance_typeface_makefromfile', () => { return new PerformanceTypeFaceMakeFromFile(); }],
      ['performance_shadowLayer_create', () => { return new PerformanceShadowLayerCreate(StyleType.DRAW_STYLE_PERFORMANCE_TEST); }],
      ['performance_pen_setColor', () => { return new PerformancePenSetColor(StyleType.DRAW_STYLE_PERFORMANCE_TEST); }],
      ['performance_pen_getColor', () => { return new PerformancePenGetColor(StyleType.DRAW_STYLE_PERFORMANCE_TEST); }],
      ['performance_pen_setStrokeWidth', () => { return new PerformancePenSetStrokeWidth(StyleType.DRAW_STYLE_PERFORMANCE_TEST); }],
      ['performance_pen_setAntiAlias', () => { return new PerformancePenSetAntiAlias(StyleType.DRAW_STYLE_PERFORMANCE_TEST); }],
      ['performance_pen_setAlpha', () => { return new PerformancePenSetAlpha(StyleType.DRAW_STYLE_PERFORMANCE_TEST); }],
      ['performance_pen_getAlpha', () => { return new PerformancePenGetAlpha(StyleType.DRAW_STYLE_PERFORMANCE_TEST); }],
      ['performance_pen_setColorFilter', () => { return new PerformancePenSetColorFilter(StyleType.DRAW_STYLE_PERFORMANCE_TEST); }],
      ['performance_pen_getColorFilter', () => { return new PerformancePenGetColorFilter(StyleType.DRAW_STYLE_PERFORMANCE_TEST); }],
      ['performance_pen_setMaskFilter', () => { return new PerformancePenSetMaskFilter(StyleType.DRAW_STYLE_PERFORMANCE_TEST); }],
      ['performance_pen_setMiterLimit', () => { return new PerformancePenSetMiterLimit(StyleType.DRAW_STYLE_PERFORMANCE_TEST); }],
      ['performance_pen_getMiterLimit', () => { return new PerformancePenGetMiterLimit(StyleType.DRAW_STYLE_PERFORMANCE_TEST); }],
      ['performance_pen_setPathEffect', () => { return new PerformancePenSetPathEffect(StyleType.DRAW_STYLE_PERFORMANCE_TEST); }],
      ['performance_pen_setShadowLayer', () => { return new PerformancePenSetShadowLayer(StyleType.DRAW_STYLE_PERFORMANCE_TEST); }],
      ['performance_pen_setShaderEffect', () => { return new PerformancePenSetShaderEffect(StyleType.DRAW_STYLE_PERFORMANCE_TEST); }],
      ['performance_pen_setBlendMode', () => { return new PerformancePenSetBlendMode(StyleType.DRAW_STYLE_PERFORMANCE_TEST); }],
      ['performance_pen_setDither', () => { return new PerformancePenSetDither(StyleType.DRAW_STYLE_PERFORMANCE_TEST); }],
      ['performance_pen_setJoinStyle', () => { return new PerformancePenSetJoinStyler(StyleType.DRAW_STYLE_PERFORMANCE_TEST); }],
      ['performance_pen_getJoinStyle', () => { return new PerformancePenGetJoinStyler(StyleType.DRAW_STYLE_PERFORMANCE_TEST); }],
      ['performance_pen_setCapStyle', () => { return new PerformancePenSetCapStyle(StyleType.DRAW_STYLE_PERFORMANCE_TEST); }],
      ['performance_pen_getCapStyle', () => { return new PerformancePenGetCapStyle(StyleType.DRAW_STYLE_PERFORMANCE_TEST); }],
      ['performance_pen_getWidth', () => { return new PerformancePenGetWidth(StyleType.DRAW_STYLE_PERFORMANCE_TEST); }],
      ['performance_pen_isAntiAlias', () => { return new PerformancePenIsAntiAlias(StyleType.DRAW_STYLE_PERFORMANCE_TEST); }],
      ['performance_pen_reset', () => { return new PerformancePenReset(StyleType.DRAW_STYLE_PERFORMANCE_TEST); }],
      ['performance_pen_getFillPath', () => { return new PerformancePenGetFillPath(StyleType.DRAW_STYLE_PERFORMANCE_TEST); }],
      ['performance_pen_setImageFilter', () => { return new PerformancePenSetImageFilter(StyleType.DRAW_STYLE_PERFORMANCE_TEST); }],
      ['performance_region_isPointContained', () => { return new PerformanceRegionIsPointContained(StyleType.DRAW_STYLE_PERFORMANCE_TEST); }],
      ['performance_region_isRegionContained', () => { return new PerformanceRegionIsRegionContained(StyleType.DRAW_STYLE_PERFORMANCE_TEST); }],
      ['performance_region_op', () => { return new PerformanceRegionOp(StyleType.DRAW_STYLE_PERFORMANCE_TEST); }],
      ['performance_region_quickReject', () => { return new PerformanceRegionQuickReject(StyleType.DRAW_STYLE_PERFORMANCE_TEST); }],
      ['performance_region_setRect', () => { return new PerformanceRegionSetRect(StyleType.DRAW_STYLE_PERFORMANCE_TEST); }],
      ['performance_region_setPath', () => { return new PerformanceRegionSetPath(StyleType.DRAW_STYLE_PERFORMANCE_TEST); }],
    ]
  );
  
  static StabilityMap: Map<string, Function> = new Map(
    [
      ['canvasdrawrect', () => { return new CanvasDrawRect(); }],
    ]
  );

  static getStabilityCpuAllCase(): Map<string, Function> {
    return this.StabilityMap;
  }    

  static getFunctonCpuAllCase(): Map<string, Function> {
    return this.FunctionMap;
  }

   static getFunctonCpuCase(caseName: string): TestBase {
     let func:Function | undefined = this.FunctionMap.get(caseName);
     if (func == undefined || func == null) {
       console.error(TAG, 'Testcase name is invalid');
       return null;
     }
     return func();
   }

  static getPerformanceCpuCase(caseName: string): TestBase {
    let func:Function | undefined = this.PerformanceMap.get(caseName);
    if (func == undefined || func == null) {
      console.error(TAG, 'Testcase name is invalid');
      return null;
    }
    return func();
  }
  static getFunctonGpuUpScreenCase(caseName: string): TestBase {
    let func:Function | undefined = this.FunctionMap.get(caseName);
    if (func == undefined || func == null) {
      console.error(TAG, 'Testcase name is invalid');
      return null;
    }
    return func();
  }

  static getPerformanceGpuUpScreenCase(caseName: string): TestBase {
    let func:Function | undefined = this.PerformanceMap.get(caseName);
    if (func == undefined || func == null) {
      console.error(TAG, 'Testcase name is invalid');
      return null;
    }
    return func();
  }
}