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

#ifndef DATA_H
#define DATA_H

#include "impl_interface/data_impl.h"
#include "utils/drawing_macros.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
class DRAWING_API Data {
public:
    Data() noexcept;
    virtual ~Data() {};

    /**
     * @brief         Create a new Data from a pointer allocated by malloc. When Data is destroyed, data is released.
     * @param data    A pointer to data.
     * @param length  Length of data.
     * @return        If create Data successed, return true.
     */
    bool BuildFromMalloc(const void* data, size_t length);

    /**
     * @brief         Create a new Data by copying the specified data.
     * @param data    A pointer to data. It must not be nullptr.
     * @param length  Length of data.
     * @return        If create Data successed, return true.
     */
    bool BuildWithCopy(const void* data, size_t length);

    /**
     * @brief         Create a new dataref, taking the ptr as is,
                      and using the releaseproc to free it. The proc may be NULL.
     * @param ptr     A pointer to data. It must not be nullptr.
     * @param length  Length of data.
     * @param proc    release callback func.
     * @param ctx     context, usually nullptr.
     * @return        If create Data successed, return true.
     */
    bool BuildWithProc(const void* ptr, size_t length, DataReleaseProc proc, void* ctx);

    /**
     * @brief         Create a new Data. When Data is destroyed, data isn't released.
     * @param data    A pointer to data.
     * @param length  Length of data.
     * @return        If create Data successed, return true.
     */
    bool BuildWithoutCopy(const void* data, size_t length);

    /**
     * @brief:        Create a new Data with uninitialized contents.
     * @param length  Size of Data buffer.
     * @return        If create Data successed, return true.
     */
    bool BuildUninitialized(size_t length);

    bool BuildEmpty();

    /**
     * @brief   Gets a writable pointer to Data buffer.
     * @return  A writable pointer to Data buffer.
     */
    void* WritableData();

    /**
     * @brief   Gets length of Data buffer.
     * @return  Length of Data buffer.
     */
    size_t GetSize() const;

    /**
     * @brief   Gets a const pointer to Data buffer.
     * @return  A const pointer to Data buffer
     */
    const void* GetData() const;

    /**
     * @brief         Create a new data with the specified path.
     * @param length  The specified path.
     * @return        A shared pointer to Data.
     */
    static std::shared_ptr<Data> MakeFromFileName(const char path[]);

    /**
     * @brief   Get the adaptation layer instance, called in the adaptation layer.
     * @return  Adaptation Layer instance.
     */
    template<typename T>
    T* GetImpl() const
    {
        return impl_->DowncastingTo<T>();
    }

    std::shared_ptr<Data> Serialize() const;

private:
    std::shared_ptr<DataImpl> impl_;
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif
