#!/bin/bash
SCRIPTPATH="$( cd "$(dirname "$0")" ; pwd -P )"
BUILDROOT="release"
if [ -n "$1" ] 
then
	VERSION="${1}_"
	echo "will package as version: ${VERSION}"
else
	echo "will package without a version string"
	VERSION=""
fi
FILENAME="bedrock-viz_${VERSION}"

pushd "${SCRIPTPATH}/.."

mkdir -vp "${BUILDROOT}"

tar --exclude='./cmake-*' \
    --exclude=".git" \
    --exclude="./release" \
    --exclude=".DS_Store" \
    --exclude="./.idea" \
    --exclude="./.vscode" \
    --exclude="build" -zcvf ${BUILDROOT}/${FILENAME}macOS.tar.gz .

echo "macOS tarball created, will now convert to linux tarball"

pushd $BUILDROOT
if [ -e bedrock-viz ]
then
	echo "Can't repack macOS tarball into Linux tarball... due to existing $BUILDROOT/bedrock-viz";
	exit 1;
fi

tar -xzf ${FILENAME}macOS.tar.gz --transform="s,\./,bedrock-viz/,g" && \
	tar -cvzf ${FILENAME}linux.tar.gz bedrock-viz && \
	rm -rf bedrock-viz/

popd #out of buildroot

popd #out of scriptroot

echo ""
shasum "${BUILDROOT}"/${FILENAME}*.tar.gz -a 256

