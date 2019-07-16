/******************************************************************************
 *
 * Copyright(c) 2016 - 2019 Realtek Corporation. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 ******************************************************************************/

#include "halmac_pcie_8814b.h"
#include "halmac_pwr_seq_8814b.h"
#include "../halmac_init_88xx_v1.h"
#include "../halmac_common_88xx_v1.h"
#include "../halmac_efuse_88xx_v1.h"

#if (HALMAC_8814B_SUPPORT && HALMAC_PCIE_SUPPORT)

static u8
is_pwr_patch_8814b(struct halmac_adapter *adatper);

static enum halmac_ret_status
pwr_patch_8814b(struct halmac_adapter *adapter, u8 patch,
		enum halmac_mac_power pwr);

/**
 * mac_pwr_switch_pcie_8814b() - switch mac power
 * @adapter : the adapter of halmac
 * @pwr : power state
 * Author : KaiYuan Chang / Ivan Lin
 * Return : enum halmac_ret_status
 * More details of status code can be found in prototype document
 */
enum halmac_ret_status
mac_pwr_switch_pcie_8814b(struct halmac_adapter *adapter,
			  enum halmac_mac_power pwr)
{
	u8 value8;
	u8 rpwm;
	u8 is_patch;
	struct halmac_api *api = (struct halmac_api *)adapter->halmac_api;

	PLTFM_MSG_TRACE("[TRACE]%s ===>\n", __func__);
	PLTFM_MSG_TRACE("[TRACE]pwr = %x\n", pwr);
	PLTFM_MSG_TRACE("[TRACE]8814B pwr seq ver = %s\n",
			HALMAC_8814B_PWR_SEQ_VER);

	adapter->rpwm = HALMAC_REG_R8(REG_DBGSEL_PCIE_HRPWM1_HCPWM1_V1 + 1);

	/* Check FW still exist or not */
	if (HALMAC_REG_R16(REG_MCUFW_CTRL) == 0xC078) {
		/* Leave 32K */
		rpwm = (u8)((adapter->rpwm ^ BIT(7)) & 0x80);
		HALMAC_REG_W8(REG_DBGSEL_PCIE_HRPWM1_HCPWM1_V1 + 1, rpwm);
	}

	value8 = HALMAC_REG_R8(REG_CR);
	if (value8 == 0xEA)
		adapter->halmac_state.mac_pwr = HALMAC_MAC_POWER_OFF;
	else
		adapter->halmac_state.mac_pwr = HALMAC_MAC_POWER_ON;

	/* Check if power switch is needed */
	if (pwr == HALMAC_MAC_POWER_ON &&
	    adapter->halmac_state.mac_pwr == HALMAC_MAC_POWER_ON) {
		PLTFM_MSG_WARN("[WARN]power state unchange!!\n");
		return HALMAC_RET_PWR_UNCHANGE;
	}

	is_patch = is_pwr_patch_8814b(adapter);

	if (pwr == HALMAC_MAC_POWER_OFF) {
		if (pwr_seq_parser_88xx_v1(adapter, card_dis_flow_8814b) !=
		    HALMAC_RET_SUCCESS) {
			PLTFM_MSG_ERR("[ERR]Handle power off cmd error\n");
			return HALMAC_RET_POWER_OFF_FAIL;
		}

		pwr_patch_8814b(adapter, is_patch, HALMAC_MAC_POWER_OFF);

		adapter->halmac_state.mac_pwr = HALMAC_MAC_POWER_OFF;
		adapter->halmac_state.dlfw_state = HALMAC_DLFW_NONE;
		init_adapter_dynamic_param_88xx_v1(adapter);
	} else {
		pwr_patch_8814b(adapter, is_patch, HALMAC_MAC_POWER_ON);

		if (pwr_seq_parser_88xx_v1(adapter, card_en_flow_8814b) !=
		    HALMAC_RET_SUCCESS) {
			pwr_patch_8814b(adapter, is_patch,
					HALMAC_MAC_POWER_OFF);

			PLTFM_MSG_ERR("[ERR]Handle power on cmd error\n");
			return HALMAC_RET_POWER_ON_FAIL;
		}

		adapter->halmac_state.mac_pwr = HALMAC_MAC_POWER_ON;
	}

	PLTFM_MSG_TRACE("[TRACE]%s <===\n", __func__);

	return HALMAC_RET_SUCCESS;
}

/**
 * halmac_pcie_switch_8814b() - pcie gen1/gen2 switch
 * @adapter : the adapter of halmac
 * @cfg : gen1/gen2 selection
 * Author : KaiYuan Chang / Ivan Lin
 * Return : enum halmac_ret_status
 * More details of status code can be found in prototype document
 */
enum halmac_ret_status
pcie_switch_8814b(struct halmac_adapter *adapter, enum halmac_pcie_cfg cfg)
{
	return HALMAC_RET_NOT_SUPPORT;
}

/**
 * phy_cfg_pcie_8814b() - phy config
 * @adapter : the adapter of halmac
 * Author : KaiYuan Chang / Ivan Lin
 * Return : enum halmac_ret_status
 * More details of status code can be found in prototype document
 */
enum halmac_ret_status
phy_cfg_pcie_8814b(struct halmac_adapter *adapter,
		   enum halmac_intf_phy_platform pltfm)
{
	return HALMAC_RET_SUCCESS;
}

/**
 * intf_tun_pcie_8814b() - pcie interface fine tuning
 * @adapter : the adapter of halmac
 * Author : Rick Liu
 * Return : enum halmac_ret_status
 * More details of status code can be found in prototype document
 */
enum halmac_ret_status
intf_tun_pcie_8814b(struct halmac_adapter *adapter)
{
	return HALMAC_RET_SUCCESS;
}

/**
 * cfgspc_set_pcie_8814b() - pcie configuration space setting
 * @adapter : the adapter of halmac
 * Author : Rick Liu
 * Return : enum halmac_ret_status
 * More details of status code can be found in prototype document
 */
enum halmac_ret_status
cfgspc_set_pcie_8814b(struct halmac_adapter *adapter,
		      struct halmac_pcie_cfgspc_param *param)
{
	return HALMAC_RET_NOT_SUPPORT;
}

static u8
is_pwr_patch_8814b(struct halmac_adapter *adapter)
{
	struct halmac_api *api = (struct halmac_api *)adapter->halmac_api;
	u8 map[3];

	if (read_efuse_88xx_v1(adapter, 0x3FD, 3, map) != HALMAC_RET_SUCCESS) {
		PLTFM_MSG_ERR("[ERR]Read Efuse error\n");
		return 0;
	}

	/* Patch for 8814BE C-cut */
	if (adapter->chip_ver >= HALMAC_CHIP_VER_C_CUT && map[0] == 0xFF &&
	    map[1] == 0x0C && map[2] == 0x00)
		return 1;
	else
		return 0;
}

static enum halmac_ret_status
pwr_patch_8814b(struct halmac_adapter *adapter, u8 patch,
		enum halmac_mac_power pwr)
{
	struct halmac_api *api = (struct halmac_api *)adapter->halmac_api;

	if (patch) {
		HALMAC_REG_W8_SET(REG_PMC_DBG_CTRL2,
				  BIT_SYSON_DIS_PMCREG_WRMSK);
		if (pwr == HALMAC_MAC_POWER_OFF)
			HALMAC_REG_W8_CLR(REG_ANAPARSW_POW_MAC, BIT(1));
		else if (pwr == HALMAC_MAC_POWER_ON)
			HALMAC_REG_W8_SET(REG_ANAPARSW_POW_MAC, BIT(1));
		HALMAC_REG_W8_CLR(REG_PMC_DBG_CTRL2,
				  BIT_SYSON_DIS_PMCREG_WRMSK);
	}

	return HALMAC_RET_SUCCESS;
}

#endif /* HALMAC_8814B_SUPPORT */
