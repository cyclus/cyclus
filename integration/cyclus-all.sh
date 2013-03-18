#!/bin/bash

########################################
########### Helper Functions ###########
########################################

# \brief clones the repo if it doesn't yet exist or pulls it if it does
# \param[in] src  the full source directory
# \param[in] url  the global github url
# \param[in] repo  the repository
# \param[in] branch  the branch
function get_repo() 
{
    HERE=$PWD
    
    local src=$1
    local url=$2
    local repo=$3
    local branch=$4
    
    cd $src
    if [ ! -d $repo ]; then
	git clone $url/$repo
    else
	cd $repo
	git pull origin $branch
    fi

    cd $HERE
}

# \brief removes the build folder, runs install command, makes, and installs 
# \param[in] src  the full source directory
# \param[in] repo  the repository
# \param[in] cmd  the cmake installation command
function install()
{
    HERE=$PWD

    local src=$1
    local repo=$2
    local cmd=$3

    cd $src/$repo
    if [ -d build ]; then
	rm -rf build
    fi
    mkdir build && cd build && $cmd && make install

    cd $HERE
}


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
if [ $# -gt 0 ]
then
    if [ "$1" = "debug" ]
    then
        DEBUG="-DCMAKE_BUILD_TYPE:STRING=Debug"
    fi
fi

# get absolute path
WD=`echo $(pwd)/$line`

ROOT=$WD/cyclus-full
SRC=$ROOT/src
INSTALL=$ROOT/install
URL=http://github.com/cyclus

echo $ROOT

if [ ! -d $ROOT ]; then
    mkdir $ROOT
fi
if [ ! -d $SRC ]; then
    mkdir $SRC
fi
if [ ! -d $INSTALL ]; then
    mkdir $INSTALL
fi


INSTALL_CMD="cmake ../src -DCMAKE_INSTALL_PREFIX=$INSTALL $DEBUG"

# get and build cyclopts
REPO=cyclopts
BRANCH=master
echo "Fetching the latest version of Cyclopts"
get_repo $SRC $URL $REPO $BRANCH 
echo "Installing Cyclopts"
install $SRC $REPO "$INSTALL_CMD"

# get and build cyclus
REPO=cyclus
BRANCH=develop
echo "Fetching the latest version of Cyclus"
get_repo $SRC $URL $REPO $BRANCH 
echo "Installing Cyclus"
install $SRC $REPO "$INSTALL_CMD"

# get and build cycamore
REPO=cycamore
BRANCH=develop
echo "Fetching the latest version of Cycamore"
get_repo $SRC $URL $REPO $BRANCH 
echo "Installing Cycamore"
install $SRC $REPO "$INSTALL_CMD"

# test the installation
echo "Running cyclus tests:"
$INSTALL/cyclus/bin/CyclusUnitTestDriver | grep -A100 PASSED
echo "Running cycamore tests:"
$INSTALL/cycamore/bin/CycamoreUnitTestDriver | grep -A100 PASSED