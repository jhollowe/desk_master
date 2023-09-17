ARG PYTHON_VERSION=3.11
FROM mcr.microsoft.com/devcontainers/python:1-${PYTHON_VERSION}

COPY dev_requirements.txt /tmp/pip-tmp/dev_requirements.txt
RUN pip3 --disable-pip-version-check --no-cache-dir install -r /tmp/pip-tmp/dev_requirements.txt \
   && rm -rf /tmp/pip-tmp
