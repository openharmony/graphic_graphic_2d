/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#ifndef FRAMEWORKS_SURFACE_INCLUDE_HDR_VIVID_METADATA_H
#define FRAMEWORKS_SURFACE_INCLUDE_HDR_VIVID_METADATA_H

namespace OHOS {
using HdrVividMetadataV1 = struct {
    unsigned int systemStartCode;
    unsigned int minimumMaxRgbPq;
    unsigned int averageMaxRgbPq;
    unsigned int varianceMaxRgbPq;
    unsigned int maximumMaxRgbPq;
    unsigned int toneMappingMode;
    unsigned int toneMappingParamNum;
    unsigned int targetedSystemDisplayMaximumLuminance[2];
    unsigned int baseFlag[4];
    unsigned int baseParamMp[2];
    unsigned int baseParamMm[2];
    unsigned int baseParamMa[2];
    unsigned int baseParamMb[2];
    unsigned int baseParamMn[2];
    unsigned int baseParamK1[2];
    unsigned int baseParamK2[2];
    unsigned int baseParamK3[2];
    unsigned int baseParamDeltaMode[2];
    unsigned int baseParamDelta[2];
    unsigned int threeSplineFlag[2];
    unsigned int threeSplineNum[2];
    unsigned int threeSplineThMode[2][4];
    unsigned int threeSplineThMb[2][4];
    unsigned int threeSplineTh[2][4][3];
    unsigned int threeSplineStrength[2][4];
    unsigned int colorSaturationMappingFlag;
    unsigned int colorSaturationNum;
    unsigned int colorSaturationGain[16];
};
} // namespace OHOS

#endif // FRAMEWORKS_SURFACE_INCLUDE_HDR_VIVID_METADATA_H