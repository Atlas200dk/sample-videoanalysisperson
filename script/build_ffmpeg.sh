#!/bin/bash
script_path="$( cd "$(dirname "$0")" ; pwd -P )"
ffmpeg_version="n4.1.2"
remote_host=$1

. ${script_path}/func_util.sh

function download_code()
{
    if [ -d ${script_path}/ffmpeg ];then
        echo "FFmpeg code is found..."
        return 0
    else 
        echo "Download ffmpeg code..."
        ffmpeg_download_url="https://codeload.github.com/FFmpeg/FFmpeg/tar.gz/${ffmpeg_version}"
        wget -O ${script_path}/${ffmpeg_version}.ing ${ffmpeg_download_url} --no-check-certificate
        if [[ $? -ne 0 ]];then
            echo "ERROR: download failed, please check ${ffmpeg_download_url} connection."
            return 1
        fi
    fi

    mv ${script_path}/${ffmpeg_version}.ing ${script_path}/${ffmpeg_version}
    tar -zxvf ${script_path}/${ffmpeg_version} -C ${script_path} 1>/dev/null
    if [[ $? -ne 0 ]];then
        echo "ERROR: uncompress ffmpeg tar.gz file failed, please check ${ffmpeg_download_url} connection."
        return 1
    fi
    mv ${script_path}/FFmpeg-${ffmpeg_version} ${script_path}/ffmpeg
    rm -rf ${script_path}/${ffmpeg_version}
    rm -rf ${script_path}/${ffmpeg_version}.ing
    return 0

}

function build_ffmpeg()
{
    echo "Build ffmpeg..."
    install_prefix=${script_path}/ffmpeg/install_path
    
    mkdir -p ${install_prefix}
    ffmpeg_configure_options=" --cross-prefix=/usr/bin/aarch64-linux-gnu- --enable-pthreads --enable-cross-compile --target-os=linux --arch=aarch64 --enable-shared --enable-network --enable-protocol=tcp --enable-protocol=udp --enable-protocol=rtp --enable-demuxer=rtsp --disable-debug --disable-stripping --disable-doc --disable-ffplay --disable-ffprobe --disable-htmlpages --disable-manpages --disable-podpages  --disable-txtpages --disable-w32threads --disable-os2threads --prefix=${install_prefix}"
    
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


    echo "Start to deploy ffmpeg"
    upload_tar_file "${script_path}/ffmpeg_lib.tar" "~/HIAI_PROJECTS/ascend_lib"
    if [[ $? -ne 0 ]];then
        return 1
    fi

    echo "Finish to deploy ffmpeg"
    rm -rf ${script_path}/ffmpeg_lib.tar
    exit 0
}

main



