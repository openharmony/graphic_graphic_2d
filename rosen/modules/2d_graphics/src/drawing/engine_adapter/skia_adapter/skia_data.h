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

#ifndef SKIA_DATA_H
#define SKIA_DATA_H

#include "include/core/SkData.h"
#include "impl_interface/data_impl.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
class DRAWING_API SkiaData : public DataImpl {
public:
    static inline constexpr AdapterType TYPE = AdapterType::SKIA_ADAPTER;

    SkiaData() noexcept;
    ~SkiaData() override {}

    AdapterType GetType() const override
    {
        return AdapterType::SKIA_ADAPTER;
    }

    bool BuildFromMalloc(const void* data, size_t length) override;
    bool BuildWithCopy(const void* data, size_t length) override;
    bool BuildWithProc(const void* ptr, size_t length, DataReleaseProc proc, void* ctx) override;
    bool BuildWithoutCopy(const void* data, size_t length) override;
    bool BuildUninitialized(size_t length) override;
    bool BuildEmpty() override;

    void* WritableData() override;
    size_t GetSize() const override;
    const void* GetData() const override;

    static std::shared_ptr<Data> MakeFromFileName(const char path[]);

    /*
     * @brief  Export Skia member variables for use by the adaptation layer.
     */
    sk_sp<SkData> GetSkData() const;

    /*
     * @brief  Update the member variable to filter, adaptation layer calls.
     */
    void SetSkData(const sk_sp<SkData>& data);

    std::shared_ptr<Data> Serialize() const override;
private:
    sk_sp<SkData> skData_;
};
} // Drawing
} // Rosen
} // OHOS

#endif
