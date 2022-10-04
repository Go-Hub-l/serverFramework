#!/bin/bash
echo
echo ============= fastdfs ==============
# 关闭已启动的 tracker 和 storage
./fastdfs.sh stop
# 启动 tracker 和 storage
./fastdfs.sh all
# 关闭nginx
echo
echo ============= nginx ==============
/usr/local/nginx/sbin/nginx -s stop
/usr/local/nginx/sbin/nginx
# 启动nginx
./nginx.sh start
# 关闭redis
echo
echo ============= redis ==============
./redis.sh stop
# 启动redis
./redis.sh start
