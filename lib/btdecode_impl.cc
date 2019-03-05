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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gnuradio/io_signature.h>
#include <stdio.h>
#include <btbb.h>
#include <btbr/btbr.h>
#include "btdecode_impl.h"

namespace gr {
  namespace btbr {

    btdecode::sptr
    btdecode::make(unsigned chan_num)
    {
      return gnuradio::get_initial_sptr
        (new btdecode_impl(chan_num));
    }

    btdecode_impl::btdecode_impl(unsigned chan_num)
	: gr::sync_block("btdecode",
              gr::io_signature::make(1, 1, 1),
              gr::io_signature::make(0, 0, 0))
		, d_channel(chan_num)
		, d_samp_num(0)
    {
	//	set_history(BTBR_SYM_COUNT+72);
		message_port_register_out(pmt::mp("out"));
	}

	uint64_t btdecode_impl::get_time(void)
	{
		struct timeval tv;

		gettimeofday(&tv, NULL);

		return (uint64_t)tv.tv_sec*1000000 + (uint64_t)tv.tv_usec;
		
	}

    int
    btdecode_impl::work (int noutput_items,
			gr_vector_const_void_star &input_items,
			gr_vector_void_star &output_items)
    {
      const char *in = (const char *) input_items[0];
	  int ninput = noutput_items;
      int offset;
      unsigned len;
      btbb_packet *pkt = NULL;
	  struct timeval tv;
	  uint64_t ts;
      unsigned consume = 0;
      pmt::pmt_t blob, dict;

      if (ninput < 72)
      {
          /* Not enough samples, consume nothing */
          consume = 0;
          goto ret;
      }

      if ((offset = btbb_find_ac((char*)in, ninput - 72, LAP_ANY, 1, &pkt)) < 0)
      {
        /* No preamble found, consume up to next possible preamble */
        consume = ninput - (72 - 1);
        goto ret;
      }

      /* Found preamble */
      if (offset + BTBR_SYM_COUNT > ninput)
      {
        /* Not enough samples for payload, consume up to preamble */
        consume = offset;
        goto ret;
      }
      /* We have a packet  */

	  /* FIXME: this is not a proper timestamp */
	  gettimeofday(&tv, NULL);
	  ts = (uint64_t)tv.tv_sec * 1000000 + (uint64_t)tv.tv_usec;

      /* FIXME: we don't know the actual length */
      len = BTBR_SYM_COUNT;
	  //ts = d_samp_num + offset;
	  blob = pmt::make_blob(in+offset, len);
	  dict = pmt::make_dict();
	  dict = pmt::dict_add(dict, pmt::mp("channel"), pmt::from_uint64(d_channel));
	  dict = pmt::dict_add(dict, pmt::mp("timestamp"), pmt::from_uint64(ts));
	  dict = pmt::dict_add(dict, pmt::mp("lap"), pmt::from_uint64(btbb_packet_get_lap(pkt)));
	  dict = pmt::dict_add(dict, pmt::mp("proto"), pmt::from_uint64(0));
	  message_port_pub(pmt::mp("out"), pmt::cons(dict, blob));

      /* consume up to message end */
      consume = offset + len;
  ret:
      if (consume)
      {
        d_samp_num += ninput;
        consume_each(consume);
      }
      return 0;
    }
  } /* namespace btbr */
} /* namespace gr */

