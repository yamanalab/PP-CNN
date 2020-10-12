if [ $# -ge 1 ]; then
    cmd=$1
else
    cmd="start"
fi

if [ $cmd == "start" ]; then
    docker run -d --rm -it -e DISPLAY=$DISPLAY -v /tmp/.X11-unix/:/tmp/.X11-unix -v `pwd`:/root/work/PP-CNN --name pp-cnn pp-cnn
elif [ $cmd == "stop" ]; then
    docker stop pp-cnn
fi
