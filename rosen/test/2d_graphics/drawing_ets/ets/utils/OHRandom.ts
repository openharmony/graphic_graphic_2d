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

import common2D from '@ohos.graphics.common2D';
import drawing from '@ohos.graphics.drawing';
import ArrayList from '@ohos.util.ArrayList';
const TAG = '[OHRandom]';
const KKMUL = 30345;
const KjMUL = 18000;
const KADD = 1013904223;
export const W = 640;
export const H = 480;
export const N = 300;
const OFFSET = 1.0 / 3;
export class OHRandom {
  private fk:number = 0;
  private fj:number = 0;

  public constructor(seed ?: number ) {
    console.log(TAG, 'create TestBase seed is:' + seed);
    if (seed !== undefined) {
      this.init(seed);
    } else {
      this.init(0);
    }
    console.log(TAG, 'create TestBase');
  }

  public init(seed : number) {
    this.fk = KKMUL * seed + KADD;
    if (this.fk === 0) {
      this.fk = KKMUL * this.fk + KADD;
    }
    this.fj = KKMUL * this.fk + KADD;
    if (this.fj === 0) {
      this.fj = KKMUL * this.fj + KADD;
    }
  }

  public SetXYWH(canvasRect: common2D.Rect, x: number, y:number, width: number, height: number) {
    canvasRect.left = x;
    canvasRect.top = y;
    canvasRect.right = x + width;
    canvasRect.bottom = y + height;
  }

  public offset(canvasRect: common2D.Rect, dx: number, dy: number) {
    canvasRect.left = canvasRect.left + dx;
    canvasRect.top = canvasRect.top + dy;
    canvasRect.right = canvasRect.right + dx;
    canvasRect.bottom = canvasRect.bottom + dy;
  }

  public nextU(): number {
    this.fk = KKMUL * (this.fk & 0xffff) + (this.fk >> 16);
    this.fj = KjMUL * (this.fj & 0xffff) + (this.fj >> 16);
    return (((this.fk << 16) | (this.fk >> 16)) + this.fj);
  }

  public setColorAndRect(paintColors : ArrayList<common2D.Color>, canvasRects : ArrayList<common2D.Rect>,
                         shiftValue: number) {
    for (let i = 0; i < N; i++) {
      let canvasRect: common2D.Rect = {left: 0, top: 0, right: 0, bottom: 0};
      let paintColor: common2D.Color = {alpha : 0, red: 0, green : 0, blue : 0};
      let x: number = this.nextU() % W;
      let y: number = this.nextU() % H;
      let w: number = this.nextU() % W;
      let h: number = this.nextU() % H;
      w >>= shiftValue;
      h >>= shiftValue;
      x -= w/2;
      y -= h/2;
      this.SetXYWH(canvasRect, x, y, w,h);
      this.offset(canvasRect, OFFSET, OFFSET);
      canvasRects.add(canvasRect);
      paintColor.alpha = this.nextU() | 0xFF;
      paintColor.red = this.nextU() | 0x80;
      paintColor.green = this.nextU() | 0x80;
      paintColor.blue = this.nextU() | 0x80;
      paintColors.add(paintColor);
    }
  }

  public nextUFixed1(): number {
    return this.nextU() >> 16
  }

  public nextUScalar1(): number {
    let a = this.nextUFixed1() * 0.0000152587890625
    return a > 0 ? a : -a
  }

  public nextRangeU(min: number, max: number): number {
    let a = this.nextU()
    a = a < 0 ? -a : a
    let range: number = max - min + 1;
    if (0 == range) {
      return a;
    } else {
      return min + a % range;
    }
  }

  public nextULessThan(count: number) {
    return this.nextRangeU(0, count -1)
  }

  public nextRangeF(min: number, max: number): number {
    return this.nextUScalar1() * (max - min) + min;
  }

}