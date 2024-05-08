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
import common from '@ohos.app.ability.common'
import image from '@ohos.multimedia.image'

export default class globalThis {
  private constructor() {}
  private static instance:globalThis
  private _uiContexts = new Map<string, common.UIAbilityContext>()
  private _pixelMap = new Map<string, image.PixelMap>()

  public static getInstance(): globalThis {
    if (!globalThis.instance) {
      globalThis.instance = new globalThis()
    }
    return globalThis.instance
  }

  getContext(key: string): common.UIAbilityContext | undefined {
    return this._uiContexts.get(key)
  }

  setContext(key: string, value: common.UIAbilityContext): void {
    this._uiContexts.set(key, value)
  }

  setPixelMap(key: string, pixelMap: image.PixelMap): void {
    this._pixelMap.set(key, pixelMap)
  }

  getPixelMap(key: string): image.PixelMap {
    return this._pixelMap.get(key)
  }
}