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