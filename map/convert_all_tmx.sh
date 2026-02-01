#!/bin/bash

source venv/bin/activate

# only read level<num>.tmx files
for file in level*.tmx; do
    if [[ $file =~ level([0-9]+)\.tmx ]]; then
        level="${BASH_REMATCH[1]}"
        echo "Converting $file... level=$level"
        python3 convert_tmx.py "$level"
    else
        echo "Skipping $file (does not match pattern)"
    fi
done
