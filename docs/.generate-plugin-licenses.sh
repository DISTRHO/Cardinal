#!/bin/bash

set -e

cd $(dirname $0)

for p in ../plugins/*/plugin.json; do
    name=$(jq -crM .name ${p})
    license=$(jq -crM .license ${p})
    echo "| ${name} | ${license} |"
done
