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
import {TestBase, TestFunctionStyleType} from '../../pages/testbase';

export class JoinRect extends TestBase {

    public constructor() {
      super();
    }

    public OnTestPerformance(canvas: drawing.Canvas) {
      let rect: common2D.Rect = { left: 0, top: 100, right: 300, bottom: 600 };
      let other: common2D.Rect = { left: 100, top: 0, right: 600, bottom: 300 };
      for (let i = 0; i < this.testCount_; i++) {
        common2D.Utils.joinRect(rect, other);
      }
    }

    public OnTestFunction(canvas: drawing.Canvas) {
      let rect: common2D.Rect = { left: 0, top: 100, right: 300, bottom: 600 };
      let other: common2D.Rect = { left: 100, top: 0, right: 600, bottom: 300 };

      let brush = new drawing.Brush();
      let pen = new drawing.Pen();
      brush.setColor({ alpha: 100, red: 0, green: 255, blue: 0 });
      pen.setColor({ alpha: 255, red: 0, green: 255, blue: 0 });
      pen.setStrokeWidth(10);
      canvas.attachBrush(brush);
      canvas.attachPen(pen);
      canvas.drawRect(rect);
      canvas.detachPen();
      canvas.detachBrush();

      brush.setColor({ alpha: 100, red: 100, green: 0, blue: 100 });
      pen.setColor({ alpha: 255, red: 100, green: 0, blue: 100 });
      canvas.attachPen(pen);
      canvas.attachBrush(brush);
      canvas.drawRect(other);
      canvas.detachPen();
      canvas.detachBrush();

      let result: boolean = false;
      
      try {
        result = common2D.Utils.joinRect(rect, other);
      } catch(err) {
        console.error("Utils.joinRect exception: ", err.name, ":", err.message, err.stack);
      }

      console.log("Result = " + result);
      console.log("Expected rect: left = 0, top = 0, right = 600, bottom = 600");
      console.log("Actual rect: left = " + rect.left + ", top = " + rect.top +
                  ", right = " + rect.right + ", bottom = " + rect.bottom);

      if(result) {
        result = (rect.left == 0 && rect.top == 0 && rect.right == 600 && rect.bottom == 600);
      }

      if (result) {
        this.ApplyPenStyle(pen, TestFunctionStyleType.DRAW_STYLE_TYPE_1);
        canvas.attachPen(pen);
        canvas.drawRect(rect);
        canvas.detachPen();
      }
    }
  }