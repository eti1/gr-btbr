#!/usr/bin/env python2
# -*- coding: utf-8 -*-
##################################################
# GNU Radio Python Flow Graph
# Title: Top Block
# Generated: Fri May 18 20:13:06 2018
##################################################

if __name__ == '__main__':
	import ctypes
	import sys

from PyQt4 import Qt
from gnuradio import analog
from gnuradio import blocks
from gnuradio import digital
from gnuradio import eng_notation
from gnuradio import filter
from gnuradio import gr
from gnuradio.eng_option import eng_option
from gnuradio.filter import firdes
from gnuradio.filter import pfb
from optparse import OptionParser
import math
import osmosdr
import limesdr
import sys
import btbr

class top_block(gr.top_block, Qt.QWidget):
	def make_osmocom_source(self, args='', gain=None):
		src = osmosdr.source(args=args)
		src.set_sample_rate(self.samp_rate)
		src.set_freq_corr(0, 0)
		src.set_dc_offset_mode(0, 0)
		src.set_iq_balance_mode(0, 0)
		src.set_gain_mode(False, 0)
		src.set_if_gain(20, 0)
		src.set_bb_gain(20, 0)
		src.set_antenna("", 0)
		src.set_bandwidth(0, 0)
		src.set_center_freq(self.freq, 0)
		if gain is None:
			g = src.gain_range()
			gain = float(g[0]+g[1])/2
		src.set_gain(gain)
		return src

	def set_decoder(self, chan, num):
		print("Add decoder for channel %d"%num)
		decoder = btbr.btdecode(num)
		# Both methods are quite equivalent
		if 1:
			quad_demod = analog.quadrature_demod_cf(self.samp_per_sym/(2*math.pi))
			clock_recov = digital.clock_recovery_mm_ff(
					omega = self.samp_per_sym,
					gain_omega = 0.25*self.gain_mu**2,
					mu = 0.32, 
					gain_mu = self.gain_mu,
					omega_relative_limit = 0.005)
			binary_slicer = digital.binary_slicer_fb()

			self.connect(chan, quad_demod)
			self.connect(quad_demod, clock_recov)
			self.connect(clock_recov, binary_slicer)
			self.connect(binary_slicer, decoder)
			self.msg_connect((decoder, 'out'), (self.bthandler, 'in'))
		else:
			gfsk_demod = digital.gfsk_demod(
				samples_per_symbol=self.samp_per_sym,
				sensitivity=2*math.pi / self.samp_per_sym,
				gain_mu=self.gain_mu,
				mu=0.32,
				omega_relative_limit=0.005,
				freq_error=0.0,
				verbose=False,
				log=False,
			)
			self.connect(chan, gfsk_demod)
			self.connect(gfsk_demod, decoder)
			self.msg_connect((decoder, 'out'), (self.bthandler, 'in'))

	def __init__(self, options):
		gr.top_block.__init__(self)


		##################################################
		# Variables
		##################################################
		self.num_chans = num_chans = 1+options.chan_max-options.chan_min

		assert(self.num_chans > 0 and self.num_chans%2 == 0)
		center_chan = (options.chan_max+options.chan_min)/2
		self.samp_rate = 1e6 * (num_chans)
		self.freq = freq = 1e6 * (2401 + center_chan)
		self.samp_per_sym = samp_per_sym = 2
		self.gain_mu = gain_mu = 0.175

		self.cutoff_freq = cutoff_freq = 500e3
		self.transition_width = transition_width = 200e3

		print("Create %d channels around %d"%(self.num_chans, center_chan))

		##################################################
		# Blocks
		##################################################
		self.source = self.make_osmocom_source(gain=14)

		# Filtering
		self.taps = firdes.low_pass(1.0, 2e6, self.cutoff_freq, self.transition_width, firdes.WIN_BLACKMAN, 6.76)

		self.pfb_channelizer = pfb.channelizer_ccf(
			num_chans,
			(self.taps),
			samp_per_sym,
			100)

		self.bthandler = btbr.btsink()
		self.pfb_channelizer.set_channel_map(range(num_chans/2+1,num_chans) + range(0,num_chans/2+1))

		# Connections
		self.connect(self.source, self.pfb_channelizer)
		for i in range(num_chans):
			self.set_decoder((self.pfb_channelizer, i), options.chan_min+i)



def main(top_block_cls=top_block, options=None):
	usage="%prog: [options]"
	parser = OptionParser(option_class=eng_option, usage=usage)
	parser.add_option("-C", "--channels", type="string", default="1-4",
					help="set channels, -C=num or -C=min-max\" [default=%default]")
	parser.add_option("-g", "--gain", type="eng_float", default=0.7,
					help="set gain in dB (default is midpoint)")

	(options, args) = parser.parse_args ()
	if len(args) != 0:
		parser.print_help()
		raise SystemExit, 1

	chans = options.channels.split("-")
	if len(chans) == 2:	
		chans=int(chans[0]),int(chans[1])
	else:
		c, = chans
		c = int(c)
		chans=c,c
	options.chan_min, options.chan_max = chans

	try:
		top_block(options).run()
	except KeyboardInterrupt:
		pass

if __name__ == '__main__':
	main()
