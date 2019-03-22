FROM debian:buster
RUN apt-get update && apt-get install -y make git build-essential cmake python3 python3-pip libmsgsl-dev --no-install-recommends && apt-get clean
RUN pip3 install flask flask-socketio
COPY . /opt/ocelots
RUN /opt/ocelots/backend/build.sh /opt/ocelots/oo-backend
WORKDIR /opt/ocelots/frontend-flask
ENTRYPOINT python3 server.py

