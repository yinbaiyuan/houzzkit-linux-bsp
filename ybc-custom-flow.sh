#!/bin/bash

. $HOME/ybashrc/command-common.sh

customFlowStart()
{
	echo " "
}

buildAction()
{
    #export FORCE_UNSAFE_CONFIGURE=1
    #cmdExecute "echo '44' | ./build.sh init"
    #cmdExecute "./build.sh"
    #cmdExecute "echo '46' | ./build.sh init"
    cmdExecute "./build.sh"
}


main() 
{
    commandArr="build  manual"
    commandDesc="生成系统镜像 手动操作"
    inputQuestion="请输入流程名称"
    command=""
    if [ -z $1 ];then  
        commandInput "$commandArr" "$inputQuestion" "$commandDesc"
        command="$GLOBAL_INPUT_RESULT"
    else
        command=${1,,}
    fi
    case $command in
        build ) 
            buildAction
            ;;
        manual ) 
            customFlowStart
            ;;
        * ) 
            echoRed "[ERROR] 流程名称错误!"
            exit 1
    esac
}

main $@