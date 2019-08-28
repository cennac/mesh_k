/**
 * 获取浏览器语言类型
 * @return {string} 浏览器国家语言
 */
var getNavLanguage = function(){
    if(navigator.appName == "Netscape"){
        var navLanguage = navigator.language;
        return navLanguage;
    }
    return false;
}

/**
 * 设置语言类型： 默认为中文
 */
var i18nLanguage = "zh-CN";

/*
设置一下网站支持的语言种类
 */
var webLanguage = ['zh-CN', 'en'];

/**
 * 执行页面i18n方法
 * @return
 */ 
var execI18n = function(){
    /*
    获取一下资源文件名
     */
    var optionEle = $("#i18n_pagename");
    if (optionEle.length < 1) {
        console.log("未找到页面名称元素，请在页面写入\n <meta id=\"i18n_pagename\" content=\"页面名(对应语言包的语言文件名)\">");
        return false;
    }
    var sourceName = optionEle.attr('content');
    sourceName = sourceName.split('-');
    /*
    首先获取用户浏览器设备之前选择过的语言类型
     */
    if ($.cookie("userLanguage")) {
        i18nLanguage = $.cookie("userLanguage");
        if(i18nLanguage=="en"){
        	$(".logo_img").attr("src","img/ks_link.png");
        	$(".login_logo img").attr("src","img/ks_link.png");
        }else{
        	$(".logo_img").attr("src","img/KSLINK.png");
        	$(".login_logo img").attr("src","img/KSLINK.png");
        }
    } else {
        // 获取浏览器语言
        var navLanguage = getNavLanguage();
        if (navLanguage) {
            // 判断是否在网站支持语言数组里
            var charSize = $.inArray(navLanguage, webLanguage);
            if (charSize > -1) {
                i18nLanguage = navLanguage;
                if(navLanguage=="en"){
		        	$(".logo_img").attr("src","img/ks_link.png");
		        	$(".login_logo img").attr("src","img/ks_link.png");
		        }else{
		        	$(".logo_img").attr("src","img/KSLINK.png");
		        	$(".login_logo img").attr("src","img/KSLINK.png");
		        }
		        
		        //execI18n();
		        //将系统语言设置为浏览器对应的语言
//		        var insertEle = $(".i18n");
//	            insertEle.each(function() {
//	                // 根据i18n元素的 name 获取内容写入
//	                $(this).html($.i18n.prop($(this).attr('name')));
//	            });
//	
//	            var insertInputEle = $(".i18n-input");
//	            insertInputEle.each(function() {
//	                var selectAttr = $(this).attr('selectattr');
//	                if (!selectAttr) {
//	                    selectAttr = "value";
//	                }
//	                $(this).attr(selectAttr, $.i18n.prop($(this).attr('selectname')));
//	                var guest_net_text=$(this).attr('selectname');
//	                if(guest_net_text.indexOf("guest_net")!=-1){
//	                	$(this).text($.i18n.prop($(this).attr('selectname')));
//	                }
//	                if($(".uploadurl").text()!=""||$(".uploadurl").text()!=undefined||$(".uploadurl").text()!=null){
//						$(".Unselected_file").text($.i18n.prop($(this).attr('selectname')));
//					}else{
//						$(".Unselected_file").text("");
//					}
//	            });
                // 存到缓存中
                $.cookie("userLanguage",navLanguage,{
                    expires:30
                });
            }
        } else{
            console.log("not navigator");
            return false;
        }
    }
    /* 需要引入 i18n 文件*/
    if ($.i18n == undefined) {
        console.log("请引入i18n js 文件")
        return false;
    }

    /*
     * i18n翻译
     */
    jQuery.i18n.properties({
        name : sourceName, //资源文件名称
        path : 'i18n/' + i18nLanguage +'/', //资源文件路径
        mode : 'map', //用Map的方式使用资源文件中的值
        language : i18nLanguage,
        callback : function() {//加载成功后设置显示内容
            var insertEle = $(".i18n");
            insertEle.each(function() {
                // 根据i18n元素的 name 获取内容写入
                $(this).html($.i18n.prop($(this).attr('name')));
            });

            var insertInputEle = $(".i18n-input");
            insertInputEle.each(function() {
                var selectAttr = $(this).attr('selectattr');
                if (!selectAttr) {
                    selectAttr = "value";
                }
                $(this).attr(selectAttr, $.i18n.prop($(this).attr('selectname')));
                var guest_net_text=$(this).attr('selectname');
                if(guest_net_text.indexOf("guest_net")!=-1){
                	$(this).text($.i18n.prop($(this).attr('selectname')));
                }
                if($(".uploadurl").text()!=""||$(".uploadurl").text()!=undefined||$(".uploadurl").text()!=null){
					$(".Unselected_file").text($.i18n.prop($(this).attr('selectname')));
				}else{
					$(".Unselected_file").text("");
				}
            });
        }
    });
}

$(function(){

    /*执行I18n翻译*/
    execI18n();

    /*将语言选择默认选中缓存中的值*/
    $("#language option[value="+i18nLanguage+"]").attr("selected",true);

    /* 选择语言 */
    $("#language").bind('change', function() {
        var language = $(this).children('option:selected').val();
        if(language=="en"){
        	$(".logo_img").attr("src","img/ks_link.png");
        	$(".login_logo img").attr("src","img/ks_link.png");
        }else{
        	$(".logo_img").attr("src","img/KSLINK.png");
        	$(".login_logo img").attr("src","img/KSLINK.png");
        }
        $.cookie("userLanguage",language,{
            expires:30
        });
        
//      function addClass_name(){
	        $(".nav li").each(function(){
	        	var current_list=$(this).attr("class");
	        	if(current_list.indexOf("active")!=-1){
	        		var add_class_name=$(this).attr("class");	        		
//	        		alert(1111);
	        	}
	        })
//      }
        //setInterval("addClass_name()",10);
        location.reload();
    });
});