#!/bin/bash

#####################################################################
################## Dependency installation ##########################
#####################################################################

# determine system type and set dependencies appropriately
SYSTEM=`uname -a`
DEPENDENCIES='git cmake libxml++2.6-dev libboost-dev libsqlite3-dev libbz2-dev coinor-libcbc-dev coinor-libclp-dev coinor-libcoinutils-dev coinor-libosi-dev coinor-libcgl-dev'
if [[ `echo $SYSTEM | grep Ubuntu` ]]; then
  DEPENDENCIES='git cmake libxml++2.6-dev libboost-dev libsqlite3-dev libbz2-dev coinor-libcbc-dev coinor-libclp-dev coinor-libcoinutils-dev coinor-libosi-dev coinor-libcgl-dev'
else
  echo "WARNING: Unsupported operating system type '$SYSTEM', using default parameters"
fi

# Discover missing dependencies
PKGSTOINSTALL=""
for DEP in $DEPENDENCIES
do
  if which dpkg &> /dev/null; then
    if [[ ! `dpkg -l | grep -w "ii  $DEP "` ]]; then
      PKGSTOINSTALL="$PKGSTOINSTALL $DEP"
    fi
  # OpenSuse, Mandriva, Fedora, CentOs, ecc. (with rpm)
  elif which rpm &> /dev/null; then
    if [[ ! `rpm -q $DEP` ]]; then
      PKGSTOINSTALL=$PKGSTOINSTALL" "$DEP
    fi
  # ArchLinux (with pacman)
  elif which pacman &> /dev/null; then
    if [[ ! `pacman -Qqe | grep "$DEP"` ]]; then
      PKGSTOINSTALL=$PKGSTOINSTALL" "$DEP
    fi
  # If it's impossible to determine if there are missing dependencies, mark all as missing
  else
    PKGSTOINSTALL=$PKGSTOINSTALL" "$DEP
  fi
done

# Install missing dependencies
if [ "$PKGSTOINSTALL" != "" ]; then
  echo "Missing dependencies: $PKGSTOINSTALL"
  echo -n "Do you want to install them? (Y/n): "
  read SURE
  # If user want to install missing dependencies
  if [[ $SURE = "Y" || $SURE = "y" || $SURE = "" ]]; then
    # Debian, Ubuntu and derivatives (with apt-get)
    if which apt-get &> /dev/null; then
      apt-get install $PKGSTOINSTALL
    # OpenSuse (with zypper)
    elif which zypper &> /dev/null; then
      zypper in $PKGSTOINSTALL
    # Mandriva (with urpmi)
    elif which urpmi &> /dev/null; then
      urpmi $PKGSTOINSTALL
    # Fedora and CentOS (with yum)
    elif which yum &> /dev/null; then
      yum install $PKGSTOINSTALL
    # ArchLinux (with pacman)
    elif which pacman &> /dev/null; then
      pacman -Sy $PKGSTOINSTALL
    # Else, if no package manager has been founded
    else
      echo "ERROR: No package manager found. Please, install manually ${PKGSTOINSTALL}."
      exit 1
    fi
    # Check if installation is successful
    if [ $? -eq 0 ]; then
      echo "All dependencies installed successfully"
    # Else, if installation isn't successful
    else
      echo "ERROR: failed to install some missing dependencies. Please, install manually ${PKGSTOINSTALL}."
      exit 1
    fi
  # Else, if user don't want to install missing dependencies
  else
    echo "WARNING: Some dependencies may be missing. Please, install manually ${PKGSTOINSTALL}."
    exit 1
  fi
fi

#####################################################################
################## Cyclus and Friends Installation ##################
#####################################################################

#check for debug cli flag
DEBUG=""
if [ $# > 1 ]
then
    if [[ "$1" = "debug" ]]
    then
        DEBUG="-DCMAKE_BUILD_TYPE:STRING=Debug" foo = 1
    fi
fi

# get absolute path
WD=`echo $(pwd)/$line`

ROOT=$WD/cyclus-full
SRC=$ROOT/src
INSTALL=$ROOT/install
URL=http://github.com/cyclus

mkdir $ROOT
mkdir $SRC
mkdir $INSTALL

INSTALL_CMD="cmake ../src -DCMAKE_INSTALL_PREFIX=$INSTALL $DEBUG"

# get and build cyclopts
REPO=cyclopts
cd $SRC
git clone $URL/$REPO
cd $REPO
mkdir build && cd build
$INSTALL_CMD
make && make install

# get and build cyclus
REPO=cyclus
cd $SRC
git clone $URL/$REPO
cd $REPO
mkdir build && cd build
$INSTALL_CMD -DCYCLOPTS_ROOT_DIR=$INSTALL
make && make install

# get and build cycamore
REPO=cycamore
cd $SRC
git clone $URL/$REPO
cd $REPO
mkdir build && cd build
$INSTALL_CMD -DCYCLOPTS_ROOT_DIR=$INSTALL -DCYCLUS_ROOT_DIR=$INSTALL
make && make install

# test the installation
echo "Running cyclus tests:"
$INSTALL/cyclus/bin/CyclusUnitTestDriver | grep -A100 PASSED
echo "Running cycamore tests:"
$INSTALL/cycamore/bin/CycamoreUnitTestDriver | grep -A100 PASSED

