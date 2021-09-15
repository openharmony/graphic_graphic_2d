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

#ifndef UTILS_INCLUDE_BUFFER_HANDLE_PARCEL_H
#define UTILS_INCLUDE_BUFFER_HANDLE_PARCEL_H

#ifdef __cplusplus
#include "buffer_handle.h"
#include "message_parcel.h"
namespace OHOS {
/* *
 * @Description: Write BufferHanedle to MessageParcel
 * @param parcel which the buffer handle will write to
 * @param handle Buffer handle which will wtite to parcel
 * @return  Returns true if the operation is successful; returns <b>false</b> otherwise.
 */
bool WriteBufferHandle(MessageParcel &parcel, const BufferHandle &handle);

/* *
 * @Description: Read BufferHanedle from MessageParcel
 * @param parcel message parcel which should has a buffer handle
 * @return  Returns pointer to buffer handle if the operation is successful; returns <b>nullptr</b> otherwise.
 */
BufferHandle *ReadBufferHandle(MessageParcel &parcel);
} // namespace OHO
#endif // __cplusplus

#endif // UTILS_INCLUDE_BUFFER_HANDLE_PARCEL_H
