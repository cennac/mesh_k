/******************************************************************************
 *
 * Copyright(c) 2007 - 2017 Realtek Corporation.
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

/*Image2HeaderVersion: R3 1.5.6*/
#include "mp_precomp.h"

#if (DM_ODM_SUPPORT_TYPE == ODM_WIN)
#if RT_PLATFORM == PLATFORM_MACOSX
#include "phydm_precomp.h"
#else
#include "../phydm_precomp.h"
#endif
#else
#include "../../phydm_precomp.h"
#endif

#define D_S_SIZE DELTA_SWINGIDX_SIZE
#define D_ST_SIZE DELTA_SWINTSSI_SIZE

#if (RTL8812F_SUPPORT == 1)
static boolean
check_positive(struct dm_struct *dm,
	       const u32	condition1,
	       const u32	condition2,
	       const u32	condition3,
	       const u32	condition4
)
{
	u32	cond1 = condition1, cond2 = condition2,
		cond3 = condition3, cond4 = condition4;

	u8	cut_version_for_para =
		(dm->cut_version ==  ODM_CUT_A) ? 15 : dm->cut_version;

	u8	pkg_type_for_para =
		(dm->package_type == 0) ? 15 : dm->package_type;

	u32	driver1 = cut_version_for_para << 24 |
			(dm->support_interface & 0xF0) << 16 |
			dm->support_platform << 16 |
			pkg_type_for_para << 12 |
			(dm->support_interface & 0x0F) << 8  |
			dm->rfe_type;

	u32	driver2 = (dm->type_glna & 0xFF) <<  0 |
			(dm->type_gpa & 0xFF)  <<  8 |
			(dm->type_alna & 0xFF) << 16 |
			(dm->type_apa & 0xFF)  << 24;

	u32	driver3 = 0;

	u32	driver4 = (dm->type_glna & 0xFF00) >>  8 |
			(dm->type_gpa & 0xFF00) |
			(dm->type_alna & 0xFF00) << 8 |
			(dm->type_apa & 0xFF00)  << 16;

	PHYDM_DBG(dm, ODM_COMP_INIT,
		  "===> %s (cond1, cond2, cond3, cond4) = (0x%X 0x%X 0x%X 0x%X)\n",
		  __func__, cond1, cond2, cond3, cond4);
	PHYDM_DBG(dm, ODM_COMP_INIT,
		  "===> %s (driver1, driver2, driver3, driver4) = (0x%X 0x%X 0x%X 0x%X)\n",
		  __func__, driver1, driver2, driver3, driver4);

	PHYDM_DBG(dm, ODM_COMP_INIT,
		  "	(Platform, Interface) = (0x%X, 0x%X)\n",
		  dm->support_platform, dm->support_interface);
	PHYDM_DBG(dm, ODM_COMP_INIT, "	(RFE, Package) = (0x%X, 0x%X)\n",
		  dm->rfe_type, dm->package_type);

	/*============== value Defined Check ===============*/
	/*cut version [27:24] need to do value check*/
	if (((cond1 & 0x0F000000) != 0) &&
	    ((cond1 & 0x0F000000) != (driver1 & 0x0F000000)))
		return false;

	/*pkg type [15:12] need to do value check*/
	if (((cond1 & 0x0000F000) != 0) &&
	    ((cond1 & 0x0000F000) != (driver1 & 0x0000F000)))
		return false;

	/*interface [11:8] need to do value check*/
	if (((cond1 & 0x00000F00) != 0) &&
	    ((cond1 & 0x00000F00) != (driver1 & 0x00000F00)))
		return false;
	/*=============== Bit Defined Check ================*/
	/* We don't care [31:28] */

	cond1 &= 0x000000FF;
	driver1 &= 0x000000FF;

	if (cond1 == driver1)
		return true;
	else
		return false;
}

/******************************************************************************
 *                           radioa.TXT
 ******************************************************************************/

const u32 array_mp_8812f_radioa[] = {
		0x000, 0x00030000,
		0x018, 0x00013124,
		0x0DE, 0x00000020,
		0x08E, 0x000A5540,
		0x081, 0x0000FC01,
		0x081, 0x0002FC01,
		0x081, 0x0003FC01,
		0x08A, 0x000E7DE3,
		0x08B, 0x0008FE00,
		0x0EE, 0x00000008,
		0x033, 0x00000000,
		0x03F, 0x00000023,
		0x033, 0x00000001,
		0x03F, 0x00000023,
		0x0EE, 0x00000000,
		0x0EF, 0x00004000,
		0x033, 0x00000000,
		0x03F, 0x0000000F,
		0x033, 0x00000002,
		0x03F, 0x00000000,
		0x0EF, 0x00000000,
		0x0EF, 0x00020000,
		0x033, 0x00000000,
		0x03E, 0x00001910,
		0x03F, 0x00020000,
		0x033, 0x00000001,
		0x03E, 0x00001C02,
		0x03F, 0x00020000,
		0x033, 0x00000002,
		0x03E, 0x00000F02,
		0x03F, 0x00020000,
		0x033, 0x00000003,
		0x03E, 0x00000F00,
		0x03F, 0x00020000,
		0x033, 0x00000004,
		0x03E, 0x00000086,
		0x03F, 0x00020000,
		0x033, 0x00000005,
		0x03E, 0x00000002,
		0x03F, 0x00020000,
		0x033, 0x00000006,
		0x03E, 0x00000000,
		0x03F, 0x00020000,
		0x033, 0x00000007,
		0x03E, 0x00000000,
		0x03F, 0x0002C010,
		0x033, 0x00000008,
		0x03E, 0x00001910,
		0x03F, 0x00020000,
		0x033, 0x00000009,
		0x03E, 0x00001C02,
		0x03F, 0x00020000,
		0x033, 0x0000000A,
		0x03E, 0x00000F02,
		0x03F, 0x00020000,
		0x033, 0x0000000B,
		0x03E, 0x00000F00,
		0x03F, 0x00020000,
		0x033, 0x0000000C,
		0x03E, 0x00000086,
		0x03F, 0x00020000,
		0x033, 0x0000000D,
		0x03E, 0x00000002,
		0x03F, 0x00020000,
		0x033, 0x0000000E,
		0x03E, 0x00000000,
		0x03F, 0x00020000,
		0x033, 0x0000000F,
		0x03E, 0x00000000,
		0x03F, 0x0002C010,
		0x033, 0x00000010,
		0x03E, 0x00001910,
		0x03F, 0x00020000,
		0x033, 0x00000011,
		0x03E, 0x00001C02,
		0x03F, 0x00020000,
		0x033, 0x00000012,
		0x03E, 0x00000F02,
		0x03F, 0x00020000,
		0x033, 0x00000013,
		0x03E, 0x00000F00,
		0x03F, 0x00020000,
		0x033, 0x00000014,
		0x03E, 0x00000086,
		0x03F, 0x00020000,
		0x033, 0x00000015,
		0x03E, 0x00000002,
		0x03F, 0x00020000,
		0x033, 0x00000016,
		0x03E, 0x00000000,
		0x03F, 0x00020000,
		0x033, 0x00000017,
		0x03E, 0x00000000,
		0x03F, 0x0002C010,
		0x033, 0x00000018,
		0x03E, 0x00001910,
		0x03F, 0x00020000,
		0x033, 0x00000019,
		0x03E, 0x00001C02,
		0x03F, 0x00020000,
		0x033, 0x0000001A,
		0x03E, 0x00000F02,
		0x03F, 0x00020000,
		0x033, 0x0000001B,
		0x03E, 0x00000F00,
		0x03F, 0x00020000,
		0x033, 0x0000001C,
		0x03E, 0x00000086,
		0x03F, 0x00020000,
		0x033, 0x0000001D,
		0x03E, 0x00000002,
		0x03F, 0x00020000,
		0x033, 0x0000001E,
		0x03E, 0x00000000,
		0x03F, 0x00020000,
		0x033, 0x0000001F,
		0x03E, 0x00000000,
		0x03F, 0x0002C010,
		0x033, 0x00000020,
		0x03E, 0x00001910,
		0x03F, 0x00020000,
		0x033, 0x00000021,
		0x03E, 0x00001C02,
		0x03F, 0x00020000,
		0x033, 0x00000022,
		0x03E, 0x00000F02,
		0x03F, 0x00020000,
		0x033, 0x00000023,
		0x03E, 0x00000F00,
		0x03F, 0x00020000,
		0x033, 0x00000024,
		0x03E, 0x00000086,
		0x03F, 0x00020000,
		0x033, 0x00000025,
		0x03E, 0x00000002,
		0x03F, 0x00020000,
		0x033, 0x00000026,
		0x03E, 0x00000000,
		0x03F, 0x00020000,
		0x033, 0x00000027,
		0x03E, 0x00000000,
		0x03F, 0x0002C010,
		0x033, 0x00000028,
		0x03E, 0x00001910,
		0x03F, 0x00020000,
		0x033, 0x00000029,
		0x03E, 0x00001C02,
		0x03F, 0x00020000,
		0x033, 0x0000002A,
		0x03E, 0x00000F02,
		0x03F, 0x00020000,
		0x033, 0x0000002B,
		0x03E, 0x00000F00,
		0x03F, 0x00020000,
		0x033, 0x0000002C,
		0x03E, 0x00000086,
		0x03F, 0x00020000,
		0x033, 0x0000002D,
		0x03E, 0x00000002,
		0x03F, 0x00020000,
		0x033, 0x0000002E,
		0x03E, 0x00000000,
		0x03F, 0x00020000,
		0x033, 0x0000002F,
		0x03E, 0x00000000,
		0x03F, 0x0002C010,
		0x0EF, 0x00000000,
		0x061, 0x0000D233,
		0x062, 0x0004D232,
		0x063, 0x00000002,
		0x0EF, 0x00000200,
		0x030, 0x00000226,
		0x030, 0x00001226,
		0x030, 0x00002226,
		0x030, 0x00003226,
		0x030, 0x00004226,
		0x030, 0x00005226,
		0x030, 0x00006226,
		0x030, 0x00007226,
		0x030, 0x00008226,
		0x030, 0x00009226,
		0x030, 0x0000A227,
		0x030, 0x0000B227,
		0x030, 0x0000C227,
		0x030, 0x0000D227,
		0x030, 0x0000E227,
		0x030, 0x0000F227,
		0x030, 0x00010227,
		0x030, 0x00011227,
		0x030, 0x00012227,
		0x030, 0x00013227,
		0x030, 0x00014226,
		0x030, 0x00015226,
		0x030, 0x00016226,
		0x030, 0x00017226,
		0x030, 0x00018226,
		0x030, 0x00019237,
		0x0EF, 0x00000000,
		0x0EF, 0x00000080,
		0x030, 0x00000234,
		0x030, 0x00001234,
		0x030, 0x00002234,
		0x030, 0x00003234,
		0x030, 0x00004234,
		0x030, 0x00005234,
		0x030, 0x00006234,
		0x030, 0x00007234,
		0x030, 0x00008234,
		0x030, 0x00009234,
		0x030, 0x0000A234,
		0x030, 0x0000B234,
		0x0EF, 0x00000000,
		0x0EF, 0x00000040,
		0x030, 0x00000733,
		0x030, 0x00001733,
		0x030, 0x00002420,
		0x030, 0x00003420,
		0x030, 0x00004310,
		0x030, 0x00005310,
		0x030, 0x00008733,
		0x030, 0x0000A420,
		0x030, 0x0000C310,
		0x0EF, 0x00000000,
		0x0EE, 0x00010000,
	0x80000001,	0x00000000,	0x40000000,	0x00000000,
		0x033, 0x00000200,
		0x03F, 0x00000004,
		0x033, 0x00000201,
		0x03F, 0x00000007,
		0x033, 0x00000202,
		0x03F, 0x00000024,
		0x033, 0x00000203,
		0x03F, 0x00000027,
		0x033, 0x00000204,
		0x03F, 0x0000002A,
		0x033, 0x00000205,
		0x03F, 0x0000002D,
		0x033, 0x00000206,
		0x03F, 0x0000006A,
		0x033, 0x00000207,
		0x03F, 0x0000006D,
		0x033, 0x00000208,
		0x03F, 0x00000070,
		0x033, 0x00000209,
		0x03F, 0x0000046D,
		0x033, 0x0000020A,
		0x03F, 0x00000470,
	0x90000003,	0x00000000,	0x40000000,	0x00000000,
		0x033, 0x00000200,
		0x03F, 0x00000004,
		0x033, 0x00000201,
		0x03F, 0x00000007,
		0x033, 0x00000202,
		0x03F, 0x00000024,
		0x033, 0x00000203,
		0x03F, 0x00000027,
		0x033, 0x00000204,
		0x03F, 0x0000002A,
		0x033, 0x00000205,
		0x03F, 0x0000002D,
		0x033, 0x00000206,
		0x03F, 0x0000006A,
		0x033, 0x00000207,
		0x03F, 0x0000006D,
		0x033, 0x00000208,
		0x03F, 0x00000070,
		0x033, 0x00000209,
		0x03F, 0x0000046D,
		0x033, 0x0000020A,
		0x03F, 0x00000470,
	0xA0000000,	0x00000000,
		0x033, 0x00000200,
		0x03F, 0x0000006A,
		0x033, 0x00000201,
		0x03F, 0x0000006D,
		0x033, 0x00000202,
		0x03F, 0x0000046A,
		0x033, 0x00000203,
		0x03F, 0x0000086A,
		0x033, 0x00000204,
		0x03F, 0x00000C89,
		0x033, 0x00000205,
		0x03F, 0x00000CE8,
		0x033, 0x00000206,
		0x03F, 0x00000CEB,
		0x033, 0x00000207,
		0x03F, 0x00000CEE,
		0x033, 0x00000208,
		0x03F, 0x00000CF1,
		0x033, 0x00000209,
		0x03F, 0x00000CF4,
		0x033, 0x0000020A,
		0x03F, 0x00000CF7,
	0xB0000000,	0x00000000,
	0x80000001,	0x00000000,	0x40000000,	0x00000000,
		0x033, 0x00000280,
		0x03F, 0x00000004,
		0x033, 0x00000281,
		0x03F, 0x00000007,
		0x033, 0x00000282,
		0x03F, 0x00000024,
		0x033, 0x00000283,
		0x03F, 0x00000027,
		0x033, 0x00000284,
		0x03F, 0x0000002A,
		0x033, 0x00000285,
		0x03F, 0x0000002D,
		0x033, 0x00000286,
		0x03F, 0x0000006A,
		0x033, 0x00000287,
		0x03F, 0x0000006D,
		0x033, 0x00000288,
		0x03F, 0x00000070,
		0x033, 0x00000289,
		0x03F, 0x0000046D,
		0x033, 0x0000028A,
		0x03F, 0x00000470,
	0x90000003,	0x00000000,	0x40000000,	0x00000000,
		0x033, 0x00000280,
		0x03F, 0x00000004,
		0x033, 0x00000281,
		0x03F, 0x00000007,
		0x033, 0x00000282,
		0x03F, 0x00000024,
		0x033, 0x00000283,
		0x03F, 0x00000027,
		0x033, 0x00000284,
		0x03F, 0x0000002A,
		0x033, 0x00000285,
		0x03F, 0x0000002D,
		0x033, 0x00000286,
		0x03F, 0x0000006A,
		0x033, 0x00000287,
		0x03F, 0x0000006D,
		0x033, 0x00000288,
		0x03F, 0x00000070,
		0x033, 0x00000289,
		0x03F, 0x0000046D,
		0x033, 0x0000028A,
		0x03F, 0x00000470,
	0xA0000000,	0x00000000,
		0x033, 0x00000280,
		0x03F, 0x0000006A,
		0x033, 0x00000281,
		0x03F, 0x0000006D,
		0x033, 0x00000282,
		0x03F, 0x0000046A,
		0x033, 0x00000283,
		0x03F, 0x0000086A,
		0x033, 0x00000284,
		0x03F, 0x00000C89,
		0x033, 0x00000285,
		0x03F, 0x00000CE8,
		0x033, 0x00000286,
		0x03F, 0x00000CEB,
		0x033, 0x00000287,
		0x03F, 0x00000CEE,
		0x033, 0x00000288,
		0x03F, 0x00000CF1,
		0x033, 0x00000289,
		0x03F, 0x00000CF4,
		0x033, 0x0000028A,
		0x03F, 0x00000CF7,
	0xB0000000,	0x00000000,
	0x80000001,	0x00000000,	0x40000000,	0x00000000,
		0x033, 0x00000300,
		0x03F, 0x00000004,
		0x033, 0x00000301,
		0x03F, 0x00000007,
		0x033, 0x00000302,
		0x03F, 0x00000024,
		0x033, 0x00000303,
		0x03F, 0x00000027,
		0x033, 0x00000304,
		0x03F, 0x0000002A,
		0x033, 0x00000305,
		0x03F, 0x0000002D,
		0x033, 0x00000306,
		0x03F, 0x0000006A,
		0x033, 0x00000307,
		0x03F, 0x0000006D,
		0x033, 0x00000308,
		0x03F, 0x00000070,
		0x033, 0x00000309,
		0x03F, 0x0000046D,
		0x033, 0x0000030A,
		0x03F, 0x00000470,
		0x0EE, 0x00000000,
	0x90000003,	0x00000000,	0x40000000,	0x00000000,
		0x033, 0x00000300,
		0x03F, 0x00000004,
		0x033, 0x00000301,
		0x03F, 0x00000007,
		0x033, 0x00000302,
		0x03F, 0x00000024,
		0x033, 0x00000303,
		0x03F, 0x00000027,
		0x033, 0x00000304,
		0x03F, 0x0000002A,
		0x033, 0x00000305,
		0x03F, 0x0000002D,
		0x033, 0x00000306,
		0x03F, 0x0000006A,
		0x033, 0x00000307,
		0x03F, 0x0000006D,
		0x033, 0x00000308,
		0x03F, 0x00000070,
		0x033, 0x00000309,
		0x03F, 0x0000046D,
		0x033, 0x0000030A,
		0x03F, 0x00000470,
		0x0EE, 0x00000000,
	0xA0000000,	0x00000000,
		0x033, 0x00000300,
		0x03F, 0x0000006A,
		0x033, 0x00000301,
		0x03F, 0x0000006D,
		0x033, 0x00000302,
		0x03F, 0x0000046A,
		0x033, 0x00000303,
		0x03F, 0x0000086A,
		0x033, 0x00000304,
		0x03F, 0x00000C89,
		0x033, 0x00000305,
		0x03F, 0x00000CE8,
		0x033, 0x00000306,
		0x03F, 0x00000CEB,
		0x033, 0x00000307,
		0x03F, 0x00000CEE,
		0x033, 0x00000308,
		0x03F, 0x00000CF1,
		0x033, 0x00000309,
		0x03F, 0x00000CF4,
		0x033, 0x0000030A,
		0x03F, 0x00000CF7,
		0x0EE, 0x00000000,
	0xB0000000,	0x00000000,
		0x05C, 0x000FCC00,
		0x067, 0x0000A505,
		0x043, 0x00005000,
		0x07F, 0x00000000,
		0x0B0, 0x0001F0FC,
		0x0B1, 0x0007DBE4,
		0x0B2, 0x00022400,
		0x0B3, 0x000FC760,
		0x0B4, 0x00099D40,
		0x0B5, 0x0004103F,
		0x0B6, 0x000187F8,
		0x0B7, 0x00030018,
		0x0BC, 0x00000024,
		0x0D3, 0x00000542,
		0x0DD, 0x00000540,
		0x0BB, 0x00040010,
		0x0B0, 0x0001F0FA,
		0x0FE, 0x00000000,
		0x0CA, 0x00080000,
		0x0CA, 0x00080001,
		0x0FE, 0x00000000,
		0x0B0, 0x0001F0F8,
		0x0B3, 0x000FC700,
		0x018, 0x0001B124,
		0xFFE, 0x00000000,
		0xFFE, 0x00000000,
		0x0B3, 0x000FC760,
		0x018, 0x00013124,
		0x0CC, 0x0000F000,
		0x0CD, 0x00089600,
		0x0CD, 0x00009600,
		0x0CD, 0x00089600,
		0x059, 0x000A0000,
		0x05A, 0x00060000,
		0x05B, 0x00014000,
		0x0ED, 0x00000008,
		0x033, 0x00000001,
		0x03F, 0x0000000F,
		0x0ED, 0x00000000,
		0x0FE, 0x00000000,
		0x01B, 0x00003A40,
		0x0EE, 0x00000002,
		0x033, 0x00000017,
	0x80000001,	0x00000000,	0x40000000,	0x00000000,
		0x03F, 0x0000001F,
	0x90000003,	0x00000000,	0x40000000,	0x00000000,
		0x03F, 0x0000001F,
	0xA0000000,	0x00000000,
		0x03F, 0x0000003F,
	0xB0000000,	0x00000000,
		0x033, 0x00000018,
	0x80000001,	0x00000000,	0x40000000,	0x00000000,
		0x03F, 0x0000001F,
	0x90000003,	0x00000000,	0x40000000,	0x00000000,
		0x03F, 0x0000001F,
	0xA0000000,	0x00000000,
		0x03F, 0x0000003F,
	0xB0000000,	0x00000000,
		0x033, 0x00000019,
		0x03F, 0x00000000,
		0x033, 0x0000001A,
		0x03F, 0x0000003F,
		0x033, 0x0000001B,
		0x03F, 0x0000003F,
		0x033, 0x0000001C,
		0x03F, 0x0000003F,
		0x0EE, 0x00000000,
		0x0ED, 0x00000200,
		0x033, 0x00000000,
		0x03F, 0x000F45A4,
		0x033, 0x00000001,
		0x03F, 0x000F49A4,
		0x033, 0x00000002,
		0x03F, 0x000F49A4,
		0x033, 0x00000003,
		0x03F, 0x000F69A4,
		0x033, 0x00000004,
		0x03F, 0x000F69A4,
		0x033, 0x00000005,
		0x03F, 0x000F69A4,
		0x033, 0x00000006,
		0x03F, 0x000F6DA4,
		0x033, 0x00000007,
		0x03F, 0x000F6DA4,
		0x033, 0x00000008,
		0x03F, 0x000F6DA4,
		0x033, 0x00000009,
		0x03F, 0x000F8DA4,
		0x033, 0x0000000A,
		0x03F, 0x000F8DA4,
		0x033, 0x0000000B,
		0x03F, 0x000F8DA4,
		0x033, 0x0000000C,
		0x03F, 0x000F91A4,
		0x033, 0x0000000D,
		0x03F, 0x000F91A4,
		0x033, 0x0000000E,
		0x03F, 0x000F91A4,
		0x033, 0x0000000F,
		0x03F, 0x000FB1A4,
		0x033, 0x00000010,
		0x03F, 0x000FB1A4,
		0x033, 0x00000011,
		0x03F, 0x000FB1A4,
		0x033, 0x00000012,
		0x03F, 0x000FB5A4,
		0x033, 0x00000013,
		0x03F, 0x000FB5A4,
		0x033, 0x00000014,
		0x03F, 0x000FD9A4,
		0x033, 0x00000015,
		0x03F, 0x000FD9A4,
		0x033, 0x00000016,
		0x03F, 0x000FF9A4,
		0x033, 0x00000017,
		0x03F, 0x000FF9A4,
		0x033, 0x00000018,
		0x03F, 0x000FFDA4,
		0x033, 0x00000019,
		0x03F, 0x000FFDA4,
		0x033, 0x0000001A,
		0x03F, 0x000FFDA4,
		0x0ED, 0x00000000,

};

void
odm_read_and_config_mp_8812f_radioa(struct dm_struct *dm)
{
	u32	i = 0;
	u8	c_cond;
	boolean	is_matched = true, is_skipped = false;
	u32	array_len =
			sizeof(array_mp_8812f_radioa) / sizeof(u32);
	u32	*array = (u32 *)array_mp_8812f_radioa;

	u32	v1 = 0, v2 = 0, pre_v1 = 0, pre_v2 = 0;
	u32	a1 = 0, a2 = 0, a3 = 0, a4 = 0;

	PHYDM_DBG(dm, ODM_COMP_INIT, "===> %s\n", __func__);

	while ((i + 1) < array_len) {
		v1 = array[i];
		v2 = array[i + 1];

		if (v1 & (BIT(31) | BIT(30))) {/*positive & negative condition*/
			if (v1 & BIT(31)) {/* positive condition*/
				c_cond  =
					(u8)((v1 & (BIT(29) | BIT(28))) >> 28);
				if (c_cond == COND_ENDIF) {/*end*/
					is_matched = true;
					is_skipped = false;
					PHYDM_DBG(dm, ODM_COMP_INIT, "ENDIF\n");
				} else if (c_cond == COND_ELSE) { /*else*/
					is_matched = is_skipped ? false : true;
					PHYDM_DBG(dm, ODM_COMP_INIT, "ELSE\n");
				} else {/*if , else if*/
					pre_v1 = v1;
					pre_v2 = v2;
					PHYDM_DBG(dm, ODM_COMP_INIT,
						  "IF or ELSE IF\n");
				}
			} else if (v1 & BIT(30)) { /*negative condition*/
				if (!is_skipped) {
					a1 = pre_v1; a2 = pre_v2;
					a3 = v1; a4 = v2;
					if (check_positive(dm,
							   a1, a2, a3, a4)) {
						is_matched = true;
						is_skipped = true;
					} else {
						is_matched = false;
						is_skipped = false;
					}
				} else {
					is_matched = false;
				}
			}
		} else {
			if (is_matched)
				odm_config_rf_radio_a_8812f(dm, v1, v2);
		}
		i = i + 2;
	}
}

u32
odm_get_version_mp_8812f_radioa(void)
{
		return 11;
}

/******************************************************************************
 *                           radiob.TXT
 ******************************************************************************/

const u32 array_mp_8812f_radiob[] = {
		0x000, 0x00030000,
		0x018, 0x00013124,
		0x0EF, 0x00080000,
		0x033, 0x00000001,
		0x03F, 0x00091020,
		0x0EF, 0x00000000,
		0x0DE, 0x00000020,
		0x08E, 0x000A5540,
		0x081, 0x0000FC01,
		0x081, 0x0002FC01,
		0x081, 0x0003FC01,
		0x08A, 0x000E7DE3,
		0x08B, 0x0008FE00,
		0x0EE, 0x00000008,
		0x033, 0x00000000,
		0x03F, 0x00000023,
		0x033, 0x00000001,
		0x03F, 0x00000023,
		0x0EE, 0x00000000,
		0x0EF, 0x00004000,
		0x033, 0x00000000,
		0x03F, 0x0000000F,
		0x033, 0x00000002,
		0x03F, 0x00000000,
		0x0EF, 0x00000000,
		0x0EF, 0x00020000,
		0x033, 0x00000000,
		0x03E, 0x00001910,
		0x03F, 0x00020000,
		0x033, 0x00000001,
		0x03E, 0x00001C02,
		0x03F, 0x00020000,
		0x033, 0x00000002,
		0x03E, 0x00000F02,
		0x03F, 0x00020000,
		0x033, 0x00000003,
		0x03E, 0x00000F00,
		0x03F, 0x00020000,
		0x033, 0x00000004,
		0x03E, 0x00000086,
		0x03F, 0x00020000,
		0x033, 0x00000005,
		0x03E, 0x00000002,
		0x03F, 0x00020000,
		0x033, 0x00000006,
		0x03E, 0x00000000,
		0x03F, 0x00020000,
		0x033, 0x00000007,
		0x03E, 0x00000000,
		0x03F, 0x0002C010,
		0x033, 0x00000008,
		0x03E, 0x00001910,
		0x03F, 0x00020000,
		0x033, 0x00000009,
		0x03E, 0x00001C02,
		0x03F, 0x00020000,
		0x033, 0x0000000A,
		0x03E, 0x00000F02,
		0x03F, 0x00020000,
		0x033, 0x0000000B,
		0x03E, 0x00000F00,
		0x03F, 0x00020000,
		0x033, 0x0000000C,
		0x03E, 0x00000086,
		0x03F, 0x00020000,
		0x033, 0x0000000D,
		0x03E, 0x00000002,
		0x03F, 0x00020000,
		0x033, 0x0000000E,
		0x03E, 0x00000000,
		0x03F, 0x00020000,
		0x033, 0x0000000F,
		0x03E, 0x00000000,
		0x03F, 0x0002C010,
		0x033, 0x00000010,
		0x03E, 0x00001910,
		0x03F, 0x00020000,
		0x033, 0x00000011,
		0x03E, 0x00001C02,
		0x03F, 0x00020000,
		0x033, 0x00000012,
		0x03E, 0x00000F02,
		0x03F, 0x00020000,
		0x033, 0x00000013,
		0x03E, 0x00000F00,
		0x03F, 0x00020000,
		0x033, 0x00000014,
		0x03E, 0x00000086,
		0x03F, 0x00020000,
		0x033, 0x00000015,
		0x03E, 0x00000002,
		0x03F, 0x00020000,
		0x033, 0x00000016,
		0x03E, 0x00000000,
		0x03F, 0x00020000,
		0x033, 0x00000017,
		0x03E, 0x00000000,
		0x03F, 0x0002C010,
		0x033, 0x00000018,
		0x03E, 0x00001910,
		0x03F, 0x00020000,
		0x033, 0x00000019,
		0x03E, 0x00001C02,
		0x03F, 0x00020000,
		0x033, 0x0000001A,
		0x03E, 0x00000F02,
		0x03F, 0x00020000,
		0x033, 0x0000001B,
		0x03E, 0x00000F00,
		0x03F, 0x00020000,
		0x033, 0x0000001C,
		0x03E, 0x00000086,
		0x03F, 0x00020000,
		0x033, 0x0000001D,
		0x03E, 0x00000002,
		0x03F, 0x00020000,
		0x033, 0x0000001E,
		0x03E, 0x00000000,
		0x03F, 0x00020000,
		0x033, 0x0000001F,
		0x03E, 0x00000000,
		0x03F, 0x0002C010,
		0x033, 0x00000020,
		0x03E, 0x00001910,
		0x03F, 0x00020000,
		0x033, 0x00000021,
		0x03E, 0x00001C02,
		0x03F, 0x00020000,
		0x033, 0x00000022,
		0x03E, 0x00000F02,
		0x03F, 0x00020000,
		0x033, 0x00000023,
		0x03E, 0x00000F00,
		0x03F, 0x00020000,
		0x033, 0x00000024,
		0x03E, 0x00000086,
		0x03F, 0x00020000,
		0x033, 0x00000025,
		0x03E, 0x00000002,
		0x03F, 0x00020000,
		0x033, 0x00000026,
		0x03E, 0x00000000,
		0x03F, 0x00020000,
		0x033, 0x00000027,
		0x03E, 0x00000000,
		0x03F, 0x0002C010,
		0x033, 0x00000028,
		0x03E, 0x00001910,
		0x03F, 0x00020000,
		0x033, 0x00000029,
		0x03E, 0x00001C02,
		0x03F, 0x00020000,
		0x033, 0x0000002A,
		0x03E, 0x00000F02,
		0x03F, 0x00020000,
		0x033, 0x0000002B,
		0x03E, 0x00000F00,
		0x03F, 0x00020000,
		0x033, 0x0000002C,
		0x03E, 0x00000086,
		0x03F, 0x00020000,
		0x033, 0x0000002D,
		0x03E, 0x00000002,
		0x03F, 0x00020000,
		0x033, 0x0000002E,
		0x03E, 0x00000000,
		0x03F, 0x00020000,
		0x033, 0x0000002F,
		0x03E, 0x00000000,
		0x03F, 0x0002C010,
		0x0EF, 0x00000000,
		0x061, 0x0000D233,
		0x062, 0x0004D232,
		0x063, 0x00000002,
		0x0EF, 0x00000200,
		0x030, 0x00000226,
		0x030, 0x00001226,
		0x030, 0x00002226,
		0x030, 0x00003226,
		0x030, 0x00004226,
		0x030, 0x00005226,
		0x030, 0x00006226,
		0x030, 0x00007226,
		0x030, 0x00008226,
		0x030, 0x00009226,
		0x030, 0x0000A227,
		0x030, 0x0000B227,
		0x030, 0x0000C227,
		0x030, 0x0000D227,
		0x030, 0x0000E227,
		0x030, 0x0000F227,
		0x030, 0x00010227,
		0x030, 0x00011227,
		0x030, 0x00012227,
		0x030, 0x00013227,
		0x030, 0x00014226,
		0x030, 0x00015226,
		0x030, 0x00016226,
		0x030, 0x00017226,
		0x030, 0x00018226,
		0x030, 0x00019237,
		0x0EF, 0x00000000,
		0x0EF, 0x00000080,
		0x030, 0x00000234,
		0x030, 0x00001234,
		0x030, 0x00002234,
		0x030, 0x00003234,
		0x030, 0x00004234,
		0x030, 0x00005234,
		0x030, 0x00006234,
		0x030, 0x00007234,
		0x030, 0x00008234,
		0x030, 0x00009234,
		0x030, 0x0000A234,
		0x030, 0x0000B234,
		0x0EF, 0x00000000,
		0x0EF, 0x00000040,
		0x030, 0x00000733,
		0x030, 0x00001733,
		0x030, 0x00002420,
		0x030, 0x00003420,
		0x030, 0x00004310,
		0x030, 0x00005310,
		0x030, 0x00008733,
		0x030, 0x0000A420,
		0x030, 0x0000C310,
		0x0EF, 0x00000000,
		0x0EE, 0x00010000,
	0x80000001,	0x00000000,	0x40000000,	0x00000000,
		0x033, 0x00000200,
		0x03F, 0x00000004,
		0x033, 0x00000201,
		0x03F, 0x00000007,
		0x033, 0x00000202,
		0x03F, 0x00000024,
		0x033, 0x00000203,
		0x03F, 0x00000027,
		0x033, 0x00000204,
		0x03F, 0x0000002A,
		0x033, 0x00000205,
		0x03F, 0x0000002D,
		0x033, 0x00000206,
		0x03F, 0x0000006A,
		0x033, 0x00000207,
		0x03F, 0x0000006D,
		0x033, 0x00000208,
		0x03F, 0x00000070,
		0x033, 0x00000209,
		0x03F, 0x0000046D,
		0x033, 0x0000020A,
		0x03F, 0x00000470,
	0x90000003,	0x00000000,	0x40000000,	0x00000000,
		0x033, 0x00000200,
		0x03F, 0x00000004,
		0x033, 0x00000201,
		0x03F, 0x00000007,
		0x033, 0x00000202,
		0x03F, 0x00000024,
		0x033, 0x00000203,
		0x03F, 0x00000027,
		0x033, 0x00000204,
		0x03F, 0x0000002A,
		0x033, 0x00000205,
		0x03F, 0x0000002D,
		0x033, 0x00000206,
		0x03F, 0x0000006A,
		0x033, 0x00000207,
		0x03F, 0x0000006D,
		0x033, 0x00000208,
		0x03F, 0x00000070,
		0x033, 0x00000209,
		0x03F, 0x0000046D,
		0x033, 0x0000020A,
		0x03F, 0x00000470,
	0xA0000000,	0x00000000,
		0x033, 0x00000200,
		0x03F, 0x0000006A,
		0x033, 0x00000201,
		0x03F, 0x0000006D,
		0x033, 0x00000202,
		0x03F, 0x0000046A,
		0x033, 0x00000203,
		0x03F, 0x0000086A,
		0x033, 0x00000204,
		0x03F, 0x00000C89,
		0x033, 0x00000205,
		0x03F, 0x00000CE8,
		0x033, 0x00000206,
		0x03F, 0x00000CEB,
		0x033, 0x00000207,
		0x03F, 0x00000CEE,
		0x033, 0x00000208,
		0x03F, 0x00000CF1,
		0x033, 0x00000209,
		0x03F, 0x00000CF4,
		0x033, 0x0000020A,
		0x03F, 0x00000CF7,
	0xB0000000,	0x00000000,
	0x80000001,	0x00000000,	0x40000000,	0x00000000,
		0x033, 0x00000280,
		0x03F, 0x00000004,
		0x033, 0x00000281,
		0x03F, 0x00000007,
		0x033, 0x00000282,
		0x03F, 0x00000024,
		0x033, 0x00000283,
		0x03F, 0x00000027,
		0x033, 0x00000284,
		0x03F, 0x0000002A,
		0x033, 0x00000285,
		0x03F, 0x0000002D,
		0x033, 0x00000286,
		0x03F, 0x0000006A,
		0x033, 0x00000287,
		0x03F, 0x0000006D,
		0x033, 0x00000288,
		0x03F, 0x00000070,
		0x033, 0x00000289,
		0x03F, 0x0000046D,
		0x033, 0x0000028A,
		0x03F, 0x00000470,
	0x90000003,	0x00000000,	0x40000000,	0x00000000,
		0x033, 0x00000280,
		0x03F, 0x00000004,
		0x033, 0x00000281,
		0x03F, 0x00000007,
		0x033, 0x00000282,
		0x03F, 0x00000024,
		0x033, 0x00000283,
		0x03F, 0x00000027,
		0x033, 0x00000284,
		0x03F, 0x0000002A,
		0x033, 0x00000285,
		0x03F, 0x0000002D,
		0x033, 0x00000286,
		0x03F, 0x0000006A,
		0x033, 0x00000287,
		0x03F, 0x0000006D,
		0x033, 0x00000288,
		0x03F, 0x00000070,
		0x033, 0x00000289,
		0x03F, 0x0000046D,
		0x033, 0x0000028A,
		0x03F, 0x00000470,
	0xA0000000,	0x00000000,
		0x033, 0x00000280,
		0x03F, 0x0000006A,
		0x033, 0x00000281,
		0x03F, 0x0000006D,
		0x033, 0x00000282,
		0x03F, 0x0000046A,
		0x033, 0x00000283,
		0x03F, 0x0000086A,
		0x033, 0x00000284,
		0x03F, 0x00000C89,
		0x033, 0x00000285,
		0x03F, 0x00000CE8,
		0x033, 0x00000286,
		0x03F, 0x00000CEB,
		0x033, 0x00000287,
		0x03F, 0x00000CEE,
		0x033, 0x00000288,
		0x03F, 0x00000CF1,
		0x033, 0x00000289,
		0x03F, 0x00000CF4,
		0x033, 0x0000028A,
		0x03F, 0x00000CF7,
	0xB0000000,	0x00000000,
	0x80000001,	0x00000000,	0x40000000,	0x00000000,
		0x033, 0x00000300,
		0x03F, 0x00000004,
		0x033, 0x00000301,
		0x03F, 0x00000007,
		0x033, 0x00000302,
		0x03F, 0x00000024,
		0x033, 0x00000303,
		0x03F, 0x00000027,
		0x033, 0x00000304,
		0x03F, 0x0000002A,
		0x033, 0x00000305,
		0x03F, 0x0000002D,
		0x033, 0x00000306,
		0x03F, 0x0000006A,
		0x033, 0x00000307,
		0x03F, 0x0000006D,
		0x033, 0x00000308,
		0x03F, 0x00000070,
		0x033, 0x00000309,
		0x03F, 0x0000046D,
		0x033, 0x0000030A,
		0x03F, 0x00000470,
		0x0EE, 0x00000000,
	0x90000003,	0x00000000,	0x40000000,	0x00000000,
		0x033, 0x00000300,
		0x03F, 0x00000004,
		0x033, 0x00000301,
		0x03F, 0x00000007,
		0x033, 0x00000302,
		0x03F, 0x00000024,
		0x033, 0x00000303,
		0x03F, 0x00000027,
		0x033, 0x00000304,
		0x03F, 0x0000002A,
		0x033, 0x00000305,
		0x03F, 0x0000002D,
		0x033, 0x00000306,
		0x03F, 0x0000006A,
		0x033, 0x00000307,
		0x03F, 0x0000006D,
		0x033, 0x00000308,
		0x03F, 0x00000070,
		0x033, 0x00000309,
		0x03F, 0x0000046D,
		0x033, 0x0000030A,
		0x03F, 0x00000470,
		0x0EE, 0x00000000,
	0xA0000000,	0x00000000,
		0x033, 0x00000300,
		0x03F, 0x0000006A,
		0x033, 0x00000301,
		0x03F, 0x0000006D,
		0x033, 0x00000302,
		0x03F, 0x0000046A,
		0x033, 0x00000303,
		0x03F, 0x0000086A,
		0x033, 0x00000304,
		0x03F, 0x00000C89,
		0x033, 0x00000305,
		0x03F, 0x00000CE8,
		0x033, 0x00000306,
		0x03F, 0x00000CEB,
		0x033, 0x00000307,
		0x03F, 0x00000CEE,
		0x033, 0x00000308,
		0x03F, 0x00000CF1,
		0x033, 0x00000309,
		0x03F, 0x00000CF4,
		0x033, 0x0000030A,
		0x03F, 0x00000CF7,
		0x0EE, 0x00000000,
	0xB0000000,	0x00000000,
		0x05C, 0x000FCC00,
		0x067, 0x0000A505,
		0x043, 0x00005000,
		0x059, 0x000A0000,
		0x05A, 0x00060000,
		0x05B, 0x00014000,
		0x001, 0x00040023,
		0x0FE, 0x00000000,
		0x01B, 0x00003A40,
		0x0EE, 0x00000002,
		0x033, 0x00000017,
	0x80000001,	0x00000000,	0x40000000,	0x00000000,
		0x03F, 0x0000001F,
	0x90000003,	0x00000000,	0x40000000,	0x00000000,
		0x03F, 0x0000001F,
	0xA0000000,	0x00000000,
		0x03F, 0x0000003F,
	0xB0000000,	0x00000000,
		0x033, 0x00000018,
	0x80000001,	0x00000000,	0x40000000,	0x00000000,
		0x03F, 0x0000001F,
	0x90000003,	0x00000000,	0x40000000,	0x00000000,
		0x03F, 0x0000001F,
	0xA0000000,	0x00000000,
		0x03F, 0x0000003F,
	0xB0000000,	0x00000000,
		0x033, 0x00000019,
		0x03F, 0x00000000,
		0x033, 0x0000001A,
		0x03F, 0x0000003F,
		0x033, 0x0000001B,
		0x03F, 0x0000003F,
		0x033, 0x0000001C,
		0x03F, 0x0000003F,
		0x0EE, 0x00000000,

};

void
odm_read_and_config_mp_8812f_radiob(struct dm_struct *dm)
{
	u32	i = 0;
	u8	c_cond;
	boolean	is_matched = true, is_skipped = false;
	u32	array_len =
			sizeof(array_mp_8812f_radiob) / sizeof(u32);
	u32	*array = (u32 *)array_mp_8812f_radiob;

	u32	v1 = 0, v2 = 0, pre_v1 = 0, pre_v2 = 0;
	u32	a1 = 0, a2 = 0, a3 = 0, a4 = 0;

	PHYDM_DBG(dm, ODM_COMP_INIT, "===> %s\n", __func__);

	while ((i + 1) < array_len) {
		v1 = array[i];
		v2 = array[i + 1];

		if (v1 & (BIT(31) | BIT(30))) {/*positive & negative condition*/
			if (v1 & BIT(31)) {/* positive condition*/
				c_cond  =
					(u8)((v1 & (BIT(29) | BIT(28))) >> 28);
				if (c_cond == COND_ENDIF) {/*end*/
					is_matched = true;
					is_skipped = false;
					PHYDM_DBG(dm, ODM_COMP_INIT, "ENDIF\n");
				} else if (c_cond == COND_ELSE) { /*else*/
					is_matched = is_skipped ? false : true;
					PHYDM_DBG(dm, ODM_COMP_INIT, "ELSE\n");
				} else {/*if , else if*/
					pre_v1 = v1;
					pre_v2 = v2;
					PHYDM_DBG(dm, ODM_COMP_INIT,
						  "IF or ELSE IF\n");
				}
			} else if (v1 & BIT(30)) { /*negative condition*/
				if (!is_skipped) {
					a1 = pre_v1; a2 = pre_v2;
					a3 = v1; a4 = v2;
					if (check_positive(dm,
							   a1, a2, a3, a4)) {
						is_matched = true;
						is_skipped = true;
					} else {
						is_matched = false;
						is_skipped = false;
					}
				} else {
					is_matched = false;
				}
			}
		} else {
			if (is_matched)
				odm_config_rf_radio_b_8812f(dm, v1, v2);
		}
		i = i + 2;
	}
}

u32
odm_get_version_mp_8812f_radiob(void)
{
		return 11;
}

/******************************************************************************
 *                           txpowertrack.TXT
 ******************************************************************************/

#ifdef CONFIG_8812F
const u8 delta_swingidx_mp_5gb_n_txpwrtrk_8812f[][D_S_SIZE] = {
	{0, 1, 2, 3, 3, 4, 5, 6, 6, 7, 8, 9, 9, 10, 10, 11, 11,
	 12, 12, 12, 13, 13, 14, 14, 14, 15, 15, 15, 15, 15},
	{0, 1, 1, 2, 2, 3, 3, 4, 5, 5, 6, 7, 7, 8, 8, 9, 10, 10,
	 11, 12, 12, 13, 13, 14, 14, 14, 14, 14, 14, 14},
	{0, 1, 2, 2, 3, 3, 4, 4, 5, 6, 6, 7, 7, 8, 9, 9, 10, 10,
	 11, 12, 12, 13, 13, 14, 14, 14, 14, 14, 14, 14},
};

const u8 delta_swingidx_mp_5gb_p_txpwrtrk_8812f[][D_S_SIZE] = {
	{0, 1, 2, 2, 3, 4, 5, 6, 7, 8, 8, 9, 10, 11, 11, 12, 13,
	 14, 15, 15, 16, 17, 18, 18, 19, 19, 19, 19, 19, 19},
	{0, 1, 2, 2, 3, 4, 5, 6, 6, 7, 8, 8, 9, 9, 10, 11, 12,
	 12, 13, 14, 15, 16, 17, 17, 18, 18, 18, 18, 18, 18},
	{0, 1, 2, 2, 3, 4, 5, 5, 6, 6, 7, 8, 8, 9, 10, 10, 11,
	 12, 13, 14, 15, 15, 16, 16, 17, 17, 17, 17, 17, 17},
};

const u8 delta_swingidx_mp_5ga_n_txpwrtrk_8812f[][D_S_SIZE] = {
	{0, 1, 2, 2, 3, 3, 4, 5, 6, 7, 8, 8, 9, 9, 10, 11, 11,
	 12, 12, 12, 13, 13, 14, 14, 14, 15, 15, 15, 15, 15},
	{0, 1, 2, 2, 3, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 10, 10,
	 11, 11, 12, 12, 13, 13, 14, 14, 14, 14, 14, 14, 14},
	{0, 1, 2, 2, 3, 4, 4, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10, 10,
	 11, 12, 12, 13, 13, 14, 14, 14, 14, 14, 14, 14},
};

const u8 delta_swingidx_mp_5ga_p_txpwrtrk_8812f[][D_S_SIZE] = {
	{0, 1, 2, 2, 3, 4, 5, 5, 6, 7, 8, 9, 9, 10, 11, 12, 13,
	 14, 14, 15, 16, 17, 18, 19, 19, 20, 20, 20, 20, 20},
	{0, 1, 2, 2, 3, 4, 4, 5, 6, 6, 7, 7, 8, 9, 9, 10, 11,
	 11, 12, 13, 14, 15, 16, 16, 17, 17, 18, 18, 18, 18},
	{0, 1, 2, 3, 3, 4, 5, 5, 6, 6, 7, 8, 8, 9, 10, 11, 12,
	 12, 13, 14, 15, 15, 16, 17, 17, 18, 18, 18, 18, 18},
};

const u8 delta_swingidx_mp_2gb_n_txpwrtrk_8812f[]    = {
	0, 1, 2, 3, 3, 4, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 13,
	 14, 15, 16, 16, 17, 18, 18, 18, 18, 18, 18, 18, 18};
const u8 delta_swingidx_mp_2gb_p_txpwrtrk_8812f[]    = {
	0, 1, 1, 2, 3, 4, 4, 5, 6, 7, 7, 8, 9, 10, 11, 12, 12,
	 13, 14, 15, 16, 17, 17, 18, 19, 20, 21, 22, 22, 22};
const u8 delta_swingidx_mp_2ga_n_txpwrtrk_8812f[]    = {
	0, 1, 2, 3, 3, 4, 5, 6, 6, 7, 8, 9, 10, 11, 12, 13, 13,
	 14, 15, 16, 17, 18, 18, 18, 18, 18, 18, 18, 18, 18};
const u8 delta_swingidx_mp_2ga_p_txpwrtrk_8812f[]    = {
	0, 1, 2, 3, 3, 4, 5, 6, 6, 7, 8, 9, 9, 10, 11, 12, 12,
	 13, 14, 15, 16, 17, 18, 19, 19, 20, 21, 22, 22, 22};
const u8 delta_swingidx_mp_2g_cck_b_n_txpwrtrk_8812f[] = {
	0, 1, 1, 2, 3, 4, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 13,
	 14, 15, 16, 16, 17, 17, 17, 17, 17, 17, 17, 17, 17};
const u8 delta_swingidx_mp_2g_cck_b_p_txpwrtrk_8812f[] = {
	0, 1, 1, 2, 3, 4, 4, 5, 6, 7, 8, 9, 9, 10, 11, 12, 12,
	 13, 14, 15, 16, 17, 18, 19, 19, 20, 21, 22, 22, 22};
const u8 delta_swingidx_mp_2g_cck_a_n_txpwrtrk_8812f[] = {
	0, 1, 2, 3, 3, 4, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 13,
	 14, 15, 16, 17, 18, 18, 18, 18, 18, 18, 18, 18, 18};
const u8 delta_swingidx_mp_2g_cck_a_p_txpwrtrk_8812f[] = {
	0, 1, 2, 3, 3, 4, 5, 6, 6, 7, 8, 9, 9, 10, 11, 12, 12,
	 13, 14, 15, 16, 17, 18, 19, 19, 20, 21, 22, 22, 22};
#endif

void
odm_read_and_config_mp_8812f_txpowertrack(struct dm_struct *dm)
{
#ifdef CONFIG_8812F

struct dm_rf_calibration_struct  *cali_info = &dm->rf_calibrate_info;

PHYDM_DBG(dm, ODM_COMP_INIT, "===> ODM_ReadAndConfig_MP_mp_8812f\n");

odm_move_memory(dm, cali_info->delta_swing_table_idx_2ga_p,
		(void *)delta_swingidx_mp_2ga_p_txpwrtrk_8812f,
		DELTA_SWINGIDX_SIZE);
odm_move_memory(dm, cali_info->delta_swing_table_idx_2ga_n,
		(void *)delta_swingidx_mp_2ga_n_txpwrtrk_8812f,
		DELTA_SWINGIDX_SIZE);
odm_move_memory(dm, cali_info->delta_swing_table_idx_2gb_p,
		(void *)delta_swingidx_mp_2gb_p_txpwrtrk_8812f,
		DELTA_SWINGIDX_SIZE);
odm_move_memory(dm, cali_info->delta_swing_table_idx_2gb_n,
		(void *)delta_swingidx_mp_2gb_n_txpwrtrk_8812f,
		DELTA_SWINGIDX_SIZE);

odm_move_memory(dm, cali_info->delta_swing_table_idx_2g_cck_a_p,
		(void *)delta_swingidx_mp_2g_cck_a_p_txpwrtrk_8812f,
		DELTA_SWINGIDX_SIZE);
odm_move_memory(dm, cali_info->delta_swing_table_idx_2g_cck_a_n,
		(void *)delta_swingidx_mp_2g_cck_a_n_txpwrtrk_8812f,
		DELTA_SWINGIDX_SIZE);
odm_move_memory(dm, cali_info->delta_swing_table_idx_2g_cck_b_p,
		(void *)delta_swingidx_mp_2g_cck_b_p_txpwrtrk_8812f,
		DELTA_SWINGIDX_SIZE);
odm_move_memory(dm, cali_info->delta_swing_table_idx_2g_cck_b_n,
		(void *)delta_swingidx_mp_2g_cck_b_n_txpwrtrk_8812f,
		DELTA_SWINGIDX_SIZE);

odm_move_memory(dm, cali_info->delta_swing_table_idx_5ga_p,
		(void *)delta_swingidx_mp_5ga_p_txpwrtrk_8812f,
		DELTA_SWINGIDX_SIZE * 3);
odm_move_memory(dm, cali_info->delta_swing_table_idx_5ga_n,
		(void *)delta_swingidx_mp_5ga_n_txpwrtrk_8812f,
		DELTA_SWINGIDX_SIZE * 3);
odm_move_memory(dm, cali_info->delta_swing_table_idx_5gb_p,
		(void *)delta_swingidx_mp_5gb_p_txpwrtrk_8812f,
		DELTA_SWINGIDX_SIZE * 3);
odm_move_memory(dm, cali_info->delta_swing_table_idx_5gb_n,
		(void *)delta_swingidx_mp_5gb_n_txpwrtrk_8812f,
		DELTA_SWINGIDX_SIZE * 3);
#endif
}

/******************************************************************************
 *                           txpowertrack_type0.TXT
 ******************************************************************************/

#ifdef CONFIG_8812F_TYPE0
const u8 delta_swingidx_mp_5gb_n_txpwrtrk_type0_8812f[][D_S_SIZE] = {
	{0, 0, 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7,
	 7, 8, 8, 8, 9, 9, 9, 10, 10, 11, 11, 12, 12},
	{0, 0, 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7,
	 7, 8, 8, 8, 9, 9, 9, 10, 10, 11, 11, 12, 12},
	{0, 0, 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7,
	 7, 8, 8, 8, 9, 9, 9, 10, 10, 11, 11, 12, 12},
};

const u8 delta_swingidx_mp_5gb_p_txpwrtrk_type0_8812f[][D_S_SIZE] = {
	{0, 0, 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7,
	 7, 8, 8, 8, 9, 9, 9, 10, 10, 11, 11, 12, 12},
	{0, 0, 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7,
	 7, 8, 8, 8, 9, 9, 9, 10, 10, 11, 11, 12, 12},
	{0, 0, 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7,
	 7, 8, 8, 8, 9, 9, 9, 10, 10, 11, 11, 12, 12},
};

const u8 delta_swingidx_mp_5ga_n_txpwrtrk_type0_8812f[][D_S_SIZE] = {
	{0, 0, 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7,
	 7, 8, 8, 8, 9, 9, 9, 10, 10, 11, 11, 12, 12},
	{0, 0, 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7,
	 7, 8, 8, 8, 9, 9, 9, 10, 10, 11, 11, 12, 12},
	{0, 0, 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7,
	 7, 8, 8, 8, 9, 9, 9, 10, 10, 11, 11, 12, 12},
};

const u8 delta_swingidx_mp_5ga_p_txpwrtrk_type0_8812f[][D_S_SIZE] = {
	{0, 0, 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7,
	 7, 8, 8, 8, 9, 9, 9, 10, 10, 11, 11, 12, 12},
	{0, 0, 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7,
	 7, 8, 8, 8, 9, 9, 9, 10, 10, 11, 11, 12, 12},
	{0, 0, 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7,
	 7, 8, 8, 8, 9, 9, 9, 10, 10, 11, 11, 12, 12},
};

const u8 delta_swingidx_mp_2gb_n_txpwrtrk_type0_8812f[]    = {
	0, 1, 2, 3, 3, 4, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 13,
	 14, 15, 16, 16, 17, 18, 18, 18, 18, 18, 18, 18, 18};
const u8 delta_swingidx_mp_2gb_p_txpwrtrk_type0_8812f[]    = {
	0, 1, 1, 2, 3, 4, 4, 5, 6, 7, 7, 8, 9, 10, 11, 12, 12,
	 13, 14, 15, 16, 17, 17, 18, 19, 20, 21, 22, 22, 22};
const u8 delta_swingidx_mp_2ga_n_txpwrtrk_type0_8812f[]    = {
	0, 1, 2, 3, 3, 4, 5, 6, 6, 7, 8, 9, 10, 11, 12, 13, 13,
	 14, 15, 16, 17, 18, 18, 18, 18, 18, 18, 18, 18, 18};
const u8 delta_swingidx_mp_2ga_p_txpwrtrk_type0_8812f[]    = {
	0, 1, 2, 3, 3, 4, 5, 6, 6, 7, 8, 9, 9, 10, 11, 12, 12,
	 13, 14, 15, 16, 17, 18, 19, 19, 20, 21, 22, 22, 22};
const u8 delta_swingidx_mp_2g_cck_b_n_txpwrtrk_type0_8812f[] = {
	0, 1, 1, 2, 3, 4, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 13,
	 14, 15, 16, 16, 17, 17, 17, 17, 17, 17, 17, 17, 17};
const u8 delta_swingidx_mp_2g_cck_b_p_txpwrtrk_type0_8812f[] = {
	0, 1, 1, 2, 3, 4, 4, 5, 6, 7, 8, 9, 9, 10, 11, 12, 12,
	 13, 14, 15, 16, 17, 18, 19, 19, 20, 21, 22, 22, 22};
const u8 delta_swingidx_mp_2g_cck_a_n_txpwrtrk_type0_8812f[] = {
	0, 1, 2, 3, 3, 4, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 13,
	 14, 15, 16, 17, 18, 18, 18, 18, 18, 18, 18, 18, 18};
const u8 delta_swingidx_mp_2g_cck_a_p_txpwrtrk_type0_8812f[] = {
	0, 1, 2, 3, 3, 4, 5, 6, 6, 7, 8, 9, 9, 10, 11, 12, 12,
	 13, 14, 15, 16, 17, 18, 19, 19, 20, 21, 22, 22, 22};
#endif

void
odm_read_and_config_mp_8812f_txpowertrack_type0(struct dm_struct *dm)
{
#ifdef CONFIG_8812F_TYPE0

struct dm_rf_calibration_struct  *cali_info = &dm->rf_calibrate_info;

PHYDM_DBG(dm, ODM_COMP_INIT, "===> ODM_ReadAndConfig_MP_mp_8812f\n");

odm_move_memory(dm, cali_info->delta_swing_table_idx_2ga_p,
		(void *)delta_swingidx_mp_2ga_p_txpwrtrk_type0_8812f,
		DELTA_SWINGIDX_SIZE);
odm_move_memory(dm, cali_info->delta_swing_table_idx_2ga_n,
		(void *)delta_swingidx_mp_2ga_n_txpwrtrk_type0_8812f,
		DELTA_SWINGIDX_SIZE);
odm_move_memory(dm, cali_info->delta_swing_table_idx_2gb_p,
		(void *)delta_swingidx_mp_2gb_p_txpwrtrk_type0_8812f,
		DELTA_SWINGIDX_SIZE);
odm_move_memory(dm, cali_info->delta_swing_table_idx_2gb_n,
		(void *)delta_swingidx_mp_2gb_n_txpwrtrk_type0_8812f,
		DELTA_SWINGIDX_SIZE);

odm_move_memory(dm, cali_info->delta_swing_table_idx_2g_cck_a_p,
		(void *)delta_swingidx_mp_2g_cck_a_p_txpwrtrk_type0_8812f,
		DELTA_SWINGIDX_SIZE);
odm_move_memory(dm, cali_info->delta_swing_table_idx_2g_cck_a_n,
		(void *)delta_swingidx_mp_2g_cck_a_n_txpwrtrk_type0_8812f,
		DELTA_SWINGIDX_SIZE);
odm_move_memory(dm, cali_info->delta_swing_table_idx_2g_cck_b_p,
		(void *)delta_swingidx_mp_2g_cck_b_p_txpwrtrk_type0_8812f,
		DELTA_SWINGIDX_SIZE);
odm_move_memory(dm, cali_info->delta_swing_table_idx_2g_cck_b_n,
		(void *)delta_swingidx_mp_2g_cck_b_n_txpwrtrk_type0_8812f,
		DELTA_SWINGIDX_SIZE);

odm_move_memory(dm, cali_info->delta_swing_table_idx_5ga_p,
		(void *)delta_swingidx_mp_5ga_p_txpwrtrk_type0_8812f,
		DELTA_SWINGIDX_SIZE * 3);
odm_move_memory(dm, cali_info->delta_swing_table_idx_5ga_n,
		(void *)delta_swingidx_mp_5ga_n_txpwrtrk_type0_8812f,
		DELTA_SWINGIDX_SIZE * 3);
odm_move_memory(dm, cali_info->delta_swing_table_idx_5gb_p,
		(void *)delta_swingidx_mp_5gb_p_txpwrtrk_type0_8812f,
		DELTA_SWINGIDX_SIZE * 3);
odm_move_memory(dm, cali_info->delta_swing_table_idx_5gb_n,
		(void *)delta_swingidx_mp_5gb_n_txpwrtrk_type0_8812f,
		DELTA_SWINGIDX_SIZE * 3);
#endif
}

/******************************************************************************
 *                           txpowertrack_type1.TXT
 ******************************************************************************/

#ifdef CONFIG_8812F_TYPE1
const u8 delta_swingidx_mp_5gb_n_txpwrtrk_type1_8812f[][D_S_SIZE] = {
	{0, 2, 4, 5, 7, 8, 9, 10, 12, 13, 14, 15, 16, 18, 20, 21,
	 22, 24, 25, 27, 28, 29, 30, 30, 30, 30, 30, 30, 30, 30},
	{0, 2, 4, 5, 7, 8, 9, 10, 12, 13, 14, 15, 16, 18, 20, 21,
	 22, 24, 25, 27, 28, 29, 30, 30, 30, 30, 30, 30, 30, 30},
	{0, 2, 4, 5, 7, 8, 9, 10, 11, 12, 13, 15, 16, 17, 18, 19,
	 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 30, 30, 30},
};

const u8 delta_swingidx_mp_5gb_p_txpwrtrk_type1_8812f[][D_S_SIZE] = {
	{0, 1, 2, 4, 5, 7, 8, 10, 12, 13, 15, 16, 18, 19, 20, 22,
	 23, 24, 26, 28, 30, 31, 33, 34, 36, 37, 39, 40, 40, 40},
	{0, 1, 2, 4, 5, 7, 8, 10, 12, 13, 15, 16, 18, 19, 20, 22,
	 23, 24, 26, 28, 30, 31, 33, 34, 36, 37, 39, 40, 40, 40},
	{0, 1, 2, 4, 6, 7, 9, 10, 12, 13, 15, 17, 19, 20, 22, 23,
	 25, 27, 28, 30, 31, 33, 34, 36, 37, 38, 40, 40, 40, 40},
};

const u8 delta_swingidx_mp_5ga_n_txpwrtrk_type1_8812f[][D_S_SIZE] = {
	{0, 2, 4, 5, 7, 8, 9, 10, 12, 13, 14, 15, 16, 18, 20, 21,
	 22, 24, 25, 27, 28, 29, 30, 30, 30, 30, 30, 30, 30, 30},
	{0, 2, 4, 5, 7, 8, 9, 10, 12, 13, 14, 15, 16, 18, 20, 21,
	 22, 24, 25, 27, 28, 29, 30, 30, 30, 30, 30, 30, 30, 30},
	{0, 2, 4, 5, 7, 8, 9, 11, 12, 13, 14, 15, 16, 17, 18, 19,
	 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 30, 30, 30},
};

const u8 delta_swingidx_mp_5ga_p_txpwrtrk_type1_8812f[][D_S_SIZE] = {
	{0, 1, 2, 4, 5, 7, 8, 10, 12, 13, 15, 16, 18, 19, 20, 22,
	 23, 24, 26, 28, 30, 31, 33, 34, 36, 37, 39, 40, 40, 40},
	{0, 1, 2, 4, 5, 7, 8, 10, 12, 13, 15, 16, 18, 19, 20, 22,
	 23, 24, 26, 28, 30, 31, 33, 34, 36, 37, 39, 40, 40, 40},
	{0, 1, 2, 4, 5, 7, 8, 10, 12, 14, 15, 17, 18, 19, 21, 22,
	 24, 25, 27, 29, 31, 32, 34, 36, 37, 39, 40, 40, 40, 40},
};

const u8 delta_swingidx_mp_2gb_n_txpwrtrk_type1_8812f[]    = {
	0, 1, 2, 3, 3, 4, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 13,
	 14, 15, 16, 16, 17, 18, 18, 18, 18, 18, 18, 18, 18};
const u8 delta_swingidx_mp_2gb_p_txpwrtrk_type1_8812f[]    = {
	0, 1, 1, 2, 3, 4, 4, 5, 6, 7, 7, 8, 9, 10, 11, 12, 12,
	 13, 14, 15, 16, 17, 17, 18, 19, 20, 21, 22, 22, 22};
const u8 delta_swingidx_mp_2ga_n_txpwrtrk_type1_8812f[]    = {
	0, 1, 2, 3, 3, 4, 5, 6, 6, 7, 8, 9, 10, 11, 12, 13, 13,
	 14, 15, 16, 17, 18, 18, 18, 18, 18, 18, 18, 18, 18};
const u8 delta_swingidx_mp_2ga_p_txpwrtrk_type1_8812f[]    = {
	0, 1, 2, 3, 3, 4, 5, 6, 6, 7, 8, 9, 9, 10, 11, 12, 12,
	 13, 14, 15, 16, 17, 18, 19, 19, 20, 21, 22, 22, 22};
const u8 delta_swingidx_mp_2g_cck_b_n_txpwrtrk_type1_8812f[] = {
	0, 1, 1, 2, 3, 4, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 13,
	 14, 15, 16, 16, 17, 17, 17, 17, 17, 17, 17, 17, 17};
const u8 delta_swingidx_mp_2g_cck_b_p_txpwrtrk_type1_8812f[] = {
	0, 1, 1, 2, 3, 4, 4, 5, 6, 7, 8, 9, 9, 10, 11, 12, 12,
	 13, 14, 15, 16, 17, 18, 19, 19, 20, 21, 22, 22, 22};
const u8 delta_swingidx_mp_2g_cck_a_n_txpwrtrk_type1_8812f[] = {
	0, 1, 2, 3, 3, 4, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 13,
	 14, 15, 16, 17, 18, 18, 18, 18, 18, 18, 18, 18, 18};
const u8 delta_swingidx_mp_2g_cck_a_p_txpwrtrk_type1_8812f[] = {
	0, 1, 2, 3, 3, 4, 5, 6, 6, 7, 8, 9, 9, 10, 11, 12, 12,
	 13, 14, 15, 16, 17, 18, 19, 19, 20, 21, 22, 22, 22};
#endif

void
odm_read_and_config_mp_8812f_txpowertrack_type1(struct dm_struct *dm)
{
#ifdef CONFIG_8812F_TYPE1

struct dm_rf_calibration_struct  *cali_info = &dm->rf_calibrate_info;

PHYDM_DBG(dm, ODM_COMP_INIT, "===> ODM_ReadAndConfig_MP_mp_8812f\n");

odm_move_memory(dm, cali_info->delta_swing_table_idx_2ga_p,
		(void *)delta_swingidx_mp_2ga_p_txpwrtrk_type1_8812f,
		DELTA_SWINGIDX_SIZE);
odm_move_memory(dm, cali_info->delta_swing_table_idx_2ga_n,
		(void *)delta_swingidx_mp_2ga_n_txpwrtrk_type1_8812f,
		DELTA_SWINGIDX_SIZE);
odm_move_memory(dm, cali_info->delta_swing_table_idx_2gb_p,
		(void *)delta_swingidx_mp_2gb_p_txpwrtrk_type1_8812f,
		DELTA_SWINGIDX_SIZE);
odm_move_memory(dm, cali_info->delta_swing_table_idx_2gb_n,
		(void *)delta_swingidx_mp_2gb_n_txpwrtrk_type1_8812f,
		DELTA_SWINGIDX_SIZE);

odm_move_memory(dm, cali_info->delta_swing_table_idx_2g_cck_a_p,
		(void *)delta_swingidx_mp_2g_cck_a_p_txpwrtrk_type1_8812f,
		DELTA_SWINGIDX_SIZE);
odm_move_memory(dm, cali_info->delta_swing_table_idx_2g_cck_a_n,
		(void *)delta_swingidx_mp_2g_cck_a_n_txpwrtrk_type1_8812f,
		DELTA_SWINGIDX_SIZE);
odm_move_memory(dm, cali_info->delta_swing_table_idx_2g_cck_b_p,
		(void *)delta_swingidx_mp_2g_cck_b_p_txpwrtrk_type1_8812f,
		DELTA_SWINGIDX_SIZE);
odm_move_memory(dm, cali_info->delta_swing_table_idx_2g_cck_b_n,
		(void *)delta_swingidx_mp_2g_cck_b_n_txpwrtrk_type1_8812f,
		DELTA_SWINGIDX_SIZE);

odm_move_memory(dm, cali_info->delta_swing_table_idx_5ga_p,
		(void *)delta_swingidx_mp_5ga_p_txpwrtrk_type1_8812f,
		DELTA_SWINGIDX_SIZE * 3);
odm_move_memory(dm, cali_info->delta_swing_table_idx_5ga_n,
		(void *)delta_swingidx_mp_5ga_n_txpwrtrk_type1_8812f,
		DELTA_SWINGIDX_SIZE * 3);
odm_move_memory(dm, cali_info->delta_swing_table_idx_5gb_p,
		(void *)delta_swingidx_mp_5gb_p_txpwrtrk_type1_8812f,
		DELTA_SWINGIDX_SIZE * 3);
odm_move_memory(dm, cali_info->delta_swing_table_idx_5gb_n,
		(void *)delta_swingidx_mp_5gb_n_txpwrtrk_type1_8812f,
		DELTA_SWINGIDX_SIZE * 3);
#endif
}

/******************************************************************************
 *                           txpowertrack_type2.TXT
 ******************************************************************************/

#ifdef CONFIG_8812F_TYPE2
const u8 delta_swingidx_mp_5gb_n_txpwrtrk_type2_8812f[][D_S_SIZE] = {
	{0, 0, 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7,
	 7, 8, 8, 8, 9, 9, 9, 10, 10, 11, 11, 12, 12},
	{0, 0, 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7,
	 7, 8, 8, 8, 9, 9, 9, 10, 10, 11, 11, 12, 12},
	{0, 0, 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7,
	 7, 8, 8, 8, 9, 9, 9, 10, 10, 11, 11, 12, 12},
};

const u8 delta_swingidx_mp_5gb_p_txpwrtrk_type2_8812f[][D_S_SIZE] = {
	{0, 0, 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7,
	 7, 8, 8, 8, 9, 9, 9, 10, 10, 11, 11, 12, 12},
	{0, 0, 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7,
	 7, 8, 8, 8, 9, 9, 9, 10, 10, 11, 11, 12, 12},
	{0, 0, 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7,
	 7, 8, 8, 8, 9, 9, 9, 10, 10, 11, 11, 12, 12},
};

const u8 delta_swingidx_mp_5ga_n_txpwrtrk_type2_8812f[][D_S_SIZE] = {
	{0, 0, 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7,
	 7, 8, 8, 8, 9, 9, 9, 10, 10, 11, 11, 12, 12},
	{0, 0, 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7,
	 7, 8, 8, 8, 9, 9, 9, 10, 10, 11, 11, 12, 12},
	{0, 0, 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7,
	 7, 8, 8, 8, 9, 9, 9, 10, 10, 11, 11, 12, 12},
};

const u8 delta_swingidx_mp_5ga_p_txpwrtrk_type2_8812f[][D_S_SIZE] = {
	{0, 0, 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7,
	 7, 8, 8, 8, 9, 9, 9, 10, 10, 11, 11, 12, 12},
	{0, 0, 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7,
	 7, 8, 8, 8, 9, 9, 9, 10, 10, 11, 11, 12, 12},
	{0, 0, 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7,
	 7, 8, 8, 8, 9, 9, 9, 10, 10, 11, 11, 12, 12},
};

const u8 delta_swingidx_mp_2gb_n_txpwrtrk_type2_8812f[]    = {
	0, 1, 2, 3, 3, 4, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 13,
	 14, 15, 16, 16, 17, 18, 18, 18, 18, 18, 18, 18, 18};
const u8 delta_swingidx_mp_2gb_p_txpwrtrk_type2_8812f[]    = {
	0, 1, 1, 2, 3, 4, 4, 5, 6, 7, 7, 8, 9, 10, 11, 12, 12,
	 13, 14, 15, 16, 17, 17, 18, 19, 20, 21, 22, 22, 22};
const u8 delta_swingidx_mp_2ga_n_txpwrtrk_type2_8812f[]    = {
	0, 1, 2, 3, 3, 4, 5, 6, 6, 7, 8, 9, 10, 11, 12, 13, 13,
	 14, 15, 16, 17, 18, 18, 18, 18, 18, 18, 18, 18, 18};
const u8 delta_swingidx_mp_2ga_p_txpwrtrk_type2_8812f[]    = {
	0, 1, 2, 3, 3, 4, 5, 6, 6, 7, 8, 9, 9, 10, 11, 12, 12,
	 13, 14, 15, 16, 17, 18, 19, 19, 20, 21, 22, 22, 22};
const u8 delta_swingidx_mp_2g_cck_b_n_txpwrtrk_type2_8812f[] = {
	0, 1, 1, 2, 3, 4, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 13,
	 14, 15, 16, 16, 17, 17, 17, 17, 17, 17, 17, 17, 17};
const u8 delta_swingidx_mp_2g_cck_b_p_txpwrtrk_type2_8812f[] = {
	0, 1, 1, 2, 3, 4, 4, 5, 6, 7, 8, 9, 9, 10, 11, 12, 12,
	 13, 14, 15, 16, 17, 18, 19, 19, 20, 21, 22, 22, 22};
const u8 delta_swingidx_mp_2g_cck_a_n_txpwrtrk_type2_8812f[] = {
	0, 1, 2, 3, 3, 4, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 13,
	 14, 15, 16, 17, 18, 18, 18, 18, 18, 18, 18, 18, 18};
const u8 delta_swingidx_mp_2g_cck_a_p_txpwrtrk_type2_8812f[] = {
	0, 1, 2, 3, 3, 4, 5, 6, 6, 7, 8, 9, 9, 10, 11, 12, 12,
	 13, 14, 15, 16, 17, 18, 19, 19, 20, 21, 22, 22, 22};
#endif

void
odm_read_and_config_mp_8812f_txpowertrack_type2(struct dm_struct *dm)
{
#ifdef CONFIG_8812F_TYPE2

struct dm_rf_calibration_struct  *cali_info = &dm->rf_calibrate_info;

PHYDM_DBG(dm, ODM_COMP_INIT, "===> ODM_ReadAndConfig_MP_mp_8812f\n");

odm_move_memory(dm, cali_info->delta_swing_table_idx_2ga_p,
		(void *)delta_swingidx_mp_2ga_p_txpwrtrk_type2_8812f,
		DELTA_SWINGIDX_SIZE);
odm_move_memory(dm, cali_info->delta_swing_table_idx_2ga_n,
		(void *)delta_swingidx_mp_2ga_n_txpwrtrk_type2_8812f,
		DELTA_SWINGIDX_SIZE);
odm_move_memory(dm, cali_info->delta_swing_table_idx_2gb_p,
		(void *)delta_swingidx_mp_2gb_p_txpwrtrk_type2_8812f,
		DELTA_SWINGIDX_SIZE);
odm_move_memory(dm, cali_info->delta_swing_table_idx_2gb_n,
		(void *)delta_swingidx_mp_2gb_n_txpwrtrk_type2_8812f,
		DELTA_SWINGIDX_SIZE);

odm_move_memory(dm, cali_info->delta_swing_table_idx_2g_cck_a_p,
		(void *)delta_swingidx_mp_2g_cck_a_p_txpwrtrk_type2_8812f,
		DELTA_SWINGIDX_SIZE);
odm_move_memory(dm, cali_info->delta_swing_table_idx_2g_cck_a_n,
		(void *)delta_swingidx_mp_2g_cck_a_n_txpwrtrk_type2_8812f,
		DELTA_SWINGIDX_SIZE);
odm_move_memory(dm, cali_info->delta_swing_table_idx_2g_cck_b_p,
		(void *)delta_swingidx_mp_2g_cck_b_p_txpwrtrk_type2_8812f,
		DELTA_SWINGIDX_SIZE);
odm_move_memory(dm, cali_info->delta_swing_table_idx_2g_cck_b_n,
		(void *)delta_swingidx_mp_2g_cck_b_n_txpwrtrk_type2_8812f,
		DELTA_SWINGIDX_SIZE);

odm_move_memory(dm, cali_info->delta_swing_table_idx_5ga_p,
		(void *)delta_swingidx_mp_5ga_p_txpwrtrk_type2_8812f,
		DELTA_SWINGIDX_SIZE * 3);
odm_move_memory(dm, cali_info->delta_swing_table_idx_5ga_n,
		(void *)delta_swingidx_mp_5ga_n_txpwrtrk_type2_8812f,
		DELTA_SWINGIDX_SIZE * 3);
odm_move_memory(dm, cali_info->delta_swing_table_idx_5gb_p,
		(void *)delta_swingidx_mp_5gb_p_txpwrtrk_type2_8812f,
		DELTA_SWINGIDX_SIZE * 3);
odm_move_memory(dm, cali_info->delta_swing_table_idx_5gb_n,
		(void *)delta_swingidx_mp_5gb_n_txpwrtrk_type2_8812f,
		DELTA_SWINGIDX_SIZE * 3);
#endif
}

/******************************************************************************
 *                           txpowertrack_type3.TXT
 ******************************************************************************/

#ifdef CONFIG_8812F_TYPE3
const u8 delta_swingidx_mp_5gb_n_txpwrtrk_type3_8812f[][D_S_SIZE] = {
	{0, 2, 4, 5, 7, 8, 9, 10, 12, 13, 14, 15, 16, 18, 20, 21,
	 22, 24, 25, 27, 28, 29, 30, 30, 30, 30, 30, 30, 30, 30},
	{0, 2, 4, 5, 7, 8, 9, 10, 12, 13, 14, 15, 16, 18, 20, 21,
	 22, 24, 25, 27, 28, 29, 30, 30, 30, 30, 30, 30, 30, 30},
	{0, 2, 4, 5, 7, 8, 9, 10, 11, 12, 13, 15, 16, 17, 18, 19,
	 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 30, 30, 30},
};

const u8 delta_swingidx_mp_5gb_p_txpwrtrk_type3_8812f[][D_S_SIZE] = {
	{0, 1, 2, 4, 5, 7, 8, 10, 12, 13, 15, 16, 18, 19, 20, 22,
	 23, 24, 26, 28, 30, 31, 33, 34, 36, 37, 39, 40, 40, 40},
	{0, 1, 2, 4, 5, 7, 8, 10, 12, 13, 15, 16, 18, 19, 20, 22,
	 23, 24, 26, 28, 30, 31, 33, 34, 36, 37, 39, 40, 40, 40},
	{0, 1, 2, 4, 6, 7, 9, 10, 12, 13, 15, 17, 19, 20, 22, 23,
	 25, 27, 28, 30, 31, 33, 34, 36, 37, 38, 40, 40, 40, 40},
};

const u8 delta_swingidx_mp_5ga_n_txpwrtrk_type3_8812f[][D_S_SIZE] = {
	{0, 2, 4, 5, 7, 8, 9, 10, 12, 13, 14, 15, 16, 18, 20, 21,
	 22, 24, 25, 27, 28, 29, 30, 30, 30, 30, 30, 30, 30, 30},
	{0, 2, 4, 5, 7, 8, 9, 10, 12, 13, 14, 15, 16, 18, 20, 21,
	 22, 24, 25, 27, 28, 29, 30, 30, 30, 30, 30, 30, 30, 30},
	{0, 2, 4, 5, 7, 8, 9, 11, 12, 13, 14, 15, 16, 17, 18, 19,
	 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 30, 30, 30},
};

const u8 delta_swingidx_mp_5ga_p_txpwrtrk_type3_8812f[][D_S_SIZE] = {
	{0, 1, 2, 4, 5, 7, 8, 10, 12, 13, 15, 16, 18, 19, 20, 22,
	 23, 24, 26, 28, 30, 31, 33, 34, 36, 37, 39, 40, 40, 40},
	{0, 1, 2, 4, 5, 7, 8, 10, 12, 13, 15, 16, 18, 19, 20, 22,
	 23, 24, 26, 28, 30, 31, 33, 34, 36, 37, 39, 40, 40, 40},
	{0, 1, 2, 4, 5, 7, 8, 10, 12, 14, 15, 17, 18, 19, 21, 22,
	 24, 25, 27, 29, 31, 32, 34, 36, 37, 39, 40, 40, 40, 40},
};

const u8 delta_swingidx_mp_2gb_n_txpwrtrk_type3_8812f[]    = {
	0, 1, 2, 3, 3, 4, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 13,
	 14, 15, 16, 16, 17, 18, 18, 18, 18, 18, 18, 18, 18};
const u8 delta_swingidx_mp_2gb_p_txpwrtrk_type3_8812f[]    = {
	0, 1, 1, 2, 3, 4, 4, 5, 6, 7, 7, 8, 9, 10, 11, 12, 12,
	 13, 14, 15, 16, 17, 17, 18, 19, 20, 21, 22, 22, 22};
const u8 delta_swingidx_mp_2ga_n_txpwrtrk_type3_8812f[]    = {
	0, 1, 2, 3, 3, 4, 5, 6, 6, 7, 8, 9, 10, 11, 12, 13, 13,
	 14, 15, 16, 17, 18, 18, 18, 18, 18, 18, 18, 18, 18};
const u8 delta_swingidx_mp_2ga_p_txpwrtrk_type3_8812f[]    = {
	0, 1, 2, 3, 3, 4, 5, 6, 6, 7, 8, 9, 9, 10, 11, 12, 12,
	 13, 14, 15, 16, 17, 18, 19, 19, 20, 21, 22, 22, 22};
const u8 delta_swingidx_mp_2g_cck_b_n_txpwrtrk_type3_8812f[] = {
	0, 1, 1, 2, 3, 4, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 13,
	 14, 15, 16, 16, 17, 17, 17, 17, 17, 17, 17, 17, 17};
const u8 delta_swingidx_mp_2g_cck_b_p_txpwrtrk_type3_8812f[] = {
	0, 1, 1, 2, 3, 4, 4, 5, 6, 7, 8, 9, 9, 10, 11, 12, 12,
	 13, 14, 15, 16, 17, 18, 19, 19, 20, 21, 22, 22, 22};
const u8 delta_swingidx_mp_2g_cck_a_n_txpwrtrk_type3_8812f[] = {
	0, 1, 2, 3, 3, 4, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 13,
	 14, 15, 16, 17, 18, 18, 18, 18, 18, 18, 18, 18, 18};
const u8 delta_swingidx_mp_2g_cck_a_p_txpwrtrk_type3_8812f[] = {
	0, 1, 2, 3, 3, 4, 5, 6, 6, 7, 8, 9, 9, 10, 11, 12, 12,
	 13, 14, 15, 16, 17, 18, 19, 19, 20, 21, 22, 22, 22};
#endif

void
odm_read_and_config_mp_8812f_txpowertrack_type3(struct dm_struct *dm)
{
#ifdef CONFIG_8812F_TYPE3

struct dm_rf_calibration_struct  *cali_info = &dm->rf_calibrate_info;

PHYDM_DBG(dm, ODM_COMP_INIT, "===> ODM_ReadAndConfig_MP_mp_8812f\n");

odm_move_memory(dm, cali_info->delta_swing_table_idx_2ga_p,
		(void *)delta_swingidx_mp_2ga_p_txpwrtrk_type3_8812f,
		DELTA_SWINGIDX_SIZE);
odm_move_memory(dm, cali_info->delta_swing_table_idx_2ga_n,
		(void *)delta_swingidx_mp_2ga_n_txpwrtrk_type3_8812f,
		DELTA_SWINGIDX_SIZE);
odm_move_memory(dm, cali_info->delta_swing_table_idx_2gb_p,
		(void *)delta_swingidx_mp_2gb_p_txpwrtrk_type3_8812f,
		DELTA_SWINGIDX_SIZE);
odm_move_memory(dm, cali_info->delta_swing_table_idx_2gb_n,
		(void *)delta_swingidx_mp_2gb_n_txpwrtrk_type3_8812f,
		DELTA_SWINGIDX_SIZE);

odm_move_memory(dm, cali_info->delta_swing_table_idx_2g_cck_a_p,
		(void *)delta_swingidx_mp_2g_cck_a_p_txpwrtrk_type3_8812f,
		DELTA_SWINGIDX_SIZE);
odm_move_memory(dm, cali_info->delta_swing_table_idx_2g_cck_a_n,
		(void *)delta_swingidx_mp_2g_cck_a_n_txpwrtrk_type3_8812f,
		DELTA_SWINGIDX_SIZE);
odm_move_memory(dm, cali_info->delta_swing_table_idx_2g_cck_b_p,
		(void *)delta_swingidx_mp_2g_cck_b_p_txpwrtrk_type3_8812f,
		DELTA_SWINGIDX_SIZE);
odm_move_memory(dm, cali_info->delta_swing_table_idx_2g_cck_b_n,
		(void *)delta_swingidx_mp_2g_cck_b_n_txpwrtrk_type3_8812f,
		DELTA_SWINGIDX_SIZE);

odm_move_memory(dm, cali_info->delta_swing_table_idx_5ga_p,
		(void *)delta_swingidx_mp_5ga_p_txpwrtrk_type3_8812f,
		DELTA_SWINGIDX_SIZE * 3);
odm_move_memory(dm, cali_info->delta_swing_table_idx_5ga_n,
		(void *)delta_swingidx_mp_5ga_n_txpwrtrk_type3_8812f,
		DELTA_SWINGIDX_SIZE * 3);
odm_move_memory(dm, cali_info->delta_swing_table_idx_5gb_p,
		(void *)delta_swingidx_mp_5gb_p_txpwrtrk_type3_8812f,
		DELTA_SWINGIDX_SIZE * 3);
odm_move_memory(dm, cali_info->delta_swing_table_idx_5gb_n,
		(void *)delta_swingidx_mp_5gb_n_txpwrtrk_type3_8812f,
		DELTA_SWINGIDX_SIZE * 3);
#endif
}

/******************************************************************************
 *                           txpwr_lmt.TXT
 ******************************************************************************/

#ifdef CONFIG_8812F
const char *array_mp_8812f_txpwr_lmt[] = {
	"FCC", "2.4G", "20M", "CCK", "1T", "01", "32",
	"ETSI", "2.4G", "20M", "CCK", "1T", "01", "28",
	"MKK", "2.4G", "20M", "CCK", "1T", "01", "30",
	"FCC", "2.4G", "20M", "CCK", "1T", "02", "32",
	"ETSI", "2.4G", "20M", "CCK", "1T", "02", "28",
	"MKK", "2.4G", "20M", "CCK", "1T", "02", "30",
	"FCC", "2.4G", "20M", "CCK", "1T", "03", "32",
	"ETSI", "2.4G", "20M", "CCK", "1T", "03", "28",
	"MKK", "2.4G", "20M", "CCK", "1T", "03", "30",
	"FCC", "2.4G", "20M", "CCK", "1T", "04", "32",
	"ETSI", "2.4G", "20M", "CCK", "1T", "04", "28",
	"MKK", "2.4G", "20M", "CCK", "1T", "04", "30",
	"FCC", "2.4G", "20M", "CCK", "1T", "05", "32",
	"ETSI", "2.4G", "20M", "CCK", "1T", "05", "28",
	"MKK", "2.4G", "20M", "CCK", "1T", "05", "30",
	"FCC", "2.4G", "20M", "CCK", "1T", "06", "32",
	"ETSI", "2.4G", "20M", "CCK", "1T", "06", "28",
	"MKK", "2.4G", "20M", "CCK", "1T", "06", "30",
	"FCC", "2.4G", "20M", "CCK", "1T", "07", "32",
	"ETSI", "2.4G", "20M", "CCK", "1T", "07", "28",
	"MKK", "2.4G", "20M", "CCK", "1T", "07", "30",
	"FCC", "2.4G", "20M", "CCK", "1T", "08", "32",
	"ETSI", "2.4G", "20M", "CCK", "1T", "08", "28",
	"MKK", "2.4G", "20M", "CCK", "1T", "08", "30",
	"FCC", "2.4G", "20M", "CCK", "1T", "09", "32",
	"ETSI", "2.4G", "20M", "CCK", "1T", "09", "28",
	"MKK", "2.4G", "20M", "CCK", "1T", "09", "30",
	"FCC", "2.4G", "20M", "CCK", "1T", "10", "32",
	"ETSI", "2.4G", "20M", "CCK", "1T", "10", "28",
	"MKK", "2.4G", "20M", "CCK", "1T", "10", "30",
	"FCC", "2.4G", "20M", "CCK", "1T", "11", "32",
	"ETSI", "2.4G", "20M", "CCK", "1T", "11", "28",
	"MKK", "2.4G", "20M", "CCK", "1T", "11", "30",
	"FCC", "2.4G", "20M", "CCK", "1T", "12", "26",
	"ETSI", "2.4G", "20M", "CCK", "1T", "12", "28",
	"MKK", "2.4G", "20M", "CCK", "1T", "12", "30",
	"FCC", "2.4G", "20M", "CCK", "1T", "13", "20",
	"ETSI", "2.4G", "20M", "CCK", "1T", "13", "28",
	"MKK", "2.4G", "20M", "CCK", "1T", "13", "28",
	"FCC", "2.4G", "20M", "CCK", "1T", "14", "63",
	"ETSI", "2.4G", "20M", "CCK", "1T", "14", "63",
	"MKK", "2.4G", "20M", "CCK", "1T", "14", "32",
	"FCC", "2.4G", "20M", "OFDM", "1T", "01", "26",
	"ETSI", "2.4G", "20M", "OFDM", "1T", "01", "30",
	"MKK", "2.4G", "20M", "OFDM", "1T", "01", "34",
	"FCC", "2.4G", "20M", "OFDM", "1T", "02", "30",
	"ETSI", "2.4G", "20M", "OFDM", "1T", "02", "30",
	"MKK", "2.4G", "20M", "OFDM", "1T", "02", "34",
	"FCC", "2.4G", "20M", "OFDM", "1T", "03", "32",
	"ETSI", "2.4G", "20M", "OFDM", "1T", "03", "30",
	"MKK", "2.4G", "20M", "OFDM", "1T", "03", "34",
	"FCC", "2.4G", "20M", "OFDM", "1T", "04", "34",
	"ETSI", "2.4G", "20M", "OFDM", "1T", "04", "30",
	"MKK", "2.4G", "20M", "OFDM", "1T", "04", "34",
	"FCC", "2.4G", "20M", "OFDM", "1T", "05", "34",
	"ETSI", "2.4G", "20M", "OFDM", "1T", "05", "30",
	"MKK", "2.4G", "20M", "OFDM", "1T", "05", "34",
	"FCC", "2.4G", "20M", "OFDM", "1T", "06", "34",
	"ETSI", "2.4G", "20M", "OFDM", "1T", "06", "30",
	"MKK", "2.4G", "20M", "OFDM", "1T", "06", "34",
	"FCC", "2.4G", "20M", "OFDM", "1T", "07", "34",
	"ETSI", "2.4G", "20M", "OFDM", "1T", "07", "30",
	"MKK", "2.4G", "20M", "OFDM", "1T", "07", "34",
	"FCC", "2.4G", "20M", "OFDM", "1T", "08", "34",
	"ETSI", "2.4G", "20M", "OFDM", "1T", "08", "30",
	"MKK", "2.4G", "20M", "OFDM", "1T", "08", "34",
	"FCC", "2.4G", "20M", "OFDM", "1T", "09", "32",
	"ETSI", "2.4G", "20M", "OFDM", "1T", "09", "30",
	"MKK", "2.4G", "20M", "OFDM", "1T", "09", "34",
	"FCC", "2.4G", "20M", "OFDM", "1T", "10", "30",
	"ETSI", "2.4G", "20M", "OFDM", "1T", "10", "30",
	"MKK", "2.4G", "20M", "OFDM", "1T", "10", "34",
	"FCC", "2.4G", "20M", "OFDM", "1T", "11", "28",
	"ETSI", "2.4G", "20M", "OFDM", "1T", "11", "30",
	"MKK", "2.4G", "20M", "OFDM", "1T", "11", "34",
	"FCC", "2.4G", "20M", "OFDM", "1T", "12", "22",
	"ETSI", "2.4G", "20M", "OFDM", "1T", "12", "30",
	"MKK", "2.4G", "20M", "OFDM", "1T", "12", "34",
	"FCC", "2.4G", "20M", "OFDM", "1T", "13", "14",
	"ETSI", "2.4G", "20M", "OFDM", "1T", "13", "30",
	"MKK", "2.4G", "20M", "OFDM", "1T", "13", "34",
	"FCC", "2.4G", "20M", "OFDM", "1T", "14", "63",
	"ETSI", "2.4G", "20M", "OFDM", "1T", "14", "63",
	"MKK", "2.4G", "20M", "OFDM", "1T", "14", "63",
	"FCC", "2.4G", "20M", "HT", "1T", "01", "26",
	"ETSI", "2.4G", "20M", "HT", "1T", "01", "30",
	"MKK", "2.4G", "20M", "HT", "1T", "01", "34",
	"FCC", "2.4G", "20M", "HT", "1T", "02", "30",
	"ETSI", "2.4G", "20M", "HT", "1T", "02", "30",
	"MKK", "2.4G", "20M", "HT", "1T", "02", "34",
	"FCC", "2.4G", "20M", "HT", "1T", "03", "32",
	"ETSI", "2.4G", "20M", "HT", "1T", "03", "30",
	"MKK", "2.4G", "20M", "HT", "1T", "03", "34",
	"FCC", "2.4G", "20M", "HT", "1T", "04", "34",
	"ETSI", "2.4G", "20M", "HT", "1T", "04", "30",
	"MKK", "2.4G", "20M", "HT", "1T", "04", "34",
	"FCC", "2.4G", "20M", "HT", "1T", "05", "34",
	"ETSI", "2.4G", "20M", "HT", "1T", "05", "30",
	"MKK", "2.4G", "20M", "HT", "1T", "05", "34",
	"FCC", "2.4G", "20M", "HT", "1T", "06", "34",
	"ETSI", "2.4G", "20M", "HT", "1T", "06", "30",
	"MKK", "2.4G", "20M", "HT", "1T", "06", "34",
	"FCC", "2.4G", "20M", "HT", "1T", "07", "34",
	"ETSI", "2.4G", "20M", "HT", "1T", "07", "30",
	"MKK", "2.4G", "20M", "HT", "1T", "07", "34",
	"FCC", "2.4G", "20M", "HT", "1T", "08", "34",
	"ETSI", "2.4G", "20M", "HT", "1T", "08", "30",
	"MKK", "2.4G", "20M", "HT", "1T", "08", "34",
	"FCC", "2.4G", "20M", "HT", "1T", "09", "32",
	"ETSI", "2.4G", "20M", "HT", "1T", "09", "30",
	"MKK", "2.4G", "20M", "HT", "1T", "09", "34",
	"FCC", "2.4G", "20M", "HT", "1T", "10", "30",
	"ETSI", "2.4G", "20M", "HT", "1T", "10", "30",
	"MKK", "2.4G", "20M", "HT", "1T", "10", "34",
	"FCC", "2.4G", "20M", "HT", "1T", "11", "26",
	"ETSI", "2.4G", "20M", "HT", "1T", "11", "30",
	"MKK", "2.4G", "20M", "HT", "1T", "11", "34",
	"FCC", "2.4G", "20M", "HT", "1T", "12", "20",
	"ETSI", "2.4G", "20M", "HT", "1T", "12", "30",
	"MKK", "2.4G", "20M", "HT", "1T", "12", "34",
	"FCC", "2.4G", "20M", "HT", "1T", "13", "14",
	"ETSI", "2.4G", "20M", "HT", "1T", "13", "30",
	"MKK", "2.4G", "20M", "HT", "1T", "13", "34",
	"FCC", "2.4G", "20M", "HT", "1T", "14", "63",
	"ETSI", "2.4G", "20M", "HT", "1T", "14", "63",
	"MKK", "2.4G", "20M", "HT", "1T", "14", "63",
	"FCC", "2.4G", "20M", "HT", "2T", "01", "26",
	"ETSI", "2.4G", "20M", "HT", "2T", "01", "18",
	"MKK", "2.4G", "20M", "HT", "2T", "01", "30",
	"FCC", "2.4G", "20M", "HT", "2T", "02", "28",
	"ETSI", "2.4G", "20M", "HT", "2T", "02", "18",
	"MKK", "2.4G", "20M", "HT", "2T", "02", "30",
	"FCC", "2.4G", "20M", "HT", "2T", "03", "30",
	"ETSI", "2.4G", "20M", "HT", "2T", "03", "18",
	"MKK", "2.4G", "20M", "HT", "2T", "03", "30",
	"FCC", "2.4G", "20M", "HT", "2T", "04", "30",
	"ETSI", "2.4G", "20M", "HT", "2T", "04", "18",
	"MKK", "2.4G", "20M", "HT", "2T", "04", "30",
	"FCC", "2.4G", "20M", "HT", "2T", "05", "32",
	"ETSI", "2.4G", "20M", "HT", "2T", "05", "18",
	"MKK", "2.4G", "20M", "HT", "2T", "05", "30",
	"FCC", "2.4G", "20M", "HT", "2T", "06", "32",
	"ETSI", "2.4G", "20M", "HT", "2T", "06", "18",
	"MKK", "2.4G", "20M", "HT", "2T", "06", "30",
	"FCC", "2.4G", "20M", "HT", "2T", "07", "32",
	"ETSI", "2.4G", "20M", "HT", "2T", "07", "18",
	"MKK", "2.4G", "20M", "HT", "2T", "07", "30",
	"FCC", "2.4G", "20M", "HT", "2T", "08", "30",
	"ETSI", "2.4G", "20M", "HT", "2T", "08", "18",
	"MKK", "2.4G", "20M", "HT", "2T", "08", "30",
	"FCC", "2.4G", "20M", "HT", "2T", "09", "30",
	"ETSI", "2.4G", "20M", "HT", "2T", "09", "18",
	"MKK", "2.4G", "20M", "HT", "2T", "09", "30",
	"FCC", "2.4G", "20M", "HT", "2T", "10", "28",
	"ETSI", "2.4G", "20M", "HT", "2T", "10", "18",
	"MKK", "2.4G", "20M", "HT", "2T", "10", "30",
	"FCC", "2.4G", "20M", "HT", "2T", "11", "26",
	"ETSI", "2.4G", "20M", "HT", "2T", "11", "18",
	"MKK", "2.4G", "20M", "HT", "2T", "11", "30",
	"FCC", "2.4G", "20M", "HT", "2T", "12", "20",
	"ETSI", "2.4G", "20M", "HT", "2T", "12", "18",
	"MKK", "2.4G", "20M", "HT", "2T", "12", "30",
	"FCC", "2.4G", "20M", "HT", "2T", "13", "14",
	"ETSI", "2.4G", "20M", "HT", "2T", "13", "18",
	"MKK", "2.4G", "20M", "HT", "2T", "13", "30",
	"FCC", "2.4G", "20M", "HT", "2T", "14", "63",
	"ETSI", "2.4G", "20M", "HT", "2T", "14", "63",
	"MKK", "2.4G", "20M", "HT", "2T", "14", "63",
	"FCC", "2.4G", "40M", "HT", "1T", "01", "63",
	"ETSI", "2.4G", "40M", "HT", "1T", "01", "63",
	"MKK", "2.4G", "40M", "HT", "1T", "01", "63",
	"FCC", "2.4G", "40M", "HT", "1T", "02", "63",
	"ETSI", "2.4G", "40M", "HT", "1T", "02", "63",
	"MKK", "2.4G", "40M", "HT", "1T", "02", "63",
	"FCC", "2.4G", "40M", "HT", "1T", "03", "26",
	"ETSI", "2.4G", "40M", "HT", "1T", "03", "30",
	"MKK", "2.4G", "40M", "HT", "1T", "03", "34",
	"FCC", "2.4G", "40M", "HT", "1T", "04", "26",
	"ETSI", "2.4G", "40M", "HT", "1T", "04", "30",
	"MKK", "2.4G", "40M", "HT", "1T", "04", "34",
	"FCC", "2.4G", "40M", "HT", "1T", "05", "30",
	"ETSI", "2.4G", "40M", "HT", "1T", "05", "30",
	"MKK", "2.4G", "40M", "HT", "1T", "05", "34",
	"FCC", "2.4G", "40M", "HT", "1T", "06", "32",
	"ETSI", "2.4G", "40M", "HT", "1T", "06", "30",
	"MKK", "2.4G", "40M", "HT", "1T", "06", "34",
	"FCC", "2.4G", "40M", "HT", "1T", "07", "30",
	"ETSI", "2.4G", "40M", "HT", "1T", "07", "30",
	"MKK", "2.4G", "40M", "HT", "1T", "07", "34",
	"FCC", "2.4G", "40M", "HT", "1T", "08", "26",
	"ETSI", "2.4G", "40M", "HT", "1T", "08", "30",
	"MKK", "2.4G", "40M", "HT", "1T", "08", "34",
	"FCC", "2.4G", "40M", "HT", "1T", "09", "26",
	"ETSI", "2.4G", "40M", "HT", "1T", "09", "30",
	"MKK", "2.4G", "40M", "HT", "1T", "09", "34",
	"FCC", "2.4G", "40M", "HT", "1T", "10", "20",
	"ETSI", "2.4G", "40M", "HT", "1T", "10", "30",
	"MKK", "2.4G", "40M", "HT", "1T", "10", "34",
	"FCC", "2.4G", "40M", "HT", "1T", "11", "14",
	"ETSI", "2.4G", "40M", "HT", "1T", "11", "30",
	"MKK", "2.4G", "40M", "HT", "1T", "11", "34",
	"FCC", "2.4G", "40M", "HT", "1T", "12", "63",
	"ETSI", "2.4G", "40M", "HT", "1T", "12", "63",
	"MKK", "2.4G", "40M", "HT", "1T", "12", "63",
	"FCC", "2.4G", "40M", "HT", "1T", "13", "63",
	"ETSI", "2.4G", "40M", "HT", "1T", "13", "63",
	"MKK", "2.4G", "40M", "HT", "1T", "13", "63",
	"FCC", "2.4G", "40M", "HT", "1T", "14", "63",
	"ETSI", "2.4G", "40M", "HT", "1T", "14", "63",
	"MKK", "2.4G", "40M", "HT", "1T", "14", "63",
	"FCC", "2.4G", "40M", "HT", "2T", "01", "63",
	"ETSI", "2.4G", "40M", "HT", "2T", "01", "63",
	"MKK", "2.4G", "40M", "HT", "2T", "01", "63",
	"FCC", "2.4G", "40M", "HT", "2T", "02", "63",
	"ETSI", "2.4G", "40M", "HT", "2T", "02", "63",
	"MKK", "2.4G", "40M", "HT", "2T", "02", "63",
	"FCC", "2.4G", "40M", "HT", "2T", "03", "24",
	"ETSI", "2.4G", "40M", "HT", "2T", "03", "18",
	"MKK", "2.4G", "40M", "HT", "2T", "03", "30",
	"FCC", "2.4G", "40M", "HT", "2T", "04", "24",
	"ETSI", "2.4G", "40M", "HT", "2T", "04", "18",
	"MKK", "2.4G", "40M", "HT", "2T", "04", "30",
	"FCC", "2.4G", "40M", "HT", "2T", "05", "26",
	"ETSI", "2.4G", "40M", "HT", "2T", "05", "18",
	"MKK", "2.4G", "40M", "HT", "2T", "05", "30",
	"FCC", "2.4G", "40M", "HT", "2T", "06", "28",
	"ETSI", "2.4G", "40M", "HT", "2T", "06", "18",
	"MKK", "2.4G", "40M", "HT", "2T", "06", "30",
	"FCC", "2.4G", "40M", "HT", "2T", "07", "26",
	"ETSI", "2.4G", "40M", "HT", "2T", "07", "18",
	"MKK", "2.4G", "40M", "HT", "2T", "07", "30",
	"FCC", "2.4G", "40M", "HT", "2T", "08", "26",
	"ETSI", "2.4G", "40M", "HT", "2T", "08", "18",
	"MKK", "2.4G", "40M", "HT", "2T", "08", "30",
	"FCC", "2.4G", "40M", "HT", "2T", "09", "26",
	"ETSI", "2.4G", "40M", "HT", "2T", "09", "18",
	"MKK", "2.4G", "40M", "HT", "2T", "09", "30",
	"FCC", "2.4G", "40M", "HT", "2T", "10", "20",
	"ETSI", "2.4G", "40M", "HT", "2T", "10", "18",
	"MKK", "2.4G", "40M", "HT", "2T", "10", "30",
	"FCC", "2.4G", "40M", "HT", "2T", "11", "14",
	"ETSI", "2.4G", "40M", "HT", "2T", "11", "18",
	"MKK", "2.4G", "40M", "HT", "2T", "11", "30",
	"FCC", "2.4G", "40M", "HT", "2T", "12", "63",
	"ETSI", "2.4G", "40M", "HT", "2T", "12", "63",
	"MKK", "2.4G", "40M", "HT", "2T", "12", "63",
	"FCC", "2.4G", "40M", "HT", "2T", "13", "63",
	"ETSI", "2.4G", "40M", "HT", "2T", "13", "63",
	"MKK", "2.4G", "40M", "HT", "2T", "13", "63",
	"FCC", "2.4G", "40M", "HT", "2T", "14", "63",
	"ETSI", "2.4G", "40M", "HT", "2T", "14", "63",
	"MKK", "2.4G", "40M", "HT", "2T", "14", "63",
	"FCC", "5G", "20M", "OFDM", "1T", "36", "30",
	"ETSI", "5G", "20M", "OFDM", "1T", "36", "32",
	"MKK", "5G", "20M", "OFDM", "1T", "36", "30",
	"FCC", "5G", "20M", "OFDM", "1T", "40", "32",
	"ETSI", "5G", "20M", "OFDM", "1T", "40", "32",
	"MKK", "5G", "20M", "OFDM", "1T", "40", "30",
	"FCC", "5G", "20M", "OFDM", "1T", "44", "32",
	"ETSI", "5G", "20M", "OFDM", "1T", "44", "32",
	"MKK", "5G", "20M", "OFDM", "1T", "44", "30",
	"FCC", "5G", "20M", "OFDM", "1T", "48", "32",
	"ETSI", "5G", "20M", "OFDM", "1T", "48", "32",
	"MKK", "5G", "20M", "OFDM", "1T", "48", "30",
	"FCC", "5G", "20M", "OFDM", "1T", "52", "32",
	"ETSI", "5G", "20M", "OFDM", "1T", "52", "32",
	"MKK", "5G", "20M", "OFDM", "1T", "52", "28",
	"FCC", "5G", "20M", "OFDM", "1T", "56", "32",
	"ETSI", "5G", "20M", "OFDM", "1T", "56", "32",
	"MKK", "5G", "20M", "OFDM", "1T", "56", "28",
	"FCC", "5G", "20M", "OFDM", "1T", "60", "32",
	"ETSI", "5G", "20M", "OFDM", "1T", "60", "32",
	"MKK", "5G", "20M", "OFDM", "1T", "60", "28",
	"FCC", "5G", "20M", "OFDM", "1T", "64", "28",
	"ETSI", "5G", "20M", "OFDM", "1T", "64", "32",
	"MKK", "5G", "20M", "OFDM", "1T", "64", "28",
	"FCC", "5G", "20M", "OFDM", "1T", "100", "26",
	"ETSI", "5G", "20M", "OFDM", "1T", "100", "32",
	"MKK", "5G", "20M", "OFDM", "1T", "100", "32",
	"FCC", "5G", "20M", "OFDM", "1T", "104", "32",
	"ETSI", "5G", "20M", "OFDM", "1T", "104", "32",
	"MKK", "5G", "20M", "OFDM", "1T", "104", "32",
	"FCC", "5G", "20M", "OFDM", "1T", "108", "32",
	"ETSI", "5G", "20M", "OFDM", "1T", "108", "32",
	"MKK", "5G", "20M", "OFDM", "1T", "108", "32",
	"FCC", "5G", "20M", "OFDM", "1T", "112", "32",
	"ETSI", "5G", "20M", "OFDM", "1T", "112", "32",
	"MKK", "5G", "20M", "OFDM", "1T", "112", "32",
	"FCC", "5G", "20M", "OFDM", "1T", "116", "32",
	"ETSI", "5G", "20M", "OFDM", "1T", "116", "32",
	"MKK", "5G", "20M", "OFDM", "1T", "116", "32",
	"FCC", "5G", "20M", "OFDM", "1T", "120", "32",
	"ETSI", "5G", "20M", "OFDM", "1T", "120", "32",
	"MKK", "5G", "20M", "OFDM", "1T", "120", "32",
	"FCC", "5G", "20M", "OFDM", "1T", "124", "32",
	"ETSI", "5G", "20M", "OFDM", "1T", "124", "32",
	"MKK", "5G", "20M", "OFDM", "1T", "124", "32",
	"FCC", "5G", "20M", "OFDM", "1T", "128", "32",
	"ETSI", "5G", "20M", "OFDM", "1T", "128", "32",
	"MKK", "5G", "20M", "OFDM", "1T", "128", "32",
	"FCC", "5G", "20M", "OFDM", "1T", "132", "32",
	"ETSI", "5G", "20M", "OFDM", "1T", "132", "32",
	"MKK", "5G", "20M", "OFDM", "1T", "132", "32",
	"FCC", "5G", "20M", "OFDM", "1T", "136", "32",
	"ETSI", "5G", "20M", "OFDM", "1T", "136", "32",
	"MKK", "5G", "20M", "OFDM", "1T", "136", "32",
	"FCC", "5G", "20M", "OFDM", "1T", "140", "28",
	"ETSI", "5G", "20M", "OFDM", "1T", "140", "32",
	"MKK", "5G", "20M", "OFDM", "1T", "140", "32",
	"FCC", "5G", "20M", "OFDM", "1T", "144", "28",
	"ETSI", "5G", "20M", "OFDM", "1T", "144", "32",
	"MKK", "5G", "20M", "OFDM", "1T", "144", "63",
	"FCC", "5G", "20M", "OFDM", "1T", "149", "32",
	"ETSI", "5G", "20M", "OFDM", "1T", "149", "63",
	"MKK", "5G", "20M", "OFDM", "1T", "149", "63",
	"FCC", "5G", "20M", "OFDM", "1T", "153", "32",
	"ETSI", "5G", "20M", "OFDM", "1T", "153", "63",
	"MKK", "5G", "20M", "OFDM", "1T", "153", "63",
	"FCC", "5G", "20M", "OFDM", "1T", "157", "32",
	"ETSI", "5G", "20M", "OFDM", "1T", "157", "63",
	"MKK", "5G", "20M", "OFDM", "1T", "157", "63",
	"FCC", "5G", "20M", "OFDM", "1T", "161", "32",
	"ETSI", "5G", "20M", "OFDM", "1T", "161", "63",
	"MKK", "5G", "20M", "OFDM", "1T", "161", "63",
	"FCC", "5G", "20M", "OFDM", "1T", "165", "32",
	"ETSI", "5G", "20M", "OFDM", "1T", "165", "63",
	"MKK", "5G", "20M", "OFDM", "1T", "165", "63",
	"FCC", "5G", "20M", "HT", "1T", "36", "30",
	"ETSI", "5G", "20M", "HT", "1T", "36", "32",
	"MKK", "5G", "20M", "HT", "1T", "36", "28",
	"FCC", "5G", "20M", "HT", "1T", "40", "32",
	"ETSI", "5G", "20M", "HT", "1T", "40", "32",
	"MKK", "5G", "20M", "HT", "1T", "40", "28",
	"FCC", "5G", "20M", "HT", "1T", "44", "32",
	"ETSI", "5G", "20M", "HT", "1T", "44", "32",
	"MKK", "5G", "20M", "HT", "1T", "44", "28",
	"FCC", "5G", "20M", "HT", "1T", "48", "32",
	"ETSI", "5G", "20M", "HT", "1T", "48", "32",
	"MKK", "5G", "20M", "HT", "1T", "48", "28",
	"FCC", "5G", "20M", "HT", "1T", "52", "32",
	"ETSI", "5G", "20M", "HT", "1T", "52", "32",
	"MKK", "5G", "20M", "HT", "1T", "52", "28",
	"FCC", "5G", "20M", "HT", "1T", "56", "32",
	"ETSI", "5G", "20M", "HT", "1T", "56", "32",
	"MKK", "5G", "20M", "HT", "1T", "56", "28",
	"FCC", "5G", "20M", "HT", "1T", "60", "32",
	"ETSI", "5G", "20M", "HT", "1T", "60", "32",
	"MKK", "5G", "20M", "HT", "1T", "60", "28",
	"FCC", "5G", "20M", "HT", "1T", "64", "28",
	"ETSI", "5G", "20M", "HT", "1T", "64", "32",
	"MKK", "5G", "20M", "HT", "1T", "64", "28",
	"FCC", "5G", "20M", "HT", "1T", "100", "26",
	"ETSI", "5G", "20M", "HT", "1T", "100", "32",
	"MKK", "5G", "20M", "HT", "1T", "100", "32",
	"FCC", "5G", "20M", "HT", "1T", "104", "32",
	"ETSI", "5G", "20M", "HT", "1T", "104", "32",
	"MKK", "5G", "20M", "HT", "1T", "104", "32",
	"FCC", "5G", "20M", "HT", "1T", "108", "32",
	"ETSI", "5G", "20M", "HT", "1T", "108", "32",
	"MKK", "5G", "20M", "HT", "1T", "108", "32",
	"FCC", "5G", "20M", "HT", "1T", "112", "32",
	"ETSI", "5G", "20M", "HT", "1T", "112", "32",
	"MKK", "5G", "20M", "HT", "1T", "112", "32",
	"FCC", "5G", "20M", "HT", "1T", "116", "32",
	"ETSI", "5G", "20M", "HT", "1T", "116", "32",
	"MKK", "5G", "20M", "HT", "1T", "116", "32",
	"FCC", "5G", "20M", "HT", "1T", "120", "32",
	"ETSI", "5G", "20M", "HT", "1T", "120", "32",
	"MKK", "5G", "20M", "HT", "1T", "120", "32",
	"FCC", "5G", "20M", "HT", "1T", "124", "32",
	"ETSI", "5G", "20M", "HT", "1T", "124", "32",
	"MKK", "5G", "20M", "HT", "1T", "124", "32",
	"FCC", "5G", "20M", "HT", "1T", "128", "32",
	"ETSI", "5G", "20M", "HT", "1T", "128", "32",
	"MKK", "5G", "20M", "HT", "1T", "128", "32",
	"FCC", "5G", "20M", "HT", "1T", "132", "32",
	"ETSI", "5G", "20M", "HT", "1T", "132", "32",
	"MKK", "5G", "20M", "HT", "1T", "132", "32",
	"FCC", "5G", "20M", "HT", "1T", "136", "32",
	"ETSI", "5G", "20M", "HT", "1T", "136", "32",
	"MKK", "5G", "20M", "HT", "1T", "136", "32",
	"FCC", "5G", "20M", "HT", "1T", "140", "26",
	"ETSI", "5G", "20M", "HT", "1T", "140", "32",
	"MKK", "5G", "20M", "HT", "1T", "140", "32",
	"FCC", "5G", "20M", "HT", "1T", "144", "26",
	"ETSI", "5G", "20M", "HT", "1T", "144", "63",
	"MKK", "5G", "20M", "HT", "1T", "144", "63",
	"FCC", "5G", "20M", "HT", "1T", "149", "32",
	"ETSI", "5G", "20M", "HT", "1T", "149", "63",
	"MKK", "5G", "20M", "HT", "1T", "149", "63",
	"FCC", "5G", "20M", "HT", "1T", "153", "32",
	"ETSI", "5G", "20M", "HT", "1T", "153", "63",
	"MKK", "5G", "20M", "HT", "1T", "153", "63",
	"FCC", "5G", "20M", "HT", "1T", "157", "32",
	"ETSI", "5G", "20M", "HT", "1T", "157", "63",
	"MKK", "5G", "20M", "HT", "1T", "157", "63",
	"FCC", "5G", "20M", "HT", "1T", "161", "32",
	"ETSI", "5G", "20M", "HT", "1T", "161", "63",
	"MKK", "5G", "20M", "HT", "1T", "161", "63",
	"FCC", "5G", "20M", "HT", "1T", "165", "32",
	"ETSI", "5G", "20M", "HT", "1T", "165", "63",
	"MKK", "5G", "20M", "HT", "1T", "165", "63",
	"FCC", "5G", "20M", "HT", "2T", "36", "28",
	"ETSI", "5G", "20M", "HT", "2T", "36", "20",
	"MKK", "5G", "20M", "HT", "2T", "36", "22",
	"FCC", "5G", "20M", "HT", "2T", "40", "30",
	"ETSI", "5G", "20M", "HT", "2T", "40", "20",
	"MKK", "5G", "20M", "HT", "2T", "40", "22",
	"FCC", "5G", "20M", "HT", "2T", "44", "30",
	"ETSI", "5G", "20M", "HT", "2T", "44", "20",
	"MKK", "5G", "20M", "HT", "2T", "44", "22",
	"FCC", "5G", "20M", "HT", "2T", "48", "30",
	"ETSI", "5G", "20M", "HT", "2T", "48", "20",
	"MKK", "5G", "20M", "HT", "2T", "48", "22",
	"FCC", "5G", "20M", "HT", "2T", "52", "30",
	"ETSI", "5G", "20M", "HT", "2T", "52", "20",
	"MKK", "5G", "20M", "HT", "2T", "52", "22",
	"FCC", "5G", "20M", "HT", "2T", "56", "30",
	"ETSI", "5G", "20M", "HT", "2T", "56", "20",
	"MKK", "5G", "20M", "HT", "2T", "56", "22",
	"FCC", "5G", "20M", "HT", "2T", "60", "30",
	"ETSI", "5G", "20M", "HT", "2T", "60", "20",
	"MKK", "5G", "20M", "HT", "2T", "60", "22",
	"FCC", "5G", "20M", "HT", "2T", "64", "28",
	"ETSI", "5G", "20M", "HT", "2T", "64", "20",
	"MKK", "5G", "20M", "HT", "2T", "64", "22",
	"FCC", "5G", "20M", "HT", "2T", "100", "26",
	"ETSI", "5G", "20M", "HT", "2T", "100", "20",
	"MKK", "5G", "20M", "HT", "2T", "100", "30",
	"FCC", "5G", "20M", "HT", "2T", "104", "30",
	"ETSI", "5G", "20M", "HT", "2T", "104", "20",
	"MKK", "5G", "20M", "HT", "2T", "104", "30",
	"FCC", "5G", "20M", "HT", "2T", "108", "32",
	"ETSI", "5G", "20M", "HT", "2T", "108", "20",
	"MKK", "5G", "20M", "HT", "2T", "108", "30",
	"FCC", "5G", "20M", "HT", "2T", "112", "32",
	"ETSI", "5G", "20M", "HT", "2T", "112", "20",
	"MKK", "5G", "20M", "HT", "2T", "112", "30",
	"FCC", "5G", "20M", "HT", "2T", "116", "32",
	"ETSI", "5G", "20M", "HT", "2T", "116", "20",
	"MKK", "5G", "20M", "HT", "2T", "116", "30",
	"FCC", "5G", "20M", "HT", "2T", "120", "32",
	"ETSI", "5G", "20M", "HT", "2T", "120", "20",
	"MKK", "5G", "20M", "HT", "2T", "120", "30",
	"FCC", "5G", "20M", "HT", "2T", "124", "32",
	"ETSI", "5G", "20M", "HT", "2T", "124", "20",
	"MKK", "5G", "20M", "HT", "2T", "124", "30",
	"FCC", "5G", "20M", "HT", "2T", "128", "32",
	"ETSI", "5G", "20M", "HT", "2T", "128", "20",
	"MKK", "5G", "20M", "HT", "2T", "128", "30",
	"FCC", "5G", "20M", "HT", "2T", "132", "32",
	"ETSI", "5G", "20M", "HT", "2T", "132", "20",
	"MKK", "5G", "20M", "HT", "2T", "132", "30",
	"FCC", "5G", "20M", "HT", "2T", "136", "30",
	"ETSI", "5G", "20M", "HT", "2T", "136", "20",
	"MKK", "5G", "20M", "HT", "2T", "136", "30",
	"FCC", "5G", "20M", "HT", "2T", "140", "26",
	"ETSI", "5G", "20M", "HT", "2T", "140", "20",
	"MKK", "5G", "20M", "HT", "2T", "140", "30",
	"FCC", "5G", "20M", "HT", "2T", "144", "26",
	"ETSI", "5G", "20M", "HT", "2T", "144", "63",
	"MKK", "5G", "20M", "HT", "2T", "144", "63",
	"FCC", "5G", "20M", "HT", "2T", "149", "32",
	"ETSI", "5G", "20M", "HT", "2T", "149", "63",
	"MKK", "5G", "20M", "HT", "2T", "149", "63",
	"FCC", "5G", "20M", "HT", "2T", "153", "32",
	"ETSI", "5G", "20M", "HT", "2T", "153", "63",
	"MKK", "5G", "20M", "HT", "2T", "153", "63",
	"FCC", "5G", "20M", "HT", "2T", "157", "32",
	"ETSI", "5G", "20M", "HT", "2T", "157", "63",
	"MKK", "5G", "20M", "HT", "2T", "157", "63",
	"FCC", "5G", "20M", "HT", "2T", "161", "32",
	"ETSI", "5G", "20M", "HT", "2T", "161", "63",
	"MKK", "5G", "20M", "HT", "2T", "161", "63",
	"FCC", "5G", "20M", "HT", "2T", "165", "32",
	"ETSI", "5G", "20M", "HT", "2T", "165", "63",
	"MKK", "5G", "20M", "HT", "2T", "165", "63",
	"FCC", "5G", "40M", "HT", "1T", "38", "22",
	"ETSI", "5G", "40M", "HT", "1T", "38", "30",
	"MKK", "5G", "40M", "HT", "1T", "38", "30",
	"FCC", "5G", "40M", "HT", "1T", "46", "30",
	"ETSI", "5G", "40M", "HT", "1T", "46", "30",
	"MKK", "5G", "40M", "HT", "1T", "46", "30",
	"FCC", "5G", "40M", "HT", "1T", "54", "30",
	"ETSI", "5G", "40M", "HT", "1T", "54", "30",
	"MKK", "5G", "40M", "HT", "1T", "54", "30",
	"FCC", "5G", "40M", "HT", "1T", "62", "24",
	"ETSI", "5G", "40M", "HT", "1T", "62", "30",
	"MKK", "5G", "40M", "HT", "1T", "62", "30",
	"FCC", "5G", "40M", "HT", "1T", "102", "24",
	"ETSI", "5G", "40M", "HT", "1T", "102", "30",
	"MKK", "5G", "40M", "HT", "1T", "102", "30",
	"FCC", "5G", "40M", "HT", "1T", "110", "30",
	"ETSI", "5G", "40M", "HT", "1T", "110", "30",
	"MKK", "5G", "40M", "HT", "1T", "110", "30",
	"FCC", "5G", "40M", "HT", "1T", "118", "30",
	"ETSI", "5G", "40M", "HT", "1T", "118", "30",
	"MKK", "5G", "40M", "HT", "1T", "118", "30",
	"FCC", "5G", "40M", "HT", "1T", "126", "30",
	"ETSI", "5G", "40M", "HT", "1T", "126", "30",
	"MKK", "5G", "40M", "HT", "1T", "126", "30",
	"FCC", "5G", "40M", "HT", "1T", "134", "30",
	"ETSI", "5G", "40M", "HT", "1T", "134", "30",
	"MKK", "5G", "40M", "HT", "1T", "134", "30",
	"FCC", "5G", "40M", "HT", "1T", "142", "30",
	"ETSI", "5G", "40M", "HT", "1T", "142", "63",
	"MKK", "5G", "40M", "HT", "1T", "142", "63",
	"FCC", "5G", "40M", "HT", "1T", "151", "30",
	"ETSI", "5G", "40M", "HT", "1T", "151", "63",
	"MKK", "5G", "40M", "HT", "1T", "151", "63",
	"FCC", "5G", "40M", "HT", "1T", "159", "30",
	"ETSI", "5G", "40M", "HT", "1T", "159", "63",
	"MKK", "5G", "40M", "HT", "1T", "159", "63",
	"FCC", "5G", "40M", "HT", "2T", "38", "20",
	"ETSI", "5G", "40M", "HT", "2T", "38", "20",
	"MKK", "5G", "40M", "HT", "2T", "38", "22",
	"FCC", "5G", "40M", "HT", "2T", "46", "30",
	"ETSI", "5G", "40M", "HT", "2T", "46", "20",
	"MKK", "5G", "40M", "HT", "2T", "46", "22",
	"FCC", "5G", "40M", "HT", "2T", "54", "30",
	"ETSI", "5G", "40M", "HT", "2T", "54", "20",
	"MKK", "5G", "40M", "HT", "2T", "54", "22",
	"FCC", "5G", "40M", "HT", "2T", "62", "22",
	"ETSI", "5G", "40M", "HT", "2T", "62", "20",
	"MKK", "5G", "40M", "HT", "2T", "62", "22",
	"FCC", "5G", "40M", "HT", "2T", "102", "22",
	"ETSI", "5G", "40M", "HT", "2T", "102", "20",
	"MKK", "5G", "40M", "HT", "2T", "102", "30",
	"FCC", "5G", "40M", "HT", "2T", "110", "30",
	"ETSI", "5G", "40M", "HT", "2T", "110", "20",
	"MKK", "5G", "40M", "HT", "2T", "110", "30",
	"FCC", "5G", "40M", "HT", "2T", "118", "30",
	"ETSI", "5G", "40M", "HT", "2T", "118", "20",
	"MKK", "5G", "40M", "HT", "2T", "118", "30",
	"FCC", "5G", "40M", "HT", "2T", "126", "30",
	"ETSI", "5G", "40M", "HT", "2T", "126", "20",
	"MKK", "5G", "40M", "HT", "2T", "126", "30",
	"FCC", "5G", "40M", "HT", "2T", "134", "30",
	"ETSI", "5G", "40M", "HT", "2T", "134", "20",
	"MKK", "5G", "40M", "HT", "2T", "134", "30",
	"FCC", "5G", "40M", "HT", "2T", "142", "30",
	"ETSI", "5G", "40M", "HT", "2T", "142", "63",
	"MKK", "5G", "40M", "HT", "2T", "142", "63",
	"FCC", "5G", "40M", "HT", "2T", "151", "30",
	"ETSI", "5G", "40M", "HT", "2T", "151", "63",
	"MKK", "5G", "40M", "HT", "2T", "151", "63",
	"FCC", "5G", "40M", "HT", "2T", "159", "30",
	"ETSI", "5G", "40M", "HT", "2T", "159", "63",
	"MKK", "5G", "40M", "HT", "2T", "159", "63",
	"FCC", "5G", "80M", "VHT", "1T", "42", "20",
	"ETSI", "5G", "80M", "VHT", "1T", "42", "30",
	"MKK", "5G", "80M", "VHT", "1T", "42", "28",
	"FCC", "5G", "80M", "VHT", "1T", "58", "20",
	"ETSI", "5G", "80M", "VHT", "1T", "58", "30",
	"MKK", "5G", "80M", "VHT", "1T", "58", "28",
	"FCC", "5G", "80M", "VHT", "1T", "106", "20",
	"ETSI", "5G", "80M", "VHT", "1T", "106", "30",
	"MKK", "5G", "80M", "VHT", "1T", "106", "30",
	"FCC", "5G", "80M", "VHT", "1T", "122", "30",
	"ETSI", "5G", "80M", "VHT", "1T", "122", "30",
	"MKK", "5G", "80M", "VHT", "1T", "122", "30",
	"FCC", "5G", "80M", "VHT", "1T", "138", "30",
	"ETSI", "5G", "80M", "VHT", "1T", "138", "63",
	"MKK", "5G", "80M", "VHT", "1T", "138", "63",
	"FCC", "5G", "80M", "VHT", "1T", "155", "30",
	"ETSI", "5G", "80M", "VHT", "1T", "155", "63",
	"MKK", "5G", "80M", "VHT", "1T", "155", "63",
	"FCC", "5G", "80M", "VHT", "2T", "42", "18",
	"ETSI", "5G", "80M", "VHT", "2T", "42", "20",
	"MKK", "5G", "80M", "VHT", "2T", "42", "22",
	"FCC", "5G", "80M", "VHT", "2T", "58", "18",
	"ETSI", "5G", "80M", "VHT", "2T", "58", "20",
	"MKK", "5G", "80M", "VHT", "2T", "58", "22",
	"FCC", "5G", "80M", "VHT", "2T", "106", "20",
	"ETSI", "5G", "80M", "VHT", "2T", "106", "20",
	"MKK", "5G", "80M", "VHT", "2T", "106", "30",
	"FCC", "5G", "80M", "VHT", "2T", "122", "30",
	"ETSI", "5G", "80M", "VHT", "2T", "122", "20",
	"MKK", "5G", "80M", "VHT", "2T", "122", "30",
	"FCC", "5G", "80M", "VHT", "2T", "138", "30",
	"ETSI", "5G", "80M", "VHT", "2T", "138", "63",
	"MKK", "5G", "80M", "VHT", "2T", "138", "63",
	"FCC", "5G", "80M", "VHT", "2T", "155", "30",
	"ETSI", "5G", "80M", "VHT", "2T", "155", "63",
	"MKK", "5G", "80M", "VHT", "2T", "155", "63"
};
#endif

void
odm_read_and_config_mp_8812f_txpwr_lmt(struct dm_struct *dm)
{
#ifdef CONFIG_8812F

	u32	i = 0;
#if (DM_ODM_SUPPORT_TYPE == ODM_IOT)
	u32	array_len =
			sizeof(array_mp_8812f_txpwr_lmt) / sizeof(u8);
	u8	*array = (u8 *)array_mp_8812f_txpwr_lmt;
#else
	u32	array_len =
			sizeof(array_mp_8812f_txpwr_lmt) / sizeof(u8 *);
	u8	**array = (u8 **)array_mp_8812f_txpwr_lmt;
#endif

#if (DM_ODM_SUPPORT_TYPE == ODM_WIN)
	void	*adapter = dm->adapter;
	HAL_DATA_TYPE	*hal_data = GET_HAL_DATA(((PADAPTER)adapter));

	odm_memory_set(dm, hal_data->BufOfLinesPwrLmt, 0,
		       MAX_LINES_HWCONFIG_TXT *
		       MAX_BYTES_LINE_HWCONFIG_TXT);
	hal_data->nLinesReadPwrLmt = array_len / 7;
#endif

	PHYDM_DBG(dm, ODM_COMP_INIT, "===> %s\n", __func__);

	for (i = 0; i < array_len; i += 7) {
#if (DM_ODM_SUPPORT_TYPE == ODM_IOT)
		u8	regulation = array[i];
		u8	band = array[i + 1];
		u8	bandwidth = array[i + 2];
		u8	rate = array[i + 3];
		u8	rf_path = array[i + 4];
		u8	chnl = array[i + 5];
		u8	val = array[i + 6];
#else
		u8	*regulation = array[i];
		u8	*band = array[i + 1];
		u8	*bandwidth = array[i + 2];
		u8	*rate = array[i + 3];
		u8	*rf_path = array[i + 4];
		u8	*chnl = array[i + 5];
		u8	*val = array[i + 6];
#endif

		odm_config_bb_txpwr_lmt_8812f(dm, regulation, band, bandwidth,
					      rate, rf_path, chnl, val);
#if (DM_ODM_SUPPORT_TYPE == ODM_WIN)
		rsprintf((char *)hal_data->BufOfLinesPwrLmt[i / 7], 100, "\"%s\", \"%s\", \"%s\", \"%s\", \"%s\", \"%s\", \"%s\",",
			 regulation, band, bandwidth, rate, rf_path, chnl, val);
#endif
	}

#endif
}

#endif /* end of HWIMG_SUPPORT*/

