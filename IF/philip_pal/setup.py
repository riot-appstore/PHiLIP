#!/usr/bin/env python3
# Copyright (c) 2018 Kevin Weiss, for HAW Hamburg  <kevin.weiss@haw-hamburg.de>
#
# This file is subject to the terms and conditions of the MIT License. See the
# file LICENSE in the top level directory for more details.
# SPDX-License-Identifier:    MIT
"""
    Setup file for riot_pal.
"""
from setuptools import setup, find_packages

with open("README.md", "r") as fh:
    LONG_DESCRIPTION = fh.read()


setup(
    name="philip_pal",
    version="1.1.0",
    author="Kevin Weiss, Sebastian Meiling",
    author_email="weiss.kevin604@gmail.com, s@mlng.net",
    license="MIT",
    description="Protocol abstraction and parser for PHiLIP",
    long_description=LONG_DESCRIPTION,
    long_description_content_type="text/markdown",
    url="https://github.com/riot-appstore/PHiLIP",
    packages=find_packages(),
    platforms='any',
    python_requires='>=3.4.*',
    include_package_data=True,
    classifiers=[
        "Programming Language :: Python :: 3",
        "License :: OSI Approved :: MIT License",
        "Operating System :: OS Independent",
        "Development Status :: 3 - Alpha",
        "Intended Audience :: Developers"
    ],
    setup_requires=["pytest-runner"],
    tests_require=["pytest", "pytest-regtest", "pprint"],
    install_requires=['pyserial'],
    entry_points={
        'console_scripts': ['philip_shell=philip_pal.philip_shell:main'],
    }
)
