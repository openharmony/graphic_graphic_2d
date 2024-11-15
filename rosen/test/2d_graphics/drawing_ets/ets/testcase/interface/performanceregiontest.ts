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

export class PerformanceRegionIsPointContained extends TestBase {
  public constructor(styleType: number = StyleType.DRAW_STYLE_NONE) {
    super();
    this.styleType_ = styleType;
  }

  public OnTestPerformance(canvas: drawing.Canvas) {
    let region = new drawing.Region();
    this.StartPerformanceCaculate();
    for (let i = 0; i < this.testCount_; i++) {
      region.isPointContained(10, 10);
    }
  }
}

export class PerformanceRegionIsRegionContained extends TestBase {
  public constructor(styleType: number = StyleType.DRAW_STYLE_NONE) {
    super();
    this.styleType_ = styleType;
  }

  public OnTestPerformance(canvas: drawing.Canvas) {
    let region = new drawing.Region();
    let regionContain = new drawing.Region();
    this.StartPerformanceCaculate();
    for (let i = 0; i < this.testCount_; i++) {
      region.isRegionContained(regionContain);
    }
  }
}

export class PerformanceRegionOp extends TestBase {
  public constructor(styleType: number = StyleType.DRAW_STYLE_NONE) {
    super();
    this.styleType_ = styleType;
  }

  public OnTestPerformance(canvas: drawing.Canvas) {
    let region = new drawing.Region();
    let regionOp = new drawing.Region();
    this.StartPerformanceCaculate();
    for (let i = 0; i < this.testCount_; i++) {
      region.op(regionOp, drawing.RegionOp.INTERSECT);
    }
  }
}

export class PerformanceRegionQuickReject extends TestBase {
  public constructor(styleType: number = StyleType.DRAW_STYLE_NONE) {
    super();
    this.styleType_ = styleType;
  }

  public OnTestPerformance(canvas: drawing.Canvas) {
    let region = new drawing.Region();
    this.StartPerformanceCaculate();
    for (let i = 0; i < this.testCount_; i++) {
      region.quickReject(10, 20, 100, 200);
    }
  }
}

export class PerformanceRegionSetRect extends TestBase {
  public constructor(styleType: number = StyleType.DRAW_STYLE_NONE) {
    super();
    this.styleType_ = styleType;
  }

  public OnTestPerformance(canvas: drawing.Canvas) {
    let region = new drawing.Region();
    this.StartPerformanceCaculate();
    for (let i = 0; i < this.testCount_; i++) {
      region.setRect(10, 20, 100, 200);
    }
  }
}

export class PerformanceRegionSetPath extends TestBase {
  public constructor(styleType: number = StyleType.DRAW_STYLE_NONE) {
    super();
    this.styleType_ = styleType;
  }

  public OnTestPerformance(canvas: drawing.Canvas) {
    let region = new drawing.Region();
    let regionPath = new drawing.Region();
    let path = new drawing.Path();
    this.StartPerformanceCaculate();
    for (let i = 0; i < this.testCount_; i++) {
      region.setPath(path, regionPath);
    }
  }
}


