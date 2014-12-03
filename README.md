# freesub

The aim of this project is to provide a simple stdin/stdout to/from tcp
forwarding tool. The main use case is to "proxy" the data through SSH and the
target daemon via SSH subsystem.

### Installation

#### Prerequisites

Because of OpenWrt origins we are using _libubox_ - a small utility library
developed for OpenWrt containing a event loop, linked lists, blobmsg, stream
buffer management, and other. The sources can be found
[here](http://git.openwrt.org/?p=project/libubox.git).
    
#### Installation

For most users it will be much easier to use already made packages. At the
moment packages can be found for
[OpenWrt](https://github.com/openwrt-management/packages/blob/master/freesub/Makefile)
and [Arch](https://aur.archlinux.org/packages/freesub-git/).

_freesub_ can also be easily compiled and installed from source:

    cmake CMakeLists.txt DCMAKE_INSTALL_PREFIX=/usr
    make
    sudo make install
    
### Configuration

In case you are using _freesub_ for _OpenSSH_ managed NETCONF subsystem and
want to use _freenetconfd_ NETCONF server make sure that you add the following
to your _/etc/ssh/sshd_config_:

    Subsystem netconf /usr/bin/freesub -H 127.0.0.1 -P 1831

You can also make symbolic link called netconf:

    sudo ln -s /usr/bin/freesub /usr/bin/netconf

After that this will work as well:

    Subsystem netconf /usr/bin/netconf
