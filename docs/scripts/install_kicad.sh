#!/bin/bash
OPTS='-o "Dir::Etc::sourcelist=docs/sources.list" -o "debug::nolocking=true" -o "dir::cache=apt/cache" -o "dir::state=apt/state"'
apt $OPTS update
apt $OPTS install kicad
