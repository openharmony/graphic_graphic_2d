#!/usr/bin/env python
# -*- coding: utf-8 -*-

# Copyright (c) 2024 Huawei Device Co., Ltd.
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

import os
import sys
import shutil
import errno


def main():
    if (len(sys.argv) != 3):
        sys.stderr.write("MUST have 1 parameter for the source path and 1 parameter for the destination")
        sys.stderr.write(os.linesep)
        exit(errno.EINVAL)
    
    source_dir = os.path.realpath(sys.argv[1])
    if (not os.path.isdir(source_dir)):
        sys.stderr.write("Source path MUST be a directory")
        sys.stderr.write(os.linesep)
        exit(errno.EINVAL)

    dest_dir = os.path.realpath(sys.argv[2])
    if (not os.path.isdir(dest_dir)):
        sys.stderr.write("Destination path MUST be a directory")
        sys.stderr.write(os.linesep)
        exit(errno.EINVAL)

    for item in os.listdir(source_dir):
        if item in ["ohos", "preview"]:
            continue

        if not os.path.isdir(os.path.join(source_dir, item)):
            continue

        file_path = os.path.join(source_dir, item, "build", "platform.gni")
        if not os.path.isfile(file_path):
            continue

        dest_dir_build_folder = os.path.join(dest_dir, item, "build")
        if not os.path.exists(dest_dir_build_folder):
            shutil.copytree(os.path.join(source_dir, item, "build"), dest_dir_build_folder, dirs_exist_ok=True)

if __name__ == "__main__":
    main()
