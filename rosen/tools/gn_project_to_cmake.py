"""
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
"""

"""
gn gen --ide=json
1. modifty at ./build/core/build_scripts/make_main.sh

${BUILD_TOOLS_DIR}/gn gen ${TARGET_OUT_DIR} --> ${BUILD_TOOLS_DIR}/gn gen --ide=json ${TARGET_OUT_DIR}

2. build object --> generate project.json at out/ohos-arm-release/project.json

3. python3 gn_project_to_cmake.py root_path_of_project(eg. home/xxxx/code/openharmony)

to generate CMakeLists.txt at home/xxxx/code/openharmony
"""

import datetime
import json
import os
import re
import sys
from pathlib import Path, PureWindowsPath
from typing import List

def to_abs_path(path: str) -> str:
    if path.startswith("//"):
        path = path[2:]
    return path


class GnTarget(object):
    def __init__(self, full_name: str, content: object):
        self.full_name = full_name
        self.content = content
        self.resolved = False

    @property
    def dep_names(self) -> set:
        return set(self.content.get('deps', []))

    @property
    def inputs_field(self) -> set:
        return set(self.content.get('inputs', set()))

    @property
    def sources_field(self) -> set:
        return set(self.content.get('sources', set()))

    @property
    def include_dirs_field(self) -> set:
        return set(self.content.get('include_dirs', set()))

    def __filter_src_dirs(self, files) -> set:
        return set(f.split("/src/")[0] + "/src" for f in files)

    @property
    def cpp_src_files(self) -> set:
        return set(to_abs_path(jf) for jf in self.sources_field
                   if
                   jf.endswith(".cpp") or jf.endswith(".h") or jf.endswith(".c++") or jf.endswith(".cc"))

    @property
    def cpp_hdr_dirs(self) -> set:
        return set(to_abs_path(x) for x in self.include_dirs_field if x != "//" and not x.startswith("//out"))


class ProjectJson(object):
    def __init__(self, path: str):
        self.path = path
        with open(path, 'r') as project_file:
            self.jobj = json.load(project_file)

        self.gn_targets_cache = dict()
        project_targets = self.jobj['targets']
        for k, v in project_targets.items():
            if (k.find("rosen") < 0):
                continue
            print(k)
            self.gn_targets_cache[k] = GnTarget(full_name=k, content=v)

    @property
    def targets(self):
        return self.gn_targets_cache.values()

    @property
    def target_names(self):
        return set([x.full_name for x in self.targets])

    def find_targets_startswith(self, prefix: str) -> List[GnTarget]:
        return [x for x in self.targets if x.full_name.startswith(prefix)]

    def get_target(self, name: str) -> GnTarget:
        return next(x for x in self.targets if x.full_name == name)


class CMakeCreator(object):
    def __init__(self, project_json: ProjectJson, project_root_dir: str):
        self.project_json = project_json
        self.project_root_dir = project_root_dir
        self.project_name = os.path.basename(self.project_root_dir)

    @property
    def project_cpp_srcs(self):
        ret = set()
        for x in self.project_json.targets:
            #print(x.cpp_src_files)
            for f in x.cpp_src_files:
                f = os.path.join(self.project_root_dir, f)
                if os.path.isfile(f):
                    ret.add(f)
                    print(f)

        return sorted(ret)

    @property
    def project_cpp_hdrs(self):
        ret = set()
        for x in self.project_json.targets:
            ret.update([_ for _ in x.cpp_hdr_dirs if os.path.isdir(self.project_root_dir + "/" + _)])
        return sorted(ret)

    @property
    def jar_deps(self):
        ret = set()
        for x in self.project_json.targets:
            ret.update([_ for _ in x.jar_files if os.path.isfile(self.project_root_dir + "/" + _)])
        return sorted(ret)

    def write_cmakelist(self):
        hdr_list_str = "\n                                   ".join(self.project_cpp_hdrs)
        src_list_str = "\n                                   ".join(self.project_cpp_srcs)

        project_name = self.project_name + "clion"

        with open(os.path.join(self.project_root_dir, 'CMakeLists.txt'), "w") as f:
            f.write("""cmake_minimum_required(VERSION 3.15)\n""")
            f.write("project(" + project_name + ")\n""")
            f.write("set(CMAKE_CXX_STANDARD 17)\n")
            f.write("include_directories(" + hdr_list_str + ")\n")
            f.write("add_executable(" + project_name + " " + src_list_str + ")\n")

        with open(os.path.join(self.project_root_dir, 'CMakeLists.txt'), "w") as f:
            f.write('# Generated by gn_project_to_cmake.py.\n')
            f.write("""cmake_minimum_required(VERSION 3.15)
project({}clion)
set(CMAKE_CXX_STANDARD 17)
include_directories({})
add_executable({}clion  {})
        """.format(self.project_name, hdr_list_str, self.project_name, src_list_str))

    def write(self):
        self.write_cmakelist()


if __name__ == "__main__":
    #
    # read args
    #
    if len(sys.argv) < 2:
        print("""Usage: {} <project_root_dir>,
eg. {} /home/xxxxxx/codes/openharmony""".format(sys.argv[0], sys.argv[0]))
        sys.exit(1)

    project_root_dir = sys.argv[1]
    if not os.path.isdir(project_root_dir):
        print(project_root_dir + " not exist!")
        sys.exit(1)

    #
    # parse project.json
    #
    project_json = ProjectJson(path=os.path.join(*[project_root_dir, "out", "ohos-arm-release", "project.json"]))
    print(project_json)

    #
    # write CMakeLists.txt
    #
    CMakeCreator(project_json=project_json, project_root_dir=project_root_dir).write()

