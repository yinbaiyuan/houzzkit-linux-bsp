# RK312X Linux SDK Note

---

**Versions**

[TOC]

---

## rk312x_linux_release_v1.4.0_20220620.xml Note

**Buildroot(2018.02-rc3)**

```
- Fix sd upgrade and boot issue with SDDiskTool
- Support Vulkan and vkmark
- Switch weston to dispaly as launcher
- Update weston to fix some issues
- Upgrade wayland to buildroot upstream's 1.20.0
- Upgrade gstreamer1 to buildroot upstream's 1.20.0
- Upgrade glibc to buildroot upstream's 2.35
- Upgrade chromium-wayland to 101.0.4951.54
```

**Debian**

```
- Update the font to chinese by default
- Update mirrors.ustc.edu.cn for source.list
- Update mpp/gst-rkmpp/xserver
- Update rockchip-test
- Update gstreamer to fix the format issues
- Add rktoolkit and partition init for recovery
- Reduce the rootfs size
```

**Kernel**

```
- Adapt to new RK3126C & RK3128 EVB Board
```

**rkbin**

```
- rk3126_tee_laddr upgrade to v2.03
- rk3126_tee_ta upgrade to v1.04
```

## rk312x_linux_release_v1.2.0_20200806.xml Note

- Unified rk3126c/rk3128 buildroot config to rockchip_rk312x_defconfig
- Update U-Boot version from rockchip internal next-dev branch
- Kernel (4.4):  Update version from rockchip internal develop-4.4 branch
- rkbin (Rockchip binary): Update Trust: rk3126/rk3128: support 128MB DDR memory layout
- Debian (stretch-9.11): Add rk3128 debian BoardConfig_debian.mk
- Update docs and tools

## rk312x_linux_release_v1.1.3_20200317.xml Note

- Kernel (4.4): rk3128-fireprime: fix dmc probe error
- Debian (stretch-9.11): update some function

## rk312x_linux_release_v1.1.2_20200312.xml Note

- Debian (stretch-9.11): Add rk3128 platform support

## rk312x_linux_release_v1.1.1_20200229.xml Note

- Kernel (4.4): rk3128-fireprime add vpu function
- Buildroot (2018.02-rc3): app/QLauncher: fix QLauncher do not start when weston no ready

## rk312x_linux_release_v1.0.0_20190719.xml Note

- Buildroot (2018.02-rc3):
      - fix recovery mode can not find misc partition

## rk312x_linux_release_v1.0.0_20190719.xml Note

- First Release Version.
