#!/bin/bash

### show version of this program source
git describe --tags --always --dirty=-modified 2>/dev/null
[[ $? -ne 0 ]] &&echo dev-nogit
