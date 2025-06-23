#!/bin/bash
# NOTE: OS type is Ubuntu Server 20.04.5 LTS (64-BIT), available on Raspberry Pi Imager.

# NOTE: flags
# NOTE: IP address range is 192.168.1.180 - 183.
ip_addr=""
leader_ip_addr="192.168.1.180"
gateway="192.168.1.1"
access_point=""
access_point_password=""
dry_run=0

print_usage() {
  echo "Sets up Ubuntu environment for mini cluster."
  echo ""
  echo "OPTIONS:"
  echo ""
  echo "  --ip_addr <addr>                   Static IP address.                                     REQUIRED."
  echo "  --access_point <access_point>      Wifi name.                                             REQUIRED."
  echo "  --access_point_password <password> Wifi password.                                         REQUIRED."
  echo "  --leader_ip_addr <addr>            IP address of cluster leader. (Default: 192.168.1.180) OPTIONAL."
  echo "  --gateway <gateway>                Gateway (Default: 192.168.1.1)                         OPTIONAL."
  echo "  -d --dry_run                       If defined, does not apply file changes.               OPTIONAL."
  echo "  -h --help                          Shows this text.                                       OPTIONAL."
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
        access_point="$1"
        shift
        ;;
      --access_point_password)
        shift
        access_point_password="$1"
        shift
        ;;
      -d|--dry_run)
        dry_run=1
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

setup_home_dir() {
  cd ~
  mkdir bin
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

  if [[ dry_run -eq 1 ]]; then
    echo "dry run: netplan configuration:"
    echo "$netplan_cfg"
    echo ""
    echo "dry run: leader ip address: $leader_ip_addr"
  else
    echo "generating netplan configuration."
    rm /etc/netplan/*
    echo "$netplan_cfg" > /etc/netplan/01-network.yaml

    echo "applying netplan configuration."
    netplan apply

    echo "saving cluster lead address as environment variable."
    echo "LEADER_IP_ADDR=$leader_ip_addr" >> /etc/environment
  fi

  return 0
}

install_dependencies() {
  if [[ dry_run -eq 1 ]]; then
    echo "dry run: skipping dependency installation."
    return 0
  fi

  echo "updating and upgrading."
  apt update -y && apt upgrade -y

  # bazel
  echo "installing bazel."
  cd ~/bin
  wget https://github.com/bazelbuild/bazelisk/releases/download/v1.26.0/bazelisk-linux-arm64
  chmod +x bazelisk-linux-arm64
  ln -s ~/bin/bazelisk-linux-arm64 ~/bin/bazel
  cd ~
  # https://github.com/bazelbuild/bazel/issues/25843
  echo "USE_BAZEL_VERSION=8.1.1" >> /etc/environment

  return 0
}

set -e
if [[ $(/usr/bin/id -u) -ne 0 ]]; then
  echo "Not running as root."
  exit
fi
cd ~

parse_flags $@
validate_flags
setup_home_dir
setup_network
install_dependencies

if [[ dry_run -eq 0 ]]; then
  echo "setup done. please reboot."
fi
