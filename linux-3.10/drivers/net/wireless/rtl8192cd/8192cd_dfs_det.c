/*
 *  Handling routines for DFS DET functions
 *
 *  Copyright (c) 2017 Realtek Semiconductor Corp.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 2 as
 *  published by the Free Software Foundation.
 */

#define _8192CD_DFS_DET_C_

#include "./8192cd_cfg.h"
#include "./8192cd.h"
#include "./8192cd_headers.h"
#include "./8192cd_debug.h"


#if defined(DFS) && !defined(CONFIG_RTL_92D_SUPPORT)
extern u2Byte db_invert_table[8][12];

static u4Byte 
ConvertTo_dB(
	u4Byte 	Value)
{
	u1Byte i;
	u1Byte j;
	u4Byte dB;

	Value = Value & 0xFFFF;
	
	for (i=0;i<8;i++)
	{
		if (Value <= db_invert_table[i][11])
		{
			break;
		}
	}

	if (i >= 8)
	{
		return (96);	// maximum 96 dB
	}

	for (j=0;j<12;j++)
	{
		if (Value <= db_invert_table[i][j])
		{
			break;
		}
	}

	dB = i*12 + j + 1;

	return (dB);
}


static int
GetPSDData_8812(
	IN struct dm_struct*	p_dm_odm,
	unsigned int 	point,
	u1Byte initial_gain_psd)
{
	int	psd_report;
	struct rtl8192cd_priv *priv=p_dm_odm->priv;
	
	//Set DCO frequency index, offset=(40MHz/SamplePts)*point
	odm_set_bb_reg(p_dm_odm, 0x910, 0x3FF, point);

	//Start PSD calculation, Reg808[22]=0->1
	odm_set_bb_reg(p_dm_odm, 0x910, BIT22, 1);

	//Need to wait for HW PSD report
	delay_us(priv->pshare->rf_ft_var.dfs_psd_delay);

	odm_set_bb_reg(p_dm_odm, 0x910, BIT22, 0);

	//Read PSD report, Reg8B4[15:0]
	psd_report = (int)odm_get_bb_reg(p_dm_odm,0xf44, bMaskDWord) & 0x0000FFFF;
	if(priv->pshare->rf_ft_var.psd_skip_lookup_table){
		if(psd_report >=14)
			psd_report = 23;
		else
			psd_report = 8;
	}
	else{
		psd_report = (int)(ConvertTo_dB((u4Byte)psd_report));		
	}

	return psd_report;
	
}


void Scan_BB_PSD(
	IN struct dm_struct*	p_dm_odm,
	int *PSD_report_right,
	int *PSD_report_left,
	int len,
	int initial_gain)
{
	struct rtl8192cd_priv *priv=p_dm_odm->priv;
	struct phydm_dig_struct*  p_dm_dig_table = &p_dm_odm->dm_dig_table;
	u1Byte  ST_TH_origin;
	u1Byte	idx[20]={//96,99,102,106,109,112,115,118,122,125,
	                     224,227,230,234,237,240,243,246,250,253,
					0,3,6,10,13,16,19,22,26,29};
	int tone_idx, channel_org, channel, i, BW_org;

	// set DFS ST_TH to max value
	ST_TH_origin = RTL_R8(0x91c);
	RTL_W8(0x91c, 0x4e);
	
	// Turn off CCK
	odm_set_bb_reg(p_dm_odm, 0x808, BIT28, 0);   //808[28]

	// Turn off TX
	// Pause TX Queue
	if (!priv->pmib->dot11DFSEntry.disable_tx)
		odm_write_1byte(p_dm_odm, 0x522, 0xFF); //REG_TXPAUSE 改為0x522

	// Turn off CCA
	if((GET_CHIP_VER(priv) == VERSION_8814A) || (GET_CHIP_VER(priv) == VERSION_8822B)){
		odm_set_bb_reg(p_dm_odm, 0x838, BIT1, 0x1); //838[1] 設為1
	}
	else{
		odm_set_bb_reg(p_dm_odm, 0x838, BIT3, 0x1); //838[3] 設為1
	}
	
	// PHYTXON while loop
	phy_set_bb_reg(priv, 0x8fc, 0xfff, 0);
	i = 0;
	while (odm_get_bb_reg(p_dm_odm, 0xfa0, BIT18)) {
		i++;
		if (i > 1000000) {
			panic_printk("Wait in %s() more than %d times!\n", __FUNCTION__, i);
			break;
		}
	}

	// backup IGI_origin , set IGI = 0x3e;
	//p_dm_dig_table->is_psd_in_progress = TRUE;
	odm_pause_dig(p_dm_odm, PHYDM_PAUSE, PHYDM_PAUSE_LEVEL_2, initial_gain);

	// Turn off 3-wire
	odm_set_bb_reg(p_dm_odm, 0xC00, BIT1|BIT0, 0x0); //c00[1:0] 寫0

	// pts value = 128, 256, 512, 1024
	odm_set_bb_reg(p_dm_odm, 0x910, BIT14|BIT15, 0x1); //910[15:14]設為1, 用256點
	odm_set_bb_reg(p_dm_odm, 0x910, BIT12|BIT13, 0x1); //910[13:12]設為1, avg 8 次

	// scan in-band PSD
	channel_org = odm_get_rf_reg(p_dm_odm, RF_PATH_A, RF_CHNLBW, 0x3FF);
	BW_org = priv->pshare->CurrentChannelBW;
	if(priv->pshare->CurrentChannelBW != CHANNEL_WIDTH_20){
		priv->pshare->No_RF_Write = 0;
		SwBWMode(priv, CHANNEL_WIDTH_20, 0);
		priv->pshare->No_RF_Write = 1;
	}
	
	if (priv->pshare->rf_ft_var.dfs_scan_inband) {
		int PSD_report_inband[20];
		for (tone_idx=0;tone_idx<len;tone_idx++)
			PSD_report_inband[tone_idx] = GetPSDData_8812(p_dm_odm, idx[tone_idx], initial_gain);
		panic_printk("PSD inband: ");
		for (i=0; i<len; i++)
			panic_printk("%d ", PSD_report_inband[i]);
		panic_printk("\n");
	}

	// scan right(higher) neighbor channel
	if (priv->pshare->CurrentChannelBW == CHANNEL_WIDTH_20)
		channel = channel_org + 4;
	else if (priv->pshare->CurrentChannelBW == CHANNEL_WIDTH_40)
		channel = channel_org + 6;
	else
		channel = channel_org + 10;
	delay_us(300);  // for idle 20M, it will  emit signal in right 20M channel
	priv->pshare->No_RF_Write = 0;
	odm_set_rf_reg(p_dm_odm, RF_PATH_A, RF_CHNLBW, 0x3FF, channel);
	priv->pshare->No_RF_Write = 1;

	for (tone_idx=0;tone_idx<len;tone_idx++)
		PSD_report_right[tone_idx] = GetPSDData_8812(p_dm_odm, idx[tone_idx], initial_gain);
	
	// scan left(lower) neighbor channel
	if (priv->pshare->CurrentChannelBW == CHANNEL_WIDTH_20)
		channel = channel_org - 4;
	else if (priv->pshare->CurrentChannelBW == CHANNEL_WIDTH_40)
		channel = channel_org - 6;
	else
		channel = channel_org - 10;
	priv->pshare->No_RF_Write = 0;
	odm_set_rf_reg(p_dm_odm, RF_PATH_A, RF_CHNLBW, 0x3FF, channel);
	priv->pshare->No_RF_Write = 1;

	for (tone_idx=0;tone_idx<len;tone_idx++)
		PSD_report_left[tone_idx] = GetPSDData_8812(p_dm_odm, idx[tone_idx], initial_gain);
	

	// restore originl center frequency
	if(BW_org != priv->pshare->CurrentChannelBW){
		priv->pshare->No_RF_Write = 0;
		SwBWMode(priv, BW_org, priv->pshare->offset_2nd_chan);
		priv->pshare->No_RF_Write = 1;
	}
	priv->pshare->No_RF_Write = 0;
	odm_set_rf_reg(p_dm_odm, RF_PATH_A, RF_CHNLBW, 0x3FF, channel_org);
	priv->pshare->No_RF_Write = 1;

	// Turn on 3-wire
	odm_set_bb_reg(p_dm_odm, 0xc00, BIT1|BIT0, 0x3); //c00[1:0] 寫3

	// Restore Current Settings
	// Resume DIG
	//p_dm_dig_table->is_psd_in_progress = FALSE;
	odm_pause_dig(p_dm_odm, PHYDM_RESUME, PHYDM_PAUSE_LEVEL_2, NONE);

	//Turn on CCA
	if((GET_CHIP_VER(priv) == VERSION_8814A) || (GET_CHIP_VER(priv) == VERSION_8822B)){
		odm_set_bb_reg(p_dm_odm, 0x838, BIT1, 0); //838[1] 設為0
	}
	else{
		odm_set_bb_reg(p_dm_odm, 0x838, BIT3, 0); //838[3] 設為0
	}
	

	// Turn on TX
	// Resume TX Queue
	if (!priv->pmib->dot11DFSEntry.disable_tx)
		odm_write_1byte(p_dm_odm, 0x522, 0x00); //REG_TXPAUSE 改為0x522

	// CCK on
	if (priv->pmib->dot11RFEntry.phyBandSelect == PHY_BAND_2G)
	odm_set_bb_reg(p_dm_odm, 0x808, BIT28, 1); //808[28]	
	
	// Resume DFS ST_TH
	RTL_W8(0x91c, ST_TH_origin);
}

#if defined(CONFIG_WLAN_HAL_8814BE)
void dfs_radar_distinguish_for_8814b(struct rtl8192cd_priv *priv)
{
	unsigned int reg_2e08, reg_2e0c, reg_2e10, reg_2e18;
	unsigned int pw_rpt_14b_set[4]={0}, pri_rpt_14b;
	signed char loct_rpt_14b_set[8]={0};
	unsigned char i, dc_cnt = 0, short_cnt_th = 0, long_cnt_th = 0, loct_loop_cnt = 0, pw_loop_cnt = 0;
	unsigned int pw_cnt_14b = 0, loct_diff_14b = 0;

	/*for MIB*/
	unsigned int pw_lowth1_14b = 0, pw_uppth1_14b = 0, pw_lowth2_14b = 0, pw_cnt_th_14b = 0;
	unsigned int loct_diff_th_14b = 0, pri_rpt_lowth1_14b = 0;
	unsigned int pw_uppth2_14b = 0, pri_rpt_uppth1_14b = 0;

	pw_lowth1_14b = priv->pshare->rf_ft_var.pw_lowth1;
	pw_uppth1_14b = priv->pshare->rf_ft_var.pw_uppth1;
	pw_lowth2_14b = priv->pshare->rf_ft_var.pw_lowth2;
	pw_uppth2_14b = priv->pshare->rf_ft_var.pw_uppth2;
	pw_cnt_th_14b = priv->pshare->rf_ft_var.pw_cnt_th;
	pri_rpt_lowth1_14b = priv->pshare->rf_ft_var.pri_rpt_lowth1;
	pri_rpt_uppth1_14b = priv->pshare->rf_ft_var.pri_rpt_uppth1;
	loct_diff_th_14b = priv->pshare->rf_ft_var.loct_diff_th;

	if (priv->pshare->rf_ft_var.manual_dfs_regdomain == 3) {
		priv->pshare->rf_ft_var.pw_lowth1 = 1;
		priv->pshare->rf_ft_var.pw_uppth1 = 38;
		priv->pshare->rf_ft_var.pw_lowth2 = 49;
		priv->pshare->rf_ft_var.pw_uppth2 = 76;
	}	
		
	/*read pw/pri/lctn report*/
	
	/*Wordlength : U(8,0) , unit: 400ns*/
	reg_2e08 = phy_query_bb_reg(priv, 0x2e08, 0xffffffff);
	pw_rpt_14b_set[0]=(unsigned int)(reg_2e08 & 0x000000ff);
	pw_rpt_14b_set[1]=(unsigned int)((reg_2e08 & 0x0000ff00)>>8);
	pw_rpt_14b_set[2]=(unsigned int)((reg_2e08 & 0x00ff0000)>>16);
	pw_rpt_14b_set[3]=(unsigned int)((reg_2e08 & 0xff000000)>>24);

	/* Wordlength : U(12,0) , unit: us */
	reg_2e18 = phy_query_bb_reg(priv, 0x2e18, 0xffffffff);
	pri_rpt_14b = (unsigned int)((reg_2e18 & 0x0fff0000)>>16);
	
	/* Wordlength : S(8,0) , unit: MHz */
	reg_2e0c = phy_query_bb_reg(priv, 0x2e0c, 0xffffffff);
	loct_rpt_14b_set[0]=(signed char)(reg_2e0c & 0x000000ff);
	loct_rpt_14b_set[1]=(signed char)((reg_2e0c & 0x0000ff00)>>8);
	loct_rpt_14b_set[2]=(signed char)((reg_2e0c & 0x00ff0000)>>16);
	loct_rpt_14b_set[3]=(signed char)((reg_2e0c & 0xff000000)>>24);
	reg_2e10 = phy_query_bb_reg(priv, 0x2e10, 0xffffffff);
	loct_rpt_14b_set[4]=(signed char)(reg_2e10 & 0x000000ff);
	loct_rpt_14b_set[5]=(signed char)((reg_2e10 & 0x0000ff00)>>8);
	loct_rpt_14b_set[6]=(signed char)((reg_2e10 & 0x00ff0000)>>16);
	loct_rpt_14b_set[7]=(signed char)((reg_2e10 & 0xff000000)>>24);

	priv->pulsewidth_flag = 0;
	priv->loct_flag = 0;
	priv->pulseinterval_flag = 0;
	for (i = 0; i < 4; i++) {
		priv->pw_14b_rslt[i] = 0;
	}
	for (i = 0; i < 8; i++) {
		priv->loct_14b_rslt[i] = 0;
	}
	priv->pri_14b_rslt = 0;

	short_cnt_th = phy_query_bb_reg(priv, 0xa50, 0x00f00000);
	long_cnt_th = phy_query_bb_reg(priv, 0xa4c, 0xf0000000);

	if (priv->pshare->rf_ft_var.manual_dfs_regdomain == 3) {
		loct_loop_cnt = short_cnt_th + 2;
		pw_loop_cnt = short_cnt_th + 2;
	}
	else{
		if (long_cnt_th > short_cnt_th)
		{
			loct_loop_cnt = short_cnt_th  + 2;
			pw_loop_cnt = short_cnt_th + 2;
		}	
		else
		{
			loct_loop_cnt = long_cnt_th  + 2;
			pw_loop_cnt = long_cnt_th  + 2;
		}
	}
	
	if (loct_loop_cnt > 8)
		loct_loop_cnt = 8;
	if (pw_loop_cnt > 4)
		pw_loop_cnt = 4;	

	/*Calculate the difference between each element (1st Not calculated)*/
	for(i = 0; i < loct_loop_cnt; i++)
		priv->loct_14b_rslt[i] = loct_rpt_14b_set[i];
	for(i = 1; i < loct_loop_cnt; i++)
		loct_diff_14b = loct_diff_14b + abs(priv->loct_14b_rslt[i] - priv->loct_14b_rslt[i-1]);
	/*The value may less than the average deviation, since the variable type is int (not float)*/
	if (loct_diff_14b < loct_diff_th_14b)
		priv->loct_flag = 1;
	
	for(i = 0; i < loct_loop_cnt; i++) {
		if (-2 < loct_rpt_14b_set[i] || loct_rpt_14b_set[i] < 2)
			dc_cnt++;
		if (dc_cnt >= 3)
			pw_lowth1_14b = 0;
	}

	/*PW check*/
	for (i = 0; i < pw_loop_cnt; i++) {
		priv->pw_14b_rslt[i] = pw_rpt_14b_set[i];
		if (pw_lowth1_14b <= priv->pw_14b_rslt[i] && priv->pw_14b_rslt[i] <= pw_uppth1_14b)
			pw_cnt_14b++;
	}
		if (pw_cnt_14b > pw_cnt_th_14b)
			priv->pulsewidth_flag = 1;
		
	for (i = 0; i < pw_loop_cnt; i++) {
		if (pw_lowth2_14b <= priv->pw_14b_rslt[i] && priv->pw_14b_rslt[i] <= pw_uppth2_14b) {
			priv->pulsewidth_flag = 1;
			if (priv->pri_14b_rslt==0) {	// No pri rep with long radar
				priv->long_radar_flag = 1;
				priv->pulseinterval_flag = 1;
			}
		}
	}
	
	priv->pri_14b_rslt = pri_rpt_14b;
	if (priv->idle_flag == 1 && priv->pri_14b_rslt != 0) {
		if (pri_rpt_lowth1_14b <= priv->pri_14b_rslt && priv->pri_14b_rslt <= pri_rpt_uppth1_14b)
			priv->pulseinterval_flag = 1;
		else if (priv->long_radar_flag) 
			priv->pulseinterval_flag = 1;
	} else {
		priv->pulseinterval_flag = 1;
	}

	if (priv->pshare->rf_ft_var.dfs_print_hist_report) {
		panic_printk("8814b current mib information:\n");
		panic_printk("pw_lowth1_14b = %d, pw_uppth1_14b = %d, pw_cnt_th_14b = %d\n",
			     pw_lowth1_14b, pw_uppth1_14b, pw_cnt_th_14b);
		panic_printk("pw_lowth2_14b = %d, pw_uppth2_14b = %d\n",
			     pw_lowth2_14b, pw_uppth2_14b);
		panic_printk("loct_diff_th_14b = %d, pri_rpt_lowth1_14b = %d, pri_rpt_uppth1_14b = %d\n",
			     loct_diff_th_14b, pri_rpt_lowth1_14b, pri_rpt_uppth1_14b);
		panic_printk("\n");

		panic_printk("8814b current register:\n");
		panic_printk("reg_2e08 = [%08x]\n", reg_2e08);
		panic_printk("reg_2e0c = [%08x]\n", reg_2e0c);
		panic_printk("reg_2e10 = [%08x]\n", reg_2e10);
		panic_printk("reg_2e18 = [%08x]\n", reg_2e18);
		panic_printk("\n");

		panic_printk("8814b current report:\n");
		panic_printk("pw_rpt(1-4) = [%d, %d, %d, %d]\n",
			     pw_rpt_14b_set[0], pw_rpt_14b_set[1], pw_rpt_14b_set[2], pw_rpt_14b_set[3]);
		panic_printk("loct_rpt(1-8) = [%d, %d, %d, %d, %d, %d, %d, %d]\n",
			     loct_rpt_14b_set[0], loct_rpt_14b_set[1], loct_rpt_14b_set[2], loct_rpt_14b_set[3],loct_rpt_14b_set[4], loct_rpt_14b_set[5], loct_rpt_14b_set[6], loct_rpt_14b_set[7]);
		panic_printk("pri information = [%d]\n", pri_rpt_14b);
		panic_printk("\n");
	}
}
#endif

#if defined(CONFIG_WLAN_HAL_8812FE)
void dfs_radar_distinguish_for_8812f(struct rtl8192cd_priv *priv)
{
	unsigned int reg_2e08, reg_2e24, reg_2e28, reg_2e0c, reg_2e10, reg_2e20, reg_2e2c, reg_2e30, reg_2e34;
	unsigned int pw_rpt_12f_set[12]={0}, pri_rpt_12f_set[12]={0};
	signed char loct_rpt_12f_set[12]={0};
	unsigned char i, dc_cnt = 0, short_cnt_th = 0, long_cnt_th = 0, loop_cnt = 0;
	unsigned int pw_cnt_12f = 0, pw_long_cnt_12f = 0, loct_diff = 0, pw_diff = 0, pri_diff = 0;
	unsigned int pri_cnt_12f = 0, pri_long_cnt_12f = 0, pri_min;

	/*For MIB*/
	unsigned int pw_lowth1_12f = 0, pw_uppth1_12f = 0, pw_lowth2_12f = 0, pw_cnt_th_12f = 0, pri_cnt_th_12f = 0; 
	unsigned int loct_diff_th_12f = 0, pri_rpt_lowth1_12f = 0;
	unsigned int pw_uppth2_12f = 0, pri_rpt_uppth1_12f = 0;
	unsigned int pri_rpt_lowth2_12f = 0, pri_rpt_uppth2_12f = 0, pri_diff_th_fin;

	// For short pulse
	pw_lowth1_12f = priv->pshare->rf_ft_var.pw_lowth1;
	pw_uppth1_12f = priv->pshare->rf_ft_var.pw_uppth1;
	pri_rpt_lowth1_12f = priv->pshare->rf_ft_var.pri_rpt_lowth1;
	pri_rpt_uppth1_12f = priv->pshare->rf_ft_var.pri_rpt_uppth1;
	pri_diff_th_fin = priv->pshare->rf_ft_var.pri_diff_th;

	// For long pulse
	pw_lowth2_12f = priv->pshare->rf_ft_var.pw_lowth2;
	pw_uppth2_12f = priv->pshare->rf_ft_var.pw_uppth2;
	pri_rpt_lowth2_12f = priv->pshare->rf_ft_var.pri_rpt_lowth2;
	pri_rpt_uppth2_12f = priv->pshare->rf_ft_var.pri_rpt_uppth2;
	
	loct_diff_th_12f = priv->pshare->rf_ft_var.loct_diff_th;

	/*read pw/pri/lctn report*/
	/* PW : Wordlength : U(8,0) , unit: 400ns*/
	reg_2e08 = phy_query_bb_reg(priv, 0x2e08, 0xffffffff);
	pw_rpt_12f_set[0]=(unsigned int)(reg_2e08 & 0x000000ff);
	pw_rpt_12f_set[1]=(unsigned int)((reg_2e08 & 0x0000ff00)>>8);
	pw_rpt_12f_set[2]=(unsigned int)((reg_2e08 & 0x00ff0000)>>16);
	pw_rpt_12f_set[3]=(unsigned int)((reg_2e08 & 0xff000000)>>24);
	reg_2e24 = phy_query_bb_reg(priv, 0x2e24, 0xffffffff);
	pw_rpt_12f_set[4]=(unsigned int)(reg_2e24 & 0x000000ff);
	pw_rpt_12f_set[5]=(unsigned int)((reg_2e24 & 0x0000ff00)>>8);
	pw_rpt_12f_set[6]=(unsigned int)((reg_2e24 & 0x00ff0000)>>16);
	pw_rpt_12f_set[7]=(unsigned int)((reg_2e24 & 0xff000000)>>24);
	reg_2e28 = phy_query_bb_reg(priv, 0x2e28, 0xffffffff);
	pw_rpt_12f_set[8]=(unsigned int)(reg_2e28 & 0x000000ff);
	pw_rpt_12f_set[9]=(unsigned int)((reg_2e28 & 0x0000ff00)>>8);
	pw_rpt_12f_set[10]=(unsigned int)((reg_2e28 & 0x00ff0000)>>16);
	pw_rpt_12f_set[11]=(unsigned int)((reg_2e28 & 0xff000000)>>24);

	/* PRI : Wordlength : U(8,0) , unit: 25.6us */
	reg_2e2c = phy_query_bb_reg(priv, 0x2e2c, 0xffffffff);
	pri_rpt_12f_set[0]=(unsigned int)(reg_2e2c & 0x000000ff);
	pri_rpt_12f_set[1]=(unsigned int)((reg_2e2c & 0x0000ff00)>>8);
	pri_rpt_12f_set[2]=(unsigned int)((reg_2e2c & 0x00ff0000)>>16);
	pri_rpt_12f_set[3]=(unsigned int)((reg_2e2c & 0xff000000)>>24);
	reg_2e30 = phy_query_bb_reg(priv, 0x2e30, 0xffffffff);
	pri_rpt_12f_set[4]=(unsigned int)(reg_2e30 & 0x000000ff);
	pri_rpt_12f_set[5]=(unsigned int)((reg_2e30 & 0x0000ff00)>>8);
	pri_rpt_12f_set[6]=(unsigned int)((reg_2e30 & 0x00ff0000)>>16);
	pri_rpt_12f_set[7]=(unsigned int)((reg_2e30 & 0xff000000)>>24);
	reg_2e34 = phy_query_bb_reg(priv, 0x2e34, 0xffffffff);
	pri_rpt_12f_set[8]=(unsigned int)(reg_2e34 & 0x000000ff);
	pri_rpt_12f_set[9]=(unsigned int)((reg_2e34 & 0x0000ff00)>>8);
	pri_rpt_12f_set[10]=(unsigned int)((reg_2e34 & 0x00ff0000)>>16);
	pri_rpt_12f_set[11]=(unsigned int)((reg_2e34 & 0xff000000)>>24);
	
	/* Location : Wordlength : S(8,0) , unit: MHz */
	reg_2e0c = phy_query_bb_reg(priv, 0x2e0c, 0xffffffff);
	loct_rpt_12f_set[0]=(signed char)(reg_2e0c & 0x000000ff);
	loct_rpt_12f_set[1]=(signed char)((reg_2e0c & 0x0000ff00)>>8);
	loct_rpt_12f_set[2]=(signed char)((reg_2e0c & 0x00ff0000)>>16);
	loct_rpt_12f_set[3]=(signed char)((reg_2e0c & 0xff000000)>>24);
	reg_2e10 = phy_query_bb_reg(priv, 0x2e10, 0xffffffff);
	loct_rpt_12f_set[4]=(signed char)(reg_2e10 & 0x000000ff);
	loct_rpt_12f_set[5]=(signed char)((reg_2e10 & 0x0000ff00)>>8);
	loct_rpt_12f_set[6]=(signed char)((reg_2e10 & 0x00ff0000)>>16);
	loct_rpt_12f_set[7]=(signed char)((reg_2e10 & 0xff000000)>>24);
	reg_2e20 = phy_query_bb_reg(priv, 0x2e20, 0xffffffff);
	loct_rpt_12f_set[8]=(signed char)(reg_2e20 & 0x000000ff);
	loct_rpt_12f_set[9]=(signed char)((reg_2e20 & 0x0000ff00)>>8);
	loct_rpt_12f_set[10]=(signed char)((reg_2e20 & 0x00ff0000)>>16);
	loct_rpt_12f_set[11]=(signed char)((reg_2e20 & 0xff000000)>>24);

	priv->pulsewidth_flag = 0;
	priv->loct_flag = 0;
	priv->pulseinterval_flag = 0;
	for (i = 0; i < 12; i++) {
		priv->pw_12f_rslt[i] = 0;
		priv->loct_12f_rslt[i] = 0;
		priv->pri_12f_rslt[i] = 0;
	}

	short_cnt_th = phy_query_bb_reg(priv, 0xa50, 0x00f00000);
	long_cnt_th = phy_query_bb_reg(priv, 0xa4c, 0xf0000000);

	// In 8812F, a50[23:20] = N => The short_pulse_cnt_th is N+2
	if (priv->pshare->rf_ft_var.manual_dfs_regdomain == 3) {
		loop_cnt = short_cnt_th + 2;
	} else {
		if (short_cnt_th < long_cnt_th)
			loop_cnt = short_cnt_th  + 2;
		else
			loop_cnt = long_cnt_th  + 2;
	}
	
	if (loop_cnt > 12)
		loop_cnt = 12;

	if (priv->pshare->rf_ft_var.pw_pri_cnt_debug == 1) {
		pw_cnt_th_12f = priv->pshare->rf_ft_var.pw_cnt_th;
		pri_cnt_th_12f = priv->pshare->rf_ft_var.pri_cnt_th;
	} else {
		pw_cnt_th_12f = loop_cnt / 2;
		pri_cnt_th_12f = loop_cnt / 2;
	}

	/*Calculate the difference between each element (1st Not calculated)*/
	for(i = 0; i < loop_cnt; i++)
		priv->loct_12f_rslt[i] = loct_rpt_12f_set[i];
	for(i = 1; i < loop_cnt; i++) {
		//if (priv->loct_12f_rslt[i] != 0)
		loct_diff = loct_diff + abs(priv->loct_12f_rslt[i] - priv->loct_12f_rslt[i-1]);
	}
	/*The value may less than the average deviation, since the variable type is int (not float)*/
	if (loct_diff <= loct_diff_th_12f)
		priv->loct_flag = 1;

	/*Low PW in DC, relax pw_lowth1_12f*/
	for(i = 0; i < loop_cnt; i++) {
		if (-2 < loct_rpt_12f_set[i] && loct_rpt_12f_set[i] < 2)
			dc_cnt++;
		if (dc_cnt >= 3) {
			pw_lowth1_12f = 0;
			pri_rpt_lowth1_12f = 0;
			pri_diff_th_fin = pri_diff_th_fin + 10;
		}
	}
		
	/*PW check*/
	for (i = 0; i < loop_cnt; i++) {
		priv->pw_12f_rslt[i] = pw_rpt_12f_set[i];
		if (pw_lowth1_12f <= priv->pw_12f_rslt[i] && priv->pw_12f_rslt[i] <= pw_uppth1_12f)
			pw_cnt_12f++;
		
		// Since long radar doesn't exist in ETSI
		if (priv->pshare->rf_ft_var.manual_dfs_regdomain == 3) {
			if (pw_lowth2_12f <= priv->pw_12f_rslt[i] && priv->pw_12f_rslt[i] <= pw_uppth2_12f)
				pw_cnt_12f++;
		} else {
			if (pw_lowth2_12f <= priv->pw_12f_rslt[i] && priv->pw_12f_rslt[i] <= pw_uppth2_12f)
				pw_long_cnt_12f++;
		}
	}

	for(i = 1; i < loop_cnt; i++)
		pw_diff = pw_diff + abs(priv->pw_12f_rslt[i] - priv->pw_12f_rslt[i-1]);
		
	if ((pw_cnt_12f >= pw_cnt_th_12f || pw_long_cnt_12f >= pw_cnt_th_12f) && pw_diff <= priv->pshare->rf_ft_var.pw_diff_th)
		priv->pulsewidth_flag = 1;

	/*PRI check*/
	for(i = 0; i < loop_cnt -1; i++) {
		priv->pri_12f_rslt[i] = pri_rpt_12f_set[i];
		if (pri_rpt_lowth1_12f <= priv->pri_12f_rslt[i] && priv->pri_12f_rslt[i] <= pri_rpt_uppth1_12f)
			pri_cnt_12f++;
		
		// Since long radar doesn't exist in ETSI
		if (priv->pshare->rf_ft_var.manual_dfs_regdomain != 3) {
			if (pri_rpt_lowth2_12f <= priv->pri_12f_rslt[i] && priv->pri_12f_rslt[i] <= pri_rpt_uppth2_12f)
				pri_long_cnt_12f++;
		}
	}
	
	pri_min = 32767;
	for(i = 0; i < loop_cnt -1; i++) {
		if(priv->pri_12f_rslt[i] < pri_min && priv->pri_12f_rslt[i] != 0)
			pri_min = priv->pri_12f_rslt[i];
	}
	for(i = 1; i < loop_cnt -1; i++)
		pri_diff = pri_diff + abs((priv->pri_12f_rslt[i] % pri_min) - (priv->pri_12f_rslt[i-1] % pri_min));

	if ((pri_cnt_12f >= pri_cnt_th_12f || pri_long_cnt_12f >= pri_cnt_th_12f) && pri_diff <= pri_diff_th_fin)
		priv->pulseinterval_flag = 1;

	if (pri_long_cnt_12f >= pri_cnt_th_12f && pw_long_cnt_12f >= pw_cnt_th_12f)
		priv->long_radar_flag = 1;

	if (priv->pshare->rf_ft_var.dfs_print_hist_report) {
		panic_printk("8812f current mib information:\n");
		panic_printk("loop_cnt = %d\n", loop_cnt);
		panic_printk("pw_lowth1_12f = %d, pw_uppth1_12f = %d\n",
			     pw_lowth1_12f, pw_uppth1_12f);
		panic_printk("pw_lowth2_12f = %d, pw_uppth2_12f = %d\n",
			     pw_lowth2_12f, pw_uppth2_12f);
		panic_printk("pri_rpt_lowth1_12f = %d, pri_rpt_uppth1_12f = %d\n",
			     pri_rpt_lowth1_12f, pri_rpt_uppth1_12f);
		panic_printk("pri_rpt_lowth2_12f = %d, pri_rpt_uppth2_12f = %d\n",
			     pri_rpt_lowth2_12f, pri_rpt_uppth2_12f);
		panic_printk("pw_cnt_th_12f = %d, loct_diff_th_12f = %d, pri_cnt_th_12f = %d\n",
			     pw_cnt_th_12f, loct_diff_th_12f, pri_cnt_th_12f);
		panic_printk("\n");

		panic_printk("8812f current register:\n");
		panic_printk("PW :\n");
		panic_printk("reg_2e08 = [%08x]\n", reg_2e08);
		panic_printk("reg_2e24 = [%08x]\n", reg_2e24);
		panic_printk("reg_2e28 = [%08x]\n", reg_2e28);
		panic_printk("Location :\n");
		panic_printk("reg_2e0c = [%08x]\n", reg_2e0c);
		panic_printk("reg_2e10 = [%08x]\n", reg_2e10);
		panic_printk("reg_2e20 = [%08x]\n", reg_2e20);
		panic_printk("PRI :\n");
		panic_printk("reg_2e2c = [%08x]\n", reg_2e2c);
		panic_printk("reg_2e30 = [%08x]\n", reg_2e30);
		panic_printk("reg_2e34 = [%08x]\n", reg_2e34);
		panic_printk("\n");

		panic_printk("8812f current report:\n");
		panic_printk("pw_rpt(1-12) = [%d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d]\n",
			     pw_rpt_12f_set[0], pw_rpt_12f_set[1], pw_rpt_12f_set[2], pw_rpt_12f_set[3], pw_rpt_12f_set[4], pw_rpt_12f_set[5], pw_rpt_12f_set[6], pw_rpt_12f_set[7],pw_rpt_12f_set[8], pw_rpt_12f_set[9], pw_rpt_12f_set[10], pw_rpt_12f_set[11]);
		panic_printk("loct_rpt(1-12) = [%d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d]\n",
			     loct_rpt_12f_set[0], loct_rpt_12f_set[1], loct_rpt_12f_set[2], loct_rpt_12f_set[3], loct_rpt_12f_set[4], loct_rpt_12f_set[5], loct_rpt_12f_set[6], loct_rpt_12f_set[7], loct_rpt_12f_set[8], loct_rpt_12f_set[9], loct_rpt_12f_set[10], loct_rpt_12f_set[11]);
		panic_printk("pri_rpt(1-12) = [%d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d]\n",
			     pri_rpt_12f_set[0], pri_rpt_12f_set[1], pri_rpt_12f_set[2], pri_rpt_12f_set[3], pri_rpt_12f_set[4], pri_rpt_12f_set[5], pri_rpt_12f_set[6], pri_rpt_12f_set[7], pri_rpt_12f_set[8], pri_rpt_12f_set[9], pri_rpt_12f_set[10], pri_rpt_12f_set[11]);
		panic_printk("\n");
		
		panic_printk("pw_cnt_12f = %d, pri_cnt_12f = %d\n",
			     pw_cnt_12f, pri_cnt_12f);
		panic_printk("loct_diff = %d, pw_diff = %d, pri_diff = %d\n",
			     loct_diff, pw_diff, pri_diff);
		panic_printk("\n");
	}
}
#endif

void rtl8192cd_dfs_det_chk(struct rtl8192cd_priv *priv)
{
	struct dm_struct *dm = &(priv->pshare->_dmODM);
	struct ccx_info *ccx = &dm->dm_ccx_info;
	unsigned int regf98_value;
	unsigned int reg2e00_value;
#if defined(CONFIG_WLAN_HAL_8814BE)
	unsigned int reg2e20_value;
#endif
	unsigned int reg918_value;
	unsigned int reg91c_value;
	unsigned int reg920_value;
	unsigned int reg924_value;
	unsigned int reg19e4_value;

	unsigned int rega40_value;
	unsigned int rega44_value;
	unsigned int rega48_value;
	unsigned int rega4c_value;
	unsigned int rega50_value;
	unsigned int rega54_value;
	u32 reg_dfsrpt_value = 0;
	
	unsigned int FA_count_cur=0, FA_count_inc=0;
	unsigned int CCA_count_cur=0, CCA_count_inc=0;
	unsigned int VHT_CRC_ok_cnt_cur=0, VHT_CRC_ok_cnt_inc=0;
	unsigned int HT_CRC_ok_cnt_cur=0, HT_CRC_ok_cnt_inc=0;
	unsigned int LEG_CRC_ok_cnt_cur=0, LEG_CRC_ok_cnt_inc=0;
	unsigned int Total_CRC_OK_cnt_inc=0, FA_CRCOK_ratio=0;
	unsigned char DFS_tri_short_pulse=0, DFS_tri_long_pulse=0, fa_mask_mid_th=0, fa_mask_lower_th=0;
	unsigned int short_pulse_cnt_cur=0, short_pulse_cnt_inc=0;
	unsigned int long_pulse_cnt_cur=0, long_pulse_cnt_inc=0;
	unsigned int total_pulse_count_inc=0, max_sht_pusle_cnt_th=0;
	unsigned int sum, k, fa_flag=0;
	unsigned int st_L2H_new=0, st_L2H_tmp, index=0, fault_flag_det, fault_flag_psd;
	int j, flags=0;
	unsigned long throughput = 0;

	int i, PSD_report_right[20], PSD_report_left[20];
	int max_right, max_left;
	int max_fa_in_hist=0, total_fa_in_hist=0, pre_post_now_acc_fa_in_hist=0;

	if (priv->det_asoc_clear > 0) {
		priv->det_asoc_clear--;
		priv->pmib->dot11DFSEntry.DFS_detected = 0;
		priv->FA_count_pre = 0;
		priv->CCA_count_pre = 0;
		priv->VHT_CRC_ok_cnt_pre = 0;
		priv->HT_CRC_ok_cnt_pre = 0;
		priv->LEG_CRC_ok_cnt_pre = 0;
		priv->mask_idx = 0;
		priv->hist_idx = 0;
		priv->mask_hist_checked = 0;
		memset(priv->radar_det_mask_hist, 0, sizeof(priv->radar_det_mask_hist));
		memset(priv->pulse_flag_hist, 0, sizeof(priv->pulse_flag_hist));
		memset(priv->pulse_type_hist, 0, sizeof(priv->pulse_type_hist));
		mod_timer(&priv->dfs_det_chk_timer, jiffies + RTL_MILISECONDS_TO_JIFFIES(priv->pshare->rf_ft_var.dfs_det_period*10));
		return;
	}
	
	throughput = priv->ext_stats.tx_avarage+priv->ext_stats.rx_avarage;
#ifdef UNIVERSAL_REPEATER
	if (GET_VXD_PRIV(priv) && (GET_VXD_PRIV(priv)->drv_state & DRV_STATE_OPEN))
		throughput += (GET_VXD_PRIV(priv)->ext_stats.tx_avarage + GET_VXD_PRIV(priv)->ext_stats.rx_avarage);
#endif

#ifdef MBSSID
	if (priv->pmib->miscEntry.vap_enable) {
		for (j=0; j<RTL8192CD_NUM_VWLAN; j++) {
			if (IS_DRV_OPEN(priv->pvap_priv[j])) {
				throughput += priv->pvap_priv[j]->ext_stats.tx_avarage+priv->pvap_priv[j]->ext_stats.rx_avarage;
			}
		}
	}
#endif

	// Get FA count during past 100ms
	if ((GET_CHIP_VER(priv) == VERSION_8814B) || (GET_CHIP_VER(priv) == VERSION_8812F)){
		FA_count_cur = phy_query_bb_reg(priv, 0x2d00, 0x0000ffff);
		CCA_count_cur = phy_query_bb_reg(priv, 0x2c08, 0xffff0000);
	}
	else {
		FA_count_cur = phy_query_bb_reg(priv, 0xf48, 0x0000ffff);
		CCA_count_cur = phy_query_bb_reg(priv, 0xf08, 0xffff0000);
	}
	
	if (priv->FA_count_pre == 0)
		FA_count_inc = 0;
	else if (FA_count_cur >= priv->FA_count_pre)
		FA_count_inc = FA_count_cur - priv->FA_count_pre;
	else
		FA_count_inc = FA_count_cur;
	priv->FA_count_pre = FA_count_cur;

	if (priv->CCA_count_pre == 0)
		CCA_count_inc = 0;
	else if (CCA_count_cur >= priv->CCA_count_pre)
		CCA_count_inc = CCA_count_cur - priv->CCA_count_pre;
	else
		CCA_count_inc = CCA_count_cur;	
	priv->CCA_count_pre = CCA_count_cur;

	priv->fa_inc_hist[priv->mask_idx] = FA_count_inc;	
	for (i=0; i<5; i++) {		
		total_fa_in_hist = total_fa_in_hist + priv->fa_inc_hist[i]; 	
		if (priv->fa_inc_hist[i] > max_fa_in_hist)			
			max_fa_in_hist = priv->fa_inc_hist[i];	
	}	
	/*the index is the two times before*/
	if (priv->mask_idx >= priv->pshare->rf_ft_var.dfs_det_flag_offset)		
		index = priv->mask_idx - priv->pshare->rf_ft_var.dfs_det_flag_offset;
	else		
		index = priv->pshare->rf_ft_var.dfs_det_hist_len + priv->mask_idx - priv->pshare->rf_ft_var.dfs_det_flag_offset;	
	if (index == 0) 	
		pre_post_now_acc_fa_in_hist = priv->fa_inc_hist[index] + priv->fa_inc_hist[index+1] + priv->fa_inc_hist[4]; 
	else if (index == 4)		
		pre_post_now_acc_fa_in_hist = priv->fa_inc_hist[index] + priv->fa_inc_hist[0] + priv->fa_inc_hist[index-1]; 
	else		
		pre_post_now_acc_fa_in_hist = priv->fa_inc_hist[index] + priv->fa_inc_hist[index+1] + priv->fa_inc_hist[index-1];

	// Get VHT CRC32 ok count during past 100ms
	if ((GET_CHIP_VER(priv) == VERSION_8814B) || (GET_CHIP_VER(priv) == VERSION_8812F)) {
		VHT_CRC_ok_cnt_cur = phy_query_bb_reg(priv, 0x2c0c, 0x0000ffff);
		if (VHT_CRC_ok_cnt_cur >= priv->VHT_CRC_ok_cnt_pre)
			VHT_CRC_ok_cnt_inc = VHT_CRC_ok_cnt_cur - priv->VHT_CRC_ok_cnt_pre;
		else
			VHT_CRC_ok_cnt_inc = VHT_CRC_ok_cnt_cur;
		priv->VHT_CRC_ok_cnt_pre = VHT_CRC_ok_cnt_cur;
	} else {
		VHT_CRC_ok_cnt_cur = phy_query_bb_reg(priv, 0xf0c, 0x00003fff);
		if (VHT_CRC_ok_cnt_cur >= priv->VHT_CRC_ok_cnt_pre)
			VHT_CRC_ok_cnt_inc = VHT_CRC_ok_cnt_cur - priv->VHT_CRC_ok_cnt_pre;
		else
			VHT_CRC_ok_cnt_inc = VHT_CRC_ok_cnt_cur;
		priv->VHT_CRC_ok_cnt_pre = VHT_CRC_ok_cnt_cur;
	}
	

	// Get HT CRC32 ok count during past 100ms
	if ((GET_CHIP_VER(priv) == VERSION_8814B) || (GET_CHIP_VER(priv) == VERSION_8812F)) {
		HT_CRC_ok_cnt_cur = phy_query_bb_reg(priv, 0x2c10, 0x0000ffff);
		if (HT_CRC_ok_cnt_cur >= priv->HT_CRC_ok_cnt_pre)
			HT_CRC_ok_cnt_inc = HT_CRC_ok_cnt_cur - priv->HT_CRC_ok_cnt_pre;
		else
			HT_CRC_ok_cnt_inc = HT_CRC_ok_cnt_cur;
		priv->HT_CRC_ok_cnt_pre = HT_CRC_ok_cnt_cur;
	} else {
		HT_CRC_ok_cnt_cur = phy_query_bb_reg(priv, 0xf10, 0x00003fff);
		if (HT_CRC_ok_cnt_cur >= priv->HT_CRC_ok_cnt_pre)
			HT_CRC_ok_cnt_inc = HT_CRC_ok_cnt_cur - priv->HT_CRC_ok_cnt_pre;
		else
			HT_CRC_ok_cnt_inc = HT_CRC_ok_cnt_cur;
		priv->HT_CRC_ok_cnt_pre = HT_CRC_ok_cnt_cur;
	}

	// Get Legacy CRC32 ok count during past 100ms
	if ((GET_CHIP_VER(priv) == VERSION_8814B) || (GET_CHIP_VER(priv) == VERSION_8812F)) {
		LEG_CRC_ok_cnt_cur = phy_query_bb_reg(priv, 0x2c14, 0x0000ffff);
			if (LEG_CRC_ok_cnt_cur >= priv->LEG_CRC_ok_cnt_pre)
				LEG_CRC_ok_cnt_inc = LEG_CRC_ok_cnt_cur - priv->LEG_CRC_ok_cnt_pre;
			else
				LEG_CRC_ok_cnt_inc = LEG_CRC_ok_cnt_cur;
		priv->LEG_CRC_ok_cnt_pre = LEG_CRC_ok_cnt_cur;
	} else {
		LEG_CRC_ok_cnt_cur = phy_query_bb_reg(priv, 0xf14, 0x00003fff);
			if (LEG_CRC_ok_cnt_cur >= priv->LEG_CRC_ok_cnt_pre)
				LEG_CRC_ok_cnt_inc = LEG_CRC_ok_cnt_cur - priv->LEG_CRC_ok_cnt_pre;
			else
				LEG_CRC_ok_cnt_inc = LEG_CRC_ok_cnt_cur;
		priv->LEG_CRC_ok_cnt_pre = LEG_CRC_ok_cnt_cur;
	} 

	if ((GET_CHIP_VER(priv) == VERSION_8814B) || (GET_CHIP_VER(priv) == VERSION_8812F)) {
		if (VHT_CRC_ok_cnt_cur == 0xffff ||
			HT_CRC_ok_cnt_cur == 0xffff ||
			LEG_CRC_ok_cnt_cur == 0xffff) {
		phy_set_bb_reg(priv, 0x1eb4, BIT(25), 1);
		phy_set_bb_reg(priv, 0x1eb4, BIT(25), 0);
		}
	} else {
		if ((VHT_CRC_ok_cnt_cur == 0x3fff) || (HT_CRC_ok_cnt_cur == 0x3fff) || (LEG_CRC_ok_cnt_cur == 0x3fff)) {
		phy_set_bb_reg(priv, 0xb58, BIT(0), 1);
		phy_set_bb_reg(priv, 0xb58, BIT(0), 0);
		}
	}

	Total_CRC_OK_cnt_inc = VHT_CRC_ok_cnt_inc + HT_CRC_ok_cnt_inc + LEG_CRC_ok_cnt_inc;

	// check if the FA occrus frequencly during 100ms
	// FA_count_inc is divided by Total_CRC_OK_cnt_inc, which helps to distinguish normal trasmission from interference
	if (Total_CRC_OK_cnt_inc > 0)
		FA_CRCOK_ratio	= FA_count_inc / Total_CRC_OK_cnt_inc;

	//=====dynamic power threshold (DPT) ========
	// Get short pulse count, need carefully handle the counter overflow
#if defined(CONFIG_WLAN_HAL_8814BE)
	if ((GET_CHIP_VER(priv) == VERSION_8814B)) {
		if (priv->seg1_flag == 1) {
			reg2e20_value = phy_query_bb_reg(priv, 0x2e20, 0xffffffff);
			short_pulse_cnt_cur = (u16)((reg2e20_value & 0x000ff800) >> 11);
			long_pulse_cnt_cur = (u16)((reg2e20_value & 0x0fc00000) >> 22);
		} else {
			reg2e00_value =  phy_query_bb_reg(priv, 0x2e00, 0xffffffff);
			short_pulse_cnt_cur = (u16)((reg2e00_value & 0x000ff800) >> 11);
			long_pulse_cnt_cur = (u16)((reg2e00_value & 0x0fc00000) >> 22);
		}
	} else 
#endif
	if ((GET_CHIP_VER(priv) == VERSION_8812F)){
		reg2e00_value =  phy_query_bb_reg(priv, 0x2e00, 0xffffffff);
		short_pulse_cnt_cur = (u16)((reg2e00_value & 0x000ff800) >> 11);
		long_pulse_cnt_cur = (u16)((reg2e00_value & 0x0fc00000) >> 22);
	} else 

	{
		regf98_value = phy_query_bb_reg(priv, 0xf98, 0xffffffff);
		short_pulse_cnt_cur = regf98_value & 0x000000ff;
		long_pulse_cnt_cur = (regf98_value & 0x0000ff00) >> 8;
	}
	
	if (short_pulse_cnt_cur >= priv->short_pulse_cnt_pre)
		short_pulse_cnt_inc = short_pulse_cnt_cur - priv->short_pulse_cnt_pre;
	else
		short_pulse_cnt_inc = short_pulse_cnt_cur;
	priv->short_pulse_cnt_pre = short_pulse_cnt_cur;

	// Get long pulse count, need carefully handle the counter overflow

	if (long_pulse_cnt_cur >= priv->long_pulse_cnt_pre)
		long_pulse_cnt_inc = long_pulse_cnt_cur - priv->long_pulse_cnt_pre;
	else
		long_pulse_cnt_inc = long_pulse_cnt_cur;
	priv->long_pulse_cnt_pre = long_pulse_cnt_cur;

	total_pulse_count_inc = short_pulse_cnt_inc + long_pulse_cnt_inc;

	if (priv->pshare->rf_ft_var.dfs_det_print) {
		panic_printk("=====================================================================\n");
		panic_printk("Total_CRC_OK_cnt_inc[%d] VHT_CRC_ok_cnt_inc[%d] HT_CRC_ok_cnt_inc[%d] LEG_CRC_ok_cnt_inc[%d] FA_count_inc[%d] CCA_count_inc[%d] FA_CRCOK_ratio[%d]\n",
			Total_CRC_OK_cnt_inc, VHT_CRC_ok_cnt_inc, HT_CRC_ok_cnt_inc, LEG_CRC_ok_cnt_inc, FA_count_inc, CCA_count_inc, FA_CRCOK_ratio);
		panic_printk("Init_Gain[%x] 0x91c[%x] 0xf98[%08x] short_pulse_cnt_inc[%d] long_pulse_cnt_inc[%d]\n",
			priv->ini_gain_cur, priv->st_L2H_cur, regf98_value, short_pulse_cnt_inc, long_pulse_cnt_inc);
		panic_printk("Throughput: %luMbps\n", (throughput>>17));
		panic_printk("nhm_duty_cycle = %d\n", (100-((ccx->nhm_rpt_sum * 100) >> 8)));
		if ((GET_CHIP_VER(priv) == VERSION_8814B) || (GET_CHIP_VER(priv) == VERSION_8812F)){
			rega40_value = phy_query_bb_reg(priv, 0xa40, 0xffffffff);
			rega44_value = phy_query_bb_reg(priv, 0xa44, 0xffffffff);
			rega48_value = phy_query_bb_reg(priv, 0xa48, 0xffffffff);
			rega4c_value = phy_query_bb_reg(priv, 0xa4c, 0xffffffff);
			rega50_value = phy_query_bb_reg(priv, 0xa50, 0xffffffff);
			rega54_value = phy_query_bb_reg(priv, 0xa54, 0xffffffff);
			reg2e00_value =  phy_query_bb_reg(priv, 0x2e00, 0xffffffff);
			panic_printk("0xa40[%08x] 0xa44[%08x] 0xa48[%08x] 0xa4c[%08x] 0xa50[%08x] 0xa54[%08x] 0x2e00[%08x] \n", rega40_value, rega44_value, rega48_value, rega4c_value, rega50_value, rega54_value, reg2e00_value);
#if defined(CONFIG_WLAN_HAL_8814BE)
			reg2e20_value = phy_query_bb_reg(priv, 0x2e20, 0xffffffff);
			panic_printk("0x2e20[%08x] \n", reg2e20_value);
#endif
		} else {
			reg918_value = phy_query_bb_reg(priv, 0x918, 0xffffffff);
			reg91c_value = phy_query_bb_reg(priv, 0x91c, 0xffffffff);
			reg920_value = phy_query_bb_reg(priv, 0x920, 0xffffffff);
			reg924_value = phy_query_bb_reg(priv, 0x924, 0xffffffff);
			reg19e4_value = phy_query_bb_reg(priv, 0x19e4, 0xffffffff);
			panic_printk("0x918[%08x] 0x91c[%08x] 0x920[%08x] 0x924[%08x] 0x19e4[%08x] \n", reg918_value, reg91c_value, reg920_value, reg924_value, reg19e4_value);
		}
		
	}

	// read hw dfs detection flag
#if defined(CONFIG_WLAN_HAL_8814BE)
	if ((GET_CHIP_VER(priv) == VERSION_8814B)) {
		if (priv->seg1_flag == 1) {
			DFS_tri_short_pulse = (reg2e20_value & BIT(20)) ? 1 : 0;
			DFS_tri_long_pulse = (reg2e20_value & BIT(28)) ? 1 : 0;
		} else {
			DFS_tri_short_pulse = (reg2e00_value & BIT(20)) ? 1 : 0;
			DFS_tri_long_pulse = (reg2e00_value & BIT(28)) ? 1 : 0;
		}
	} else
#endif
	if ((GET_CHIP_VER(priv) == VERSION_8812F)) {
		DFS_tri_short_pulse = (reg2e00_value & BIT(20)) ? 1 : 0;
		DFS_tri_long_pulse = (reg2e00_value & BIT(28)) ? 1 : 0;
	} else {
		DFS_tri_short_pulse = (regf98_value & BIT(17))? 1 : 0;
		DFS_tri_long_pulse = (regf98_value & BIT(19))? 1 : 0;
	}

	if (DFS_tri_short_pulse) {
		//RTL_W32(0x920, RTL_R32(0x920) | BIT(24));
		//RTL_W32(0x920, RTL_R32(0x920) & ~BIT(24));
		//RTL_W32(0x924, RTL_R32(0x924) & ~BIT(15));
		//RTL_W32(0x924, RTL_R32(0x924) | BIT(15));
	}
	if (DFS_tri_long_pulse) {
		//RTL_W32(0x920, RTL_R32(0x920) | BIT(25));
		//RTL_W32(0x920, RTL_R32(0x920) & ~BIT(25));
		//RTL_W32(0x924, RTL_R32(0x924) & ~BIT(15));
		//RTL_W32(0x924, RTL_R32(0x924) | BIT(15));
		// MKK
		if (priv->pshare->rf_ft_var.manual_dfs_regdomain == 2) {	
			if ((priv->pmib->dot11RFEntry.dot11channel >= 52) &&
				(priv->pmib->dot11RFEntry.dot11channel <= 64)) {
				DFS_tri_long_pulse = 0;
			}
		}
		// ETSI
		if (priv->pshare->rf_ft_var.manual_dfs_regdomain == 3) {
			DFS_tri_long_pulse = 0;
		}
	}

	st_L2H_new = priv->st_L2H_cur;
	priv->pulse_flag_hist[priv->mask_idx] = DFS_tri_short_pulse | DFS_tri_long_pulse;
	priv->pulse_type_hist[priv->mask_idx] = (DFS_tri_long_pulse)? 1 : 0;
	
#if defined(CONFIG_WLAN_HAL_8814BE)
	if((GET_CHIP_VER(priv) == VERSION_8814B)) {
		if(priv->pshare->rf_ft_var.dfs_radar_diff_on && priv->pshare->rf_ft_var.dfs_det_off == 0) {
			if(priv->pulse_flag_hist[priv->mask_idx])
				dfs_radar_distinguish_for_8814b(priv);
		}
	}
#endif

#if defined(CONFIG_WLAN_HAL_8812FE)
	if((GET_CHIP_VER(priv) == VERSION_8812F)) {
		if(priv->pshare->rf_ft_var.dfs_radar_diff_on && priv->pshare->rf_ft_var.dfs_det_off == 0) {
			if(priv->pulse_flag_hist[priv->mask_idx])
				dfs_radar_distinguish_for_8812f(priv);
		}
	}
#endif

	//max_sht_pusle_cnt_th =  phy_query_bb_reg(priv, 0x920, 0x000f0000)-1; //read 920[19:16]
	max_sht_pusle_cnt_th = priv->pshare->rf_ft_var.dfs_max_sht_pusle_cnt_th;
	if (priv->pshare->rf_ft_var.dfs_det_print3)
		panic_printk("max_sht_pusle_cnt_th = %d\n", max_sht_pusle_cnt_th);

	if(priv->idle_flag == 1){		
		if(priv->pshare->rf_ft_var.dfs_psd_idle_on == 1){
			SAVE_INT_AND_CLI(flags);
			SMP_LOCK(flags);
			Scan_BB_PSD(ODMPTR, PSD_report_right, PSD_report_left, 20, 0x3e);	
			SMP_UNLOCK(flags);
			RESTORE_INT(flags);
		}
		else{
			/*for (i=0; i<20; i++) {
				PSD_report_right[i] = 0;
				PSD_report_left[i] = 0;
			}*/
		}		
	}
	else{
		if(priv->pshare->rf_ft_var.dfs_psd_TP_on == 1){
			SAVE_INT_AND_CLI(flags);
			SMP_LOCK(flags);
			Scan_BB_PSD(ODMPTR, PSD_report_right, PSD_report_left, 20, 0x3e);	
			SMP_UNLOCK(flags);
			RESTORE_INT(flags);
		}
		else{
			for (i=0; i<20; i++) {
				PSD_report_right[i] = 0;
				PSD_report_left[i] = 0;
			}
		}
	}
	
	for (i=0; i<20; i++) {
		PSD_report_right[i] = (-110 + 0x3e) - 39 + PSD_report_right[i];
		PSD_report_left[i] = (-110 + 0x3e) - 39 + PSD_report_left[i];
	}
	if (priv->pshare->rf_ft_var.dfs_det_print_psd) {
		panic_printk("PSD right: ");
		for (i=0; i<20; i++)
			panic_printk("%d ", PSD_report_right[i]);
		//panic_printk("\n");
		panic_printk("	 PSD left: ");
		for (i=0; i<20; i++)
			panic_printk("%d ", PSD_report_left[i]);
		panic_printk("\n");
	}

	fault_flag_det = 0;
	fault_flag_psd = 0;
	fa_flag = 0;
	// ETSI
	if(priv->pshare->rf_ft_var.manual_dfs_regdomain == 3){
		fa_mask_mid_th = priv->pshare->rf_ft_var.dfs_fa_cnt_mid + 20;
		fa_mask_lower_th = priv->pshare->rf_ft_var.dfs_fa_cnt_lower + 40;
	}
	else{
		fa_mask_mid_th = priv->pshare->rf_ft_var.dfs_fa_cnt_mid;
		fa_mask_lower_th = priv->pshare->rf_ft_var.dfs_fa_cnt_lower;
	}
	//if (max_fa_in_hist >= fa_mask_mid_th || total_fa_in_hist >= priv->pshare->rf_ft_var.dfs_fa_hist || pre_post_now_acc_fa_in_hist >= fa_mask_mid_th){		
		/*if (priv->mask_idx >= priv->pshare->rf_ft_var.dfs_det_flag_offset)			
			index = priv->mask_idx - priv->pshare->rf_ft_var.dfs_det_flag_offset;		
		else			
			index = priv->pshare->rf_ft_var.dfs_det_hist_len + priv->mask_idx - priv->pshare->rf_ft_var.dfs_det_flag_offset;	*/	
	/*	priv->radar_det_mask_hist[index] = 1;		
		if (priv->pulse_flag_hist[index] == 1){ 		
			priv->pulse_flag_hist[index] = 0;			
			if (priv->pshare->rf_ft_var.dfs_det_print4){				
				panic_printk("Radar is masked : FA mask case3\n");						
			}		
		}				
		fa_flag = 1;	
	}
	if (((FA_count_inc >= priv->pshare->rf_ft_var.dfs_dpt_fa_th_upper) && (short_pulse_cnt_inc > max_sht_pusle_cnt_th)) ||
		(priv->ini_gain_cur >= priv->pshare->rf_ft_var.dpt_ini_gain_th)) {
		st_L2H_new = priv->pshare->rf_ft_var.dfs_dpt_st_l2h_max;
		if (priv->pshare->rf_ft_var.dfs_det_print3)
			panic_printk("[1] st_L2H_new %x\n", st_L2H_new);
		priv->radar_det_mask_hist[priv->mask_idx] = 1;
		if (priv->pulse_flag_hist[priv->mask_idx] == 1){
			priv->pulse_flag_hist[priv->mask_idx] = 0;
			if (priv->pshare->rf_ft_var.dfs_det_print4){
				panic_printk("Radar is masked : FA mask case1\n");			
			}
		}
		fa_flag = 1;
	}
	else if (((FA_count_inc >= fa_mask_mid_th) ||
				((FA_count_inc >= fa_mask_lower_th) &&
					(FA_count_inc >= priv->pshare->rf_ft_var.dfs_fa_cnt_inc_ratio * priv->FA_count_inc_pre))) 
		&& (short_pulse_cnt_inc > max_sht_pusle_cnt_th)) {
		if (priv->pshare->rf_ft_var.dfs_dpt_st_l2h_add)
			st_L2H_new += 2;
		if (priv->pshare->rf_ft_var.dfs_det_print3)
			panic_printk("[2] st_L2H_new %x\n", st_L2H_new);
		priv->radar_det_mask_hist[priv->mask_idx] = 1;
		if (priv->pulse_flag_hist[priv->mask_idx] == 1){
			priv->pulse_flag_hist[priv->mask_idx] = 0;
			if (priv->pshare->rf_ft_var.dfs_det_print4){
				panic_printk("Radar is masked : FA mask case2\n");			
			}
		}
		fa_flag = 1;
	}
	else 
	{
		if (((FA_CRCOK_ratio > priv->pshare->rf_ft_var.dfs_fa_ratio_th) &&
				(FA_count_inc >= priv->pshare->rf_ft_var.dfs_fa_cnt_lower) &&
				(Total_CRC_OK_cnt_inc > priv->pshare->rf_ft_var.dfs_crc32_cnt_lower)) ||
			((short_pulse_cnt_inc <= max_sht_pusle_cnt_th) && (short_pulse_cnt_inc != 0) &&
				(priv->idle_flag == 1) && (priv->pshare->rf_ft_var.manual_dfs_regdomain == 2)) ||
			(FA_count_inc >= priv->pshare->rf_ft_var.dfs_fa_cnt_upper))
			priv->radar_det_mask_hist[priv->mask_idx] = 1;
		else
			priv->radar_det_mask_hist[priv->mask_idx] = 0;
	}
*/
	priv->radar_det_mask_hist[priv->mask_idx] = 0;

		if ((dm->support_ability & ODM_BB_ENV_MONITOR)){	
		if ((100-((ccx->nhm_rpt_sum * 100) >> 8)) >= priv->pshare->rf_ft_var.dfs_nhm_duty_cycle_th) {
		if (priv->pulse_flag_hist[index] == 1) {
			priv->pulse_flag_hist[index] = 0;
			if (priv->pshare->rf_ft_var.dfs_det_print4) {
				panic_printk("Radar is masked : NHM duty cycle over TH\n");
			}
		}
	}
		}
	if (priv->pshare->rf_ft_var.dfs_det_print2) {
		panic_printk("mask_idx: %d\n", priv->mask_idx);
		panic_printk("radar_det_mask_hist: ");
		for (i=0; i<priv->pshare->rf_ft_var.dfs_det_hist_len; i++)
			panic_printk("%d ", priv->radar_det_mask_hist[i]);
		panic_printk("pulse_flag_hist: ");
		for (i=0; i<priv->pshare->rf_ft_var.dfs_det_hist_len; i++)
			panic_printk("%d ", priv->pulse_flag_hist[i]);
		panic_printk("fa_inc_hist: ");		
		for (i=0; i<5; i++) 		
			panic_printk("%d ", priv->fa_inc_hist[i]);		
		panic_printk("\n"); 	
		panic_printk("max_fa_in_hist: %d pre_post_now_acc_fa_in_hist: %d ",max_fa_in_hist,pre_post_now_acc_fa_in_hist);
	}

	/*memcpy(&priv->PSD_report_right[priv->mask_idx][0], PSD_report_right, 20*sizeof(int));
	memcpy(&priv->PSD_report_left[priv->mask_idx][0], PSD_report_left, 20*sizeof(int));

	for (i=0; i<20; i++) {
		priv->max_hold_right[i] = -1000;
		priv->max_hold_left[i] = -1000;
	}

	for (k=0; k<5; k++) {
		for (i=0; i<20; i++) {
			if (priv->PSD_report_right[k][i] > priv->max_hold_right[i])
				priv->max_hold_right[i] = priv->PSD_report_right[k][i];
			if (priv->PSD_report_left[k][i] > priv->max_hold_left[i])
				priv->max_hold_left[i] = priv->PSD_report_left[k][i];
		}
	}

	if ((priv->pshare->rf_ft_var.dfs_psd_op == 1) && (priv->idle_flag == 0))
	{
		int right_index_start, left_index_end;
		int avg_1 = (priv->max_hold_right[0]+priv->max_hold_right[1]+priv->max_hold_right[2])/3;
		int avg_2 = (priv->max_hold_right[17]+priv->max_hold_right[18]+priv->max_hold_right[19])/3;
		max_right = -1000;
		if(RTL_ABS(avg_1, avg_2) <= 3)
			right_index_start=2;
		else
			right_index_start=11;								
		for (i=right_index_start; i<20; i++) {
				if ((i != 10) && (max_right < priv->max_hold_right[i]))
					max_right = priv->max_hold_right[i];
		}

		avg_1 = (priv->max_hold_left[0]+priv->max_hold_left[1]+priv->max_hold_left[2])/3;
		avg_2 = (priv->max_hold_left[17]+priv->max_hold_left[18]+priv->max_hold_left[19])/3;
		max_left = -1000;
		if (RTL_ABS(avg_1, avg_2) <= 3)
			left_index_end=20;								
		else
			left_index_end=8;		
		for (i=0; i<left_index_end; i++) {
			if ((i != 10) && (max_left < priv->max_hold_left[i]))
				max_left = priv->max_hold_left[i];
		}
		if(max_right == 0){
			max_right = -99;
		}
		if(max_left == 0){
			max_left = -99;
		}
	}
	else
	{
		max_right = -1000;
		for (i=2; i<20; i++) {
			if ((i != 10) && (max_right < priv->max_hold_right[i]))
					max_right = priv->max_hold_right[i];
		}
		max_left = -1000;
		for (i=0; i<20; i++) {
			if ((i != 10) && (max_left < priv->max_hold_left[i]))
				max_left = priv->max_hold_left[i];
		}
		if(max_right == 0){
			max_right = -99;
		}
		if(max_left == 0){
			max_left = -99;
		}
	}
*/

	max_right = -99;
	max_left = -99;
	if (priv->pshare->rf_ft_var.dfs_det_print)
		panic_printk("max_right %d, max_left %d\n", max_right, max_left);

	/*if (priv->mask_idx >= priv->pshare->rf_ft_var.dfs_det_flag_offset)
		index = priv->mask_idx - priv->pshare->rf_ft_var.dfs_det_flag_offset;
	else
		index = priv->pshare->rf_ft_var.dfs_det_hist_len + priv->mask_idx - priv->pshare->rf_ft_var.dfs_det_flag_offset;*/
		
	// use PSD detection result
	if ((max_right > (0-(int)priv->pshare->rf_ft_var.dfs_psd_pw_th)) || (max_left > (0-(int)priv->pshare->rf_ft_var.dfs_psd_pw_th))) {
		if (priv->pshare->rf_ft_var.dfs_dpt_st_l2h_add) {
			st_L2H_tmp = 110 + ((max_right > max_left)? max_right:max_left) - priv->pshare->rf_ft_var.dfs_psd_fir_decay;
			if (st_L2H_tmp > priv->st_L2H_cur)
				st_L2H_new = st_L2H_tmp;
		}
		if (priv->pshare->rf_ft_var.dfs_det_print3)
			panic_printk("[3] st_L2H_new %x\n", st_L2H_new);

		if (priv->pshare->rf_ft_var.dfs_det_print)
			panic_printk("st_L2H_cur %x pwdb_th %x\n", st_L2H_new, priv->pwdb_th);

		fault_flag_psd = 1;
		if (priv->pulse_flag_hist[index] == 1){
			priv->pulse_flag_hist[index] = 0;
			if (priv->pshare->rf_ft_var.dfs_det_print4){
				panic_printk("Radar is masked : PSD mask\n");			
			}
		}
		priv->radar_det_mask_hist[index] = 1;
	}

	sum = 0;
	for (k=0; k<priv->pshare->rf_ft_var.dfs_det_hist_len; k++) {
		if (priv->radar_det_mask_hist[k] == 1)
			sum++;
	}

	if (priv->mask_hist_checked <= priv->pshare->rf_ft_var.dfs_det_hist_len)
		priv->mask_hist_checked++;

	//only the decision result of short pulse needs to refer the FA decision results
	if ((priv->mask_hist_checked >= priv->pshare->rf_ft_var.dfs_det_hist_len) &&
		//(DFS_tri_long_pulse || DFS_tri_short_pulse) &&
		priv->pulse_flag_hist[index])
	{		
		//if ((sum <= priv->pshare->rf_ft_var.dfs_det_sum_th) &&
		//	(fa_flag == 0))
		if (sum <= priv->pshare->rf_ft_var.dfs_det_sum_th) 
		{
//#if defined(CONFIG_WLAN_HAL_8814AE) || defined(CONFIG_WLAN_HAL_8822BE)
			if(((GET_CHIP_VER(priv) == VERSION_8814A) || (GET_CHIP_VER(priv) == VERSION_8822B)) &&
				priv->pshare->rf_ft_var.dfs_radar_diff_on){
				if (priv->pulse_type_hist[index] == 0){
					priv->radar_type = 0;
					if (priv->pulsewidth_flag && priv->pulseinterval_flag && priv->pri_type3_4_flag){
						priv->pmib->dot11DFSEntry.DFS_detected = 1 ; // DFS detect
						if (priv->pshare->rf_ft_var.dfs_det_print4){
							panic_printk("hist_idx= %d\n", (priv->hist_idx+3)%4);
							for (j=0;j<4;j++){
								for (i=0;i<6;i++){
									panic_printk("pluseinterval_hold = %d ", priv->pluseinterval_hold[j][i]);
								}
								panic_printk("\n");
							}			
							panic_printk("\n");
							for (j=0;j<4;j++){
								for (i=0;i<6;i++){
									panic_printk("plusewidth_hold = %d ", priv->plusewidth_hold[j][i]);
								}
								panic_printk("\n");
							}			
							panic_printk("\n");
							panic_printk("idle_flag = %d\n",priv->idle_flag);
							panic_printk("plusewidth_hold_sum = %d %d %d %d %d %d\n",
					priv->plusewidth_hold_sum[0], priv->plusewidth_hold_sum[1],priv->plusewidth_hold_sum[2],priv->plusewidth_hold_sum[3],priv->plusewidth_hold_sum[4],priv->plusewidth_hold_sum[5]);
							panic_printk("pluseinterval_hold_sum = %d %d %d %d %d %d\n",
					priv->pluseinterval_hold_sum[0], priv->pluseinterval_hold_sum[1],priv->pluseinterval_hold_sum[2],priv->pluseinterval_hold_sum[3],priv->pluseinterval_hold_sum[4],priv->pluseinterval_hold_sum[5]);
						}
					}
					else{
						if(priv->pshare->rf_ft_var.dfs_det_print4){
							if (priv->pulse_flag_hist[index] && (priv->pulseinterval_flag==0)){
								panic_printk("PRI criterion is not satisfied!\n");
								panic_printk("pulseinterval_variation = %d\n",priv->pri_std);
								if(priv->pri_cond1 == 0)
									panic_printk("pri_cond1 is not satisfied!\n");
								if(priv->pri_cond2 == 0)
									panic_printk("pri_cond2 is not satisfied!\n");
								if(priv->pri_cond3 == 0){
									panic_printk("pri_cond3 is not satisfied!\n");
								}
								if(priv->pri_cond4 == 0)
									panic_printk("pri_cond4 is not satisfied!\n");
								if(priv->pri_cond5 == 0)
									panic_printk("pri_cond5 is not satisfied!\n");
							}
							if (priv->pulse_flag_hist[index] && (priv->pulsewidth_flag==0)){
								panic_printk("PW criterion is not satisfied!\n");
								panic_printk("pulsewidth_variation = %d\n",priv->pw_std);
								if(priv->pw_cond1 == 0)
									panic_printk("pw_cond1 is not satisfied!\n");
								if(priv->pw_cond2 == 0)
									panic_printk("pw_cond2 is not satisfied!\n");
								if(priv->pw_cond3 == 0){
									panic_printk("pw_cond3 is not satisfied!\n");
								}
							}
							if (priv->pulse_flag_hist[index] && (priv->pri_type3_4_flag==0)){
								panic_printk("pri_type3_4 criterion is not satisfied!\n");
								if(priv->pri_type3_4_cond1 == 0)
									panic_printk("pri_type3_4_cond1 is not satisfied!\n");
								if(priv->pri_type3_4_cond2 == 0)
									panic_printk("pri_type3_4_cond2 is not satisfied!\n");
							}
							panic_printk("hist_idx= %d\n", (priv->hist_idx+3)%4);
							for (j=0;j<4;j++){
								for (i=0;i<6;i++){
									panic_printk("pluseinterval_hold = %d ", priv->pluseinterval_hold[j][i]);
								}
								panic_printk("\n");
							}			
							panic_printk("\n");
							for (j=0;j<4;j++){
								for (i=0;i<6;i++){
									panic_printk("plusewidth_hold = %d ", priv->plusewidth_hold[j][i]);
								}
								panic_printk("\n");
							}			
							panic_printk("\n");
							panic_printk("idle_flag = %d\n",priv->idle_flag);
							panic_printk("plusewidth_hold_sum = %d %d %d %d %d %d\n",
					priv->plusewidth_hold_sum[0], priv->plusewidth_hold_sum[1],priv->plusewidth_hold_sum[2],priv->plusewidth_hold_sum[3],priv->plusewidth_hold_sum[4],priv->plusewidth_hold_sum[5]);
							panic_printk("pluseinterval_hold_sum = %d %d %d %d %d %d\n",
					priv->pluseinterval_hold_sum[0], priv->pluseinterval_hold_sum[1],priv->pluseinterval_hold_sum[2],priv->pluseinterval_hold_sum[3],priv->pluseinterval_hold_sum[4],priv->pluseinterval_hold_sum[5]);
						}				
					}			
				}
				else{
					priv->radar_type = 1;
					/* Long radar should satisfy three conditions */
					if(priv->long_radar_flag == 1) {						
						priv->pmib->dot11DFSEntry.DFS_detected = 1 ; // DFS detect
						if (priv->pshare->rf_ft_var.dfs_det_print4) {
							panic_printk("\n");
							panic_printk("idle_flag = %d\n",priv->idle_flag);
							panic_printk("long_radar_plusewidth_hold_sum = %d %d %d %d %d %d\n",
							priv->plusewidth_long_hold_sum[0], priv->plusewidth_long_hold_sum[1],priv->plusewidth_long_hold_sum[2],priv->plusewidth_long_hold_sum[3],priv->plusewidth_long_hold_sum[4],priv->plusewidth_long_hold_sum[5]);
							panic_printk("long_radar_pluseinterval_hold_sum = %d %d %d %d %d %d\n",
							priv->pluseinterval_long_hold_sum[0], priv->pluseinterval_long_hold_sum[1],priv->pluseinterval_long_hold_sum[2],priv->pluseinterval_long_hold_sum[3],priv->pluseinterval_long_hold_sum[4],priv->pluseinterval_long_hold_sum[5]);
							panic_printk("\n");
						}
					} else {
						if (priv->pshare->rf_ft_var.dfs_det_print4){
							if(priv->pw_long_cond1 == 0)
								panic_printk("--pw_long_cond1 is not satisfied!--\n");
							if(priv->pw_long_cond2 == 0)
								panic_printk("--pw_long_cond2 is not satisfied!--\n");
							if(priv->pri_long_cond1 == 0)
								panic_printk("--pri_long_cond1 is not satisfied!--\n");
						}
					}
				}			
				
			}
#if defined(CONFIG_WLAN_HAL_8814BE)			
			else if((GET_CHIP_VER(priv) == VERSION_8814B) && priv->pshare->rf_ft_var.dfs_radar_diff_on) {
				if (priv->pulse_type_hist[index] == 0) {
					priv->radar_type = 0;
					if (priv->pulsewidth_flag && priv->pulseinterval_flag && priv->loct_flag) {
						priv->pmib->dot11DFSEntry.DFS_detected = 1 ; // DFS detect
						if (priv->pshare->rf_ft_var.dfs_det_print4) {
							panic_printk("Short radar pulse is detected\n");
							panic_printk("pw_rpt(1-8) = [%d, %d, %d, %d]\n",
										priv->pw_14b_rslt[0], priv->pw_14b_rslt[1], priv->pw_14b_rslt[2], priv->pw_14b_rslt[3]);
							panic_printk("loct_rpt(1-8) = [%d, %d, %d, %d, %d, %d, %d, %d]\n",
										priv->loct_14b_rslt[0], priv->loct_14b_rslt[1], priv->loct_14b_rslt[2], priv->loct_14b_rslt[3],
										priv->loct_14b_rslt[4], priv->loct_14b_rslt[5], priv->loct_14b_rslt[6], priv->loct_14b_rslt[7]);
							panic_printk("pri information = %d\n", priv->pri_14b_rslt);
							panic_printk("\n");
						}
					} else {
						if(priv->pshare->rf_ft_var.dfs_det_print4) {
							panic_printk("Suspected short radar result is dropped by SW mechanism\n");
							if (priv->pulse_flag_hist[index] && (priv->pulsewidth_flag==0)) {
								panic_printk("pw_14b_rslt = [%d, %d, %d, %d]\n",
										 priv->pw_14b_rslt[0],priv->pw_14b_rslt[1],priv->pw_14b_rslt[2],priv->pw_14b_rslt[3]);
								panic_printk("legal pw result is not enough\n");
								panic_printk("\n");
							}
							if (priv->pulse_flag_hist[index] && (priv->loct_flag==0)) {
								panic_printk("loct_rpt(1-8) = [%d, %d, %d, %d, %d, %d, %d, %d]\n",
										priv->loct_14b_rslt[0], priv->loct_14b_rslt[1], priv->loct_14b_rslt[2], priv->loct_14b_rslt[3],
										priv->loct_14b_rslt[4], priv->loct_14b_rslt[5], priv->loct_14b_rslt[6], priv->loct_14b_rslt[7]);
								panic_printk("The gap of location result is too much\n");
								panic_printk("\n");
							}
							if (priv->pulse_flag_hist[index] && (priv->pulseinterval_flag==0)) {
								panic_printk("PRI=%d isn't satisfied in the interval!\n",priv->pri_14b_rslt);
								panic_printk("\n");
							}
						}
					}
				}
				else {
					priv->radar_type = 1;
					if(priv->long_radar_flag == 1) {						
						priv->pmib->dot11DFSEntry.DFS_detected = 1 ; // DFS detect
						if (priv->pshare->rf_ft_var.dfs_det_print4) {
							panic_printk("Long radar pulse\n");
							panic_printk("pw_rpt(1-4) = [%d, %d, %d, %d]\n",
										priv->pw_14b_rslt[0], priv->pw_14b_rslt[1], priv->pw_14b_rslt[2], priv->pw_14b_rslt[3]);
							panic_printk("loct_rpt(1-8) = [%d, %d, %d, %d, %d, %d, %d, %d]\n",
										priv->loct_14b_rslt[0], priv->loct_14b_rslt[1], priv->loct_14b_rslt[2], priv->loct_14b_rslt[3],
										priv->loct_14b_rslt[4], priv->loct_14b_rslt[5], priv->loct_14b_rslt[6], priv->loct_14b_rslt[7]);
							panic_printk("pri information = %d\n", priv->pri_14b_rslt);
							panic_printk("\n");
						}
					} else {
						if (priv->pshare->rf_ft_var.dfs_det_print4) {
							panic_printk("Suspected long radar result is dropped by SW mechanism\n");
							if (priv->pulse_flag_hist[index] && (priv->pulsewidth_flag==0)) {
								panic_printk("pw_rpt(1-4) = [%d, %d, %d, %d]\n",
										priv->pw_14b_rslt[0], priv->pw_14b_rslt[1], priv->pw_14b_rslt[2], priv->pw_14b_rslt[3]);
								panic_printk("None of PWs are in the interval\n");
								panic_printk("\n");
							}
							if (priv->pulse_flag_hist[index] && (priv->loct_flag==0)) {
								panic_printk("loct_rpt(1-8) = [%d, %d, %d, %d, %d, %d, %d, %d]\n",
										priv->loct_14b_rslt[0], priv->loct_14b_rslt[1], priv->loct_14b_rslt[2], priv->loct_14b_rslt[3],
										priv->loct_14b_rslt[4], priv->loct_14b_rslt[5], priv->loct_14b_rslt[6], priv->loct_14b_rslt[7]);
								panic_printk("The gap of location result is too much\n");
								panic_printk("\n");
							}
							if (priv->pulse_flag_hist[index] && (priv->pulseinterval_flag==0)) {
								panic_printk("PRI = %d, which is not zero!\n",priv->pri_14b_rslt);
								panic_printk("\n");
							}
						}
					}
				}
			}
#endif
#if defined(CONFIG_WLAN_HAL_8812FE)
			else if((GET_CHIP_VER(priv) == VERSION_8812F) && priv->pshare->rf_ft_var.dfs_radar_diff_on) {
				if (priv->pulse_type_hist[index] == 0) {
					priv->radar_type = 0;
					if (priv->pulsewidth_flag && priv->pulseinterval_flag && priv->loct_flag) {
						priv->pmib->dot11DFSEntry.DFS_detected = 1 ; // DFS detect
						if (priv->pshare->rf_ft_var.dfs_det_print4) {
							panic_printk("Short radar pulse is detected\n");
							panic_printk("pw_rpt(1-12) = [%d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d]\n",
									 priv->pw_12f_rslt[0],priv->pw_12f_rslt[1],priv->pw_12f_rslt[2],priv->pw_12f_rslt[3],
									 priv->pw_12f_rslt[4],priv->pw_12f_rslt[5],priv->pw_12f_rslt[6],priv->pw_12f_rslt[7],
									 priv->pw_12f_rslt[8],priv->pw_12f_rslt[9],priv->pw_12f_rslt[10],priv->pw_12f_rslt[11]);
							panic_printk("loct_rpt(1-12) = [%d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d]\n",
										priv->loct_12f_rslt[0], priv->loct_12f_rslt[1], priv->loct_12f_rslt[2], priv->loct_12f_rslt[3],
										priv->loct_12f_rslt[4], priv->loct_12f_rslt[5], priv->loct_12f_rslt[6], priv->loct_12f_rslt[7],
										priv->loct_12f_rslt[8], priv->loct_12f_rslt[9], priv->loct_12f_rslt[10], priv->loct_12f_rslt[11]);
							panic_printk("pri_rpt(1-12) = [%d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d]\n",
										priv->pri_12f_rslt[0], priv->pri_12f_rslt[1], priv->pri_12f_rslt[2], priv->pri_12f_rslt[3],
										priv->pri_12f_rslt[4], priv->pri_12f_rslt[5], priv->pri_12f_rslt[6], priv->pri_12f_rslt[7],
										priv->pri_12f_rslt[8], priv->pri_12f_rslt[9], priv->pri_12f_rslt[10], priv->pri_12f_rslt[11]);
							panic_printk("\n");
						}
					} else {
						if(priv->pshare->rf_ft_var.dfs_det_print4) {
							panic_printk("Suspected short radar result is dropped by SW mechanism\n");
							panic_printk("pulsewidth_flag = %d\n",priv->pulsewidth_flag);
							panic_printk("loct_flag = %d\n",priv->loct_flag);
							panic_printk("pulseinterval_flag = %d\n",priv->pulseinterval_flag);
							panic_printk("\n");
							if (priv->pulse_flag_hist[index] && (priv->pulsewidth_flag==0)) {
								panic_printk("pw_rpt(1-12) = [%d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d]\n",
										 priv->pw_12f_rslt[0],priv->pw_12f_rslt[1],priv->pw_12f_rslt[2],priv->pw_12f_rslt[3],
										 priv->pw_12f_rslt[4],priv->pw_12f_rslt[5],priv->pw_12f_rslt[6],priv->pw_12f_rslt[7],
										 priv->pw_12f_rslt[8],priv->pw_12f_rslt[9],priv->pw_12f_rslt[10],priv->pw_12f_rslt[11]);
								panic_printk("legal pw result is not enough!\n");
								panic_printk("\n");
							}
							if (priv->pulse_flag_hist[index] && (priv->loct_flag==0)) {
								panic_printk("loct_rpt(1-12) = [%d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d]\n",
										priv->loct_12f_rslt[0], priv->loct_12f_rslt[1], priv->loct_12f_rslt[2], priv->loct_12f_rslt[3],
										priv->loct_12f_rslt[4], priv->loct_12f_rslt[5], priv->loct_12f_rslt[6], priv->loct_12f_rslt[7],
										priv->loct_12f_rslt[8], priv->loct_12f_rslt[9], priv->loct_12f_rslt[10], priv->loct_12f_rslt[11]);
								panic_printk("The gap of location result is too much!\n");
								panic_printk("\n");
							}
							if (priv->pulse_flag_hist[index] && (priv->pulseinterval_flag==0)) {
								panic_printk("pri_rpt(1-12) = [%d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d]\n",
										priv->pri_12f_rslt[0], priv->pri_12f_rslt[1], priv->pri_12f_rslt[2], priv->pri_12f_rslt[3],
										priv->pri_12f_rslt[4], priv->pri_12f_rslt[5], priv->pri_12f_rslt[6], priv->pri_12f_rslt[7],
										priv->pri_12f_rslt[8], priv->pri_12f_rslt[9], priv->pri_12f_rslt[10], priv->pri_12f_rslt[11]);
								panic_printk("Legal PRI result is not enough!\n");
								panic_printk("\n");
							}
						}
					}
				}
				else {
					priv->radar_type = 1;
					if(priv->long_radar_flag == 1) {						
						priv->pmib->dot11DFSEntry.DFS_detected = 1 ; // DFS detect
						if (priv->pshare->rf_ft_var.dfs_det_print4) {
							panic_printk("Long radar pulse is detected\n");
							panic_printk("\n");
						}
					} else {
						if (priv->pshare->rf_ft_var.dfs_det_print4) {
							panic_printk("Suspected long radar result is dropped by SW mechanism\n");
							if (priv->pulse_flag_hist[index] && (priv->pulsewidth_flag==0)) {
								panic_printk("pw_rpt(1-12) = [%d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d]\n",
										priv->pw_12f_rslt[0], priv->pw_12f_rslt[1], priv->pw_12f_rslt[2], priv->pw_12f_rslt[3],
										priv->pw_12f_rslt[4], priv->pw_12f_rslt[5], priv->pw_12f_rslt[6], priv->pw_12f_rslt[7],
										priv->pw_12f_rslt[8], priv->pw_12f_rslt[9], priv->pw_12f_rslt[10], priv->pw_12f_rslt[11]);
								panic_printk("\n");
							}
							if (priv->pulse_flag_hist[index] && (priv->loct_flag==0)) {
								panic_printk("loct_rpt(1-12) = [%d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d]\n",
										priv->loct_12f_rslt[0], priv->loct_12f_rslt[1], priv->loct_12f_rslt[2], priv->loct_12f_rslt[3],
										priv->loct_12f_rslt[4], priv->loct_12f_rslt[5], priv->loct_12f_rslt[6], priv->loct_12f_rslt[7],
										priv->loct_12f_rslt[8], priv->loct_12f_rslt[9], priv->loct_12f_rslt[10], priv->loct_12f_rslt[11]);
								panic_printk("\n");
							}
							if (priv->pulse_flag_hist[index] && (priv->pulseinterval_flag==0)) {
								panic_printk("pri_rpt(1-12) = [%d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d]\n",
										priv->pri_12f_rslt[0], priv->pri_12f_rslt[1], priv->pri_12f_rslt[2], priv->pri_12f_rslt[3],
										priv->pri_12f_rslt[4], priv->pri_12f_rslt[5], priv->pri_12f_rslt[6], priv->pri_12f_rslt[7],
										priv->pri_12f_rslt[8], priv->pri_12f_rslt[9], priv->pri_12f_rslt[10], priv->pri_12f_rslt[11]);
								panic_printk("\n");
							}
						}
					}
				}
			}
#endif			
			else
			{
				if (priv->pulse_type_hist[index] == 0)
					priv->radar_type = 0;
				else if (priv->pulse_type_hist[index] == 1)
					priv->radar_type = 1;				
				priv->pmib->dot11DFSEntry.DFS_detected = 1 ; // DFS detect
			}
		}
		else {
			fault_flag_det = 1;
			if (priv->pshare->rf_ft_var.dfs_det_print4){
				panic_printk("Radar is masked : mask_hist large than thd\n");			
			}
		}
	}

	priv->mask_idx++;
	if (priv->mask_idx == priv->pshare->rf_ft_var.dfs_det_hist_len)
		priv->mask_idx = 0;

	if ((fault_flag_det == 0) && (fault_flag_psd == 0) && (fa_flag ==0)) {
		//if (total_pulse_count_inc < priv->pshare->rf_ft_var.dfs_dpt_pulse_th_lower) {
		//	// limit the ST value to absoulte lower bound 0x1c
		//	st_L2H_new -= 4;
		//	if (priv->pshare->rf_ft_var.dfs_det_print3)
		//		panic_printk("[4] st_L2H_new %x\n", st_L2H_new);
		//}
		///*else if (total_pulse_count_inc < priv->pshare->rf_ft_var.dfs_dpt_pulse_th_mid) {
		//	// limit the ST value to absoulte lower bound 0x1c
		//	st_L2H_new -= 2;
		//	if (priv->pshare->rf_ft_var.dfs_det_print3)
		//		panic_printk("[5] st_L2H_new %x\n", st_L2H_new);
		//}*/
		//else if (ini_gain_inc < 0){
		//	st_L2H_new += ini_gain_inc;
		//	if (priv->pshare->rf_ft_var.dfs_det_print3)
		//		panic_printk("[6] st_L2H_new %x\n", st_L2H_new);
		//}
		///*else if ((total_pulse_count_inc < max_sht_pusle_cnt_th) &&
		//		 (((priv->ext_stats.tx_avarage+priv->ext_stats.rx_avarage)>>17) <= priv->pshare->rf_ft_var.dfs_psd_tp_th)) {
		//	st_L2H_new += priv->pshare->rf_ft_var.dfs_dpt_st_l2h_idle_th;
		//}*/
		if (priv->ini_gain_cur < priv->pshare->rf_ft_var.dpt_ini_gain_th) {
			if ((priv->idle_flag == 1) && (priv->pshare->rf_ft_var.manual_dfs_regdomain == 2))
				st_L2H_new = priv->pshare->rf_ft_var.dfs_dpt_st_l2h_min + priv->pshare->rf_ft_var.dfs_dpt_st_l2h_idle_offset;
			else
				st_L2H_new = priv->pshare->rf_ft_var.dfs_dpt_st_l2h_min;
		}
	 }	
	else{
		
		
		if (priv->pshare->rf_ft_var.dfs_dpt_st_l2h_add)
			st_L2H_new += 2;
		if (priv->pshare->rf_ft_var.dfs_det_print3)
			panic_printk("[7] st_L2H_new %x\n", st_L2H_new);
		
		
		if (DFS_tri_short_pulse) {
			//RTL_W32(0x920, RTL_R32(0x920) | (BIT(24) | BIT(28)));
			//RTL_W32(0x920, RTL_R32(0x920) & ~(BIT(24) | BIT(28)));
			//RTL_W32(0x924, RTL_R32(0x924) & ~BIT(15));
			//RTL_W32(0x924, RTL_R32(0x924) | BIT(15));
		}
		if (DFS_tri_long_pulse) {
			//RTL_W32(0x920, RTL_R32(0x920) | (BIT(25) | BIT(28)));
			//RTL_W32(0x920, RTL_R32(0x920) & ~(BIT(25) | BIT(28)));
			//RTL_W32(0x924, RTL_R32(0x924) & ~BIT(15));
			//RTL_W32(0x924, RTL_R32(0x924) | BIT(15));
		}
	}

	if(priv->pmib->dot11nConfigEntry.dot11nUse40M == CHANNEL_WIDTH_20){
		if(priv->pshare->highTP_found_pstat != NULL){
			if(priv->pshare->highTP_found_pstat->rf_info.mimorssi[0] >= 20 && priv->pshare->highTP_found_pstat->rf_info.mimorssi[0] <= 30)
				st_L2H_new = priv->pshare->rf_ft_var.dfs_dpt_st_l2h_min - 19 + priv->pshare->highTP_found_pstat->rf_info.mimorssi[0];
		}
	}

	if ((st_L2H_new != priv->st_L2H_cur)) {
		// limit the ST value to absoulte lower bound 0x22
		// limit the ST value to absoulte upper bound 0x4e
		if (st_L2H_new < priv->pshare->rf_ft_var.dfs_dpt_st_l2h_min) {
			if ((priv->idle_flag == 1) && (priv->pshare->rf_ft_var.manual_dfs_regdomain == 2))
				priv->st_L2H_cur = priv->pshare->rf_ft_var.dfs_dpt_st_l2h_min + priv->pshare->rf_ft_var.dfs_dpt_st_l2h_idle_offset;
			else
				priv->st_L2H_cur = priv->pshare->rf_ft_var.dfs_dpt_st_l2h_min;
			//priv->st_L2H_cur = priv->pshare->rf_ft_var.dfs_dpt_st_l2h_min;
		}
		else if (st_L2H_new > priv->pshare->rf_ft_var.dfs_dpt_st_l2h_max)
			priv->st_L2H_cur = priv->pshare->rf_ft_var.dfs_dpt_st_l2h_max;
		else
			priv->st_L2H_cur = st_L2H_new;
		RTL_W8(0x91c, priv->st_L2H_cur);

		priv->pwdb_th = ((int)priv->st_L2H_cur - (int)priv->ini_gain_cur)/2 + priv->pshare->rf_ft_var.dfs_pwdb_scalar_factor;
		priv->pwdb_th = MAX_NUM(priv->pwdb_th, (int)priv->pshare->rf_ft_var.dfs_pwdb_th); // limit the pwdb value to absoulte lower bound 0xa
		priv->pwdb_th = MIN_NUM(priv->pwdb_th, 0x1f);	 // limit the pwdb value to absoulte upper bound 0x1f
		if ((GET_CHIP_VER(priv) == VERSION_8814B) || (GET_CHIP_VER(priv) == VERSION_8812F)) {
			phy_set_bb_reg(priv, 0xa50, 0x000000f0, priv->pwdb_th);
		} else {
			phy_set_bb_reg(priv, 0x918, 0x00001f00, priv->pwdb_th);
		}
	}

	if (priv->pshare->rf_ft_var.dfs_det_print2) {
		panic_printk("fault_flag_det[%d], fault_flag_psd[%d], DFS_detected [%d]\n",fault_flag_det, fault_flag_psd, priv->pmib->dot11DFSEntry.DFS_detected );
	}
	
	priv->FA_count_inc_pre = FA_count_inc;
#if defined(CONFIG_WLAN_HAL_8814BE)
	if ((GET_CHIP_VER(priv) == VERSION_8814B)) {
		if (priv->seg1_flag == 1) {
			reg_dfsrpt_value = phy_query_bb_reg(priv, 0x2e20, 0xffffffff);
			if ((reg_dfsrpt_value & BIT(20)) || (reg_dfsrpt_value & BIT(28))) {
				odm_set_bb_reg(dm, 0xa6c, BIT(15), 0);
				odm_set_bb_reg(dm, 0xa6c, BIT(15), 1);
			}
		} else {
			reg_dfsrpt_value = phy_query_bb_reg(priv, 0x2e00, 0xffffffff);
			if ((reg_dfsrpt_value & BIT(20)) || (reg_dfsrpt_value & BIT(28))) {
				odm_set_bb_reg(dm, 0xa40, BIT(15), 0);
				odm_set_bb_reg(dm, 0xa40, BIT(15), 1);
			}
		}
	} else
#endif
	if ((GET_CHIP_VER(priv) == VERSION_8812F)) {
		reg_dfsrpt_value = phy_query_bb_reg(priv, 0x2e00, 0xffffffff);
		if ((reg_dfsrpt_value & BIT(20)) || (reg_dfsrpt_value & BIT(28))) {
			odm_set_bb_reg(dm, 0xa40, BIT(15), 0);
			odm_set_bb_reg(dm, 0xa40, BIT(15), 1);
		}
	} else {
		if (phy_query_bb_reg(priv, 0xf98, BIT(17)) || phy_query_bb_reg(priv, 0xf98, BIT(19))) {
			RTL_W32(0x924, RTL_R32(0x924) & ~BIT(15));
			RTL_W32(0x924, RTL_R32(0x924) | BIT(15));
		}
	}
	
}



void rtl8192cd_dfs_dynamic_setting(struct rtl8192cd_priv *priv)
{
	unsigned char peak_th_cur=0, short_pulse_cnt_th_cur=0, long_pulse_cnt_th_cur=0, three_peak_opt_cur=0, three_peak_th2_cur=0;
	unsigned char peak_window_cur=0, nb2wb_th_cur=0;
	
	if ((priv->idle_flag == 1)) { // idle (no traffic)
		peak_th_cur = 3;
		short_pulse_cnt_th_cur = 6;
		long_pulse_cnt_th_cur = 9;
		peak_window_cur = 2;
		three_peak_opt_cur = 0;
		three_peak_th2_cur = 2;
		// MKK
		if(priv->pshare->rf_ft_var.manual_dfs_regdomain == 2){
			if ((priv->pmib->dot11RFEntry.dot11channel >= 52) &&
			(priv->pmib->dot11RFEntry.dot11channel <= 64)) {
				short_pulse_cnt_th_cur = priv->pshare->rf_ft_var.dfs_pc0_th_idle_w53;
				long_pulse_cnt_th_cur = 15;
				three_peak_th2_cur = 0;                
			}
			else {
				short_pulse_cnt_th_cur = priv->pshare->rf_ft_var.dfs_pc0_th_idle_w56;
				three_peak_th2_cur = 0;
				long_pulse_cnt_th_cur = 10;
			}
		}
		// FCC
		else if(priv->pshare->rf_ft_var.manual_dfs_regdomain == 1){
			three_peak_th2_cur = 0;
		}
		// ETSI
		else if(priv->pshare->rf_ft_var.manual_dfs_regdomain == 3){
			long_pulse_cnt_th_cur = 15;
			if((priv->ch_120_132_CAC_end == 0) && IS_METEOROLOGY_CHANNEL(priv->pmib->dot11RFEntry.dot11channel)){
				peak_th_cur = 2;
				three_peak_th2_cur = 0;	
				short_pulse_cnt_th_cur = 7;
			}
			else{
				three_peak_th2_cur = 0;	
				short_pulse_cnt_th_cur = 7;
			}
		}
		else{  // default: FCC
		}
	}
	else{ // in service (with TP)
		peak_th_cur = 2;
		short_pulse_cnt_th_cur = 6;
		long_pulse_cnt_th_cur = 7;
		peak_window_cur = 2;
		three_peak_opt_cur = 0;
		three_peak_th2_cur = 2;
		// MKK
		if(priv->pshare->rf_ft_var.manual_dfs_regdomain == 2){
			if ((priv->pmib->dot11RFEntry.dot11channel >= 52) &&
			(priv->pmib->dot11RFEntry.dot11channel <= 64)) {
				long_pulse_cnt_th_cur = 15;
				short_pulse_cnt_th_cur = 5; // for high duty cycle
				three_peak_th2_cur = 0;			
			}
			else {
				three_peak_opt_cur = 0;
				three_peak_th2_cur = 0;
				long_pulse_cnt_th_cur = 8;
			}
		}
		// FCC
		else if(priv->pshare->rf_ft_var.manual_dfs_regdomain == 1){
		}
		// ETSI
		else if(priv->pshare->rf_ft_var.manual_dfs_regdomain == 3){
			long_pulse_cnt_th_cur = 15;
			short_pulse_cnt_th_cur = 5;
				three_peak_opt_cur = 0;
		}
		else{
		}
	}

	if ((GET_CHIP_VER(priv) == VERSION_8814B) || (GET_CHIP_VER(priv) == VERSION_8812F)) {
		if ((priv->idle_flag == 1)) {
			short_pulse_cnt_th_cur = 8;	// Real cnt in 8812F
			long_pulse_cnt_th_cur = 6;	// Real cnt in 8812F
			// [8812F] a50[23:20] = N => The short_pulse_cnt_th is N+2
			short_pulse_cnt_th_cur = short_pulse_cnt_th_cur - 2;
			long_pulse_cnt_th_cur = long_pulse_cnt_th_cur - 2;
		} else {
			short_pulse_cnt_th_cur = 6;	// Real cnt in 8812F
			long_pulse_cnt_th_cur = 4;	// Real cnt in 8812F
			// In 8812F, a50[23:20] = N => The short_pulse_cnt_th is N+2
			short_pulse_cnt_th_cur = short_pulse_cnt_th_cur - 2;
			long_pulse_cnt_th_cur = long_pulse_cnt_th_cur - 2;
		}	
	}

	
	if ((GET_CHIP_VER(priv) == VERSION_8814B) || (GET_CHIP_VER(priv) == VERSION_8812F)) {
		if((priv->peak_th != peak_th_cur)){		
			phy_set_bb_reg(priv, 0xa48, 0x00c00000, peak_th_cur);
		}
		if((priv->short_pulse_cnt_th != short_pulse_cnt_th_cur)){		
			phy_set_bb_reg(priv, 0xa50, 0x00f00000, short_pulse_cnt_th_cur);
		}
		if((priv->long_pulse_cnt_th != long_pulse_cnt_th_cur)){		
			phy_set_bb_reg(priv, 0xa4c, 0xf0000000, long_pulse_cnt_th_cur);
		}
		if((priv->peak_window != peak_window_cur)){		
			phy_set_bb_reg(priv, 0xa40, 0x00030000, peak_window_cur);
		}
		/*if((priv->three_peak_opt != three_peak_opt_cur)){		
			phy_set_bb_reg(priv, 0xa40, 0x30000000, three_peak_opt_cur);
		}*/
		if((priv->three_peak_th2 != three_peak_th2_cur)){		
			phy_set_bb_reg(priv, 0xa44, 0x00000007, three_peak_th2_cur);
		}
	} else {
		if((priv->peak_th != peak_th_cur)){		
			phy_set_bb_reg(priv, 0x918, 0x00030000, peak_th_cur);
		}
		if((priv->short_pulse_cnt_th != short_pulse_cnt_th_cur)){		
			phy_set_bb_reg(priv, 0x920, 0x000f0000, short_pulse_cnt_th_cur);
		}
		if((priv->long_pulse_cnt_th != long_pulse_cnt_th_cur)){		
			phy_set_bb_reg(priv, 0x920, 0x00f00000, long_pulse_cnt_th_cur);
		}
		if((priv->peak_window != peak_window_cur)){		
			phy_set_bb_reg(priv, 0x920, 0x00000300, peak_window_cur);
		}
		if((priv->nb2wb_th != nb2wb_th_cur)){		
			phy_set_bb_reg(priv, 0x920, 0x0000e000, nb2wb_th_cur);
		}
		if((priv->three_peak_opt != three_peak_opt_cur)){		
			phy_set_bb_reg(priv, 0x924, 0x00000180, three_peak_opt_cur);
		}
		if((priv->three_peak_th2 != three_peak_th2_cur)){		
			phy_set_bb_reg(priv, 0x924, 0x00007000, three_peak_th2_cur);
		}
	}
	
    	priv->peak_th = peak_th_cur;
	priv->short_pulse_cnt_th = short_pulse_cnt_th_cur;
	priv->long_pulse_cnt_th = long_pulse_cnt_th_cur;
	priv->peak_window = peak_window_cur;
	priv->nb2wb_th = nb2wb_th_cur;
	priv->three_peak_opt = three_peak_opt_cur;
	priv->three_peak_th2 = three_peak_th2_cur;
}

#if defined(CONFIG_WLAN_HAL_8814AE) || defined(CONFIG_WLAN_HAL_8822BE)

void rtl8192cd_radar_type_differentiation(struct rtl8192cd_priv *priv)
{
	struct dm_struct *dm = &(priv->pshare->_dmODM);
	unsigned char i, need_reset, g_ti_cur[16], g_pw_cur[6], g_pri_cur[6], g_ti_inc[16], g_pw_inc[6], g_pri_inc[6];	// ti = tone index, pw = pulse width, pri = pulse repetition interval
	unsigned int g_total, g0_ratio, g1_ratio, g2_ratio, g3_ratio, g4_ratio, g5_ratio;
	unsigned int regf98_value, reg2e00_value, reg2e20_value;
	unsigned char DFS_tri_short_pulse=0, DFS_tri_long_pulse=0, short_pulse_th;
	unsigned char dfs_plusewidth_thd1, dfs_plusewidth_thd2, dfs_plusewidth_thd3, dfs_plusewidth_thd4, dfs_plusewidth_thd5;
	unsigned int dfs_hist1_peak_index, dfs_hist2_peak_index, dfs_hist1_pulse_width, dfs_hist2_pulse_width, dfs_hist1_pulse_rep_intvl, dfs_hist2_pulse_rep_intvl;
	unsigned char g_plusewidth[6],j,g_peakindex[16];
	unsigned int g_plusewidth_total,g_peakindex_total,g_mask_32;

#if defined(CONFIG_WLAN_HAL_8814BE)
	if ((GET_CHIP_VER(priv) == VERSION_8814B)) {
		if (priv->seg1_flag == 1) {
			reg2e20_value = phy_query_bb_reg(priv, 0x2e20, 0xffffffff);
			DFS_tri_short_pulse = (reg2e20_value & BIT(20)) ? 1 : 0;
			DFS_tri_long_pulse = (reg2e20_value & BIT(28)) ? 1 : 0;
		} else {
			reg2e00_value = phy_query_bb_reg(priv, 0x2e00, 0xffffffff);
			DFS_tri_short_pulse = (reg2e00_value & BIT(20)) ? 1 : 0;
			DFS_tri_long_pulse = (reg2e00_value & BIT(28)) ? 1 : 0;
		}
		short_pulse_th = phy_query_bb_reg(priv, 0xa50, 0x00f00000);
	} else
#endif
	if ((GET_CHIP_VER(priv) == VERSION_8812F)) {
		reg2e00_value = phy_query_bb_reg(priv, 0x2e00, 0xffffffff);
		DFS_tri_short_pulse = (reg2e00_value & BIT(20)) ? 1 : 0;
		DFS_tri_long_pulse = (reg2e00_value & BIT(28)) ? 1 : 0;
		short_pulse_th = phy_query_bb_reg(priv, 0xa50, 0x00f00000);
	} else {
		regf98_value = phy_query_bb_reg(priv, 0xf98, 0xffffffff);
		DFS_tri_short_pulse = (regf98_value & BIT(17))? 1 : 0;
		DFS_tri_long_pulse = (regf98_value & BIT(19))? 1 : 0;
		short_pulse_th = phy_query_bb_reg(priv, 0x920, 0x000f0000);
	}

	if (!((GET_CHIP_VER(priv) == VERSION_8814B) || (GET_CHIP_VER(priv) == VERSION_8812F))) {
		phy_set_bb_reg(priv, 0x19b8, 0x40, 0);  // switch 0xf5c & 0xf74 to DFS report
		// read peak index hist report
		phy_set_bb_reg(priv, 0x19e4, 0x00c00000, 0);  // report selection = 0 (peak index)
	}

	/*g_ti_cur[0] = phy_query_bb_reg(priv, 0xf74, 0xf0000000);
	if (g_ti_cur[0] >= priv->g_ti_pre[0])
		g_ti_inc[0] = g_ti_cur[0] - priv->g_ti_pre[0];
	else
		g_ti_inc[0] = g_ti_cur[0];
	priv->g_ti_pre[0] = g_ti_cur[0];
	
	g_ti_cur[1] = phy_query_bb_reg(priv, 0xf74, 0x0f000000);
	if (g_ti_cur[1] >= priv->g_ti_pre[1])
		g_ti_inc[1] = g_ti_cur[1] - priv->g_ti_pre[1];
	else
		g_ti_inc[1] = g_ti_cur[1];
	priv->g_ti_pre[1] = g_ti_cur[1];
	
	g_ti_cur[2] = phy_query_bb_reg(priv, 0xf74, 0x00f00000);
	if (g_ti_cur[2] >= priv->g_ti_pre[2])
		g_ti_inc[2] = g_ti_cur[2] - priv->g_ti_pre[2];
	else
		g_ti_inc[2] = g_ti_cur[2];
	priv->g_ti_pre[2] = g_ti_cur[2];
	
	g_ti_cur[3] = phy_query_bb_reg(priv, 0xf74, 0x000f0000);
	if (g_ti_cur[3] >= priv->g_ti_pre[3])
		g_ti_inc[3] = g_ti_cur[3] - priv->g_ti_pre[3];
	else
		g_ti_inc[3] = g_ti_cur[3];
	priv->g_ti_pre[3] = g_ti_cur[3];
	
	g_ti_cur[4] = phy_query_bb_reg(priv, 0xf74, 0x0000f000);
	if (g_ti_cur[4] >= priv->g_ti_pre[4])
		g_ti_inc[4] = g_ti_cur[4] - priv->g_ti_pre[4];
	else
		g_ti_inc[4] = g_ti_cur[4];
	priv->g_ti_pre[4] = g_ti_cur[4];
	
	g_ti_cur[5] = phy_query_bb_reg(priv, 0xf74, 0x00000f00);
	if (g_ti_cur[5] >= priv->g_ti_pre[5])
		g_ti_inc[5] = g_ti_cur[5] - priv->g_ti_pre[5];
	else
		g_ti_inc[5] = g_ti_cur[5];
	priv->g_ti_pre[5] = g_ti_cur[5];
	
	g_ti_cur[6] = phy_query_bb_reg(priv, 0xf74, 0x000000f0);
	if (g_ti_cur[6] >= priv->g_ti_pre[6])
		g_ti_inc[6] = g_ti_cur[6] - priv->g_ti_pre[6];
	else
		g_ti_inc[6] = g_ti_cur[6];
	priv->g_ti_pre[6] = g_ti_cur[6];
	
	g_ti_cur[7] = phy_query_bb_reg(priv, 0xf74, 0x0000000f);
	if (g_ti_cur[7] >= priv->g_ti_pre[7])
		g_ti_inc[7] = g_ti_cur[7] - priv->g_ti_pre[7];
	else
		g_ti_inc[7] = g_ti_cur[7];
	priv->g_ti_pre[7] = g_ti_cur[7];
	
	g_ti_cur[8] = phy_query_bb_reg(priv, 0xf5c, 0xf0000000);
	if (g_ti_cur[8] >= priv->g_ti_pre[8])
		g_ti_inc[8] = g_ti_cur[8] - priv->g_ti_pre[8];
	else
		g_ti_inc[8] = g_ti_cur[8];
	priv->g_ti_pre[8] = g_ti_cur[8];
	
	g_ti_cur[9] = phy_query_bb_reg(priv, 0xf5c, 0x0f000000);
	if (g_ti_cur[9] >= priv->g_ti_pre[9])
		g_ti_inc[9] = g_ti_cur[9] - priv->g_ti_pre[9];
	else
		g_ti_inc[9] = g_ti_cur[9];
	priv->g_ti_pre[9] = g_ti_cur[9];
	
	g_ti_cur[10] = phy_query_bb_reg(priv, 0xf5c, 0x00f00000);
	if (g_ti_cur[10] >= priv->g_ti_pre[10])
		g_ti_inc[10] = g_ti_cur[10] - priv->g_ti_pre[10];
	else
		g_ti_inc[10] = g_ti_cur[10];
	priv->g_ti_pre[10] = g_ti_cur[10];
	
	g_ti_cur[11] = phy_query_bb_reg(priv, 0xf5c, 0x000f0000);
	if (g_ti_cur[11] >= priv->g_ti_pre[0])
		g_ti_inc[11] = g_ti_cur[11] - priv->g_ti_pre[11];
	else
		g_ti_inc[11] = g_ti_cur[11];
	priv->g_ti_pre[11] = g_ti_cur[11];
	
	g_ti_cur[12] = phy_query_bb_reg(priv, 0xf5c, 0x0000f000);
	if (g_ti_cur[12] >= priv->g_ti_pre[12])
		g_ti_inc[12] = g_ti_cur[12] - priv->g_ti_pre[12];
	else
		g_ti_inc[12] = g_ti_cur[12];
	priv->g_ti_pre[12] = g_ti_cur[12];
	
	g_ti_cur[13] = phy_query_bb_reg(priv, 0xf5c, 0x00000f00);
	if (g_ti_cur[13] >= priv->g_ti_pre[13])
		g_ti_inc[13] = g_ti_cur[13] - priv->g_ti_pre[13];
	else
		g_ti_inc[13] = g_ti_cur[13];
	priv->g_ti_pre[13] = g_ti_cur[13];
	
	g_ti_cur[14] = phy_query_bb_reg(priv, 0xf5c, 0x000000f0);
	if (g_ti_cur[14] >= priv->g_ti_pre[14])
		g_ti_inc[14] = g_ti_cur[14] - priv->g_ti_pre[14];
	else
		g_ti_inc[14] = g_ti_cur[14];
	priv->g_ti_pre[14] = g_ti_cur[14];
	
	g_ti_cur[15] = phy_query_bb_reg(priv, 0xf5c, 0x0000000f);
	if (g_ti_cur[15] >= priv->g_ti_pre[15])
		g_ti_inc[15] = g_ti_cur[15] - priv->g_ti_pre[15];
	else
		g_ti_inc[15] = g_ti_cur[15];
	priv->g_ti_pre[15] = g_ti_cur[15];

	// read pulse width hist report
	phy_set_bb_reg(priv, 0x19e4, 0x00c00000, 1);  // report selection = 1 (pulse width)

	g_pw_cur[0] = phy_query_bb_reg(priv, 0xf74, 0xff000000);
	if (g_pw_cur[0] >= priv->g_pw_pre[0])
		g_pw_inc[0] = g_pw_cur[0] - priv->g_pw_pre[0];
	else
		g_pw_inc[0] = g_pw_cur[0];
	priv->g_pw_pre[0] = g_pw_cur[0];
	
	g_pw_cur[1] = phy_query_bb_reg(priv, 0xf74, 0x00ff0000);
	if (g_pw_cur[1] >= priv->g_pw_pre[1])
		g_pw_inc[1] = g_pw_cur[1] - priv->g_pw_pre[1];
	else
		g_pw_inc[1] = g_pw_cur[1];
	priv->g_pw_pre[1] = g_pw_cur[1];
	
	g_pw_cur[2] = phy_query_bb_reg(priv, 0xf74, 0x0000ff00);
	if (g_pw_cur[2] >= priv->g_pw_pre[2])
		g_pw_inc[2] = g_pw_cur[2] - priv->g_pw_pre[2];
	else
		g_pw_inc[2] = g_pw_cur[2];
	priv->g_pw_pre[2] = g_pw_cur[2];
	
	g_pw_cur[3] = phy_query_bb_reg(priv, 0xf74, 0x000000ff);
	if (g_pw_cur[3] >= priv->g_pw_pre[3])
		g_pw_inc[3] = g_pw_cur[3] - priv->g_pw_pre[3];
	else
		g_pw_inc[3] = g_pw_cur[3];
	priv->g_pw_pre[3] = g_pw_cur[3];
	
	g_pw_cur[4] = phy_query_bb_reg(priv, 0xf5c, 0xff000000);
	if (g_pw_cur[4] >= priv->g_pw_pre[4])
		g_pw_inc[4] = g_pw_cur[4] - priv->g_pw_pre[4];
	else
		g_pw_inc[4] = g_pw_cur[4];
	priv->g_pw_pre[4] = g_pw_cur[4];
	
	g_pw_cur[5] = phy_query_bb_reg(priv, 0xf5c, 0x00ff0000);
	if (g_pw_cur[5] >= priv->g_pw_pre[5])
		g_pw_inc[5] = g_pw_cur[5] - priv->g_pw_pre[5];
	else
		g_pw_inc[5] = g_pw_cur[5];
	priv->g_pw_pre[5] = g_pw_cur[5];

	g_total = g_pw_inc[0] + g_pw_inc[1] + g_pw_inc[2] + g_pw_inc[3] + g_pw_inc[4] + g_pw_inc[5];
	if(g_pw_inc[0] == 0) g0_ratio = 0;
	else g0_ratio = g_total / g_pw_inc[0];
	if(g_pw_inc[1] == 0) g1_ratio = 0;
	else g1_ratio = g_total / g_pw_inc[1];
	if(g_pw_inc[2] == 0) g2_ratio = 0;
	else g2_ratio = g_total / g_pw_inc[2];
	if(g_pw_inc[3]== 0) g3_ratio = 0;
	else g3_ratio = g_total / g_pw_inc[3];
	if(g_pw_inc[4]== 0) g4_ratio = 0;
	else g4_ratio = g_total / g_pw_inc[4];
	if(g_pw_inc[5] == 0) g5_ratio = 0;
	else g5_ratio = g_total / g_pw_inc[5];

	// read pulse repetition interval hist report
	phy_set_bb_reg(priv, 0x19e4, 0x00c00000, 3);  // report selection = 3 (pulse repetition interval)
	g_pri_cur[0] = phy_query_bb_reg(priv, 0xf74, 0xff000000);
	if (g_pri_cur[0] >= priv->g_pri_pre[0])
		g_pri_inc[0] = g_pri_cur[0] - priv->g_pri_pre[0];
	else
		g_pri_inc[0] = g_pri_cur[0];
	priv->g_pri_pre[0] = g_pri_cur[0];
	
	g_pri_cur[1] = phy_query_bb_reg(priv, 0xf74, 0x00ff0000);
	if (g_pri_cur[1] >= priv->g_pri_pre[1])
		g_pri_inc[1] = g_pri_cur[1] - priv->g_pri_pre[1];
	else
		g_pri_inc[1] = g_pri_cur[1];
	priv->g_pri_pre[1] = g_pri_cur[1];
	
	g_pri_cur[2] = phy_query_bb_reg(priv, 0xf74, 0x0000ff00);
	if (g_pri_cur[2] >= priv->g_pri_pre[2])
		g_pri_inc[2] = g_pri_cur[2] - priv->g_pri_pre[2];
	else
		g_pri_inc[2] = g_pri_cur[2];
	priv->g_pri_pre[2] = g_pri_cur[2];
	
	g_pri_cur[3] = phy_query_bb_reg(priv, 0xf74, 0x000000ff);
	if (g_pri_cur[3] >= priv->g_pri_pre[3])
		g_pri_inc[3] = g_pri_cur[3] - priv->g_pri_pre[3];
	else
		g_pri_inc[3] = g_pri_cur[3];
	 priv->g_pri_pre[3] = g_pri_cur[3];
	
	g_pri_cur[4] = phy_query_bb_reg(priv, 0xf5c, 0xff000000);
	if (g_pri_cur[4] >= priv->g_pri_pre[4])
		g_pri_inc[4] = g_pri_cur[4] - priv->g_pri_pre[4];
	else
		g_pri_inc[4] = g_pri_cur[4];
	priv->g_pri_pre[4] = g_pri_cur[4];
	
	g_pri_cur[5] = phy_query_bb_reg(priv, 0xf5c, 0x00ff0000);
	if (g_pri_cur[5] >= priv->g_pri_pre[5])
		g_pri_inc[5] = g_pri_cur[5] - priv->g_pri_pre[5];
	else
		g_pri_inc[5] = g_pri_cur[5];
	priv->g_pri_pre[5] = g_pri_cur[5];
	
	need_reset = 0;
	for(i = 0; i < 6; i++){
		if((priv->g_pw_pre[i]==255) || (priv->g_pri_pre[i] == 255)){
			need_reset = 1;
		}			
	}	
	if(1){
		phy_set_bb_reg(priv, 0x19b4, 0x10000000, 1);  // reset histogram report
		phy_set_bb_reg(priv, 0x19b4, 0x10000000, 0);  // continue histogram report
		for(i = 0; i < 6; i++){
			priv->g_pw_pre[i]=0;
			priv->g_pri_pre[i]=0;
		}
		for(i = 0; i < 16; i++){
			priv->g_ti_pre[i]=0;
		}		
	}*/

	if(DFS_tri_short_pulse || DFS_tri_long_pulse){
		//if(1){
/*		if(priv->pshare->rf_ft_var.dfs_print_hist_report){
			dfs_plusewidth_thd1 = PHY_QueryBBReg(priv, 0x19e4, 0xff000000);
			dfs_plusewidth_thd2 = PHY_QueryBBReg(priv, 0x19e8, 0x000000ff);
			dfs_plusewidth_thd3 = PHY_QueryBBReg(priv, 0x19e8, 0x0000ff00);
			dfs_plusewidth_thd4 = PHY_QueryBBReg(priv, 0x19e8, 0x00ff0000);
			dfs_plusewidth_thd5 = PHY_QueryBBReg(priv, 0x19e8, 0xff000000);
			PHY_SetBBReg(priv, 0x19e4, BIT(22)|BIT(23), 0x0);
			dfs_hist1_peak_index = PHY_QueryBBReg(priv, 0xf5c, 0xffffffff);
			dfs_hist2_peak_index = PHY_QueryBBReg(priv, 0xf74, 0xffffffff);
			PHY_SetBBReg(priv, 0x19e4, BIT(22)|BIT(23), 0x1);
			dfs_hist1_pulse_width = PHY_QueryBBReg(priv, 0xf5c, 0xffffffff);
			dfs_hist2_pulse_width = PHY_QueryBBReg(priv, 0xf74, 0xffffffff);
			g_plusewidth[0] = (unsigned char)(dfs_hist2_pulse_width & 0xff000000)>>24;
			g_plusewidth[1] = (unsigned char)(dfs_hist2_pulse_width & 0x00ff0000)>>16;
			g_plusewidth[2] = (unsigned char)(dfs_hist2_pulse_width & 0x0000ff00)>>8;
			g_plusewidth[3] = (unsigned char)dfs_hist2_pulse_width & 0x000000ff;
			g_plusewidth[4] = (unsigned char)(dfs_hist1_pulse_width & 0xff000000)>>24;
			g_plusewidth[5] = (unsigned char)(dfs_hist1_pulse_width & 0x00ff0000)>>16;
			g_plusewidth_total = 0;
			
			for (i=0; i<6 ; i++)
				g_plusewidth_total += g_plusewidth[i];
			
			PHY_SetBBReg(priv, 0x19e4, BIT(22)|BIT(23), 0x2);
			dfs_hist1_pulse_rep_intvl = PHY_QueryBBReg(priv, 0xf5c, 0xffffffff);
			dfs_hist2_pulse_rep_intvl = PHY_QueryBBReg(priv, 0xf74, 0xffffffff);
			PHY_SetBBReg(priv, 0x19b4, 0x10000000, 1);  // reset histogram report
			PHY_SetBBReg(priv, 0x19b4, 0x10000000, 0);  // continue histogram report
			panic_printk("dfs_plusewidth_thd=%d %d %d %d %d\n",
				dfs_plusewidth_thd1, dfs_plusewidth_thd2, dfs_plusewidth_thd3, dfs_plusewidth_thd4, dfs_plusewidth_thd5);
			panic_printk("peak index hist\n");
			panic_printk("dfs_hist_peak_index=%x %x\n",
				dfs_hist1_peak_index, dfs_hist2_peak_index);
			//panic_printk("g0 = %d, g1 = %d, g2 = %d, g3 = %d, g4 = %d, g5 = %d, g6 = %d, g7 = %d\n", g_ti_inc[0], g_ti_inc[1], g_ti_inc[2], g_ti_inc[3], g_ti_inc[4], g_ti_inc[5], g_ti_inc[6], g_ti_inc[7]);
			//panic_printk("g8 = %d, g9 = %d, g10 = %d, g11 = %d, g12 = %d, g13 = %d, g14 = %d, g15 = %d\n", g_ti_inc[8], g_ti_inc[9], g_ti_inc[10], g_ti_inc[11], g_ti_inc[12], g_ti_inc[13], g_ti_inc[14], g_ti_inc[15]);
			panic_printk("pulse width hist\n");
			panic_printk("dfs_hist_pulse_width=%x %x\n",
				dfs_hist1_pulse_width, dfs_hist2_pulse_width);
			panic_printk("g_pulse_width_hist = %x %x %x %x %x %x, g_pulse_width_hist_total = %x\n",
				g_plusewidth[0], g_plusewidth[1],g_plusewidth[2],g_plusewidth[3],g_plusewidth[4],g_plusewidth[5],g_plusewidth_total);
			
			//panic_printk("g0 = %d, g1 = %d, g2 = %d, g3 = %d, g4 = %d, g5 = %d\n",g_pw_inc[0], g_pw_inc[1], g_pw_inc[2], g_pw_inc[3], g_pw_inc[4], g_pw_inc[5]);
			//panic_printk("g0_ratio = %d, g1_ratio = %d, g2_ratio = %d, g3_ratio = %d, g4_ratio = %d, g5_ratio = %d\n", g0_ratio, g1_ratio, g2_ratio, g3_ratio, g4_ratio, g5_ratio);
			//panic_printk("pulse repetition interval hist\n");
			//panic_printk("g0 = %d, g1 = %d, g2 = %d, g3 = %d, g4 = %d, g5 = %d\n",g_pri_inc[0], g_pri_inc[1], g_pri_inc[2], g_pri_inc[3], g_pri_inc[4], g_pri_inc[5]);
		}*/
		
		// MKK
		if(priv->pshare->rf_ft_var.manual_dfs_regdomain == 2) {
			if((priv->pmib->dot11RFEntry.dot11channel >= 52) && (priv->pmib->dot11RFEntry.dot11channel <= 64)){
				// classify radar by pulse width hist
				if(g_pw_inc[1] >= short_pulse_th + 1){ 
					if(priv->pshare->rf_ft_var.dfs_radar_diff_print){
						panic_printk("MKK w53 radar type1 is detected!\n");
					}
				}
				else if(g_pw_inc[3] >= short_pulse_th + 1){
					if(priv->pshare->rf_ft_var.dfs_radar_diff_print){
						panic_printk("MKK w53 radar type2 is detected!\n");
					}
				}
				else{
					DFS_tri_long_pulse = 0;
					DFS_tri_short_pulse = 0;
				}
			}
			else{
				if (DFS_tri_long_pulse){
					if(priv->pshare->rf_ft_var.dfs_radar_diff_print){
						panic_printk("MKK w56 radar type7 is detected!\n");
					}
				}
				else{
					if(g_pw_inc[0] >= short_pulse_th + 1){
						if(priv->pshare->rf_ft_var.dfs_radar_diff_print){
							panic_printk("MKK w56 radar type1 is detected!\n");
						}
					}
					else if(g_pw_inc[1] >= short_pulse_th + 1){
						if(priv->pshare->rf_ft_var.dfs_radar_diff_print){
							panic_printk("MKK w56 radar type2 or type8 is detected!\n");
						}
					}
					else if(g_pw_inc[2] >= short_pulse_th + 1){
						if(priv->pshare->rf_ft_var.dfs_radar_diff_print){
							panic_printk("MKK w56 radar type3 is detected!\n");
						}
					}
					else if(g_pw_inc[1]+g_pw_inc[2]+g_pw_inc[3] >= short_pulse_th + 1){
						if(priv->pshare->rf_ft_var.dfs_radar_diff_print){
							panic_printk("MKK w56 radar type4 is detected!\n");
						}
					}
					else if(g_pw_inc[4] >= short_pulse_th + 1){
						if(priv->pshare->rf_ft_var.dfs_radar_diff_print){
							panic_printk("MKK w56 radar type5 is detected!\n");
						}
					}
					else if(g_pw_inc[5] >= short_pulse_th + 1){
						if(priv->pshare->rf_ft_var.dfs_radar_diff_print){
							panic_printk("MKK w56 radar type6 is detected!\n");
						}
					}
					else{
						DFS_tri_short_pulse = 0;
					}
				}
			}
		}
		// ETSI
		else if(priv->pshare->rf_ft_var.manual_dfs_regdomain == 3){
		}
		// FCC
		else if(priv->pshare->rf_ft_var.manual_dfs_regdomain == 1){
			if (DFS_tri_long_pulse){
				if(priv->pshare->rf_ft_var.dfs_radar_diff_print){
					panic_printk("FCC radar type5 is detected!\n");
				}
			}
			else{
				if(g_pw_inc[1] >= short_pulse_th + 1){
					if(priv->pshare->rf_ft_var.dfs_radar_diff_print){
						panic_printk("FCC radar type1 or type6 is detected!\n");
					}
				}
				else if(g_pw_inc[1] + g_pw_inc[2]>= short_pulse_th + 1){
					if(priv->pshare->rf_ft_var.dfs_radar_diff_print){
						panic_printk("FCC radar type2 is detected!\n");
					}
				}
				else if(g_pw_inc[3] >= short_pulse_th + 1){
					if(priv->pshare->rf_ft_var.dfs_radar_diff_print){
						panic_printk("FCC radar type3 is detected!\n");
					}
				}
				else if(g_pw_inc[4] >= short_pulse_th + 1){
					if(priv->pshare->rf_ft_var.dfs_radar_diff_print){
						panic_printk("FCC radar type4 is detected!\n");
					}
				}
				else{
					DFS_tri_short_pulse = 0;
				}
			}
		}
		else{
		} 
	}	
	
}
#endif //#if defined(CONFIG_WLAN_HAL_8814AE)

#endif

