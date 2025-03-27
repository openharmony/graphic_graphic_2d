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

const blurStyleList: LinkedList<BlurStyle> = new LinkedList()
blurStyleList.add(BlurStyle.Thin)
blurStyleList.add(BlurStyle.Regular)
blurStyleList.add(BlurStyle.Thick)
blurStyleList.add(BlurStyle.BACKGROUND_THIN)
blurStyleList.add(BlurStyle.BACKGROUND_REGULAR)
blurStyleList.add(BlurStyle.BACKGROUND_THICK)
blurStyleList.add(BlurStyle.BACKGROUND_ULTRA_THICK)
blurStyleList.add(BlurStyle.NONE)
blurStyleList.add(BlurStyle.COMPONENT_ULTRA_THIN)
blurStyleList.add(BlurStyle.COMPONENT_THIN)
blurStyleList.add(BlurStyle.COMPONENT_REGULAR)
blurStyleList.add(BlurStyle.COMPONENT_THICK)
blurStyleList.add(BlurStyle.COMPONENT_ULTRA_THICK)

const adaptiveColorList: LinkedList<AdaptiveColor> = new LinkedList()
adaptiveColorList.add(AdaptiveColor.DEFAULT)
adaptiveColorList.add(AdaptiveColor.AVERAGE)

const blurStyleActivePolicyList: LinkedList<BlurStyleActivePolicy> = new LinkedList()
blurStyleActivePolicyList.add(BlurStyleActivePolicy.FOLLOWS_WINDOW_ACTIVE_STATE)
blurStyleActivePolicyList.add(BlurStyleActivePolicy.ALWAYS_ACTIVE)
blurStyleActivePolicyList.add(BlurStyleActivePolicy.ALWAYS_INACTIVE)

const colorModeList: LinkedList<ThemeColorMode> = new LinkedList()
colorModeList.add(ThemeColorMode.SYSTEM)
colorModeList.add(ThemeColorMode.LIGHT)
colorModeList.add(ThemeColorMode.DARK)

@Component
export struct MaterialBlurTest {
  @State mBgBlurStyle: BlurStyle = BlurStyle.NONE
  @State mBgBlurStyleOpts: BackgroundBlurStyleOptions = {
    colorMode: ThemeColorMode.SYSTEM,
    scale: 1,
    adaptiveColor: AdaptiveColor.DEFAULT,
    blurOptions: { grayscale: [0, 0] },
    policy: BlurStyleActivePolicy.ALWAYS_ACTIVE,
    inactiveColor: Color.Red
  }
  @State mFgBlurStyle: BlurStyle = BlurStyle.NONE
  @State mFgBlurStyleOpts: ForegroundBlurStyleOptions = {
    colorMode: ThemeColorMode.SYSTEM,
    scale: 1,
    adaptiveColor: AdaptiveColor.DEFAULT,
    blurOptions: { grayscale: [0, 0] }
  }
  @State mBgBlurEffectOpts: BackgroundEffectOptions = {
    radius: 0,
    saturation: 1,
    brightness: 1,
    color: Color.Transparent,
    adaptiveColor: AdaptiveColor.DEFAULT,
    blurOptions: { grayscale: [0, 0] },
    policy: BlurStyleActivePolicy.ALWAYS_ACTIVE,
    inactiveColor: Color.Red
  }

  build() {
    NavDestination() {
      Column() {
        Row() {
          Text('材质模糊(前景) .foregroundBlurStyle')
            .fontColor(Color.White)
            .fontSize(10)
            .backgroundImage($r('app.media.test1'))
            .backgroundImageSize(ImageSize.Contain)
            .width('50%')
            .height(120)
            .foregroundBlurStyle(this.mFgBlurStyle, this.mFgBlurStyleOpts)
            .borderColor(Color.Black)
            .borderWidth(1)
          Blank(5)
          Column() {
            Select([{ value: 'Thin' }, { value: 'Regular' }, { value: 'Thick' },
              { value: 'BG_THIN' }, { value: 'BG_REGULAR' }, { value: 'BG_THICK' }, { value: 'BG_ULTRA_THICK' },
              { value: 'NONE' }, { value: 'CP_ULTRA_THIN' }, { value: 'CP_THIN' }, { value: 'CP_REGULAR' },
              { value: 'CP_THICK' }, { value: 'CP_ULTRA_THICK' }])
              .selected(7)
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
                this.mFgBlurStyle = blurStyleList.get(index)
              })
              .width('35%')
            Select([{ value: 'DEFAULT' }, { value: 'AVERAGE' }])
              .selected(0)
              .value('DEFAULT')
              .font({ size: 16, weight: 400 })
              .fontColor('#182431')
              .selectedOptionFont({ size: 16, weight: 400 })
              .optionFont({ size: 16, weight: 400 })
              .arrowPosition(ArrowPosition.END)
              .menuAlign(MenuAlignType.START, { dx: 0, dy: 0 })
              .optionWidth(200)
              .optionHeight(300)
              .onSelect((index: number) => {
                this.mFgBlurStyleOpts.adaptiveColor = adaptiveColorList.get(index)
              })
              .width('35%')
            Select([{ value: 'SYSTEM' }, { value: 'LIGHT' }, { value: 'DARK' }])
              .selected(0)
              .value('SYSTEM')
              .font({ size: 16, weight: 400 })
              .fontColor('#182431')
              .selectedOptionFont({ size: 16, weight: 400 })
              .optionFont({ size: 16, weight: 400 })
              .arrowPosition(ArrowPosition.END)
              .menuAlign(MenuAlignType.START, { dx: 0, dy: 0 })
              .optionWidth(200)
              .optionHeight(300)
              .onSelect((index: number) => {
                this.mFgBlurStyleOpts.colorMode = colorModeList.get(index)
              })
              .width('35%')
          }
        }

        Row() {
          Text('fg scale:' + this.mFgBlurStyleOpts.scale).width('25%')
          Slider({
            value: this.mFgBlurStyleOpts.scale as number * 10,
            min: 0,
            max: 10,
            style: SliderStyle.OutSet
          }).onChange((value: number) => {
            this.mFgBlurStyleOpts.scale = value / 10
          }).width('50%')
        }

        Row() {
          Text('材质模糊(内容和背景之间) .backgroundBlurStyle')
            .fontColor(Color.White)
            .fontSize(10)
            .backgroundImage($r('app.media.test1'))
            .backgroundImageSize(ImageSize.Contain)
            .width('50%')
            .height(120)
            .foregroundBlurStyle(this.mBgBlurStyle, this.mBgBlurStyleOpts)
            .borderColor(Color.Black)
            .borderWidth(1)
          Blank(5)
          Column() {
            Select([{ value: 'Thin' }, { value: 'Regular' }, { value: 'Thick' },
              { value: 'BG_THIN' }, { value: 'BG_REGULAR' }, { value: 'BG_THICK' }, { value: 'BG_ULTRA_THICK' },
              { value: 'NONE' }, { value: 'CP_ULTRA_THIN' }, { value: 'CP_THIN' }, { value: 'CP_REGULAR' },
              { value: 'CP_THICK' }, { value: 'CP_ULTRA_THICK' }])
              .selected(7)
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
                this.mBgBlurStyle = blurStyleList.get(index)
              })
              .width('35%')
            Select([{ value: 'DEFAULT' }, { value: 'AVERAGE' }])
              .selected(0)
              .value('DEFAULT')
              .font({ size: 16, weight: 400 })
              .fontColor('#182431')
              .selectedOptionFont({ size: 16, weight: 400 })
              .optionFont({ size: 16, weight: 400 })
              .arrowPosition(ArrowPosition.END)
              .menuAlign(MenuAlignType.START, { dx: 0, dy: 0 })
              .optionWidth(200)
              .optionHeight(300)
              .onSelect((index: number) => {
                this.mBgBlurStyleOpts.adaptiveColor = adaptiveColorList.get(index)
              })
              .width('35%')
            Select([{ value: 'SYSTEM' }, { value: 'LIGHT' }, { value: 'DARK' }])
              .selected(0)
              .value('SYSTEM')
              .font({ size: 16, weight: 400 })
              .fontColor('#182431')
              .selectedOptionFont({ size: 16, weight: 400 })
              .optionFont({ size: 16, weight: 400 })
              .arrowPosition(ArrowPosition.END)
              .menuAlign(MenuAlignType.START, { dx: 0, dy: 0 })
              .optionWidth(200)
              .optionHeight(300)
              .onSelect((index: number) => {
                this.mBgBlurStyleOpts.colorMode = colorModeList.get(index)
              })
              .width('35%')
            Select([{ value: 'FOLLOWS_WINDOW_ACTIVE_STATE' }, { value: 'ALWAYS_ACTIVE' }, { value: 'ALWAYS_INACTIVE' }])
              .selected(1)
              .value('ALWAYS_ACTIVE')
              .font({ size: 16, weight: 400 })
              .fontColor('#182431')
              .selectedOptionFont({ size: 16, weight: 400 })
              .optionFont({ size: 16, weight: 400 })
              .arrowPosition(ArrowPosition.END)
              .menuAlign(MenuAlignType.START, { dx: 0, dy: 0 })
              .optionWidth(200)
              .optionHeight(300)
              .onSelect((index: number) => {
                this.mBgBlurStyleOpts.policy = blurStyleActivePolicyList.get(index)
              })
              .width('35%')
          }
        }

        Row() {
          Text('bg scale:' + this.mBgBlurStyleOpts.scale).width('25%')
          Slider({
            value: this.mBgBlurStyleOpts.scale as number * 10,
            min: 0,
            max: 10,
            style: SliderStyle.OutSet
          }).onChange((value: number) => {
            this.mBgBlurStyleOpts.scale = value / 10
          }).width('50%')
        }

        Row() {
          Text('自定义材质模糊(内容和背景之间) .backgroundEffect')
            .fontColor(Color.White)
            .fontSize(10)
            .backgroundImage($r('app.media.test1'))
            .backgroundImageSize(ImageSize.Contain)
            .width('50%')
            .height(120)
            .backgroundEffect(this.mBgBlurEffectOpts)
            .borderColor(Color.Black)
            .borderWidth(1)
          Blank(5)
          Column() {
            Select([{ value: 'DEFAULT' }, { value: 'AVERAGE' }])
              .selected(0)
              .value('DEFAULT')
              .font({ size: 16, weight: 400 })
              .fontColor('#182431')
              .selectedOptionFont({ size: 16, weight: 400 })
              .optionFont({ size: 16, weight: 400 })
              .arrowPosition(ArrowPosition.END)
              .menuAlign(MenuAlignType.START, { dx: 0, dy: 0 })
              .optionWidth(200)
              .optionHeight(300)
              .onSelect((index: number) => {
                this.mBgBlurEffectOpts.adaptiveColor = adaptiveColorList.get(index)
              })
              .width('35%')
            Select([{ value: 'FOLLOWS_WINDOW_ACTIVE_STATE' }, { value: 'ALWAYS_ACTIVE' }, { value: 'ALWAYS_INACTIVE' }])
              .selected(1)
              .value('ALWAYS_ACTIVE')
              .font({ size: 16, weight: 400 })
              .fontColor('#182431')
              .selectedOptionFont({ size: 16, weight: 400 })
              .optionFont({ size: 16, weight: 400 })
              .arrowPosition(ArrowPosition.END)
              .menuAlign(MenuAlignType.START, { dx: 0, dy: 0 })
              .optionWidth(200)
              .optionHeight(300)
              .onSelect((index: number) => {
                this.mBgBlurEffectOpts.policy = blurStyleActivePolicyList.get(index)
              })
              .width('35%')
          }
        }

        Row() {
          Text('radius:' + this.mBgBlurEffectOpts.radius).width('25%')
          Slider({
            value: this.mBgBlurEffectOpts.radius,
            min: 0,
            max: 200,
            style: SliderStyle.OutSet
          }).onChange((value: number) => {
            this.mBgBlurEffectOpts.radius = value
          }).width('50%')
        }

        Row() {
          Text('saturation:' + this.mBgBlurEffectOpts.saturation).width('25%')
          Slider({
            value: this.mBgBlurEffectOpts.saturation as number * 100,
            min: 0,
            max: 200,
            style: SliderStyle.OutSet
          }).onChange((value: number) => {
            this.mBgBlurEffectOpts.saturation = value / 100
          }).width('50%')
        }

        Row() {
          Text('brightness:' + this.mBgBlurEffectOpts.brightness).width('25%')
          Slider({
            value: this.mBgBlurEffectOpts.brightness as number * 100,
            min: 0,
            max: 200,
            style: SliderStyle.OutSet
          }).onChange((value: number) => {
            this.mBgBlurEffectOpts.brightness = value / 100
          }).width('50%')
        }
      }
    }.hideTitleBar(true)
  }
}