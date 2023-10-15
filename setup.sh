#!/usr/bin/env bash

#
# Setup ASDF and Plugins
#  ASDF_PLUGINS=(nodejs pnpm bats) \
#  ASDF_PLUGIN_URL_nodejs=https://someurl \
#  ./pkg/setup-tooling/setup.bash
#

set -e

[ $(command -v git) ] || {
  echo "📛📦 Missing git"
  exit 1
}
[ $(command -v curl) ] || {
  echo "📛📦 Missing curl"
  exit 1
}

# Local vars
ASDF_VERSION=${ASDF_VERSION:-v0.8.1}
ASDF_HOME=$HOME/.asdf
ASDF_BIN=$ASDF_HOME/asdf.sh

#
# Helper to idempotently add strings to target files
#
append_uniquely() {
  if ! grep -q "$2" "$1"; then
    echo "====> ✍ Writing \"$2\" into \"$1\" "
    echo "${2}" >>$1
  fi
}


case "${SHELL}" in
/bin/bash)
  SHELL_PROFILE=~/.bashrc
  SHELL_NAME="bash"
  ;;
/bin/zsh)
  SHELL_PROFILE=~/.zshrc
  SHELL_NAME="zsh"
  ;;
esac

echo "=> 💁 [ASDF] install with plugins"

if [ ! -f "$ASDF_BIN" ]; then
  echo "===> ⤵️ ASDF not detected ... installing"
  git clone https://github.com/asdf-vm/asdf.git "$ASDF_HOME" --branch $ASDF_VERSION
  [ ! command -v asdf ] &>/dev/null && {
    echo "====> ⚕️ adding to shell profile"
    append_uniquely "$SHELL_PROFILE" ". $ASDF_HOME/asdf.sh"
    append_uniquely "$SHELL_PROFILE" ". $ASDF_HOME/completions/asdf.bash"
  }
fi

source "$ASDF_BIN"

for plugin in $(cut -d' ' -f1 ./.tool-versions)
do
  echo "
==> 💁 [ASDF] Ensure ${plugin} plugin"

  if [ -d "$ASDF_HOME/plugins/${plugin}" ]; then
    echo "===> 📦 attempting upgrade"
    asdf plugin-update "${plugin}"
  else
    echo "===> 🌐 installing"
    plugin_url_var=ASDF_PLUGIN_URL_${plugin//-/_}
    plugin_url="${!plugin_url_var}"

    if [ ${!plugin_url_var+x} ]; then
      echo "====> 💁 [${plugin}] installed from ${plugin_url}"
    fi

    asdf plugin-add "${plugin}" "${plugin_url}"
  fi
done


echo "==> 💁 [ASDF] install tools"
asdf install

echo "==> 💁 [ASDF] reshim globals"
asdf reshim

echo "==> 💁 [ASDF] Done ✅"
