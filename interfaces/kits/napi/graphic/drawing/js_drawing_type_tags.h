/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#ifndef OHOS_JS_DRAWING_TYPE_TAGS_H
#define OHOS_JS_DRAWING_TYPE_TAGS_H


namespace OHOS::Rosen::Drawing {

constexpr napi_type_tag BRUSH_TYPE_TAG = {
    .lower = 0x6d26e173007038bf,
    .upper = 0x0b874c14dadbcf94,
};
constexpr napi_type_tag CANVAS_TYPE_TAG = {
    .lower = 0x2710bc10cdee8db3,
    .upper = 0xc5189f88510eadc1,
};
constexpr napi_type_tag COLOR_FILTER_TYPE_TAG = {
    .lower = 0xee0530d6eedfac84,
    .upper = 0xac039ac872495a71,
};
constexpr napi_type_tag FONT_TYPE_TAG = {
    .lower = 0x523ac70fd96d0639,
    .upper = 0xf405574ba6af063f,
};
constexpr napi_type_tag IMAGE_FILTER_TYPE_TAG = {
    .lower = 0x43bced7e6850d12a,
    .upper = 0x102733d6f3ba7524,
};
constexpr napi_type_tag LATTICE_TYPE_TAG = {
    .lower = 0xa27eec3e3bc81b13,
    .upper = 0x0f190977135a2dec,
};
constexpr napi_type_tag MASK_FILTER_TYPE_TAG = {
    .lower = 0x01d4f7a1fc402bb4,
    .upper = 0x65219a3bf8a64a34,
};
constexpr napi_type_tag MATRIX_TYPE_TAG = {
    .lower = 0x6ac09c93398ca526,
    .upper = 0x0532ce389092b72d,
};
constexpr napi_type_tag PATH_EFFECT_TYPE_TAG = {
    .lower = 0xc22808a8d2af3ad1,
    .upper = 0x9e6907d60d31e157,
};
constexpr napi_type_tag PATH_ITERATOR_TYPE_TAG = {
    .lower = 0x0c7e9574a06dd566,
    .upper = 0x44ae93b0b2bf78d2,
};
constexpr napi_type_tag PATH_TYPE_TAG = {
    .lower = 0xe5a5ba02df062ac9,
    .upper = 0x287eb7348c3e0c4b,
};
constexpr napi_type_tag PEN_TYPE_TAG = {
    .lower = 0xc1886ea1d41954a8,
    .upper = 0x6775cb211a94d53f,
};
constexpr napi_type_tag POINT_UTILS_TYPE_TAG = {
    .lower = 0xc83f4354505b2fc4,
    .upper = 0x5d0765bddfbdafea,
};
constexpr napi_type_tag RECT_UTILS_TYPE_TAG = {
    .lower = 0xe79040cebf7111a4,
    .upper = 0x4c955f999396025f,
};
constexpr napi_type_tag REGION_TYPE_TAG = {
    .lower = 0x02ec15d221e29183,
    .upper = 0xef4a87dbd47823b3,
};
constexpr napi_type_tag ROUND_RECT_TYPE_TAG = {
    .lower = 0x5fafee3267f7c2c2,
    .upper = 0xd5235f76f012defc,
};
constexpr napi_type_tag SAMPLING_OPTIONS_TYPE_TAG = {
    .lower = 0x66ef2f590d0c3bd6,
    .upper = 0x1a7dbf3509901f0d,
};
constexpr napi_type_tag SHADER_EFFECT_TYPE_TAG = {
    .lower = 0x686c88dea6940c90,
    .upper = 0x481c2a856e6047b8,
};
constexpr napi_type_tag SHADOW_LAYER_TYPE_TAG = {
    .lower = 0x3022de31a0934f83,
    .upper = 0x48ef442d72112d8c,
};
constexpr napi_type_tag TEXT_BLOB_TYPE_TAG = {
    .lower = 0x689a3ad720150120,
    .upper = 0xcb4b4a03b74a0feb,
};
constexpr napi_type_tag TOOL_TYPE_TAG = {
    .lower = 0xb19ed638c2b527c0,
    .upper = 0x22ce6f42f0155c61,
};
constexpr napi_type_tag TYPEFACE_ARGUMENTS_TYPE_TAG = {
    .lower = 0xd64bf25046db9bc6,
    .upper = 0xc7bad98a7d258836,
};
constexpr napi_type_tag TYPEFACE_TYPE_TAG = {
    .lower = 0xda1f72db5f99324d,
    .upper = 0xaf4d91b715a76ca2,
};

} // namespace OHOS::Rosen::Drawing
#endif // OHOS_JS_DRAWING_TYPE_TAGS_H
