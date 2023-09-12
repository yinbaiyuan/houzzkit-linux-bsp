# RK3588 Linux SDK Note

---

**Versions**

[TOC]

---
## rk3588_linux_release_v1.0.3_20220920.xml Note

- Upgrade bifrost DDK from g12p0-01eac0 to g13p0-01eac0
- Upgrade NPU verison to V1.4.0
- Refactor rkwifibt code
- Update Weston to support to set cursor size and launchers
- Add pm-utils for PowerManager
- Use chromium R88 by default
- Update some packages to buildroot upstream version
- Update Linux_Upgrade_Tool to v2.17
- Update UEFI
- Update audio/crypto/sata/PCIe/usb/rockit/gstreamer/rga... docs

## rk3588_linux_release_v1.0.2_20220820.xml Note

- Remove libglCompositor/avb repositories
- Add rockit/librkcrypto repositories
- Fix a few download/extract errors
- Upgrade Weston version to 10.0.1 and improve its stability
- Add support for buildroot X11 related functions
- Add support for Linux enlightment Wayland desktop
- Upgrade kernel to 5.10.110
- Update the chromium browser version of Debian to R101
- Update Linux_Upgrade_Tool, RKDevTool and SDDiskTool
- Improve HDMI compatibility and HDMI-IN stability
- Upgrade GStreamer of builderroot to 1.20.3 to improve audio and video compatibility
- Add gamma support for xserver and fix the problem of multi touch
- Update loader to improve chip stability
- Update NPU and fix memory leakage
- Update MPP to improve compatibility
- Update docs

## rk3588_linux_release_v1.0.1_20220620.xml Note

- Fixes Secureboot function issue
- Fixes SD boot and upgrade issue
- Update docs include secureboot,pinctrl,display,AVl and so on
- Update kernel to fix some issues
- Add RK3588M Socs to support
- Add RK3588 evb7 to support single pmic hardware design

## rk3588_linux_release_v1.0.0_20220520.xml Note

- Update all the projects with lastest
- Update kernel/uboot/rkbin to improve the chips stability
- Update documents and tools to make the SDK more better
- Fixes some bugs on buildroot weston10
- Fixes the multivideo hang issue
- Fixes the suspend to resume issues
- Fixes the kmssink/rksimageink plugins on gstreamer
- Fixes the bluthooth on/off during the suspend to resume
- Fixes the pcie-ssd performance with write/read
- Fixes the userdata address in parameter.txt
- Fixes the cheese app on debian
- Fixes the permission with pulseaudio
- Improve the video decode performance with mpp fast mode
- Improve the Audio/Video compatibility
- Upgrade RKNN SDK to v1.3.0
- Upgrade RKAIQ from v3.0x8.7 to v3.0x8.8
- Upgrade Debian version from 11.2 to 11.3
- Reduce the debian rootfs size from 5.5G to 3.2G

## rk3588_linux_beta_v0.1.1_20220421.xml Note

- Support midi and fluidsynth format for gstreamer
- Convert github git:// to https:/ on buildroot
- Support mpp fast-mode property

## rk3588_linux_beta_v0.1.0_20220414.xml Note

```
- The first beta version
```

## rk3588_linux_alpha_v0.0.1_20220115.xml Note

```
- The first alpha version
```
