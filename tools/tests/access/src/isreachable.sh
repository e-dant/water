#!/usr/bin/env bash

function testaccess(){
  if [ $# -ne 1 ]; then echo "${FUNCNAME[0]} <resource/path>"; return 1; fi
  for uri in $(cat "test.uris"); do
    curl -sL "$uri/$1"
    if [ $? -ne 0 ]; then
      echo "[ unreachable ] $uri/$i"
    else
      echo "[  reachable  ] $uri/$i"
    fi
  done
}

testaccess $@
