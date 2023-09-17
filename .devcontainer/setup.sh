#!/bin/sh

# install the git hook for pre-commit
pre-commit install

# run pre-commit on a simple file to ensure it downloads all needed tools
pre-commit run --files .pre-commit-config.yaml
