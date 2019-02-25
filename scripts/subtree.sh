#!/usr/bin/env bash

set -e -x

git subtree pull --prefix=src/ext/thirdParty/rxi-log https://github.com/rxi/log.c master --squash
