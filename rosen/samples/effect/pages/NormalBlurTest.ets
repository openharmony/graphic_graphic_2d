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
 
@Component
export struct NormalBlurTest {
  @State mFgEffectBlurRadius: number = 5
  @State mBackdropBlurRadius: number = 5
  @State mBlurRadius: number = 5
  @State mImageBlurRadius: number = 5

  build() {
    NavDestination() {
      Column() {
        Image($r('app.media.test1')).width('50%').blur(this.mImageBlurRadius).borderColor(Color.Black).borderWidth(1)
        Text('image blur:' + this.mImageBlurRadius)
        Row() {
          Slider({
            value: this.mImageBlurRadius,
            min: 0,
            max: 30,
            style: SliderStyle.OutSet
          }).onChange((value: number) => {
            this.mImageBlurRadius = value
          })
        }.width('80%')

        Blank()
        Text('内容模糊 .blur')
          .fontColor(Color.Red)
          .fontSize(10)
          .backgroundImage($r('app.media.test1'), ImageRepeat.NoRepeat)
          .backgroundImageSize(ImageSize.Contain)
          .width('50%')
          .height(120)
          .blur(this.mBlurRadius)
          .borderColor(Color.Black)
          .borderWidth(1)
        Text('text blur:' + this.mBlurRadius)
        Row() {
          Slider({
            value: this.mBlurRadius,
            min: 0,
            max: 30,
            style: SliderStyle.OutSet
          }).onChange((value: number) => {
            this.mBlurRadius = value
          })
        }.width('80%')

        Blank()
        Text('背景模糊 .backdropBlur')
          .fontColor(Color.Red)
          .fontSize(10)
          .backgroundImage($r('app.media.test1'), ImageRepeat.NoRepeat)
          .backgroundImageSize(ImageSize.Contain)
          .width('50%')
          .height(120)
          .backdropBlur(this.mBackdropBlurRadius)
          .borderColor(Color.Black)
          .borderWidth(1)
        Text('text backdropBlur:' + this.mBackdropBlurRadius)
        Row() {
          Slider({
            value: this.mBackdropBlurRadius,
            min: 0,
            max: 30,
            style: SliderStyle.OutSet
          }).onChange((value: number) => {
            this.mBackdropBlurRadius = value
          })
        }.width('80%')

        Blank()
        Text('前景模糊 .foregroundEffect')
          .fontColor(Color.Red)
          .fontSize(10)
          .backgroundImage($r('app.media.test1'), ImageRepeat.NoRepeat)
          .backgroundImageSize(ImageSize.Contain)
          .width('50%')
          .height(120)
          .foregroundEffect({ radius: this.mFgEffectBlurRadius })
          .borderColor(Color.Black)
          .borderWidth(1)
        Text('text foregroundEffect:' + this.mFgEffectBlurRadius)
        Row() {
          Slider({
            value: this.mFgEffectBlurRadius,
            min: 0,
            max: 30,
            style: SliderStyle.OutSet
          }).onChange((value: number) => {
            this.mFgEffectBlurRadius = value
          })
        }.width('80%')
      }
    }.hideTitleBar(true)
  }
}