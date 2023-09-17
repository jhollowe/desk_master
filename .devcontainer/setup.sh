#!/bin/sh

# install the git hook for pre-commit
pre-commit install

# run pre-commit on a simple file to ensure it downloads all needed tools
pre-commit run --files .pre-commit-config.yaml

# let ESPHome download needed dependencies and pre-compile all sources
esphome compile sample_config.yaml
