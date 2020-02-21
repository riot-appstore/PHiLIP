#!/bin/bash
PHILIP_BASE="$( cd "$( dirname "${BASH_SOURCE[1]}" )" >/dev/null 2>&1 && pwd )"
echo $PHILIP_BASE
generate_map -cfgp $PHILIP_BASE/FW/MMM/PHiLIP_config.json -odir $PHILIP_BASE/FW/MMM/ -ocsv $PHILIP_BASE/IF/philip_pal/philip_pal/mem_map/ -ocfg $PHILIP_BASE/FW/MMM/PHiLIP_config.json
cd $PHILIP_BASE/IF/philip_pal
rm -rf dist

python3 setup.py sdist

pip3 install --upgrade --force-reinstall dist/philip_pal-*
