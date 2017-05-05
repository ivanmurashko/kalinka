klkhttpstreamerdir = $(KLK_PYTHON_PREFIX_PATH)/klk/httpstreamer
klkhttpstreamer_PYTHON = \
 __init__.py \
 defines.py \
 httpstreamer.py \
 cli.py

if DEBUGAM
klktesthttpstreamerdir = $(KLK_TEST_PYTHON_PREFIX_PATH)/testhttpstreamer
klktesthttpstreamer_PYTHON = \
 httpstreamertestdefines.py \
 testhttpstreamer.py
endif

EXTRA_DIST=defines.py.in httpstreamertestdefines.py.in
