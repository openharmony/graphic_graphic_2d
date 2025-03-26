/*
 * Copyright (C) 2025 Huawei Device Co., Ltd.
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

import { LinkedList } from '@kit.ArkTS'

const directionList: LinkedList<GradientDirection> = new LinkedList()
directionList.add(GradientDirection.Left)
directionList.add(GradientDirection.Top)
directionList.add(GradientDirection.Right)
directionList.add(GradientDirection.Bottom)
directionList.add(GradientDirection.LeftTop)
directionList.add(GradientDirection.LeftBottom)
directionList.add(GradientDirection.RightTop)
directionList.add(GradientDirection.RightBottom)
directionList.add(GradientDirection.None)

@Component
export struct LinearGradientBlurTest {
  @State mBlurRadius: number = 0
  @State mLGBlurOptions: LinearGradientBlurOptions =
    { fractionStops: [[0, 0], [0, 0.2], [0.4, 0.4], [0.7, 0.6], [1, 0.8], [1, 1]], direction: GradientDirection.Bottom }

  build() {
    NavDestination() {
      Column() {
        Image($r('app.media.test1')).width('80%').linearGradientBlur(this.mBlurRadius, this.mLGBlurOptions)
        Row() {
          Slider({
            value: this.mBlurRadius,
            min: 0,
            max: 1000,
            style: SliderStyle.OutSet
          })
            .onChange((value: number) => {
              this.mBlurRadius = value
            }).width('50%')
        }

        Select([{ value: 'Left' }, { value: 'Top' }, { value: 'Right' }, { value: 'Bottom' },
          { value: 'LeftTop' }, { value: 'LeftBottom' }, { value: 'RightTop' }, { value: 'RightBottom' },
          { value: 'None' }])
          .selected(3)
          .value('Bottom')
          .font({ size: 16, weight: 400 })
          .fontColor('#182431')
          .selectedOptionFont({ size: 16, weight: 400 })
          .optionFont({ size: 16, weight: 400 })
          .arrowPosition(ArrowPosition.END)
          .menuAlign(MenuAlignType.START, { dx: 0, dy: 0 })
          .optionWidth(200)
          .optionHeight(300)
          .onSelect((index: number) => {
            this.mLGBlurOptions.direction = directionList.get(index)
          })
          .width('35%')
      }
    }.hideTitleBar(true)
  }
}