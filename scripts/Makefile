
PY=python3

splash.h: make_splash.py splash1.png splash2.png
	${PY} make_splash.py splash1.png splash1 >$@
	${PY} make_splash.py splash2.png splash2 >>$@

clean:
	rm -f splash.h

