#!/bin/bash

set -euo pipefail

VERSION_TXT="VERSION.txt"

cd "$(dirname "$0")"

function get_meta() {
  local branch=$(git name-rev HEAD | sed 's/.* \([^~]*\).*/\1/')

  if [ "$branch" == "master" ]; then
    exit
  fi

  echo -n "+"

  if [ "${TRAVIS_PULL_REQUEST:-false}" != false ]; then
    echo "pr_$TRAVIS_PULL_REQUEST"
  else
    echo "${branch//[^[:alnum:]-]/-}"
  fi
}

function main() {
  local base_version=$(cat $VERSION_TXT)
  local meta_version=$(get_meta)

  echo "${base_version}${meta_version}"
}

main


