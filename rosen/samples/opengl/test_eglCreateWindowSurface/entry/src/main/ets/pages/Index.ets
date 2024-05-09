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

@Entry
@Component
struct Index {
  private xComponentContext: Record<string, () => void> = {};

  build() {
    Column() {
      Column() {
        XComponent({
          id: 'xcomponentId',
          type: 'surface',
          libraryname: 'nativerender',
        })
          .onLoad((xComponentContext?: object | Record<string, () => void>) => {
            if (xComponentContext) {
              this.xComponentContext = xComponentContext as Record<string, () => void>;
            }
          })
      }
      .margin({ top: 27, left: 12, right: 12 })
      .height('32.3%')

      Row() {
        Button("Draw Rectangle")
          .fontSize(16)
          .fontWeight(500)
          .margin({ bottom:24 })
          .onClick(() => {
            if (this.xComponentContext) {
              this.xComponentContext.drawRectangle();
            }
          })
          .width('53.6%')
          .height(40)
      }
      .width('100%')
      .justifyContent(FlexAlign.Center)
      .alignItems(VerticalAlign.Bottom)
      .layoutWeight(1)
    }
    .width('100%')
    .height('100%')
  }
}