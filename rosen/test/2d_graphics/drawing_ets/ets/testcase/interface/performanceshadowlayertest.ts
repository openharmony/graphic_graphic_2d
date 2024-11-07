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

import drawing from '@ohos.graphics.drawing';
import { StyleType, TestBase } from '../../pages/testbase';
import { common2D } from '@kit.ArkGraphics2D';

export class PerformanceShadowLayerCreate extends TestBase {
  public constructor(styleType: number = StyleType.DRAW_STYLE_NONE) {
    super();
    this.styleType_ = styleType;
  }

  public OnTestPerformance(canvas: drawing.Canvas) {
    let color: common2D.Color = { alpha: 255, red: 1, green: 255, blue: 1 };
    this.StartPerformanceCaculate();
    for (let i = 0; i < this.testCount_; i++) {
      drawing.ShadowLayer.create(10, 2, 2, color);
    }
  }
}



