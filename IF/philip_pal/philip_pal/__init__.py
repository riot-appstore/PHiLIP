# Copyright (c) 2019 Kevin Weiss, for HAW Hamburg  <kevin.weiss@haw-hamburg.de>
#
# This file is subject to the terms and conditions of the MIT License. See the
# file LICENSE in the top level directory for more details.
# SPDX-License-Identifier:    MIT
"""packet init for PHiLIP PAL
This exposes useful modules in the PHiLIP PAL packet
"""
from mm_pal import RESULT_ERROR, RESULT_SUCCESS, RESULT_TIMEOUT
from .philip_if import PhilipIf as Phil

__all__ = ['Phil']
__author__ = "Kevin Weiss"
__email__ = "kevin.weiss@gmail.com"
__version__ = "2.0.0"
