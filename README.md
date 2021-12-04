# Tapeutape

This is a fork of the legacy Tapeutape with some bug fixes and additional features.

## Build

**Dependencies** (as debian packages)
```
libjack-jackd2-dev liblo-dev libfltk1.3-dev libasound2-dev \
libsndfile1-dev libsamplerate0-dev libxml2-dev
```

**Build**
```
make clean && make -j8
```

**Install**

```bash
sudo make install
```

Append `PREFIX=/usr` to override the default installation path (`/usr/local`)

**Uninstall**

```bash
sudo make uninstall
```

Append `PREFIX=/usr` to override the default uninstallation path (`/usr/local`)

## Documentation

TODO

## Authors

Florent Berthaut (until 2011)

Aurélien Roux and Jean-Emmanuel Doucet @ [AMMD](https://ammd.net) (since 2019)
