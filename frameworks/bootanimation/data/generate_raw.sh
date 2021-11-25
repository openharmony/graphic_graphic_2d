#!/bin/bash
# Copyright (c) 2021 Huawei Device Co., Ltd.
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

GENERATE_RAW=bootanimation.raw

declare -A HEADER_TYPE=(
    [NONE]=0
    [RAW]=1
    [COMPRESSED]=2
)

echo_int32_t_to_bin()
{
    local int32_t=$1

    local big_endian=$(printf "%08x" $int32_t)
    local little_endian=${big_endian:6:2}${big_endian:4:2}${big_endian:2:2}${big_endian:0:2}
    echo "0: $little_endian" | xxd -r
}

get_file_size()
{
    local filename=$1

    du -b $filename | awk '{print $1}'
}

echo_file_part()
{
    local filename=$1
    local position=$2
    local length=$3

    xxd -l $length -s $position -o $((-$position)) $filename | xxd -r
}

main()
{
    ls * | while read filename; do
        # check mime type
        file $filename | grep "image" >/dev/null 2>&1
        if [ $? != 0 ]; then
            continue
        fi

        # generate raw data
        convert $filename $filename.rgba
        xxd -c 1 $filename.rgba > $filename.xxd
        ofilename=$filename.rgba

        if [ "$last_filename" = "" ]; then
            # first image
            header_type=${HEADER_TYPE["RAW"]}
            position=0
            length="$(get_file_size $ofilename)"
        else
            # damage range
            result="$(diff -y $filename.xxd $last_filename.xxd | grep " |$(echo -e "\x09")")"
            if [ "$result" = "" ]; then
                header_type=${HEADER_TYPE["NONE"]}
                position=0
                length=0
            else
                header_type=${HEADER_TYPE["RAW"]}
                position="$(printf "%d\n" 0x$(echo "$result" | head -1 | awk '{print $1}' | cut -d: -f1))"
                length="$(printf "%d\n" 0x$(echo "$result" | tail -1 | awk '{print $1}' | cut -d: -f1))"
                ((length -= ${position}))
                ((length = $length / 4 * 4))
            fi
        fi

        # compress
        clen=$length
        if [ "$header_type" = "${HEADER_TYPE["RAW"]}" ]; then
            header_type=${HEADER_TYPE["COMPRESSED"]}
            ofilename=$filename.compress
            echo_file_part $filename.rgba $position $length | zlib-flate -compress=9 > $ofilename
            clen=$(get_file_size $ofilename)
        else
            if [ "$header_type" = "${HEADER_TYPE["NONE"]}" ]; then
                clen=0
            fi
        fi

        if [ "$last_filename" = "" ]; then
            echo -en "RAW.diff" > $GENERATE_RAW
        fi
        echo_int32_t_to_bin $header_type >> $GENERATE_RAW
        echo_int32_t_to_bin $position >> $GENERATE_RAW
        echo_int32_t_to_bin $length >> $GENERATE_RAW
        echo_int32_t_to_bin $clen >> $GENERATE_RAW
        echo_file_part $ofilename 0 $clen >> $GENERATE_RAW

        # for BUS_ADRALN
        (( align = $clen - $clen / 4 * 4 ))
        [ "$align" != "0" ] && (( align = 4 - $align ))
        [ "$align" != "0" ] && echo -en "\x00" >> $GENERATE_RAW && (( align-- ))
        [ "$align" != "0" ] && echo -en "\x00" >> $GENERATE_RAW && (( align-- ))
        [ "$align" != "0" ] && echo -en "\x00" >> $GENERATE_RAW && (( align-- ))

        echo $filename $header_type $position $length $clen

        last_filename=$filename
    done

    rm -f *.rgba
    rm -f *.xxd
    rm -f *.compress
    return 0
}

main $*
exit $?
