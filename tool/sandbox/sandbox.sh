#! /usr/bin/env bash

sandbox::build() {
  podman build -t sandbox:latest .
}

sandbox::run() {
  podman run -it --privileged sandbox:latest
}

sandbox::clean() {
  podman kill -a
  podman rmi $(podman image list --format "{{.ID}}") --force
}

sandbox() {
  # go local
  local lastdir="$PWD"
  cd "$(dirname $0)"

  # define
  local _this="${FUNCNAME[0]}"
  local cmdb="$_this"

  # parse
  for i in "$@"; do
    echo "$i" | grep -q -- "--" \
      && cmdb+="::${i##--}" \
      || cmdb+=" $i "
  done

  # log
  echo "[$_this] => ${cmdb[*]}"

  # run
  ${cmdb[*]} 

  # go back
  cd $lastdir
}

sandbox "$@"

