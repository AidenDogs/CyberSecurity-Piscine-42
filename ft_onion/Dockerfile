FROM debian:bookworm-slim

RUN apt-get update && \
    apt-get install -y --no-install-recommends \
        nginx \
        openssh-server \
        tor \
        curl && \
    rm -rf /var/lib/apt/lists/*

RUN useradd -m -s /bin/bash student && \
    usermod -p '*' student && \
    mkdir -p /run/sshd /home/student/.ssh && \
    chmod 700 /home/student/.ssh && \
    chown -R student:student /home/student/.ssh

COPY index.html /usr/share/nginx/html/index.html
COPY nginx.conf /etc/nginx/nginx.conf
COPY sshd_config /etc/ssh/sshd_config
COPY torrc /etc/tor/torrc
COPY entrypoint.sh /entrypoint.sh

RUN chmod +x /entrypoint.sh && \
    chmod 600 /etc/ssh/sshd_config && \
    chown debian-tor:debian-tor /etc/tor/torrc && \
    chmod 600 /etc/tor/torrc && \
    mkdir -p /var/lib/tor/hidden_service && \
    chown -R debian-tor:debian-tor /var/lib/tor/hidden_service && \
    chmod 700 /var/lib/tor/hidden_service

CMD ["/entrypoint.sh"]