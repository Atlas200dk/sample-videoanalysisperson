#!/bin/bash
script_path="$( cd "$(dirname "$0")" ; pwd -P )"
ffmpeg_version="n4.1.2"
remote_host=$1
AGENT_PATH="${HOME}/ascend_ddk"

. ${script_path}/func_util.sh

function download_code()
{
    if [ -d ${AGENT_PATH}/ffmpeg ];then
        echo "FFmpeg code is found..."
        return 0
    else 
        echo "Download ffmpeg code..."
        ffmpeg_download_url="https://gitee.com/mirrors/ffmpeg/repository/archive/${ffmpeg_version}.tar.gz"
        wget -O ${AGENT_PATH}/${ffmpeg_version}.ing ${ffmpeg_download_url} --no-check-certificate
        if [[ $? -ne 0 ]];then
            echo "ERROR: download failed, please check ${ffmpeg_download_url} connection."
            return 1
        fi
    fi

    mv ${AGENT_PATH}/${ffmpeg_version}.ing ${AGENT_PATH}/${ffmpeg_version}
    tar -zxvf ${AGENT_PATH}/${ffmpeg_version} -C ${AGENT_PATH} 1>/dev/null
    if [[ $? -ne 0 ]];then
        echo "ERROR: uncompress ffmpeg tar.gz file failed, please check ${ffmpeg_download_url} connection."
        return 1
    fi
    mv ${AGENT_PATH}/FFmpeg-${ffmpeg_version} ${AGENT_PATH}/ffmpeg
    rm -rf ${AGENT_PATH}/${ffmpeg_version}
    rm -rf ${AGENT_PATH}/${ffmpeg_version}.ing
    return 0

}

function build_ffmpeg()
{
    echo "Build ffmpeg..."
    install_prefix=${AGENT_PATH}/ffmpeg/install_path
    
    if [ -e "${AGENT_PATH}/ffmpeg/build_OK" ];then
        echo "ffmpeg so is ok.."
        cd ${install_prefix}/lib && tar -cvf ${AGENT_PATH}/ffmpeg_lib.tar ./* >> ${AGENT_PATH}/ffmpeg/ffbuild/build.log
        cd ${AGENT_PATH}
        return 0
    fi
    mkdir -p ${install_prefix}
    ffmpeg_configure_options=" --cross-prefix=/usr/bin/aarch64-linux-gnu- --enable-pthreads --enable-cross-compile --target-os=linux --arch=aarch64 --enable-shared --enable-network --enable-protocol=tcp --enable-protocol=udp --enable-protocol=rtp --enable-demuxer=rtsp --disable-debug --disable-stripping --disable-doc --disable-ffplay --disable-ffprobe --disable-htmlpages --disable-manpages --disable-podpages  --disable-txtpages --disable-w32threads --disable-os2threads --prefix=${install_prefix}"
    
    mkdir -p ${AGENT_PATH}/ffmpeg/ffbuild
    echo "" > ${AGENT_PATH}/ffmpeg/ffbuild/build.log
    #make clean -C ${script_path}/ffmpeg >> ${script_path}/ffmpeg/ffbuild/build.log 2>&1
    cd ${AGENT_PATH}/ffmpeg && bash ./configure ${ffmpeg_configure_options} >> ${AGENT_PATH}/ffmpeg/ffbuild/build.log 2>&1
    if [[ $? -ne 0 ]];then
        echo "ERROR: configure ffmpeg failed, please check build.log and config.log in ${AGENT_PATH}/ffmpeg/ffbuild"
        return 1
    fi
    make install -C ${AGENT_PATH}/ffmpeg >> ${AGENT_PATH}/ffmpeg/ffbuild/build.log 2>&1
    if [[ $? -ne 0 ]];then
        echo "ERROR: build ffmpeg failed, please check build.log in ${AGENT_PATH}/ffmpeg/ffbuild"
        return 1
    fi
    
    mkdir -p ${HOME}/ascend_ddk/include/third_party/ffmpeg
    mkdir -p ${HOME}/ascend_ddk/host/lib
    cp -rdp ${install_prefix}/include/* ${HOME}/ascend_ddk/include/third_party/ffmpeg
    cp -rdp ${install_prefix}/lib/* ${HOME}/ascend_ddk/host/lib
    echo "" > ${AGENT_PATH}/ffmpeg/build_OK
    cd ${install_prefix}/lib && tar -cvf ${AGENT_PATH}/ffmpeg_lib.tar ./* >> ${AGENT_PATH}/ffmpeg/ffbuild/build.log
    cd ${AGENT_PATH}
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
    upload_tar_file "${AGENT_PATH}/ffmpeg_lib.tar" "~/HIAI_PROJECTS/ascend_lib"
    if [[ $? -ne 0 ]];then
        return 1
    fi

    echo "Finish to deploy ffmpeg"
    rm -rf ${AGENT_PATH}/ffmpeg_lib.tar
    exit 0
}

main




