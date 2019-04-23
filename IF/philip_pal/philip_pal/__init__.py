# Copyright (c) 2019 Kevin Weiss, for HAW Hamburg  <kevin.weiss@haw-hamburg.de>
#
# This file is subject to the terms and conditions of the MIT License. See the
# file LICENSE in the top level directory for more details.
# SPDX-License-Identifier:    MIT
"""packet init for PHiLIP PAL
This exposes useful modules in the PHiLIP PAL packet
"""
from .philip_if import PhilipExtIf as Phil

__all__ = ['Phil']
