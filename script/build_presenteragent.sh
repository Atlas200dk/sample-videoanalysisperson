#!/bin/bash

script_path="$( cd "$(dirname ${BASH_SOURCE})" ; pwd -P )"
remote_host=$1
presenteragent_version="1.2.0"
HOST_LIB_PATH="${HOME}/ascend_ddk/host/lib"

. ${script_path}/func_util.sh

function download_code()
{
    if [ -d ${script_path}/presenteragent ];then
        echo "Presenteragent code is found..."
        return 0
    else
        echo "Download presenteragent code..."
        presenteragent_download_url="https://gitee.com/Atlas200DK/sdk-presenter/repository/archive/1.2.0?format=zip"
        wget -O ${script_path}/presenteragent-${presenteragent_version}.ing ${presenteragent_download_url} --no-check-certificate 1>/dev/null 2>&1
    fi
    if [[ $? -ne 0 ]];then
        echo "ERROR: download failed, please check ${presenteragent_download_url} connection."
        return 1
    fi

    mv ${script_path}/presenteragent-${presenteragent_version}.ing ${script_path}/presenteragent-${presenteragent_version}.zip
    unzip ${script_path}/presenteragent-${presenteragent_version}.zip -d ${script_path} 1>/dev/null
    if [[ $? -ne 0 ]];then
        echo "ERROR: uncompress presenteragent tar.gz file failed, please check ${presenteragent_download_url} connection."
        return 1
    fi
	
	mkdir -p ${script_path}/presenteragent;rm -rf ${script_path}/presenteragent/*
    cp -rf  ${script_path}/sdk-presenter/presenteragent/* ${script_path}/presenteragent/

    rm -rf ${script_path}/presenteragent-${presenteragent_version}.zip
    rm -rf ${script_path}/sdk-presenter
    return 0

}

function build_presenteragent()
{
    echo "Build presenteragent..."
    make clean -C ${script_path}/presenteragent 1>/dev/null 2>&1
    if [[ $? -ne 0 ]];then
        echo "ERROR: compile presenteragent failed, please check the env."
        return 1
    fi
    make install -C ${script_path}/presenteragent 1>/dev/null 2>&1
    if [[ $? -ne 0 ]];then
        echo "ERROR: compile presenteragent failed, please check the env."
        return 1
    fi
}

main()
{
    download_code
    if [[ $? -ne 0 ]];then
        return 1
    fi

    build_presenteragent

    if [[ $? -ne 0 ]];then
        return 1
    fi

    echo "Finish to Build presenteragent."

    echo "Start to deploy presenteragent"
    upload_file "${HOST_LIB_PATH}/libpresenteragent.so" "~/HIAI_PROJECTS/ascend_lib"
    if [ $? -ne 0 ];then
        return  1
    fi
    echo "Finish to deploy presenteragent"


    exit 0
}

main
