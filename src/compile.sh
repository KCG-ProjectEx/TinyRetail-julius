#!/bin/sh

USER="pi"
PW="yasuda"
# WkDir="~/TinyRetail-julius/src"
# FILE=TinyRetail_Julius.cpp
# WkDir="~/TinyRetail-julius/src/include"
# FILE=include/TinyRetail_Julius.h
WkDir="~/TinyRetail-julius/src"
FILE=TinyRetail_main.cpp
# WkDir="~/TinyRetail-julius/src"
# FILE=Makefile
# WkDir="~/TinyRetail-julius/src"
# FILE=Post_curl.cpp

while getopts abi:f: OPT
#http://shellscript.sunone.me/parameter.html 参考文献
do
    case $OPT in
        "a" ) FLG_A="TRUE" ;;
        "b" ) FLG_B="TRUE" ;;
        "i" ) FLG_I="TRUE" VALUE_I="$OPTARG" ;;
        "f" ) FLG_F="TRUE" VALUE_F="$OPTARG" ;;
    esac
done

#IPアドレス設定
if [ "$FLG_I" = "TRUE" ]; then
    IP=$VALUE_I
else
    IP="192.168.0.12"
fi

#ファイルを指定
if [ "$FLG_F" = "TRUE" ]; then
    if [ -e $VALUE_F ]; then
        FILE=$VALUE_F
    else
        echo "ERR : 指定したFILEが存在しません"
        exit 1
    fi
fi

#ファイル転送するなら
if [ "$FLG_A" = "TRUE" ]; then
    expect -c "
    set timeout 5
    spawn scp -r ${FILE} ${USER}@${IP}:${WkDir}
    expect \"(yes/no)?\" {
        send \"yes\n\"
        expect \"password:\"
        send \"${PW}\n\"
    } \"password:\" {
        send \"${PW}\n\"
    } \"Permission denied (publickey,gssapi-keyex,gssapi-with-mic).\" {
        exit
    }
    expect \"$\"
    exit 0
    "

    if [ $? = 1 ]; then
        echo "ERR : ファイル転送失敗"
        exit 1
    fi
fi

#コンパイルするなら
if [ "$FLG_B" = "TRUE" ]; then
    expect -c "
    set timeout 5
    spawn ssh ${USER}@${IP}
    expect \"(yes/no)?\" {
        send \"yes\n\"
        expect \"password:\"
        send \"${PW}\n\"
    } \"password:\" {
        send \"${PW}\n\"
    } \"Permission denied (publickey,gssapi-keyex,gssapi-with-mic).\" {
        exit
    }
    expect \"pi@raspberrypi:\"
    send \"cd ${WkDir}\n\"
    expect \"pi@raspberrypi:\"
    send \"make\n\"
    expect \"pi@raspberrypi:\"
    send \"exit\n\"
    exit 0
    "
    # interact #自動入力解除時はコメントアウトを外す

    if [ $? = 1 ]; then
        echo "ERR : SSH接続失敗"
        exit 1
    fi
fi
