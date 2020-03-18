#!/bin/bash
PHILIP_BASE="$( cd "$( dirname "${BASH_SOURCE[1]}" )" >/dev/null 2>&1 && pwd )"
echo $PHILIP_BASE
generate_map -cfgp $PHILIP_BASE/FW/MMM/PHiLIP_config.json -odir $PHILIP_BASE/FW/MMM/ -ocsv $PHILIP_BASE/IF/philip_pal/philip_pal/mem_map/ -ocfg $PHILIP_BASE/FW/MMM/PHiLIP_config.json
cd $PHILIP_BASE/IF/philip_pal

python3 setup.py install --force --user
rm -rf build
rm -rf dist
rm -rf philip_pal.egg-info
rm -rf .egg
