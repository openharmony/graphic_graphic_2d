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

namespace OHOS {
namespace Rosen {
namespace Drawing {
class Data {
public:
    Data() noexcept;
    virtual ~Data() {};

    /*
     * @brief         Create a new Data from a pointer allocated by malloc. When Data is destroyed, data is released.
     * @param data    A pointer to data.
     * @param length  Length of data.
     * @return        If create Data successed, return true.
     */
    bool BuildFromMalloc(const void* data, size_t length);

    /*
     * @brief         Create a new Data by copying the specified data.
     * @param data    A pointer to data. It must not be nullptr.
     * @param length  Length of data.
     * @return        If create Data successed, return true.
     */
    bool BuildWithCopy(const void* data, size_t length);

    /*
     * @brief         Create a new Data. When Data is destroyed, data isn't released.
     * @param data    A pointer to data.
     * @param length  Length of data.
     * @return        If create Data successed, return true.
     */
    bool BuildWithoutCopy(const void* data, size_t length);

    /*
     * @brief:        Create a new Data with uninitialized contents.
     * @param length  Size of Data buffer.
     * @return        If create Data successed, return true.
     */
    bool BuildUninitialized(size_t length);

    /*
     * @brief   Gets a writable pointer to Data buffer.
     * @return  A writable pointer to Data buffer.
     */
    void* WritableData();

    /*
     * @brief   Gets length of Data buffer.
     * @return  Length of Data buffer.
     */
    size_t GetSize() const;

    /*
     * @brief   Gets a const pointer to Data buffer.
     * @return  A const pointer to Data buffer
     */
    const void* GetData() const;

    /*
     * @brief   Get the adaptation layer instance, called in the adaptation layer.
     * @return  Adaptation Layer instance.
     */
    template<typename T>
    const std::shared_ptr<T> GetImpl() const
    {
        return (impl_ == nullptr) ? nullptr : impl_->DowncastingTo<T>();
    }

private:
    std::shared_ptr<DataImpl> impl_;
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif
