from setuptools import setup, find_packages
from codecs import open
from os import path

here = path.abspath(path.dirname(__file__))

# Get the long description from the README file
with open(path.join(here, 'README.md'), encoding='utf-8') as f:
    long_description = f.read()

setup(
    name='solar-viability-tester',
    version='1.0.0',
    description='Solar viability tester utilizing the AT&T IoT Starter Kit and PubNub.',
    long_description=long_description,
    url='https://github.com/ckuzma/solar-viability-tester',
    license='Apache-2.0'
)
