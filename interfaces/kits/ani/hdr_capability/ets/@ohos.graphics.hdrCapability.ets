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

export namespace hdrCapability {
  export enum HDRFormat {
    NONE = 0,
    VIDEO_HLG = 1,
    VIDEO_HDR10 = 2,
    VIDEO_HDR_VIVID = 3,
    IMAGE_HDR_VIVID_DUAL = 4,
    IMAGE_HDR_VIVID_SINGLE = 5,
    IMAGE_HDR_ISO_DUAL = 6,
    IMAGE_HDR_ISO_SINGLE = 7
  }
}
