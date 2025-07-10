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

#ifndef CJ_MATRIX_H
#define CJ_MATRIX_H

#include "cj_common_ffi.h"
#include "cj_drawing_util.h"
#include "ffi_remote_data.h"
#include "utils/matrix.h"

namespace OHOS {
namespace CJDrawing {

class CjMatrix : public OHOS::FFI::FFIData {
    DECL_TYPE(CjMatrix, OHOS::FFI::FFIData)
public:
    explicit CjMatrix(std::shared_ptr<Rosen::Drawing::Matrix> ptr) : m_matrix(ptr) {};
    virtual ~CjMatrix();
    int32_t SetRotation(scalar degree, scalar px, scalar py);
    int32_t SetScale(scalar sx, scalar sy, scalar px, scalar py);
    int32_t SetTranslation(scalar dx, scalar dy);
    int32_t SetSkew(scalar kx, scalar ky, scalar px, scalar py);
    int32_t SetConcat(CjMatrix& matrix1, CjMatrix& matrix2);
    int32_t SetSinCos(scalar sinValue, scalar cosValue, scalar px, scalar py);
    int32_t SetMatrix(const CArrFloat& values);
    int32_t SetMatrixByMatrix(CjMatrix& matrix);
    int32_t PostRotate(scalar degree, scalar px, scalar py);
    int32_t PostScale(scalar sx, scalar sy, scalar px, scalar py);
    int32_t PostTranslate(scalar dx, scalar dy);
    int32_t PostSkew(scalar kx, scalar ky, scalar px, scalar py);
    int32_t PostConcat(CjMatrix& matrix);
    int32_t PreRotate(scalar degree, scalar px, scalar py);
    int32_t PreScale(scalar sx, scalar sy, scalar px, scalar py);
    int32_t PreTranslate(scalar dx, scalar dy);
    int32_t PreSkew(scalar kx, scalar ky, scalar px, scalar py);
    int32_t PreConcat(CjMatrix& matrix);
    RetDataBool Invert(CjMatrix& matrix);
    int32_t Reset();
    RetDataBool IsEqual(CjMatrix& matrix);
    RetDataBool IsIdentity();
    RetDataBool IsAffine();
    RetDataBool RectStaysRect();
    RetDataScalar GetValue(int64_t index);
    int32_t GetAll(const CArrFloat& arr);
    RetDataScalar MapRadius(scalar radius);
    int32_t MapPoints(const CArrPoint& src, const CArrPoint& dst);
    RetDataBool MapRect(CRect& dst, const CRect& src);
    RetDataBool SetRectToRect(const CRect& src, const CRect& dst, int32_t scaleToFit);
    RetDataBool SetPolyToPoly(const CArrPoint& src, const CArrPoint& dst, int64_t count);

    std::shared_ptr<Rosen::Drawing::Matrix> GetMatrix();
    static int64_t CreateCjMatrix(int64_t id);

private:
    std::shared_ptr<Rosen::Drawing::Matrix> m_matrix = nullptr;
};

}  // CJDrawing
}  // OHOS
#endif  // CJ_MATRIX_H
