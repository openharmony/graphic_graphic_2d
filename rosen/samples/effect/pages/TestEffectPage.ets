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

import { SegmentButton, SegmentButtonItemTuple, SegmentButtonOptions } from '@kit.ArkUI'

@Component
export struct TestEffectPage {
  @State mOpacity: number = 1
  @State mGrayScale: number = 0
  @State mBrightness: number = 1
  @State mSaturate: number = 1
  @State mContrast: number = 1
  @State mInvert: number = 0
  @State mSepia: number = 0
  @State mHueRotate: number = 0
  @State mColorBlend: Color | undefined = undefined
  @State singleSelectCapsuleOptions: SegmentButtonOptions = SegmentButtonOptions.capsule({
    buttons: [{ text: '无' }, { text: '红' }, { text: '黄' }, { text: '蓝' }, { text: '绿' }] as SegmentButtonItemTuple,
    multiply: false,
    backgroundBlurStyle: BlurStyle.BACKGROUND_THICK
  })
  @State singleSelectCapsuleSelectedIndexes: number[] = [0]

  build() {
    NavDestination() {
      Column() {
        Image($r('app.media.test1'))
          .height(100)
          .opacity(this.mOpacity)
          .grayscale(this.mGrayScale)
          .brightness(this.mBrightness)
          .saturate(this.mSaturate)
          .contrast(this.mContrast)
          .invert(this.mInvert)
          .sepia(this.mSepia)
          .hueRotate(this.mHueRotate)
          .colorBlend(this.mColorBlend)

        Column() {
          Text('opacity:' + this.mOpacity)
          Row() {
            Slider({ value: this.mOpacity * 10, min: 0, max: 10, style: SliderStyle.OutSet })
              .onChange((value: number) => {
                this.mOpacity = value / 10
              }).width('80%')
            Blank().width('5%')
            Image($r('app.media.app_icon')).width('8%').onClick(()=> {
              this.mOpacity = 1
            })
          }
        }

        Column() {
          Text('grayscale:' + this.mGrayScale)
          Row() {
            Slider({ value: this.mGrayScale * 10, min: 0, max: 10, style: SliderStyle.OutSet })
              .onChange((value: number) => {
                this.mGrayScale = value / 10
              }).width('80%')
            Blank().width('5%')
            Image($r('app.media.app_icon')).width('8%').onClick(()=> {
              this.mGrayScale = 1
            })
          }
        }

        Column() {
          Text('brightness:' + this.mBrightness)
          Row() {
            Slider({ value: this.mBrightness * 10, min: 0, max: 30, style: SliderStyle.OutSet })
              .onChange((value: number) => {
                this.mBrightness = value / 10
              }).width('80%')
            Blank().width('5%')
            Image($r('app.media.app_icon')).width('8%').onClick(()=> {
              this.mBrightness = 1
            })
          }
        }

        Column() {
          Text('saturate:' + this.mSaturate)
          Row() {
            Slider({ value: this.mSaturate, min: 0, max: 60, style: SliderStyle.OutSet })
              .onChange((value: number) => {
                this.mSaturate = value
              }).width('80%')
            Blank().width('5%')
            Image($r('app.media.app_icon')).width('8%').onClick(()=> {
              this.mSaturate = 1
            })
          }
        }

        Column() {
          Text('contrast:' + this.mContrast)
          Row() {
            Slider({ value: this.mContrast, min: 0, max: 20, style: SliderStyle.OutSet })
              .onChange((value: number) => {
                this.mContrast = value
              }).width('80%')
            Blank().width('5%')
            Image($r('app.media.app_icon')).width('8%').onClick(()=> {
              this.mContrast = 1
            })
          }
        }

        Column() {
          Text('invert:' + this.mInvert)
          Row() {
            Slider({ value: this.mInvert * 10, min: 0, max: 10, style: SliderStyle.OutSet })
              .onChange((value: number) => {
                this.mInvert = value / 10
              }).width('80%')
            Blank().width('5%')
            Image($r('app.media.app_icon')).width('8%').onClick(()=> {
              this.mInvert = 0
            })
          }
        }

        Column() {
          Text('sepia:' + this.mSepia)
          Row() {
            Slider({ value: this.mSepia * 10, min: 0, max: 10, style: SliderStyle.OutSet })
              .onChange((value: number) => {
                this.mSepia = value / 10
              }).width('80%')
            Blank().width('5%')
            Image($r('app.media.app_icon')).width('8%').onClick(()=> {
              this.mSepia = 0
            })
          }
        }

        Column() {
          Text('invert:' + this.mHueRotate)
          Row() {
            Slider({ value: this.mHueRotate, min: -360, max: 360, style: SliderStyle.OutSet })
              .onChange((value: number) => {
                this.mHueRotate = value
              }).width('80%')
            Blank().width('5%')
            Image($r('app.media.app_icon')).width('8%').onClick(()=> {
              this.mHueRotate = 0
            })
          }
        }

        Column() {
          Text('colorblend:' + this.mColorBlend)
          Row() {
            SegmentButton({ options: this.singleSelectCapsuleOptions,
              selectedIndexes: this.singleSelectCapsuleSelectedIndexes,
              onItemClicked: (value: number)=> {
              switch (value) {
                case 0:
                  {
                    this.mColorBlend = undefined
                    break;
                  }
                case 1:
                  {
                    this.mColorBlend = Color.Red
                    break;
                  }
                case 2:
                  {
                    this.mColorBlend = Color.Yellow
                    break;
                  }
                case 3:
                  {
                    this.mColorBlend = Color.Blue
                    break;
                  }
                case 4:
                  {
                    this.mColorBlend = Color.Green
                    break;
                  }
              }
              }})
          }.width('80%')
        }
      }
    }.height('100%').width('100%').hideTitleBar(true)
  }
}