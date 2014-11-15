#/bin/sh

# gsoap directory
gsoap_dir=/home/nfs/software/gsoap-2.8.10/gsoap
code_dir=../onvif
# client or server side option
sc_opt="-2"
wsdl_list="imaging.wsdl event.wsdl analytics.wsdl devicemgmt.wsdl media.wsdl remotediscovery.wsdl ptz.wsdl event.wsdl analytics.wsdl deviceio.wsdl"

generateheader_11()
{
# -d : use DOM to populate xs:any and xsd:anyType elements
# -g : generate global top-level element declarations
./wsdl2h_11 -s -c -g -o onviff_11.h -t typemap.dat $wsdl_list
}

generateheader_10()
{
# -d : use DOM to populate xs:any and xsd:anyType elements
# -g : generate global top-level element declarations
./wsdl2h -s -c -g -o onviff_10.h -t typemap.dat $wsdl_list
}

generatesource()
{
# delete old sources
cd $code_dir
rm -f soap*
rm -f onvif.nsmap
cd -
# -c : generate pure c code
# -2 : generate soap 1.2 binding
# -n : generate 
echo "sc_opt:"$sc_opt", code_dir:"$code_dir"."
./soapcpp2 -c $sc_opt -L -x -d$code_dir onviff.h -I$gsoap_dir/import:$gsoap_dir
cd $code_dir
mv wsdd.nsmap _wsdd
rm *.nsmap
mv _wsdd onvif.nsmap
find -type f -user root | xargs chown nobody:nogroup
}

set_server_client_opt()
{
if [ "$1" = "-S" ];then
	sc_opt="-S -2"
elif [ "$1" = "-C" ];then
	sc_opt="-C -2"
elif [ "$1" = "-SC" ];then
	sc_opt="-2"
else
	echo "sc_opt invalid!"
	exit 1;
fi
echo "sc_opt is ""$sc_opt"".";
}

patch_generate_header()
{
echo "patch generated header..."
# import wsee.h
# using event headers generated by 2.8.11
patch onviff_10.h < ../patch/onviff.patch
mv onviff_10.h onviff.h
echo "patch header done!"
}

patch_generate_source()
{
# get soapstub patch file
echo "patch generated sources,arg:"$1" ..."
local soapstub_patch_file=""
if [ "$1" = "-S" ];then
	soapstub_patch_file="soapStub_S.patch"
elif [ "$1" = "-C" ];then
	soapstub_patch_file="soapStub_C.patch"
elif [ "$1" = "-SC" ];then
	soapstub_patch_file="soapStub_CS.patch"
else
	echo "sc_opt invalid!"
	exit 1;
fi
echo "soapStub file is "$soapstub_patch_file;
# undef SOAP_WSA_2005
patch $code_dir/soapStub.h < ../patch/$soapstub_patch_file
# fix redefined soap_serve_SOAP_ENV__Fault
patch $code_dir/soapServer.c < ../patch/soapServer.patch
# fix redefined soap_send_SOAP_ENV__Fault
# fix redefined soap_recv_SOAP_ENV__Fault
patch $code_dir/soapClient.c < ../patch/soapClient.patch
# remove wsdd interfaces in soapClient, because it's defined int wsdd.c'
# patch $code_dir/soapClient.c < ../patch/soapClient_remove_wsdd_functions.patch
# add ns
patch $code_dir/onvif.nsmap < ../patch/nsmap.patch
# fix hikvision bug (can't setup venc')
# patch $code_dir/soapC.c < ../patch/soapC.patch

echo "patch sources done!"
}

printusage()
{
	echo "usage: ./setup.sh {-1|-2|-3|-4|-5|-A} {SC_OPT}"
	echo "    SC_OPT : {-S|-C|-SC}"
	echo "./setup -1  : generate header from wsdl by gsoap-2.8.10"
	echo "./setup -2 : generate header from wsdl by gsoap-2.8.11"
	echo "./setup -3 : patch generated header"
	echo "./setup -4 {SC_OPT} , generate server side(clent-side/both-sides) source code"
	echo "./setup -5 {SC_OPT} : patch generated sources"
	echo "./setup -A : the same to runing commands as below:"
	echo "   ./setup -1"
	echo "   ./setup -2"
	echo "   ./setup -3"
	echo "   ./setup -4 {SC_OPT}"
	echo "   ./setup -5 {SC_OPT}"
	exit 1;
}

batch_all()
{
set_server_client_opt $1;
generateheader_10;
#generateheader_11;
patch_generate_header;
generatesource;
patch_generate_source $1;
}

if [ $# == 0 ];then
	printusage;
elif [ $# > 0 ];then
	if [ "$1" = "-1" ];then
		generateheader_10;
	elif [ "$1" = "-2" ];then
		generateheader_11;
	elif [ "$1" = "-3" ];then
		patch_generate_header;
	elif [ "$1" = "-5" ];then
		patch_generate_source $2;
	elif [ "$1" = "-A" ];then
		if [ $# != 2 ];then
			printusage;
		else
			batch_all $2;
		fi
	else
		if [ $# != 2 ];then
			printusage;
		else
			set_server_client_opt $2;
			echo "done set sc opt";
			generatesource;
		fi

	fi
else
	printusage;
fi

echo "command:'"$0" "$1" "$2"'success done"
exit 0;