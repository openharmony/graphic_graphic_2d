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

import { LinearGradientBlurTest } from './LinearGradientBlurTest'
import { MaterialBlurTest } from './MaterialBlurTest'
import { NormalBlurTest } from './NormalBlurTest'
import { TestBlendPage } from './TestBlendPage'
import { TestBlurPage } from './TestBlurPage'
import { TestEffectPage } from './TestEffectPage'
import { TestImagePage } from './TestImagePage'
import { TestShadowPage } from './TestShadowPage'

@Entry
@Component
struct Index {
  @Provide
  private stack: NavPathStack = new NavPathStack()

  @Builder
  MyPageMap(name: string) {
    if (name === 'TestShadowPage') {
      TestShadowPage()
    } else if (name === 'TestEffectPage') {
      TestEffectPage()
    } else if (name === 'TestBlurPage') {
      TestBlurPage()
    } else if (name === 'TestBlendPage') {
      TestBlendPage()
    } else if (name === 'TestImagePage') {
      TestImagePage()
    } else if (name === 'MaterialBlurTest') {
      MaterialBlurTest()
    } else if (name === 'NormalBlurTest') {
      NormalBlurTest()
    } else if (name === 'LinearGradientBlurTest') {
      LinearGradientBlurTest()
    }
  }
  build() {
    Navigation(this.stack) {
      Column() {
        Button('Test Shadow').onClick(()=> {
          this.stack.pushPath( { name : 'TestShadowPage'})
        })
        Blank()
        Button('Test Effect').onClick(()=> {
          this.stack.pushPath( { name : 'TestEffectPage'})
        })
        Blank()
        Button('Test Blur').onClick(()=> {
          this.stack.pushPath( { name : 'TestBlurPage'})
        })
        Blank()
        Button('Test Blend').onClick(()=> {
          this.stack.pushPath( { name : 'TestBlendPage'})
        })
        Blank()
        Button('Test Image').onClick(()=> {
          this.stack.pushPath( { name : 'TestImagePage'})
        })
      }
    }
  }
}