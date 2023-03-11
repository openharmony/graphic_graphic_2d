/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#ifndef DIRECTORY_EX_H
#define DIRECTORY_EX_H

#include <string>
#include <vector>
#include <sys/stat.h>

namespace OHOS {

/**
 * @brief Get the full absolute path to the current program.
 *
 * /proc/self/exe represents the current program, and its source path can be 
 * read with the readlink function to get the absolute path of the current 
 * program.
 */
std::string GetCurrentProcFullFileName();
std::string GetCurrentProcFullFileName();

/**
 * @brief Get the absolute path of the current program.
 */
std::string GetCurrentProcPath();

/**
 * @brief Obtain the path to the corresponding file by the full path.
 */
std::string ExtractFilePath(const std::string& fileFullName);

/**
 * @brief Obtain the name to the corresponding file by the full path.
 */
std::string ExtractFileName(const std::string& fileFullName);

/**
 * @brief Obtain the filename extension to the corresponding file by the full 
 * path.
 */
std::string ExtractFileExt(const std::string& fileName);

/**
 * @brief Exclude the end '/' from the strPath.
 *
 * Determine whether the path has ended with '/', and returns the path after 
 * removing '/', otherwise returns the path directly.
 */
std::string ExcludeTrailingPathDelimiter(const std::string& path);

/**
 * @brief Include the end '/' from the strPath.
 *
 * Determine whether the path has ended with "/", and returns the path after 
 * adding '/', otherwise returns the path directly.
 */
std::string IncludeTrailingPathDelimiter(const std::string& path);

/**
 * @brief Get names of all files under `path` recursively.
 *
 * @param path Input path.
 * @param files Target `std::vector` to store the file names.
 */
void GetDirFiles(const std::string& path, std::vector<std::string>& files);

/**
 * @brief Judge if the path is empty.
 *
 * @return Return true if is empty, else false.
 */
bool IsEmptyFolder(const std::string& path);

/**
 * @brief Create the dir recursively.
 *
 * Parent directory can be created at the same time when it does not exist.
 *
 * @note If there are problems such as 'Permission Denied', the creation may 
 * also fail.
 * @return Return true if create success, else false.
 */
bool ForceCreateDirectory(const std::string& path);

/**
 * @brief Delete the specified dir.
 *
 * All subdirs and files will also be deleted.
 *
 * @note It is not necessarily successful to delete.
 * @note If there are problems such as 'Permission Denied', the deletion may 
 * also fail.
 * @return Return true if delete success, else false.
 */
bool ForceRemoveDirectory(const std::string& path);

/**
 * @brief Remove the file specified by fileName.
 *
 * @return Return true if remove success, else false.
 */
bool RemoveFile(const std::string& fileName);

/**
 * @brief Get the folder size(bytes).
 */
uint64_t GetFolderSize(const std::string& path);

/**
 * @brief Change the file authority.
 *
 * @param mode Specify the changed permissions, see chmod().
 * @return Return true if change success, else false.
 */
bool ChangeModeFile(const std::string& fileName, const mode_t& mode);

/**
 * @brief Change authority of the directory specified by path and all of its 
 * subdirectories.
 *
 * @param mode Specify the changed permissions, see chmod().
 * @return Return true if change success, else false.
 */
bool ChangeModeDirectory(const std::string& path, const mode_t& mode);

/**
 * @brief Get real path from relative path.
 *
 * @return Return true if get success, else false.
 */
bool PathToRealPath(const std::string& path, std::string& realPath);
} // OHOS
#endif
