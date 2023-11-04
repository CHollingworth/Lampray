# setup-tooling

Installs ASDF with plugins

## Requirements

- a `.tool-versions` file in the root of the repo

## Usage

```yml
  Release:

    runs-on: ubuntu-latest

    permissions:
      contents: write
      deployments: write

    steps:

      - uses: actions/checkout@v3

      - name: Setup tooling
        uses: ./.github/actions/setup-tooling
        env:
          # if you use an asdf plugin called "something" and its not in the shortname repo: https://github.com/asdf-vm/asdf-plugins
          # then it gets listed here as `ASDF_PLUGIN_URL_<pluginname>: the plugin repo`
          ASDF_PLUGIN_URL_something: https://get.thething/asdf-plugin
```

- installs asdf plugins for each item listed in your `.tools-version`
- if one of your plugins isn't on the official asdf list, then provide the
  install url by defining an ENVVAR of `ASDF_PLUGIN_URL_pluginname=url`

### Custom ASDF Setup command

Normally you'd have a `setup.sh` in the root of the repo. If it's somewhere else, 
then specify with your own version of the setup command:

```yml
  Release:

    runs-on: ubuntu-latest

    steps:

      - uses: actions/checkout@v3

      - name: Setup tooling
        uses: ./.github/actions/setup-tooling
        with:
          SetupCommand: ./your/path/to/your/asdf/setup.sh
```
