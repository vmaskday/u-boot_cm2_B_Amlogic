
/*
 * board/khadas/configs/kvim3l.h
 *
 * Copyright (C) 2019 Wesion, Inc. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#ifndef __KVIM3L_H__
#define __KVIM3L_H__

#include <asm/arch/cpu.h>

#define CONFIG_SYS_GENERIC_BOARD  1
#ifndef CONFIG_AML_MESON
#warning "include warning"
#endif

/*
 * platform power init config
 */
#define CONFIG_PLATFORM_POWER_INIT
#define CONFIG_VCCK_INIT_VOLTAGE	800		// VCCK power up voltage
#define CONFIG_VDDEE_INIT_VOLTAGE	880		// VDDEE power up voltage
#define CONFIG_VDDEE_SLEEP_VOLTAGE	770		// VDDEE suspend voltage

/* Khadas commands */
#define CONFIG_KHADAS_KBI 1
#define CONFIG_KHADAS_CFGLOAD 1
#define CONFIG_KHADAS_SCRIPT 1

#define CONFIG_TCA6408 1
#define CONFIG_POWER_FUSB302 1

/* configs for CEC */
#define CONFIG_CEC_OSD_NAME		"KVIM3L"
#define CONFIG_CEC_WAKEUP
/*if use bt-wakeup,open it*/
#define CONFIG_BT_WAKEUP
/* SMP Definitinos */
#define CPU_RELEASE_ADDR		secondary_boot_func

/* config saradc*/
#define CONFIG_CMD_SARADC 1
#define CONFIG_SARADC_CH  2

/* Bootloader Control Block function
   That is used for recovery and the bootloader to talk to each other
  */
#define CONFIG_BOOTLOADER_CONTROL_BLOCK

#define CONFIG_CMD_BOOTCTOL_AVB

/* Serial config */
#define CONFIG_CONS_INDEX 2
#define CONFIG_BAUDRATE  115200
#define CONFIG_AML_MESON_SERIAL   1
#define CONFIG_SERIAL_MULTI		1

//Enable ir remote wake up for bl30
#define CONFIG_IR_REMOTE_POWER_UP_KEY_VAL1 0xeb14ff00 //khadas ir --- power
#define CONFIG_IR_REMOTE_POWER_UP_KEY_VAL2 0XBB44FB04 //amlogic tv ir --- ch+
#define CONFIG_IR_REMOTE_POWER_UP_KEY_VAL3 0xF20DFE01 //amlogic tv ir --- ch-
#define CONFIG_IR_REMOTE_POWER_UP_KEY_VAL4 0XBA45BD02 //amlogic small ir--- power
#define CONFIG_IR_REMOTE_POWER_UP_KEY_VAL5 0xe51afb04
#define CONFIG_IR_REMOTE_POWER_UP_KEY_VAL6 0xFFFFFFFF
#define CONFIG_IR_REMOTE_POWER_UP_KEY_VAL7 0xFFFFFFFF
#define CONFIG_IR_REMOTE_POWER_UP_KEY_VAL8 0xFFFFFFFF
#define CONFIG_IR_REMOTE_POWER_UP_KEY_VAL9 0xFFFFFFFF

/*config the default parameters for adc power key*/
#define CONFIG_ADC_POWER_KEY_CHAN   2  /*channel range: 0-7*/
#define CONFIG_ADC_POWER_KEY_VAL    0  /*sample value range: 0-1023*/

/* args/envs */
#define CONFIG_SYS_MAXARGS  64
#define CONFIG_EXTRA_ENV_SETTINGS \
        "firstboot=1\0"\
        "upgrade_step=0\0"\
        "jtag=disable\0"\
        "port_mode=0\0"\
        "loadaddr=1080000\0"\
        "panel_type=lcd_0\0" \
        "outputmode=panel\0" \
        "hdmimode=1080p60hz\0" \
        "colorattribute=444,8bit\0"\
        "cvbsmode=576cvbs\0" \
        "display_width=1920\0" \
        "display_height=1080\0" \
        "display_bpp=24\0" \
        "display_color_index=24\0" \
        "display_layer=osd0\0" \
        "display_color_fg=0xffff\0" \
        "display_color_bg=0\0" \
        "dtb_mem_addr=0x1000000\0" \
        "fb_addr=0x3d800000\0" \
        "fb_width=1920\0" \
        "fb_height=1080\0" \
        "frac_rate_policy=1\0" \
        "sdr2hdr=2\0" \
        "hdmi_read_edid=1\0" \
        "usb_burning=update 1000\0" \
        "otg_device=1\0"\
        "fdt_high=0x20000000\0"\
        "try_auto_burn=update 700 750;\0"\
        "sdcburncfg=aml_sdc_burn.ini\0"\
        "sdc_burning=sdc_burn ${sdcburncfg}\0"\
        "wipe_data=successful\0"\
        "wipe_cache=successful\0"\
        "EnableSelinux=permissive\0" \
        "recovery_part=recovery\0"\
        "recovery_offset=0\0"\
        "cvbs_drv=0\0"\
        "osd_reverse=0\0"\
        "video_reverse=0\0"\
        "lock=10001000\0"\
        "active_slot=normal\0"\
        "boot_part=boot\0"\
        "reboot_mode_android=""normal""\0"\
        "Irq_check_en=0\0"\
        "reboot_mode_android=""normal""\0"\
        "fs_type=""rootfstype=ext4""\0"\
        "initargs="\
            "root=LABEL=ROOTFS rootflags=data=writeback rw rootfstype=ext4 console=tty0 console=ttyS0,115200n8 no_console_suspend consoleblank=0 fsck.repair=yes net.ifnames=0 "\
            "\0"\
        "upgrade_check="\
            "echo upgrade_step=${upgrade_step}; "\
            "if itest ${upgrade_step} == 3; then "\
                "run init_display; run storeargs; run update;"\
            "else fi;"\
            "\0"\
        "storeargs="\
            "setenv bootargs ${initargs} logo=${display_layer},loaded,${fb_addr} vout=${outputmode},enable hdmitx=${cecconfig},${colorattribute} hdmimode=${hdmimode} cvbsmode=${cvbsmode} osd_reverse=${osd_reverse} video_reverse=${video_reverse} jtag=${jtag}; "\
	"setenv bootargs ${bootargs} androidboot.hardware=amlogic reboot_mode=${reboot_mode} ddr_size=${ddr_size};"\
            "run cmdline_keys;"\
            "\0"\
        "switch_bootmode="\
            "get_rebootmode;"\
            "if test ${reboot_mode} = factory_reset; then "\
                    "setenv reboot_mode_android ""normal"";"\
                    "run storeargs;"\
                    "run recovery_from_flash;"\
            "else if test ${reboot_mode} = update; then "\
                    "setenv reboot_mode_android ""normal"";"\
                    "run storeargs;"\
                    "run update;"\
            "else if test ${reboot_mode} = quiescent; then "\
                    "setenv reboot_mode_android ""quiescent"";"\
                    "run storeargs;"\
                    "setenv bootargs ${bootargs} androidboot.quiescent=1;"\
            "else if test ${reboot_mode} = cold_boot; then "\
                    "setenv reboot_mode_android ""normal"";"\
                    "run storeargs;"\
            "else if test ${reboot_mode} = fastboot; then "\
                "setenv reboot_mode_android ""normal"";"\
                "run storeargs;"\
                "fastboot;"\
            "fi;fi;fi;fi;fi;"\
            "\0" \
        "storeboot="\
            "kbi resetflag 0;"\
            "boot_cooling;"\
            "cfgload;" \
            "if load mmc 0:1 1020000 s905_autoscript || load mmc 1:1 1020000 s905_autoscript || load mmc 1:5 1020000 /boot/s905_autoscript; then autoscr 1020000; fi;"\
            "ext4load mmc 1:5 1080000 zImage;ext4load mmc 1:5 10000000 uInitrd;ext4load mmc 1:5 20000000 dtb.img;booti 1080000 10000000 20000000;"\
            "for p in 1 2 3 4 5 6 7 8 9 A B C D E F 10 11 12 13 14 15 16 17 18; do if fatload mmc 1:${p} ${loadaddr} aml_autoscript; then autoscr ${loadaddr}; fi; done;"\
            "run update;"\
            "\0"\
        "factory_reset_poweroff_protect="\
            "echo wipe_data=${wipe_data}; echo wipe_cache=${wipe_cache};"\
            "if test ${wipe_data} = failed; then "\
                "run init_display; run storeargs;"\
                "if mmcinfo; then "\
                    "run recovery_from_sdcard;"\
                "fi;"\
                "if usb start 0; then "\
                    "run recovery_from_udisk;"\
                "fi;"\
                "run recovery_from_flash;"\
            "fi; "\
            "if test ${wipe_cache} = failed; then "\
                "run init_display; run storeargs;"\
                "if mmcinfo; then "\
                    "run recovery_from_sdcard;"\
                "fi;"\
                "if usb start 0; then "\
                    "run recovery_from_udisk;"\
                "fi;"\
                "run recovery_from_flash;"\
            "fi; \0" \
         "update="\
            /*first usb burning, second sdc_burn, third ext-sd autoscr/recovery, last udisk autoscr/recovery*/\
            "kbi lcd_reset; "\
            "run usb_burning; "\
            "run sdc_burning; "\
            "if mmcinfo; then "\
                "run recovery_from_sdcard;"\
            "fi;"\
            "if usb start 0; then "\
                "run recovery_from_udisk;"\
            "fi;"\
            "run recovery_from_flash;"\
            "\0"\
        "recovery_from_sdcard="\
            "if fatload mmc 0 ${loadaddr} aml_autoscript; then autoscr ${loadaddr}; fi;"\
            "if fatload mmc 0 ${loadaddr} recovery.img; then "\
                    "if fatload mmc 0 ${dtb_mem_addr} dtb.img; then echo sd dtb.img loaded; fi;"\
                    "wipeisb; "\
                    "bootm ${loadaddr};fi;"\
            "\0"\
        "recovery_from_udisk="\
            "if fatload usb 0 ${loadaddr} aml_autoscript; then autoscr ${loadaddr}; fi;"\
            "if fatload usb 0 ${loadaddr} recovery.img; then "\
                "if fatload usb 0 ${dtb_mem_addr} dtb.img; then echo udisk dtb.img loaded; fi;"\
                "wipeisb; "\
                "bootm ${loadaddr};fi;"\
            "\0"\
        "recovery_from_flash="\
            "get_valid_slot;"\
            "echo active_slot: ${active_slot};"\
            "if test ${active_slot} = normal; then "\
                "setenv bootargs ${bootargs} aml_dt=${aml_dt} recovery_part={recovery_part} recovery_offset={recovery_offset};"\
                "if imgread kernel ${recovery_part} ${loadaddr} ${recovery_offset}; then wipeisb; bootm ${loadaddr}; fi;"\
            "else "\
                "setenv bootargs ${bootargs} aml_dt=${aml_dt} recovery_part=${boot_part} recovery_offset=${recovery_offset};"\
                "if imgread kernel ${boot_part} ${loadaddr}; then bootm ${loadaddr}; fi;"\
            "fi;"\
            "\0"\
        "init_display="\
            "get_rebootmode;"\
            "echo reboot_mode:::: ${reboot_mode};"\
            "if test ${reboot_mode} = quiescent; then "\
                    "setenv reboot_mode_android ""quiescent"";"\
                    "run storeargs;"\
                    "setenv bootargs ${bootargs} androidboot.quiescent=1;"\
                    "osd open;osd clear;"\
            "else "\
                "setenv reboot_mode_android ""normal"";"\
                "run storeargs;"\
                "hdmitx get_preferred_mode;"\
                "hdmitx edid;"\
                "hdmitx hpd;"\
                "if test ${lcd_exist} = 1 && test ${outputmode} = panel; then "\
                    "if test ${panel_type} = lcd_2; then "\
                        "setenv fb_width 1920;"\
                        "setenv fb_height 1200;"\
                    "else "\
                        "setenv fb_width 1088;"\
                        "setenv fb_height 1920;"\
                    "fi;"\
                "fi;"\
                "osd open;"\
                "osd clear;"\
                "if load mmc 0:2 ${loadaddr} /usr/share/fenix/logo/logo.bmp || load mmc 1:2 ${loadaddr} /usr/share/fenix/logo/logo.bmp || load mmc 1:5 ${loadaddr} /usr/share/fenix/logo/logo.bmp; then "\
                    "bmp display ${loadaddr};"\
                    "bmp scale;"\
                "fi;"\
                "vout output ${outputmode};"\
                "vpp hdrpkt;"\
            "fi;"\
            "\0"\
        "wol_init="\
            "kbi init;"\
            "kbi powerstate;"\
            "kbi trigger wol r;"\
            "setenv bootargs ${bootargs} wol_enable=${wol_enable};"\
            "if test ${power_state} = 1; then "\
            "kbi trigger wol w 1;"\
            "kbi poweroff;"\
            "fi;"\
            "\0"\
        "port_mode_change="\
            "fdt addr ${dtb_mem_addr}; "\
            "kbi portmode r;"\
            "if test ${port_mode} = 0; then "\
                "fdt set /usb3phy@ffe09080 status okay;"\
                "fdt set /pcieA@fc000000 status disable;"\
            "else "\
                "fdt set /usb3phy@ffe09080 status disable;"\
                "fdt set /pcieA@fc000000 status okay;"\
            "fi;"\
            "\0"\
        "cmdline_keys="\
            "if keyman init 0x1234; then "\
                "if keyman read usid ${loadaddr} str; then "\
                    "setenv bootargs ${bootargs} androidboot.serialno=${usid};"\
                    "setenv serial ${usid};"\
                "else "\
                    "setenv bootargs ${bootargs} androidboot.serialno=1234567890;"\
                    "setenv serial 1234567890;"\
                "fi;"\
                "kbi ethmac;"\
                "setenv bootargs ${bootargs} mac=${eth_mac} androidboot.mac=${eth_mac};"\
                "if keyman read deviceid ${loadaddr} str; then "\
                    "setenv bootargs ${bootargs} androidboot.deviceid=${deviceid};"\
                "fi;"\
            "fi;"\
            "\0"\
        "bcb_cmd="\
            "get_avb_mode;"\
            "get_valid_slot;"\
            "\0"\
        "upgrade_key="\
            "if gpio input GPIOAO_7; then "\
                "echo detect upgrade key;"\
                "gpio set GPIOAO_4;"\
                "run fan_stop;"\
                "run update;"\
            "fi;"\
            "\0"\
	    "irremote_update="\
            "if irkey 2500000 0xe31cfb04 0xb748fb04; then "\
                "echo read irkey ok!; " \
                "if itest ${irkey_value} == 0xe31cfb04; then " \
                    "run update;" \
                "else if itest ${irkey_value} == 0xb748fb04; then " \
                    "run update;\n" \
			    "fi;fi;" \
            "fi;\0" \
        "updateu="\
            "tftp 1080000 u-boot.bin;"\
            "mmc dev 1;"\
            "store rom_write 1080000 0 ${filesize}"\
            "\0" \
        "vim3_check="\
            "kbi hwver; "\
            "if test ${hwver} != Unknow; then " \
                "echo Product checking: pass! Hardware version: ${hwver};" \
            "else " \
                "echo Product checking: fail!; sleep 5; reboot;" \
            "fi;" \
            "setenv bootargs ${bootargs} hwver=${hwver};"\
            "\0"\
        "fan_stop=" \
            "i2c mw 0x18 0x88 0" \
            "\0"\

#define CONFIG_PREBOOT  \
            "run upgrade_check;"\
            "run init_display;"\
            "run storeargs;"\
            "run upgrade_key;"\
            "run vim3_check;" \
            "run wol_init;"\
            "run port_mode_change;"\
            "forceupdate;" \
            "run fan_stop;"

#define CONFIG_BOOTCOMMAND "run storeboot"

//#define CONFIG_ENV_IS_NOWHERE  1
#define CONFIG_ENV_SIZE   (64*1024)
#define CONFIG_FIT 1
#define CONFIG_OF_LIBFDT 1
#define CONFIG_ANDROID_BOOT_IMAGE 1
#define CONFIG_ANDROID_IMG 1
#define CONFIG_SYS_BOOTM_LEN (64<<20) /* Increase max gunzip size*/

/* cpu */
#define CONFIG_CPU_CLK					1200 //MHz. Range: 360-2000, should be multiple of 24

/* ATTENTION */
/* DDR configs move to board/amlogic/[board]/firmware/timing.c */

//#define CONFIG_DDR_AUTO_DTB             1

#define CONFIG_NR_DRAM_BANKS			1
/* ddr functions */
#define CONFIG_DDR_FULL_TEST			0 //0:disable, 1:enable. ddr full test
#define CONFIG_CMD_DDR_D2PLL			0 //0:disable, 1:enable. d2pll cmd
#define CONFIG_CMD_DDR_TEST				0 //0:disable, 1:enable. ddrtest cmd
#define CONFIG_CMD_DDR_TEST_G12			0 //0:disable, 1:enable. G12 ddrtest cmd
#define CONFIG_DDR_LOW_POWER			0 //0:disable, 1:enable. ddr clk gate for lp
#define CONFIG_DDR_ZQ_PD				0 //0:disable, 1:enable. ddr zq power down
#define CONFIG_DDR_USE_EXT_VREF			0 //0:disable, 1:enable. ddr use external vref
#define CONFIG_DDR4_TIMING_TEST			0 //0:disable, 1:enable. ddr4 timing test function
#define CONFIG_DDR_PLL_BYPASS			0 //0:disable, 1:enable. ddr pll bypass function
#define CONFIG_DDR_NONSEC_SCRAMBLE		0 //0:disable, 1:enable. non-sec region scramble function

/* storage: emmc/nand/sd */
#define		CONFIG_STORE_COMPATIBLE 1
#define 	CONFIG_ENV_OVERWRITE
#define 	CONFIG_CMD_SAVEENV
/* fixme, need fix*/

#if (defined(CONFIG_ENV_IS_IN_AMLNAND) || defined(CONFIG_ENV_IS_IN_MMC)) && defined(CONFIG_STORE_COMPATIBLE)
#error env in amlnand/mmc already be compatible;
#endif

/*
*				storage
*		|---------|---------|
*		|					|
*		emmc<--Compatible-->nand
*					|-------|-------|
*					|				|
*					MTD<-Exclusive->NFTL
*/
/* axg only support slc nand */
/* swither for mtd nand which is for slc only. */
/* support for mtd */
//#define CONFIG_AML_MTD 1

#ifdef CONFIG_AML_MTD

/* bootlaoder is construct by bl2 and fip
 * when DISCRETE_BOOTLOADER is enabled, bl2 & fip
 * will not be stored continuously, and nand layout
 * would be bl2|rsv|fip|normal, but not
 * bl2|fip|rsv|noraml anymore
 */
#define CONFIG_DISCRETE_BOOTLOADER

#ifdef  CONFIG_DISCRETE_BOOTLOADER
#define CONFIG_TPL_SIZE_PER_COPY          0x200000
#define CONFIG_TPL_COPY_NUM               4
#define CONFIG_TPL_PART_NAME              "tpl"
/* for bl2, restricted by romboot */
//SKT 1024 pages only support 4 block, so 4 copies
#define CONFIG_BL2_COPY_NUM               4
#endif /* CONFIG_DISCRETE_BOOTLOADER */

#define CONFIG_MTD_DEVICE y
/* mtd parts of ourown.*/
#define CONFIFG_AML_MTDPART	1
/* mtd parts by env default way.*/
/*
#define MTDIDS_NAME_STR		"aml_nand.0"
#define MTDIDS_DEFAULT		"nand1=" MTDIDS_NAME_STR
#define MTDPARTS_DEFAULT	"mtdparts=" MTDIDS_NAME_STR ":" \
					"3M@8192K(logo),"	\
					"10M(recovery),"	\
					"8M(kernel),"	\
					"40M(rootfs),"	\
					"-(data)"
*/
#define CONFIG_CMD_UBI
#define CONFIG_CMD_UBIFS
#define CONFIG_RBTREE
#define CONFIG_CMD_MTDPARTS   1
#define CONFIG_MTD_PARTITIONS 1
#endif
/* endof CONFIG_AML_MTD */
#define		CONFIG_AML_SD_EMMC 1
#ifdef		CONFIG_AML_SD_EMMC
	#define 	CONFIG_GENERIC_MMC 1
	#define 	CONFIG_CMD_MMC 1
	#define CONFIG_CMD_GPT 1
	#define	CONFIG_SYS_MMC_ENV_DEV 1
	#define CONFIG_EMMC_DDR52_EN 0
	#define CONFIG_EMMC_DDR52_CLK 35000000
#endif
#define		CONFIG_PARTITIONS 1
#define 	CONFIG_SYS_NO_FLASH  1
//#define     CONFIG_AML_GPT

/* meson SPI */
#define CONFIG_AML_SPIFC
#if defined CONFIG_AML_SPIFC || defined CONFIG_AML_SPICC
	#define CONFIG_OF_SPI
	#define CONFIG_DM_SPI
	#define CONFIG_CMD_SPI
#endif
/* SPI flash config */
#ifdef CONFIG_AML_SPIFC
	#define CONFIG_SPI_FLASH
	#define CONFIG_DM_SPI_FLASH
	#define CONFIG_CMD_SF
	/* SPI flash surpport list */
	#define CONFIG_SPI_FLASH_ATMEL
	#define CONFIG_SPI_FLASH_EON
	#define CONFIG_SPI_FLASH_GIGADEVICE
	#define CONFIG_SPI_FLASH_MACRONIX
	#define CONFIG_SPI_FLASH_SPANSION
	#define CONFIG_SPI_FLASH_STMICRO
	#define CONFIG_SPI_FLASH_SST
	#define CONFIG_SPI_FLASH_WINBOND
	#define CONFIG_SPI_FRAM_RAMTRON
	#define CONFIG_SPI_M95XXX
	#define CONFIG_SPI_FLASH_ESMT
	/* SPI nand flash support */
	#define CONFIG_BL2_SIZE (64 * 1024)
#endif

#if defined CONFIG_AML_MTD || defined CONFIG_SPI_NAND
	#define CONFIG_MTD_DEVICE y
	#define CONFIG_RBTREE
	#define CONFIG_CMD_MTDPARTS   1
	#define CONFIG_MTD_PARTITIONS 1
#endif

/* vpu */
#define CONFIG_AML_VPU 1
//#define CONFIG_VPU_CLK_LEVEL_DFT 7

/* DISPLAY & HDMITX */
#define CONFIG_AML_HDMITX20 1
#define CONFIG_AML_CANVAS 1
#define CONFIG_AML_VOUT 1
#define CONFIG_AML_OSD 1
#define CONFIG_OSD_SCALE_ENABLE 1
#define CONFIG_CMD_BMP 1
#define CONFIG_AML_MINUI 1

#if defined(CONFIG_AML_VOUT)
#define CONFIG_AML_CVBS 1
#endif

#define CONFIG_AML_LCD    1
#define CONFIG_AML_LCD_TABLET 1
#define CONFIG_AML_LCD_EXTERN 1

/* USB
 * Enable CONFIG_MUSB_HCD for Host functionalities MSC, keyboard
 * Enable CONFIG_MUSB_UDD for Device functionalities.
 */
/* #define CONFIG_MUSB_UDC		1 */
#define CONFIG_CMD_USB 1
#if defined(CONFIG_CMD_USB)
	#define CONFIG_GXL_XHCI_BASE            0xff500000
	#define CONFIG_GXL_USB_PHY2_BASE        0xffe09000
	#define CONFIG_GXL_USB_PHY3_BASE        0xffe09080
	#define CONFIG_USB_PHY_20				0xff636000
	#define CONFIG_USB_PHY_21				0xff63A000
	#define CONFIG_USB_STORAGE      1
	#define CONFIG_USB_XHCI		1
	#define CONFIG_USB_XHCI_AMLOGIC_V2 1
	#define CONFIG_USB_GPIO_PWR  			GPIOEE(GPIOA_6)
	#define CONFIG_USB_GPIO_PWR_NAME		"GPIOA_6"
	//#define CONFIG_USB_XHCI_AMLOGIC_USB3_V2		1
#endif //#if defined(CONFIG_CMD_USB)

#define CONFIG_TXLX_USB        1
#define CONFIG_USB_DEVICE_V2    1
#define USB_PHY2_PLL_PARAMETER_1	0x09400414
#define USB_PHY2_PLL_PARAMETER_2	0x927e0000
#define USB_PHY2_PLL_PARAMETER_3	0xAC5F49E5
#define USB_G12x_PHY_PLL_SETTING_1	(0xfe18)
#define USB_G12x_PHY_PLL_SETTING_2	(0xfff)
#define USB_G12x_PHY_PLL_SETTING_3	(0x78000)
#define USB_G12x_PHY_PLL_SETTING_4	(0xe0004)
#define USB_G12x_PHY_PLL_SETTING_5	(0xe000c)

//UBOOT fastboot config
#define CONFIG_CMD_FASTBOOT 1
#define CONFIG_FASTBOOT_FLASH_MMC_DEV 1
#define CONFIG_FASTBOOT_FLASH 1
#define CONFIG_USB_GADGET 1
#define CONFIG_USBDOWNLOAD_GADGET 1
#define CONFIG_SYS_CACHELINE_SIZE 64
#define CONFIG_FASTBOOT_MAX_DOWN_SIZE	0x8000000
#define CONFIG_DEVICE_PRODUCT	"kvim3l"

//UBOOT Facotry usb/sdcard burning config
#define CONFIG_AML_V2_FACTORY_BURN              1       //support facotry usb burning
#define CONFIG_AML_FACTORY_BURN_LOCAL_UPGRADE   1       //support factory sdcard burning
#define CONFIG_POWER_KEY_NOT_SUPPORTED_FOR_BURN 1       //There isn't power-key for factory sdcard burning
#define CONFIG_SD_BURNING_SUPPORT_UI            1       //Displaying upgrading progress bar when sdcard/udisk burning

#define CONFIG_AML_SECURITY_KEY                 1
#define CONFIG_UNIFY_KEY_MANAGE                 1

/* net */
#define CONFIG_CMD_NET   1
/*#define CONFIG_PHY_REALTEK 1*/
#if defined(CONFIG_CMD_NET)
	#define CONFIG_DESIGNWARE_ETH 1
	#define CONFIG_PHYLIB	1
	#define CONFIG_NET_MULTI 1
	#define CONFIG_CMD_PING 1
	#define CONFIG_CMD_DHCP 1
	#define CONFIG_CMD_RARP 1
	#define CONFIG_HOSTNAME        KVIM3L
//	#define CONFIG_RANDOM_ETHADDR  1				   /* use random eth addr, or default */
	#define CONFIG_ETHADDR         00:15:18:01:81:31   /* Ethernet address */
	#define CONFIG_IPADDR          192.168.1.200          /* Our ip address */
	#define CONFIG_GATEWAYIP       192.168.1.1           /* Our getway ip address */
	#define CONFIG_SERVERIP        192.168.1.230         /* Tftp server ip address */
	#define CONFIG_NETMASK         255.255.255.0
#endif /* (CONFIG_CMD_NET) */

/* other devices */
/* I2C DM driver*/
//#define CONFIG_DM_I2C
#if defined(CONFIG_DM_I2C)
#define CONFIG_SYS_I2C_MESON		1
#else
#define CONFIG_SYS_I2C_AML			1
#define CONFIG_SYS_I2C_SPEED		400000
#endif

/* PWM DM driver*/
#define CONFIG_DM_PWM
#define CONFIG_PWM_MESON

#define CONFIG_EFUSE 1

/* commands */
#define CONFIG_CMD_CACHE 1
#define CONFIG_CMD_BOOTI 1
#define CONFIG_CMD_EFUSE 1
#define CONFIG_CMD_I2C 1
#define CONFIG_CMD_MEMORY 1
#define CONFIG_CMD_FAT 1
#define CONFIG_CMD_GPIO 1
#define CONFIG_CMD_RUN
#define CONFIG_CMD_REBOOT 1
#define CONFIG_CMD_ECHO 1
#define CONFIG_CMD_JTAG	1
#define CONFIG_CMD_AUTOSCRIPT 1
#define CONFIG_CMD_MISC 1
#define CONFIG_CMD_PLLTEST 1
#define CONFIG_CMD_EXT4 1
#define CONFIG_CMD_EXT2 1
#define CONFIG_CMD_ENV_EXISTS 1
#define CONFIG_CMD_FS_GENERIC 1
#define CONFIG_CMD_PART 1

/*file system*/
#define CONFIG_DOS_PARTITION 1
#define CONFIG_EFI_PARTITION 1
#define CONFIG_AML_PARTITION 1
#define CONFIG_MMC 1
#define CONFIG_FS_FAT 1
#define CONFIG_FS_EXT4 1
#define CONFIG_LZO 1
#define CONFIG_CMD_UNZIP    1
#define CONFIG_LZMA         1
#define CONFIG_FAT_WRITE	1
#define CONFIG_CMD_EXT4_WRITE	1

//#define CONFIG_MDUMP_COMPRESS 1

/* Cache Definitions */
//#define CONFIG_SYS_DCACHE_OFF
//#define CONFIG_SYS_ICACHE_OFF

/* other functions */
#define CONFIG_NEED_BL301	1
#define CONFIG_NEED_BL32	1
#define CONFIG_CMD_RSVMEM	1
#define CONFIG_FIP_IMG_SUPPORT	1
#define CONFIG_BOOTDELAY	1 //delay 1s
#define CONFIG_SYS_LONGHELP 1
#define CONFIG_CMD_MISC     1
#define CONFIG_CMD_ITEST    1
#define CONFIG_CMD_CPU_TEMP 1
#define CONFIG_SYS_MEM_TOP_HIDE 0x08000000 //hide 128MB for kernel reserve
#define CONFIG_CMD_LOADB    1

#define CONFIG_CMDLINE_EDITING 1
#define CONFIG_AUTO_COMPLETE 1
#define CONFIG_CMD_CHIPID 1

/* debug mode defines */
//#define CONFIG_DEBUG_MODE           1
#ifdef CONFIG_DEBUG_MODE
#define CONFIG_DDR_CLK_DEBUG        636
#define CONFIG_CPU_CLK_DEBUG        600
#endif

//support secure boot
#define CONFIG_AML_SECURE_UBOOT   1

#if defined(CONFIG_AML_SECURE_UBOOT)

//unify build for generate encrypted bootloader "u-boot.bin.encrypt"
#define CONFIG_AML_CRYPTO_UBOOT   1

//unify build for generate encrypted kernel image
//SRC : "board/amlogic/(board)/boot.img"
//DST : "fip/boot.img.encrypt"
//#define CONFIG_AML_CRYPTO_IMG       1

#endif //CONFIG_AML_SECURE_UBOOT

#define CONFIG_SECURE_STORAGE 1

//build with uboot auto test
//#define CONFIG_AML_UBOOT_AUTO_TEST 1

//board customer ID
//#define CONFIG_CUSTOMER_ID  (0x6472616F624C4D41)

#if defined(CONFIG_CUSTOMER_ID)
  #undef CONFIG_AML_CUSTOMER_ID
  #define CONFIG_AML_CUSTOMER_ID  CONFIG_CUSTOMER_ID
#endif

/* Choose One of Ethernet Type */
#undef CONFIG_ETHERNET_NONE
#define ETHERNET_EXTERNAL_PHY
#undef  ETHERNET_INTERNAL_PHY

#define CONFIG_CMD_AML_MTEST 1
#if defined(CONFIG_CMD_AML_MTEST)
#if !defined(CONFIG_SYS_MEM_TOP_HIDE)
#error CONFIG_CMD_AML_MTEST depends on CONFIG_SYS_MEM_TOP_HIDE;
#endif
#if !(CONFIG_SYS_MEM_TOP_HIDE)
#error CONFIG_SYS_MEM_TOP_HIDE should not be zero;
#endif
#endif

#define CONFIG_HIGH_TEMP_COOL 90
#endif

