#!/bin/sh  
#
# written by Georg E Schneider <georg@georgs.org>
# This script is Freeware you can use and modify in any way you want.
# But ther is no Warranty about all
#  
# Wie versprochen Paketiere ich auch deine Software dass jedoch Distributions
# unabhaengig auch rpm-Pkete gemacht werden koennen dazu dient dises kleine
# script

# Abfragen welche rpmbuild Version installiert ist <4.* verwendet rpm -b
# 4.* verwendet rpmbuild -b
# Um vielfaltiger agieren zu koennen ein paar Dinge am anfang
# Das Verzeichniss im dem sich die software befindet muss $SOFWARENAME-$VERSION
# heissen

SOFWARENAME=ggseq
VERSION=0.2.0

echo 
echo "****************************************"
echo "*  Automaticly Building RPM-Files for  *"
echo "*  $SOFWARENAME-$VERSION"
echo "****************************************"
echo
# Abfragen ob als root
echo -n "Testing if you are root........................"
if [ ! `/usr/bin/id -u` = 0 ];then
	if [ ! -f $HOME/.rpmmacros ];then
	echo "no userbuild possible - EXIT"
	exit 0;
	else
	echo -n " User build bulding user=$USER"
	RPMBUILDPATH=`cat $HOME/.rpmmacros | grep %_topdir | cut -d " " -f2`
	fi

else
echo -n " root build OK"
fi
echo
if 
test "x`cd "${dir}" 2>/dev/null && pwd`" != "x`pwd`"
then
    echo "This script must be executed directly from the source directory."
    exit 1
fi
#cd ../
#if [ ! -r $(`ls -d` "$SOFWARENAME-$VERSION") ]; then
#    echo "The Directory has to have the same Name like the software."
#    exit 1
#fi
#cd ./$SOFWARENAME-$VERSION
#pwd
#cd ./$SOFWARENAME
RPMBUILD="rpmbuild"
if [ !  -r /usr/bin/$RPMBUILD ];then
 	if [ ! -r /bin/$RPMBUILD ];then
   	echo "rpmbuild ..... no"
   	echo "trying rpm -b"
   	# first we need to know if rpm is installed
   		if [ !  -r /usr/bin/rpm ];then
 			if [ ! -r /bin/rpm ];then
   			echo "rpm ..... no"
   			echo "You have to have rpm see http://www.rpm.org"
   			exit 1
			fi
		fi
		echo -n "rpm found ..................................... "
		echo -n $(whereis rpm | cut -d " " -f2)" -ba"
		echo
		RPMBUILD=$(whereis rpm | cut -d " " -f2)" -ba"
		fi
else
echo -n "rpmbuild found ................................ "
echo $(whereis rpmbuild | cut -d " " -f2)" -ba"
sleep 1
RPMBUILD=$(whereis rpmbuild | cut -d " " -f2)" -ba"
fi 
sleep 1
if [ `/usr/bin/id -u` = 0 ];then

#Abfragen wo das RPM - Build verzeichniss (wie z.Bsp. /usr/src/redhat/) ist
#Zur Zeit weiss ich gerade ein paar andere sollen folgen
#ALTLinux = /usr/src/RPM
#trustix = /usr/src/RPM
#Arklinuc = /usr/src/arc
#TurboLinux = /usr/src/RPM
#Mandrake = /usr/src/RPM
#Caldera = /usr/src/OpenLinux (rpm 3*)
#Suse = /usr/src/packages (rpm 3*)
#PLD = ?
#Slackware = /usr/src/rpm
RPMBUILDPATH=
	echo -n "Default RPM Build -Directory .................. "
	if [ -r /usr/src/RPM ];then
	RPMBUILDPATH=/usr/src/RPM
	echo $RPMBUILDPATH
	fi
	if [ -r /usr/src/arc ];then
	RPMBUILDPATH=/usr/src/arc
	echo $RPMBUILDPATH
	fi
	if [ -r /usr/src/OpenLinux ];then
	RPMBUILDPATH=/usr/src/OpenLinux
	echo $RPMBUILDPATH
	fi
	if [ -r /usr/src/packages ];then
	RPMBUILDPATH=/usr/src/packages
	echo $RPMBUILDPATH
	fi
	if [ -r /usr/src/redhat ];then
	RPMBUILDPATH=/usr/src/redhat
	echo $RPMBUILDPATH
	fi
	if [ -r /usr/src/rpm ];then
	RPMBUILDPATH=/usr/src/rpm
	echo $RPMBUILDPATH
	fi
	if [ $RPMBUILDPATH = "" ];then
	echo
	echo "No RPM-Build Path found"
	exit 2
	fi
fi
rm -f $RPMBUILDPATH/SPECS/$SOFWARENAME.spec
rm -f $RPMBUILDPATH/SOURCES/$SOFWARENAME.tar.bz2
rm -Rf $RPMBUILDPATH/BUILD/$SOFWARENAME*
sleep 1
# Quellcode nach $RPMBuild/SOURCE in gezippter form verschieben
# Da nicht auf allen systemen bzip2 installiert sein muss nehmen wir gzip
# muessen wir nicht testen fuer rpm und rpmbuild muss ja gzip installiert sein
# =================================================
# Richard aus diesem Grund gzip
# =================================================
# zur zeit bz2 (muss ich leider wegen der Quellenangabe im SPEC File so machen)
echo -n "move and compress ............................. "
cd ..
# Das rpm nicht aussteigt muss die Versionsnummer im Verzeichnissnamen sein
echo "mv $SOFWARENAME $SOFWARENAME-$VERSION"
mv $SOFWARENAME $SOFWARENAME-$VERSION
tar -cjf $RPMBUILDPATH/SOURCES/$SOFWARENAME-$VERSION.tar.bz2  $SOFWARENAME-$VERSION
cd ./$SOFWARENAME-$VERSION
echo "done"
sleep 1
#SPECFILE erstellen
#Diese muss Softwaremaessig angepasst werden desshalb eine eigene Datei
#
echo -n "Copying SPEC - File .......................... "
# ===========================================================================
cp -f $SOFWARENAME.spec $RPMBUILDPATH/SPECS/$SOFWARENAME.spec
# ===========================================================================
echo "done"
sleep 1
#clening Sourcedir
cd ..
mv $SOFWARENAME-$VERSION $SOFWARENAME

#rpm(build) -ba $SPECFILE

$RPMBUILD --nodeps $RPMBUILDPATH/SPECS/$SOFWARENAME.spec
