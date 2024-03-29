/*++
Copyright (c) Realtek Semiconductor Corp. All rights reserved.

Module Name:
	Hal8197GPhyCfg.c
	
Abstract:
	Defined HAL 8197G PHY BB setting functions
	    
Major Change History:
	When       Who               What
	---------- ---------------   -------------------------------
	2012-11-14 Eric              Create.	
--*/
#if !defined(__ECOS) && !defined(CPTCFG_CFG80211_MODULE)
#include "HalPrecomp.h"
#else
#include "../../HalPrecomp.h"
#endif

// TODO: this function should be modified
void 
TXPowerTracking_ThermalMeter_Tmp8197G(
    IN  HAL_PADAPTER    Adapter
)
{
     // Do Nothing now
}

u4Byte
PHY_QueryRFReg_8197G(
	IN  HAL_PADAPTER                Adapter,
	IN  u4Byte                      eRFPath,
	IN  u4Byte                      RegAddr,
	IN  u4Byte                      BitMask
)
{
    u4Byte          Readback_Value;
    unsigned long   flags;
    HAL_PADAPTER    priv     = Adapter;

#if CFG_HAL_DISABLE_BB_RF
    return 0;
#endif //CFG_HAL_DISABLE_BB_RF


    HAL_SAVE_INT_AND_CLI(flags);

    Readback_Value = config_phydm_read_rf_reg_8197g((&(Adapter->pshare->_dmODM)), eRFPath, RegAddr, BitMask);
    HAL_RESTORE_INT(flags);
    return Readback_Value;

}

void PHY_SetRFReg_8197G
(
    IN  HAL_PADAPTER                Adapter,
    IN  u4Byte                      eRFPath,
    IN  u4Byte                      RegAddr,
    IN  u4Byte                      BitMask,
    IN  u4Byte                      Data
)
{
    u4Byte          flags;
    HAL_PADAPTER    priv     = Adapter;    
#if CFG_HAL_DISABLE_BB_RF
    return;
#endif //CFG_HAL_DISABLE_BB_RF

	HAL_SAVE_INT_AND_CLI(flags);

    
	config_phydm_write_rf_reg_8197g((&(Adapter->pshare->_dmODM)), eRFPath, RegAddr, BitMask, Data);
	HAL_RESTORE_INT(flags);
	delay_us(8);
	return;
}

void
SwBWMode88XX_8197G(
	IN  HAL_PADAPTER    Adapter,
	IN  u4Byte          bandwidth,
	IN  s4Byte          offset
)
{
	u1Byte regDataSC;
    switch (bandwidth) {
        case CHANNEL_WIDTH_20:
            bandwidth = CHANNEL_WIDTH_20;
            break;

        case CHANNEL_WIDTH_40:            
            bandwidth = CHANNEL_WIDTH_40;           
            regDataSC = HAL_RTL_R8(REG_DATA_SC);
            regDataSC &= 0xf0;
            regDataSC |= Adapter->pshare->txsc_20_92e;
            HAL_RTL_W8(REG_DATA_SC, regDataSC);
            break;
    }

    config_phydm_switch_bandwidth_8197g((&(Adapter->pshare->_dmODM)), offset, bandwidth);

	return;

}

