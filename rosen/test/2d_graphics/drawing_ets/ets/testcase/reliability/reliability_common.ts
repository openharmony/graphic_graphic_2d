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
import { common2D, drawing } from '@kit.ArkGraphics2D';

export function drawAll(canvas: drawing.Canvas) {
  // 1.圆弧
  let rect: common2D.Rect = {
    left: 50,
    top: 50,
    right: 150,
    bottom: 150,
  };
  let translateX = 0;
  let translateY = 100;
  canvas.translate(translateX, translateY);
  let startAngle = 0;
  let sweepAngle = 180;
  canvas.drawArc(rect, startAngle, sweepAngle);
  // 2.圆形
  let x = 100;
  let y = 200;
  let radix = 50;
  canvas.drawCircle(x, y, radix);
  // 3.画线
  let path = new drawing.Path();
  let startX = 0;
  let startY = 300;
  let lineX1 = 100;
  let lineY1 = 400;
  let lineX2 = 200;
  let lineY2 = 300;
  let lineX3 = 300;
  let lineY3 = 400;
  path.moveTo(startX, startY);
  path.lineTo(lineX1, lineY1);
  path.lineTo(lineX2, lineY2);
  path.lineTo(lineX3, lineY3);
  canvas.drawPath(path);
  // 4.字体
  const str: string = 'TS reliability test';
  const font: drawing.Font = new drawing.Font();
  const textBlob: drawing.TextBlob = drawing.TextBlob.makeFromString(
    str,
    font,
    drawing.TextEncoding.TEXT_ENCODING_UTF8
  );
  let textX = 50;
  let textY = 500;
  canvas.drawTextBlob(textBlob, textX, textY);
  let charX = 300;
  let charY = 20;
  canvas.drawSingleCharacter('A', font, charX, charY);
  // 5.设置背景颜色
  let brush = new drawing.Brush();
  // todo: setColor(color: number): void;此方法还未转测
  // brush.setColor(0x22ff0088);
  let color: common2D.Color = { alpha: 22, red: 255, green: 0, blue: 88 };
  brush.setColor(color);
  canvas.drawBackground(brush);
  // 6.绘制点
  let pointX = 500;
  let pointY = 100;
  canvas.drawPoint(pointX, pointY);
}