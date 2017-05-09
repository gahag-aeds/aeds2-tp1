#!/bin/bash

while read -r line || [[ -n "$line" ]]; do
  echo -n "$line : "
  echo $line | $1 | grep Average | tr -d 'h' | sed 's/m$//'
done < "$2"
