#!/usr/bin/env python3
from pathlib import Path
import hashlib
b=Path('identity48_applet.bin').read_bytes()
assert len(b)==414, len(b)
h=hashlib.sha256(b).hexdigest(); assert h=='12467c87c7b5ab48f4c7b38047276ad64eb0de45cad13b4ec903b5beb2738f48',h
print(f'OK size={len(b)} sha256={h}')
