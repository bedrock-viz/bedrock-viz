#!/bin/bash
SCRIPTPATH="$( cd "$(dirname "$0")" ; pwd -P )"
BUILDROOT="release"
# TODO tar all files and submodules, then calculate sha256

pushd "${SCRIPTPATH}/.."

mkdir -p "${BUILDROOT}"

tar --exclude='./cmake-*' \
    --exclude=".git" \
    --exclude="./release" \
    --exclude="./scripts" \
    --exclude=".DS_Store" \
    --exclude="./.idea" \
    --exclude="./.vscode" \
    --exclude="build" -zcvf ${BUILDROOT}/release.tar.gz .

echo ""
shasum "${BUILDROOT}"/release.tar.gz -a 256

popd