# RedshiftGtk
Redshift Configuration GUI  
Currently in development  

Buy me a beer/coffee?  
[![Donate](https://img.shields.io/badge/Donate-PayPal-blue.svg)](https://paypal.me/StefanRic)

# Building
```
meson --prefix=/usr build
ninja -C build
sudo ninja -C build install
```

## Dependencies
```
gio-2.0
gtk+-3.0
libm
```

# TODO
- Make it possible to set dusk and dawn manually

# Screenshots
![Landing view](data/screenshots/main.png)
