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

const blendModeList: LinkedList<BlendMode> = new LinkedList()
blendModeList.add(BlendMode.NONE)
blendModeList.add(BlendMode.CLEAR)
blendModeList.add(BlendMode.SRC)
blendModeList.add(BlendMode.DST)
blendModeList.add(BlendMode.SRC_OVER)
blendModeList.add(BlendMode.DST_OVER)
blendModeList.add(BlendMode.SRC_IN)
blendModeList.add(BlendMode.DST_IN)
blendModeList.add(BlendMode.SRC_OUT)
blendModeList.add(BlendMode.DST_OUT)
blendModeList.add(BlendMode.SRC_ATOP)
blendModeList.add(BlendMode.DST_ATOP)
blendModeList.add(BlendMode.XOR)
blendModeList.add(BlendMode.PLUS)
blendModeList.add(BlendMode.MODULATE)
blendModeList.add(BlendMode.SCREEN)
blendModeList.add(BlendMode.OVERLAY)
blendModeList.add(BlendMode.DARKEN)
blendModeList.add(BlendMode.LIGHTEN)
blendModeList.add(BlendMode.COLOR_DODGE)
blendModeList.add(BlendMode.COLOR_BURN)
blendModeList.add(BlendMode.HARD_LIGHT)
blendModeList.add(BlendMode.SOFT_LIGHT)
blendModeList.add(BlendMode.DIFFERENCE)
blendModeList.add(BlendMode.EXCLUSION)
blendModeList.add(BlendMode.MULTIPLY)
blendModeList.add(BlendMode.HUE)
blendModeList.add(BlendMode.SATURATION)
blendModeList.add(BlendMode.COLOR)
blendModeList.add(BlendMode.LUMINOSITY)

@Component
export struct TestBlendPage {
  @State mBlendMode: BlendMode = BlendMode.NONE
  @State mUseOffscreen: boolean = false
  @State mBrightness: number = 1
  @State mBrightnessRate: number = 0.5
  @State mLightUpDegree: number = 0.5

  build() {
    NavDestination() {
      Column() {
        Column() {
          Row() {
            Circle().width(100).height(100).fill(Color.Green).position({ x: 20, y: 30 })
            Circle().width(100).height(100).fill(Color.Red).position({ x: 70, y: 30 })
          }.blendMode(this.mBlendMode, this.mUseOffscreen ? BlendApplyType.OFFSCREEN : BlendApplyType.FAST)
          .alignItems(VerticalAlign.Center).width('100%').height('100%')
        }.height('20%').width('80%').backgroundImage($r('app.media.test1')).backgroundImageSize(ImageSize.FILL)

        Row() {
          Text('useOffScreen:' + this.mUseOffscreen)
          Toggle({ type: ToggleType.Switch, isOn: this.mUseOffscreen })
            .selectedColor('#007DFF')
            .switchPointColor('#FFFFFF')
            .onChange((isOn: boolean) => {
              this.mUseOffscreen = isOn
            })
          Select([{ value: 'NONE' }, { value: 'CLEAR' }, { value: 'SRC' }, { value: 'DST' },
            { value: 'SRC_OVER' }, { value: 'DST_OVER' }, { value: 'SRC_IN' }, { value: 'DST_IN' },
            { value: 'SRC_OUT' }, { value: 'DST_OUT' }, { value: 'SRC_ATOP' }, { value: 'DST_ATOP' },
            { value: 'XOR' }, { value: 'PLUS' }, { value: 'MODULATE' }, { value: 'SCREEN' },
            { value: 'OVERLAY' }, { value: 'DARKEN' }, { value: 'LIGHTEN' }, { value: 'COLOR_DODGE' },
            { value: 'EXCLUSION' }, { value: 'MULTIPLY' }, { value: 'HUE' }, { value: 'SATURATION' },
            { value: 'COLOR' }, { value: 'LUMINOSITY' }])
            .selected(0)
            .value('NONE')
            .font({ size: 16, weight: 400 })
            .fontColor('#182431')
            .selectedOptionFont({ size: 16, weight: 400 })
            .optionFont({ size: 16, weight: 400 })
            .arrowPosition(ArrowPosition.END)
            .menuAlign(MenuAlignType.START, { dx: 0, dy: 0 })
            .optionWidth(200)
            .optionHeight(300)
            .onSelect((index: number) => {
              this.mBlendMode = blendModeList.get(index)
            })
        }

        Column() {
          Row() {
            Text('亮度调节(0-2) ')
            Text('brightness:' + this.mBrightness).fontColor(Color.White)
          }.width('100%').height('80%').backgroundColor('#aaaccbba').brightness(this.mBrightness)

          Slider({
            value: this.mBrightness * 10,
            min: 0,
            max: 20,
            style: SliderStyle.OutSet
          }).onChange((value: number) => {
            this.mBrightness = value / 10
          }).width('50%')
        }.width('80%').height('20%').backgroundImage($r('app.media.background')).backgroundImageSize(ImageSize.Cover)

        Blank(5)
        Column() {
          Row() {
            Text('backgroundBrightness ')
            Text('rate=' + this.mBrightnessRate + ' deg=' + this.mLightUpDegree).fontColor(Color.White)
          }
          .width('100%')
          .height('100%')
          .backgroundColor('#aaaccbba')
          .backgroundBrightness({ rate: this.mBrightnessRate, lightUpDegree: this.mLightUpDegree })

          Slider({
            value: this.mBrightnessRate * 10,
            min: 0,
            max: 10,
            style: SliderStyle.OutSet
          }).onChange((value: number) => {
            this.mBrightnessRate = value / 10
          }).width('80%')
        }.width('80%').height('20%').backgroundImage($r('app.media.background')).backgroundImageSize(ImageSize.Cover)

        Slider({
          value: this.mLightUpDegree * 10,
          min: -10,
          max: 10,
          style: SliderStyle.OutSet
        }).onChange((value: number) => {
          this.mLightUpDegree = value / 10
        }).width('80%')
      }.width('80%').height('20%').backgroundImage($r('app.media.background')).backgroundImageSize(ImageSize.Cover)
    }.hideTitleBar(true)
  }
}