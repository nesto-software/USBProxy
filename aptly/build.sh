#!/bin/bash
# ARG: $1 (KEEP_BUILD_CONTAINER_RUNNING): set to 1 to build and jump into the container afterwards

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
IMAGE_NAME=aptly:latest
CONTAINER_NAME=aptly

# load env vars to reference gpg key id
source ./.env
gpg --export --armor ${GPG_KEY_ID} > public.asc
gpg --export-secret-key --armor ${GPG_KEY_ID} > private.asc

echo "${GPG_KEY_PASSWORD}" > key_passwd

docker build -t ${IMAGE_NAME} -f Dockerfile ..
if [ $? -ne 0 ]; then
  echo -e "\nWe MUST abort because docker build already failed and no new image was created!\n"
  exit 1;
fi

if [ ! -z "$1" ] && [[ ( "$1" == 1 ) ]]; then
    echo -e "\nCommands to run in another shell:\n---------------------------------\nConnect: docker exec -it $CONTAINER_NAME bash\nKill: docker kill $CONTAINER_NAME\n"
    # -v "${DIR}/../docker-crosstool-ng-arm/bin/:/root/usb-proxy"
    docker run --rm -v "${DIR}/key_passwd:/root/usb-proxy/key_passwd" --env-file ./.env --name "${CONTAINER_NAME}" "${IMAGE_NAME}" tail -f /dev/null; exit
fi

# in order to publish to S3, please run "./build.sh 1" and then paste:
# aptly publish repo -batch=true -passphrase-file="/root/usb-proxy/key_passwd" -gpg-key="92F91ABA4816493E" -component=local -distribution=unofficial nesto-pos-adapter-devel s3:nesto-debian-repo-devel:
