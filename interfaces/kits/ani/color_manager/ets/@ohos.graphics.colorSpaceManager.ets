/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

namespace colorSpaceManager {
  loadLibrary("colorspacemanager_ani");
  export enum ColorSpace {
    UNKNOWN = 0,
    ADOBE_RGB_1998 = 1,
    DCI_P3 = 2,
    DISPLAY_P3 = 3,
    SRGB = 4,
    BT709 = 6,
    BT601_EBU = 7,
    BT601_SMPTE_C = 8,
    BT2020_HLG = 9,
    BT2020_PQ = 10,
    P3_HLG = 11,
    P3_PQ = 12,
    ADOBE_RGB_1998_LIMIT = 13,
    DISPLAY_P3_LIMIT = 14,
    SRGB_LIMIT = 15,
    BT709_LIMIT = 16,
    BT601_EBU_LIMIT = 17,
    BT601_SMPTE_C_LIMIT = 18,
    BT2020_HLG_LIMIT = 19,
    BT2020_PQ_LIMIT = 20,
    P3_HLG_LIMIT = 21,
    P3_PQ_LIMIT = 22,
    LINEAR_P3 = 23,
    LINEAR_SRGB = 24,
    LINEAR_BT709 = 24,
    LINEAR_BT2020 = 25,
    DISPLAY_SRGB = 4,
    DISPLAY_P3_SRGB = 3,
    DISPLAY_P3_HLG = 11,
    DISPLAY_P3_PQ = 12,
    CUSTOM = 5
  }

  export interface ColorSpacePrimaries {
    redX: number;
    redY: number;
    greenX: number;
    greenY: number;
    blueX: number;
    blueY: number;
    whitePointX: number;
    whitePointY: number;
  }

  export interface ColorSpaceManager {
    getColorSpaceName(): ColorSpace;
    getWhitePoint(): Array<number>;
    getGamma(): number;
  }

  class ColorSpaceManagerInner implements ColorSpaceManager {
    static { loadLibrary("colorspacemanager_ani"); }
    private nativePtr: long = 0;

    constructor(nativePtr: long) {
      if (this.nativePtr == 0) {
        this.nativePtr = nativePtr;
      }
    }
    native getColorSpaceName(): ColorSpace
    native getWhitePoint(): Array<number>
    native getGamma(): number
  }
  
  native function createByColorSpace(colorSpaceName: ColorSpace): ColorSpaceManager;
  native function createByColorSpacePrimaries(primaries: ColorSpacePrimaries, gamma: number): ColorSpaceManager;

  export function create(colorSpaceName: ColorSpace) {
    return createByColorSpace(colorSpaceName);
  }
  export function create(primaries: ColorSpacePrimaries, gamma: number) {
    return createByColorSpacePrimaries(primaries, gamma);
  }
}

export default colorSpaceManager;
