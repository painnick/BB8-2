#!/bin/bash
set -e

docker start vc-02-kit_env
docker cp vc-02-kit_env:/unione_lite_app_hb_m/output/ .