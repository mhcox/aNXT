# This Makefile is free software; the Free Software Foundation
# gives unlimited permission to copy, distribute and modify it.

-include Makefile.config

all: lib/libanxt.a lib/libanxttools.a lib/libanxtfile.a lib/libanxtnet.a bin/nxtd
	make -C examples/
	make -C tools/

##### Make libanxt #####

lib/libanxt.a: nxt.o us.o nxtcam.o
	$(AR) rs $@ $^
	$(CC) -shared -Wl,-soname,libanxt.so.1 -o lib/libanxt.so.1 $^ -lc

nxt.o: nxt.c
	$(CC) $(CFLAGS) -c -o $@ $< -DNXTD_PIDFILE=\"$(NXTD_PIDFILE)\" -DPATH_BIN=\"$(PATH_BIN)\"

us.o: us.c
	$(CC) $(CFLAGS) -c -o $@ $<

nxtcam.o: nxtcam.c
	$(CC) $(CFLAGS) -c -o $@ $<

##### Make libanxttools #####

lib/libanxttools.a: nxttools.c
	$(CC) $(CFLAGS) -c -o nxttools.o $<
	$(AR) rs $@ nxttools.o
	$(CC) -shared -Wl,-soname,libanxttools.so.1 -o lib/libanxttools.so.1 nxttools.o -lc

##### Make libanxtfile #####

lib/libanxtfile.a: cal.o ric.o rmd.o rso.o nvconfig.o
	$(AR) rs $@ $?
	$(CC) -shared -Wl,-soname,libanxtfile.so.1 -o lib/libanxtfile.so.1 $^ -lc

cal.o: cal.c
	$(CC) $(CFLAGS) -c -o $@ $<

ric.o: ric.c
	$(CC) $(CFLAGS) -c -o $@ $<

rmd.o: rmd.c
	$(CC) $(CFLAGS) -c -o $@ $<

rso.o: rso.c
	$(CC) $(CFLAGS) -c -o $@ $<

nvconfig.o: nvconfig.c
	$(CC) $(CFLAGS) -c -o $@ $<

##### Make nxtd #####

bin/nxtd: nxtd.c nxtd_usb_$(USB_MOD).c nxtd_bt_$(BT_MOD).c lib/libanxtnet.a
	$(CC) $(CFLAGS) -o $@ nxtd.c nxtd_usb_$(USB_MOD).c nxtd_bt_$(BT_MOD).c -Llib/ -lusb -lbluetooth -lanxtnet -DNXTD_PIDFILE=\"$(NXTD_PIDFILE)\"

##### Make libanxtnet #####

lib/libanxtnet.a: nxtnet.c
	$(CC) $(CFLAGS) -c -o nxtnet.o $<
	$(AR) rs $@ nxtnet.o
	$(CC) -shared -Wl,-soname,libanxtnet.so.1 -o lib/libanxtnet.so.1 nxtnet.o -lc

##### Install target #####

install:
	mkdir -p $(PATH_LIB)
	cp -R lib/* $(PATH_LIB)
	ln -sf $(PATH_LIB)/libanxt.so.1 $(PATH_LIB)/libanxt.so
	ln -sf $(PATH_LIB)/libanxttools.so.1 $(PATH_LIB)/libanxttools.so
	ln -sf $(PATH_LIB)/libanxtfile.so.1 $(PATH_LIB)/libanxtfile.so
	mkdir -p $(PATH_INCLUDE)
	cp -R include/* $(PATH_INCLUDE)
	mkdir -p $(PATH_BIN)
	cp -R bin/* $(PATH_BIN)
	mkdir -p $(PATH_MAN)
	cp -R doc/man/* $(PATH_MAN)
	sh tools/anxt-config-creator.sh $(PREFIX) > $(PATH_BIN)/anxt-config
	chmod 755 $(PATH_BIN)/anxt-config

##### Build distributable archive

tar.gz:
	(set -x;ANXT=aNXT-`bin/anxt-config --version` && rm -rf /tmp/$$ANXT && mkdir /tmp/$$ANXT && tar -cvf - * | (cd /tmp/$$ANXT && tar -xf -) && (cd /tmp/$$ANXT && make realclean && cd .. && tar -cf $$ANXT.tar $$ANXT && gzip -9f $$ANXT.tar))

##### Clean target #####

clean:
	rm -Rf *.o lib/* bin/nxtd
	make -C examples/ clean
	make -C tools/ clean

realclean: clean
	(find . -name .svn -print | xargs rm -rf)

# (header) dependencies generated by "gcc -MM -Iinclude *.c"
cal.o: cal.c include/nxtfile/cal.h
nvconfig.o: nvconfig.c include/nxtfile/nvconfig.h
nxt.o: nxt.c include/nxtnet.h include/nxt.h
nxtcam.o: nxtcam.c include/nxt.h include/nxtnet.h \
  include/nxt_i2c/nxtcam.h
nxtd.o: nxtd.c include/nxtnet.h nxtd.h
nxtd_bt_bluez.o: nxtd_bt_bluez.c nxtd.h
nxtd_bt_dummy.o: nxtd_bt_dummy.c nxtd.h
nxtd_usb_dummy.o: nxtd_usb_dummy.c nxtd.h
nxtd_usb_libusb.o: nxtd_usb_libusb.c nxtd.h
nxtnet.o: nxtnet.c include/nxtnet.h
nxttools.o: nxttools.c include/nxttools.h include/nxt.h include/nxtnet.h
ric.o: ric.c include/nxtfile/ric.h
rmd.o: rmd.c
rso.o: rso.c include/nxtfile/rso.h
us.o: us.c include/nxt.h include/nxtnet.h include/nxt_i2c/us.h

