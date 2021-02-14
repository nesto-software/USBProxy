#!/bin/bash
# Usage: merge all feature branches into dev first
#        run this script on dev branch
#        create a PR to merge dev into master (will create a new release)

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
set -e

echo "Please note: If you did not use this script before, please run ./install-release-tools.sh!"

BUMP="${1:-path}"
VERSION=$(cat "${DIR}/../VERSION")
NEW_VERSION=$(semver bump $BUMP $VERSION)
(cd "${DIR}/../src" && dch -v ${NEW_VERSION} --distribution main "$(git log -1 --pretty=%B)" --force-distribution)
