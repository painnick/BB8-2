#!/bin/bash
set -e

docker start vc-02-kit_env
docker cp unione_lite_app_hb_m/ vc-02-kit_env:/