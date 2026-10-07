FROM debian:bookworm-slim

RUN apt-get update && \
    apt-get install -y --no-install-recommends \
        nginx \
        openssh-server \
        tor \
        curl && \
    rm -rf /var/lib/apt/lists/*

RUN useradd -m -s /bin/bash student && \
    mkdir -p /run/sshd /home/student/.ssh && \
    chmod 700 /home/student/.ssh && \
    chown -R student:student /home/student/.ssh

COPY index.html /usr/share/nginx/html/index.html
COPY nginx.conf /etc/nginx/nginx.conf
COPY sshd_config /etc/ssh/sshd_config
COPY torrc /etc/tor/torrc

RUN chmod 600 /etc/ssh/sshd_config && \
    chmod 600 /etc/tor/torrc && \
    mkdir -p /var/lib/tor/hidden_service && \
    chown -R debian-tor:debian-tor /var/lib/tor/hidden_service && \
    chmod 700 /var/lib/tor/hidden_service

EXPOSE 80
EXPOSE 4242

CMD ["bash", "-c", "mkdir -p /run/sshd && nginx && /usr/sbin/sshd && exec tor -f /etc/tor/torrc"]