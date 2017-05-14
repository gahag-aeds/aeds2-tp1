#!/bin/bash

# Results format (columns):
# k1 user income
# k2 cashiers count
# k3 tray stacks count
# k4 tray stack max
# k5 tray reload load
# k6 tray reload rate
# k7 food service size
# ***
# k12 average time (hours)
# k13 average time (minutes)

sort -t' ' -k 12n -k 13n -k 2n -k 3n -k 5n -k 6nr "$1"
