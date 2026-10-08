#!/bin/bash
set -e

mkdir -p /run/sshd
mkdir -p /etc/ssh/hostkeys
[ -f /etc/ssh/hostkeys/ssh_host_ed25519_key ] || \
    ssh-keygen -q -t ed25519 -N '' -f /etc/ssh/hostkeys/ssh_host_ed25519_key

if [ -f /keys/authorized_keys ]; then
    mkdir -p /home/student/.ssh
    tr -d '\r' < /keys/authorized_keys > /home/student/.ssh/authorized_keys
    chown -R student:student /home/student/.ssh
    chmod 700 /home/student/.ssh
    chmod 600 /home/student/.ssh/authorized_keys
else
    echo "ERROR: /keys/authorized_keys not found" >&2
    exit 1
fi

nginx
/usr/sbin/sshd -e

exec su -s /bin/bash debian-tor -c "tor -f /etc/tor/torrc"