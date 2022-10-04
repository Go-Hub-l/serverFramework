#!/bin/bash

START=1
STOP=1

case $1 in
    start)
        START=1
        STOP=0
        ;;
    stop)
        START=0
        STOP=1
        ;;
    "")
        STOP=1
        START=1
        ;;
    *)
        STOP=0
        START=0
        ;;
esac

# **************************** 杀死正在运行的CGI进程 **************************** 
if [ "$STOP" -eq 1 ];then
    # 登录
    kill -9 $(ps aux | grep "./bin/mainDFS" | grep -v grep | awk '{print $2}') > /dev/null 2>&1

    echo "httpserver 程序已经成功关闭, bye-bye ..."

fi


# ******************************* 重新启动CGI进程 ******************************* 
if [ "$START" -eq 1 ];then
    # 登录
    echo -n "启动DFS Server："
	# &表示后台启动程序
    ./bin/mainDFS -s &
fi
