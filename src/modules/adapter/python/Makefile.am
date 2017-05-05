SUBDIRS=ipc

adapterdir = $(KLK_PYTHON_PREFIX_PATH)/klk/adapter
adapter_PYTHON = \
 __init__.py \
 defines.py \
 adapter.py

if DEBUGAM
klktestadapterdir = $(KLK_TEST_PYTHON_PREFIX_PATH)/testadapter
klktestadapter_PYTHON = \
 adaptertestdefines.py \
 testadapter.py
endif


EXTRA_DIST=defines.py.in 
