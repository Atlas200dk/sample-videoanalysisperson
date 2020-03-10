#!/bin/bash
script_path="$( cd "$(dirname "$0")" ; pwd -P )"
app_path="${script_path}/../src"


. ${script_path}/func_util.sh


check_param_configure()
{
    remote_host=`cat ${app_path}/param_configure.conf | grep "remote_host" | awk -F'[ =]+' '{print $2}'`
    presenter_view_app_name=`cat ${app_path}/param_configure.conf | grep "presenter_view_app_name" | awk -F'[ =]+' '{print $2}'`
    video_path_of_host=`cat ${app_path}/param_configure.conf | grep "video_path_of_host" | awk -F'[ =]+' '{print $2}'`
    rtsp_video_stream=`cat ${app_path}/param_configure.conf | grep "rtsp_video_stream" | awk -F'[ =]+' '{print $2}'`
    if [[ ${remote_host} = "" || ${presenter_view_app_name} = "" || ${video_path_of_host} = "" && ${rtsp_video_stream} = "" ]];then
        if [[ ${remote_host} = "" || ${presenter_view_app_name} = "" ]];then
            echo "please check your param_configure.conf to make sure that parameters remote_host and  presenter_view_app_name must have a value"
        else
            echo "please check your param_configure.conf to make sure that there must be a value between two parameters video_path_of_host and rtsp_video_stream"
        	return 1
        fi
    fi

    #get and check format of remost_host ip
    check_remote_host
    if [ $? -ne 0 ];then
		return 1
    fi

    #check format of presenter_view_app_name
    presenter_view_app_name=`cat ${app_path}/param_configure.conf | grep "presenter_view_app_name" | awk -F'[ =]+' '{print $2}' | grep "^[a-zA-Z0-9_]\{3,20\}$"`
    if [[ ${presenter_view_app_name} = "" ]];then
        echo "ERROR: invalid presenter_view_app_name name,please input 0-9, a-z, A-Z, _ , digit from 3 - 20 ."
        return 1
    fi
}

function build_common()
{
    echo "build common lib..."
    if [ ! -d "${HOME}/ascend_ddk" ];then
        mkdir $HOME/ascend_ddk
        if [[ $? -ne 0 ]];then
            echo "ERROR: Execute mkdir command failed, Please check your environment"
            return 1
        fi
    fi
    bash ${script_path}/build_ezdvpp.sh ${remote_host}
    if [ $? -ne 0 ];then
        echo "ERROR: Failed to deploy ezdvpp"
        return 1
    fi

    bash ${script_path}/build_presenteragent.sh ${remote_host}
    if [ $? -ne 0 ];then
        echo "ERROR: Failed to deploy presenteragent"
        return 1
    fi

    bash ${script_path}/build_ffmpeg.sh ${remote_host}
    if [ $? -ne 0 ];then
        echo "ERROR: Failed to deploy ffmpeg"
        return 1
    fi

    return 0
}

function check_video_analysis_proto_version()
{
    proto_dir=$app_path/video_analysis_post
    pb_h_file=$app_path/video_analysis_post/video_analysis_message.pb.h
    proto_file=$app_path/video_analysis_post/video_analysis_message.proto

    check_proto_version $pb_h_file $proto_file
    if [ $? -eq 1 ];then
        echo "ERROR: check video analysis proto code failed"
        return 1
    fi

    echo "Regenerate proto code success"
    return 0   
}

function main()
{
    echo "Modify param information in graph.config..."
    check_param_configure
    if [ $? -ne 0 ];then
        echo "ERROR: modify param information in graph.config failed" 
        return 1
    fi

    echo "Check video analysis proto"
    check_video_analysis_proto_version
    if [ $? -ne 0 ];then
        echo "ERROR: check video analysis proto failed"
        return 1
    fi

    build_common
    if [ $? -ne 0 ];then
        echo "ERROR: Failed to deploy common lib"
        return 1
    fi


    echo "echo Prepare app configuration..."
    parse_presenter_altasdk_ip ${remote_host}
    presenter_port=`grep presenter_server_port ${script_path}/../presenterserver/video_analysis/config/config.conf | awk -F '=' '{print $2}' | sed 's/[^0-9]//g'`
    if [ $? -ne 0 ];then
        echo "ERROR: get presenter server port failed, please check ${script_path}/../presenterserver/video_analysis/config/config.conf."
        return 1
    fi

    cp -r ${script_path}/graph_template.config ${app_path}/graph.config

    sed -i "s#\${template_channel1}#${video_path_of_host}#g" ${app_path}/graph.config
    sed -i "s#\${template_channel2}#${rtsp_video_stream}#g" ${app_path}/graph.config
    sed -i "s/\${template_app_name}/${presenter_view_app_name}/g" ${app_path}/graph.config
    sed -i "s/\${template_presenter_ip}/${presenter_atlasdk_ip}/g" ${app_path}/graph.config
    sed -i "s/\${template_presenter_port}/${presenter_port}/g" ${app_path}/graph.config
    return 0
}
main
