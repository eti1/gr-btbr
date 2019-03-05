/* -*- c++ -*- */

#define BTBR_API

%include "gnuradio.i"			// the common stuff

//load generated python docstrings
%include "btbr_swig_doc.i"

%{
#include "btbr/btdecode.h"
%}


%include "btbr/btdecode.h"
GR_SWIG_BLOCK_MAGIC2(btbr, btdecode);
