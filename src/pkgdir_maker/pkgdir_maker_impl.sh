#!/bin/bash
# Copyright 2015 Samsung Electronics, license APACHE-2.0, see LICENSE file
VERSION="0.1.0"

### CONFIG
CONFIG_HomeDir="/home"
CONFIG_UserAppDir="${CONFIG_HomeDir}/_USERID_/apps_rw"
CONFIG_UserGroup="users"
CONFIG_SkelAppDir="/etc/skel/apps_rw"
CONFIG_SkelOwner="root"
CONFIG_SkelGroup="root"


### Directories to be created
DIRS=( DIR_root DIR_data DIR_cache )
# path type owner group permission SMACK_label SMACK_transmute SMACK_exec
DIR_root=( "_PKGID_" "d" "_USERNAME_" "_USERGROUP_" 750 "User::Home" 1 0 )
DIR_data=( "_PKGID_/data" "d" "_USERNAME_" "_USERGROUP_" 750 "User::Pkg::_PKGID_" 1 0 )
DIR_cache=( "_PKGID_/cache" "d" "_USERNAME_" "_USERGROUP_" 750 "User::Pkg::_PKGID_" 1 0 )


### Utils
## Print log
function LOG
{
  LABEL=$1
  MSG="[${LABEL}] $2"
  # Skip printing debug log without OPTION_verbose
  if [[ ${LABEL} == "DEBUG" && ! -n ${OPTION_verbose} ]]; then
    return 0
  fi
  # Print log
  echo -e "${MSG}"
  # Exit if error log is given
  if [[ "${LABEL}" == "ERROR" ]]; then
    exit 1
  fi
  return 0
}

## Get valid user list
# Returns: return_getUserList (the user list array)
function getUserList
{
  return_getUserList=( )  # return variable(array)
  local homeDirList=`ls ${CONFIG_HomeDir}`
  for userName in ${homeDirList}; do
    # if $HOME/apps_rw exists, it is assumed that the user exists.
    local appDir=${CONFIG_UserAppDir/_USERID_/$userName}
    if [[ -d "$appDir" ]]; then
      LOG DEBUG "Check if $appDir/ exists...  Found. user $userName is added."
      # Append new item into array
      return_getUserList[${#return_getUserList[*]}]="$userName"
    else
      LOG DEBUG "Check if $appDir/ exists...  Not found."
    fi
  done
}


## Get real dirinfo
# Returns: return_getDirInfo (Directory info array)
function getDirInfo
{
  local dirname=$1
  local pkgid=$2
  local username=$3
  local usergroup=$4
  return_getDirInfo=( )

  # NOTE: indirect reference + array copy
  eval dirinfo_template=\( \${${dirname}[@]} \)

  for value in "${dirinfo_template[@]}"; do
    value=${value//_PKGID_/$pkgid}
    value=${value//_USERNAME_/$username}
    value=${value//_USERGROUP_/$usergroup}
    return_getDirInfo[${#return_getDirInfo[*]}]=$value
  done

  return 0
}


## Create directory
function createDir
{
  local path=$1
  local pathtype=$2
  local owner=$3
  local group=$4
  local permission=$5

  mkdir -p $path || LOG ERROR "Directory creation failure: $path"
  chown $owner:$group $path || LOG ERROR "chown failure: $path to $owner:$group"
  chmod $permission $path || LOG ERROR "chmod failure: $path to $permission"

  LOG DEBUG "Created directory: $path ($owner:$group, $permission)"

  return 0
}


## Set SMACK
function setSmackToPath
{
  local path=$1
  local smack_label=$2
  local smack_transmute=$3
  local smack_exec=$4

  chsmack -a $smack_label $path || LOG ERROR "SMACK labeling failure"
  if [[ "${smack_transmute}" == "1" ]]; then
    chsmack -t $path || LOG ERROR "SMACK transmute tagging failure"
  fi
  if [[ "${smack_exec}" == "1" ]]; then
    chsmack -e $path || LOG ERROR "SMACK exec tagging failure"
  fi
  LOG DEBUG "Set SMACK to ${path}: label=${smack_label}, transmute=${smack_transmute}, exec=${smack_exec}"
  return 0
}


## Create pkg dirs
function createPkgDir
{
  local pkgid=$1
  local username=$2
  local usergroup=$3
  local parentdir=$4

  LOG INFO "Create package directory into ${parentdir}"
  for dirname in "${DIRS[@]}"; do
    getDirInfo $dirname $pkgid $username $usergroup
    local dirinfo=( "${return_getDirInfo[@]}" )

    # Get each values
    local path="${parentdir}/${dirinfo[0]}"
    local pathtype=${dirinfo[1]}
    local owner=${dirinfo[2]}
    local group=${dirinfo[3]}
    local permission=${dirinfo[4]}
    local smack_label=${dirinfo[5]}
    local smack_transmute=${dirinfo[6]}
    local smack_exec=${dirinfo[7]}

    createDir $path $pathtype $owner $group $permission
    setSmackToPath $path $smack_label $smack_transmute $smack_exec

  done
}


## Create pkg directories into given user's home directory
function createPkgDirForUser
{
  local pkgid=$1
  local username=$2
  local group=$3

  local parentdir=${CONFIG_UserAppDir/_USERID_/$username}

  LOG DEBUG "createPkgDirForUser: pkgid=${pkgid}, parentdir=${parentdir}"
  createPkgDir $pkgid $username $group $parentdir

  return 0
}


## Create pkg directory into the skeleton
function createPkgDirToSkeleton
{
  local pkgid=$1

  local owner=${CONFIG_SkelOwner}
  local group=${CONFIG_SkelGroup}
  local parentdir=${CONFIG_SkelAppDir}

  createPkgDir $pkgid $owner $group $parentdir

  return 0
}

### create dirs
function createDirs
{
  local pkgid=$1

  # Get user list
  getUserList
  # NOTE: This return value is an array, so array copy is used.
  local userList=( "${return_getUserList[@]}" )

  echo "userList: ${userList[@]}"

  for user in ${userList[@]}; do
    createPkgDirForUser $pkgid $user $CONFIG_UserGroup
  done

  createPkgDirToSkeleton $pkgid

  return 0
}

function deletePkgDirForUser
{
  local pkgid=$1
  local username=$2

  local parentdir=${CONFIG_UserAppDir/_USERID_/$username}
  local appdir="${parentdir}/${pkgid}"

  rm -rf $appdir || LOG ERROR "Removing directory failure: $appdir"
  LOG INFO "${appdir} is removed"
}

function deletePkgDirFromSkeleton
{
  local pkgid=$1
  local parentdir=${CONFIG_SkelAppDir}
  local appdir="${parentdir}/${pkgid}"

  rm -rf $appdir || LOG ERROR "Removing directory failure: $appdir"
  LOG INFO "${appdir} is removed"
}


### delete dirs
function deleteDirs
{
  local pkgid=$1

  getUserList
  local userList=( "${return_getUserList[@]}" )

  for user in "${userList[@]}"; do
    deletePkgDirForUser $pkgid $user
  done
  deletePkgDirFromSkeleton

  return 0
}


### Option processing
_valid_option_list=( "--create" "--delete" "--pkgid=" "--verbose" "--help" )
for option in $@
do
    found_option=0
    for _valid_option in ${_valid_option_list[@]}
    do
        if [ "$_valid_option" == "${option:0:${#_valid_option}}" ]; then
            if [ ${_valid_option:${#_valid_option}-1} == "=" ]; then
                eval "export OPTION_${option#--}"
            else
                eval "export OPTION_${option#--}=1"
            fi
            found_option=1
            break
        fi
    done
    if [[ $found_option == 0 ]]; then
        LOG ERROR "Invalid option: ${option}\nUsage: `basename $0` --help"
    fi
done
if test -n "${OPTION_help}"; then
  echo -e "`basename $0` ${VERSION}"
  echo -e "    Creates or deletes package data directories of given global apps"
  echo -e "    into all users' hone directories."
  echo -e "Usage:"
  echo -e "    `basename $0` --create --pkgid=<pkgid>    : Create package dirs"
  echo -e "    `basename $0` --delete --pkgid=<pkgid>    : Delete package dirs"
  exit 0
fi
if [[ -n "${OPTION_create}" && -n "${OPTION_delete}" ]]; then
  LOG ERROR "--create and --delete cannot be used together"
fi
if [[ ! -n "${OPTION_pkgid}" ]]; then
  LOG ERROR "pkgid is not given"
fi


### main
if [[ -n "${OPTION_create}" ]]; then
  createDirs ${OPTION_pkgid}
elif [[ -n "${OPTION_delete}" ]]; then
  deleteDirs ${OPTION_pkgid}
fi


echo "Done."
exit 0
