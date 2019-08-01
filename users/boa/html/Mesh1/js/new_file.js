
var is_l7_qos_support=<% getIndex("is_l7_qos_support");%> ;	
var advanced_qos_enable=<%getIndex("advancedIpqosEnable")%>;
var hw_qos=<%getIndex("hwQosSupport")%>;
var vlan_support=<%getIndex("vlanSupport")%>;
var static_ip_support=<%getIndex("staticIpSupport")%>;

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

  
  if (document.formQosAdd.bandwidth.value!="") {
  	if ( validateKey( document.formQosAdd.bandwidth.value ) == 0 ) {
		alert(ip_qos_band_invalid);
		document.formQosAdd.bandwidth.focus();
		return false;
  	}
   }
   
   if (document.formQosAdd.bandwidth_downlink.value!="") {
  	if ( validateKey( document.formQosAdd.bandwidth_downlink.value ) == 0 ) {
		alert(ip_qos_band_invalid);
		document.formQosAdd.bandwidth_downlink.focus();
		return false;
  	}
   }
   if(advanced_qos_enable)
   {
   		with(document.formQosAdd)
   		{
			if(phyport.value && bandwidth.value && !checkFieldDigitRange(phyport,0,3,ip_qos_up_phyport_invalid))
				return false;
			if(phyport.value && bandwidth_downlink.value && !checkFieldDigitRange(phyport,4,4,ip_qos_down_phyport_invalid))
  				return false;
   		}
   }
   return true;
}




function Load_Setting()
{
	if ( <% getIndex("qosEnabled"); %> )
		document.formQosAdd.enabled.checked = true;

	if ( <% getIndex("qosAutoUplinkSpeed"); %> )
		document.formQosAdd.automaticUplinkSpeed.checked = true;

	updateQosState();
	
	
	if(is_l7_qos_support == 1)
	{				
 		document.getElementById("l7_qos_div").style.display = ""; 
	}
	else
	{
		document.getElementById("l7_qos_div").style.display = "none";
	}
	//
}