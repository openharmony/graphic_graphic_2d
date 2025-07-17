/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "cj_common_ffi.h"
#include "cj_drawing_log.h"
#include "cj_drawing_util.h"
#include "matrix_impl/cj_matrix.h"

namespace OHOS {
namespace CJDrawing {
using namespace OHOS::FFI;
extern "C" {
// Matrix
FFI_EXPORT int64_t FfiGraphicDrawingMatrixCreate(int64_t id)
{
    ROSEN_LOGD("FfiGraphicDrawingMatrixCreate start");
    return CjMatrix::CreateCjMatrix(id);
}

FFI_EXPORT int32_t FfiGraphicDrawingMatrixSetRotation(int64_t id, float degree, float px, float py)
{
    ROSEN_LOGD("FfiGraphicDrawingMatrixSetRotation start");
    auto instance = FFIData::GetData<CjMatrix>(id);
    if (instance == nullptr) {
        return ParserErr(CjDrawingErrorCode::INTERNAL_ERROR);
    }
    return instance->SetRotation(degree, px, py);
}

FFI_EXPORT int32_t FfiGraphicDrawingMatrixSetScale(int64_t id, float sx, float sy, float px, float py)
{
    ROSEN_LOGD("FfiGraphicDrawingMatrixSetScale start");
    auto instance = FFIData::GetData<CjMatrix>(id);
    if (instance == nullptr) {
        return ParserErr(CjDrawingErrorCode::INTERNAL_ERROR);
    }
    return instance->SetScale(sx, sy, px, py);
}

FFI_EXPORT int32_t FfiGraphicDrawingMatrixSetTranslation(int64_t id, float dx, float dy)
{
    ROSEN_LOGD("FfiGraphicDrawingMatrixSetTranslation start");
    auto instance = FFIData::GetData<CjMatrix>(id);
    if (instance == nullptr) {
        return ParserErr(CjDrawingErrorCode::INTERNAL_ERROR);
    }
    return instance->SetTranslation(dx, dy);
}

FFI_EXPORT int32_t FfiGraphicDrawingMatrixSetSkew(int64_t id, float kx, float ky, float px, float py)
{
    ROSEN_LOGD("FfiGraphicDrawingMatrixSetSkew start");
    auto instance = FFIData::GetData<CjMatrix>(id);
    if (instance == nullptr) {
        return ParserErr(CjDrawingErrorCode::INTERNAL_ERROR);
    }
    return instance->SetSkew(kx, ky, px, py);
}

FFI_EXPORT int32_t FfiGraphicDrawingMatrixSetConcat(int64_t id, int64_t matrix1, int64_t matrix2)
{
    ROSEN_LOGD("FfiGraphicDrawingMatrixSetConcat start");
    auto instance = FFIData::GetData<CjMatrix>(id);
    if (instance == nullptr) {
        return ParserErr(CjDrawingErrorCode::INTERNAL_ERROR);
    }
    auto cjMatrix1 = FFIData::GetData<CjMatrix>(matrix1);
    if (cjMatrix1 == nullptr) {
        return ParserErr(CjDrawingErrorCode::ERROR_PARAM_VERIFICATION_FAILED);
    }
    auto cjMatrix2 = FFIData::GetData<CjMatrix>(matrix2);
    if (cjMatrix2 == nullptr) {
        return ParserErr(CjDrawingErrorCode::ERROR_PARAM_VERIFICATION_FAILED);
    }
    return instance->SetConcat(*cjMatrix1, *cjMatrix2);
}

FFI_EXPORT int32_t FfiGraphicDrawingMatrixSetSinCos(int64_t id, float sinValue, float cosValue, float px, float py)
{
    ROSEN_LOGD("FfiGraphicDrawingMatrixSetSinCos start");
    auto instance = FFIData::GetData<CjMatrix>(id);
    if (instance == nullptr) {
        return ParserErr(CjDrawingErrorCode::INTERNAL_ERROR);
    }
    return instance->SetSinCos(sinValue, cosValue, px, py);
}

FFI_EXPORT int32_t FfiGraphicDrawingMatrixSetMatrix(int64_t id, CArrFloat values)
{
    ROSEN_LOGD("FfiGraphicDrawingMatrixSetMatrix start");
    auto instance = FFIData::GetData<CjMatrix>(id);
    if (instance == nullptr) {
        return ParserErr(CjDrawingErrorCode::INTERNAL_ERROR);
    }
    return instance->SetMatrix(values);
}

FFI_EXPORT int32_t FfiGraphicDrawingMatrixSetMatrixByMatrix(int64_t id, int64_t matrix)
{
    ROSEN_LOGD("FfiGraphicDrawingMatrixSetMatrixByMatrix start");
    auto instance = FFIData::GetData<CjMatrix>(id);
    if (instance == nullptr) {
        return ParserErr(CjDrawingErrorCode::INTERNAL_ERROR);
    }
    auto cjMatrix = FFIData::GetData<CjMatrix>(matrix);
    if (cjMatrix == nullptr) {
        return ParserErr(CjDrawingErrorCode::ERROR_PARAM_VERIFICATION_FAILED);
    }
    return instance->SetMatrixByMatrix(*cjMatrix);
}

FFI_EXPORT int32_t FfiGraphicDrawingMatrixPostRotate(int64_t id, float degree, float px, float py)
{
    ROSEN_LOGD("FfiGraphicDrawingMatrixPostRotate start");
    auto instance = FFIData::GetData<CjMatrix>(id);
    if (instance == nullptr) {
        return ParserErr(CjDrawingErrorCode::INTERNAL_ERROR);
    }
    return instance->PostRotate(degree, px, py);
}

FFI_EXPORT int32_t FfiGraphicDrawingMatrixPostScale(int64_t id, float sx, float sy, float px, float py)
{
    ROSEN_LOGD("FfiGraphicDrawingMatrixPostScale start");
    auto instance = FFIData::GetData<CjMatrix>(id);
    if (instance == nullptr) {
        return ParserErr(CjDrawingErrorCode::INTERNAL_ERROR);
    }
    return instance->PostScale(sx, sy, px, py);
}

FFI_EXPORT int32_t FfiGraphicDrawingMatrixPostTranslate(int64_t id, float dx, float dy)
{
    ROSEN_LOGD("FfiGraphicDrawingMatrixPostTranslate start");
    auto instance = FFIData::GetData<CjMatrix>(id);
    if (instance == nullptr) {
        return ParserErr(CjDrawingErrorCode::INTERNAL_ERROR);
    }
    return instance->PostTranslate(dx, dy);
}

FFI_EXPORT int32_t FfiGraphicDrawingMatrixPostSkew(int64_t id, float kx, float ky, float px, float py)
{
    ROSEN_LOGD("FfiGraphicDrawingMatrixPostSkew start");
    auto instance = FFIData::GetData<CjMatrix>(id);
    if (instance == nullptr) {
        return ParserErr(CjDrawingErrorCode::INTERNAL_ERROR);
    }
    return instance->PostSkew(kx, ky, px, py);
}

FFI_EXPORT int32_t FfiGraphicDrawingMatrixPostConcat(int64_t id, int64_t matrix)
{
    ROSEN_LOGD("FfiGraphicDrawingMatrixPostConcat start");
    auto instance = FFIData::GetData<CjMatrix>(id);
    if (instance == nullptr) {
        return ParserErr(CjDrawingErrorCode::INTERNAL_ERROR);
    }
    auto cjMatrix = FFIData::GetData<CjMatrix>(matrix);
    if (cjMatrix == nullptr) {
        return ParserErr(CjDrawingErrorCode::ERROR_PARAM_VERIFICATION_FAILED);
    }
    return instance->PostConcat(*cjMatrix);
}

FFI_EXPORT int32_t FfiGraphicDrawingMatrixPreRotate(int64_t id, float degree, float px, float py)
{
    ROSEN_LOGD("FfiGraphicDrawingMatrixPreRotate start");
    auto instance = FFIData::GetData<CjMatrix>(id);
    if (instance == nullptr) {
        return ParserErr(CjDrawingErrorCode::INTERNAL_ERROR);
    }
    return instance->PreRotate(degree, px, py);
}

FFI_EXPORT int32_t FfiGraphicDrawingMatrixPreScale(int64_t id, float sx, float sy, float px, float py)
{
    ROSEN_LOGD("FfiGraphicDrawingMatrixPreScale start");
    auto instance = FFIData::GetData<CjMatrix>(id);
    if (instance == nullptr) {
        return ParserErr(CjDrawingErrorCode::INTERNAL_ERROR);
    }
    return instance->PreScale(sx, sy, px, py);
}

FFI_EXPORT int32_t FfiGraphicDrawingMatrixPreTranslate(int64_t id, float dx, float dy)
{
    ROSEN_LOGD("FfiGraphicDrawingMatrixPreTranslate start");
    auto instance = FFIData::GetData<CjMatrix>(id);
    if (instance == nullptr) {
        return ParserErr(CjDrawingErrorCode::INTERNAL_ERROR);
    }
    return instance->PreTranslate(dx, dy);
}

FFI_EXPORT int32_t FfiGraphicDrawingMatrixPreSkew(int64_t id, float kx, float ky, float px, float py)
{
    ROSEN_LOGD("FfiGraphicDrawingMatrixPreSkew start");
    auto instance = FFIData::GetData<CjMatrix>(id);
    if (instance == nullptr) {
        return ParserErr(CjDrawingErrorCode::INTERNAL_ERROR);
    }
    return instance->PreSkew(kx, ky, px, py);
}

FFI_EXPORT int32_t FfiGraphicDrawingMatrixPreConcat(int64_t id, int64_t matrix)
{
    ROSEN_LOGD("FfiGraphicDrawingMatrixPreConcat start");
    auto instance = FFIData::GetData<CjMatrix>(id);
    if (instance == nullptr) {
        return ParserErr(CjDrawingErrorCode::INTERNAL_ERROR);
    }
    auto cjMatrix = FFIData::GetData<CjMatrix>(matrix);
    if (cjMatrix == nullptr) {
        return ParserErr(CjDrawingErrorCode::ERROR_PARAM_VERIFICATION_FAILED);
    }
    return instance->PreConcat(*cjMatrix);
}

FFI_EXPORT RetDataBool FfiGraphicDrawingMatrixInvert(int64_t id, int64_t matrix)
{
    ROSEN_LOGD("FfiGraphicDrawingMatrixInvert start");
    auto instance = FFIData::GetData<CjMatrix>(id);
    if (instance == nullptr) {
        return {.code = ParserErr(CjDrawingErrorCode::INTERNAL_ERROR), .data = false};
    }
    auto cjMatrix = FFIData::GetData<CjMatrix>(matrix);
    if (cjMatrix == nullptr) {
        return {.code = ParserErr(CjDrawingErrorCode::ERROR_PARAM_VERIFICATION_FAILED), .data = false};
    }
    return instance->Invert(*cjMatrix);
}

FFI_EXPORT int32_t FfiGraphicDrawingMatrixReset(int64_t id)
{
    ROSEN_LOGD("FfiGraphicDrawingMatrixReset start");
    auto instance = FFIData::GetData<CjMatrix>(id);
    if (instance == nullptr) {
        return ParserErr(CjDrawingErrorCode::INTERNAL_ERROR);
    }
    return instance->Reset();
}

FFI_EXPORT RetDataBool FfiGraphicDrawingMatrixIsEqual(int64_t id, int64_t matrix)
{
    ROSEN_LOGD("FfiGraphicDrawingMatrixIsEqual start");
    auto instance = FFIData::GetData<CjMatrix>(id);
    if (instance == nullptr) {
        return {.code = ParserErr(CjDrawingErrorCode::INTERNAL_ERROR), .data = false};
    }
    auto cjMatrix = FFIData::GetData<CjMatrix>(matrix);
    if (cjMatrix == nullptr) {
        return {.code = ParserErr(CjDrawingErrorCode::ERROR_PARAM_VERIFICATION_FAILED), .data = false};
    }
    return instance->IsEqual(*cjMatrix);
}

FFI_EXPORT RetDataBool FfiGraphicDrawingMatrixIsIdentity(int64_t id)
{
    ROSEN_LOGD("FfiGraphicDrawingMatrixIsIdentity start");
    auto instance = FFIData::GetData<CjMatrix>(id);
    if (instance == nullptr) {
        return {.code = ParserErr(CjDrawingErrorCode::INTERNAL_ERROR), .data = false};
    }
    return instance->IsIdentity();
}

FFI_EXPORT RetDataBool FfiGraphicDrawingMatrixIsAffine(int64_t id)
{
    ROSEN_LOGD("FfiGraphicDrawingMatrixIsAffine start");
    auto instance = FFIData::GetData<CjMatrix>(id);
    if (instance == nullptr) {
        return {.code = ParserErr(CjDrawingErrorCode::INTERNAL_ERROR), .data = false};
    }
    return instance->IsAffine();
}

FFI_EXPORT RetDataBool FfiGraphicDrawingMatrixRectStaysRect(int64_t id)
{
    ROSEN_LOGD("FfiGraphicDrawingMatrixRectStaysRect start");
    auto instance = FFIData::GetData<CjMatrix>(id);
    if (instance == nullptr) {
        return {.code = ParserErr(CjDrawingErrorCode::INTERNAL_ERROR), .data = false};
    }
    return instance->RectStaysRect();
}

FFI_EXPORT RetDataScalar FfiGraphicDrawingMatrixGetValue(int64_t id, int64_t index)
{
    ROSEN_LOGD("FfiGraphicDrawingMatrixGetValue start");
    auto instance = FFIData::GetData<CjMatrix>(id);
    if (instance == nullptr) {
        return {.code = ParserErr(CjDrawingErrorCode::INTERNAL_ERROR), .data = 0.0};
    }
    return instance->GetValue(index);
}

FFI_EXPORT int32_t FfiGraphicDrawingMatrixGetAll(int64_t id, CArrFloat arr)
{
    ROSEN_LOGD("FfiGraphicDrawingMatrixGetAll start");
    auto instance = FFIData::GetData<CjMatrix>(id);
    if (instance == nullptr) {
        return ParserErr(CjDrawingErrorCode::INTERNAL_ERROR);
    }
    return instance->GetAll(arr);
}

FFI_EXPORT RetDataScalar FfiGraphicDrawingMatrixMapRadius(int64_t id, float radius)
{
    ROSEN_LOGD("FfiGraphicDrawingMatrixMapRadius start");
    auto instance = FFIData::GetData<CjMatrix>(id);
    if (instance == nullptr) {
        return {.code = ParserErr(CjDrawingErrorCode::INTERNAL_ERROR), .data = 0.0};
    }
    return instance->MapRadius(radius);
}

FFI_EXPORT int32_t FfiGraphicDrawingMatrixMapPoints(int64_t id, CArrPoint src, CArrPoint dst)
{
    ROSEN_LOGD("FfiGraphicDrawingMatrixMapPoints start");
    auto instance = FFIData::GetData<CjMatrix>(id);
    if (instance == nullptr) {
        return ParserErr(CjDrawingErrorCode::INTERNAL_ERROR);
    }
    return instance->MapPoints(src, dst);
}

FFI_EXPORT RetDataBool FfiGraphicDrawingMatrixMapRect(int64_t id, CRect *dst, CRect src)
{
    ROSEN_LOGD("FfiGraphicDrawingMatrixMapRect start");
    auto instance = FFIData::GetData<CjMatrix>(id);
    if (instance == nullptr) {
        return {.code = ParserErr(CjDrawingErrorCode::INTERNAL_ERROR), .data = false};
    }
    return instance->MapRect(*dst, src);
}

FFI_EXPORT RetDataBool FfiGraphicDrawingMatrixSetRectToRect(int64_t id, CRect src, CRect dst, int32_t scaleToFit)
{
    ROSEN_LOGD("FfiGraphicDrawingMatrixSetRectToRect start");
    auto instance = FFIData::GetData<CjMatrix>(id);
    if (instance == nullptr) {
        return {.code = ParserErr(CjDrawingErrorCode::INTERNAL_ERROR), .data = false};
    }
    return instance->SetRectToRect(src, dst, scaleToFit);
}

FFI_EXPORT RetDataBool FfiGraphicDrawingMatrixSetPolyToPoly(int64_t id, CArrPoint src, CArrPoint dst, int64_t count)
{
    ROSEN_LOGD("FfiGraphicDrawingMatrixSetPolyToPoly start");
    auto instance = FFIData::GetData<CjMatrix>(id);
    if (instance == nullptr) {
        return {.code = ParserErr(CjDrawingErrorCode::INTERNAL_ERROR), .data = false};
    }
    return instance->SetPolyToPoly(src, dst, count);
}
}
}  // namespace CJDrawing
}  // namespace OHOS
