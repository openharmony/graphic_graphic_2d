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
import { SegmentButton, SegmentButtonItemTuple, SegmentButtonOptions } from '@kit.ArkUI'

const shadowStyleList: LinkedList<ShadowStyle> = new LinkedList()
shadowStyleList.add(ShadowStyle.OUTER_DEFAULT_XS)
shadowStyleList.add(ShadowStyle.OUTER_DEFAULT_SM)
shadowStyleList.add(ShadowStyle.OUTER_DEFAULT_MD)
shadowStyleList.add(ShadowStyle.OUTER_DEFAULT_LG)
shadowStyleList.add(ShadowStyle.OUTER_FLOATING_SM)
shadowStyleList.add(ShadowStyle.OUTER_FLOATING_MD)


@Component
export struct TestShadowPage {
  @State shadowSingleSelectOptions: SegmentButtonOptions = SegmentButtonOptions.capsule({
    buttons: [{ text: 'A' }, { text: 'B' }, { text: 'C' }] as SegmentButtonItemTuple,
    multiply: false,
    backgroundBlurStyle: BlurStyle.BACKGROUND_THICK
  })
  @State shadowSingleSelectIndex: number[] = [0]
  @State mCurrentShadowIndex: number = 0
  @State mUseShadowBatching: boolean = false
  @State mShadowOptionA: ShadowOptions = {
    radius: 20,
    color: Color.Green,
    offsetX: 20,
    offsetY: 20,
    fill: false,
    type: ShadowType.COLOR
  }
  @State mShadowImageOpcityA: number = 0.5
  @State mUseBlurAsShadowA: boolean = false
  @State mUseShadowStyleA: boolean = false
  @State mShadowStyleA: ShadowStyle = ShadowStyle.OUTER_DEFAULT_LG
  @State mShadowControlParamA: ShadowControlParams =
    new ShadowControlParams(this.mShadowImageOpcityA, this.mUseBlurAsShadowA, this.mUseShadowStyleA, this.mShadowStyleA)
  @State mShadowOptionB: ShadowOptions = {
    radius: 20,
    color: Color.Green,
    offsetX: 20,
    offsetY: 20,
    fill: false,
    type: ShadowType.COLOR
  }
  @State mShadowImageOpcityB: number = 0.5
  @State mUseBlurAsShadowB: boolean = false
  @State mUseShadowStyleB: boolean = false
  @State mShadowStyleB: ShadowStyle = ShadowStyle.OUTER_DEFAULT_LG
  @State mShadowControlParamB: ShadowControlParams =
    new ShadowControlParams(this.mShadowImageOpcityB, this.mUseBlurAsShadowB, this.mUseShadowStyleB, this.mShadowStyleB)
  @State mShadowOptionC: ShadowOptions = {
    radius: 20,
    color: Color.Green,
    offsetX: 20,
    offsetY: 20,
    fill: false,
    type: ShadowType.COLOR
  }
  @State mShadowImageOpcityC: number = 0.5
  @State mUseBlurAsShadowC: boolean = false
  @State mUseShadowStyleC: boolean = false
  @State mShadowStyleC: ShadowStyle = ShadowStyle.OUTER_DEFAULT_LG
  @State mShadowControlParamC: ShadowControlParams =
    new ShadowControlParams(this.mShadowImageOpcityC, this.mUseBlurAsShadowC, this.mUseShadowStyleC, this.mShadowStyleC)

  build() {
    NavDestination() {
      Column() {
        Image($r('app.media.test1'))
          .height(100)
          .shadow(this.mShadowControlParamA.useShadowStyle ? this.mShadowStyleA : this.mShadowOptionA)
          .opacity(this.mShadowControlParamA.shadowImageOpacity)
        Image($r('app.media.test1'))
          .height(100)
          .shadow(this.mShadowControlParamB.useShadowStyle ? this.mShadowStyleB : this.mShadowOptionB)
          .opacity(this.mShadowControlParamB.shadowImageOpacity)
        Image($r('app.media.test1'))
          .height(100)
          .shadow(this.mShadowControlParamC.useShadowStyle ? this.mShadowStyleC : this.mShadowOptionC)
          .opacity(this.mShadowControlParamC.shadowImageOpacity)

        Blank()
        Row() {
          Text('useShadowBatching:' + this.mUseShadowBatching)
          Toggle({ type: ToggleType.Switch, isOn: this.mUseShadowBatching })
            .selectedColor('#007DFF')
            .switchPointColor('#FFFFFF')
            .onChange((isOn: boolean) => {
              this.mUseShadowBatching = isOn
            })
        }

        Row() {
          Text('Shadow:')
          Row() {
            SegmentButton({
              options: this.shadowSingleSelectOptions,
              selectedIndexes: this.shadowSingleSelectIndex,
              onItemClicked: (value: number) => {
                this.mCurrentShadowIndex = value
              }
            }).height(20).width(200)
          }
        }

        Blank(5)
        Column() {
          if (this.mCurrentShadowIndex === 0) {
            ShadowControlView({ control: this.mShadowControlParamA, options: this.mShadowOptionA })
          } else if (this.mCurrentShadowIndex === 1) {
            ShadowControlView({ control: this.mShadowControlParamB, options: this.mShadowOptionB })
          } else {
            ShadowControlView({ control: this.mShadowControlParamC, options: this.mShadowOptionC })
          }
        }
      }
    }.hideTitleBar(true).width(200).height(20)
  }
}

class ShadowControlParams {
  public shadowImageOpacity: number = 0.5
  public useBlurAsShadow: boolean = false
  public useShadowStyle: boolean = false
  public shadowStyle: ShadowStyle = ShadowStyle.OUTER_DEFAULT_LG

  constructor(shadowImageOpacity: number, useBlurAsShadow: boolean, useShadowStyle: boolean, shadowStyle: ShadowStyle) {
    this.shadowImageOpacity = shadowImageOpacity
    this.useBlurAsShadow = useBlurAsShadow
    this.useShadowStyle = useShadowStyle
    this.shadowStyle = shadowStyle
  }
}

@Component
struct ShadowControlView {
  @Link control: ShadowControlParams
  @Link options: ShadowOptions

  build() {
    Column() {
      Row() {
        Text('radius:' + this.options.radius)
        Row() {
          Slider({
            value: this.options.radius as number,
            min: 0,
            max: 50,
            style: SliderStyle.OutSet
          }).onChange((value: number) => {
            this.options.radius = value
          })
        }.width('70%')
      }

      Blank(5)

      Row() {
        Text('offsetX:' + this.options.offsetX)
        Row() {
          Slider({
            value: this.options.offsetX as number,
            min: -500,
            max: 500,
            style: SliderStyle.OutSet
          }).onChange((value: number) => {
            this.options.offsetX = value
          })
        }.width('70%')
      }

      Blank(5)

      Row() {
        Text('offsetY:' + this.options.offsetY)
        Row() {
          Slider({
            value: this.options.offsetY as number,
            min: -500,
            max: 500,
            style: SliderStyle.OutSet
          }).onChange((value: number) => {
            this.options.offsetY = value
          })
        }.width('70%')
      }

      Blank(5)

      Row() {
        Text('useBlurAdShadow:' + this.control.useBlurAsShadow)
        Toggle({ type: ToggleType.Switch, isOn: this.control.useBlurAsShadow })
          .selectedColor('#007DFF')
          .switchPointColor('#FFFFFF')
          .onChange((isOn: boolean) => {
            this.control.useBlurAsShadow = isOn
            this.options.type = isOn ? ShadowType.BLUR : ShadowType.COLOR
          })
        Blank(10)
        Text('fill:' + this.options.fill)
        Toggle({ type: ToggleType.Switch, isOn: this.options.fill })
          .selectedColor('#007DFF')
          .switchPointColor('#FFFFFF')
          .onChange((isOn: boolean) => {
            this.options.fill = isOn
          })
      }

      Blank(5)

      Row() {
        Text('opacity:' + this.control.shadowImageOpacity)
        Row() {
          Slider({
            value: this.control.shadowImageOpacity * 10,
            min: 0,
            max: 10,
            style: SliderStyle.OutSet
          }).onChange((value: number) => {
            this.control.shadowImageOpacity = value / 10
          })
        }.width('70%')
      }

      Blank(20)
      Row() {
        Text('useStyle:' + this.control.useShadowStyle)
        Toggle({ type: ToggleType.Switch, isOn: this.control.useShadowStyle })
          .selectedColor('#007DFF')
          .switchPointColor('#FFFFFF')
          .onChange((isOn: boolean) => {
            this.control.useShadowStyle = isOn
          })
        Select([{ value: 'DEFAULT_XS' }, { value: 'DEFAULT_SM' }, { value: 'DEFAULT_MD' }, { value: 'DEFAULT_LG' },
          { value: 'FLOATING_SM' }, { value: 'FLOATING_MD' }])
          .selected(0)
          .value('DEFAULT_XS')
          .font({ size: 16, weight: 400 })
          .fontColor('#182431')
          .selectedOptionFont({ size: 16, weight: 400 })
          .optionFont({ size: 16, weight: 400 })
          .arrowPosition(ArrowPosition.END)
          .menuAlign(MenuAlignType.START, { dx: 0, dy: 0 })
          .optionWidth(200)
          .optionHeight(300)
          .onSelect((index: number) => {
            this.control.shadowStyle = shadowStyleList.get(index)
          })
      }
    }
  }
}
