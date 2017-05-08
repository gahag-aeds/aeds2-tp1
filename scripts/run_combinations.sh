#!/bin/bash

while read -r line || [[ -n "$line" ]]; do
  echo -n "$line : "
  echo $line | $1 | grep Average
done < "$2"
