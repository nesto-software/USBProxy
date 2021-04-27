#!/bin/bash

# USAGE: ./create-release.sh (major|minor|patch)

# HOWTO: merge all feature branches into dev first
#        run this script on dev branch
#        create a PR to merge dev into master (will create a new release)

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
set -e

echo "Please note: If you did not use this script before, please run ./install-release-tools.sh!"

BUMP="${1:-patch}"
VERSION=$(cat "${DIR}/../VERSION")
NEW_VERSION=$(semver bump "$BUMP" "$VERSION")

if [ -z "$DEBFULLNAME" ]; then
	export DEBFULLNAME=`git log -n 1 --pretty=format:%an`
fi

if [ -z "$DEBEMAIL" ]; then
	export DEBEMAIL=`git log -n 1 --pretty=format:%ae`
fi


(cd "${DIR}/../src" && dch -v ${NEW_VERSION} --distribution main --force-distribution)
echo -n "$NEW_VERSION" > "${DIR}/../VERSION"

echo "You are ready to commit the new version: git add -A && git commit -m \"chore: release v${NEW_VERSION}\""