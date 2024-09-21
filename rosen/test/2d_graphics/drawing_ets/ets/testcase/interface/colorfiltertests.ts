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

import drawing from "@ohos.graphics.drawing";
import common2D from "@ohos.graphics.common2D";
import {TestBase} from '../../pages/testbase';

export class ColorFilterCreateMatrix extends TestBase {
  matrix_: Array<number> = [
    1, 0, 0, 0, 0,
    0, 1, 0, 0, 0,
    0, 0, 100, 0, 0,
    0, 0, 0, 1, 0
  ];

  public constructor() {
    super();
  }
  
  public OnTestPerformance(canvas: drawing.Canvas) {
    let brush = new drawing.Brush();
    for (let i = 0; i < this.testCount_; i++) {
      let colorFilter = drawing.ColorFilter.createMatrixColorFilter(this.matrix_);
      brush.setColorFilter(colorFilter);
    }
  }

  public OnTestFunction(canvas: drawing.Canvas) {
    let brush = new drawing.Brush();
    let colorFilter = drawing.ColorFilter.createMatrixColorFilter(this.matrix_);
    let color: common2D.Color = {alpha: 255, red: 255, green: 1, blue: 1};
    let rect1: common2D.Rect = {left: 50, top: 100, right: 200, bottom: 400};
    let rect2: common2D.Rect = {left: 250, top: 100, right: 400, bottom: 400};

    brush.setColor(color);
    canvas.attachBrush(brush);
    canvas.drawRect(rect1);

    canvas.detachBrush();
    brush.setColorFilter(colorFilter);
    canvas.attachBrush(brush);
    canvas.drawRect(rect2);
    canvas.detachBrush();
  }
}