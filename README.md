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

# Translating
You will need to generate the .pot file
```
meson build
ninja -C build redshiftgtk-pot
```
After that, you can use Poedit to create your localization or run
```
cd po
msginit -l <language-code>
```
and edit the file manually.  
Don't forget to add your language to LINGUAS!

# TODO
- Make it possible to set dusk and dawn manually

# Screenshots
![Landing view](data/screenshots/main.png)
