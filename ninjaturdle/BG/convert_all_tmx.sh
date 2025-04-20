#!/bin/bash

source venv/bin/activate

# only read w<num>l<num>.tmx files
for file in w*l*.tmx; do
    # extract world and level from filename using regex
    if [[ $file =~ w([0-9]+)l([0-9]+)\.tmx ]]; then
        world="${BASH_REMATCH[1]}"
        level="${BASH_REMATCH[2]}"
        echo "Converting $file... w=$world l=$level"
        python convert_tmx.py "$world" "$level"
    else
        echo "Skipping $file (does not match pattern)"
    fi
done
