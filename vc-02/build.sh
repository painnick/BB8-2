#!/bin/bash
set -e

docker start vc-02-kit_env
docker cp unione_lite_app_hb_m/ vc-02-kit_env:/
docker exec -it vc-02-kit_env /bin/bash /unione_lite_app_hb_m/build.sh update
docker cp vc-02-kit_env:/unione_lite_app_hb_m/output/ .