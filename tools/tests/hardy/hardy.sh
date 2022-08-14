#! /usr/bin/env bash

alivef=/tmp/hardy.alive.pids
cantkillf=/tmp/hardy.cantkill.pids

function water::hardy::usage() {
  echo "
usage:
  hardy.sh <-l,-s,--live [n] | -d,--die>

summary:
  hardy is a utility that spawns and
  destroys difficult processes.

  internally, it reads from 
  '/dev/urandom', forever, making 
  endless 'shasum's of the endless, 
  random input.

  luckily, hardy can die.
  this has the effect of saving your
  computer from sounding like it's 
  entering the atmosphere at low-
  earth orbit.

examples:
  # start 5 difficult processes
  hardy.sh --live 5

  # stop being difficult
  # (kill all the spawned processes)
  hardy.sh --die

  # start and stop 100 instances
  hardy.sh -s 100;hardy.sh -d

"
}

water::hardy::die() {
  local ret=0
  for pid in $(cat $alivef); do
    isdead="$(kill "$pid")"
    if $isdead || echo "$isdead" | grep -q "No such process"; then
      sed -I '' "/^$pid$/d" $alivef
      echo "dead: $pid"
    else
      echo "$pid" >> $cantkillf
      sed -I '' "/^$pid$/d" $alivef
      echo "not dead: $pid"
      echo "please see: $alivef"
      ret=$((ret + 1))
    fi
  done
  return $ret
}

water::hardy::live() {
  local count=1
  [ -n "$1" ] && count=$1
  for i in $(seq 1 $count); do
    local hid="HARDY_PAYLOAD_$i"
    coproc "$hid" {
      shasum -a 512256 < /dev/urandom;
    }
    typeset -n pid="$hid""_PID"
    echo "alive: $pid ($i/$count)"
    echo "$pid" >> $alivef
  done
}

water::hardy() {
  case "$1" in
    -l|-s|--live)
      water::hardy::live "$2" ;;
    -d|--die)
      water::hardy::die ;;
    *)
      water::hardy::usage ;;
  esac
  echo "done."
}

water::hardy "$@"

