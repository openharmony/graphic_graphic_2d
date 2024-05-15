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
  CanvasDrawPixelMapMesh, CanvasDrawColor } from '../testcase/interface/canvastest';
import { PathLineTo, PathArcTo, PathQuadTo, PathCubicTo,PathClose, PathReset } from '../testcase/interface/pathtest';
import { MakeFromRunBuffer } from '../testcase/interface/textblobtest'
import { MakeFromString } from '../testcase/interface/textblobtest'
import { TextBlobBounds } from '../testcase/interface/textblobtest'

const TAG = '[DrawingTest]';

export class CaseFactory {
  static FunctionMap: Map<string, Function> = new Map(
    [
      ['canvasdrawrect', () => { return new CanvasDrawRect(); }],
      ['canvasdrawcircle', () => { return new CanvasDrawCircle(); }],
      ['canvasdrawcolor', () => { return new CanvasDrawColor(); }],
      ['canvasdrawpoint', () => { return new CanvasDrawPoint(); }],
      ['canvasdrawpath', () => { return new CanvasDrawPath(); }],
      ['canvasdrawline', () => { return new CanvasDrawLine(); }],
      ['canvasdrawtextblob', () => { return new CanvasDrawTextBlob(); }],
      ['canvasdrawimage', () => { return new CanvasDrawImage(); }],
    ]
  );
  static PerformanceMap: Map<string, Function> = new Map(
    [
      ['canvas_drawrect', () => { return new CanvasDrawRect(StyleType.DRAW_STYLE_COMPLEX); }], // 1000次耗时109ms
      ['canvas_drawline', () => { return new CanvasDrawLine(StyleType.DRAW_STYLE_COMPLEX); }], // 1000次耗时347ms
      ['canvas_drawpath', () => { return new CanvasDrawPath(StyleType.DRAW_STYLE_COMPLEX); }], // 100次耗时506ms
      ['canvas_drawpoints', () => { return new CanvasDrawPoint(StyleType.DRAW_STYLE_COMPLEX); }], // 1000次耗时47ms
      ['canvas_drawbitmap', () => { return new CanvasDrawImage(StyleType.DRAW_STYLE_COMPLEX); }], // 1000次耗时2ms
      ['canvas_drawcircle', () => { return new CanvasDrawCircle(StyleType.DRAW_STYLE_COMPLEX); }], // 1000次耗时1016ms
      ['canvas_drawtextblob', () => { return new CanvasDrawTextBlob(StyleType.DRAW_STYLE_COMPLEX); }], // 1000次耗时125ms
      ['canvas_drawvertices', () => { return new CanvasDrawPixelMapMesh(StyleType.DRAW_STYLE_COMPLEX); }], // 1000次耗时261ms
      ['path_lineto', () => { return new PathLineTo(StyleType.DRAW_STYLE_COMPLEX); }], // 1000次耗时70ms
      ['path_arcto', () => { return new PathArcTo(StyleType.DRAW_STYLE_COMPLEX); }], // 1000次耗时42ms
      ['path_quadto', () => { return new PathQuadTo(StyleType.DRAW_STYLE_COMPLEX); }], // 1000次耗时156ms
      ['path_cubicto', () => { return new PathCubicTo(StyleType.DRAW_STYLE_COMPLEX); }], // 1000次耗时174ms
      ['path_close', () => { return new PathClose(StyleType.DRAW_STYLE_COMPLEX); }], // 1000次耗时625ms
      ['path_reset', () => { return new PathReset(StyleType.DRAW_STYLE_COMPLEX); }], // 1000次耗时816ms
      ['textblob_createbuilder', () => { return new MakeFromRunBuffer(StyleType.DRAW_STYLE_COMPLEX); }], // 1000次耗时339ms
      ['textblob_createfrom_string', () => { return new MakeFromString(StyleType.DRAW_STYLE_COMPLEX); }], // 1000次耗时261ms
      ['textblob_getbounds', () => { return new TextBlobBounds(StyleType.DRAW_STYLE_COMPLEX); }], // 1000次耗时365ms
    ]
  );

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