#!/usr/bin/env python3
from pathlib import Path
import sys
b=Path(sys.argv[1]).read_bytes(); start=int(sys.argv[3],0); end=int(sys.argv[4],0)
if len(b)<end: raise SystemExit(f'image too short: {len(b)} < {end}')
Path(sys.argv[2]).write_bytes(b[start:end])
