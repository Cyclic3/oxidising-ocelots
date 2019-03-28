FROM debian:buster
RUN apt-get update && apt-get install -y make git build-essential cmake python3 python3-pip libmsgsl-dev --no-install-recommends && apt-get clean
RUN pip3 install flask flask-socketio gevent
