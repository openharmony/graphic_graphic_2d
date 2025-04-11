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
export struct TestImagePage {
  private colorFilterMatrix: number[] = [1, 0, 1, 0, 1, 0, 0, 0, 1, 0, 1, 0, 1, 0, 0, 0, 1, 0, 1, 0]
  @State mRotateOption: RotateOptions = {
    x: 20,
    y: 0,
    z: 0,
    centerX: 0,
    centerY: 0,
    centerZ: 0,
    angle: 0
  }

  build() {
    NavDestination() {
      Column() {
        Image($r('app.media.test1')).width(100).height(100)
          .colorFilter(new ColorFilter(this.colorFilterMatrix))
          .rotate(this.mRotateOption)
        Row() {
          Text('x:' + this.mRotateOption.x)
          Slider({
            value: this.mRotateOption.x,
            min: -200,
            max: 200,
            style: SliderStyle.OutSet
          })
            .onChange((value: number) => {
              this.mRotateOption.x = value
            })
        }

        Row() {
          Text('y:' + this.mRotateOption.y)
          Slider({
            value: this.mRotateOption.y,
            min: -200,
            max: 200,
            style: SliderStyle.OutSet
          })
            .onChange((value: number) => {
              this.mRotateOption.y = value
            })
        }

        Row() {
          Text('z:' + this.mRotateOption.z)
          Slider({
            value: this.mRotateOption.z,
            min: -200,
            max: 200,
            style: SliderStyle.OutSet
          })
            .onChange((value: number) => {
              this.mRotateOption.z = value
            })
        }

        Row() {
          Text('cX:' + this.mRotateOption.centerX)
          Slider({
            value: this.mRotateOption.centerX as number,
            min: -200,
            max: 200,
            style: SliderStyle.OutSet
          })
            .onChange((value: number) => {
              this.mRotateOption.centerX = value
            })
        }

        Row() {
          Text('cY:' + this.mRotateOption.centerY)
          Slider({
            value: this.mRotateOption.centerY as number,
            min: -200,
            max: 200,
            style: SliderStyle.OutSet
          })
            .onChange((value: number) => {
              this.mRotateOption.centerY = value
            })
        }

        Row() {
          Text('cZ:' + this.mRotateOption.centerZ)
          Slider({
            value: this.mRotateOption.centerZ,
            min: -200,
            max: 200,
            style: SliderStyle.OutSet
          })
            .onChange((value: number) => {
              this.mRotateOption.centerZ = value
            })
        }

        Row() {
          Text('angle:' + this.mRotateOption.angle)
          Slider({
            value: this.mRotateOption.angle as number,
            min: -200,
            max: 200,
            style: SliderStyle.OutSet
          })
            .onChange((value: number) => {
              this.mRotateOption.angle = value
            })
        }

        Button('重置', { type: ButtonType.Capsule, stateEffect: true })
          .backgroundColor(0x317aff)
          .width(50)
          .height(50)
          .onClick(() => {
            this.mRotateOption = {
              x: 20,
              y: 0,
              z: 0,
              centerX: 0,
              centerY: 0,
              centerZ: 0,
              angle: 0
            }
          })
      }
    }
  }
}