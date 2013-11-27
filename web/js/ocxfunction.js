var oSelected = [],
	oSearchOcx,
	oCommonLibrary;	
	var oActiveEvents = ['Add','Delete','ModifyUserLevel','ModifyUserPasswd','AddArea','ModifyArea','RemoveArea','AddGroup','RemoveGroup','ModifyGroup','ModifyChannel'];  //事件名称集合
	$(function(){ 
		$('#area_0').data('data',{'area_id':'0','area_name':'区域_root','pid':'0','pareaname':'root'})
		$('#group_0').data('data',{'group_id':'0','group_name':'分组_root'});// 主区数据句填充.
		oCommonLibrary = document.getElementById('commonLibrary')
		oSearchOcx = document.getElementById('devSearch');
		//搜索设备;
		oSearchOcx.AddEventProc('SearchDeviceSuccess','callback(oJson);');
		oSearchOcx.Start();
		setTimeout(function(){oSearchOcx.Stop()},5000);
		//分组列表;
		groupList2Ui();
		//区域列表;
		areaList2Ui();
		//debug();
		for (i in oActiveEvents){
			AddActivityEvent(oActiveEvents[i]+'Success',oActiveEvents[i]+'Success(data)');
			AddActivityEvent(oActiveEvents[i]+'Faild','Faild(ev)');
		}
	})	

	function AddSuccess(ev){
		var name = document.getElementById('add_username').value;
		var usrLev= $('#menu2 div.select span').html();
		var No = $('#UserMan table.UserMan tbody tr').length - 1;
		$('<tr><td>'+No+'</td><td>'+name+'</td><td>'+usrLev+'</td></tr>').appendTo('#UserMan table.UserMan');		
	}
	function AddFaild(ev){
		Confirm('添加失败!');
	}
	function DeleteSuccess(){
		oSelected = [];
		$('#UserMan table.UserMan tbody tr input:hidden').val('');
		$('#UserMan table.UserMan tbody tr').filter(function(){ 
			return $(this).attr('class') == 'selected';
		}).remove();
	}
	function DeleteFaild(ev){ 
		Confirm('删除失败');
	}
	function Faild(ev){ 
		Confirm(ev.faild);	
	}
	function ModifyUserLevelSuccess(ev){
		$('#UserMan table.UserMan tbody tr').filter(function(){ 
			return $(this).attr('class') == 'selected';
		}).eq(0).find('td:last').html($('#menu3 div.select span').html());
	}

	//搜索设备控件方法.
	function searchFlush(){
		$('#SerachDevList tbody tr').remove();	 
		oSearchOcx.Flush();
	}
	//设备搜索回调函数
	function callback(oJson){
		var bUsed = true;
		$('#SerachDevList tbody tr').each(function(){ 
			if(parseInt($(this).find('td:eq(1)').html()) == oJson.SearchVendor_ID){
				bUsed = false;
			}
		})
		if(bUsed){
			$('<tr><td><input type="checkbox"/>'+oJson.SearchVendor_ID+'</td><td>'+	oJson.SearchDeviceId_ID+'</td><td>'+oJson.SearchIP_ID+'</td><td>'+oJson.SearchChannelCount_ID+'</td></tr>').appendTo($('#SerachDevList tbody')).data('data',oJson);
		}		
	}
	function AddAreaSuccess(data){
		var name = $('#area_name_ID').val();
		var pid = $('#pid_ID').val();
		var pidname = oCommonLibrary.GetAreaName(pid);
		var id = data.areaid;
		var add = $('<li><span class="area" id="area_'+id+'">'+name+'</span><ul></ul></li>').appendTo($('div.dev_list:eq(0) #area_'+pid).next('ul'));
		add.find('span.area').data('data',{'area_name':name,'pareaname':pidname,'area_id':id,'pid':pid});
		$('ul.filetree:eq(0)').treeview({add:add});
		closeMenu();
	} 

	function RemoveAreaSuccess(){
		var id = $('#confirm input:hidden').val();
		if(id != 0){
			$('div.dev_list').find('#area_'+id).parent('li').remove();
			$('ul.filetree').treeview();
		}
		closeMenu();
	}

	function ModifyAreaSuccess(){
		var oArea = $('#area_'+$('#area_id_ID').val());
		var sNewName = $('#area_name_ID').val()
		oArea.html(sNewName);
		oArea.data('data')['area_name']=sNewName;
		closeMenu();
	} 
	function AddGroupSuccess(data){
		var name = $('#group_name_ID').val();
		var id = data.groupid;
		var add = $('<li><span class="group" id="group_'+id+'">'+name+'</span><ul></ul></li>').appendTo($('#group_0').next('ul'));
			add.find('span.group').data('data',{'group_id':id,'group_name':name});
			$('ul.filetree:eq(1)').treeview({add:add});
		closeMenu();
	}
	function RemoveGroupSuccess(data){ 
		var id=$('#group_id_ID').val();
		$('#group_'+id).parent('li').remove();
		$('ul.filetree').treeview();
		closeMenu();
	}
	function ModifyGroupSuccess(data){ 
		alert(123);
		/*var id=$('#group_id_ID').val();
		var name = $('#group_name_ID').val();
		alert(id+'+'+name);
		$('#group_'+id).html(name);
		$('#group_'+id).data('data')['group_name'] = name;*/
		closeMenu();
	}

	function ModifyChannelSuccess() {
		var id= $('#channel_id_ID').val();
		var oChannel=$('#channel_'+id)
		var name =$('#channel_name_ID').val();
		oChannel.html(name);
		oChannel.data('data')['channel_name'] = name;
		closeMenu();
	}
	function debug (){ 
		$('#area_0').data('data',{'area_id':'0','area_name':'区域_root','pid':'0','pareaname':'区域_root'});// 主区数据句填充.
		$('#area_1').data('data',{'area_id':'1','area_name':'test1','pid':'0','pareaname':'区域_root'});// 
		$('#group_0').data('data',['0',$('#group_0').html()]);// 主区数据句填充.
		$('#dev_1').data('data',{'devname':'test4','address':'123','eseeid':'10001685','username':'admin','devid':'1'});// 主区数据句填充.
		$('.debug').show();
	}
	function Confirm(str){ 
		$('#confirm').find('h4').html(str).show();
		$('div.close').html('确定')
		objShowCenter($('#confirm'));
	}