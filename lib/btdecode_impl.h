/* -*- c++ -*- */
/* 
 * Copyright 2018 <+YOU OR YOUR COMPANY+>.
 * 
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 * 
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this software; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifndef INCLUDED_BTBR_BTDECODE_IMPL_H
#define INCLUDED_BTBR_BTDECODE_IMPL_H

#include <btbr/btdecode.h>

namespace gr {
  namespace btbr {

    class btdecode_impl : public btdecode
    {
     private:
	   unsigned d_channel;
	   unsigned long d_samp_num;

     public:
      btdecode_impl(unsigned chan_num);
	  uint64_t get_time(void);

	  int work (int noutput_items,
			gr_vector_const_void_star &input_items,
			gr_vector_void_star &output_items);
    };

  } // namespace btbr
} // namespace gr

#endif /* INCLUDED_BTBR_BTDECODE_IMPL_H */

