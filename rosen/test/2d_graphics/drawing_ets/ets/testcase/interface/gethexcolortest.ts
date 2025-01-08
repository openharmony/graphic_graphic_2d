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

import {TestBase, TestFunctionStyleType} from '../../pages/testbase';
import { OHRandom } from '../../utils/OHRandom';
import hilog from '@ohos.hilog';
import systemDateTime from '@ohos.systemDateTime';
import { drawing, common2D } from '@kit.ArkGraphics2D';

export class PenBrushGetHexColor extends TestBase {
  private rand: OHRandom = new OHRandom(systemDateTime.getTime());
  public constructor() {
    super();
  }

  public OnTestPerformance(canvas: drawing.Canvas) {
    let pen = new drawing.Pen();
    let color: common2D.Color = { alpha: this.rand.nextRangeU(0,255), red: this.rand.nextRangeU(0,255), green: this.rand.nextRangeU(0,255), blue: this.rand.nextRangeU(0,255) };
    pen.setColor(color);
    pen.setStrokeWidth(32);
    canvas.attachPen(pen);
    let pathSrc: drawing.Path = new drawing.Path();
    let pathDst: drawing.Path = new drawing.Path();
    pathSrc.moveTo(0, 200);
    pathSrc.lineTo(this.width_, this.height_);
    pen.getFillPath(pathSrc, pathDst);

    let brush = new drawing.Brush();
    color = { alpha: this.rand.nextRangeU(0,255), red: this.rand.nextRangeU(0,255), green: this.rand.nextRangeU(0,255), blue: this.rand.nextRangeU(0,255) };
    brush.setColor(color);
    canvas.attachBrush(brush);

    for (let i = 0; i < this.testCount_; i++) {
      pen.getHexColor();
      brush.getHexColor();
    }
    canvas.drawPath(pathDst);
    canvas.drawPath(pathSrc);
    canvas.detachPen();
    canvas.detachBrush();
  }

  public OnTestFunction(canvas: drawing.Canvas) {
    {
      let color: common2D.Color = { alpha: this.rand.nextRangeU(0,255), red: this.rand.nextRangeU(0,255), green: this.rand.nextRangeU(0,255), blue: this.rand.nextRangeU(0,255) };

      // test Pen.getHexColor(): number
      let pen = new drawing.Pen();
      pen.setColor(color);
      pen.setStrokeWidth(32);
      canvas.attachPen(pen);

      let pathSrc: drawing.Path = new drawing.Path();
      let pathDst: drawing.Path = new drawing.Path();
      pathSrc.moveTo(0, 200);
      pathSrc.lineTo(this.width_, this.height_);
      pen.getFillPath(pathSrc, pathDst);

      let pen_color_text = "Pen ARGB tuple: " + String(pen.getColor().alpha) + ' ' + String(pen.getColor().red) + ' '  + String(pen.getColor().green) + ' '  + String(pen.getColor().blue)
      let pen_hexcolor_text = "Pen ARGB hex: " + pen.getHexColor().toString(16);
      hilog.info(0, "Test graphic2d_get_hex_color", pen_color_text);
      hilog.info(0, "Test graphic2d_get_hex_color", pen_hexcolor_text);

      canvas.drawPath(pathDst);
      canvas.drawPath(pathSrc);
      canvas.detachPen();

      //test Brush.getHexColor(): number
      let brush = new drawing.Brush();
      color = { alpha: this.rand.nextRangeU(0,255), red: this.rand.nextRangeU(0,255), green: this.rand.nextRangeU(0,255), blue: this.rand.nextRangeU(0,255) };
      brush.setColor(color);

      canvas.attachBrush(brush);
      canvas.drawCircle(480, 520, 120);

      let brush_color_text = "Brush ARGB tuple: " + String(brush.getColor().alpha) + ' ' + String(brush.getColor().red) + ' '  + String(brush.getColor().green) + ' '  + String(brush.getColor().blue);
      let brush_hexcolor_text = "Brush ARGB hex: " + brush.getHexColor().toString(16);
      hilog.info(0, "Test graphic2d_get_hex_color", brush_color_text);
      hilog.info(0, "Test graphic2d_get_hex_color", brush_hexcolor_text);

      canvas.detachBrush();

      const font: drawing.Font = new drawing.Font();
      font.setSize(20);
      let textBlob = drawing.TextBlob.makeFromString(
        pen_color_text + '\t' + pen_hexcolor_text,
        font, drawing.TextEncoding.TEXT_ENCODING_UTF8
      );
      canvas.drawTextBlob(textBlob, 0, 30);
      textBlob = drawing.TextBlob.makeFromString(
        brush_color_text + '\t' + brush_hexcolor_text,
        font, drawing.TextEncoding.TEXT_ENCODING_UTF8
      );
      canvas.drawTextBlob(textBlob, 0, 60);
    }
  }
}