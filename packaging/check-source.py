#!/usr/bin/env python3
"""Reject incomplete project source before packaging, including ignored inputs."""
import argparse,json,re,subprocess
from pathlib import Path

def check(root):
    root=Path(root).resolve()
    required={'CMakeLists.txt','core_sources.cmake','c/src/core.c',
              'c/apps/cli/main.c','c/apps/gtk/main.c','c/apps/gtk/property_sheet.c'}
    listing=root/'core_sources.cmake'
    if listing.is_file():
        required.update(re.findall(r'(?m)^\s*(c/[^\s)]+\.[ch])\s*$',listing.read_text()))
    manifest=root/'SOURCE-PROVENANCE.json'
    if manifest.is_file():required.update(json.loads(manifest.read_text())['files'])
    errors=[f'missing source: {p}' for p in sorted(required) if not (root/p).is_file()]
    # A working-tree build can succeed with ignored inputs that disappear from
    # git archive/GitHub downloads. Check the index, not just disk existence.
    if (root/'.git').exists():
        tracked=set(subprocess.check_output(['git','-C',str(root),'ls-files','-z']).decode().split('\0'))
        errors += [f'not tracked for source publication: {p}' for p in sorted(required-tracked)]
    if errors:raise RuntimeError('\n'.join(errors))
    return len(required)
if __name__=='__main__':
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--root',type=Path,default=Path(__file__).resolve().parents[1]);args=parser.parse_args()
    try:count=check(args.root)
    except (RuntimeError,OSError,KeyError,ValueError) as error:parser.exit(1,f'{error}\n')
    print(f'PASS source publication completeness: {count} required files')
