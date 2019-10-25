#!/bin/bash
#
#   =======================================================================
#
# Copyright (C) 2018, Hisilicon Technologies Co., Ltd. All Rights Reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
#   1 Redistributions of source code must retain the above copyright notice,
#     this list of conditions and the following disclaimer.
#
#   2 Redistributions in binary form must reproduce the above copyright notice,
#     this list of conditions and the following disclaimer in the documentation
#     and/or other materials provided with the distribution.
#
#   3 Neither the names of the copyright holders nor the names of the
#   contributors may be used to endorse or promote products derived from this
#   software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
# LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
# CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
# SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
# INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
# CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
# ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.
#   =======================================================================

# ************************Variable*********************************************

script_path="$( cd "$(dirname "$0")" ; pwd -P )"

download_mode=$1
ffmpeg_version="n4.1.2"

function download_code()
{
    if [ -d ${script_path}/ffmpeg ];then
        echo "FFmpeg code is found..."
        return 0
    else
        if [[ ${download_mode} == "local" ]];then
            echo "WARNING: no ffmpeg code found."
            read -p "Do you want to download from internet?(y/n, default:y)" confirm
            if [[ ${confirm}"X" != "X" && ${confirm} != "y" && ${confirm} != "Y" ]];then
                echo "ERROR: no ffmpeg code found and no download choice, please put ffmpeg code in ${script_path}/ffmpeg path manually."
                return 1
            fi
        fi
    fi
    echo "Download ffmpeg code..."
    ffmpeg_download_url="https://gitee.com/mirrors/ffmpeg/repository/archive/${ffmpeg_version}?format=tar.gz"
    wget -O ${script_path}/${ffmpeg_version}.ing ${ffmpeg_download_url} --no-check-certificate
    if [[ $? -ne 0 ]];then
        echo "ERROR: download failed, please check ${ffmpeg_download_url} connection."
        return 1
    fi

    mv ${script_path}/${ffmpeg_version}.ing ${script_path}/${ffmpeg_version}
    tar -zxvf ${script_path}/${ffmpeg_version} -C ${script_path} 1>/dev/null
    if [[ $? -ne 0 ]];then
        echo "ERROR: uncompress ffmpeg tar.gz file failed, please check ${ffmpeg_download_url} connection."
        return 1
    fi
    rm -rf ${script_path}/${ffmpeg_version}
    rm -rf ${script_path}/${ffmpeg_version}.ing
    return 0

}

function build_ffmpeg()
{
    echo "Build ffmpeg..."
    atlas_target=`grep "TARGET" ${DDK_HOME}/ddk_info | awk -F '"' '{print $4}'`
    if [[ $? -ne 0 ]];then
        echo "ERROR: can not get TARGET from ${DDK_HOME}/ddk_info, please check your env"
        return 1
    fi

    atlas_target=`echo ${atlas_target} | sed 's/ //g' `

    install_prefix=${script_path}/ffmpeg/install_path
    mkdir -p ${install_prefix}

    if [[ ${atlas_target} == "ASIC" ]];then
        ffmpeg_configure_options=" --enable-pthreads --enable-cross-compile --target-os=linux --enable-shared --enable-network --enable-protocol=tcp --enable-protocol=udp --enable-protocol=rtp --enable-demuxer=rtsp --disable-debug --disable-stripping --disable-doc --disable-ffplay --disable-ffprobe --disable-htmlpages --disable-manpages --disable-podpages  --disable-txtpages --disable-w32threads --disable-os2threads --disable-x86asm --prefix=${install_prefix}"
    else
        ffmpeg_configure_options=" --cross-prefix=/usr/bin/aarch64-linux-gnu- --enable-pthreads --enable-cross-compile --target-os=linux --arch=aarch64 --enable-shared --enable-network --enable-protocol=tcp --enable-protocol=udp --enable-protocol=rtp --enable-demuxer=rtsp --disable-debug --disable-stripping --disable-doc --disable-ffplay --disable-ffprobe --disable-htmlpages --disable-manpages --disable-podpages  --disable-txtpages --disable-w32threads --disable-os2threads --prefix=${install_prefix}"
    fi



    mkdir -p ${script_path}/ffmpeg/ffbuild
    echo "" > ${script_path}/ffmpeg/ffbuild/build.log
    #make clean -C ${script_path}/ffmpeg >> ${script_path}/ffmpeg/ffbuild/build.log 2>&1
    cd ${script_path}/ffmpeg && bash ./configure ${ffmpeg_configure_options} >> ${script_path}/ffmpeg/ffbuild/build.log 2>&1
    if [[ $? -ne 0 ]];then
        echo "ERROR: configure ffmpeg failed, please check build.log and config.log in ${script_path}/ffmpeg/ffbuild"
        return 1
    fi
    make install -C ${script_path}/ffmpeg >> ${script_path}/ffmpeg/ffbuild/build.log 2>&1
    if [[ $? -ne 0 ]];then
        echo "ERROR: build ffmpeg failed, please check build.log in ${script_path}/ffmpeg/ffbuild"
        return 1
    fi
    
    mkdir -p ${HOME}/ascend_ddk/include/third_party/ffmpeg
    mkdir -p ${HOME}/ascend_ddk/host/lib
    cp -rdp ${install_prefix}/include/* ${HOME}/ascend_ddk/include/third_party/ffmpeg
    cp -rdp ${install_prefix}/lib/* ${HOME}/ascend_ddk/host/lib

    cd ${install_prefix}/lib && tar -cvf ${script_path}/ffmpeg_lib.tar ./* >> ${script_path}/ffmpeg/ffbuild/build.log
    cd ${script_path}
}

main()
{
    #download code
    download_code
    if [[ $? -ne 0 ]];then
        return 1
    fi

    build_ffmpeg

    if [[ $? -ne 0 ]];then
        return 1
    fi

    echo "Finish to Build ffmpeg."
    exit 0
}

main
