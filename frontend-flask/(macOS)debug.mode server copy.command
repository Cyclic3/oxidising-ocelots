#!/bin/bash
cd "$(dirname "$0")"/
export FLASK_APP=server
export FLASK_ENV=production
bash -c 'while [ 0 ]; do flask run --host=0.0.0.0 --port 5000 --with-threads;done'