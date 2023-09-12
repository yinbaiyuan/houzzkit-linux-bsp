# houzzkit-linux-bsp

此仓库为houzzkit硬件项目的板级支持包，本项目由Rockchip官方提供的BSP修改而来。

本仓库的源码可以方便的为houzzkit硬件项目编译出正常驱动所有板载外设的Linux系统。

### **注意**

>1. 不能放在虚拟机的共享文件夹中编译！
>2. SDK的编译环境建议使用 ubuntu 18.04 或更高版本。
>3. 务必使用非root权限用户编译

### 编译环境依赖包

安装命令：
```bash
sudo apt-get install repo git ssh make gcc libssl-dev liblz4-tool \
expect g++ patchelf chrpath gawk texinfo chrpath diffstat binfmt-support \
qemu-user-static live-build bison flex fakeroot cmake gcc-multilib g++-multilib unzip \
device-tree-compiler python-pip libncurses-dev python2 bison flex libssl-dev \
liblz4-tool expect unzip g++ bzip2
```
若还有其他缺少的依赖包，请根据报错信息自行安装。

### 编译步骤
1. 执行以下命令，选择对应的配置文件序号。Houzzkit Force 1 选择44
   ```
   ./build.sh init
   ```
2. 全局编译。执行如下命令，可以自动编译 uboot、kernel、rootfs、recovery
   ```
   ./build.sh
   ```
3. 编译成功后将会在 rockdev/ 目录下生成如下镜像文件
    >MiniLoaderAll.bin  #引导 uboot、检测启动方式  
    >parameter.txt      #系统分区表、内核传参  
    >uboot.img          #引导系统分区   
    >trust.img          #ARM TrustZone 特性分区  
    >misc.img           #记录 recovery 升级信息  
    >boot.img           #内核、设备树、logo 分区  
    >recovery.img       #恢复还原分区  
    >rootfs.img         #根文件系统分区  
    >oem.img            #存放 OEM 资源分区  
    >update-***.img     #带日期后缀的完整固件 用于整包烧写  
4. 单独编译。通过以下命令可以单独编译 uboot、kernel、rootfs、recovery
   >./build.sh uboot  #在u-boot/ 下生成uboot.img 、trust.img  
   >./build.sh kernel  #在 rockdev/ 下生成 boot.img  
   >./build.sh rootfs  #在 rockdev/ 下生成 rootfs.img  
   >./build.sh recovery  
5. 编译buntu系统  
    5.1 编译Ubuntu系统前需要先完整编译一遍buildroot系统  
    5.2 将ubuntu根文件系统的.img文件复制到 ubuntu/ 目录下(img文件获取方式参考本文结尾)  
    5.3 执行以下命令，选择对应的配置文件序号。Houzzkit Force 1 选择46  
    ```
    ./build.sh init  
    ```  
    5.4 全局编译。执行如下命令，可以自动编译 uboot、kernel、rootfs、recovery  
    ```
    ./build.sh
    ```
    5.5 编译成功后将会在 rockdev/ 目录下生成如下镜像文件。整包烧录请使用带日期后缀的镜像：update-xxx.img

### 烧录
1. 虽然瑞芯微提供了不同操作系统的工具，但推荐一般用户使用Windows下的工具进行烧录。
2. 安装USB驱动。驱动程序路径：tooant_v5.12.zip。解压后执行DriverInstall.exe即可。
3. 镜像烧录工具。工具路径：tools/windows/RKDevTool_Release_v2.92.zip。解压后执行RKDevTool.exe即可。
4. 烧录镜像前硬件的准备工作   
    4.1 为主板插上电源。  
    4.2 将主板USB(OTG接口，HouzzKit Force 1 为下面的USB口)与电脑连接。如电脑有Type C口，可以用一头为TypeA口，一头为TypeC口的数据线。  
    4.3 将主板启动为LOADER模式或MASKROOM模式。   

   >当主板已经烧录过固件，能正常工作的情况下，按住主板上的RECOVERY键，然后按一下RESET键，然后松开RECOVERY键。此时主板进入LOADER模式。 

   >当主板没有烧录过固件，或者烧录的固件有问题，无法正常工作的情况下，按住主板上的MASKROOM键，然后按一下RESET键，然后松开MASKROOM键。此时主板进入MASKROOM模式。

   >针对HouzzKit Force 1，主板右侧有三个按键。从靠近tf卡槽一侧开始，依次为reset，recovery，poweron。主板左侧，靠近无线网卡的板边缘，有一个小按键，为maskroom按键。

5. 如第四步执行正常，烧录工具的底部会显示“发现一个LOADER设备”或者“发现一个MASKROOM设备”
6. 点击工具上方的“升级固件”按钮，点击“固件”，选择对应的镜像文件，等待固件校验完成后，点击“升级”按钮，等待烧录完成即可。
   
### 获取根文件系统镜像
   克隆  houzzkit-linux-rootfs 仓库，根据参考README文件操作即可。