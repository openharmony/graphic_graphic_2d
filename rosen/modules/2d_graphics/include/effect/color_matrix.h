/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#ifndef COLOR_MATRIX_H
#define COLOR_MATRIX_H

#include <memory>
#include <securec.h>
#include <string>

#include "utils/log.h"
#include "utils/scalar.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
class ColorMatrix {
public:
    ColorMatrix() noexcept
    {
        SetIdentity();
    }

    ~ColorMatrix() {}

    void SetIdentity()
    {
        for (size_t i = 0; i < sizeof(array_); i = i + 6) {
            array_[i] = 1;
        }
    }

    void SetArray(const scalar src[20])
    {
        auto ret = memcpy_s(array_, sizeof(array_), src, sizeof(array_));
        if (ret != EOK) {
            LOGE("Drawing：ColorMatrix memcpy_s failed");
        }
    }

    void GetArray(scalar dst[20]) const
    {
        auto ret = memcpy_s(dst, sizeof(array_), array_, sizeof(array_));
        if (ret != EOK) {
            LOGE("Drawing：ColorMatrix memcpy_s failed");
        }
    }

    void SetConcat(const ColorMatrix& m1, const ColorMatrix& m2)
    {
        scalar tmp[20] = { 0 };
        scalar* target;

        if (array_ == m1.array_ || array_ == m2.array_) {
            target = tmp;
        } else {
            target = array_;
        }

        int index = 0;
        for (int j = 0; j < 20; j = j + 5) {
            for (int i = 0; i < 4; i++) {
                target[index++] = m1.array_[j + 0] * m2.array_[i + 0] + m1.array_[j + 1] * m2.array_[i + 5] +
                    m1.array_[j + 2] * m2.array_[i + 10] + m1.array_[j + 3] * m2.array_[i + 15];
            }
            target[index++] = m1.array_[j + 0] * m2.array_[4] + m1.array_[j + 1] * m2.array_[9] +
                m1.array_[j + 2] * m2.array_[14] + m1.array_[j + 3] * m2.array_[19] + m1.array_[j + 4];
        }

        if (target != array_) {
            auto ret = memcpy_s(array_, sizeof(array_), target, sizeof(array_));
            if (ret != EOK) {
                LOGE("Drawing：ColorMatrix memcpy_s failed");
            }
        }
    }

    void PreConcat(const ColorMatrix& m)
    {
        SetConcat(*this, m);
    }

    void PostConcat(const ColorMatrix& m)
    {
        SetConcat(m, *this);
    }

    void SetScale(scalar sr, scalar sg, scalar sb, scalar sa)
    {
        auto ret = memset_s(array_, sizeof(array_), 0, sizeof(array_));
        if (ret != EOK) {
            LOGE("Drawing：ColorMatrix memset_s failed");
            return;
        }
        array_[0] = sr;
        array_[6] = sg;
        array_[12] = sb;
        array_[18] = sa;
    }

private:
    scalar array_[20] = { 0 };
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif