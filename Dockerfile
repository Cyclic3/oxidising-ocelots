FROM cyclic3/oxidising-ocelots-base
COPY . /opt/ocelots
RUN /opt/ocelots/backend/build.sh /opt/ocelots/oo-backend
WORKDIR /opt/ocelots/frontend-flask
ENTRYPOINT python3 server.py /opt/ocelots/frontend-flask

