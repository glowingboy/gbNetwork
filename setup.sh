#!/bin/bash

if [ $EUID -ne 0 ]
then
    echo "need run as root"
    exit
fi

# if [ -z $1 ]
# then
#     echo "no user name input"
#     exit
# fi

# usr_name=$1

echo "**** checking dependecies ****"

fs_git_repo_addr="https://github.com/glowingboy/"

function fs_dependency_check()
{
    echo "*** checking for $1 ***"
    bFound=0
    if [ -d "/usr/local/$1" ]
    then
	bFound=1
    fi

    if [ $bFound -eq 1 ]
    then
	echo "*** $1 found ***"
    else
	echo "*** $1 not found ***"
    fi
    
    return $bFound
}

function fs_get_dependency()
{
    echo "** downloading $1 **"
    rm -rfv $1
    $usr_name git clone $fs_git_repo_addr$1.git
    echo "** $1 downloaded **"
    echo "** installing $1 **"
    cd $1
    mkdir build && cd build
    rm -rfv ./* && cmake -DCMAKE_BUILD_TYPE=Debug .. && cmake --build . --target install
    rm -rfv ./* && cmake -DCMAKE_BUILD_TYPE=Release .. && cmake --build . --target install
    cd ../../
    rm -rfv $1
    echo "** $1 installed"
}

#lua
fs_dependency_check lua

if [ $? -ne 1 ]
then
    #if on ubuntu
    echo "*** install libreadline-dev for lua compiling"
    printf "y\n" | apt-get install libreadline-dev
    
    fs_get_dependency lua
fi

#gbUtils
fs_dependency_check gbUtils

if [ $? -ne 1 ]
then
    fs_get_dependency gbUtils
fi

#Theron
fs_dependency_check Theron

if [ $? -ne 1 ]
then
    echo "** downloading Theron **"
    rm -rfv Theron
    $usr_name git clone "$fs_git_repo_addr"Theron.git
    echo "** Theron downloaded **"
    echo "** installing Theron **"
    cd Theron
    make install mode=debug boost=off c++11=on posix=on
    make install mode=release boost=off c++11=on posix=on
    cd ..
    rm -rfv Theron
    echo "** Theron installed"
fi

#libevent
bFound=0
if [ -f "/usr/local/lib/libevent_core.a" ]
then
    bFound=1
fi

if [ $bFound -eq 1 ]
then
    echo "*** libevent found ***"
else
    echo "*** libevent not found ***"
fi

if [ $bFound -ne 1 ]
then
    echo "** downloading libevent **"
    rm -rfv libevent
    $usr_name git clone "$fs_git_repo_addr"libevent.git
    echo "** libevent downloaded **"
    echo "** installing libevent **"
    cd libevent
    git checkout gbRelease-2.1.8
    mkdir build
    cd build
    rm -rfv ./* && cmake -DCMAKE_BUILD_TYPE=Debug .. && cmake --build . --target install
    rm -rfv ./* && cmake -DCMAKE_BUILD_TYPE=Release .. && cmake --build . --target install
    cd ../../
    rm -rfv libevent
    echo "** libevent installed **"
fi

echo "**** dependencies checked ****"
