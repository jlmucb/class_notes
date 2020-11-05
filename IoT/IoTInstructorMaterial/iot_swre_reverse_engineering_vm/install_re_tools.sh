#!/bin/bash

# Copyright 2019 BAE Systems
# 
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
# 
#     https://www.apache.org/licenses/LICENSE-2.0
# 
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

# Approved for public release; unlimited distribution
# Not export controlled per ES-FL-091619-0203

if [ "$EUID" -ne 0 ]; then
    echo "This script must be run as root.  Exiting..."
    exit
fi

cd ~student/

echo "Install miscellaneous tools..."
echo "=============================="

# vim
apt -y install vim

# net-tools (provides netstat)
apt -y install net-tools

# tree
apt -y install tree


echo "Install Buildroot required dependencies..."
echo "=========================================="

apt -y install build-essential
apt -y install libncurses5 libncurses5-dev
apt -y install libglib2.0-dev
apt -y install libglade2-0 libglade2-dev
apt -y install libcanberra-gtk-module


echo "Install Ghidra required dependencies..."
echo "======================================="

# OpenJDK 11
apt -y install openjdk-11-jdk


echo "Install cross compilation tools..."
echo "=================================="

apt -y install gcc-arm-linux-gnueabi binutils-arm-linux-gnueabi


echo "Install QEMU..."
echo "=================================="

apt -y install qemu


echo "Install Ghidra..."
echo "================="

(
cat <<'EOF'
#!/usr/bin/env xdg-open
[Desktop Entry]
Version=1.0
Type=Application
Terminal=false
Exec=/home/student/ghidra_9.0.4/ghidraRun
Name=Ghidra
Comment=Ghidra
Icon=/home/student/ghidra_9.0.4/docs/GhidraClass/Beginner/Images/GhidraLogo64.png
EOF
) > /usr/share/applications/Ghidra.desktop

chown root:root /usr/share/applications/Ghidra.desktop
chmod 644 /usr/share/applications/Ghidra.desktop

sudo -u student wget -P ~student/Downloads/ https://ghidra-sre.org/ghidra_9.0.4_PUBLIC_20190516.zip
sudo -u student unzip ~student/Downloads/ghidra_9.0.4_PUBLIC_20190516.zip


echo "Install Buildroot..."
echo "===================="

sudo -u student wget -P ~student/Downloads/ https://buildroot.org/downloads/buildroot-2019.02.4.tar.gz
sudo -u student tar -xzpf ~/Downloads/buildroot-2019.02.4.tar.gz
