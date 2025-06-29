#!/bin/bash

# NOTE: flags
ip_addr=""
leader_ip_addr="192.168.1.180"
gateway="192.168.1.1"
access_point=""
access_point_password=""

print_usage() {
  echo "Sets up Ubuntu environment for mini cluster."
  echo ""
  echo "OPTIONS:"
  echo ""
  echo "  --ip_addr <addr>                   Static IP address.                       REQUIRED."
  echo "  --access_point <access_point>      Wifi name.                               REQUIRED."
  echo "  --access_point_password <password> Wifi password.                           REQUIRED."
  echo "  --leader_ip_addr <addr>            IP address of cluster leader.            OPTIONAL."
  echo "  --gateway <gateway>                Gateway (Default: 192.168.1.1)           OPTIONAL."
  echo "  -h --help                          Shows this text.                         OPTIONAL."
}

parse_flags() {
  while [[ $# -gt 0 ]]; do
    case $1 in
      -h|--help)
        print_usage
        return 0
        ;;
      --ip_addr)
        shift
        ip_addr="$1"
        shift
        ;;
      --gateway)
        shift
        gateway="$1"
        shift
        ;;
      --leader_ip_addr)
        shift
        leader_ip_addr="$1"
        shift
        ;;
      --access_point)
        shift
        access_point="$2"
        shift
        ;;
      --access_point_password)
        shift
        access_point_password="$1"
        shift
        ;;
      *)
        print_usage
        return 1
        ;;
    esac
  done
  return 0
}

validate_flags() {
  local status=0
  if [[ -z "$ip_addr" ]]; then
    echo "--ip_addr must not be empty."
    status=1
  fi
  if [[ -z "$leader_ip_addr" ]]; then
    echo "--leader_ip_addr must not be empty."
    status=1
  fi
  if [[ -z "$gateway" ]]; then
    echo "--gateway must not be empty."
    status=1
  fi
  if [[ -z "$access_point" ]]; then
    echo "--access_point must not be empty."
    status=1
  fi
  if [[ -z "$access_point_password" ]]; then
    echo "--access_point_password must not be empty."
    status=1
  fi
  return $status
}

setup_network() {
  # TODO: prefer ethernet
netplan_cfg="network:
  version: 2
  renderer: networkd
  wifis:
    wlan0:
      access-points:
        '$access_point':
          password: '$access_point_password'
      dhcp4: no
      addresses: [$ip_addr/24]
      routes:
        - to: default
          via: $gateway
      nameservers:
          addresses: [8.8.8.8,8.8.4.4]"
  if [[ "$ip_addr" == "$leader_ip_addr" ]]; then
    leader_ip_addr="SELF"
  fi

  echo "generating netplan configuration."
  echo "$netplan_cfg" | sudo tee /etc/netplan/01-network.yaml

  echo "applying netplan configuration."
  sudo netplan apply

  echo "saving cluster lead address as environment variable."
  echo "LEADER_IP_ADDR=$leader_ip_addr" | sudo tee -a /etc/environment

  return 0
}

install_dependencies() {
  mkdir ~/bin

  echo "installing apt packages."
  sudo apt install -y g++ gcc zsh tmux neovim

  echo "installing dotfiles."
  mkdir ~/.dotfiles
  git clone https://github.com/mccloskeybr/dotfiles ~/.dotfiles
  ~/.dotfiles/install --minimal
  chsh -s $(which zsh)
  curl -sL --proto-redir -all,https https://raw.githubusercontent.com/zplug/installer/master/installer.zsh | zsh

  echo "installing bazel."
  wget --retry-on-host-error -O ~/bin/bazel https://github.com/bazelbuild/bazelisk/releases/download/v1.26.0/bazelisk-linux-arm64
  sudo chmod +x ~/bin/bazel
  # https://github.com/bazelbuild/bazel/issues/25843
  echo "USE_BAZEL_VERSION=8.1.1" | sudo tee -a /etc/environment
  export USE_BAZEL_VERSION=8.1.1

  if [[ "$leader_ip_addr" == "SELF" ]]; then
    echo "installing bazel-remote."
    sudo adduser bazel-remote
    sudo mkdir /home/bazel-remote/bin
    sudo mkdir /home/bazel-remote/cache

    git clone https://github.com/buchgr/bazel-remote/ ~/bazel-remote
    cd ~/bazel-remote
    bazel build :bazel-remote
    sudo cp ./bazel-bin/bazel-remote_/bazel-remote /home/bazel-remote/bin/bazel-remote
    cd ~
    rm -r bazel-remote

bazel_remote_service_cfg="[Unit]
Description=bazel-remote cache

[Service]
User=bazel-remote
Group=bazel-remote

LimitNOFILE=40000
LimitNPROC=infinity
TasksMax=infinity
Restart=on-failure
Environment=GODEBUG=gctrace=1

Environment=GOMEMLIMIT=8GiB

ExecStart=/home/bazel-remote/bin/bazel-remote \
	--max_size 1000 \
	--dir /home/bazel-remote/cache \
	--profile_host 127.0.0.1 \
	--port 5000

[Install]
WantedBy=multi-user.target
"
    echo bazel_remote_service_cfg | sudo tee /etc/systemd/system/bazel-remote.service
    sudo systemctl daemon-reload
    sudo systemctl enable bazel-remote
  fi

  echo "updating and upgrading."
  sudo apt update -y && sudo apt upgrade -y

  return 0
}

set -e
parse_flags $@
validate_flags
setup_network
install_dependencies

echo "setup done. please reboot."
