function addRuleClick()
{
  if (!document.formQosAdd.enabled.checked)
  	return true;

  if (!document.formQosAdd.automaticUplinkSpeed.checked && (document.formQosAdd.manualUplinkSpeed.value=="" || document.formQosAdd.manualUplinkSpeed.value < 100)) {
	alert(ip_qos_upspeed_empty);
	document.formQosAdd.manualUplinkSpeed.focus();
	return false;
  }
 
  if(!document.formQosAdd.automaticUplinkSpeed.checked && validateKey(document.formQosAdd.manualUplinkSpeed.value) == 0)
  {
  	alert(ip_qos_band_invalid);
	document.formQosAdd.manualUplinkSpeed.focus();
	return false;
  }
 
  if (!document.formQosAdd.automaticDownlinkSpeed.checked && (document.formQosAdd.manualDownlinkSpeed.value=="" || document.formQosAdd.manualDownlinkSpeed.value<100)) {
	alert(ip_qos_downspeed_empty);
	document.formQosAdd.manualDownlinkSpeed.focus();
	return false;
  }
  
  if(!document.formQosAdd.automaticDownlinkSpeed.checked && validateKey(document.formQosAdd.manualDownlinkSpeed.value) == 0)
  {
  	alert(ip_qos_band_invalid);
	document.formQosAdd.manualDownlinkSpeed.focus();
	return false;
  }
   return true;
}