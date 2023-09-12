#!/usr/bin/env python
# SPDX-License-Identifier: (GPL-2.0+ OR MIT)
# Copyright (c) 2018 Fuzhou Rockchip Electronics Co., Ltd
#


"""
Multiple dtb package tool

Usage: scripts/mkmultidtb.py board
The board is what you defined in DTBS dictionary like DTBS['board'],
Such as: PX30-EVB, RK3308-EVB

"""
import os
import sys
import shutil
from collections import OrderedDict

DTBS = {}

DTBS['nano-box-rk3566'] = OrderedDict([
('nano-box-rk3566-lvds-10-1280-800', '#_saradc_ch2=0'),
('nano-box-rk3566-mipi0-5-720-1280-v2-boxTP', '#_saradc_ch2=343'), 
('nano-box-rk3566-mipi0-8-800-1280-v3', '#_saradc_ch2=407'),
('nano-box-rk3566-mipi0-8-1200-1920-v2', '#_saradc_ch2=471'),
('nano-box-rk3566-lvds-7-1024-600', '#_saradc_ch2=511'),
('nano-box-rk3566-mipi0-7-720-1280', '#_saradc_ch2=556'),
('nano-box-rk3566-mipi0-10-1200-1920', '#_saradc_ch2=607'),
('nano-box-rk3566-mipi0-10-800-1280-v3', '#_saradc_ch2=647'),
('nano-box-rk3566-edp-13.3-15.6-1920-1080', '#_saradc_ch2=943')
])

DTBS['nano-box-rk3568'] = OrderedDict([
('nano-box-rk3568-lvds-10-1280-800', '#_saradc_ch3=0'),
('nano-box-rk3568-mipi0-5-720-1280-v2-boxTP', '#_saradc_ch3=343'),
('nano-box-rk3568-mipi0-8-800-1280-v3', '#_saradc_ch3=407'),
('nano-box-rk3568-mipi0-8-1200-1920-v2', '#_saradc_ch3=471'),
('nano-box-rk3568-lvds-7-1024-600', '#_saradc_ch3=511'),
('nano-box-rk3568-mipi0-7-720-1280', '#_saradc_ch3=556'),
('nano-box-rk3568-mipi0-10-1200-1920', '#_saradc_ch3=607'),
('nano-box-rk3568-mipi0-10-800-1280-v3', '#_saradc_ch3=639'),
('nano-box-rk3568-edp-13.3-15.6-1920-1080', '#_saradc_ch3=926')
])

DTBS['rp-box-rk3566'] = OrderedDict([
('rp-box-rk3566-lvds-10-1280-800', '#_saradc_ch1=0'),
('rp-box-rk3566-mipi0-5-720-1280-v2-boxTP', '#_saradc_ch1=339'),
('rp-box-rk3566-mipi0-8-800-1280-v3', '#_saradc_ch1=403'),
('rp-box-rk3566-mipi0-8-1200-1920-v2', '#_saradc_ch1=471'),
('rp-box-rk3566-lvds-7-1024-600', '#_saradc_ch1=511'),
('rp-box-rk3566-mipi0-7-720-1280', '#_saradc_ch1=556'),
('rp-box-rk3566-mipi0-10-1200-1920', '#_saradc_ch1=607'),
('rp-box-rk3566-mipi0-10-800-1280-v3', '#_saradc_ch1=651'),
('rp-box-rk3566-edp-13.3-15.6-1920-1080', '#_saradc_ch1=971')
])

# DTBS['rp-box-rk3568'] = OrderedDict([
# ('rp-box-rk3568-edp-13.3-15.6-1920-1080', '#_saradc_ch1=0'),
# ('rp-box-rk3568-hdmi', '#_saradc_ch1=0'),
# ('rp-box-rk3568-lvds-10-1024-600-raw', '#_saradc_ch1=0'),
# ('rp-box-rk3568-lvds-10-1280-800', '#_saradc_ch1=0'),
# ('rp-box-rk3568-lvds-7-1024-600-v2', '#_saradc_ch1=0'),
# ('rp-box-rk3568-mipi0-10-1200-1920', '#_saradc_ch1=0'),
# ('rp-box-rk3568-mipi0-10-800-1280-v3', '#_saradc_ch1=0'),
# ('rp-box-rk3568-mipi0-5-720-1280-v2-boxTP', '#_saradc_ch1=0'),
# ('rp-box-rk3568-mipi0-7-720-1280', '#_saradc_ch1=0'),
# ('rp-box-rk3568-mipi0-8-1200-1920', '#_saradc_ch1=0'),
# ('rp-box-rk3568-mipi0-8-800-1280-v3', '#_saradc_ch1=0')
# ])





def main():
    if (len(sys.argv) < 2) or (sys.argv[1] == '-h'):
        print __doc__
        sys.exit(2)

    BOARD = sys.argv[1]
    TARGET_DTBS = DTBS[BOARD]
    target_dtb_list = ''
    default_dtb = True

    for dtb, value in TARGET_DTBS.items():
        if default_dtb:
            ori_file = 'arch/arm64/boot/dts/rockchip/' + dtb + '.dtb'
            shutil.copyfile(ori_file, "rk-kernel.dtb")
            target_dtb_list += 'rk-kernel.dtb '
            default_dtb = False
        new_file = dtb + value + '.dtb'
        ori_file = 'arch/arm64/boot/dts/rockchip/' + dtb + '.dtb'
        shutil.copyfile(ori_file, new_file)
        target_dtb_list += ' ' + new_file

    print target_dtb_list
    os.system('scripts/resource_tool logo.bmp logo_kernel.bmp ' + target_dtb_list)
    os.system('rm ' + target_dtb_list)

if __name__ == '__main__':
    main()
