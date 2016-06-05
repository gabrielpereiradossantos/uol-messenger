MOD=glib
VER=2.12.7
THIS=$MOD-$VER
DEPS=''
sed -e 's/need_relink=yes/need_relink=no # no way --tml/' <ltmain.sh >ltmain.temp && mv ltmain.temp ltmain.sh
usedev
usemsvs6
CC='gcc -mtune=pentium3 -mthreads' CPPFLAGS='-I/opt/gnu/include' LDFLAGS='-L/opt/gnu/lib -Wl,--enable-auto-image-base' CFLAGS=-O2 ./configure --with-libiconv --disable-gtk-doc --prefix=c:/devel/target/$THIS
libtoolcacheize
make glibconfig.h.win32 &&
make glibconfig.h &&
mv glibconfig.h glibconfig.h.autogened &&
cp glibconfig.h.win32 glibconfig.h &&
PATH=/devel/target/$THIS/bin:.libs:$PATH make install &&
(cd /devel/target/$THIS/bin; strip --strip-unneeded *.dll *.exe) &&
./glib-zip &&
(cd /devel/src/tml && zip /tmp/$MOD-dev-$VER.zip make/$THIS.make)
manifestify /tmp/$MOD*-$VER.zip
