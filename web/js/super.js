// JavaScript Document
var screenShotSearch,oSearchOcx,autoSearchDev;
$(document).ready(function() {
      document.getElementById('commonLibrary').getLanguage()== 'en_PR' ?  $('#Persian').show(): $('#Persian').hide();
		 oSearchOcx = document.getElementById('devSearch');
		 autoSearchDev = document.getElementById('atuoSearchDevice');
		 screenShotSearch = document.getElementById('screenShotSearch');
		 
		 //��������ʾת��
		$('li[title],div[title],a[title],span[title]').each(function(){
			$(this).attr('title',lang[$(this).attr('title')])
		})

      
			$('.hover').each(function(){  // ��ťԪ���������¼���Ӧ��ʽ
		   var action = $(this).attr('class').split(' ')[0];
		    addMouseStyleByLimit($(this),action,1<<11);
	       })

		 var username = autoSearchDev.getCurrentUser();
		  username && $('.top_nav div p span:eq(1)').html(username);
		  
		 /* $('#set_content .left li').each(function(index){
			  $(this).click(function(){
				var warp = $('#set_content div.right div.right_content').hide().eq(index).show();  
				//$('#ajaxHint').html('').stop(true,true).hide();
			  })
			    
		  });*/
		  $('a.close').click(closeMenu);  //�����������²���Ԫ����ӹرմ����¼�
		   
		 $('#windowAllSelect').click(function(){
		    
			var oCheckbox = $('.window_ckbox :checkbox');
			var b = $(this).prop('checked');
			oCheckbox.prop('checked',b);	 
			   
		 });
		 
		 $('.window_ckbox').on('click',':checkbox',function(event){
				var b = true;
				$('.window_ckbox :checkbox').each(function(){
					if(!$(this).prop('checked')){
						b = false;
					}
				})

			$('#windowAllSelect').prop('checked',b);
				event.stopPropagation();
		 })
          
		  $('#maxprev,#maxnext,#minprev,#minnext').hover(function(){
			  
			  $(this).css('background','#999');
			    
		   },function(){
			  
			  $(this).css('background','none');  
			   
		    }).mousedown(function(){
			  	
				$(this).css('background','#666');
				
			}).mouseup(function(){
			  
			   $(this).css('background','#999');
			  	
			});
			
			var startDateTextBox =$('#searchStart');
			var endDateTextBox = $('#searchEnd');
   
			startDateTextBox.datepicker({
				 
			   dateFormat: 'yy-mm-dd',
			   currentText: _T('current_time'),
			   closeText: _T('confirm'),
			   showButtonPanel: true,
			   changeMonth: true,  
			   changeYear: true,
			   monthNames: ["01","02","03","04","05","06","07","08","09","10","11","12"],
			   monthNamesShort:["01","02","03","04","05","06","07","08","09","10","11","12"], 
			   dayNames: ["Su","1","2","3","4","5","6"], 
			   dayNamesShort: ["Su","1","2","3","4","5","6"], 
			   
				onClose: function(dateText, inst) {
				  
				  if (endDateTextBox.val() != '') {
					
					var testStartDate = startDateTextBox.val();
					var testEndDate = endDateTextBox.val();
					var dataStart =startDateTextBox.attr('data');

					if (testStartDate > testEndDate){
						 
						confirm_tip(_T('Start_end_time_error'));
						var timer =setTimeout(function(){
							closeMenu();
							clearTimeout(timer);
						},2000);
					 startDateTextBox.val(testEndDate);
					}
				  }else {
					endDateTextBox.val(dateText);
				  }
			
				}
			});	
		
			 
			endDateTextBox.datepicker({ 
		   
			   dateFormat: 'yy-mm-dd',
			   currentText: _T('current_time'),
			   closeText: _T('confirm'),
			   showButtonPanel: true,
			   changeMonth: true,  
			   changeYear: true,
			   monthNames: ["01","02","03","04","05","06","07","08","09","10","11","12"],
			   monthNamesShort:["01","02","03","04","05","06","07","08","09","10","11","12"], 
			   dayNames: ["Su","1","2","3","4","5","6"], 
			   dayNamesShort: ["Su","1","2","3","4","5","6"], 
				
				onClose: function(dateText, inst) {
					if (startDateTextBox.val() != '') {
						var testStartDate = startDateTextBox.val();
						var testEndDate = endDateTextBox.val();
						var endData =endDateTextBox.attr('data');       
						if (testStartDate > testEndDate){
							confirm_tip(_T('Start_end_time_error'));
							var timer =setTimeout(function(){
								closeMenu();
								clearTimeout(timer);
							},2000);
							endDateTextBox.val(testStartDate);
						}
					}else{
						startDateTextBox.val(dateText);
					}
				}
			});
				
		
		  set_contentMax();
		  
		 AddActivityEvent('Validation','Validationcallback(data)'); 
		 
		autoSearchDev.AddEventProc("useStateChange",'useStateChange(ev)');
		autoSearchDev.startGetUserLoginStateChangeTime();
		screenShotSearch.AddEventProc("ScreenShotInfo","ScreenShotInfocallback(data)");
});

 //�û���¼״̬�ص�����
    function useStateChange(ev){
		//console.log(ev);
		if(ev.status==0){
		 $('.top_nav p span:eq(1)').html(ev.userName);	
		}else{
		  $('.top_nav p span:eq(1)').html(_T("not_Login"));
		}	
	}
	function set_contentMax(){
			var W = $(window).width(),
				H = $(window).height();
				W = W<1000?1000:W;
				H = H<600?600:H;
			 
			$('#set_content div.left').height(H -106);
	
			var warp = $('#set_content div.right').css({ 
				width:W - 250,
				height:H - 106
			}).find('div.right_content:visible');
	        
			$('#picSearch').css({
				
				height:warp.height(),
			});
			
			$('.box').css({
			  width:warp.width()-240,
			  height:warp.height()-30	
				
			}); 
			
			$('#maxbox').css({
			   height:$('.box').height()*3/5	
			});
			
			$('#maxprev,#maxnext').css({
			   
			   top:($('#maxbox').height()-$('#maxprev').height())/2	
				
			});

			$('#foot').css('top',$('#set_content div.right').height()+78);
		}
	
	function Validationcallback(data){ //id��ťȨ����֤
		    // console.log(data);
			if(data.ErrorCode=="1"){
				//autoSearchDev.showUserLoginUi(336,300);
			}else if(data.ErrorCode=="2"){
				closeMenu();
				confirm_tip(_T('no_limit'));
				var timer =setTimeout(function(){
					closeMenu();
					clearTimeout(timer);
				},2000);
			}
		}
	function ScreenShotInfocallback(data){
	   //console.log('=============================================');
	  // console.log(data);
	   var dataArr = {'name':data.fileName,'dir':data.fileDir,'time':data.time,'type':data.type,'user':data.userName,'window':data.wndId}
	    $('<li><img src="/'+$.trim(data.fileDir)+'/'+$.trim(data.fileName)+'" width="100" height="100"/></li>').appendTo($('#minbox ul')).data('data',dataArr);
		//console.log(dataArr);
	  $('<img src="/'+$.trim(data.fileDir)+'/'+$.trim(data.fileName)+'"/>').appendTo($('#imgs'));
	}
	
	function picSearch(){
		
			 $('#minbox li,#imgs img').remove();
			 
			 var date1 = '';
			 var date2 = '';
			 var type =0;
			 var chl=0;
			 var user = autoSearchDev.getCurrentUser();
			 if(!user){ lock();}
			 
			 if($('#windowAllSelect').prop('checked')){

				 chl = 562949953421311;
				 
			 }else{
				 $('.window_ckbox :checkbox').each(function(index){
					if($(this).prop('checked')){
						chl+=Math.pow(2,index);
					}
				})
			 }
        
			$('.picType').each(function(index){
				if($(this).prop('checked')){
						type+=Math.pow(2,index);
					}
			});
			date1 = $('#searchStart').val();
			date2 = $('#searchEnd').val();
			//alert(date1+' '+date2);
			 var num =  screenShotSearch.searchScreenShot(chl.toString(2),date1,date2,type,user);
			 if(num==1){
			  confirm_tip(_T('Parameter_error'));	 
			  var timer =setTimeout(function(){
				closeMenu();
				clearTimeout(timer);
			   },2000);
			 }else if(num==0){
				$('#box1').focusPic({ 
				      piclist:'#imgs',//ͼƬ�б�
					  frame:'#iframe',//���ֲ�
					  box:"#box",//�ܿ��
					  maxbox:"#maxbox",//��ͼ���
					  minbox:"#minbox",//Сͼ���
					  minPrev:"#minprev",//Сͼ���ͷ
					  minNext:"#minnext",//Сͼ�Ҽ�ͷ
					  maxPrev:"#maxprev",//��ͼ���ͷ
					  maxNext:"#maxnext",//��ͼ�Ҽ�ͷ
					  autoplay:false,//�Ƿ��Զ�����
					  interTime:4000,//ͼƬ�Զ��л����
					  delayTime:800,//�л�һ��ͼƬʱ��
					  order:0,//��ǰ��ʾ��ͼƬ����0��ʼ��
					  Maxpicdire:true,//��ͼ��������trueˮƽ���������
					  Minpicdire:true,//Сͼ��������trueˮƽ���������
					  minPicshowNum:6,//Сͼ��ʾ����
				}); 
				
				 
			  $('.maxpic').css({
				 
				 left:($('.box').width()*0.8-$('.maxpic').width())/2,
				 top:($('#maxbox').height()-$('.maxpic').height())/2
			  });
              
			 for(var i in document.images){
				 document.images[i].ondragstart=function(){
					 return false;
				 };
			 }

				
			 }

   }
   
   function checkUserRight(uicode,uisubcode){
	  //console.log('uicode:'+uicode+' uisubcode:'+uisubcode);
	  var itema= autoSearchDev.checkUserLimit(uicode.toString(2),uisubcode);
	   //console.log("��ǰ�û�"+autoSearchDev.getCurrentUser()+" ��¼״̬��"+itema);
		return itema;
 }
   function checkUserRightBtn(uicode,uisubcode,fn,num){
	  //console.log('uicode:'+uicode+' uisubcode:'+uisubcode);
	  var itema= autoSearchDev.checkUserLimit(uicode.toString(2),uisubcode);
	//console.log("��ǰ�û�"+autoSearchDev.getCurrentUser()+" ��¼״̬��"+itema);
		if(itema==0){
			window[fn](num);
		}else if(itema==1){
		}else{
		   closeMenu();
		   confirm_tip(_T('no_limit'));
			var timer =setTimeout(function(){
				closeMenu();
				clearTimeout(timer);
			},2000);
			
		}
 }
   function checkUserRightdiv(uicode,uisubcode,fn,num){
	  //console.log('uicode:'+uicode+' uisubcode:'+uisubcode);
	  var itema= autoSearchDev.checkUserLimit(uicode.toString(2),uisubcode);
	//console.log("��ǰ�û�"+autoSearchDev.getCurrentUser()+" ��¼״̬��"+itema);
		if(itema==0){
			window[fn](num);
		}else if(itema==1){
			
             var show = autoSearchDev.showUserLoginUi(336,300);
			  if(show==0){
				  var timer = setTimeout(function(){
					   checkUserRightBtn(uicode,uisubcode,fn,num);
					   clearTimeout(timer);
					  },300);
			  }else{
				  var timer1 = setTimeout(function(){
				    if(fn=='userList2Ui'){
					        $('.right_content:visible ul.ope_list li').eq(0).addClass('ope_listAct').siblings('li').removeClass('ope_listAct');
							  $('.right_content:visible div.switch').hide();
							  $('.right_content:visible div.switch').eq(0).show();
					  }  
					   clearTimeout(timer1);
					  },300);
			  }
		}else{
		   closeMenu();
		   confirm_tip(_T('no_limit'));
			var timer =setTimeout(function(){
				closeMenu();
				if(fn=='userList2Ui'){
				$('.right_content:visible ul.ope_list li').eq(0).addClass('ope_listAct').siblings('li').removeClass('ope_listAct');
						$('.right_content:visible div.switch').hide();
						$('.right_content:visible div.switch').eq(0).show();
			     cUserinfo2Ui();
				}
				clearTimeout(timer);
			},300);
			
		}
 }