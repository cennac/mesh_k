/******************************************************************************
 *
 * Copyright(c) 2007 - 2017  Realtek Corporation.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * The full GNU General Public License is included in this distribution in the
 * file called LICENSE.
 *
 * Contact Information:
 * wlanfae <wlanfae@realtek.com>
 * Realtek Corporation, No. 2, Innovation Road II, Hsinchu Science Park,
 * Hsinchu 300, Taiwan.
 *
 * Larry Finger <Larry.Finger@lwfinger.net>
 *
 *****************************************************************************/
#include "mp_precomp.h"
#include "../phydm_precomp.h"

#if (RTL8822C_SUPPORT)
void phydm_dynamic_switch_htstf_agc_8822c(struct dm_struct *dm)
{
	if (dm->bhtstfdisabled)
		return;

	switch (dm->linked_bf_support) {
	case PHYDM_IS_BF_LINKED:
		odm_set_bb_reg(dm, R_0x8a0, BIT(2), 0x1);
		break;
	case PHYDM_NO_BF_LINKED:
		odm_set_bb_reg(dm, R_0x8a0, BIT(2), 0x0);
		break;
	default:
		if (*dm->band_type == ODM_BAND_5G)
			odm_set_bb_reg(dm, R_0x8a0, BIT(2), 0x1);
		else if (*dm->band_type == ODM_BAND_2_4G)
			odm_set_bb_reg(dm, R_0x8a0, BIT(2), 0x0);
		break;
	}
}

void phydm_hwsetting_8822c(struct dm_struct *dm)
{
	phydm_dynamic_switch_htstf_agc_8822c(dm);
}
#endif
