klktranscodedir = $(KLK_PYTHON_PREFIX_PATH)/klk/transcode
klktranscode_PYTHON = \
 __init__.py \
 defines.py \
 transcode.py \
 sourcetype.py \
 videoquality.py \
 videosize.py \
 schedule.py

if DEBUGAM
klktesttranscodedir = $(KLK_TEST_PYTHON_PREFIX_PATH)/testtranscode
klktesttranscode_PYTHON = \
 testtranscode.py \
 transcodetestdefines.py
endif

EXTRA_DIST=defines.py.in sourcetype.py.in transcodetestdefines.py.in \
 videoquality.py.in videosize.py.in schedule.py.in