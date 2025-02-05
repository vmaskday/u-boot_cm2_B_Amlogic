
/*
 * board/khadas/kvim3l/kvim3l.c
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

#include <common.h>
#include <asm/gpio.h>
#include <khadas_tca6408.h>
#include <malloc.h>
#include <errno.h>
#include <environment.h>
#include <fdt_support.h>
#include <libfdt.h>
#include <asm/cpu_id.h>
#include <asm/arch/secure_apb.h>
#ifdef CONFIG_SYS_I2C_AML
#include <aml_i2c.h>
#endif
#ifdef CONFIG_SYS_I2C_MESON
#include <amlogic/i2c.h>
#endif
#ifdef CONFIG_PWM_MESON
#include <pwm.h>
#include <amlogic/pwm.h>
#endif
#ifdef CONFIG_AML_VPU
#include <vpu.h>
#endif
#include <vpp.h>
#ifdef CONFIG_AML_V2_FACTORY_BURN
#include <amlogic/aml_v2_burning.h>
#endif// #ifdef CONFIG_AML_V2_FACTORY_BURN
#ifdef CONFIG_AML_HDMITX20
#include <amlogic/hdmi.h>
#endif
#ifdef CONFIG_AML_LCD
#include <amlogic/aml_lcd.h>
#endif
#include <asm/arch/eth_setup.h>
#include <phy.h>
#include <linux/mtd/partitions.h>
#include <linux/sizes.h>
#include <asm-generic/gpio.h>
#include <dm.h>
#ifdef CONFIG_AML_SPIFC
#include <amlogic/spifc.h>
#endif
#ifdef CONFIG_TCA6408
#include <khadas_tca6408.h>
#endif
#ifdef CONFIG_POWER_FUSB302
#include <fusb302.h>
#endif
#include <asm/arch/timer.h>

#define SW_I2C_DELAY() udelay(2)
#define SW_I2C_SCL_H() set_gpio_level(1,1) //TO-DO: set scl pin to high
#define SW_I2C_SCL_L() set_gpio_level(1,0) //TO-DO: set scl pin to low
#define SW_I2C_SDA_H() set_gpio_level(0,1) //TO-DO: set sda pin to high
#define SW_I2C_SDA_L() set_gpio_level(0,0) //TO-DO: set sda pin to low
#define SW_I2C_SDA_IS_H() get_sda_gpio_level() //TO-DO: get sda pin level

DECLARE_GLOBAL_DATA_PTR;

static bool get_sda_gpio_level(void)
{
	writel(readl(PREG_PAD_GPIO5_EN_N) | (1 << 14), PREG_PAD_GPIO5_EN_N);
	if (((readl(PREG_PAD_GPIO5_I) & 0x4000) == 0x4000)) {
		return 1;
	}
	return 0;
}

static void set_gpio_level(int pin, int high)
{
	//pin 0: sda 1: scl
	if (pin == 1){ //scl
		if (high == 1) {
			writel(readl(PREG_PAD_GPIO5_O) | (1 << 15), PREG_PAD_GPIO5_O);
			writel(readl(PREG_PAD_GPIO5_EN_N) & (~(1 << 15)), PREG_PAD_GPIO5_EN_N);
		} else {
			writel(readl(PREG_PAD_GPIO5_O) & (~(1 << 15)), PREG_PAD_GPIO5_O);
			writel(readl(PREG_PAD_GPIO5_EN_N) & (~(1 << 15)), PREG_PAD_GPIO5_EN_N);
		}
		writel(readl(PERIPHS_PIN_MUX_E) & (~(0xf << 28)), PERIPHS_PIN_MUX_E);
	} else { //sda
		if (high == 1) {
			writel(readl(PREG_PAD_GPIO5_O) | (1 << 14), PREG_PAD_GPIO5_O);
			writel(readl(PREG_PAD_GPIO5_EN_N) & (~(1 << 14)), PREG_PAD_GPIO5_EN_N);
		} else {
			writel(readl(PREG_PAD_GPIO5_O) & (~(1 << 14)), PREG_PAD_GPIO5_O);
			writel(readl(PREG_PAD_GPIO5_EN_N) & (~(1 << 14)), PREG_PAD_GPIO5_EN_N);
		}
		writel(readl(PERIPHS_PIN_MUX_E) & (~(0xf << 24)), PERIPHS_PIN_MUX_E);
	}
}

void sw_i2c_start(void)
{
	SW_I2C_SDA_H();
	udelay(50);
	SW_I2C_SCL_H();
	SW_I2C_DELAY();
	SW_I2C_DELAY();

	SW_I2C_SDA_L();
	SW_I2C_DELAY();
	SW_I2C_SCL_L();
	SW_I2C_DELAY();
}

void sw_i2c_stop(void)
{
	SW_I2C_SCL_H();
	SW_I2C_SDA_L();

	SW_I2C_DELAY();
	SW_I2C_DELAY();

	SW_I2C_SDA_H();
	SW_I2C_DELAY();
}

void sw_i2c_tx_ack(void)
{
	SW_I2C_SDA_L();
	SW_I2C_DELAY();

	SW_I2C_SCL_H();
	SW_I2C_DELAY();

	SW_I2C_SCL_L();
	SW_I2C_DELAY();

	SW_I2C_SDA_H();
}

void sw_i2c_tx_nack(void)
{
	SW_I2C_SDA_H();
	SW_I2C_DELAY();

	SW_I2C_SCL_H();
	SW_I2C_DELAY();
	SW_I2C_DELAY();

	SW_I2C_SCL_L();
	SW_I2C_DELAY();
	SW_I2C_DELAY();
}

uint8_t sw_i2c_rx_ack(void)
{
	uint8_t ret = 0;
	uint16_t ucErrTime = 0;

	SW_I2C_SDA_IS_H();
	SW_I2C_DELAY();
	SW_I2C_SCL_H();
	SW_I2C_DELAY();
	SW_I2C_DELAY();

	while(SW_I2C_SDA_IS_H()){
		if((++ucErrTime) > 250) {
			sw_i2c_stop();
			return 1;
		}
	}

	SW_I2C_SCL_L();
	SW_I2C_DELAY();

	return ret;
}

void sw_i2c_tx_byte(uint8_t dat)
{
	uint8_t i = 0;
	uint8_t temp = dat;

	for (i = 0; i < 8; i++) {
		if (temp & 0x80) {
			SW_I2C_SDA_H();
		} else {
			SW_I2C_SDA_L();
		}
		SW_I2C_DELAY();

		SW_I2C_SCL_H();
		SW_I2C_DELAY();
		SW_I2C_DELAY();

		SW_I2C_SCL_L();
		SW_I2C_DELAY();

		temp <<= 1;
	}
}

uint8_t sw_i2c_rx_byte(void)
{
	uint8_t i = 0;
	uint8_t dat = 0;

	for (i = 0; i < 8; i++) {
		dat <<= 1;

		SW_I2C_SCL_H();
		SW_I2C_DELAY();
		SW_I2C_DELAY();

		if (SW_I2C_SDA_IS_H()) {
			dat++;
		}

		SW_I2C_SCL_L();
		SW_I2C_DELAY();
		SW_I2C_DELAY();
	}
	return dat;
}

void sw_i2c_read(uint8_t device_addr, uint8_t reg_addr, uint8_t dat[], uint8_t len)
{
	uint8_t i = 0;
	sw_i2c_start();
	sw_i2c_tx_byte(device_addr & 0xFE);
	sw_i2c_rx_ack();
	sw_i2c_tx_byte(reg_addr);
	sw_i2c_rx_ack();

	sw_i2c_start();
	sw_i2c_tx_byte(device_addr | 0x01);
	sw_i2c_rx_ack();

	for (i = 0; i < len; i++) {
		dat[i] = sw_i2c_rx_byte();
		if (i == (len-1))
		{
			sw_i2c_tx_nack();
		} else {
			sw_i2c_tx_ack();
		}
	}
	sw_i2c_stop();
}

//new static eth setup
struct eth_board_socket*  eth_board_skt;

void sys_led_init(void)
{
	//set GPIOAO_11 drive strength
	setbits_le32(AO_PAD_DS_A,(3<<22)); //GPIOAO_11 set drive strength "3"
}

int serial_set_pin_port(unsigned long port_base)
{
    //UART in "Always On Module"
    //GPIOAO_0==tx,GPIOAO_1==rx
    //setbits_le32(P_AO_RTI_PIN_MUX_REG,3<<11);
    return 0;
}

int dram_init(void)
{
	gd->ram_size = PHYS_SDRAM_1_SIZE;
	return 0;
}

/* secondary_boot_func
 * this function should be write with asm, here, is is only for compiling pass
 * */
void secondary_boot_func(void)
{
}
#ifdef  ETHERNET_INTERNAL_PHY
void internalPhyConfig(struct phy_device *phydev)
{
}

static int dwmac_meson_cfg_pll(void)
{
	writel(0x39C0040A, P_ETH_PLL_CTL0);
	writel(0x927E0000, P_ETH_PLL_CTL1);
	writel(0xAC5F49E5, P_ETH_PLL_CTL2);
	writel(0x00000000, P_ETH_PLL_CTL3);
	udelay(200);
	writel(0x19C0040A, P_ETH_PLL_CTL0);
	return 0;
}

static int dwmac_meson_cfg_analog(void)
{
	/*Analog*/
	writel(0x20200000, P_ETH_PLL_CTL5);
	writel(0x0000c002, P_ETH_PLL_CTL6);
	writel(0x00000023, P_ETH_PLL_CTL7);

	return 0;
}

static int dwmac_meson_cfg_ctrl(void)
{
	/*config phyid should between  a 0~0xffffffff*/
	/*please don't use 44000181, this has been used by internal phy*/
	writel(0x33000180, P_ETH_PHY_CNTL0);

	/*use_phy_smi | use_phy_ip | co_clkin from eth_phy_top*/
	writel(0x260, P_ETH_PHY_CNTL2);

	writel(0x74043, P_ETH_PHY_CNTL1);
	writel(0x34043, P_ETH_PHY_CNTL1);
	writel(0x74043, P_ETH_PHY_CNTL1);
	return 0;
}

static void setup_net_chip(void)
{
	eth_aml_reg0_t eth_reg0;

	eth_reg0.d32 = 0;
	eth_reg0.b.phy_intf_sel = 4;
	eth_reg0.b.rx_clk_rmii_invert = 0;
	eth_reg0.b.rgmii_tx_clk_src = 0;
	eth_reg0.b.rgmii_tx_clk_phase = 0;
	eth_reg0.b.rgmii_tx_clk_ratio = 4;
	eth_reg0.b.phy_ref_clk_enable = 1;
	eth_reg0.b.clk_rmii_i_invert = 1;
	eth_reg0.b.clk_en = 1;
	eth_reg0.b.adj_enable = 1;
	eth_reg0.b.adj_setup = 0;
	eth_reg0.b.adj_delay = 9;
	eth_reg0.b.adj_skew = 0;
	eth_reg0.b.cali_start = 0;
	eth_reg0.b.cali_rise = 0;
	eth_reg0.b.cali_sel = 0;
	eth_reg0.b.rgmii_rx_reuse = 0;
	eth_reg0.b.eth_urgent = 0;
	setbits_le32(P_PREG_ETH_REG0, eth_reg0.d32);// rmii mode

	dwmac_meson_cfg_pll();
	dwmac_meson_cfg_analog();
	dwmac_meson_cfg_ctrl();

	/* eth core clock */
	setbits_le32(HHI_GCLK_MPEG1, (0x1 << 3));
	/* eth phy clock */
	setbits_le32(HHI_GCLK_MPEG0, (0x1 << 4));

	/* eth phy pll, clk50m */
	setbits_le32(HHI_FIX_PLL_CNTL3, (0x1 << 5));

	/* power on memory */
	clrbits_le32(HHI_MEM_PD_REG0, (1 << 3) | (1<<2));
}
#endif

#ifdef ETHERNET_EXTERNAL_PHY

static int dwmac_meson_cfg_drive_strength(void)
{
	writel(0xaaaaaaa5, P_PAD_DS_REG4A);
	return 0;
}

static void setup_net_chip_ext(void)
{
	eth_aml_reg0_t eth_reg0;
	writel(0x11111111, P_PERIPHS_PIN_MUX_6);
	writel(0x111111, P_PERIPHS_PIN_MUX_7);

	eth_reg0.d32 = 0;
	eth_reg0.b.phy_intf_sel = 1;
	eth_reg0.b.rx_clk_rmii_invert = 0;
	eth_reg0.b.rgmii_tx_clk_src = 0;
	eth_reg0.b.rgmii_tx_clk_phase = 1;
	eth_reg0.b.rgmii_tx_clk_ratio = 4;
	eth_reg0.b.phy_ref_clk_enable = 1;
	eth_reg0.b.clk_rmii_i_invert = 0;
	eth_reg0.b.clk_en = 1;
	eth_reg0.b.adj_enable = 0;
	eth_reg0.b.adj_setup = 0;
	eth_reg0.b.adj_delay = 0;
	eth_reg0.b.adj_skew = 0;
	eth_reg0.b.cali_start = 0;
	eth_reg0.b.cali_rise = 0;
	eth_reg0.b.cali_sel = 0;
	eth_reg0.b.rgmii_rx_reuse = 0;
	eth_reg0.b.eth_urgent = 0;
	setbits_le32(P_PREG_ETH_REG0, eth_reg0.d32);// rmii mode

	setbits_le32(HHI_GCLK_MPEG1, 0x1 << 3);
	/* power on memory */
	clrbits_le32(HHI_MEM_PD_REG0, (1 << 3) | (1<<2));
}
#endif
extern struct eth_board_socket* eth_board_setup(char *name);
extern int designware_initialize(ulong base_addr, u32 interface);

int board_eth_init(bd_t *bis)
{
#ifdef CONFIG_ETHERNET_NONE
	return 0;
#endif

#ifdef ETHERNET_EXTERNAL_PHY
	dwmac_meson_cfg_drive_strength();
	setup_net_chip_ext();
#endif
#ifdef ETHERNET_INTERNAL_PHY
	setup_net_chip();
#endif
	udelay(1000);
	designware_initialize(ETH_BASE, PHY_INTERFACE_MODE_RMII);
	return 0;
}

#if CONFIG_AML_SD_EMMC
#include <mmc.h>
#include <asm/arch/sd_emmc.h>
static int  sd_emmc_init(unsigned port)
{
    switch (port)
	{
		case SDIO_PORT_A:
			break;
		case SDIO_PORT_B:
			//todo add card detect
			/* check card detect */
			clrbits_le32(P_PERIPHS_PIN_MUX_9, 0xF << 24);
			setbits_le32(P_PREG_PAD_GPIO1_EN_N, 1 << 6);
			setbits_le32(P_PAD_PULL_UP_EN_REG1, 1 << 6);
			setbits_le32(P_PAD_PULL_UP_REG1, 1 << 6);
			break;
		case SDIO_PORT_C:
			//enable pull up
			//clrbits_le32(P_PAD_PULL_UP_REG3, 0xff<<0);
			break;
		default:
			break;
	}

	return cpu_sd_emmc_init(port);
}

extern unsigned sd_debug_board_1bit_flag;


static void sd_emmc_pwr_prepare(unsigned port)
{
	cpu_sd_emmc_pwr_prepare(port);
}

static void sd_emmc_pwr_on(unsigned port)
{
    switch (port)
	{
		case SDIO_PORT_A:
			break;
		case SDIO_PORT_B:
//            clrbits_le32(P_PREG_PAD_GPIO5_O,(1<<31)); //CARD_8
//            clrbits_le32(P_PREG_PAD_GPIO5_EN_N,(1<<31));
			/// @todo NOT FINISH
			break;
		case SDIO_PORT_C:
			break;
		default:
			break;
	}
	return;
}
static void sd_emmc_pwr_off(unsigned port)
{
	/// @todo NOT FINISH
    switch (port)
	{
		case SDIO_PORT_A:
			break;
		case SDIO_PORT_B:
//            setbits_le32(P_PREG_PAD_GPIO5_O,(1<<31)); //CARD_8
//            clrbits_le32(P_PREG_PAD_GPIO5_EN_N,(1<<31));
			break;
		case SDIO_PORT_C:
			break;
				default:
			break;
	}
	return;
}

// #define CONFIG_TSD      1
static void board_mmc_register(unsigned port)
{
	struct aml_card_sd_info *aml_priv=cpu_sd_emmc_get(port);
    if (aml_priv == NULL)
		return;

	aml_priv->sd_emmc_init=sd_emmc_init;
	aml_priv->sd_emmc_detect=sd_emmc_detect;
	aml_priv->sd_emmc_pwr_off=sd_emmc_pwr_off;
	aml_priv->sd_emmc_pwr_on=sd_emmc_pwr_on;
	aml_priv->sd_emmc_pwr_prepare=sd_emmc_pwr_prepare;
	aml_priv->desc_buf = malloc(NEWSD_MAX_DESC_MUN*(sizeof(struct sd_emmc_desc_info)));

	if (NULL == aml_priv->desc_buf)
		printf(" desc_buf Dma alloc Fail!\n");
	else
		printf("aml_priv->desc_buf = 0x%p\n",aml_priv->desc_buf);

	sd_emmc_register(aml_priv);
}
int board_mmc_init(bd_t	*bis)
{
#ifdef CONFIG_VLSI_EMULATOR
	//board_mmc_register(SDIO_PORT_A);
#else
	//board_mmc_register(SDIO_PORT_B);
#endif
	board_mmc_register(SDIO_PORT_B);
	board_mmc_register(SDIO_PORT_C);
//	board_mmc_register(SDIO_PORT_B1);
	return 0;
}

#ifdef CONFIG_SYS_I2C_AML
static void board_i2c_set_pinmux(void){

	//disable all other pins which share with I2C_SDA_AO & I2C_SCK_AO
	clrbits_le32(P_AO_RTI_PINMUX_REG0, ((1<<8)|(1<<9)|(1<<10)|(1<<11)));
	clrbits_le32(P_AO_RTI_PINMUX_REG0, ((1<<12)|(1<<13)|(1<<14)|(1<<15)));
	//enable I2C MASTER AO pins
	setbits_le32(P_AO_RTI_PINMUX_REG0,
	(MESON_I2C_MASTER_AO_GPIOAO_2_BIT | MESON_I2C_MASTER_AO_GPIOAO_3_BIT));

	udelay(10);
};
struct aml_i2c_platform g_aml_i2c_plat = {
	.wait_count         = 1000000,
	.wait_ack_interval  = 5,
	.wait_read_interval = 5,
	.wait_xfer_interval = 5,
	.master_no          = AML_I2C_MASTER_AO,
	.use_pio            = 0,
	.master_i2c_speed   = AML_I2C_SPPED_400K,
	.master_ao_pinmux = {
		.scl_reg    = (unsigned long)MESON_I2C_MASTER_AO_GPIOAO_2_REG,
		.scl_bit    = MESON_I2C_MASTER_AO_GPIOAO_2_BIT,
		.sda_reg    = (unsigned long)MESON_I2C_MASTER_AO_GPIOAO_3_REG,
		.sda_bit    = MESON_I2C_MASTER_AO_GPIOAO_3_BIT,
	}
};
static void board_i2c_init(void)
{
	//set I2C pinmux with PCB board layout
	board_i2c_set_pinmux();

	//Amlogic I2C controller initialized
	//note: it must be call before any I2C operation
	i2c_plat_init();
	aml_i2c_init();

	udelay(10);
}
#endif
#endif

#if defined(CONFIG_BOARD_EARLY_INIT_F)
int board_early_init_f(void){
	/*add board early init function here*/
	return 0;
}
#endif

#ifdef CONFIG_USB_XHCI_AMLOGIC_V2
#include <asm/arch/usb-v2.h>
#include <asm/arch/gpio.h>
#define CONFIG_GXL_USB_U2_PORT_NUM	2

#ifdef CONFIG_USB_XHCI_AMLOGIC_USB3_V2
#define CONFIG_GXL_USB_U3_PORT_NUM	1
#else
#define CONFIG_GXL_USB_U3_PORT_NUM	0
#endif

static void gpio_set_vbus_power(char is_power_on)
{
	int ret;

	ret = gpio_request(CONFIG_USB_GPIO_PWR,
		CONFIG_USB_GPIO_PWR_NAME);
	if (ret && ret != -EBUSY) {
		printf("gpio: requesting pin %u failed\n",
			CONFIG_USB_GPIO_PWR);
		return;
	}

	if (is_power_on) {
		gpio_direction_output(CONFIG_USB_GPIO_PWR, 1);
	} else {
		gpio_direction_output(CONFIG_USB_GPIO_PWR, 0);
	}
}

struct amlogic_usb_config g_usb_config_GXL_skt={
	CONFIG_GXL_XHCI_BASE,
	USB_ID_MODE_HARDWARE,
	gpio_set_vbus_power,//gpio_set_vbus_power, //set_vbus_power
	CONFIG_GXL_USB_PHY2_BASE,
	CONFIG_GXL_USB_PHY3_BASE,
	CONFIG_GXL_USB_U2_PORT_NUM,
	CONFIG_GXL_USB_U3_PORT_NUM,
	.usb_phy2_pll_base_addr = {
		CONFIG_USB_PHY_20,
		CONFIG_USB_PHY_21,
	}
};

#endif /*CONFIG_USB_XHCI_AMLOGIC*/

#ifdef CONFIG_AML_HDMITX20
static void hdmi_tx_set_hdmi_5v(void)
{
}
#endif

/*
 * mtd nand partition table, only care the size!
 * offset will be calculated by nand driver.
 */
#ifdef CONFIG_AML_MTD
static struct mtd_partition normal_partition_info[] = {
#ifdef CONFIG_DISCRETE_BOOTLOADER
    /* MUST NOT CHANGE this part unless u know what you are doing!
     * inherent parition for descrete bootloader to store fip
     * size is determind by TPL_SIZE_PER_COPY*TPL_COPY_NUM
     * name must be same with TPL_PART_NAME
     */
    {
        .name = "tpl",
        .offset = 0,
        .size = 0,
    },
#endif
    {
        .name = "logo",
        .offset = 0,
        .size = 2*SZ_1M,
    },
    {
        .name = "recovery",
        .offset = 0,
        .size = 16*SZ_1M,
    },
    {
        .name = "boot",
        .offset = 0,
        .size = 15*SZ_1M,
    },
    {
        .name = "system",
        .offset = 0,
        .size = 280*SZ_1M,
    },
	/* last partition get the rest capacity */
    {
        .name = "data",
        .offset = MTDPART_OFS_APPEND,
        .size = MTDPART_SIZ_FULL,
    },
};
struct mtd_partition *get_aml_mtd_partition(void)
{
	return normal_partition_info;
}
int get_aml_partition_count(void)
{
	return ARRAY_SIZE(normal_partition_info);
}
#endif /* CONFIG_AML_MTD */

#ifdef CONFIG_AML_SPIFC
/*
 * BOOT_3: NOR_HOLDn:reg0[15:12]=3
 * BOOT_4: NOR_D:reg0[19:16]=3
 * BOOT_5: NOR_Q:reg0[23:20]=3
 * BOOT_6: NOR_C:reg0[27:24]=3
 * BOOT_7: NOR_WPn:reg0[31:28]=3
 * BOOT_14: NOR_CS:reg1[27:24]=3
 */
#define SPIFC_NUM_CS 1
static int spifc_cs_gpios[SPIFC_NUM_CS] = {54};

static int spifc_pinctrl_enable(void *pinctrl, bool enable)
{
	unsigned int val;

	val = readl(P_PERIPHS_PIN_MUX_0);
	val &= ~(0xfffff << 12);
	if (enable)
		val |= 0x33333 << 12;
	writel(val, P_PERIPHS_PIN_MUX_0);

	val = readl(P_PERIPHS_PIN_MUX_1);
	val &= ~(0xf << 24);
	writel(val, P_PERIPHS_PIN_MUX_1);
	return 0;
}

static const struct spifc_platdata spifc_platdata = {
	.reg = 0xffd14000,
	.mem_map = 0xf6000000,
	.pinctrl_enable = spifc_pinctrl_enable,
	.num_chipselect = SPIFC_NUM_CS,
	.cs_gpios = spifc_cs_gpios,
};

U_BOOT_DEVICE(spifc) = {
	.name = "spifc",
	.platdata = &spifc_platdata,
};
#endif /* CONFIG_AML_SPIFC */

extern void aml_pwm_cal_init(int mode);

#ifdef CONFIG_SYS_I2C_MESON
static const struct meson_i2c_platdata i2c_data[] = {
	{ 0, 0xffd1f000, 166666666, 3, 15, 100000 },
	{ 1, 0xffd1e000, 166666666, 3, 15, 100000 },
	{ 2, 0xffd1d000, 166666666, 3, 15, 100000 },
	{ 3, 0xffd1c000, 166666666, 3, 15, 100000 },
	{ 4, 0xff805000, 166666666, 3, 15, 100000 },
};

U_BOOT_DEVICES(meson_i2cs) = {
	{ "i2c_meson", &i2c_data[0] },
	{ "i2c_meson", &i2c_data[1] },
	{ "i2c_meson", &i2c_data[2] },
	{ "i2c_meson", &i2c_data[3] },
	{ "i2c_meson", &i2c_data[4] },
};

/*
 *GPIOH_6 I2C_SDA_M1
 *GPIOH_7 I2C_SCK_M1
 *pinmux configuration seperated with i2c controller configuration
 * config it when you use
 */
void set_i2c_m1_pinmux(void)
{
	/*ds =3 */
	clrbits_le32(PAD_DS_REG3A, 0xf << 12);
	setbits_le32(PAD_DS_REG3A, 0x3 << 12 | 0x3 << 14);
	/*pull up en*/
	clrbits_le32(PAD_PULL_UP_EN_REG3, 0x3 << 6);
	setbits_le32(PAD_PULL_UP_EN_REG3, 0x3 << 6 );
	/*pull up*/
	clrbits_le32(PAD_PULL_UP_REG3, 0x3 << 6);
	setbits_le32(PAD_PULL_UP_REG3, 0x3 << 6 );
	/*pin mux to i2cm1*/
	clrbits_le32(PERIPHS_PIN_MUX_B, 0xff << 24);
	setbits_le32(PERIPHS_PIN_MUX_B, 0x4 << 24 | 0x4 << 28);

	return;
}

#endif /*end CONFIG_SYS_I2C_MESON*/

#ifdef CONFIG_PWM_MESON
static const struct meson_pwm_platdata pwm_data[] = {
	{ PWM_AB, 0xffd1b000, IS_DOUBLE_CHANNEL, IS_BLINK },
	{ PWM_CD, 0xffd1a000, IS_DOUBLE_CHANNEL, IS_BLINK },
	{ PWM_EF, 0xffd19000, IS_DOUBLE_CHANNEL, IS_BLINK },
	{ PWMAO_AB, 0xff807000, IS_DOUBLE_CHANNEL, IS_BLINK },
	{ PWMAO_CD, 0xff802000, IS_DOUBLE_CHANNEL, IS_BLINK },
};

U_BOOT_DEVICES(meson_pwm) = {
	{ "amlogic,general-pwm", &pwm_data[0] },
	{ "amlogic,general-pwm", &pwm_data[1] },
	{ "amlogic,general-pwm", &pwm_data[2] },
	{ "amlogic,general-pwm", &pwm_data[3] },
	{ "amlogic,general-pwm", &pwm_data[4] },
};
#endif /*end CONFIG_PWM_MESON*/

#if (defined (CONFIG_AML_LCD) && defined(CONFIG_TCA6408))
// detect whether the LCD is exist
void board_lcd_detect(void)
{
	u8 value = 0;
	uchar linebuf[1] = {0};
	tca6408_output_set_value(TCA_TP_RST_MASK, TCA_TP_RST_MASK);
	mdelay(5);
	tca6408_output_set_value((0<<6), (1<<6));
	mdelay(20);
	tca6408_output_set_value((1<<6), (1<<6));
	mdelay(50);

		sw_i2c_read(0x70,0xA8,linebuf,1);
		if (linebuf[0] == 0x51) {//old TS050
			setenv("panel_type", "lcd_0");
			value = 1;
		} else if (linebuf[0] == 0x79) {//new TS050
			setenv("panel_type", "lcd_1");
			value = 1;
		} else {
			sw_i2c_read(0xba,0x9e,linebuf,1);
			if (linebuf[0] == 0x00) {//TS101
				setenv("panel_type", "lcd_2");
				value = 1;
			}
		}

	// detect RESET pin
	// if the LCD is connected, the RESET pin will be plll high
	// if the LCD is not connected, the RESET pin will be low

	printf("LCD_RESET PIN: %d\n", value);
	setenv_ulong("lcd_exist", value);
}
#endif /* CONFIG_AML_LCD */

extern void aml_pwm_cal_init(int mode);

extern int i2c_read(uchar chip, uint addr, int alen, uchar *buffer, int len);
static int check_forcebootsd(void)
{
	unsigned char tst_status = 0;
	unsigned char mcu_version[2] = {0};
	int retval;

	retval = i2c_read(0x18, 0x12, 1, mcu_version, 1);
	retval |= i2c_read(0x18, 0x13, 1, mcu_version + 1, 1);

	if (retval < 0) {
		printf("%s i2c_read failed!\n", __func__);
		return -1;
	}

	printf("MCU version: 0x%02x 0x%02x\n", mcu_version[0], mcu_version[1]);

	if (mcu_version[1] < 0x04) {
		printf("MCU version is to low! Doesn't support froce boot from SD card.\n");
		return -1;
	}

	retval = i2c_read(0x18, 0x90, 1, &tst_status, 1);
	if (retval < 0) {
		printf("%s i2c_read failed!\n", __func__);
		return -1;
	}

	if (1 == tst_status) {
		printf("Force boot from SD.\n");
		run_command("kbi tststatus clear", 0);
		run_command("kbi forcebootsd", 0);
	}

	return 0;
}

int board_init(void)
{
	sys_led_init();
    //Please keep CONFIG_AML_V2_FACTORY_BURN at first place of board_init
    //As NOT NEED other board init If USB BOOT MODE
#ifdef CONFIG_AML_V2_FACTORY_BURN
	if ((0x1b8ec003 != readl(P_PREG_STICKY_REG2)) && (0x1b8ec004 != readl(P_PREG_STICKY_REG2))) {
				aml_try_factory_usb_burning(0, gd->bd);
	}
#endif// #ifdef CONFIG_AML_V2_FACTORY_BURN
#ifdef CONFIG_USB_XHCI_AMLOGIC_V2
	board_usb_pll_disable(&g_usb_config_GXL_skt);
	board_usb_init(&g_usb_config_GXL_skt,BOARD_USB_MODE_HOST);
	gpio_set_vbus_power(1);
#endif /*CONFIG_USB_XHCI_AMLOGIC*/

#if 0
	aml_pwm_cal_init(0);
#endif//
#ifdef CONFIG_AML_NAND
	extern int amlnf_init(unsigned char flag);
	amlnf_init(0);
#endif
#ifdef CONFIG_SYS_I2C_MESON
	set_i2c_m1_pinmux();
#endif
#ifdef CONFIG_SYS_I2C_AML
	board_i2c_init();
	check_forcebootsd();
#endif
#ifdef CONFIG_TCA6408
	tca6408_gpio_init();
#endif

	return 0;
}

#ifdef CONFIG_BOARD_LATE_INIT
int board_late_init(void)
{
	TE(__func__);
		//update env before anyone using it
		run_command("get_rebootmode; echo reboot_mode=${reboot_mode}; "\
						"if test ${reboot_mode} = factory_reset; then "\
						"defenv_reserv;save; fi;", 0);
		run_command("if itest ${upgrade_step} == 1; then "\
						"defenv_reserv; setenv upgrade_step 2; saveenv; fi;", 0);
		/*add board late init function here*/
#ifndef DTB_BIND_KERNEL
		int ret;
		ret = run_command("store dtb read $dtb_mem_addr", 1);
        if (ret) {
				printf("%s(): [store dtb read $dtb_mem_addr] fail\n", __func__);
#ifdef CONFIG_DTB_MEM_ADDR
				char cmd[64];
				printf("load dtb to %x\n", CONFIG_DTB_MEM_ADDR);
				sprintf(cmd, "store dtb read %x", CONFIG_DTB_MEM_ADDR);
				ret = run_command(cmd, 1);
                if (ret) {
						printf("%s(): %s fail\n", __func__, cmd);
				}
#endif
		}
#elif defined(CONFIG_DTB_MEM_ADDR)
		{
				char cmd[128];
				int ret;
                if (!getenv("dtb_mem_addr")) {
						sprintf(cmd, "setenv dtb_mem_addr 0x%x", CONFIG_DTB_MEM_ADDR);
						run_command(cmd, 0);
				}
				sprintf(cmd, "imgread dtb boot ${dtb_mem_addr}");
				ret = run_command(cmd, 0);
                if (ret) {
						printf("%s(): cmd[%s] fail, ret=%d\n", __func__, cmd, ret);
				}
		}
#endif// #ifndef DTB_BIND_KERNEL

#ifdef CONFIG_POWER_FUSB302
	fusb302_init();
#endif

	run_command("gpio set GPIOA_13", 0);//5G reset
	mdelay(200);
	run_command("gpio clear GPIOA_13", 0);//5G reset
	run_command("gpio clear GPIOA_8", 0); //pcie reset-gpio

	/* load unifykey */
	run_command("keyunify init 0x1234", 0);
#ifdef CONFIG_AML_VPU
	vpu_probe();
#endif
	vpp_init();
#ifdef CONFIG_AML_HDMITX20
	hdmi_tx_set_hdmi_5v();
	hdmi_tx_init();
#endif
#ifdef CONFIG_AML_CVBS
	run_command("cvbs init", 0);
#endif
#ifdef CONFIG_AML_LCD
	board_lcd_detect();
	lcd_probe();
#endif

#ifdef CONFIG_AML_V2_FACTORY_BURN
	if (0x1b8ec003 == readl(P_PREG_STICKY_REG2))
		aml_try_factory_usb_burning(1, gd->bd);
		aml_try_factory_sdcard_burning(0, gd->bd);
#endif// #ifdef CONFIG_AML_V2_FACTORY_BURN

	// Setup FAN to MAX speed for testing
	run_command("i2c mw 0x18 0x88 3", 0);

	cpu_id_t cpu_id = get_cpu_id();
	if (cpu_id.family_id == MESON_CPU_MAJOR_ID_SM1) {
		char cmd[16];
		setenv("maxcpus","4");
		sprintf(cmd, "%X", cpu_id.chip_rev);
		setenv("chiprev", cmd);
		setenv("board_defined_bootup", "bootup_D3");
	}

	TE(__func__);
	return 0;
}
#endif

#ifdef CONFIG_AML_TINY_USBTOOL
int usb_get_update_result(void)
{
	unsigned long upgrade_step;
	upgrade_step = simple_strtoul (getenv ("upgrade_step"), NULL, 16);
	printf("upgrade_step = %d\n", (int)upgrade_step);
	if (upgrade_step == 1)
	{
		run_command("defenv", 1);
		run_command("setenv upgrade_step 2", 1);
		run_command("saveenv", 1);
		return 0;
	}
	else
	{
		return -1;
	}
}
#endif

phys_size_t get_effective_memsize(void)
{
	// >>16 -> MB, <<20 -> real size, so >>16<<20 = <<4
#if defined(CONFIG_SYS_MEM_TOP_HIDE)
	return (((readl(AO_SEC_GP_CFG0)) & 0xFFFF0000) << 4) - CONFIG_SYS_MEM_TOP_HIDE;
#else
	return (((readl(AO_SEC_GP_CFG0)) & 0xFFFF0000) << 4);
#endif
}

#ifdef CONFIG_MULTI_DTB
int checkhw(char * name)
{
	/*
	 * set aml_dt according to chip and dram capacity
	 */
	unsigned int ddr_size=0;
	char loc_name[64] = {0};
	int i;
	cpu_id_t cpu_id=get_cpu_id();

	for (i=0; i<CONFIG_NR_DRAM_BANKS; i++) {
		ddr_size += gd->bd->bi_dram[i].size;
	}
#if defined(CONFIG_SYS_MEM_TOP_HIDE)
	ddr_size += CONFIG_SYS_MEM_TOP_HIDE;
#endif
	char *ddr_mode = getenv("mem_size");
	if (MESON_CPU_MAJOR_ID_SM1 == cpu_id.family_id) {
		switch (ddr_size) {
			case 0x80000000:
				if (!strcmp(ddr_mode, "1g")) {
					strcpy(loc_name, "sm1_ac202_1g\0");
					break;
				}
				strcpy(loc_name, "sm1_ac202_2g\0");
				break;
			case 0x40000000:
				strcpy(loc_name, "sm1_ac202_1g\0");
				break;
			case 0x2000000:
				strcpy(loc_name, "sm1_ac202_512m\0");
				break;
			default:
				strcpy(loc_name, "sm1_ac202_unsupport");
				break;
		}
	}
	else {
		switch (ddr_size) {
			case 0x80000000:
				if (!strcmp(ddr_mode, "1g")) {
					strcpy(loc_name, "g12a_u202_1g\0");
					break;
				}
				strcpy(loc_name, "g12a_u202_2g\0");
				break;
			case 0x40000000:
				strcpy(loc_name, "g12a_u202_1g\0");
				break;
			case 0x2000000:
				strcpy(loc_name, "g12a_u202_512m\0");
				break;
			default:
				strcpy(loc_name, "g12a_u202_unsupport");
				break;
		}
	}
	strcpy(name, loc_name);
	setenv("aml_dt", loc_name);
	return 0;
}
#endif

#ifdef CONFIG_DDR_AUTO_DTB
int check_ddrsize(void)
{
	unsigned long ddr_size=0;
	int i;
	for (i=0; i<CONFIG_NR_DRAM_BANKS; i++) {
		ddr_size += gd->bd->bi_dram[i].size;
	}
#if defined(CONFIG_SYS_MEM_TOP_HIDE)
		ddr_size += CONFIG_SYS_MEM_TOP_HIDE;
#endif
	switch (ddr_size) {
	case 0x80000000:
		setenv("ddr_size", "2"); //2G DDR
		break;
	case 0xe0000000:
		setenv("ddr_size", "4"); //4G DDR
		break;
	default:
		setenv("ddr_size", "0");
		break;
	}
	return 0;
}
#endif

const char * const _env_args_reserve_[] =
{
		"aml_dt",
		"firstboot",
		"lock",
		"upgrade_step",

		NULL//Keep NULL be last to tell END
};
